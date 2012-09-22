/** \file 
 ********************************************************************
 * Midi Port IO (Mutabor layer)
 *
 * Copyright:   (c) 1998-2011 TU Dresden
 * \author  R.Krauße
 * Tobias Schlemmer <keinstein@users.berlios.de>
 * \date 
 * $Date: 2012/01/29 22:08:36 $
 * \version $Revision: 1.16 $
 * \license GPL
 *
 ********************************************************************
 * \addtogroup route
 * \{
 ********************************************************************/

#include "src/kernel/routing/midi/DevMidi.h"
#include "src/kernel/Execute.h"
#include "src/kernel/GrafKern.h"
#include "src/kernel/MidiKern.h"
#include "src/kernel/routing/Route-inlines.h"
#include "wx/msgdlg.h"

// OutputMidiPort ------------------------------------------------------

//static long freq ;

#ifdef RTMIDI
#include "RtMidi.h"
#endif

#include "src/kernel/routing/midi/midicmn-inlines.h"

namespace mutabor {

	template class CommonMidiOutput<MidiPortOutputProvider,OutputDeviceClass>;

	using namespace midi;
	
	extern  RtMidiOut * rtmidiout;
	extern RtMidiIn *rtmidiin;

#if 0 // old windows code
#define MIDI_OUT3(code1, code2, code3)			\
	midiOutShortMsg(hMidiOut,			\
			((DWORD) (code3) << 16) +	\
			((DWORD) (code2) << 8) +	\
			((DWORD) (code1)))

#define MIDI_OUT2(code1, code2)				\
	midiOutShortMsg(hMidiOut,			\
			((DWORD) (code2) << 8) +	\
			((DWORD) (code1)))
#endif



        /// Save current device settings in a tree storage
        /** \argument config (tree_storage) storage class, where the data will be saved.
	 */
	void OutputMidiPort::Save (tree_storage & config) 
	{
		config.Write(_T("Device Id"),DevId);
		config.Write(_T("Device Name"),Name);
		config.Write(_T("Bending Range"),GetBendingRange());
	}

        /// Save route settings (filter settings) for a given route
	/** Some route settings (e.g. filter settings) are device type 
	 * specific. This function saves them in a tree storage.
	 * \argument config (tree_storage *) Storage class, where the data will be saved.
	 * \argument route (Route ) Route whos data shall be saved.
	 */
	void OutputMidiPort::Save (tree_storage & config, const RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		mutASSERT(route);
		
		config.toLeaf(_T("Midi Output"));
		config.Write(_T("Avoid Drum Channel"), route->ONoDrum);
		config.Write(_T("Channel Range From"), route->OFrom);
		config.Write(_T("Channel Range To"), route->OTo);
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}


/// Load current device settings from a tree storage
/** \argument config (tree_storage) storage class, where the data will be loaded from.
 */
	void OutputMidiPort::Load (tree_storage & config)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		DevId = config.Read(_T("Device Id"),0);
		Name = config.Read(_T("Device Name"), rtmidiout?(rtmidiout->getPortCount()?
				   muT(rtmidiout->getPortName(0).c_str()):wxString(_("Unknown"))):wxString(_("no device")));
		SetBendingRange(config.Read(_T("Bending Range"),DEFAULT_BENDING_RANGE));
		mutASSERT(oldpath == config.GetPath());
	}

/// Loade route settings (filter settings) for a given route
/** Some route settings (e.g. filter settings) are device type 
 * specific. This function loads them from a tree storage.
 * \argument config (tree_storage *) Storage class, where the data will be restored from.
 * \argument route (Route) Route whos data shall be loaded.
 */
	void OutputMidiPort::Load (tree_storage & config, RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		mutASSERT(route);
		config.toLeaf(_T("Midi Output"));
		route->ONoDrum = config.Read(_T("Avoid Drum Channel"), true);
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
			wxMessageBox(wxString::Format(_("The Channel range %d--%d of the MIDI output device %s must be inside %d--%d. The current route had to be corrected."),
						      oldfrom,oldto,GetName().c_str(),GetMinChannel(),GetMaxChannel()),
				     _("Warning loading route"),wxICON_EXCLAMATION);
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}



#if 0
	void OutputMidiPort::ReadData(wxConfigBase * config) 
	{
		midi.SetBendingRange(config->Read(_("Bending_Range"),
						  DEFAULT_BENDING_RANGE));
	}

	void OutputMidiPort::WriteData(wxConfigBase * config) 
	{
		config->Write(_("Bending_Range"), (long)bending_range);
	}
#endif

#ifdef WX
	wxString OutputMidiPort::TowxString() const {
		wxString channelString;
		for (int i = 0 ; i<16; i++) {
			channelString += wxString::Format(_T(" ({not implemented},%d,[%d,%d,%d,%d,%d])"),
							  ton_auf_kanal[i].active,
							  ton_auf_kanal[i].inkey,
							  ton_auf_kanal[i].outkey,
							  ton_auf_kanal[i].unique_id,
							  ton_auf_kanal[i].channel
				);
		}
		return OutputDeviceClass::TowxString()
			+ wxString::Format(_T("\
OutputMidiPort:\n\
   channels ({sound,sustain,MSB,LSB,pitch},KeyDir,[tasta,outkey,id]):\n\
             %s\n\
   nKeyOn   = %d\n\
"), (const wxChar *)channelString, nKeyOn);

	}
#endif

// InputMidiPort -------------------------------------------------------

#ifdef RTMIDI

	void mycallback( double deltatime, std::vector< unsigned char > *message, void *userData )
	{
		mutUnused(deltatime);
		DWORD data = 0;

		for (int i = message->size()-1; i >= 0; i--)
			data = ((data << 8) | ((unsigned char)(*message)[i]));

		((InputMidiPort*)userData)->Proceed(data);
	}

#else

	void CALLBACK _export MidiInPortFunc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
	{
		//  CurrentTimeStamp = dwParam2;

		if ( wMsg != MIM_DATA || dwParam1 % 256 > 240 ) return;

		// Daten in Midi-Automat
		((InputMidiPort*)dwInstance)->Proceed(dwParam1);
	}

#endif

// InputMidiPort -------------------------------------------------------
/// Save current device settings in a tree storage
/** \argument config (tree_storage) storage class, where the data will be saved.
 */
	void InputMidiPort::Save (tree_storage & config) 
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.Write(_T("Device Id"),   DevId);
		config.Write(_T("Device Name"), Name);
		mutASSERT(oldpath == config.GetPath());
	}

/// Save route settings (filter settings) for a given route
/** Some route settings (e.g. filter settings) are device type 
 * specific. This function saves them in a tree storage.
 * \argument config (tree_storage *) Storage class, where the data will be saved.
 * \argument route (Route ) Route whos data shall be saved.
 */
	void InputMidiPort::Save (tree_storage & config, const RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Midi Input"));
		config.Write(_T("Filter Type"), route->Type);
		switch(route->Type) {
		case RTchannel: 
			config.Write(_T("Channel From"), route->IFrom);
			config.Write(_T("Channel To"), route->ITo);
			break;
		case RTstaff:
			config.Write(_T("Key From"), route->IFrom);
			config.Write(_T("Key To"), route->ITo);
			break;
		case RTelse:
		case RTall:
			break;
		}
		config.toParent();
		mutASSERT(oldpath == config.GetPath());
	}


/// Load current device settings from a tree storage
/** \argument config (tree_storage) storage class, where the data will be loaded from.
 */
	void InputMidiPort::Load (tree_storage & config)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		DevId = config.Read(_T("Device Id"), 0);
		Name  = config.Read(_T("Device Name"), 
				    (rtmidiin?(	
				    rtmidiin->getPortCount()?
				    muT(rtmidiin->getPortName(0).c_str()):wxString(_("Unknown"))):wxString(_("no device"))));
		mutASSERT(oldpath == config.GetPath());
	}

/// Loade route settings (filter settings) for a given route
/** Some route settings (e.g. filter settings) are device type 
 * specific. This function loads them from a tree storage.
 * \argument config (tree_storage *) Storage class, where the data will be restored from.
 * \argument route (Route ) Route whos data shall be loaded.
 */
	void InputMidiPort::Load (tree_storage & config, RouteClass * route)
	{
#ifdef DEBUG
		wxString oldpath = config.GetPath();
#endif
		config.toLeaf(_T("Midi Input"));
		route->Type = (RouteType) config.Read(_T("Filter Type"), (int) RTchannel);
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
				wxMessageBox(wxString::Format(_("The Channel range %d--%d of the MIDI input device %s must be inside %d--%d. The current route had to be corrected."),
							      oldfrom,oldto,GetName().c_str(),GetMinChannel(),GetMaxChannel()),
					     _("Warning loading route"),wxICON_EXCLAMATION);
			break;
		}
		case RTstaff:
		{
			int oldfrom, oldto;
			route->IFrom = oldfrom = config.Read(_T("Key From"), GetMinKey());
			route->ITo = oldto = config.Read(_T("Key To"), GetMaxKey());
			bool correct = true;
			if (route->IFrom < GetMinKey()) {
				correct = false;
				route->IFrom = GetMinKey();
			}
			if (route->ITo > GetMaxKey()) {
				correct = false;
				route->ITo = GetMaxKey();
			}
			if (!correct)
				wxMessageBox(wxString::Format(_("The Channel range %d--%d of the MIDI input device must be inside %d--%d. The current route had to be corrected."),
							      oldfrom,oldto, GetName().c_str(),GetMinKey(), GetMaxKey()),
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

	bool InputMidiPort::Open()
	{
		mutASSERT(!isOpen);
		for (int i = 0; i < 16; i++)
			Cd[i].Reset();

#ifdef RTMIDI
		try {
			hMidiIn = new RtMidiIn(PACKAGE_STRING);
		} catch (RtError &error) {
			LAUFZEIT_ERROR0(_("Can not open Midi input devices."));
			return false;
		}

		try {
			hMidiIn->openPort(DevId,(const char *)(GetName().ToUTF8()));
		} catch (RtError &error) {
			LAUFZEIT_ERROR2(_("Can not open Midi input device no. %d (%s)."), DevId, (GetName().c_str()));
			return false;
		}

		hMidiIn->setCallback(mycallback, this);

#else
		midiInOpen(&hMidiIn, DevId, (DWORD)MidiInPortFunc, (DWORD)this, CALLBACK_FUNCTION);
		midiInStart(hMidiIn);
#endif
		isOpen = true;
		return true;
	}

	void InputMidiPort::Close()
	{
		mutASSERT(isOpen);
#ifdef RTMIDI
		hMidiIn->closePort();
		delete hMidiIn;
#else
		midiInStop(hMidiIn);
		midiInReset(hMidiIn);
		midiInClose(hMidiIn);
#endif
		Quite();
		isOpen = false;
	}




#ifdef WX
	wxString InputMidiPort::TowxString() const {
		wxString channelString;
		for (int i = 0; i < 16; i++) {
			channelString += wxString::Format(_T(" {not implemented}"));
		}
		return InputDeviceClass::TowxString()
			+wxString::Format(_T("\
InputMidiPort:\n\
   hMidiOut = %p\n\
   channels {sound,sustain,MSB,LSB,pitch}:\n\
             %s\n\
"), hMidiIn, (const wxChar *)channelString);

	}
#endif








/*#define MIDICODE(i)                           \
  (((BYTE*)(&midiCode))[i])
*/
// f¸r bestimmte Route Codeverarbeitung
	void InputMidiPort::ProceedRoute(DWORD midiCode, Route route)
	{
#ifdef DEBUG
		if (midiCode != 0xf8)
			DEBUGLOG(midiio,_T("midiCode = %x"), midiCode);
#endif
		int Box = route->GetBox();
		BYTE MidiChannel = midiCode & 0x0F;
		BYTE MidiStatus = midiCode & 0xF0;

		switch ( MidiStatus ) {

		case 0x90: // Note On
			if ( (midiCode & 0x7f0000) > 0 ) {
				if ( route->Active )
					AddKey(&mut_box[Box], (midiCode >> 8) & 0xff, 0, route->GetId(), NULL);

				if ( route->GetOutputDevice() )
					route->GetOutputDevice()
						->NoteOn(&mut_box[Box], 
							 (midiCode >> 8) & 0xff, 
							 (midiCode >> 16) & 0xff,
							 route.get(),
							 MidiChannel, 
							 Cd[MidiChannel]);

				break;
			}
			
		case 0x80: // Note Off
			if ( route->Active )
				DeleteKey(&mut_box[Box],(midiCode >> 8) & 0xff, 0, route->GetId());

			if ( route->GetOutputDevice() )
				route->GetOutputDevice()
					->NoteOff(&mut_box[Box], 
						  (midiCode >> 8) & 0xff, 
						  (midiCode >> 16) & 0xff, 
						  route.get(), 
						  MidiChannel,
						  false);

			break;

		case 0xC0: // Programm Change
			Cd[MidiChannel].program_change((midiCode >> 8) & 0xff);

			break;

		case CONTROLLER: // Control Change
			Cd[MidiChannel].set_controller((midiCode >> 8) & 0xff, (midiCode >> 16) & 0xff);
		case KEY_PRESSURE:
		case CHANNEL_PRESSURE: // Key Pressure, Controler, Channel Pressure
			//3 ??
			break;
		}

		// Midianalyse
		int lMidiCode[8] = {
			3, 3, 3, 3, 2, 2, 3, 1
		};

		if ( Box >= 0 && route->Active )
			for (int i = 0; i < lMidiCode[MidiStatus >> 5]; i++) {
				MidiAnalysis(&mut_box[Box],midiCode & 0xff);
				midiCode >>= 8;
			}
	}

// Routen testen und jenachdem entsprechend Codeverarbeitung
	void InputMidiPort::Proceed(DWORD midiCode)
	{
		char DidOut = 0;

		for (routeListType::iterator R = routes.begin(); 
		     R!= routes.end(); R++)
			switch ( (*R)->Type ) {

			case RTchannel:
				if ( (*R)->Check(midiCode & 0x0F) ) {
					ProceedRoute(midiCode, (*R));
					DidOut = 1;
				}

				break;

			case RTstaff:
				if ( ((midiCode & 0xF0) != 0x80 && 
				      (midiCode & 0xF0) != 0x90) 
				     || (*R)->Check((midiCode >> 8) & 0xFF) ) {
					ProceedRoute(midiCode, (*R));
					DidOut = 1;
				}

				break;

			case RTelse:
				if ( DidOut )
					break;

			case RTall:
				ProceedRoute(midiCode, *R);
			}

		FLUSH_UPDATE_UI;
	}

	MidiPortFactory::~MidiPortFactory() {}


		

	mutabor::OutputDeviceClass * MidiPortFactory::DoCreateOutput () const
	{
		OutputMidiPort * port = new OutputMidiPort();
		if (!port) throw DeviceNotCreated();
		return port;
	}

	mutabor::OutputDeviceClass * MidiPortFactory::DoCreateOutput(int devId,
							  const mutStringRef name, 
							  int id) const
	{
		OutputMidiPort * port = new OutputMidiPort(devId,name,id);
		if (!port) throw DeviceNotCreated();
		return port;
	}

	mutabor::OutputDeviceClass *  MidiPortFactory::DoCreateOutput (int devId,
							   const mutStringRef name, 
							   MutaborModeType mode, 
							   int id) const
	{
		OutputMidiPort * port = new OutputMidiPort(devId,name,id);
		if (!port) throw DeviceNotCreated();
		port->Device::SetId(id);
		switch (mode) {
		case DevicePause:
		case DeviceStop:
		case DevicePlay:
			port -> Open() ; 
			break;
		case DeviceUnregistered:
		case DeviceCompileError:
		case DeviceTimingError:
		default:
			UNREACHABLEC;
		}
		return port;
	}

	mutabor::InputDeviceClass * MidiPortFactory::DoCreateInput () const
		
	{
		InputMidiPort * port = new InputMidiPort();
		if (!port) throw DeviceNotCreated();
		return port;
	}

	mutabor::InputDeviceClass *  MidiPortFactory::DoCreateInput (int devId,
								const mutStringRef name, 
								int id) const
	{
		InputMidiPort * port = 
			new InputMidiPort(devId,name,
					  mutabor::DeviceStop,id);
		if (!port) throw DeviceNotCreated();
		return port;
	}

	mutabor::InputDeviceClass *  MidiPortFactory::DoCreateInput (int devId,
							 const mutStringRef name, 
							 MutaborModeType mode, 
							 int id) const
	{
 		InputMidiPort * port = new InputMidiPort(devId,name,mode,id);
		if (!port) throw DeviceNotCreated();
		return port;
	}



#ifdef RTMIDI
#include <string>

	using namespace std;
#include "RtMidi.h"
	RtMidiOut *rtmidiout;
	RtMidiIn *rtmidiin;
#endif

	void MidiInit()
	{
#ifdef RTMIDI

		try {
			rtmidiout = new RtMidiOut();
		} catch (RtError &error) {
			error.printMessage();
			// abort();
		}

		try {
			rtmidiin = new RtMidiIn();
		} catch (RtError &error) {
			error.printMessage();
			// abort();
		}

#endif
	}

	void MidiUninit()
	{
		delete rtmidiin;
		delete rtmidiout;
	}

}

///\}
