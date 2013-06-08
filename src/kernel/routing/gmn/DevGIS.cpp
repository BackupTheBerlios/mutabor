/** \file 
 ********************************************************************
 * Description
 *
 * Copyright:   (c) 2008 TU Dresden
 * \author  Tobias Schlemmer <keinstein@users.berlios.de>
 * \date 
 * $Date: 2011/11/02 14:31:57 $
 * \version $Revision: 1.11 $
 * \license GPL
 *
 *
 *
 ********************************************************************
 * \addtogroup route
 * \{
 ********************************************************************/
// #################################################################
// Mutabor, GIS - Devices
// #################################################################

#include "src/kernel/routing/gmn/DevGIS.h"
#include "src/kernel/routing/gmn/GSP.h"
#include <math.h>
#include "src/kernel/MidiKern.h"
#include "src/kernel/GrafKern.h"
#include "src/kernel/routing/Route-inlines.h"

#ifdef RTMIDI
#else
#ifndef MMSYSTEM_H
#define MMSYSTEM_H
//  #define WINVER 0x030a
#include <mmsystem.h>
#endif
#endif
#include "src/kernel/Execute.h"

//int ausgabe_instrument[16][4];

namespace mutabor {

	mutString CurrentId;
	mutString CurrentSep;

#define ROUND_FACTOR 1000000
#define ROUND(x) x = (floor(x*ROUND_FACTOR+0.5)) / ROUND_FACTOR
//#define ROUND(x) x=x

	void GetKeyPitch(int taste, tone_system *tonsystem, int &key, double &pitch)
	{
		int Index = (taste - tonsystem->anker) % tonsystem->breite;
		int Abstand = (taste - tonsystem->anker) / tonsystem->breite;

		if ( Index < 0 ) {
			Index += tonsystem->breite;
			Abstand--;
		}

		long Tone = (tonsystem)->ton[Index];

		if ( !Tone ) {
			key = NO_KEY;
			pitch = 0;
			return;
		}

		pitch = ((double)(tonsystem)->periode) / 0x01000000 * Abstand + ((double)Tone) / 0x01000000;

		key = (int) pitch;
		pitch -= key;

		if ( pitch < -0.5 ) {
			pitch++;
			key--;
		} else if ( pitch > 0.5 ) {
			pitch--;
			key++;
		}

		ROUND(pitch);
	}

// ##################################################################
// OutputGis
/*
  void OutputGis::NoteOn(int instr, ton_system * tonsys, int taste, int velo)
  {
  int Key;
  double Pitch;
  GetKeyPitch(taste, tonsys, Key, Pitch);
  GisWriteHead *h = GetMatchingHeader(&Head, CurrentId);
  if ( !h ) return ;
  h->CommaAtEnd = 0;
  ChordNote *ANote = h->GetFreeNote();
  ANote->CheckCloseTie();
  ANote->MutNoteOn(Key, Pitch, instr, taste, CurrentSep);
  }

  void OutputGis::NoteOff(int instr, int taste, int velo)
  {
  GisWriteHead *h = GetMatchingHeader(&Head, CurrentId);
  if ( !h ) return;
  ChordNote *ANote = h->GetNote(instr, taste);
  if ( !ANote )
  return;
  ANote->MutNoteOff();
  ANote->CheckCloseTie();
  if ( !ANote->Boss->NoteOn ) ANote->Boss->WriteChord();
  }

  void OutputGis::NotesCorrect(int instr, ton_system * tonsys)
  {
  // search sounding notes of instrument instr
  for (GisWriteHead *h = Head; h; h = h->Next)
  for (ChordNote *ANote = h->ChordNotes; ANote; ANote = ANote->Next)
  if ( ANote->InstrId == instr && ANote->Status & CNNoteOn )
  {
  // check, wether pitch has changed
  int Key;
  double Pitch;
  GetKeyPitch(ANote->Taste, tonsys, Key, Pitch);
  if ( ANote->Cmp(Key, Pitch) )
  continue;
  // look for the old key
  int OldKey = ANote->Key;
  char *OldSep = (*ANote->Cursor)->Sep;
  (*ANote->Cursor)->Sep = strdup(" ");
  int Taste = ANote->Taste;
  // insert tie
  ANote->nTie++;
  // finish the note
  ANote->MutNoteOff();
  // calculate new values
  if ( Key != NO_KEY )
  {
  Pitch += (Key - OldKey);
  Key = OldKey;
  }
  // start the new note
  ANote->MutNoteOn(Key, Pitch, instr, Taste, OldSep);
  // correct nTie (MutNoteOn does nTie++)
  ANote->nTie--;
  // clear old separator
  if ( OldSep )
  free(OldSep);
  }
  }
*/
	void OutputGis::Gis(GisToken *token, char turn)
	{
		GisWriteHeadGis(&Head, CurrentId, token, turn);
		STUBC;
	}

// InputGis ------------------------------------------------------------
/*
  void CALLBACK _export GisTimeFunc(UINT wTimerID, UINT wMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
  {
  ((InputGis*)dwUser)->IncDelta();
  }
*/

/// Save current device settings in a tree storage
/** \argument config (tree_storage) storage class, where the data will be saved.
 */
	void OutputGis::Save (tree_storage & config) 
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.Write(_T("File Name"),Name);
		mutASSERT(oldpath == config.GetPath());
	}

/// Save route settings (filter settings) for a given route
/** Some route settings (e.g. filter settings) are device type 
 * specific. This function saves them in a tree storage.
 * \argument config (tree_storage *) Storage class, where the data will be saved.
 * \argument route (Route ) Route whos data shall be saved.
 */
	void OutputGis::Save (tree_storage & config, const RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Guido File Output"));
		STUBC;
		mutUnused(route);
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}


/// Load current device settings from a tree storage
/** \argument config (tree_storage) storage class, where the data will be loaded from.
 */
	void OutputGis::Load (tree_storage & config)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		Name = config.Read(_T("File Name"), mutEmptyString);
		mutASSERT(oldpath == config.GetPath());
	}

/// Load route settings (filter settings) for a given route
/** Some route settings (e.g. filter settings) are device type 
 * specific. This function loads them from a tree storage.
 * \argument config (tree_storage *) Storage class, where the data will be restored from.
 * \argument route (Route ) Route whos data shall be loaded.
 */
	void OutputGis::Load (tree_storage & config, RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Guido File Output"));
		STUBC;
		mutUnused(route);
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}



/// Save route settings (filter settings) for a given route
/** Some route settings (e.g. filter settings) are device type 
 * specific. This function saves them in a tree storage.
 * \argument config (tree_storage *) Storage class, where the data will be saved.
 * \argument route (Route ) Route whos data shall be saved.
 */
	void InputGis::Save (tree_storage & config, const RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Guido File Input"));
		// ...
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}

/// Load route settings (filter settings) for a given route
/** Some route settings (e.g. filter settings) are device type 
 * specific. This function loads them from a tree storage.
 * \argument config (tree_storage *) Storage class, where the data will be restored from.
 * \argument route (Route ) Route whos data shall be loaded.
 */
	void InputGis::Load (tree_storage & config, RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Guido File Input"));
		// ...
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}


	bool InputGis::Open()
	{
		Data = GisParse(Name);

		if ( GspError ) {
			/* //4    *Echo << "\n  Parsing interupted !\n";
			 *Echo << "  ErrorNr: " << GspError << " - " << GspErrorText[GspError] << "\n";
			 *Echo << "  Line: " << GspErrorLineNr << ", " << GspErrorPos << "\n";
			 *Echo << "  " << GspErrorLine << "\n  "; */

			DEBUGLOG (gmnfile, _T("Issuing Compiler warning 9..."));

			compiler_warning(9, C_STR(Name), GspErrorLineNr, GspErrorPos, mutC_STR(GspErrorText[GspError]));

			DEBUGLOG (gmnfile, _T("... done"));
			Mode = DeviceCompileError;
//			InDevChanged = 1;
			return false;
		}

		/*  Head = new GisReadArtHead(NULL, Data, Id);
		    DEBUGLOG (other, _T("Head = %p"),Head);
		    Head->Box = 0; /// hier muﬂ noch was hin
		    Head->PrevPtr = (GisReadHead **)&Head;
		    //  Head->Prev = Head;
		    DEBUGLOG (gmnfile, _T("Head = %p"),Head);
		    // Mode setzen
		    Mode = DeviceStop; 
		*/
		// initialisieren
		DEBUGLOG (gmnfile, _T("Head = %p"),Head);
		return base::Open();
	}

	void InputGis::Close()
	{
		base::Close();
		
		// Speicher freigeben

		if ( Mode == DeviceCompileError )
			return;

		if ( Head ) {
			delete Head;
			Head = 0;
		}

		if ( Data ) {
			delete Data;
			Data = 0;
		}
	}

	void InputGis::Stop()
	{
		base::Stop();

		// OK ?
		if ( Mode == DeviceCompileError )
			return;

		// Header auf Start setzen
		if ( Head ) {
			delete Head;
			Head = 0;
		}

		Head = new GisReadArtHead(NULL, Data, Id);

		DEBUGLOG (gmnfile, _T("Head = %p"),Head);
		Head->Box = 0; /// hier muﬂ noch was hin
		DEBUGLOG (gmnfile, _T("Head = %p"),Head);
		//  Head->Prev = Head;
		Head->PrevPtr = (GisReadHead**)&Head;
		DEBUGLOG (gmnfile, _T("Head = %p"),Head);
		// Delta-Times lesen
		minDelta = 0;
		Mode = DeviceStop;
		Panic();
	}

	void InputGis::Panic() {
#warning "Implement Panic()"
	}

        // Gis arbeitet auf "tick" - Basis, ein tick = 1ms
	mutint64 InputGis::PrepareNextEvent()
	{
		DEBUGLOG (gmnfile, _T("Next tone. Mindelta = %d"),minDelta);

		minDelta = ReadOn(minDelta);

		// while ((minDelta = ReadOn(actDelta)) == -1)
		//  if (!(Head->Cursor)) break;

		DEBUGLOG (gmnfile, _T("Calculating next step"));

		return minDelta;
	}

	void MutaborTag(GisReadArtHead *h, GisToken *Para, int box)
	{
		if ( !Para || Para->Type() != GTParaStr )
			return; // strange parameters

		mutString ParaName = ((GisParaStr*)Para)->s;

		Para = Para->Next;

		if ( mutStrEq(ParaName, mutT("key")) ) {
			if ( GetGisType(Para) == GTParaStr )
///		KeyboardIn(Box, ((GisParaStr*)Para)->s);
				KeyboardIn(&(mut_box[box]), ((GisParaStr*)Para)->s);
		}

		if ( mutStrEq(ParaName, mutT("box")) || mutStrEq(ParaName, mutT("instrument")) ) {
			if ( GetGisType(Para) == GTParaInt )
				h->Box = ((GisParaInt*)Para)->i;
		}
	}

	ChannelData Cd(0, 0);

	void InputGis::Proceed(GisReadArtHead *h, char turn, Route route)
	{
		mutASSERT(h->Cursor);
		CurrentId = h->Id;
		CurrentSep = h->Cursor->Sep;
		// calculate box
		int Box = route->GetBox();

		if ( Box == -1 )
			Box = h->Box;

		DEBUGLOG (gmnfile, _T("Id: %s, Box: %d, Sep: %s"),
			  CurrentId.c_str(), Box, CurrentSep.c_str());

		OutputDevice out = route->GetOutputDevice();

		// check wether no box should be used
		if ( Box == -2 ) {
			if ( out )
				out->Gis(h->Cursor, turn);

			return;
		}

		// proceed the token
		int Key;

		switch ( GetGisType(h->Cursor)) {

		case GTTag:

		case GTTagBegin:
			if ( ((GisTag*)(h->Cursor))->Id == TTmutabor )
				MutaborTag(h, ((GisTag*)(h->Cursor))->Para, Box);

			if ( ((GisTag*)(h->Cursor))->Id == TTalter ) return;

			if ( out ) out->Gis(h->Cursor, turn);

			break;

		case GTTagEnd:
			if ( (((GisTagEnd*)(h->Cursor))->Begin)->Id == TTalter ) return;

			if ( out )out->Gis(h->Cursor, turn);

			break;

		case GTNote:
			Key = ((GisNote*)(h->Cursor))->GetKey();

			if ( Key == -1 ) return ;

			Key += h->GetOctave()*12;

			/** \todo provide working GIS note id. Currently Mutabor outputs only MIDI so it's no problem */
			if ( turn != 1 && route->GetActive() ) {
				if ( turn )
					DeleteKey(&mut_box[Box], Key, 0, route->GetId());
				else

					AddKey(&mut_box[Box], Key, 0, route->GetId(), NULL);
			}

			if ( turn != 2 && route->GetOutputDevice() ) {
				if ( turn )
					route->GetOutputDevice()
						->NoteOff(&mut_box[Box], 
							  Key, 
							  h->GetIntensity(turn), 
							  route.get(), 
							  0,
							  false); //4 ?? channelid aus staff
				else {
					Cd.program_change(h->GetInstr());
					route->GetOutputDevice()
						->NoteOn(&mut_box[Box], 
							 Key, 
							 h->GetIntensity(turn), 
							 route.get(), 
							 0, 
							 Cd);
				}
			}

			break;

		default:
			if ( out ) out->Gis(h->Cursor, turn);
		}
	}

	void InputGis::ProceedRoute(GisReadArtHead *h, char turn)
	{
		mutASSERT(h);
		mutASSERT(h->Cursor);
		DEBUGLOG (gmnfile, _T("h->Id = '%s' (%d), Id = '%s' (%d)"),
			  (h->Id).c_str(),(h->Id).Len(), Id.c_str(), Id.Len());
		mutChar staff = h->Id[mutLen(Id)];
		bool DidOut = false;
		DEBUGLOG (gmnfile, _T("staff: %d, DidOut: %d"),staff, DidOut);

		for (routeListType::iterator R = routes.begin(); 
		     R!= routes.end(); R++) {
			DEBUGLOG (gmnfile, _T("Route type: %d, DidOut: %d"),
				  (*R)->GetType(), DidOut);

			switch ( (*R)->GetType() ) {

			case RTstaff:
				if ( (*R)->Check(staff) ) {
					Proceed(h, turn, *R);
					DidOut = true;
				}

				break;

			case RTchannel:
				if ( (*R)->Check(h->Box) ) {
					Proceed(h, turn, *R);
					DidOut = true;
				}

				break;

			case RTelse:
				if ( DidOut )
					break;

			case RTall:
				Proceed(h, turn, *R);
			}
		}
	}


#ifdef DEBUG
// currently not used -- even in debug mode
#if 0
	static void printHeadChain(std::ostream & f,GisReadHead *H)
	{
		f << "  ";

		if (H)
			if (H->PrevPtr) {
				f << "[" << (void *)(H->PrevPtr) << std::flush;

				if (*(H->PrevPtr)) {
					f << "->(" << std::flush
					  << (void *)(*(H->PrevPtr)) << ")" << std::flush;
				}

				f << "]";
			}

		while (H) {
			f << "->" << typeid(*H).name() << "(" << (void *)H << ")" << std::flush;
			H = H->Next;
		}

		f << std::endl;
	}
#endif
#else
#define printHeadChain(a,b)
#endif
	// return -1 heißt Ende der GMN
        // ein und ausgabe in ticks
        // Sinn der ticks: verschiedene Tempi in den Spuren


	mutint64 InputGis::ReadOn(mutint64 delta)
	{
		GisReadHead **H = (GisReadHead **)&Head;
		mutint64 MinDelta = GetNO_DELTA();

	beginloop:

		//  printHeadChain(DEBUGLOG (gmnfile, _T("Head Chain:")), *H);

		while ( *H ) {

			GisReadArtHead *h = dynamic_cast<GisReadArtHead *> (*H);
			DEBUGLOG (gmnfile, _T("H = %p; h = %p"),H,
				  dynamic_cast<GisReadArtHead *>(*H));

			mutASSERT(h);
			DEBUGLOG (gmnfile, _T("h->nSub = %d"),h->nSub);

			if ( h->nSub > 0) { // header has subsGisReadHead
				H =  &(h->Next);
				continue;
			}

			// printHeadChain(DEBUGLOG (gmnfile, _T("Working Chain:")), *H);
			//DEBUGLOG (gmnfile, _T("Reading on with %p\n%s"), *H, (GISPrettyPrint(*Head).c_str()));

			if ( h->nSub == 0 ) { // all subs has finished
				ProceedRoute(h, h->Turn++);   // end of segment or sequenz
				DEBUGLOG (gmnfile, _T("CursorNext()"));
				h->CursorNext();
				h->Time = 0;
				h->Delta = 0;
			}

			DEBUGLOG (gmnfile, _T("h->Delta = %d"),h->Delta);

			if ( h->Delta > 0 ) {// header in normal state
				DEBUGLOG (gmnfile, 
					  _T("Time: %d/%d, delta: %d, speed: %d"),
					  h->Time.numerator(),
					  h->Time.denominator(), 
					  delta,
					  h->GetSpeedFactor());
				h->Time -= frac(delta, h->GetSpeedFactor());
				h->Delta -= delta;

				if ( h->Delta <= 0 ) {
					ProceedRoute(h, h->Turn++);

					if ( h->Turn == 2 ) {
						DEBUGLOG(gmnfile, _T("Turn:2; Moving time2 to time (old: %s, new: %s)"),::TowxString(h->Time).c_str(), ::TowxString(h->Time2).c_str());
						h->Time = h->Time2;
						h->Time2 = 0;
					}

					if ( h->Turn > 2 ) {
						DEBUGLOG (gmnfile, _T("CursorNext()"));
						h->CursorNext();
					}
				}

				h->Delta = (h->GetSpeedFactor() * h->Time.numerator())

					/ h->Time.denominator();
				DEBUGLOG (gmnfile, _T("Time: %ld/%ld, Time2: %ld/%ld, delta: %ld, speed: %ld"),
					  h->Time.numerator(),h->Time.denominator(),
					  h->Time2.numerator(),h->Time2.denominator(),
					  delta,
					  h->GetSpeedFactor());
			}

			h->nSub = -1; // normal header

			// now check, wether count down Time is 0
			// if h->time = 0 then h->Cursor points to the GisToken next to proceed

			DEBUGLOG (gmnfile, _T("h->Delta = %d"),h->Delta);

			while ( h->Delta == 0 ) { // read next tokens
				DEBUGLOG (gmnfile, _T("h->Turn = %d"),h->Turn);

				if ( h->Turn) {
					ProceedRoute(h, h->Turn++);
					DEBUGLOG (gmnfile, _T("h->Delta = %d, h->Turn = %d"),h->Delta, h->Turn);

					if ( h->Turn == 2 ) {
						DEBUGLOG(gmnfile, _T("Turn:2; Moving time2 to time (old: %ld, new: %ld)"),::TowxString(h->Time).c_str(), ::TowxString(h->Time2).c_str());
						h->Time = h->Time2;
						h->Time2 = 0;
						h->Delta = (h->GetSpeedFactor() * h->Time.numerator())
							/ h->Time.denominator();
						DEBUGLOG (gmnfile, _T("h->Delta = %d * %ld / %ld = %ld"), h->GetSpeedFactor(),
							  h->Time.numerator(), h->Time.denominator(), h->Delta);
					}

					if ( h->Turn > 2 ) {
						h->CursorNext();
					} else {
						DEBUGLOG2(gmnfile,_T("continue loop"));
						continue;
					}
				}

				if ( !h->Cursor ) { // header finished, kick away
					h->CutOut();

					if ( h->Boss ) h->Boss->nSub--; // inform the boss

					delete h;

					DEBUGLOG2(gmnfile,_T("goto beginloop"));

					goto beginloop;
				}

				// proceed
				DEBUGLOG (gmnfile, _T("Proceed with Turn 0"));

				ProceedRoute(h, 0);

				DEBUGLOG (gmnfile, _T("h->Read()"));

				h->Read();

				DEBUGLOG (gmnfile, _T("Time: %d/%d, Time2: %d/%d, delta: %d"),
					  h->Time.numerator(),h->Time.denominator(),
					  h->Time2.numerator(),h->Time2.denominator(),
					  delta);

				h->Delta = (h->GetSpeedFactor() * h->Time.numerator())
					/ h->Time.denominator();

				DEBUGLOG (gmnfile, _T("h->Delta = %d * %d / %d = %d"), h->GetSpeedFactor(),
					  h->Time.numerator(), h->Time.denominator(), h->Delta);

				DEBUGLOG (gmnfile, _T("Time: %d/%d, Time2: %d/%d, delta: %d, speed: %d"),
					  h->Time.numerator(),h->Time.denominator(),
					  h->Time2.numerator(),h->Time2.denominator(),
					  delta,
					  h->GetSpeedFactor());

				if ( h->nSub != -1 ) {
					DEBUGLOG2(gmnfile,_T("goto beginloop"));
					goto beginloop;
				}

				if ( !(h->Time) ) {// token without duration
					DEBUGLOG (gmnfile, _T("CursorNext()"));
					h->CursorNext();
				}
			}

			// check MinTime
			DEBUGLOG (gmnfile, _T("h->Delta = %d, MinDelta = %d"),h->Delta,MinDelta);

			if ( !IsDelta(MinDelta) || (MinDelta >= 0 && h->Delta < MinDelta) )
				MinDelta = h->Delta;

			// next Header
			H = &(h->Next);
		}

		DEBUGLOG2(gmnfile,_T("returning %d"),MinDelta);

		return MinDelta;
	}


	GisFactory::~GisFactory() {}

	OutputDeviceClass * GisFactory::DoCreateOutput () const
	{
		return new OutputGis();
	}

	OutputDeviceClass * GisFactory::DoCreateOutput (int devId,
						 const mutStringRef name, 
						 int id) const
	{
		return new OutputGis(devId,name,id);
	}

	OutputDeviceClass * GisFactory::DoCreateOutput (int devId,
						 const mutStringRef name, 
						 MutaborModeType mode, 
						 int id) const
	{
 		STUBC;
		return NULL;

		OutputGis * dev = new OutputGis(devId, name,id);
		switch (mode) {
		case DevicePause:
		case DeviceStop:
		case DevicePlay:
			dev -> Open() ; 
			break;
		case DeviceUnregistered:
		case DeviceCompileError:
		case DeviceTimingError:
		default:
			;
		}
		return dev;
	}


	InputDeviceClass * GisFactory::DoCreateInput () const
		
	{
		return new InputGis();
	}

	InputDeviceClass * GisFactory::DoCreateInput (int devId,
					       const mutStringRef name, 
					       int id) const
	{
		return new InputGis(devId, name, DeviceStop,id);
	}

	InputDeviceClass * GisFactory::DoCreateInput (int devId,
					   const mutStringRef name, 
					   MutaborModeType mode, 
					   int id) const
	{
		InputGis * dev = new InputGis(devId,name,mode,id);
		/*
		switch (mode) {
		case DevicePause:
		case DeviceStop:
			dev -> Open(); 
			break;
		case DevicePlay:
			dev -> Open();
			dev -> Play();
			break;
		case DeviceUnregistered:
		case DeviceCompileError:
		case DeviceTimingError:
		default:
			UNREACHABLEC;
		}
		*/
		return dev;
	}




}

///\}
