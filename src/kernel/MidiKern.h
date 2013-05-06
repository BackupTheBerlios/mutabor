/** \file 
 ********************************************************************
 * Some MIDI macros
 *
 * Copyright:   (c) 1998-2011 TU Dresden
 * \author  R.Krauße
 * Tobias Schlemmer <keinstein@users.berlios.de>
 * \date 
 * $Date: 2011/09/27 20:13:21 $
 * \version $Revision: 1.6 $
 * \license GPL
 *
 ********************************************************************
 * \addtogroup templates
 * \{
 ********************************************************************/
// ------------------------------------------------------------------
// ------------------------------------------------------------------

/* we guard a little bit complicated to ensure the references are set right
 */

#if (!defined(MU32_MIDIKERN_H) && !defined(PRECOMPILE)) \
	|| (!defined(MU32_MIDIKERN_H_PRECOMPILED))
#ifndef PRECOMPILE
#define MU32_MIDIKERN_H
#endif

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "Defs.h"
#include "box.h"

#ifndef MU32_MIDIKERN_H_PRECOMPILED
#define MU32_MIDIKERN_H_PRECOMPILED

// system headers which do seldom change


#define DRUMCHANNEL 9  // drum channel in general MIDI (0 based 9, 10 else).

#if 0
#define ZWZ pow(2.0l,1.0l/12.0l)// 12 √ 2
#endif

#define LONG_TO_HERTZ( freq ) (440.0*pow(2.0l,((double)(freq-69*0x01000000) / ((double) (0x01000000*12)))))
/* 440 * (12 √ 2) ^ (pitch difference to “a440” in half tones)  */

/** Despite its name the following macro translates a long value into halftones.
 *  Cents are easy to deduce: they are the first two decimals.
 */
#define LONG_TO_CENT( freq ) ( (double)(freq) / ((double) 0x01000000))

#ifdef __cplusplus
namespace mutabor {
	namespace midi {

		enum midi_command {
			NOTE_OFF                = 0x80,
			NOTE_ON                 = 0x90,
			AFTER_TOUCH             = 0xA0,
			KEY_PRESSURE            = 0xA0,
			CONTROLLER              = 0xB0,
			PROGRAM_CHANGE          = 0xC0,
			CHANNEL_PRESSURE        = 0xD0,
			PITCH_BEND              = 0xE0,
			SYSTEM                  = 0xF0,
			/* system common */
			SYSEX_START             = 0xF0,
			QUARTER_FRAME           = 0xF1,
			SONG_POSITION           = 0xF2,
			SONG_SELECT             = 0xF3,
			TUNE_REQUEST            = 0xF6,
			SYSEX_END               = 0xF7,
			/* system realtime */
			CLOCK                   = 0xF8,
			TICK                    = 0xF9,
			START_PLAY              = 0xFA,
			CONTINUE_PLAY           = 0xFB,
			STOP_PLAY               = 0xFC,
			ACTIVE_SENSE            = 0xFE,
			RESET                   = 0xFF,
			META                    = 0xFF,

			/* message group masks */
			STARTBYTE_MASK          = 0x80,
			TYPE_MASK               = 0xF0,
			CHANNEL_MASK            = 0x0F
		};

		enum midi_meta_types {
			META_SEQUENCE_NUMBER   = 0x00,
			META_EVENT_TEXT        = 0x01,
			META_COPYRIGHT_NOTICE  = 0x02,
			META_SEQUENCE_NAME     = 0x03,
			META_TRACK_NAME        = 0x03,
			META_INSTRUMENT_NAME   = 0x04,
			META_LYRIC             = 0x05,
			META_MARKER            = 0x06,
			META_CUE_POINT         = 0x07,
			META_CHANNEL_PREFIX    = 0x20,
			META_END_OF_TRACK      = 0x2F,
			META_SET_TEMPO         = 0x51,
			META_SMPTE_OFFSET      = 0x54,
			META_TIME_SIGNATURE    = 0x58,
			META_KEY_SIGNATURE     = 0x59,
			META_SEQUENCER_SPECIFIC = 0x7F
		};

		enum midi_controller_indices {
			/* coarse controllers */
			BANK_COARSE              = 0x00,
			MODULATION_WHEEL_COARSE  = 0x01,
			BREATH_COARSE            = 0x02,
			// free controller 0x03
			FOOT_PEDAL_COARSE        = 0x04,
			PORTAMENTO_TIME_COARSE   = 0x05,
			DATA_ENTRY_COARSE        = 0x06,
			VOLUME_COARSE            = 0x07,
			BALANCE_COARSE           = 0x08,
			// free controller 0x09
			PAN_POSITION_COARSE      = 0x0A,
			EXPRESSION_COARSE        = 0x0B,
			EFFECT_CONTROL_1_COARSE  = 0x0C,
			EFFECT_CONTROL_2_COARSE  = 0x0D,

			// free controllers 0x0E, 0x0F

			/* general purpose controllers */
			GENERAL_PURPOSE_SLIDER_1 = 0x10,
			GENERAL_PURPOSE_SLIDER_2 = 0x11,
			GENERAL_PURPOSE_SLIDER_3 = 0x12,
			GENERAL_PURPOSE_SLIDER_4 = 0x13,

			// free controllers 0x14-0x1F
	
			/* fine controllers */
			BANK_FINE                = 0x20,
			MODULATION_WHEEL_FINE    = 0x21,
			BREATH_FINE              = 0x22,
			// free controller 3
			FOOT_PEDAL_FINE          = 0x24,
			PORTAMENTO_TIME_FINE     = 0x25,
			DATA_ENTRY_FINE          = 0x26,
			VOLUME_FINE              = 0x27,
			BALANCE_FINE             = 0x28,
			// free controller 9
			PAN_POSITION_FINE        = 0x2A,
			EXPRESSION_FINE          = 0x2B,
			EFFECT_CONTROL_1_FINE    = 0x2C,
			EFFECT_CONTROL_2_FINE    = 0x2D,

			// free 0x2E–0x3F

			/* on/off controllers */
			HOLD_PEDAL_ON_OFF        = 0x40,
			PORTAMENTO_ON_OFF        = 0x41,
			SOSTENUTO_ON_OFF         = 0x42,
			SOFT_PEDAL_ON_OFF        = 0x43,
			LEGATO_PEDAL_ON_OFF      = 0x44,
			HOLD_2_PEDAL_ON_OFF      = 0x45,

	
			/* Sound controllers */
			SOUND_VARIATION          = 0x46,
			SOUND_TIMBRE             = 0x47,
			SOUND_RELEASE_TIME       = 0x48,
			SOUND_ATTACK_TIME        = 0x49,
			SOUND_BRIGHTNESS         = 0x4A,
			SOUND_6                  = 0x4B,
			SOUND_7                  = 0x4C,
			SOUND_8                  = 0x4D,
			SOUND_9                  = 0x4E,
			SOUND_10                 = 0x4F,

			/* general purpose on/off */
			GENERAL_PURPOSE_ON_OFF_1 = 0x50,
			GENERAL_PURPOSE_ON_OFF_2 = 0x51,
			GENERAL_PURPOSE_ON_OFF_3 = 0x52,
			GENERAL_PURPOSE_ON_OFF_4 = 0x53,

			// free 0x54-0x57
	
			/* levels */
			EFFECTS_LEVEL            = 0x5B,
			TREMULO_LEVEL            = 0x5C,
			CHORUS_LEVEL             = 0x5D,
			CELESTE_LEVEL            = 0x5E,
			PHASER_LEVEL             = 0x5F,

			/* data handling */
			DATA_BUTTON_INCREMENT    = 0x60,
			DATA_BUTTON_DECREMENT    = 0x61,
			NON_REGISTERED_PARAMETER_FINE = 0x62,
			NON_REGISTERED_PARAMETER_COARSE = 0x63,
			REGISTERED_PARAMETER_FINE = 0x64,
			REGISTERED_PARAMETER_COARSE = 0x65,

			// free 0x66-0x77

			/* management controllers */
			ALL_SOUND_OFF            = 0x78,
			ALL_CONTROLLERS_OFF      = 0x79,
			LOCAL_ON_OFF             = 0x7A, // (122)
			ALL_NOTES_OFF            = 0x7B,
			OMNI_OFF                 = 0x7C,
			OMNI_ON                  = 0x7D, // (125)
			// mono means not poly and vice versa
			MONO_ON                  = 0x7E, // 126
			POLY_OFF                 = 0x7E, // 126
			MONO_OFF                 = 0x7F, // (127)
			POLY_ON                  = 0x7F // (127)
		};


		enum midi_controller_values {
			CONTROLLER_ON           = 0xFF,
			CONTROLLER_OFF          = 0X00
		};

		enum midi_registered_parameters {
			// Form: 0x1MMLL
			PITCH_BEND_SENSITIVITY = 0x10000,
			CHANNEL_FINE_TUNING    = 0x10001,
			CHANNEL_COARSE_TUNING  = 0x10002,
			MODULATION_DEPTH_RANGE = 0x10005,
			RPN_NULL               = 0x17F7F
		};

		inline int get_data_size(uint8_t status_byte) {
			static const int midi_size[]   = { 3, 3, 3, 3, 2, 2, 3,-1 };
			static const int system_size[] = {-1, 2, 3, 2, 1, 1, 1, -1, 
							  1, 1, 1, 1, 1, 1, 1, -1 };
			
			if (!(status_byte & 0x80)) return -1;
			int size = midi_size [(status_byte >> 4) & 0x7];
			return size == -1 ? system_size[status_byte & 0x0F]:size;
		}

		inline bool is_system_exclusive(uint8_t status) {
			return status == SYSEX_START;
		}

		inline bool is_system_common(uint8_t status) {
			return ( status == QUARTER_FRAME) || 
				( status == SONG_POSITION) ||
				( status == SONG_SELECT) ||
				( status == TUNE_REQUEST) ||
				( status == SYSEX_END);
		}

		inline bool is_system_realtime(uint8_t status) {
			return (0xF8 <= status) && (status <= 0xFF);
		}
		
		inline bool is_channel_message(uint8_t status) {
			return (status & 0x80) && (((status >> 4) & 0x7) != 0x7);
		}

		inline bool reset_running_status(uint8_t status) {
			return is_system_exclusive(status) || is_system_common(status);
		}

		inline bool store_running_status(uint8_t status) {
			return is_channel_message(status);
		}


	}
}

#endif

#endif /* precompiled */
#endif



///\}
