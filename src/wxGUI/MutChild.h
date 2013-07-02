/** \file                 -*- C++ -*-
 *
 * $Header: /home/tobias/macbookbackup/Entwicklung/mutabor/cvs-backup/mutabor/mutabor/src/wxGUI/MutChild.h,v 1.16 2011/11/02 14:31:58 keinstein Exp $
 * Copyright:   (c) 2005,2006,2007-2011 TU Dresden
 * \author Rüdiger Krauße <krausze@mail.berlios.de>
 * Tobias Schlemmer <keinstein@users.berlios.de>
 * \date $Date: 2011/11/02 14:31:58 $
 * \version $Revision: 1.16 $
 * \license GPL
 *
 *
 ********************************************************************/

#if (!defined(MUWX_MUT_CHILD_H) && !defined(PRECOMPILE)) \
	|| (!defined(MUWX_MUT_CHILD_H_PRECOMPILED))
#ifndef PRECOMPILE
#define MUWX_MUT_CHILD_H
#endif

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "src/kernel/Defs.h"
#include "src/wxGUI/generic/mhDefs.h"
#include "src/wxGUI/MutTextBox.h"

#ifndef MUWX_MUT_CHILD_H_PRECOMPILED
#define MUWX_MUT_CHILD_H_PRECOMPILED

#include "wx/toolbar.h"
#include "wx/dynarray.h"
#include "wx/arrimpl.cpp" // this is a magic incantation which must be done!
#include "wx/aui/aui.h"

namespace mutaborGUI {

	class MutFrame;

	class MutChild: public MutTextBox
	{

	public:
		MutChild (WinKind winkind,
			  int boxId,
			  wxWindow * parent= NULL,
			  wxWindowID id = -1,

			  const wxPoint& pos = wxDefaultPosition,
			  const wxSize & size = wxDefaultSize);

		~MutChild();

		void OnActivate(wxActivateEvent& event);

//    void OnRefresh(wxCommandEvent& event);
//    void OnUpdateRefresh(wxUpdateUIEvent& event);
//    void OnQuit(wxCommandEvent& event);
//    void OnSize(wxSizeEvent& event);
//    void OnMove(wxMoveEvent& event);

		void deleteFromWinAttrs();

		void OnClose(wxCloseEvent& event)
		{
			mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);
			TRACEC;
			deleteFromWinAttrs();
			MutTextBox::OnClose(event);
		}


		void OnAuiClose(wxAuiManagerEvent& event)

		{
			mutUnused(event);
			mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);
			TRACEC;
			deleteFromWinAttrs();
		}

		// Override sizing for drawing the color

		void GetClientSize(int * width, int * height)
		{
			mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);
			MutTextBox::GetClientSize(width,height);

			if ((width += 2) < 0) width = 0;

			if ((height +=2) < 0) height =0;
		}

		void SetClientSize(int width, int height)
		{
			mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);
			MutTextBox::SetClientSize(width-2, height-3);
		}

		void SetClientSize(const wxSize& size)
		{
			mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);
			wxSize s = size;
			s.IncBy(-2);
			MutTextBox::SetClientSize(s);
		}

		void ClientToScreen(int * x, int * y )

		{
			mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);
			MutTextBox::ClientToScreen(x,y);
			x+=1;
			y+=1;
		}

		wxPoint ClientToScreen(const wxPoint& pt) const
		{
			mutASSERT(WK_KEY <= winKind && winKind < WK_NULL);
			return MutTextBox::ClientToScreen(pt)+wxPoint(1,1);
		}


		void OnChar(wxKeyEvent& event);

 		void OnGetFocus(wxFocusEvent& event);

//	void MenuPassOn(wxCommandEvent& event);
//	void MenuPassToParent(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
	};

	bool IsOpen(WinKind kind, int box = 0);

	bool IsWanted(WinKind kind, int box = 0);

	void DontWant(WinKind kind, int box = 0);

	int NumberOfOpen(WinKind kind);
}
#endif
#endif

///\}
