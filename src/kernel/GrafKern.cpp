/** \file
 ********************************************************************
 * Output operations
 *
 * \author Rüdiger Krauße <krausze@mail.berlios.de>
 *         Tobias Schlemmer <keinstein@users.berlios.de>
 * \date $Date: 2011/11/02 14:31:57 $
 * \version $Revision: 1.19 $
 * \license GPL
 *
 *
 ********************************************************************
 * \addtogroup kernel
 * \{
 ********************************************************************/

#if 0
// ------------------------------------------------------------------
// Mutabor 2.win, 1997, R.Krauße
// Ausgabe-Funktionen
// ------------------------------------------------------------------

#include "src/kernel/Defs.h"
#include "src/wxGUI/generic/mutDebug.h"
#include "src/kernel/Global.h"
#include "src/kernel/Interpre.h"
#include "src/kernel/GrafKern.h"
#include "src/kernel/Interval.h"
//#include "Mutabor.rh"
#include "src/kernel/Execute.h"
#include <setjmp.h>

// Needed at least for Visual C++ 8
#include "src/kernel/Runtime.h"
#include "src/kernel/routing/Route-inlines.h"

#ifdef _
#undef _
#endif
#define _ _mut

#if 0
#ifdef MUTWIN
#ifdef WX
#include "wx/msgdlg.h"
wxStaticText* CompDiaLine = NULL;
#else
HWND CompDiaLine;
#endif
#endif
#endif

int protokoll_ausgabezeile = 0;
int aktionen_ausgabezeile = 0;

extern jmp_buf weiter_gehts_nach_compilerfehler;

#define PROT_MAXLINES 30
#define PROT_MAXCHARS PROT_MAXLINES*40
#define AKTIONEN_MAX 100
#define AKTIONEN_STRLEN (AKTIONEN_MAX*50)

char protokoll_string[PROT_MAXCHARS];

char sAktionen[AKTIONEN_STRLEN];


unsigned char boxAktionen[AKTIONEN_MAX];

int lAktionen[AKTIONEN_MAX];

int nAktionen = 0;

/// \todo has_gui should be replaced by a callback variable.
char has_gui = 0;

#ifdef WX
wxString sd1, sd2, sd3, sd4, sd5, sd6;

wxString Fmeldung;

#else
char sd1[100], sd2[100];

char Fmeldung[255];

#endif


// Aktionen ---------------------------------------------------------

void AktionenInit()
{
	nAktionen = 0;
}

#ifdef MUTWIN
/*void aktionen_message( char * Meldung )
{
  while ( aktionen_ausgabezeile >= PROT_MAXLINES ||
	 strlen(aktionen_string) + strlen(Meldung) > PROT_MAXCHARS - 2 )
  {
	 char *s1 = strstr(aktionen_string, "\n");
	 if ( s1 )
    {
      strcpy(aktionen_string, &(s1[2]));
      aktionen_ausgabezeile--;
    }
  }
  if ( aktionen_ausgabezeile ) strcat(aktionen_string, "\n");
  strcat(aktionen_string, Meldung);
  aktionen_ausgabezeile++;
  aktionen_changed = 1;
}*/

char * mutabor_do_aktion_to_string(struct do_aktion * action, bool symbolic_parameters) {
#warning "Symbolic parameters are not implemented yet"
	int tmp;
	bool hasName = (action->name && action->name[0]);
	char *tmpstr, *parameters = NULL;

	switch (action->aufruf_typ) {
	case aufruf_logik:
		if (action->u.aufruf_logik.einstimmung) {
			tmp = asprintf(&parameters,"%s%s",
				       (hasName?":":""),
				       mutabor_do_aktion_to_string(action->u.aufruf_logik.einstimmung,
								   symbolic_parameters));
			if (tmp < 0) parameters = NULL;
		}
		break;
	case aufruf_tonsystem:
		/* tone_system has no name :-(
		if (action->u.aufruf_tonsystem.tonsystem && !length) {
			parameters = strdup(action->u.aufruf_tonsystem.tonsystem->name);
		}
		*/
		break;
	case aufruf_umst_taste_abs:
		if (action->u.aufruf_umst_taste_abs.keynr) {
			tmp = asprintf(&parameters,"(%d)", *(action->u.aufruf_umst_taste_abs.keynr));
			if (tmp < 0) parameters = NULL;
		}
		break;
	case aufruf_umst_breite_abs:
		if (action->u.aufruf_umst_breite_abs.width) {
			tmp = asprintf(&parameters,"(%d)", *(action->u.aufruf_umst_taste_abs.keynr));
			if (tmp < 0) parameters = NULL;
		}
		break;
	case aufruf_umst_wiederholung_abs:
		tmp = asprintf(&parameters,"(%f ct)", ((double)action->u.aufruf_umst_wiederholung_abs.faktor)*100.0/(double)(0x1000000));
		if (tmp < 0) parameters = NULL;
		break;
	case aufruf_umst_wiederholung_rel:
		tmp = asprintf(&parameters,"(%+f ct)",
				      ((double)action->u.aufruf_umst_wiederholung_abs.faktor)*100.0/(double)(0x1000000));
		if (tmp < 0) parameters = NULL;
		break;
	case aufruf_umst_taste_rel:
		if (action->u.aufruf_umst_taste_rel.distance) {
			tmp = asprintf(&parameters,"(%c%d)",
					      action->u.aufruf_umst_taste_rel.rechenzeichen,
					      *(action->u.aufruf_umst_taste_rel.distance));
			if (tmp < 0) parameters = NULL;
		}
		break;
	case aufruf_umst_breite_rel:
		if (action->u.aufruf_umst_breite_rel.difference) {
			tmp = asprintf(&parameters,"(%c%d)",
					      action->u.aufruf_umst_breite_rel.rechenzeichen,
					      *(action->u.aufruf_umst_breite_rel.difference));
			if (tmp < 0) parameters = NULL;
		}
		break;
	case aufruf_umst_toene_veraendert:
		if (!hasName) {
			parameters = strdup(_("anonymous pitch change"));
		}
		break;
	case aufruf_umst_umst_bund:
		if (!hasName) {
			parameters = strdup(_("anonymous compound"));
		}
		break;
	case aufruf_umst_umst_case:
		if (action->u.aufruf_umst_umst_case.choice) {
			tmp = asprintf(&parameters,"(%d)", *(action->u.aufruf_umst_umst_case.choice));
			if (tmp < 0) parameters = NULL;
		}
		break;

	case aufruf_midi_out:
		if (action->u.aufruf_midi_out.out_liste) {
			/* todo: work against memory fragmentation */
			tmp = asprintf(&parameters,"#%x", action->u.aufruf_midi_out.out_liste->midi_code);
			if (tmp < 0) parameters = NULL;
			else for (midiliste * i = action->u.aufruf_midi_out.out_liste->next; i ; i=i->next) {
					tmpstr = parameters;
					tmp = asprintf(&parameters,"%s,#%x",tmpstr,i->midi_code);
					if (tmp < 0) {
						parameters = tmpstr;
						break;
					}
					free(tmpstr);
			}
			tmpstr=parameters;
			tmp = asprintf(&parameters,"(%s)",tmpstr);
			if (tmp < 0) {
				parameters = tmpstr;
				break;
			}
			free(tmpstr);
		}
		break;
	case aufruf_harmony_analysis:
		if (!hasName) 
			parameters = strdup(_("harmony analysis"));
	}
	if (!hasName && parameters) {
#if 0
		if ((tmp = strlen(action->name)) > size) {
			tmpstr = (char *)realloc(s,tmp*2);
			if (!tmpstr) {
				mutabor_out_of_memory(__FILE__,__LINE__,_("Cannot enhance action description."));
			} else {
				s = tmpstr;
				size = tmp*2;
				s[size-1]=0;
			}
		}
		strncpy(s,action->name,size-length-1);
		length += tmp;
#endif

		retval = parameters;
	} else if (hasName && parameters) {
		retval = asprintf("%s %s", 
				  action->name,
				  parameters);
		free(parameters);
	} else if (hasName) {
		retval = strdup(action->name);
	} else {
		retval = strdup(_("{anonymous action}");
	}

#if 0
	if (parameters) {
		tmp = strlen(parameters);
		if (length+tmp > size-1) {
			tmpstr = (char *)realloc(s,tmp+length+1);
			if (!tmpstr) tmp = size-length-1;
			else size = tmp+length+1;
		}
		strncpy(s+length,parameters,tmp);
		s[size-1]=0;
		free(parameters);
	}
	return s;
#endif
}



void AktionenMessage(mutabor_box_type * box, struct do_aktion * aktion)
{
	int l = 0,l2;
	char * name = mutabor_do_aktion_to_string(aktion,false);

	if (nAktionen >= AKTIONEN_MAX) return;

	boxAktionen[nAktionen] = box->id;

	if ( nAktionen )
		l = lAktionen[nAktionen-1];

	l2 = strlen(name);

	if (l < AKTIONEN_STRLEN)
		strncpy(&sAktionen[l], name, AKTIONEN_STRLEN-1-l);
	sAktionen[AKTIONEN_STRLEN-1]=0;

	if (l+l2 > AKTIONEN_STRLEN)
		lAktionen[nAktionen++] = AKTIONEN_STRLEN;
	else
		lAktionen[nAktionen++] = l+ l2;

}

int GetActString(unsigned char **box, int **l, char **s)

{
	int n = nAktionen;
	*box = boxAktionen;
	*l = lAktionen;
	*s = sAktionen;
	nAktionen = 0;
	return n;
}

// Compiler-Dialog --------------------------------------------------

#if 0
#ifdef WX
void InitCompDia(mutaborGUI::CompDlg *compDia, wxString filename)
{
	mutASSERT(compDia);
	CompDiaLine = compDia->GetLine();
}

#else
void InitCompDia(HWND compDiaLine)
{
	CompDiaLine = compDiaLine;
}

#endif

#else

#include <iostream.h>

extern ostream *Echo;

void aktionen_message( char * Meldung )
{
	*Echo << "\n  Action: " << Meldung;
}

#endif
#endif

#if 0
void calc_declaration_numbers(char withNames)
{
#ifdef MUTWIN
#ifdef WX
	sd1 = wxString::Format(_T("%d"), logik_list_laenge(list_of_logiken));
	sd2 = wxString::Format(_T("%d"), ton_list_laenge(list_of_toene));
	sd3 = wxString::Format(_T("%d"), umstimmungs_list_laenge(list_of_umstimmungen));
	sd4 = wxString::Format(_T("%d"), tonsystem_list_laenge(list_of_tonsysteme));
	sd5 = wxString::Format(_T("%d"), intervall_list_laenge(list_of_intervalle));
	sd6 = wxString::Format(_T("%ld"), anzahl_eingelesene_zeichen);
#else
	sprintf(sd1, "%d\n%d\n%d",
	        logik_list_laenge(list_of_logiken),
	        ton_list_laenge(list_of_toene),
	        umstimmungs_list_laenge(list_of_umstimmungen));
	sprintf(sd2, "%d\n%d\n%d",
	        tonsystem_list_laenge(list_of_tonsysteme),
	        intervall_list_laenge(list_of_intervalle),
	        anzahl_eingelesene_zeichen);
#endif
#else

	if ( withNames) {
		sprintf(sd1, "  logics: %d\n  tones: %d\n  tunings: %d",
		        logik_list_laenge(list_of_logiken),
		        ton_list_laenge(list_of_toene),
		        umstimmungs_list_laenge(list_of_umstimmungen));
		sprintf(sd2, "  tone systems: %d\n  intervalls; %d\n  read chars: %d",
		        tonsystem_list_laenge(list_of_tonsysteme),
		        intervall_list_laenge(list_of_intervalle),
		        anzahl_eingelesene_zeichen);
	} else {
		sprintf(sd1, "%d\n%d\n%d",
		        logik_list_laenge(list_of_logiken),
		        ton_list_laenge(list_of_toene),
		        umstimmungs_list_laenge(list_of_umstimmungen));
		sprintf(sd2, "%d\n%d\n%d",
		        tonsystem_list_laenge(list_of_tonsysteme),
		        intervall_list_laenge(list_of_intervalle),
		        anzahl_eingelesene_zeichen);
	}

#endif
}
#endif

#if 0
void show_line_number( int n )

{
#ifdef WX
	DEBUGLOG2(other,_T("No. %d in WX mode"),n);

	if ( !CompDiaLine ) return;

	wxString s = _T("");
	if ( n != -1 ) s << n;

	DEBUGLOG2(other,_T("s = %s"), s.c_str());

	CompDiaLine->SetLabel(s);
	CompDiaLine->Refresh();

#else
	DEBUGLOG2(other,_T("No. %d in non-WX mode"),n);
#ifdef MUTWIN
	if ( !CompDiaLine ) return;

	char s[20] = "---      ";

	if ( n != -1 ) sprintf(s, "%d       ", n);

	::SetWindowText(CompDiaLine, s);

#endif
#endif
}
#endif

#if 0
void fatal_error( int nr, ... )
{
	va_list arglist;
	va_start(arglist,nr);
#ifdef WX
	Fmeldung = wxString::Format(_("Error %d:"),nr) +
		wxString::FormatV(wxGetTranslation(Error_text[nr]), arglist);
#else
	vsprintf( Fmeldung, Error_text[nr], arglist );
#endif
	longjmp( weiter_gehts_nach_compilerfehler , (nr==0)?255:nr );
}

// Compiler - Warning -----------------------------------------------

void compiler_warning( int nr, ... )
{
	va_list arglist;
	va_start(arglist,nr);
#ifdef MUTWIN
	DEBUGLOGBASE (other,_T(""), _T("testing %s"), _T("%s"));
	DEBUGLOGBASE (other,_T(""),
	              _T("Compiler warning %d: %s"),
	              nr,
	              Warning_text[nr]);
#if defined(WX)
	if (has_gui) {
		wxMessageBox(wxString::FormatV(wxGetTranslation(Warning_text[nr]),
					       arglist),
			     _("Compiler warning"),
			     wxOK | wxICON_ASTERISK );
	} else {
		wxString out = wxString::FormatV(wxGetTranslation(Warning_text[nr]),
						 arglist);
		std::clog << "Compiler warning: " << (const char *)out.ToUTF8() << std::endl;
	}
#else
	char Fmeldung[255];
	vsprintf( Fmeldung, Warning_text[nr], arglist );
	MessageBox(0, Fmeldung, "Compiler warning", MB_OK | MB_ICONASTERISK );
#endif
#endif
}

void laufzeit_message(const char * Meldung )
{
	if ( strlen(protokoll_string) + strlen(Meldung) <= PROT_MAXCHARS - 2 ) {
		if ( protokoll_ausgabezeile ) strcat(protokoll_string, "\n");

		strcat(protokoll_string, Meldung);

		protokoll_ausgabezeile++;
	}
}
#endif
#if 0
/* must be somewhere else */
void init_laufzeit_protokoll( void )

{
	protokoll_ausgabezeile = 0;
	strcpy(protokoll_string, "");
}

void exit_laufzeit_protokoll( void )

{}

void laufzeit_protokoll(const char * formatstring , ... )
{
	char Fmeldung[255];
	va_list arglist;

	va_start(arglist, formatstring); /* hier 0 statt nr */
	vsprintf(Fmeldung, formatstring, arglist );
	laufzeit_message (Fmeldung);
}

#endif

#ifdef MUTWIN
#if 0

char* GetTSString(int box, char asTS)

{
	init_laufzeit_protokoll();

	if ( asTS )
		protokoll_aktuelles_tonsystem(box);
	else
		protokoll_aktuelle_relationen(box);

	return protokoll_string;
}
int GetLineNumbers()
{
	return protokoll_ausgabezeile;
}

#endif
#endif

#if 0
int GetErrorLine()
{
#if defined(WX)
	int i = Fmeldung.First(_("line"));
	long l;

	if ( i != -1 && Fmeldung.Mid(i+wxString(_("line")).Length()).ToLong(&l) )
		return l;
	else
		return -1;

#else
	char *s = strstr(Fmeldung, "Zeile");

	if ( !s ) return -1;

	return atol(&(s[5]));

#endif
}
#endif

#endif
///\}
