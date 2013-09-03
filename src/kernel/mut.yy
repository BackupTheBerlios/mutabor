/** \file                             -*- C -*- 
 ********************************************************************
 * Mutabor tone systems
 *
 * Copyright:   (c) 2008 TU Dresden
 *              changes after 2011-11 (c) by the authors
 * \author  
 *              Volker Abel,
 *              Tobias Schlemmer <keinstein@users.berlios.de>
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
 * \addtogroup templates
 * \{
 ********************************************************************/
/* Muta */

%pure-parser
%locations
%error-verbose
%parse-param { mutabor_box_type * box }
%lex-param { mutabor_box_type * box, void * scanner }
%{

/* BEGIN MUT_TAB.C */

/* MUTABOR / Version 2.1 */

//#pragma warn -cln
//#pragma warn -aus
//#pragma warn -sig
//#pragma warn -par


#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include "src/kernel/box.h"
#include "src/kernel/Global.h"
#include "src/kernel/Parser.h"
#include "src/kernel/Hilfs.h"
#include "src/kernel/Execute.h"

#ifdef _
#undef _
#endif
#define _ _mut

#ifdef __cplusplus
	namespace mutabor {
		namespace hidden {
			extern "C" {
#endif

#define MAX_IDENTIFIER_LEN 80

#define FEHLERZEILE yylloc.first_line+1

#ifdef alloca
#undef alloca
#define alloca make_an_error ---
#endif
#define YYERROR_VERBOSE 1
#define YYMALLOC(size) xmalloc(box,size)
#define YYFREE(X) do { /* empty */; } while (YYID (0))
#define YYMAXLIMIT (HEAP_PORTION_SYNTAX / sizeof(YYLTYPE) - 1)
                   /* wegen fehlendem alloca in PUREC */
%}

%union {
    double      f_value;        /* f〉 Gleitkommazahlen */
    int         integer;        /* F〉 integers */
    char        *identifier;    /* F〉 Namen */
}

/* %define api.token.prefix MUTABOR_TOKEN_ */
/* data types */
%token <identifier> MUTABOR_TOKEN_IDENTIFIER
%token <f_value> MUTABOR_TOKEN_F_NUMBER
%token <integer> MUTABOR_TOKEN_INTEGER

/* sections */
%token MUTABOR_TOKEN_INTERVAL
%token MUTABOR_TOKEN_TONE
%token MUTABOR_TOKEN_TONESYSTEM
%token MUTABOR_TOKEN_RETUNING
%token MUTABOR_TOKEN_HARMONY
%token MUTABOR_TOKEN_LOGIC
%token MUTABOR_TOKEN_MIDICHANNEL

/* operators for triggers */
%token MUTABOR_TOKEN_KEY
%token MUTABOR_TOKEN_FORM

/* operators */
%token MUTABOR_TOKEN_ROOT
%token MUTABOR_TOKEN_MIDI_IN
%token MUTABOR_TOKEN_MIDI_OUT

/* actions and case values */
%token MUTABOR_TOKEN_HARMONY_ANALYSIS
%token MUTABOR_TOKEN_CALLS
%token MUTABOR_TOKEN_ELSE

/* other tokens */
%token MUTABOR_TOKEN_PARAMETER
%token MUTABOR_TOKEN_ENDOFFILE
%token MUTABOR_TOKEN_ERROR


/* ignored tokens */
%token MUTABOR_TOKEN_SPACES
%token MUTABOR_TOKEN_COMMENT
%token MUTABOR_TOKEN_COMMENT_START
%token MUTABOR_TOKEN_COMMENT_END
%token MUTABOR_TOKEN_OTHER

%type <integer> bezugs_taste
%type <f_value> GLEITKOMMA_ZAHL

%{
int yylex(YYSTYPE* lvalp, YYLTYPE* llocp,  mutabor_box_type * box);
void yyerror(YYLTYPE* locp, mutabor_box_type * box, const char* err)
{
	mutabor_error_message(box,
			      false,
			      _("%s at line %d"),
			      err,
			      locp->first_line);
}

#define YYLEX_PARAM box
//#define scanner box
%}
/*  %left '+' '-'  */

%% /* Grammar rules and actions */

start : /* empty */
        | start intervalldeklaration
        | start tondeklaration
        | start tonsystemdeklaration
      	| start umstimmungdeklaration
        | start harmoniedeklaration
        | start logikdeklaration
        | start instrumentdeklaration
        | start error {  mutabor_error_message(box,
					       false,
					       _("Syntax error in line %d."),
					       FEHLERZEILE); }
;

intervalldeklaration :
        MUTABOR_TOKEN_INTERVAL intervalldekl1 ;

intervalldekl1 :
          /* empty */
        | intervalldekl1 intervalldekl2_1
      ;

intervalldekl2_1:
          { init_komplex_ton_list (box); } intervalldekl2
      ;

intervalldekl2 :
          MUTABOR_TOKEN_IDENTIFIER '=' GLEITKOMMA_ZAHL ':' GLEITKOMMA_ZAHL
                    { if ( fabs($5) > 0.001 )
				    get_new_intervall (box, $1, $3 / $5);
                      else
			      mutabor_error_message (box,false,
					   _("Bad interval value in %s"),
					   ($1) ); }

        | MUTABOR_TOKEN_IDENTIFIER '=' GLEITKOMMA_ZAHL MUTABOR_TOKEN_ROOT GLEITKOMMA_ZAHL
                    { if ( fabs ($3) > 0.001 )
				    get_new_intervall (box, $1, pow ($5, 1 / $3));
                      else
			      mutabor_error_message (box,false,
						     _("Bad interval value in %s"),
						     ($1)); }
/*****
        | MUTABOR_TOKEN_IDENTIFIER '=' GLEITKOMMA_ZAHL
                    { get_new_intervall ($1, $3); }
****/
	| MUTABOR_TOKEN_IDENTIFIER '=' KOMPLEX_TON_LIST
	{ get_new_intervall_komplex (box,$1); }
        | MUTABOR_TOKEN_IDENTIFIER '=' error { 
		mutabor_error_message(box,
				      false,
				      _("Bad interval declaration of intervall %s in line %d."),
				      ($1),
				      FEHLERZEILE); 
	  }
        | MUTABOR_TOKEN_IDENTIFIER error { 
		mutabor_error_message(box,
				      false,
				      _("Wrong character. Expecting %s in line %d."),
				      mutT("="),FEHLERZEILE); 
	  }
      ;

tondeklaration :
        MUTABOR_TOKEN_TONE tondekl1 ;

tondekl1 :
          /* empty */
	| tondekl1 tondekl2_1
      ;

tondekl2_1:
	  { init_komplex_ton_list (box); }  tondekl2
      ;

tondekl2 :
          MUTABOR_TOKEN_IDENTIFIER '=' GLEITKOMMA_ZAHL
	  { get_new_ton_absolut (box, $1, $3); }

	| MUTABOR_TOKEN_IDENTIFIER '='
		MUTABOR_TOKEN_IDENTIFIER '-' KOMPLEX_TON_LIST
	{ get_new_ton_komplex_negative (box, $1, $3); }

	| MUTABOR_TOKEN_IDENTIFIER '='
		     MUTABOR_TOKEN_IDENTIFIER
	{ get_new_ton_komplex_positive (box, $1, $3); }

	| MUTABOR_TOKEN_IDENTIFIER '='
		     MUTABOR_TOKEN_IDENTIFIER '+' KOMPLEX_TON_LIST
	{ get_new_ton_komplex_positive (box, $1, $3); }

      | MUTABOR_TOKEN_IDENTIFIER '=' error { 
	      mutabor_error_message(box,
				    false,
				    _("Bad tone declaration of tone %s in line %d."),
				    ($1),
				    FEHLERZEILE); 
	}
      ;

KOMPLEX_TON_LIST:
	  KOMPLEX_TON_START KOMPLEX_TON_LIST_2
	;


KOMPLEX_TON_LIST_2 :
	  /* empty */
	| KOMPLEX_TON_LIST_2 KOMPLEX_TON_1

KOMPLEX_TON_1 :
          '+' MUTABOR_TOKEN_IDENTIFIER
{ get_new_faktor_anteil ( box, (double) 1, $2); }

        | '-' MUTABOR_TOKEN_IDENTIFIER
	{ get_new_faktor_anteil ( box,  (double) -1, $2); }

        | '+' GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
	{ get_new_faktor_anteil (box, $2, $3); }

        | '-' GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
	{ get_new_faktor_anteil (box, -($2), $3); }

        | '+' '/' GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
                   { if ( fabs($3) > 0.001 )
				   get_new_faktor_anteil (box, (double) 1 / ($3), $4);
			   else
				   mutabor_error_message(box,
							 false,
							 _("Division by (nearly) 0 in line %d."),
							 FEHLERZEILE);  
		   }

        | '-' '/' GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
                   { if ( fabs($3) > 0.001 )
				   get_new_faktor_anteil (box, (double) -1 / ($3), $4);
                     else
				   mutabor_error_message(box,
							 false,
							 _("Division by (nearly) 0 in line %d."),
							 FEHLERZEILE);  
		   }

        | '+' GLEITKOMMA_ZAHL '/' GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
                   { if ( fabs($4) > 0.001 )
				   get_new_faktor_anteil (box, ($2) / ($4), $5);
                     else
			     mutabor_error_message(box,
						   false,
						   _("Division by (nearly) 0 in line %d."),
						   FEHLERZEILE);  
		   }

        | '-' GLEITKOMMA_ZAHL '/' GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
                   { if ( fabs($4) > 0.001 )
				   get_new_faktor_anteil (box, -($2) / ($4), $5);
                     else
			     mutabor_error_message(box,
						   false,
						   _("Division by (nearly) 0 in line %d."),
						   FEHLERZEILE);  
		   }
/******
        | '+'  '(' MUTABOR_TOKEN_IDENTIFIER ',' MUTABOR_TOKEN_IDENTIFIER ')'
            { get_new_relativ_anteil ( (double) 1, $3, $5); }

        | '-'  '(' MUTABOR_TOKEN_IDENTIFIER ',' MUTABOR_TOKEN_IDENTIFIER ')'
            { get_new_relativ_anteil ( (double) -1, $3, $5); }

        | '+' GLEITKOMMA_ZAHL '(' MUTABOR_TOKEN_IDENTIFIER ',' MUTABOR_TOKEN_IDENTIFIER ')'
            { get_new_relativ_anteil ( $2, $4, $6); }

        | '-' GLEITKOMMA_ZAHL '(' MUTABOR_TOKEN_IDENTIFIER ',' MUTABOR_TOKEN_IDENTIFIER ')'
            { get_new_relativ_anteil ( - $2, $4, $6); }
**********/
        ;

KOMPLEX_TON_START :
	  MUTABOR_TOKEN_IDENTIFIER
	  { get_new_faktor_anteil (box, (double) 1.0 , $1); }

/***
        | '-' MUTABOR_TOKEN_IDENTIFIER
                   { get_new_faktor_anteil ( (double) -1.0 , $2); }
***/

	|  GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
	{ get_new_faktor_anteil (box, $1, $2); }

        |  '/' GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
                   { if ( fabs($2) > 0.001 )
				   get_new_faktor_anteil (box, (double) 1 / ($2), $3);
                     else
			     mutabor_error_message(box,
						   false,
						   _("Division by (nearly) 0 in line %d."),
						   FEHLERZEILE);  
		   }

        | GLEITKOMMA_ZAHL '/' GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
                   { if ( fabs($3) > 0.001 )
				   get_new_faktor_anteil (box, ($1) / ($3), $4);
                     else
			     mutabor_error_message(box,
						   false,
						   _("Division by (nearly) 0 in line %d."),
						   FEHLERZEILE);  
		   }
/****
        | '-' GLEITKOMMA_ZAHL MUTABOR_TOKEN_IDENTIFIER
                   { get_new_faktor_anteil ( -($2), $3); }
****/

/********
	|  '(' MUTABOR_TOKEN_IDENTIFIER ',' MUTABOR_TOKEN_IDENTIFIER ')'
	    { get_new_relativ_anteil ( (double) 1, $2, $4); }

        | '-'  '(' MUTABOR_TOKEN_IDENTIFIER ',' MUTABOR_TOKEN_IDENTIFIER ')'
            { get_new_relativ_anteil ( (double) -1, $3, $5); }

	|  GLEITKOMMA_ZAHL '(' MUTABOR_TOKEN_IDENTIFIER ',' MUTABOR_TOKEN_IDENTIFIER ')'
	    { get_new_relativ_anteil ( $1, $3, $5); }

        | '-' GLEITKOMMA_ZAHL '(' MUTABOR_TOKEN_IDENTIFIER ',' MUTABOR_TOKEN_IDENTIFIER ')'
            { get_new_relativ_anteil ( - $2, $4, $6); }
**********/
        ;

tonsystemdeklaration :
        MUTABOR_TOKEN_TONESYSTEM tonsystemdekl1 ;

tonsystemdekl1 :
          /* empty */
        | tonsystemdekl1 tonsystemdekl2_1
      ;
      
tonsystemdekl2_1:
         { init_ton_liste (box); } tonsystemdekl2
      ;

tonsystemdekl2 : 
         MUTABOR_TOKEN_IDENTIFIER '=' MUTABOR_TOKEN_INTEGER '[' tonliste ']' 
                      { init_komplex_ton_list (box); }
                        KOMPLEX_TON_LIST
			{ get_new_tonsystem (box, $1, $3); }
      |  MUTABOR_TOKEN_IDENTIFIER '=' MUTABOR_TOKEN_INTEGER '[' tonliste ']' 
                      { init_komplex_ton_list (box); }
                    '-' KOMPLEX_TON_LIST
		    { get_new_tonsystem_negative (box, $1, $3); }
      ;

tonliste : ton_element | tonliste ',' ton_element ;

ton_element :
MUTABOR_TOKEN_IDENTIFIER      { get_new_ton_in_tonsystem (box, $1); }
| /* empty */     { get_new_ton_in_tonsystem (box, NULL); }
        | error { 
		mutabor_error_message(box,
				      false,
				      _("Bad tonesystem declaration in line %d."),		      
				      FEHLERZEILE);
	  }
      ;

parameter_liste :      /* allgemein eine Liste von Identifiern */
MUTABOR_TOKEN_IDENTIFIER      { get_new_name_in_parameterlist (box, $1); }
        | parameter_liste ',' MUTABOR_TOKEN_IDENTIFIER
	{ get_new_name_in_parameterlist (box, $3); }
        | error {
		mutabor_error_message(box,
				      false,
				      _("Bad parameter list in call to %s in line %d"),
				      FEHLERZEILE); 
	  }
        ;


argument_liste :
        argument_listenelement {}
        | argument_liste ',' argument_listenelement {}
        | error { 
		mutabor_error_message(box,
				      false,
				      _("Bad parameter list in call to %s in line %d"),
				      FEHLERZEILE); 
	  }
;

argument_listenelement :        /* allgemein eine Liste von Identifiern
                          oder Kommazahlen */
MUTABOR_TOKEN_IDENTIFIER      { get_new_name_in_argument_list (box, $1) ;  }
| MUTABOR_TOKEN_INTEGER        { get_new_number_in_argument_list (box, $1) ;  }
| '-' MUTABOR_TOKEN_INTEGER        { get_new_number_in_argument_list (box, -($2)); }

        ;


umstimmungdeklaration :
        MUTABOR_TOKEN_RETUNING umstimmungs_dekl_1 ;

umstimmungs_dekl_1 :
          /* empty */
        | umstimmungs_dekl_1 umstimmungs_dekl_2
      ;

umstimmungs_dekl_2 :
          MUTABOR_TOKEN_IDENTIFIER
	  { init_umstimmung (box, $1);
                      init_parameter_liste (box); }
          '='       { eintrage_parameterliste_in_umstimmung (box); }
          umstimmungs_dekl_3
                    { get_new_umstimmung (box); }
        |
          MUTABOR_TOKEN_IDENTIFIER
	{ init_umstimmung (box,$1);
		init_parameter_liste (box); }
          '(' parameter_liste ')' '=' 
                    { eintrage_parameterliste_in_umstimmung (box); }
          umstimmungs_dekl_3 
                    { get_new_umstimmung (box); }
        ;
        
umstimmungs_dekl_3 :
          umstimmungs_dekl_taste_abs            {}
        | umstimmungs_dekl_taste_rel            {}
        | umstimmungs_dekl_breite_abs           {}
        | umstimmungs_dekl_breite_rel           {}
        | umstimmungs_dekl_tonhoehe_veraendert  {}
        | umstimmungs_dekl_wiederholung_abs     {}
        | umstimmungs_dekl_wiederholung_rel     {}
        | umstimmungs_dekl_umstimmungs_bund     {}
        | umstimmungs_dekl_umstimmungs_case     {} 
        | umstimmungs_dekl_midi_out             {}  
        | error {
		mutabor_error_message(box,
				      false,
				      _("Bad retuning in line %d."),
				      FEHLERZEILE);
	  }   
        ;


umstimmungs_dekl_taste_abs :
          MUTABOR_TOKEN_INTEGER '[' ']'
          { get_umstimmung_taste_abs (box, zahl, $1, NULL); }
        | MUTABOR_TOKEN_IDENTIFIER '[' ']'
	{ get_umstimmung_taste_abs (box, parameter, 0.0, $1); }
        ;

umstimmungs_dekl_taste_rel :
          '@' '+' MUTABOR_TOKEN_INTEGER '[' ']'
          { get_umstimmung_taste_rel (box, zahl, $3, NULL, '+'); }
        | '@' '+' MUTABOR_TOKEN_IDENTIFIER '[' ']'
	{ get_umstimmung_taste_rel (box, parameter, 0.0, $3, '+'); }
        | '@' '-' MUTABOR_TOKEN_INTEGER '[' ']'
	{ get_umstimmung_taste_rel (box, zahl, $3, NULL, '-'); }
        | '@' '-' MUTABOR_TOKEN_IDENTIFIER '[' ']'
	{ get_umstimmung_taste_rel (box, parameter, 0.0, $3, '-'); }
        ;

umstimmungs_dekl_breite_abs :
          '[' '<' '<' MUTABOR_TOKEN_INTEGER '>' '>' ']'
          { get_umstimmung_breite_abs (box, zahl, $4, NULL); }
        | '[' '<' '<' MUTABOR_TOKEN_IDENTIFIER '>' '>' ']'
	{ get_umstimmung_breite_abs (box, parameter, 0.0, $4); }
        ;

umstimmungs_dekl_breite_rel :
          '[' '<' '<' '@' '+' MUTABOR_TOKEN_INTEGER '>' '>' ']'
          { get_umstimmung_breite_rel (box, zahl, $6, NULL, '+'); }
        | '[' '<' '<' '@' '+' MUTABOR_TOKEN_IDENTIFIER '>' '>' ']'
	{ get_umstimmung_breite_rel (box, parameter, 0.0, $6, '+'); }
        | '[' '<' '<' '@' '-' MUTABOR_TOKEN_INTEGER '>' '>' ']'
	{ get_umstimmung_breite_rel (box, zahl, $6, NULL, '-'); }
        | '[' '<' '<' '@' '-' MUTABOR_TOKEN_IDENTIFIER '>' '>' ']'
	{ get_umstimmung_breite_rel (box, parameter, 0.0, $6, '-'); }
        | '[' '<' '<' '@' '*' MUTABOR_TOKEN_INTEGER '>' '>' ']'
	{ get_umstimmung_breite_rel (box, zahl, $6, NULL, '*'); }
        | '[' '<' '<' '@' '*' MUTABOR_TOKEN_IDENTIFIER '>' '>' ']'
	{ get_umstimmung_breite_rel (box, parameter, 0.0, $6, '*'); }
        | '[' '<' '<' '@' '/' MUTABOR_TOKEN_INTEGER '>' '>' ']'
	{ get_umstimmung_breite_rel (box, zahl, $6, NULL, '/'); }
        | '[' '<' '<' '@' '/' MUTABOR_TOKEN_IDENTIFIER '>' '>' ']'
	{ get_umstimmung_breite_rel (box, parameter, 0.0, $6, '/'); }
        ;

umstimmungs_dekl_tonhoehe_veraendert :
          '[' 
            { init_umstimm_expression_list (box); }
          nonempty_umstimm_expression_list ']'
          { get_umstimmung_tonhoehe_veraendert (box); }
        ;

/* Eine leere Liste von Umstimm-expression kollidiert syntaktisch
   mit "umstimmungs-wiederholung" oder "umstimmungs-bund" */
        
nonempty_umstimm_expression_list :
          nonempty_umstimm_expression {}
        | nonempty_umstimm_expression ',' umstimm_expression_list {}

        | ','      { init_komplex_ton_list(box);
		  get_new_umstimm_expression (box,NULL);
                   } 
          umstimm_expression_list {}
        ;
        
umstimm_expression_list :
          umstimm_expression {}
        | umstimm_expression_list ',' umstimm_expression {}
        ;
        
umstimm_expression :
          /* empty */
                   { init_komplex_ton_list(box);
		  get_new_umstimm_expression (box, NULL);
                   }
        | 
          nonempty_umstimm_expression
        ;
        
nonempty_umstimm_expression :
MUTABOR_TOKEN_IDENTIFIER  { get_new_umstimm_expression (box,$1); }
| '@'         { get_new_umstimm_expression (box, "@"); }

        | MUTABOR_TOKEN_IDENTIFIER '+' { init_komplex_ton_list (box); } 
          KOMPLEX_TON_LIST 
	  { get_new_umstimm_expression_positive (box, $1); }
| '@' '+' { init_komplex_ton_list (box); } 
          KOMPLEX_TON_LIST 
	  { get_new_umstimm_expression_positive (box, "@" ); }
        | MUTABOR_TOKEN_IDENTIFIER '-' { init_komplex_ton_list (box); } 
          KOMPLEX_TON_LIST 
	  { get_new_umstimm_expression_negative (box,$1); }
        | '@' '-' { init_komplex_ton_list (box); } 
          KOMPLEX_TON_LIST 
	  { get_new_umstimm_expression_negative (box, "@" ); }
        ;
        
        
umstimmungs_dekl_wiederholung_abs :
          '[' ']' { init_komplex_ton_list (box); }
                      KOMPLEX_TON_LIST
          { get_umstimmung_wiederholung_abs (box); }
        | '[' ']' { init_komplex_ton_list (box); }
                 '-'  KOMPLEX_TON_LIST
          { get_umstimmung_wiederholung_abs_negative (box); }
        ;

umstimmungs_dekl_wiederholung_rel :
          '[' ']' '@' '+' { init_komplex_ton_list (box); }
                            KOMPLEX_TON_LIST
          { get_umstimmung_wiederholung_rel_positive (box); }
        | '[' ']' '@' '-' { init_komplex_ton_list (box); }
                            KOMPLEX_TON_LIST
          { get_umstimmung_wiederholung_rel_negative (box); }
        ;

umstimmungs_dekl_umstimmungs_bund :
          '{' { init_aktions_liste (box); } 
          aktions_liste '}'
          { get_umstimmung_umstimmungs_bund (box); }
        ;
        
aktions_liste :
          aktion {}
        | aktions_liste ',' aktion {}
        ;
        
aktion : 
          MUTABOR_TOKEN_IDENTIFIER { init_argument_liste (box); 
		  get_new_aktion_aufruf_element (box,$1); }
        | MUTABOR_TOKEN_IDENTIFIER '(' { init_argument_liste (box); }
            argument_liste ')'
	    { get_new_aktion_aufruf_element (box,$1); }
        | MUTABOR_TOKEN_MIDI_OUT '(' 
		              { init_integersequenz (box);}
		           integersequenz ')' 
			   { get_new_aktion_midi_out_element (box); }
        | MUTABOR_TOKEN_HARMONY_ANALYSIS { get_new_aktion_harmony_analysis(box); }
        ;          

umstimmungs_dekl_umstimmungs_case :
          MUTABOR_TOKEN_INTEGER '{' { init_umstimmungs_case_liste (box); } 
          umstimmungs_case_liste '}'
          { get_umstimmung_umstimm_case_zahl (box,$1); }
        | MUTABOR_TOKEN_IDENTIFIER '{' { init_umstimmungs_case_liste (box); } 
          umstimmungs_case_liste '}'
          { get_umstimmung_umstimm_case_parameter (box,$1); }
        ;

umstimmungs_case_liste :
          umstimmungs_case_element {}
        | umstimmungs_case_liste  umstimmungs_case_element {}
        ;

umstimmungs_case_element :
          MUTABOR_TOKEN_INTEGER '-' '>' 
             { init_aktions_liste (box); }
             aktions_liste 
             { get_umstimmungs_case_zahl_element (box,$1); }
        | '-' MUTABOR_TOKEN_INTEGER '-' '>' 
             { init_aktions_liste (box); }
             aktions_liste 
             { get_umstimmungs_case_zahl_element (box,-($2)); }
        | MUTABOR_TOKEN_ELSE '-' '>' 
             { init_aktions_liste (box); }
             aktions_liste
             { get_umstimmungs_case_default_element (box); }
        ;

umstimmungs_dekl_midi_out:  
		  MUTABOR_TOKEN_MIDI_OUT '(' 
		              { init_integersequenz (box);}
		           integersequenz ')' 
		              { get_umstimmung_midi_out (box); }

        

harmoniedeklaration :
          MUTABOR_TOKEN_HARMONY harmonie_dekl_1 {}
        ;
        
harmonie_dekl_1 :
          /* empty */ {}
        | harmonie_dekl_1  harmonie_dekl_2 {}
        ;
        
harmonie_dekl_2 :
          MUTABOR_TOKEN_IDENTIFIER '=' '{' 
	  { init_tastenliste (box); }
          tasten_liste '}' bezugs_taste
	  { get_new_harmonie (box, $1, $7); }

        | MUTABOR_TOKEN_IDENTIFIER '=' error { 
		mutabor_error_message(box,
				      false,
				      _("Bad pattern declaration “%s” at line %d"),
				      ($1),
				      FEHLERZEILE); 
	  }
        ;
        
bezugs_taste :
          /* empty */      { $$ = -1; }
        | '.' MUTABOR_TOKEN_INTEGER     { $$ = $2; }
        ;
        
tasten_liste :
          taste {}
        | tasten_liste ',' taste {}
        ;

taste :
MUTABOR_TOKEN_INTEGER      { get_new_taste (box, $1, '+'); }
| '*' MUTABOR_TOKEN_INTEGER  { get_new_taste (box, $2, '*'); }
        ;
        
logikdeklaration :
          MUTABOR_TOKEN_LOGIC logik_dekl_1 {}
        ;
        
logik_dekl_1 :
          /* empty */ {}
        | logik_dekl_1 { init_ausloeser (box);
                         /* f〉 die Anfangsausloesung der Logik */
                       } 
               logik_dekl_2 {}
        ;
        
logik_dekl_2 :
          MUTABOR_TOKEN_IDENTIFIER ausloeser '=' '[' 
	  { get_new_logik (box, $1, NULL);
                init_anweisungs_liste (box); }
          anweisungs_liste ']'
              { vervollstaendige_logik (box); }
        | MUTABOR_TOKEN_IDENTIFIER ausloeser '=' MUTABOR_TOKEN_IDENTIFIER '[' 
	{ get_new_logik (box, $1, $4);
                init_anweisungs_liste (box); }
          anweisungs_liste ']'
              { vervollstaendige_logik (box); }
        ;
        
anweisungs_liste :
          /* empty */ {}
        | anweisungs_liste anweisung {}
        ;


anweisung :
          ausloeser '-' '>' '{' { init_aktions_liste (box); } 
                               aktions_liste '}'
                       { get_new_anweisung (box); }
        |
          ausloeser '-' '>'     { init_aktions_liste (box); } 
                               aktion 
                       { get_new_anweisung (box); }
        ;
        
harmoniebezeichner :
          MUTABOR_TOKEN_INTEGER '~' MUTABOR_TOKEN_IDENTIFIER '~' MUTABOR_TOKEN_INTEGER
	  { get_harmoniebezeichner (box, $1, $3,  $5); }
        |              MUTABOR_TOKEN_IDENTIFIER '~' MUTABOR_TOKEN_INTEGER
	{ get_harmoniebezeichner (box, MUTABOR_NO_KEY, $1,  $3); }
        | MUTABOR_TOKEN_INTEGER '~' MUTABOR_TOKEN_IDENTIFIER
	{ get_harmoniebezeichner (box, $1, $3, MUTABOR_NO_KEY); }
        |              MUTABOR_TOKEN_IDENTIFIER 
	{ get_harmoniebezeichner (box,  MUTABOR_NO_KEY, $1, MUTABOR_NO_KEY); }
        ; 
 
ausloeser :
          MUTABOR_TOKEN_ELSE { get_ausloeser_default (box); }
        | harmoniebezeichner  { get_ausloeser_harmonie (box); }
        | MUTABOR_TOKEN_FORM harmoniebezeichner { get_ausloeser_harmonie_form (box); }
| MUTABOR_TOKEN_KEY MUTABOR_TOKEN_IDENTIFIER { get_ausloeser_taste (box,$2); }
        | MUTABOR_TOKEN_MIDI_IN '(' { init_integersequenz (box); }
              integersequenz ')' 
                      { get_ausloeser_midi_in (box); }
        ;
        
integersequenz :
          MUTABOR_TOKEN_INTEGER 
	  { get_new_integer_in_integersequenz (box,$1);}
        | integersequenz ',' MUTABOR_TOKEN_INTEGER 
	{ get_new_integer_in_integersequenz (box,$3);}
        | error { 
		mutabor_error_message(box,
				      false,
				      _("Bad MIDI list in line %d"),  
				      FEHLERZEILE );
	  }
        ;
 

instrumentdeklaration :
          MUTABOR_TOKEN_MIDICHANNEL instrument_dekl_1 {}
        ;
        
instrument_dekl_1 :
          /* empty */ {}
        | instrument_dekl_1 instrument_dekl_2 {}
        ;

instrument_dekl_2 :
          MUTABOR_TOKEN_INTEGER '-' '>' MUTABOR_TOKEN_INTEGER 
	  { get_instrument_dekl (box, $1, $4, $4, 0, & box->file->list_of_instrumente); }
        | MUTABOR_TOKEN_INTEGER '-' '>' MUTABOR_TOKEN_INTEGER '-' MUTABOR_TOKEN_INTEGER 
	{ get_instrument_dekl (box, $1, $4, $6, 0, & box->file->list_of_instrumente); }
        | MUTABOR_TOKEN_INTEGER '-' '>' '[' MUTABOR_TOKEN_INTEGER ']' 
	{ get_instrument_dekl (box, $1, 0, 0, $5, & box->file->list_of_instrumente); }
        | MUTABOR_TOKEN_INTEGER '-' '>' MUTABOR_TOKEN_INTEGER '[' MUTABOR_TOKEN_INTEGER ']'
	{ get_instrument_dekl (box, $1, $4, $4, $6, & box->file->list_of_instrumente); }
        | MUTABOR_TOKEN_INTEGER '-' '>' MUTABOR_TOKEN_INTEGER '-' MUTABOR_TOKEN_INTEGER '[' MUTABOR_TOKEN_INTEGER ']'
	{ get_instrument_dekl (box, $1, $4, $6, $8, & box->file->list_of_instrumente); }
        ;
        
GLEITKOMMA_ZAHL:
MUTABOR_TOKEN_F_NUMBER  { $$ = $1 ; }
|  MUTABOR_TOKEN_INTEGER   { $$ = (double) $1 ; }
        ;

        
%%
/// \todo check whether this function is necessary for any system but windows
#ifdef __WXMSW__
inline int mutabor_toupper(int c) {
	if (c == EOF) return c;
	else return toupper(c);
}
#ifdef toupper
#undef toupper
#endif
#define toupper mutabor_toupper
#endif

int yylex(YYSTYPE* lvalp, YYLTYPE* llocp,  mutabor_box_type * box)
{
    int c;

start_lex:
    /* Ignore whitespace, get first nonwhitespace character */
    while ( box->scanner->anzahl_eingelesene_zeichen ++,
            isspace(c = *(box->scanner->pos++)) ) {
	    DEBUGLOG2(other,_T("char %x"),c);
	    if (c == '\n') {
		    DEBUGLOG2(other,_T("New line"));
		    if (!(llocp->first_line ++ % LINE_DRAW_QUANTUM)) 
			    show_line_number(box,llocp->first_line);
	    }
    }
    
    if (c == '"') {
	    while (box->scanner->anzahl_eingelesene_zeichen ++,
		   (c= *(box->scanner->pos++)) != '"' && c != EOF )
                    if (c == '\n') llocp->first_line ++;
        
       goto start_lex;
    }

    if (c == 0) {
	    show_line_number(box,llocp->first_line);
	    return 0;
    }

        
    /* char starts a number => parse the number. */
    if (isdigit(c)) {
	    double zahl = 0.0;
	    while (isdigit(c)) {
		    zahl *= 10;
		    zahl += ( c - '0' );
		    box->scanner->anzahl_eingelesene_zeichen ++;
		    c = *(box->scanner->pos++);
	    }
	    if (c == '.') {    /* dann nachkommastellen */
		    double faktor = 1.0;
		    while (box->scanner->anzahl_eingelesene_zeichen ++,
			   isdigit (c = *(box->scanner->pos++))) {
			    faktor /= 10;
			    zahl += faktor * ( c - '0' );
		    }
		    --(box->scanner->pos);
		    mutASSERT(*(box->scanner->pos) == c);
		    mutASSERT((box->scanner->pos) >= (box->scanner->data));
		    box->scanner->anzahl_eingelesene_zeichen --;
		    lvalp->f_value = zahl;
		    return MUTABOR_TOKEN_F_NUMBER;
	    }
	    else {
		    --(box->scanner->pos);
		    mutASSERT(*(box->scanner->pos) == c);
		    mutASSERT((box->scanner->pos) >= (box->scanner->data));
		    box->scanner->anzahl_eingelesene_zeichen --;
            
		    if (zahl > INT_MAX) {
			    lvalp->f_value = zahl;
			    return MUTABOR_TOKEN_F_NUMBER;
		    }
		    else {
			    lvalp->integer = (int)zahl;
			    return MUTABOR_TOKEN_INTEGER;
		    }
	    }

      
    }
    
    /* # starts a HEX-number => parse the number. */
    if (c == '#') {
        unsigned int help;
	char * newpos;
	int len;
	errno = 0;
	help = strtoull(box->scanner->pos, &newpos, 16);
        if (errno || newpos == box->scanner->pos) {
		mutabor_error_message(box,
				      false,
				      _("No hex number after # (line %d)"),
				      llocp->first_line + 1);
		return MUTABOR_TOKEN_ERROR;
        }
	len = newpos - box->scanner->pos;
        box->scanner->pos = newpos;
        box->scanner->anzahl_eingelesene_zeichen += len;
	lvalp->integer = help;
        return MUTABOR_TOKEN_INTEGER;

/* printf("f_number:%lf:\n", lvalp->f_value); */

    }
    
    /* Test auf reserved word oder einen Identifier */
    if (isalpha (c) || (c == '_') || (c == '\'') ) {

static struct { 
    const char *word;
    int token;
} reserved_words [] = {
/* German keywords : */
{ "INTERVALL"  , MUTABOR_TOKEN_INTERVAL  },
{ "WURZEL"     , MUTABOR_TOKEN_ROOT     },
{ "TON"        , MUTABOR_TOKEN_TONE        },
{ "TONSYSTEM"  , MUTABOR_TOKEN_TONESYSTEM  },
{ "UMSTIMMUNG" , MUTABOR_TOKEN_RETUNING },
{ "HARMONIE"   , MUTABOR_TOKEN_HARMONY   },
{ "LOGIK"      , MUTABOR_TOKEN_LOGIC      },
{ "FORM"       , MUTABOR_TOKEN_FORM       },
{ "HARMONIEANALYSE", MUTABOR_TOKEN_HARMONY_ANALYSIS },
{ "MIDIKANAL"  , MUTABOR_TOKEN_MIDICHANNEL },
{ "TASTE"      , MUTABOR_TOKEN_KEY      },
{ "MIDIIN"     , MUTABOR_TOKEN_MIDI_IN    },
{ "MIDIOUT"    , MUTABOR_TOKEN_MIDI_OUT   },
{ "ANSONSTEN"  , MUTABOR_TOKEN_ELSE  },
/* English keywords : */
{ "INTERVAL"   , MUTABOR_TOKEN_INTERVAL  },
{ "ROOT"       , MUTABOR_TOKEN_ROOT     },
{ "TONE"       , MUTABOR_TOKEN_TONE        },
{ "TONESYSTEM" , MUTABOR_TOKEN_TONESYSTEM  },
{ "RETUNING"   , MUTABOR_TOKEN_RETUNING },
{ "PATTERN"    , MUTABOR_TOKEN_HARMONY   },
{ "HARMONY_ANALYSIS", MUTABOR_TOKEN_HARMONY_ANALYSIS },
{ "LOGIC"      , MUTABOR_TOKEN_LOGIC      },
{ "SHIFTED"    , MUTABOR_TOKEN_FORM       },
{ "MIDICHANNEL", MUTABOR_TOKEN_MIDICHANNEL },
{ "KEY"        , MUTABOR_TOKEN_KEY      },
{ "ELSE"       , MUTABOR_TOKEN_ELSE  },
{ NULL         , 0          }
};


#if 0
        char *symbuffer = xmalloc ((size_t)(MAX_IDENTIFIER_LEN + 1));
        int i=0;
        
        do {
            if (c == '\'') c = 'i';
            symbuffer[i++] = c;
            c = *(box->scanner->pos++);
        } while (c != EOF                && 
                 i < MAX_IDENTIFIER_LEN  && 
                 (isalnum (c) || (c == '_') || (c == '\'') ) );
        
        --(box->scanner->pos);
	mutASSERT(*(box->scanner->pos) == c);
	mutASSERT((box->scanner->pos) >= (box->scanner->data));
        symbuffer[i] = '\0';
#else
        int i = 0;
        int max_identifier_len = 10;
        char *symbuffer = (char*) xmalloc (box, (size_t) max_identifier_len);

        do {
            if (c == '\'') c = 'i';

            if ( i + 1 == max_identifier_len ) {
		    char * help = (char*) xrealloc (box, symbuffer, (size_t) (max_identifier_len += 10));
		    memmove (help, symbuffer, (size_t) max_identifier_len);
		    symbuffer = help;
            }

            symbuffer[i++] = c;
	    DEBUGLOG2(other,_T("character #%d = %x"),i,c);
	    c = *(box->scanner->pos++);
	    DEBUGLOG2(other,_T("character #%d = %x"),i,c);
            box->scanner->anzahl_eingelesene_zeichen ++;

        } while ((c) && 
                 (isalnum (c) || (c == '_') || (c == '\'') ) );
        
        --(box->scanner->pos);
	mutASSERT(*(box->scanner->pos) == c);
	mutASSERT((box->scanner->pos) >= (box->scanner->data));
        box->scanner->anzahl_eingelesene_zeichen --;
        symbuffer[i] = '\0';

#endif

/* printf("symbuffer:%s:\n", symbuffer); */
        
        for (i=0; reserved_words[i].word; i++) {
            if ( ! strcasecmp (symbuffer, reserved_words[i].word)) {
                xfree (symbuffer);
                return reserved_words[i].token;
            }
        }
        
        lvalp->identifier = symbuffer;
        return MUTABOR_TOKEN_IDENTIFIER;
    }
    
    /* Any other character is a token by itself */
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '[':
        case ']':
        case ':':
        case '=':
        case '(':
        case ')':
        case ',':
        case '~':
        case '@':
        case '<':
        case '>':
        case '{':
        case '}':
        case ';':
               return c;
    }

    
    mutabor_error_message(box,
			  false,
			  _("Invalid character: “%c” = %x at line %d."),
			  c,c,llocp->first_line + 1);
    
    return 0;  /* um Compilerwarnungen zu vermeiden */

    
} /* yylex */

void init_yylex ()
{
	//    llocp->first_line = 0;
	//    box->scanner->anzahl_eingelesene_zeichen = 0;
}


#ifdef __cplusplus
		}
	}
 }
#endif

/* END MUT_TAB.C */

/* 
 * \}
 */
