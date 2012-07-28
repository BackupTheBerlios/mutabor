/** \file
 ********************************************************************
 * Fehlermeldungen.
 *
 * $Header: /home/tobias/macbookbackup/Entwicklung/mutabor/cvs-backup/mutabor/mutabor/includes/mutabor/errors.h,v 1.2 2005/11/03 14:30:49 keinstein Exp $
 * \author Tobias Schlemmer <keinstein_junior@gmx.net>
 * \date $Date: 2005/11/03 14:30:49 $
 * \version $Revision: 1.2 $
 *
 * $Log: errors.h,v $
 * Revision 1.2  2005/11/03 14:30:49  keinstein
 * whitespace corrections
 *
 * Revision 1.1  2005/07/20 12:37:21  keinstein
 * Fehlerkonstanten
 *
 ********************************************************************/

#ifndef __ERRORS_H_INCLUDED
#define __ERRORS_H_INCLUDED

#ifdef __cplusplus
namespace mutabor {
  extern "C" {
#endif

    extern int mutabor_debug_level; /**< Debuglevel für die Bibliothek */
    extern char * Error_text[]; /**< Liste der Warnungen */
    extern char * Warning_text[]; /**< Liste der Warnungen */



#define MUTABOR_ERROR_UNDEFINED           0
#define MUTABOR_ERROR_SYNTAX_COMMON       1
#define MUTABOR_ERROR_INVALID_CHARACTOR   2
#define MUTABOR_ERROR_FILE_OPEN           3
#define MUTABOR_ERROR_MEMORY_LOW          4
#define MUTABOR_ERROR_UNFINISHED_COMMENT  5
#define MUTABOR_ERROR_UNREACHABLE_CODE    6



#define MUTABOR_ERROR_DOUBLE_INTERVAL     10
#define MUTABOR_ERROR_DOUBLE_TONE         11
#define MUTABOR_ERROR_DOUBLE_TONE_SYSTEM  12
#define MUTABOR_ERROR_DOUBLE_RETUNING     13
#define MUTABOR_ERROR_DOUBLE_HARMONY      14
#define MUTABOR_ERROR_DOUBLE_LOGIK        15

#define MUTABOR_ERROR_DOUBLE_PARAMETER    17
#define MUTABOR_ERROR_DOUBLE_MIDI_INSTRUMENT 18
#define MUTABOR_ERROR_OVERLAPPING_MIDI_RANGE 19
#define MUTABOR_ERROR_MULTIPLE_DEFAULT_TRIGGER 20
#define MUTABOR_ERROR_MULTIPLE_DEFAULT_RETUNING 21
#define MUTABOR_ERROR_DOUBLE_ALTERNATIVE  22


#define MUTABOR_ERROR_UNDEFINED_SYMBOL    25
#define MUTABOR_ERROR_UNDEFINED_INTERVAL  26
#define MUTABOR_ERROR_UNDEFINED_TONE      27
#define MUTABOR_ERROR_UNDEFINED_TONE_SYSTEM 28
#define MUTABOR_ERROR_UNDEFINED_RETUNING  29
#define MUTABOR_ERROR_UNDEFINED_HARMONY   30
#define MUTABOR_ERROR_UNDEFINED_PARAMETER 31
#define MUTABOR_ERROR_UNDEFINED_INTERVAL_IN  32
#define MUTABOR_ERROR_UNDEFINED_TONE_IN_SYSTEM  33
#define MUTABOR_ERROR_UNDEFINED_TONE_IN_RETUNING 34

#define MUTABOR_ERROR_UNDEFINED_RETUNING_IN_RETUNING 36
#define MUTABOR_ERROR_UNDEFINED_TUNING_IN_LOGIK 37
#define MUTABOR_ERROR_UNDEFINED_ACTION_IN_LOGIK 38
#define MUTABOR_ERROR_UNDEFINED_PARAMETER_IN_LOGIK 39





#define MUTABOR_ERROR_INVALID_MIDI_CHANNEL 45
#define MUTABOR_ERROR_INVALID_INTERVAL_VALUE 46
#define MUTABOR_ERROR_INVALID_KEY_IN_SYSTEM 47
#define MUTABOR_ERROR_NON_COMPLEX_TONE_IN_RETUNING 48
#define MUTABOR_ERROR_INVALID_VALUE       49
#define MUTABOR_ERROR_INVALID_MIDI_CODE_IN_LOGIK 50
#define MUTABOR_ERROR_INVALID_LOGIK_DEFAULT_CALL 51
#define MUTABOR_ERROR_INVALID_KEY_TRIGGER 52
#define MUTABOR_ERROR_NEARLY_DIVISION_BY_ZERO 53






#define MUTABOR_ERROR_INVALID_PARAMETERS_IN_RETUNING_BOUND 60
#define MUTABOR_ERROR_INVALID_PARAMETER_NUMBER_IN_LOGIK 61
#define MUTABOR_ERROR_TUNING_WITH_INVALID_PARAMETER 62
#define MUTABOR_ERROR_TUNING_BOUND_WITH_INVALID_PARAMETER 63
#define MUTABOR_ERROR_CASE_WITH_INVALID_PARAMETER 64
#define MUTABOR_ERROR_TONE_DEPENDENCY 65
#define MUTABOR_ERROR_CIRCLE_RETUNINGS_AND_LOGICS 66
#define MUTABOR_ERROR_INTERVAL_DEPENDENCY 67
#define MUTABOR_ERROR_MIDI_CHANNEL_DEPENDENCY 68

#define MUTABOR_ERROR_B_INVALID_CHARACTER   70
#define MUTABOR_ERROR_B_INVALID_INTERVAL    71
#define MUTABOR_ERROR_B_INVALID_TONE        72
#define MUTABOR_ERROR_B_INVALID_TONE_SYSTEM 73
#define MUTABOR_ERROR_B_INVALID_PARAMETER_LIST 74
#define MUTABOR_ERROR_B_INVALID_RETUNING    75
#define MUTABOR_ERROR_B_INVALID_HARMONY     76
#define MUTABOR_ERROR_B_INVALID_MIDI_LIST   77
#define MUTABOR_ERROR_B_INVALID_HEX_NUMBER  78
#define MUTABOR_ERROR_B_DECL_EXPECTED       79
#define MUTABOR_ERROR_B_MULTIPLE_DOT_IN_NUMBER 80
#define MUTABOR_ERROR_B_ONLY_INTEGER_ALLOWED 81
#define MUTABOR_ERROR_B_MISSING_OPERAND      82







#define MUTABOR_WARNING_UNDEFINED             0
#define MUTABOR_WARNING_ZERO_MIDI_CHANNEL     1
#define MUTABOR_WARNING_INVALID_TRIGGER       2
#define MUTABOR_WARNING_UNCLEAR_TRIGGER       3
#define MUTABOR_WARNING_DEFAULT_TRIGGER_NOT_LAST 4
#define MUTABOR_WARNING_MIDI_CHANNEL_WASTE    5
#define MUTABOR_WARNING_NO_FILE               6
#define MUTABOR_WARNING_INVALID_CONFIGURATION 7
#define MUTABOR_WARNING_CHANNEL_OVERLAPPING   8
#define MUTABOR_WARNING_GMN_ERROR             9

#ifdef __cplusplus 
  } // extern "C"
} // namespace mutabor
#endif

#endif /* __ERRORS_H_INCLUDED */