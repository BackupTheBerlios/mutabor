/** \file   -*- C++ -*-
 ***********************************************************************
 * Input device selection dialog.
 *
 * Copyright:   (c) R. Krauße, TU Dresden, 
 *              2013 Tobias Schlemmer
 * \author R. Krauße <krausze@users.berlios.de>
 *         Tobias Schlemmer <keinstein@users.berlios.de>
 * \license: GPL
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
 *\addtogroup GUIroute
 *\{
 ********************************************************************/

/* we guard a little bit complicated to ensure the references are set right
 */

#if (!defined(MUWX_ROUTING_OUTPUTDEVDLG_H) && !defined(PRECOMPILE)) \
	|| (!defined(MUWX_ROUTING_OUTPUTDEVDLG_H_PRECOMPILED))
#ifndef PRECOMPILE
#define MUWX_ROUTING_OUTPUTDEVDLG_H
#endif

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "src/kernel/Defs.h"
#include "src/wxGUI/resourceload.h"
#include "src/kernel/routing/Device.h"

#ifndef MUWX_ROUTING_OUTPUTDEVDLG_H_PRECOMPILED
#define MUWX_ROUTING_OUTPUTDEVDLG_H_PRECOMPILED

// system headers which do seldom change

/*!
 * Includes
 */

#include "wx/xrc/xmlres.h"
#include "wx/html/htmlwin.h"
#include "wx/statline.h"
#include "wx/filepicker.h"
#include "wx/valgen.h"
#include "wx/valtext.h"

namespace mutaborGUI {

	class OutputDevDlg: public OutputDevDlgBase
	{
		DECLARE_DYNAMIC_CLASS( OutputDevDlg )
		DECLARE_EVENT_TABLE()

			protected:
			wxSizer *Container,* TypeBox, *PortBox, *MidiFileBox, *GuidoFileBox;

		struct TypeData:wxClientData
		{
			mutabor::DevType nr;
			TypeData(mutabor::DevType i):wxClientData()
				{
					nr=i;
				}

			bool operator == (mutabor::DevType i)
				{
					if (this)
						return i == nr;
					else return false;
				}

			operator mutabor::DevType()
				{
					if (this) return nr;
					else return mutabor::DTNotSet;
				}
		};

		int FindType (mutabor::DevType t);

	public:
		/// Constructors
		OutputDevDlg( wxWindow* parent = NULL);

		/// wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_CHOICE
		void OnChoiceSelected( wxCommandEvent& event );

		/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
		void OnRemoveClick( wxCommandEvent& event );

		void OnFileChanged ( wxFileDirPickerEvent & event );

		void UpdateLayout(mutabor::DevType type);

		int GetMidiDevice() const
			{
				return PortChoice->GetSelection() ;
			}

		void SetMidiDevice(int value)

			{
				DEBUGLOG(other, _T("%d"),value);
				PortChoice->SetSelection (value) ;
				Update();
			}

		wxString GetMidiFile() const
			{
				return MidiFilePicker->GetPath() ;
			}

		void SetMidiFile(wxString value)

			{
				DEBUGLOG(other, value);
				MidiFilePicker->SetPath(value);
				InvalidateBestSize();
				SetInitialSize(wxDefaultSize);
				Update();
				DEBUGLOG(other, _T("done"));
			}

		wxString GetGUIDOFile() const
			{
				return GuidoFilePicker->GetPath() ;
			}

		void SetGUIDOFile(wxString value)

			{
				DEBUGLOG(other, value);
				GuidoFilePicker->SetPath(value);
				Update();
			}

		mutabor::DevType GetType() const
			{
				int Type = DeviceChoice->GetSelection();
				if (Type == wxNOT_FOUND) return mutabor::DTNotSet;

				mutASSERT (dynamic_cast<TypeData *>(DeviceChoice->GetClientObject(Type)));
				TypeData * obj = (TypeData *)DeviceChoice->GetClientObject(Type);

				if (obj) return  *obj;
				else return mutabor::DTNotSet;
			}

		void SetType(mutabor::DevType value)

			{
				DEBUGLOG(other, _T("%d"),value);
				UpdateLayout(value);
				Update();
			}

		long GetMidiBendingRange() {
			return MidiBendingRange->GetValue();
		}

		void SetMidiBendingRange (long i) {
			MidiBendingRange->SetValue (i);
		}

		long GetMidiFileBendingRange() {
			return MidiFileBendingRange->GetValue();
		}

		void SetMidiFileBendingRange (long i) {
			MidiFileBendingRange->SetValue (i);
		}

		wxString GetPortString(int i)
			{
				return PortChoice->GetString (i);
			}

		void AppendPortChoice (const wxString &s)
			{
				PortChoice->Append (s);
			}

		/// Retrieves bitmap resources
		wxBitmap GetBitmapResource (const wxString& name );

		/// Retrieves icon resources
		wxIcon GetIconResource (const wxString& name );

		/// Should we show tooltips?
		static bool ShowToolTips ();
	};
}
#endif
// _OUTPUTDEVDLG_H_PRECOMPILED
#endif
// _OUTPUTDEVDLG_H_

/*
 * \}
 */
