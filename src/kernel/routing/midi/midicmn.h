/** \file               -*- C++ -*-
 ********************************************************************
 * Common Midi devic base
 *
 * Copyright:   (c) 2012 Tobias Schlemmer
 * \author  Tobias Schlemmer <keinstein@users.berlios.de>
 * \date 
 * $Date: 2011/09/27 20:13:26 $
 * \version $Version$
 * \license GPL
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

#if (!defined(ROUTING_MIDICMN_H) && !defined(PRECOMPILE)) \
	|| (!defined(ROUTING_MIDICMN_H_PRECOMPILED))
#ifndef PRECOMPILE
#define ROUTING_MIDICMN_H
#endif

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "src/kernel/Defs.h"
#include "src/kernel/routing/Device.h"

#ifndef ROUTING_MIDICMN_H_PRECOMPILED
#define ROUTING_MIDICMN_H_PRECOMPILED

// system headers which do seldom change
#include <vector>

#define DEFAULT_BENDING_RANGE 2l		

namespace mutabor {


	template <class T=char, unsigned int count=16>
	class channel_queue_template : public std::vector<T> {
	public:
		typedef std::vector<T> basetype;
		typedef typename basetype::iterator iterator;
		typedef typename basetype::const_iterator const_iterator;


		/* longest used .. joungest used  .. longest free  .. joungest free .. longest sustained .. joungest sustained */

		channel_queue_template(T size = count):basetype(size),
						      first_free(basetype::begin()),
						      last_sustained(basetype::end()){
			init();
		}
		
		
		void init () {
			first_free = this->begin();
			last_sustained = this->end();
			
			for (T i = 0 ; i < this->size(); i++) {
				this->at(i) = i;
			}
		}
		
		iterator find_channel(T nr) {
			return std::find(this->begin(),this->end(),nr);
		}

		const_iterator find_channel(T nr) const {
			return std::find(this->begin(),this->end(),nr);
		}

		

		/** 
		 * Reserves the first free channel. 
		 * That is usually the longest unused one. 
		 *
		 * \note This can change if certain Channels are
		 * registered to be sustained. In that case they are
		 * postponed until no channels are availlable anymore.
		 *
		 * Sustaining is *not* handled by this class.
		 * 
		 * \return number of the first free channel
		 * \retval end() if no free or sustained channel can be found
		 */		
		template<class F>
		const_iterator reserve_channel_filtered (F filter) {
			iterator actual; 
			for (actual = first_free; 
			     filter((int)(*actual)) && actual != this->end();
			     actual++);
			return reserve_channel (actual);
		}

		iterator reserve_channel(iterator actual) {
			if (actual != this->end()) {
				if (first_free >= last_sustained) {
					last_sustained++;
				}
				if (first_free <= actual) {
					move_to_front(first_free,actual);
					actual = first_free;
					first_free ++ ;
				} else {
					// we are rearranging used channels 
					// thus first_free doesn't change
					move_to_end(actual, first_free);
					actual = first_free - 1;
				} 
			}
			return actual;
		}

		/** 
		 * Forcibly reserve given channel regardless of its state.
		 * This function is used if there are no free channels left 
		 * and the system frees a channel depending on semantic data.
		 * 
		 * \param nr Channel to be reserved
		 * 
		 * \return iterator to the channel.
		 */
		iterator reserve_channel(T nr) {
			return reserve_channel(find_channel(nr));
		}

		/** 
		 * Find and free a channel.
		 * \note the channel must not be freed already. 
		 * 
		 * \param nr Channel to bee freed.
		 */
		void free_channel (T nr) {
			free_channel(find_channel(nr));
		}

		/** 
		 * Mark a channel to be free.
		 * 
		 * \param current iterator pointing to the channel to be regarded as free.
		 */
		void free_channel (iterator current) {
			if (current == this->end()) {
				UNREACHABLEC;
				return;
			}
			if (last_sustained <= current) {
				move_to_front(current,last_sustained);
				last_sustained ++;
				return;
			}
			if (first_free <= current) {
				UNREACHABLEC;
				return;
			}
			move_to_end(current,last_sustained);
			first_free --;
		}

		/** 
		 * Find and sustain a channel.
		 * \note the channel must not be sustaind already. 
		 * 
		 * \param nr Channel to bee sustaind.
		 */
		void sustain_channel (T nr, bool sustain = true) {
			if (sustain) 
				sustain_channel(find_channel(nr));
			else unsustain_channel(find_channel(nr));
		}

		/** 
		 * Mark a channel to be sustained.
		 * 
		 * \param current iterator pointing to the channel to be regarded as sustained.
		 */
		 iterator sustain_channel (iterator current) {			
			if (first_free <= current) {
				// includes last_sustained <= current
				UNREACHABLEC;
				return this->end();
			}
			move_to_end(current,this->end());
			last_sustained--;
			if (current < first_free) first_free --;
			return this->end() - 1;
		}
		/** 
		 * Unmark channel to be sustained;
		 * 
		 * \param current iterator pointing to the channel to be regarded as sustained.
		 */
		void unsustain_channel (iterator current) {			
			free_channel(current);
		}

		iterator  get_first_free() { return first_free; }
		iterator  get_last_sustained() { return last_sustained; }

	protected:
		iterator first_free;
		iterator last_sustained;
		
		/** 
		 * Move current pointer to the end of a range.
		 * 
		 * \param current current position
		 * \param end position after the end position of the element.
		 */
		void move_to_end (iterator current, iterator end) {
			if (current == this->end()) {
				UNREACHABLEC;
				return;
			}
			std::rotate(current, current+1, end);
		}

		/** 
		 * Move current pointer to the beginning of a range.
		 * 
		 * \param current current position
		 * \param final position where the element should be moved to
		 */
		void move_to_front (iterator current, iterator final) {
			if (current == this->end()) {
				UNREACHABLEC;
				return;
			}
			std::rotate(final, current, current+1);
		}
		
	};
	typedef channel_queue_template<unsigned char,16> channel_queue_type;

	class pitch_bend_type  {
	public:
		int pitch;
		int bend;
		
		pitch_bend_type & operator -= (const pitch_bend_type & other) {
			pitch -= other.pitch;
			bend -= other.bend;
		}

		bool operator == (pitch_bend_type & other) {
			return pitch == other.pitch && bend == other.bend;
		}
	};

	typedef struct TAK
	{
		bool active;      //< is this tone active or free?
		int inkey;        //< internal key id. provided by input device
		int velocity;     //< attack velocity or note volume
		pitch_bend_type outkey;   
		int channel;      //< route that broght the note to this device
		int midi_channel; //< MIDI channel (unsused)
		size_t unique_id; //< unique id defined by input device
                long tuned_key;   //< tuned key
	} TonAufKanal;

      
	
	
	/** 
	 * A simple MIDI output provider describing the API.
	 * 
	 */
	class DebugMidiOutputProvider {
	public:
		DebugMidiOutputProvider() {}

		/** 
		 * Outputs a three-byte message.
		 * 
		 * \param channel channel to which data shall be sent
		 *        to. How channels are split into tracks or
		 *        subdevices is managed by the OutputProvider 
		 * \param byte1 1st byte 
		 * \param byte2 2nd byte 
		 * \param byte3 3rd byte
		 */
		DebugMidiOutputProvider & operator() (int channel,
						      uint8_t byte1,
						      uint8_t byte2,
						      uint8_t byte3) 
			{
				DEBUGLOG(midiio,_T("MIDI OUT to %02x: %02x %02x %02x"), channel, byte1,byte2,byte3);
				return *this;
			}

		/** 
		 * Outputs a two-byte message.
		 * 
		 * \param channel channel to which data shall be sent
		 *        to. How channels are split into tracks or
		 *        subdevices is managed by the OutputProvider 
		 * \param byte1 1st byte
		 * \param byte2 2nd byte
		 */
		DebugMidiOutputProvider & operator() (int channel,
						      uint8_t byte1,
						      uint8_t byte2)
			{
				DEBUGLOG(midiio,_T("MIDI OUT to %02x: %02x %02x"), channel, byte1, byte2);
				return * this;
			}

		/** 
		 * Outputs a System Exclusive Message.
		 * 
		 * \param channel channel to which data shall be sent
		 *        to. How channels are split into tracks or
		 *        subdevices is managed by the OutputProvider 
		 * \param data vector containing data to be sent.
		 */
		void MidiOutSysEx(unsigned int channel,
				  std::vector<uint8_t> data) {
		}
	};

	
	template<class T, class D>
	class CommonMidiOutput:public D {
	public:
		class TooHighPitchBend {};
		class TooSmallPitchBend {};

		typedef T midiprovider;
		typedef D base;

		enum bank_type {
			lsb_first = 0,
			lsb_only,
			msb_only,
			msb_first
		};

		bool Open();
		void Close();
		channel_queue_type::const_iterator EmergencyFindChannel(RouteClass * r);
		void UpdateControllers(int channel, const ChannelData & input_channel_data);

		/** 
		 * Copies the program change and bank data from the input channel to the output
		 * choannel and issues the corresponding bank change and prgoram change messages.
		 * 
		 * \param input_channel_data input to be copied from
		 * \param output_data output to be copied to
		 * \param channel MIDI channel to output the data to
		 */
		void CopyProgramChange(const ChannelData & input_channel_data,
				       ChannelData & output_data,
				       int channel);
		void NoteOn(mutabor_box_type * box, 
			    int inkey, 
			    int velocity, 
			    RouteClass * r, 
			    size_t id, 
			    const ChannelData & input_channel_data);

		void NoteOff(mutabor_box_type * box, 
			     int inkey, 
			     int velo, 
			     RouteClass * r, 
			     size_t id,
			     bool is_note_on /* = false */
			);	
		void NotesCorrect(RouteClass * route);
		void Sustain(int channel, const ChannelData & cd);
		int GetChannel(int inkey, int channel);
		void SplitOut (BYTE * p, size_t n);
		void Quiet(RouteClass * r);
		void Panic();
		void MidiOut(BYTE *p, size_t n) {
			SplitOut(p,n);
		}



//		midiprovider & GetProvider () { return Out(); }
		void SetBendingRange(int br) { bending_range = br; }
		int GetBendingRange() const { return bending_range; }
						   
		virtual int GetMaxChannel() const = 0;
		virtual int GetMinChannel() const = 0;

	protected:
		CommonMidiOutput(int b):bending_range(b),
					channel_queue(),
					nKeyOn(0) {}
		
		/** 
		 * Sends a pitchbend signal.
		 * 
		 * \param channel Channel on which the data shall be sent.
		 * \param value   Data to be sent range has already been honoured Center is at 0.
		 */
		void pitch_bend(int channel, int value) {
			const int pitch_bend_border = 0x40 << 7;
			if (value >= pitch_bend_border)
				throw TooHighPitchBend();
			if (value < -(pitch_bend_border))
				throw TooHighPitchBend();

			int pb = value + pitch_bend_border;
			DEBUGLOG2(midiio,_T("MIDI_PITCH(%x/%d,%x/%d) = %x/%d (%x/%d, %x/%d, %x/%d)"), 
				  channel,channel,
				  value,value,
				  pb,pb,
				  (int)midi::PITCH_BEND + channel,(int)midi::PITCH_BEND + channel,
				  pb & 0x7f, pb & 0x7f,
				  (0x40 + (pb >> 7) ) & 0x7f, 
				  (0x40 + (pb >> 7) ) & 0x7f );
			Out (channel, midi::PITCH_BEND, 
			     pb & 0x7f, (pb >> 7) & 0x7f);
		}

		void program_change(int channel, int sound, int lsb, int msb) {
			DEBUGLOG(midiio,_T("ch: %d, pr: %d, lsb: %d, msb: %d, send mode: %d"), channel, sound, lsb, msb, bank_mode);
			/* if only one bank byte has been set we correct it in case we send
			   msb or lsb only. Otherwise we do not have access to the internal state
			   of the devie and leave it alone with the setting the user provided */
			if (lsb != -1) {
				if (msb != -1) {
					switch (bank_mode) {
					case lsb_first:
						controller(channel,midi::BANK_FINE,lsb);
					case msb_only:
						controller(channel,midi::BANK_COARSE,msb);
						break;
					case msb_first:
						controller(channel,midi::BANK_COARSE,msb);
					case lsb_only:
						controller(channel,midi::BANK_FINE,lsb);
						break;
					default:
						UNREACHABLEC;
					}
				} else {
					controller(channel,(bank_mode == msb_only?
							    midi::BANK_COARSE:
							    midi::BANK_FINE),lsb);
				}
			} else {
				controller(channel,(bank_mode == lsb_only?
						    midi::BANK_FINE:
						    midi::BANK_COARSE),lsb);
			}

			if ( sound != -1 ) 
				Out(channel,midi::PROGRAM_CHANGE | (channel & 0x0f), sound);
		}

		void controller(int channel, int controller, int value) {
			DEBUGLOG(midiio,_T("ch: %d, ctrl: %d, value: %d"),channel, controller, value);
			Out(channel,midi::CONTROLLER, controller, value);
		}

		void note_on(int channel, int pitch, int velocity) {
			Out(channel,midi::NOTE_ON, pitch, velocity);
		}

		void note_off(int channel, int pitch, int velocity) {
			Out(channel,midi::NOTE_OFF, pitch, velocity);
		}



		
		long fix_bend(long bend) {
			long retval;
			if (bend < 0) {
				retval = bend / bending_range;
			} else {
				mutint64 b =  bend * (mutint64)0x1fff;
				retval = b / (0x2000 * bending_range);

			}
			
			/* A naive approach assumes that pitch bend ranges 
			   from -0x2000 to 0x2000. Actually the range lasts 
			   from -0x2000 to 0x1fff.

			   The difference is one step or
			   (pitch_bend_range / 0x1fff). For 12HT pitch bend range 
			   this means about 0.3ct.

			   Note: The auditory system has about 4000 receptors, 
			   which proviedes an average distance of 3ct. But they 
			   are not equally distributed and the amplification 
			   is not linear, which makes difference tones to be real, 
			   detectable frequencies.

			   In absense of the standard (it's at work) I
			   implemented the complicate correct formula above.
			*/
			
			// must solve the equation 0x 80 00 00 / divisor + 0x 20 00 = 0x 40 00
			return retval / 0x400; //we can't use >> because of negative values
		}

		pitch_bend_type pitch_and_bend(long tuned_note) {
			pitch_bend_type data;
			data.bend = tuned_note & 0xffffff;
			data.pitch = tuned_note >> 24;
			if (data.bend >=0x800000) {
				data.pitch++;
				data.bend -= 0x1000000;
			}


			// must solve the equation 0x 80 00 00 / divisor + 0x 40 00 = 0x 1 00 00
			data.bend = fix_bend(data.bend); //we can't use >> because of negative values

			return  data;
		}

		pitch_bend_type pitch_and_bend(long tuned_note, const pitch_bend_type & old) {
			long interval_centre = ((long)old.pitch) << 24;
			long delta = tuned_note - interval_centre;
			long interval_range = ((long)bending_range) << 24;

			if (delta > interval_range || delta < -interval_range)
				return pitch_and_bend(tuned_note);

			pitch_bend_type data;
			data.pitch = old.pitch;
			data.bend = fix_bend(delta);

			return  data;
		}



			

	protected:
		midiprovider Out;
		int bending_range;
		bank_type bank_mode;
		ChannelData Cd[16];
		/*  replace KeyData by channel_queue */
		channel_queue_type channel_queue;
		TonAufKanal ton_auf_kanal[16];
		int nKeyOn;

		CommonMidiOutput():base(), 
			bending_range (2),
			bank_mode(lsb_first),
			nKeyOn(0) { }

		CommonMidiOutput(int devId, 
				 wxString name, 
				 int id = -1, 
				 int bendingRange = 2)
			: base(devId, name, id),
			bending_range (bendingRange), 
			bank_mode(lsb_first),
			nKeyOn(0)
			{
				DEBUGLOG(other,_T(""));
			}
	};

	template<class T>
	class CommonMidiInput:InputDeviceClass {
	public:
		typedef T midiprovider;

		

	protected:
		midiprovider provider;
		
	};

}




#endif // precompiled
#endif // header loaded


///\}
