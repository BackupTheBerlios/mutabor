/** \file
 ********************************************************************
 * MIDI-File als Device.
 *
 * \author Rüdiger Krauße <krausze@mail.berlios.de>
 *         Tobias Schlemmer <keinstein@users.berlios.de>
 * \date $Date: 2011/11/02 14:31:57 $
 * \version $Revision: 1.13 $
 * \license GPL
 *
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 ********************************************************************
 * \addtogroup route
 * \{
 ********************************************************************/
// ------------------------------------------------------------------
// Mutabor 3, 1998, R.Krauße
// MIDI-File als Device
// ------------------------------------------------------------------
#include "src/kernel/Defs.h"
#if defined(MUTWIN) && !defined(WX)
#include <owl/pch.h>
#endif

#ifdef RTMIDI
#  include "RtMidi.h"
#else
#ifndef H_MMSYSTEM
//  #define WINVER 0x030a
#include <mmsystem.h>
#define H_MMSYSTEM
#endif
#endif

#include "src/kernel/routing/midi/DevMidF.h"
#include "src/kernel/routing/midi/DevMidi.h"
#include "src/kernel/Execute.h"
#include "src/kernel/GrafKern.h"
#include "src/kernel/Runtime.h"
#include "src/kernel/routing/Route-inlines.h"

#include "wx/wfstream.h"
#include "wx/msgdlg.h"
#include <inttypes.h>



namespace mutabor {

/* berechnet die Tonigkeit einer Taste bzgl. tonsystem */
#define GET_INDEX(taste,tonsystem)					\
	((int)((taste)-( (tonsystem)->anker % (tonsystem)->breite ))	\
	 % (tonsystem)->breite )


// berechnet die 'Oktavlage' einer taste bzgl. tonsystem
#define GET_ABSTAND(taste,tonsystem)					\
	( (int)((taste)-( (tonsystem)->anker % (tonsystem)->breite ))	\
	  / (tonsystem)->breite -((int) (tonsystem)->anker		\
				  / (tonsystem)->breite ))

// berechnet die Frequenz in Midi-Form
#define GET_FREQ(taste,tonsystem)					\
	( ( (tonsystem)->ton[GET_INDEX(taste,(tonsystem))]==0) ?	\
	  (long) 0 :							\
	  (long)( (tonsystem)->periode *				\
		  GET_ABSTAND(taste,(tonsystem))  +			\
		  (tonsystem)->ton[GET_INDEX(taste,(tonsystem))]))

#define ZWZ 1.059463094 /* 12.Wurzel 2 */
#define LONG_TO_HERTZ( x ) (440.0*pow(ZWZ,((((double)x)/(double)16777216.0))-69))

// Midi-Ausgabe
#define MIDI_OUT3(code1, code2, code3)		\
	Tracks.MidiOut3(code1, code2, code3)

#define MIDI_OUT2(code1, code2)			\
	Tracks.MidiOut2(code1, code2)

	static inline uint8_t bytenr (unsigned long number, int index)
	{
		return (number >> index * 8) & 0xFF;
	}

// Pitch
#define MIDI_PITCH(i)							\
	int pb = ( (((int)bytenr(freq,2))<<6) + (bytenr(freq,1)>>2) ) / bending_range; \
	MIDI_OUT3(0xE0+i, bytenr(pb,0) >> 1 , 64+(bytenr(pb,1)))

// Sound
#define MIDI_SOUND(i, sound)				\
	if ( sound != -1 ) MIDI_OUT2(0xC0+i, sound)

// ID errechnen
#define MAKE_ID(route, box, taste, channel)				\
	((((DWORD)channel) << 24) + (((DWORD)route->GetId()) << 16) + ((DWORD)box << 8) + taste)

#define NO_DELTA (std::numeric_limits<mutint64>::max()) //2147483647  // long max-Zahl

	int lMidiCode[8] = { 3, 3, 3, 3, 2, 2, 3, 1 };

// Daten ¸bergeben für NoRealTime-Übersetzungen
	BYTE *pData;
	int nData;
	long NRT_Speed;

	DWORD ReadLength(mutIFstream &is)
	{
		BYTE a[4];
		mutReadStream(is,(char*)a, 4);
		return (((DWORD)a[0]) << 24) + (((DWORD)a[1]) << 16) +
			(((DWORD)a[2]) << 8) + ((DWORD)a[3]);
	}

	void WriteLength(mutOFstream &os, DWORD l)
	{
		mutPutC(os,(BYTE) ((l >> 24) & 0xFF));
		mutPutC(os,(BYTE) ((l >> 16) & 0xFF));
		mutPutC(os,(BYTE) ((l >> 8) & 0xFF));
		mutPutC(os,(BYTE) (l & 0xFF));
	}



// Tracks -----------------------------------------------------------

	void Track::WriteDelta()
	{
		BYTE w[5];
		int i = 0;
		DWORD Delta = (long)CurrentTime - Time.GetValue();
		Time = (long)CurrentTime;
		w[0] = Delta & 0x7F;
		Delta >>= 7;

		while ( Delta ) {
			i++;
			w[i] = 0x80 | (Delta & 0x7F);
			Delta >>= 7;
		}

		for (;i>=0;i--)
			Data->Add(w[i]);
	}

	void Track::Save(mutOFstream &os)
	{
		mutWriteStream(os,"MTrk",4);
		WriteLength(os, Data->GetItemsInContainer()+4);

		for (DWORD i = 0; i < Data->GetItemsInContainer(); i++)
			mutPutC(os,(BYTE)(*Data)[i]);

		mutPutC(os,(BYTE)0x00);

		mutPutC(os,(BYTE)0xFF);

		mutPutC(os,(BYTE)0x2F);

		mutPutC(os,(BYTE)0x00);
	}

// OutputMidiFile ------------------------------------------------------

/// Save current device settings in a tree storage
/** \argument config (tree_storage) storage class, where the data will be saved.
 */
	void OutputMidiFile::Save (tree_storage & config) 
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.Write(_T("Bending Range"),bending_range);
		config.Write(_T("File Name"),Name);
		mutASSERT(oldpath == config.GetPath());
	}

/// Save route settings (filter settings) for a given route
/** Some route settings (e.g. filter settings) are device type 
 * specific. This function saves them in a tree storage.
 * \argument config (tree_storage *) Storage class, where the data will be saved.
 * \argument route (Route ) Route whos data shall be saved.
 */
	void OutputMidiFile::Save (tree_storage & config, const RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Midi File Output"));
		mutASSERT(route);
		config.Write(_T("Avoid Drum Channel"), route->ONoDrum);
		config.Write(_T("Channel Range From"), route->OFrom);
		config.Write(_T("Channel Range To"), route->OTo);
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}


        /// Load current device settings from a tree storage
        /** \argument config (tree_storage) storage class, where the data will be loaded from.
	 */
	void OutputMidiFile::Load (tree_storage & config)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		bending_range=config.Read(_T("Bending Range"), bending_range);
		Name = config.Read(_T("File Name"),mutEmptyString);
		mutASSERT(oldpath == config.GetPath());
	}

        /// Loade route settings (filter settings) for a given route
	/** Some route settings (e.g. filter settings) are device type 
	 * specific. This function loads them from a tree storage.
	 * \argument config (tree_storage *) Storage class, where the data will be restored from.
	 * \argument route (Route ) Route whos data shall be loaded.
	 */
	void OutputMidiFile::Load (tree_storage & config, RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Midi File Output"));
		mutASSERT(route);
		route->ONoDrum = config.Read (_T("Avoid Drum Channel"), true);
		int oldfrom, oldto;
		oldfrom = route->OFrom = config.Read(_T("Channel Range From"), GetMinChannel());
		oldto = route->OTo = config.Read(_T("Channel Range To"), GetMaxChannel());
		bool correct = true;
		if (route->OFrom < GetMinChannel()) {
			correct = false;
			route->OFrom = GetMinChannel();
		}
		if (route->OTo > GetMaxChannel()) {
			correct = false;
			route->OTo = GetMaxChannel();
		}
		if (!correct)
			wxMessageBox(wxString::Format(_("The Channel range %d--%d of the MIDI file %s must be inside %d--%d. The current route had to be corrected."),
						      oldfrom,oldto,GetName().c_str(),GetMinChannel(),GetMaxChannel()),
				     _("Warning loading route"),wxICON_EXCLAMATION);
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}


	bool OutputMidiFile::Open()
	{
		mutASSERT(!isOpen);
		Tracks.Data->Flush();
		isOpen = true;

		for (int i = 0; i < 16; i++) {
			Cd[i].Reset();
			ton_auf_kanal[i].active = false;
			ton_auf_kanal[i].inkey = -1;
			ton_auf_kanal[i].outkey = -1;
			ton_auf_kanal[i].channel = 0;
			ton_auf_kanal[i].midi_channel = 0;
			ton_auf_kanal[i].unique_id = 0;
			KeyDir[i] = (char)i; // alle nicht benutzt
		}

		nKeyOn = 0;

		return isOpen;
	}

	void OutputMidiFile::Close()
	{
		mutASSERT(isOpen);
		isOpen = false;
		// alle liegenden Tˆne ausschalten

		for (int i = 0; i < 16; i++)
			if ( KeyDir[i] >= 16 )  {// benutzt
				mutASSERT(ton_auf_kanal[i].outkey != -1);
				MIDI_OUT3(0x80+i, ton_auf_kanal[i].outkey, 64);
			}

		// Datei speichern
		mutOpenOFstream(os,Name);

		BYTE Header[41] =
			{ 'M', 'T', 'h', 'd', 0, 0, 0, 6, 0, 1, 0, 2, 1, 0x00,
			  'M', 'T', 'r', 'k', 0, 0, 0, 0x13, 0x00, 0xFF, 0x51, 0x03, 0x07, 0xD0, 0x00,
			  0x00, 0xFF, 0x58, 0x04, 0x04, 0x02, 0x18, 0x08, 0x00, 0xFF, 0x2F, 0x00 };

		if ( !RealTime ) {
			Header[12] = ((WORD)(NRT_Speed >> 8)) & 0xFF;
			Header[13] = ((WORD)(NRT_Speed)) & 0xFF;
		}

		mutWriteStream(os,Header, 41);

		Tracks.Save(os);
	}

	void OutputMidiFile::NoteOn(int box, 
				    int taste, 
				    int velo, 
				    RouteClass * r, 
				    size_t id, 
				    ChannelData *cd)
	{
		int i = 0, s;
		DWORD p;
		unsigned long freq;
		if (!r || r == NULL) return;

		if ( box == -2 ) {
			freq = ((long)taste) << 24;
			box = 255;
		} else
			freq = GET_FREQ (taste, mut_box[box].tonesystem);

		// testen, ob nicht belegte Taste
		if ( !freq )
			return;

		int help = r->OTo;

		i = r->OFrom; // "irgendein" Startwert im g¸ltigen Bereich

		if ( i == DRUMCHANNEL && r->ONoDrum && i < r->OTo )
			i++;

		int j;

		mutASSERT(r->OFrom >= 0);
		mutASSERT(r->OTo < 16);
		for (j = r->OFrom; j <= r->OTo; j++ )
			if ( j != DRUMCHANNEL || !r->ONoDrum )
				if ( KeyDir[j] < KeyDir[i] )
					i = j;

		if ( KeyDir[i] >= 16 )  // keinen freien gefunden
		{
			// "mittelste Taste weglassen"
			int AM = 0; // arithmetisches Mittel der Tasten

			for (REUSE(int) j =r->OFrom; j <= r->OTo; j++)
				if ( j != DRUMCHANNEL || !r->ONoDrum )
					AM += ton_auf_kanal[j].inkey;

			AM /= help + 1 - r->OFrom;

			for (REUSE(int) j = r->OFrom; j <= r->OTo; j++ )
				if ( j != DRUMCHANNEL || !r->ONoDrum )
					if ( abs(AM - ton_auf_kanal[j].inkey) < abs(AM - ton_auf_kanal[i].inkey) )
						i = j;

			// Ton auf Kanal i ausschalten
			mutASSERT(ton_auf_kanal[i].outkey != -1);
			MIDI_OUT3(0x80+i, ton_auf_kanal[i].outkey, 64);

			// KeyDir umsortieren
			BYTE oldKeyDir = KeyDir[i];

			for (int k = 0; k < 16; k++)
				if ( KeyDir[k] > oldKeyDir ) KeyDir[k]--;

			KeyDir[i] = 15+nKeyOn;
		} else {
			nKeyOn++;
			KeyDir[i] = 15 + nKeyOn;
		}

		// freier Kanal = i

		// Sound testen
		if ( (s = cd->Sound) != Cd[i].Sound ) {
			MIDI_SOUND(i, s);
			Cd[i].Sound = s;
		}

		// Bank testen
		if ( (s = cd->BankSelectMSB) != Cd[i].BankSelectMSB && s != -1 ) {
			MIDI_OUT3(0xB0+i, 0, (BYTE) s);
			Cd[i].BankSelectMSB = s;
		}

		if ( (s = cd->BankSelectLSB) != Cd[i].BankSelectLSB && s != -1 ) {
			MIDI_OUT3(0xB0+i, 32, (BYTE) s);
			Cd[i].BankSelectLSB = s;
		}

		// Pitch testen
		if ( (long) (p = freq & 0xFFFFFF) != Cd[i].Pitch ) {
			MIDI_PITCH(i);
			Cd[i].Pitch = p;
		}


		ton_auf_kanal[i].active = true;
		ton_auf_kanal[i].inkey = taste;
		ton_auf_kanal[i].outkey = bytenr(freq,3) & 0x7f;
		ton_auf_kanal[i].channel = r->GetId();
		ton_auf_kanal[i].midi_channel = i;
		ton_auf_kanal[i].unique_id = id;
		MIDI_OUT3(0x90+i, ton_auf_kanal[i].outkey, velo);
	}

	void OutputMidiFile::NoteOff(int box, 
				     int taste, 
				     int velo, 
				     RouteClass * r, 
				     size_t id)
	{
		if (!r || r == NULL) return;
		if ( box == -2 )
			box = 255;

	
		if ( !velo ) //3 ?? notwendig?
			velo = 64;

		for (int i = r->OFrom; i <= r->OTo; i++)
			if ( i != DRUMCHANNEL || !r->ONoDrum )
				if ( ton_auf_kanal[i].unique_id == id ) {
					mutASSERT(ton_auf_kanal[i].outkey != -1);
					MIDI_OUT3(0x80+i, ton_auf_kanal[i].outkey, velo);
					ton_auf_kanal[i].active = false;
					ton_auf_kanal[i].inkey = 0;
					ton_auf_kanal[i].outkey =-1;
					ton_auf_kanal[i].channel = 0;
					ton_auf_kanal[i].midi_channel = -1;
					ton_auf_kanal[i].unique_id=0;
					// KeyDir umsortieren
					int oldKeyDir = KeyDir[i];

					for (int k = 0; k < 16; k++)
						if ( KeyDir[k] > oldKeyDir || KeyDir[k] < 16) KeyDir[k]--;

					nKeyOn--;

					KeyDir[i] = 15;
				}
	}

	inline long LongAbs(long x)
	{
		return (x < 0)? -x : x;
	}

	void OutputMidiFile::NotesCorrect(RouteClass * route)
	{
		for (int i = 0; i < 16; i++)
			if ( KeyDir[i] >= 16 && ton_auf_kanal[i].active ) {
				int RouteID = (ton_auf_kanal[i].channel);
					       
				if (route->GetId() != RouteID) 
					continue;

				long freq = GET_FREQ(ton_auf_kanal[i].inkey, mut_box[route->GetBox()].tonesystem);


				mutASSERT(ton_auf_kanal[i].outkey != -1);
				// hier kann ein evtl. grˆﬂerer bending_range genutzt werden, um
				// Ton aus und einschalten zu vermeiden
				if ( ton_auf_kanal[i].outkey == (bytenr(freq,3) & 0x7f) &&
				     Cd[i].Pitch == (freq & 0xFFFFFF) )
					continue;

				long Delta = freq - ((long)ton_auf_kanal[i].outkey << 24);

				char SwitchTone = (LongAbs(Delta) >= ((long)bending_range << 24));

				// evtl. Ton ausschalten
				if ( SwitchTone ) {
					MIDI_OUT3(0x80+i, ton_auf_kanal[i].outkey, 0x7F);
					ton_auf_kanal[i].outkey = bytenr(freq,3) & 0x7f;
					Delta = freq - ((DWORD)ton_auf_kanal[i].outkey << 24);
				} else if ( Delta == Cd[i].Pitch )
					continue;

				// Spezialbending (groﬂer Range)
				Cd[i].Pitch = Delta;

				Delta /= (4*bending_range);

				Delta += 0x400000;

				MIDI_OUT3(0xE0+i, ((BYTE*)&Delta)[1] >> 1, (((BYTE*)&Delta)[2]));

				// evtl. Ton einschalten
				if ( SwitchTone )
					MIDI_OUT3(0x90+i, ton_auf_kanal[i].outkey, 64);  //3 velo speichern ??
			}
	}

	void OutputMidiFile::Sustain(char on, int channel)
	{ mutUnused(on); mutUnused(channel); STUBC;}

	void OutputMidiFile::MidiOut(DWORD data, size_t n)
	{
		Tracks.WriteDelta();

		if ( n == -1 )
			n = lMidiCode[(data & 0xF0) >> 5];

		for (int i = 0; i < n; i++) {
			Tracks.Add(data & 0xFF);
			data >>= 8;
		}
	}

	void OutputMidiFile::MidiOut(BYTE *p, size_t n)
	{
		Tracks.WriteDelta();

		for (int i = 0; i < n; i++)
			Tracks.Add(p[i]);
	}

	void OutputMidiFile::Quite(RouteClass * r)
	{
		for (int i = 0; i < 16; i++)
			if ( ton_auf_kanal[i].channel == r->GetId() )
				NoteOff(r->GetBox(), ton_auf_kanal[i].inkey, 64, r, ton_auf_kanal[i].unique_id);
	}


#ifdef WX
	wxString OutputMidiFile::TowxString() const {
		wxString s = OutputDeviceClass::TowxString() +
			wxString::Format(_T("\n  Name = %s\n  DevId = %d\n  Bending Range = %d\n  nKeyOn"),
					 Name.c_str(), DevId, bending_range, nKeyOn);
		s.Printf(_T("\n  KeyDir = [ %d"), KeyDir[0]);
		for (int i = 1; i<16; i++)
			s.Printf(_T(", %d"),KeyDir[i]);
		s.Printf(_T("]\n  ton_auf_kanal = [ t=%d,k=%d,f=%d"), 
			 ton_auf_kanal[0].inkey, 
			 ton_auf_kanal[0].outkey);
		for (int i = 1; i<16; i++)
			s.Printf(_T("; t=%d,k=%d,f=%d"), 
				 ton_auf_kanal[i].inkey, 
				 ton_auf_kanal[i].outkey);
		s+=_T("]");
		return s;
		}
#endif

	void OutputMidiFile::ReadData(wxConfigBase * config) 
	{
		bending_range = config->Read(_("Bending_Range"),
					     (long)bending_range);
	}

	void OutputMidiFile::WriteData(wxConfigBase * config) 
	{
		config->Write(_("Bending_Range"), (long)bending_range);
	}


	
// InputMidiFile -------------------------------------------------------

/*
  void CALLBACK _export MidiTimeFunc(UINT wTimerID, UINT wMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
  {
  ((InputMidiFile*)dwUser)->IncDelta();
  }
*/
	/// \todo: check for array borders
	inline unsigned long ReadDelta(BYTE * data, DWORD &position)
	{
		unsigned long l = 0;
		BYTE a;

		do {
			a = data[position];
			position++;
			l = (l << 7) + (a & 0x7F);
		} while ( a & 0x80);
		return l;
	}


/// Save route settings (filter settings) for a given route
/** Some route settings (e.g. filter settings) are device type 
 * specific. This function saves them in a tree storage.
 * \argument config (tree_storage *) Storage class, where the data will be saved.
 * \argument route (Route) Route whos data shall be saved.
 */
	void InputMidiFile::Save (tree_storage & config, const RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Midi File Input"));
		config.Write(_T("Filter Type"), route->Type);
		switch(route->Type) {
		case RTchannel: 
			config.Write(_T("Channel From"), route->IFrom);
			config.Write(_T("Channel To"), route->ITo);
			break;
		case RTstaff:
			config.Write(_T("Track From"), route->IFrom);
			config.Write(_T("Track To"), route->ITo);
			break;
		case RTelse:
		case RTall:
			break;
		}
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}



	/// Loade route settings (filter settings) for a given route
	/** Some route settings (e.g. filter settings) are device type 
	 * specific. This function loads them from a tree storage.
	 * \argument config (tree_storage *) Storage class, where the data will be restored from.
	 * \argument route (Route ) Route whos data shall be loaded.
	 */
	void InputMidiFile::Load (tree_storage & config, RouteClass *  route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Midi File Input"));
		route -> Type = (RouteType) config.Read(_T("Filter Type"), (int)RTchannel);
		switch(route->Type) {
		case RTchannel: 
		{
			int oldfrom, oldto;
			oldfrom = route->IFrom = config.Read(_T("Channel From"), GetMinChannel());
			oldto = route->ITo = config.Read(_T("Channel To"), GetMaxChannel());
			bool correct = true;
			if (route->IFrom < GetMinChannel()) {
				correct = false;
				route->IFrom = GetMinChannel();
			}
			if (route->ITo > GetMaxChannel()) {
				correct = false;
				route->ITo = GetMaxChannel();
			}
			if (!correct)
				wxMessageBox(wxString::Format(_("The Channel range %d--%d of the MIDI file %s must be inside %d--%d. The current route had to be corrected."),
							      oldfrom,oldto,GetName().c_str(),GetMinChannel(),GetMaxChannel()),
					     _("Warning loading route"),wxICON_EXCLAMATION);
			break;
		}
		case RTstaff:
		{
			int oldfrom, oldto;
			route -> IFrom = oldfrom = config.Read(_T("Track From"), GetMinTrack());
			route -> ITo = oldto = config.Read(_T("Track To"), GetMaxTrack());
			bool correct = true;
			if (route->IFrom < GetMinTrack()) {
				correct = false;
				route->IFrom = GetMinTrack();
			}
			if (route->ITo > GetMaxTrack()) {
				correct = false;
				route->ITo = GetMaxTrack();
			}
			if (!correct)
				wxMessageBox(wxString::Format(_("The Track range %d--%d of the MIDI file %s must be inside %d--%d. The current route had to be corrected."),
							      oldfrom,oldto,GetName().c_str(), GetMinTrack(), GetMaxTrack()),
					     _("Warning loading route"),wxICON_EXCLAMATION);
			break;
		}
		case RTelse:
		case RTall:
			break;
		}
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}


	bool InputMidiFile::Open()
	{
		mutASSERT(!isOpen);
		DEBUGLOG (midifile, _T("start"));
		Track = 0;
		curDelta = 0;
		TrackPos = 0;
		StatusByte = 0;
		TicksPerQuarter = 0;
		MMSPerQuarter = wxLL(500000); // Default value: 120 bpm = 0.5s/Quarter

		// read file
		mutOpenIFstream(is, Name);

		if ( mutStreamBad(is) ) {
			DEBUGLOG (midifile, _T("Opening Stream failed"));
			Mode = DeviceCompileError;
//			InDevChanged = 1;
//		LAUFZEIT_ERROR1(_("Can not open Midi input file '%s'."), GetName());
			LAUFZEIT_ERROR1(_("Can not open Midi input file '%s'."), Name.c_str());
			isOpen = false;
			return false;
		}

		// Header Chunk
		char Header[5] = {0,0,0,0,0};
		mutReadStream(is,Header, 4);
		if (strcmp(Header,"MThd")) {
			LAUFZEIT_ERROR1(_("File '%s' is not a valid midi file."), Name.c_str())	;
			isOpen = false;
			return false;
		}
		DWORD l = ReadLength(is);
		if (l!=6) {
			LAUFZEIT_ERROR2(_("Unknown header (chunk length %d) in file '%s'."),l, Name.c_str());
			isOpen = false;
			return false;
		}

		BYTE a, b;

		// file type
		a = mutGetC(is); //mutGetC(is,a);
		FileType = ((int)a << 8) + mutGetC(is); //mutGetC(is,FileType);
		if (FileType > 3) {
			LAUFZEIT_ERROR2(_("Unknown file typ %d in file '%s'."), FileType, Name.c_str());
			isOpen = false;
			return false;
		}

		// number of tracks
		a = mutGetC(is); //mutGetC(is,a);
		b = mutGetC(is); // mutGetC(is,b);
		nTrack = (((int)a) << 8) + b;

		// speed info
		a = mutGetC(is); //mutGetC(is,a);
		b = mutGetC(is); //mutGetC(is,b);
		Speed = (((int)a) << 8) + b;
		DEBUGLOG(midifile, 
			 _T("File type: %d; Tracks: %d; Speed: %dTicks/Qarter"),
			 FileType, 
			 nTrack, 
			 Speed);

		NRT_Speed = Speed;

		// rest of header
		DWORD i;

		for (i = 6; i < l && !mutStreamEOF(is) && mutStreamGood(is); i++ )
			a = mutGetC(is);// mutGetC(is,a);

		// Tracks lesen
		Track = (BYTE**) malloc(nTrack*sizeof(BYTE*));

		for (i = 0; i < nTrack; i++ ) {
			mutReadStream(is,Header, 4);
			l = ReadLength(is);

			if ( l > (long) 64000 ) {
				Mode = DeviceCompileError;
//				InDevChanged = 1;
				LAUFZEIT_ERROR1(_("Midi input file '%s' is too long."), Name.c_str());
				DEBUGLOG (midifile, _T("Midi input file '%s' is too long."),Name.c_str());
				isOpen = false;
				return false;
			}

			Track[i] = (BYTE*)malloc(l*sizeof(BYTE));

			if ( l > 32000 ) {
				mutReadStream(is, (char*)Track[i], 32000);
				mutReadStream(is, (char*)&Track[i][32000], l-32000);
			} else
				mutReadStream(is, (char*)Track[i], l);

			if ( /*is.gcount() != l ||*/ mutStreamBad(is) ) {
				DEBUGLOG (midifile, _("Midi input file '%s' produces errors."),
					  Name.c_str());
				Mode = DeviceCompileError;
//				InDevChanged = 1;
				LAUFZEIT_ERROR1(_("Midi input file '%s' produces errors."), Name.c_str());
				isOpen = false;
				return false;
			}
		}

		mutCloseStream(is);

		// Daten vorbereiten
		curDelta = (mutint64 *)malloc(nTrack*sizeof(mutint64));
		TrackPos = (DWORD*)malloc(nTrack*sizeof(DWORD));
		StatusByte = (BYTE*)malloc(nTrack*sizeof(BYTE));
		
		return base::Open();
	}

	void InputMidiFile::Close()
	{
		base::Close();

		if ( Mode == DeviceCompileError )
			return;

		for (size_t i = 0; i < nTrack; i++ )
			free(Track[i]);

		free(Track);
		free(TrackPos);
		free(curDelta);
		free(StatusByte);
	}

	void InputMidiFile::Stop()
	{
		base::Stop();

		// OK ?
		if ( Mode == DeviceCompileError )
			return;

		// Delta-Times lesen
		minDelta = NO_DELTA;

		MMSPerQuarter = 1000000l;

		for (size_t i = 0; i < nTrack; i++ ) {
			TrackPos[i] = 0;
			curDelta[i] = ReadDelta(Track[i], TrackPos[i]);

			if ( curDelta[i] < minDelta )
				minDelta = curDelta[i];
		}
	}

	mutint64 InputMidiFile::PrepareNextEvent()
	{
		mutint64 passedDelta = minDelta;

		mutint64 NewMinDelta = NO_DELTA;

		for (size_t i = 0; i < nTrack; i++ ) {
			if ( curDelta[i] != NO_DELTA ) {
				if ( curDelta[i] <= passedDelta )
					curDelta[i] = ReadMidiProceed(i, passedDelta-curDelta[i]);
				else
					curDelta[i] -= passedDelta;

				if ( curDelta[i] != NO_DELTA && 
				     curDelta[i] < NewMinDelta ) {
					NewMinDelta = curDelta[i];
				}
			}
			DEBUGLOG(midifile,_T("Track: %d,delta: %ldμs"),i,curDelta[i]);
		}


		minDelta = NewMinDelta;
		DEBUGLOG(midifile,_T("Next event after %ldμs (NO_DELTA = %ld)"),minDelta,NO_DELTA);
		if (NewMinDelta == NO_DELTA) {
			return GetNO_DELTA();
		} else  {
			return minDelta;
		}
	}

	mutint64 InputMidiFile::ReadMidiProceed(size_t nr, mutint64 time)
	{
		mutint64 Delta = 0;
		size_t OldPos;

		while ( time >= Delta ) {
			time -= Delta;
			pData = &(Track[nr][TrackPos[nr]]);
			OldPos = TrackPos[nr];
			DWORD a = Track[nr][TrackPos[nr]++];

			if ( a & 0x80 )
				StatusByte[nr] = a;
			else {
				TrackPos[nr]--;
				a = StatusByte[nr];
			}

			BYTE SB = StatusByte[nr];

			if ( SB <  0xF0 ) // normaler midi code
			{
				int l = lMidiCode[(SB >> 4) & 07];
				int shift = 8;

				for (int i = 1; i < l; i++)
				{
					a += (DWORD)Track[nr][TrackPos[nr]++] << shift;
					shift += 8;
				}
			}
			else if ( SB == 0xF0 || SB == 0xF7 ) // SysEx I, SysEx II
			{
				DWORD EventLength = ReadDelta(Track[nr], TrackPos[nr]); // length
				TrackPos[nr] += EventLength;
			} else if ( SB == 0xFF ) // meta event
			{
				a += Track[nr][TrackPos[nr]++] << 8;         // event number
				DWORD EventLength = ReadDelta(Track[nr], TrackPos[nr]); // length

				if ( (a >> 8) == 0x58 ) // Time Signature
				{
					TicksPerQuarter = Track[nr][TrackPos[nr]+2];
					DEBUGLOG(midifile,_T("Change time signature to  %d ticks/qarter"),
						 TicksPerQuarter);
										
				} else if ( (a >> 8) == 0x51 ) // Tempo
				{
					long NewMMSPerQuarter =
						(((DWORD)Track[nr][TrackPos[nr]]) << 16) +
						(((DWORD)Track[nr][TrackPos[nr]+1]) << 8) +
						((DWORD)Track[nr][TrackPos[nr]+2]);

					for (size_t j = 0; j < nr; j++ )
						if ( curDelta[j] != NO_DELTA )
							curDelta[j] = (curDelta[j] * NewMMSPerQuarter) / MMSPerQuarter;

					for (REUSE(size_t) j = nr+1; j < nTrack; j++ )
						if ( curDelta[j] != NO_DELTA && curDelta[j] >= minDelta)
							curDelta[j] = ((curDelta[j]-minDelta) * NewMMSPerQuarter) / MMSPerQuarter +minDelta;
					
					DEBUGLOG(midifile,
						 _T("Change tempo from %ldμs to %ldμs per quarter (next event after %ld)"),
						 MMSPerQuarter, NewMMSPerQuarter, minDelta);

					MMSPerQuarter = NewMMSPerQuarter;
				}
				TrackPos[nr] += EventLength;
			} else if ( SB == 0xF2 ) // song position pointer
			{
				a += Track[nr][TrackPos[nr]++] << 8;
				a += Track[nr][TrackPos[nr]++] << 16;
			} else if ( SB == 0xF3 ) // song select
				a += Track[nr][TrackPos[nr]++] << 8;

			// ausf¸hren
			nData = TrackPos[nr] - OldPos;

//9    if ( *((BYTE*)(&a)) < 0xF0 )
			if ( a == 0x2FFF ) {
				TrackPos[nr] = 0xFFFFFFFF;
				return NO_DELTA;
			} else
				Proceed(a, nr);

			// Delta Time
			Delta = ReadDelta(Track[nr], TrackPos[nr]);
			
			DEBUGLOG(midifile,
				 _T("Next event on Track %d after %ld Ticks"),
				 nr, Delta);

			if ( RealTime ) {
				Delta = (MMSPerQuarter * Delta) / Speed;
				DEBUGLOG(midifile,
					 _T("This event on Track %d after %ldμs (time = %ldμs)"),
					 nr, Delta, time);
			}
		}

		return Delta - time;
	}

#define MIDICODE(i)				\
	(0xff & (midiCode >> (8*i)))
//(((BYTE*)(&midiCode))[i])

// f¸r bestimmte Route Codeverarbeitung
	void InputMidiFile::ProceedRoute(DWORD midiCode, Route route)
	{
	
		DEBUGLOG (midifile, _T("Code: %x, Active: %d, Out: %p"),midiCode,
			  route->Active,
			  route->GetOutputDevice().get());
		int Box = route->GetBox();
		BYTE MidiChannel = MIDICODE(0) & 0x0F;
		BYTE MidiStatus = MIDICODE(0) & 0xF0;
		DEBUGLOG (midifile, _T("Status: %x"), MidiStatus);

		switch ( MidiStatus ) {

		case 0x90: // Note On
			if ( MIDICODE(2) > 0 ) {
				if ( route->Active )
					AddKey(&mut_box[Box], MIDICODE(1), 0, route->GetId(), NULL);

				if ( route->GetOutputDevice() )
					route->GetOutputDevice()
						->NoteOn(Box, 
							 MIDICODE(1), 
							 MIDICODE(2), 
							 route.get(),
							 MidiChannel, 
							 &Cd[MidiChannel]);

				break;
			}

		case 0x80: // Note Off
			if ( route->Active )
				DeleteKey(&mut_box[Box], MIDICODE(1), 0, route->GetId());

			if ( route->GetOutputDevice() )
				route->GetOutputDevice()
					->NoteOff(Box, 
						  MIDICODE(1), 
						  MIDICODE(2), 
						  route.get(), 
						  MidiChannel);

			break;

		case 0xC0: // Programm Change
			Cd[MidiChannel].Sound = MIDICODE(1);

			break;

		case 0xB0:
			if ( MIDICODE(1) == 64 ) {
				Cd[MidiChannel].Sustain = MIDICODE(2);

				if ( route->GetOutputDevice() )
					route->GetOutputDevice()
						->Sustain(Cd[MidiChannel].Sustain,
							  MidiChannel);

				break;
			} else if ( MIDICODE(1) == 0 ) { // BankSelect MSB
				Cd[MidiChannel].BankSelectMSB = MIDICODE(2);
				break;
			} else if ( MIDICODE(1) == 32 ) {// BankSelectLSB
				Cd[MidiChannel].BankSelectLSB = MIDICODE(2);
				break;
			}

		case 0xA0:

		case 0xD0: // Key Pressure, Controler, Channel Pressure
			//3 ??
			break;

		case 0xF0:
			if ( route->GetOutputDevice() )
				route->GetOutputDevice()->MidiOut(pData, nData);

		}

		if ( Box >= 0 && route->Active )
			for (int i = 0; i < lMidiCode[MidiStatus >> 5]; i++) {
				MidiAnalysis(&mut_box[Box], MIDICODE(0));
				midiCode >>= 8;
			}
	}

	
#ifdef WX
	wxString InputMidiFile::TowxString() const {
		return InputDeviceClass::TowxString() +
			wxString::Format(_T("\n  FileType = %d\n  nTrack = %d\n\
  Speed = %d\n  TrackPos = %d\n  curDelta = %ld\n\
  minDelta = %ld\n  StatusByte = %x\n\
  Busy = %d\n  TicksPerQuarter = %d\n  MMSPerQuarter = %ld"),
					 FileType, nTrack, 
					 Speed, (Track?(*Track?**Track:-1):-2), 
					 (TrackPos?*TrackPos:-1),
					 (curDelta?*curDelta:-1), 
					 minDelta,
					 (StatusByte?*StatusByte:-1), 
					 Busy, TicksPerQuarter, MMSPerQuarter);
	}
#endif
	

// Routen testen und jenachdem entsprechend Codeverarbeitung
	void InputMidiFile::Proceed(DWORD midiCode, int track)
	{
		DEBUGLOG(midifile,_T("midiCode: %x, track %d"),midiCode,track);
		char DidOut = 0;
		
		for (routeListType::iterator R = routes.begin(); 
		     R != routes.end(); R++)
			switch ( (*R)->Type ) {

			case RTchannel:
				if ( (*R)->Check(midiCode & 0x0F) ) {
					ProceedRoute(midiCode, *R);
					DidOut = 1;
				}

				break;

			case RTstaff:
				if ( (*R)->Check(track) ) {
					ProceedRoute(midiCode, (*R));
					DidOut = 1;
				}

				break;

			case RTelse:
				if ( DidOut )
					break;

			case RTall:
				ProceedRoute(midiCode, (*R));
			}

		FLUSH_UPDATE_UI;
	}

	MidiFileFactory::~MidiFileFactory() {}

	OutputDeviceClass * MidiFileFactory::DoCreateOutput () const
	{
		return new OutputMidiFile();
	}

	OutputDeviceClass * MidiFileFactory::DoCreateOutput (int devId,
						      const mutStringRef name, 
						      int id) const
	{
		return new OutputMidiFile(devId,name,id);
	}

	OutputDeviceClass * MidiFileFactory::DoCreateOutput (int devId,
						      const mutStringRef name, 
						      MutaborModeType mode, 
						      int id) const
	{
 		STUBC;
		return NULL;
#if 0
		return new OutputMidiFile(devId,name,id);
#endif
	}


	InputDeviceClass * MidiFileFactory::DoCreateInput () const
		
	{
		return new InputMidiFile();
	}

	InputDeviceClass * MidiFileFactory::DoCreateInput (int devId,
							   const mutStringRef name, 
							   int id) const
	{
		return new InputMidiFile(devId,name,DeviceStop,id);
	}

	InputDeviceClass * MidiFileFactory::DoCreateInput (int devId,
							   const mutStringRef name, 
							   MutaborModeType mode, 
							   int id) const
	{
		return new InputMidiFile(devId,name,mode,id);
	}

}


///\}
