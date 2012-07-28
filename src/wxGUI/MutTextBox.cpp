/** \file 
 ********************************************************************
 * Textbox for Lists
 *
 * $Header: /home/tobias/macbookbackup/Entwicklung/mutabor/cvs-backup/mutabor/mutabor/src/wxGUI/MutTextBox.cpp,v 1.22 2011/11/02 14:31:59 keinstein Exp $
 * Copyright:   (c) 2008 TU Dresden
 * \author   R. Krauﬂe
 * Tobias Schlemmer <keinstein@users.berlios.de>
 * \date 2005/08/12
 * $Date: 2011/11/02 14:31:59 $
 * \version $Revision: 1.22 $
 * \license GPL
 *
 * $Log: MutTextBox.cpp,v $
 * Revision 1.22  2011/11/02 14:31:59  keinstein
 * fix some errors crashing Mutabor on Windows
 *
 * Revision 1.21  2011-09-30 18:07:05  keinstein
 * * make compile on windows
 * * s/wxASSERT/mutASSERT/g to get assert handler completely removed
 * * add ax_boost_base for boost detection
 *
 * Revision 1.20  2011-09-27 20:13:23  keinstein
 * * Reworked route editing backend
 * * rewireing is done by RouteClass/GUIRoute now
 * * other classes forward most requests to this pair
 * * many bugfixes
 * * Version change: We are reaching beta phase now
 *
 * Revision 1.19  2011-09-08 18:50:41  keinstein
 * Fix some further update bug
 *
 * Revision 1.18  2011-09-08 16:51:21  keinstein
 * Set foreground color in box status windows
 * Fix updating box status windows
 * update RtMidi (includes Jack compilation mode)
 *
 * Revision 1.17  2011-09-07 13:06:50  keinstein
 * Get rid of WinAttr and Fix window opening and closing
 *
 * Revision 1.16  2011-09-05 11:30:08  keinstein
 * Some code cleanups moving some global box arrays into class mutaborGUI::BoxData
 * Restore perspective on logic start
 *
 * Revision 1.15  2011-02-20 22:35:57  keinstein
 * updated license information; some file headers have to be revised, though
 *
 *
 *
 ********************************************************************
 * \addtogroup src/wxGUI
 * \{
 ********************************************************************/


// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "src/kernel/Defs.h"

#include <iostream>

#include "wx/tokenzr.h"

#include "src/wxGUI/MutTextBox.h"
#include "src/wxGUI/MutFrame.h"
#include "src/wxGUI/GUIBoxData.h"
#include "src/wxGUI/Routing/BoxShape.h"
#include "src/kernel/GrafKern.h"
#include "src/wxGUI/Action.h"


#ifdef __BORLANDC__
#pragma hdrstop
#endif

using mutaborGUI::BoxData;
using namespace mutaborGUI;

wxString TextBoxTitle[] =
        { N_("Current keys"), N_("Tone system"), N_("Actions") };
// needs wxGetTranslation();

BEGIN_EVENT_TABLE(MutTextBox, wxListBox)
        EVT_MENU(CM_UPDATEUI, MutTextBox::UpdateUI)
	EVT_CLOSE(MutTextBox::OnClose)
END_EVENT_TABLE()

wxString ListInit[1] = { _("<init>") };

MutTextBox::MutTextBox(WinKind k,
                       int boxId,
                       wxWindow* parent,
                       wxWindowID id,

                       const wxPoint& pos,
                       const wxSize& size):
		wxListBox(parent, id, pos, size, 1, ListInit),
		winKind(k),
		box(boxId)
{
	DEBUGLOG (other, _T(""));
	SetBackgroundStyle(wxBG_STYLE_COLOUR);
//	SetBackgroundColour(*wxWHITE);
	SetForegroundColour(BoxTextColour(box));
	SetBackgroundColour(BoxColour(box));
}


void MutTextBox::OnClose(wxCloseEvent& event)

{
	mutUnused(event);
	mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);
	DEBUGLOG (other, _T("winKind: %d"), winKind);
        BoxData & boxdata = BoxData::GetBox(box);
	if ( LogicOn ) {
                switch (winKind) {
                case WK_KEY: 
                        boxdata.WantKeyWindow(false);
                        break;
                case WK_TS: 
                        boxdata.WantTonesystemWindow(false);
                        break;
                case WK_ACT: 
                        boxdata.WantActionsWindow(false);
                        break;
                case WK_LOGIC:
                        wxLogWarning(_("Unexpected value: WK_LOGIC"));
			UNREACHABLEC;
                        break;
                case WK_ROUTE:
                        wxLogWarning(_("Unexpected value: WK_ROUTE"));
			UNREACHABLEC;
                        break;
                case WK_EDIT:
                        wxLogWarning(_("Unexpected value: WK_EDIT"));
			UNREACHABLEC;
                        break;
                case WK_NULL:
                        wxLogWarning(_("Unexpected value: WK_NULL"));
			UNREACHABLEC;
                        break;
                default:
                        wxLogError(_("Unexpected window kind: %d"), winKind);
			UNREACHABLEC;
                }
        } 
        Destroy();
}


// Updaten
void MutTextBox::UpdateUI(wxCommandEvent& WXUNUSED(event)) 
{
               switch (winKind) {
                case WK_KEY: 
                        NewText(GetKeyString(box, asTS));
                        break;
                case WK_TS: 
                        NewText(GetTSString(box, asTS));
                        break;
                case WK_ACT:
                        TakeOverActions();
                        if (CAW) {
                                NewText(GenerateCAWString());
                        } else {
                                NewText(GenerateACTString(box));
                        }
                        break;
                case WK_LOGIC:
                        wxLogWarning(_("Unexpected value: WK_LOGIC"));
			UNREACHABLEC;
                        break;
                case WK_ROUTE:
                        wxLogWarning(_("Unexpected value: WK_ROUTE"));
			UNREACHABLEC;
                        break;
                case WK_EDIT:
                        wxLogWarning(_("Unexpected value: WK_EDIT"));
			UNREACHABLEC;
                        break;
                case WK_NULL:
                        wxLogWarning(_("Unexpected value: WK_NULL"));
			UNREACHABLEC;
                        break;
                default:
                        wxLogError(_("Unexpected window kind: %d"), winKind);
			UNREACHABLEC;
                }        
}

void MutTextBox::NewText(char *s, bool newTitle)
{
	mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);
	DEBUGLOG (other, _T(""));
	// Text in Liste
	Clear();
	char s1[2000];
	int i = 0, j = 0;

	while ( s[i] ) {
		if ( s[i] == '\r' || s[i] == '\n' ) {
			s1[j] = 0;

			if ( j )
				Append(wxString(muT(s1)));

			j = 0;

			i++;
		} else
			s1[j++] = s[i++];
	}

	s1[j] = 0;

	if ( j )
		Append(wxString(muT(s1)));

	// Scrollen, falls Aktion-Window
	if ( winKind == WK_ACT )
		SetSelection(GetCount()-1);

	// Farbbalken ggf. korrigieren
	/*  if ( GetTopIndex() == 0 && PARENT_KIND != WK_ACT )
	  {
	    ColorBar1->MoveWindow(1, 1, 1000, 1, true);
	    ColorBar2->MoveWindow(1, 1, 1, 1000, true);
	  }*/
}

void MutTextBox::NewText(const wxString &s, bool newTitle)
{
	DEBUGLOG (other, _T("s=%s; newTitle=%d; winKind=%d"),s.c_str(),newTitle,winKind);
	mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);

	Freeze();
	// Text in Liste
	Clear();

	wxStringTokenizer tokenizer(s,_T("\r\n"));

	while (tokenizer.HasMoreTokens())
		Append(tokenizer.GetNextToken());
	Thaw();


	// Scrollen, falls Aktion-Window
	if ( winKind == WK_ACT )
		SetSelection(GetCount()-1);

	// Farbbalken ggf. korrigieren
	/*  if ( GetTopIndex() == 0 && PARENT_KIND != WK_ACT )
	  {
	    ColorBar1->MoveWindow(1, 1, 1000, 1, true);
	    ColorBar2->MoveWindow(1, 1, 1, 1000, true);
	  }*/
}


wxString MutTextBox::MakeTitle() const {
	if (winKind == WK_ACT && CAW) {
		return _("Actions - all boxes");
	} else {
		return wxString::Format(wxGetTranslation(_("%s - Box %d")),
					(const wxChar *)wxGetTranslation(TextBoxTitle[winKind]),
					box);
	}
}


/*wx void MutTextBox::EvSize(uint sizeType, TSize &size)
{
  TListBox::EvSize(sizeType, size);
  TRect R = GetWindowRect();
  TRect RC = ColorBar1->GetWindowRect();
  if ( GetTopIndex() == 0 )
  {
    ColorBar1->MoveWindow(1, 1, 1000, 1, true);
    ColorBar2->MoveWindow(1, 1, 1, 1000, true);
  }
}
*/

/*wx void MutTextBox::EvChar(uint key, uint repeatCount, uint flags)
{
  TListBox::EvChar(key, repeatCount, flags);
  if ( 'A' <= key && key <= 'z' || key == VK_TAB
       || key == VK_UP || key == VK_DOWN
       || key == VK_LEFT || key == VK_RIGHT )
  {
    pubTaste = key;
    if ( IsOpen(WK_LOGIC, Box()) )
      GetWinAttr(WK_LOGIC, Box())->Win->SendMessage(WM_COMMAND, CM_TASTE);
    else
      KeyboardAnalyseSimple(Box(), key);
  }
}*/

/*wx void MutTextBox::EvSetFocus(HWND hWndLostFocus)
{
  TListBox::EvSetFocus(hWndLostFocus);
}

void
MutTextBox::EvRButtonDown(uint, TPoint& point)
{
  RightButtonMenu(this, point);
}*/




///\}