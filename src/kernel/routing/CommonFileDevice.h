/** \file  -*- C++ -*-
 ********************************************************************
 * File player class which solves common tasks of file related devices
 * such as MIDI file players.
 *
 * Copyright:   (c) 2012 Tobias Schlemmer
 * \author  Tobias Schlemmer <keinstein@users.berlios.de>
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
/* we guard a little bit complicated to ensure the references are set right
 */

#if (!defined(MU32_ROUTING_MIDI_COMMONFILEDEVICE_H) && !defined(PRECOMPILE)) \
	|| (!defined(COMMONFILEDEVICE_PRECOMPILED))
#ifndef PRECOMPILE
#define MU32_ROUTING_MIDI_COMMONFILEDEVICE_H
#endif

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "src/kernel/Defs.h"
#include "src/kernel/routing/Device.h"
#include "src/kernel/routing/thread.h"


#ifndef MU32_ROUTING_MIDI_COMMONFILEDEVICE_H_PRECOMPILED
#define MU32_ROUTING_MIDI_COMMONFILEDEVICE_H_PRECOMPILED

// system headers which do seldom change

#include <limits>
#include <cstdio>
#include "wx/thread.h"

namespace mutabor {

	// OutMidiFile ------------------------------------------------------

	class CommonFileDeviceFactory;
	class CommonFileOutputDevice : public OutputDeviceClass
	{
		friend class CommonFileDeviceFactory;
	protected:
		CommonFileOutputDevice(): OutputDeviceClass() {}

		CommonFileOutputDevice(const mutStringRef name, 
				       int id = -1)
			: OutputDeviceClass(name, id) { }
 	public:
		virtual ~CommonFileOutputDevice() {};
	
		/// Save current device settings in a tree storage
		/** \argument config (tree_storage) storage class, where the data will be saved.
		 */
		virtual void Save (tree_storage & config);
	
	
	
		/// Load current device settings from a tree storage
		/** \argument config (tree_storage) storage class, where the data will be loaded from.
		 */
		virtual void Load (tree_storage & config);
	
		virtual bool do_Open();
		virtual void Close();

		virtual void SetName(const wxString & s) 
			{
				if (s != Name) {
					bool reopen = IsOpen();
					if (reopen) 
						Close();

					Name = s;

					if (reopen)
						Open();
				}
			}
		

		virtual mutString GetTypeName () const {
			mutASSERT(false);
			return N_("Generic output file");
		}

#ifdef WX
		virtual wxString TowxString() const;
#endif
	};




// CommonFileInputDevice -------------------------------------------------------


	class CommonFileInputDevice : public InputDeviceClass
	{
		friend class MidiFileFactory;
	public:
 		class FileTimer: public Thread
		{
	      
			boost::intrusive_ptr<CommonFileInputDevice> file;
		public:
			FileTimer(CommonFileInputDevice * f,
				  ThreadKind kind = wxTHREAD_DETACHED) : wxThread(kind),
									 file(f)
			{}
			
			virtual ~FileTimer() {
				mutASSERT(!file);
				if (file) {
					if (file->timer == this)
						file -> timer = NULL;
				}
				std::fprintf(stderr,"exiting thread");
			}

			ExitCode Entry() {
				ExitCode e;
				try {
					e = file->ThreadPlay(this);
				} catch (...) {
					e =  file->exception_error();
				}
				return e;
			}


			void OnExit() {
				mutASSERT(!IsDetached() || !file);
				if (file) {
					if (file->timer == this)
						file -> timer = NULL;
				}
				file.reset();
			}
			
			void ClearFile() {
				file.reset();
			}

			bool HasFile(CommonFileInputDevice * f) {
				return file == f;
			}
		protected:
		};

		friend class FileTimer;

	public:
		virtual ~CommonFileInputDevice() {
			if (isOpen) Close();
			mutASSERT(timer == NULL);
		};
	
		/// Save current device settings in a tree storage
		/** \argument config (tree_storage) storage class, where the data will be saved.
		 */
		virtual void Save (tree_storage & config);
	
		/// Load current device settings from a tree storage
		/** \argument config (tree_storage) storage class, where the data will be loaded from.
		 */
		virtual void Load (tree_storage & config);

	
		virtual bool Open();
		virtual void Close();
		virtual void Stop();
		virtual void Play();
		virtual void Pause();

		virtual void SetName(const wxString & s) {
			if (s != Name) {
				bool reopen = IsOpen();
				if (reopen) 
					Close();

				Name = s;

				if (reopen) 
					Open();
			}
		}

		/** 
		 * Sets the thread kind for the next thread that is
		 * being generated. If we have already a timer thread
		 * then the thread kind will not be changed, but the 
		 * value is stored for the next generated thread.
		 * 
		 * \param k thread kind to be saved
		 * 
		 * \retval true if everything is ok.  
		 * \retval false if there is a thread that
		 * could not be updated.
		 */
		virtual bool SetThreadKind(wxThreadKind k) {
			threadkind = k;
			return !timer;
		}

		/** 
		 * Issue an error message and stop the device.
		 * 
		 * 
		 * \return Error exit code.
		 */
		wxThread::ExitCode exception_error();

		/** 
		 * Play the file.
		 * This function is called from the
		 * thread object. It plays the file using sleep
		 * operations.
		 */
		wxThread::ExitCode ThreadPlay(FileTimer * timer);
		wxThread::ExitCode WaitForDeviceFinish() {
			mutASSERT(timer);
			if (timer) {
				mutASSERT(wxThread::This() != timer);
				if (wxThread::This() != timer) {
					DEBUGLOG(thread,
						 _T("Thread %p locking at threadsignal = %x"),
						 Thread::This(),
						 threadsignal.get());
					ScopedLock lock(waitMutex);
					DEBUGLOG(thread,
						 _T("Thread %p locked at threadsignal = %x"),
						 Thread::This(),
						 threadsignal.get());

					if (timer->IsDetached()) {
						switch (Mode) {
						case DevicePlay:
						case DevicePause: 
						case DeviceStop:
							return 0;
						case DeviceTimingError:
						case DeviceCompileError:
						default:
							return (void *)Mode;
							
						}
					}
				} else return timer;
			}
			return 0;
	}

		virtual mutString GetTypeName () const {
			return N_("Generic input file");
		}
	
#ifdef WX
		virtual wxString TowxString() const;
#endif
		
	protected:
		FileTimer * timer;             //< timer thread for the file player
		/// Signals to communicate with the player thread
		enum ThreadCommunication {
			Nothing       = 0,     //< proceed, no change
			RequestExit   = 1,     //< exit the thread as soon as possible
			RequestPause  = 2,     //< stop the thread execution as soon as possible
			ResetTime     = 4,      /**< restart counting
						the time of the
						thread. This should
						be used only to sigal
						a start after the
						playback has been
						stopped */
			RequestPanic = 8       //< request Panic() before stopping the device
		};
		/* volatile is handled inside the class */
		safe_integer<int> threadsignal; //< signal 
		Mutex waitMutex, threadReady, lockMode, exitLock;
		ThreadCondition waitCondition;
		/** 
		 * Fixed offset for the relative time the file returns.
		 */
		volatile mutint64 referenceTime; // ms
		volatile mutint64 pauseTime;     // ms
		timing_params timing;
		wxThreadKind threadkind;

		CommonFileInputDevice(): InputDeviceClass(),
					 timer (NULL),
					 threadsignal (Nothing),
					 waitMutex(),
					 waitCondition(waitMutex),
					 referenceTime(0),
					 pauseTime(0),
					 threadkind(wxTHREAD_DETACHED) { }

		CommonFileInputDevice(wxString name, 
				      MutaborModeType mode,
				      int id): InputDeviceClass(name, 
								mode, 
								id),
					       timer(NULL),
					       threadsignal (Nothing),
					       waitMutex(),
					       waitCondition(waitMutex),
					       referenceTime(0),
					       pauseTime(0),
					       threadkind(wxTHREAD_DETACHED) {}

	};


	class CommonFileDeviceFactory:public DeviceFactory { 
	public:
		CommonFileDeviceFactory(size_t id = 0):
			DeviceFactory(id) {}
		virtual ~CommonFileDeviceFactory();

	protected:
		virtual size_t GetType() const
			{
				return DTMidiFile;
			}


		virtual mutabor::OutputDeviceClass * DoCreateOutput() const = 0;
		
		virtual mutabor::InputDeviceClass * DoCreateInput() const = 0;
		virtual mutabor::OutputDeviceClass * DoCreateOutput(int devId,
							     const mutStringRef name, 
							     int id = -1) const = 0;
		
		virtual mutabor::InputDeviceClass * DoCreateInput(int devId,
							   const mutStringRef name, 
							   int id = -1) const = 0;

		virtual mutabor::OutputDeviceClass * DoCreateOutput(int devId,
							     const mutStringRef name, 
							     mutabor::MutaborModeType mode, 
							     int id = -1) const = 0;
		
		virtual mutabor::InputDeviceClass * DoCreateInput(int devId,
							   const mutStringRef name, 
							   mutabor::MutaborModeType mode, 
							   int id = -1) const = 0;
	};


// Function ---------------------------------------------------------

}

#endif  /* precompiled */
#endif


///\}
