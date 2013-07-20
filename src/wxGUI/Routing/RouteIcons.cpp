// -*-C++ -*-
/** \file
 ********************************************************************
 * Devices base classes. Icons used in route window.
 *
 * \author Rüdiger Krauße <krausze@mail.berlios.de>,
 * Tobias Schlemmer <keinstein@users.berlios.de>
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
 *\addtogroup route
 *\{
 ********************************************************************/
#include "src/wxGUI/Routing/RouteIcons.h"
#include "src/kernel/routing/Route-inlines.h"
#include "src/wxGUI/Routing/BoxShape.h"
#include "src/wxGUI/MutApp.h"

#include "wx/image.h"
#include "Images/Icons/xpm/DevUnknown.xpm"

namespace mutaborGUI {

	MutIcon DevUnknownBitmap;
	MutIcon MidiInputDevBitmap;
	MutIcon NewInputDevBitmap;
	MutIcon MidiOutputDevBitmap;
	MutIcon NewOutputDevBitmap;
	MutIcon MidiFileBitmap;
	MutIcon GuidoFileBitmap;
	MutIcon BoxBitmap;
	MutIcon NewBoxBitmap;
	MutIcon ActiveChannelBitmap;
	MutIcon PassiveChannelBitmap;


	static void initMutIcon(MutIcon & icon, 
				const wxString & filename,
				const wxBitmapType type) 
	{
		if (!icon.IsOk())
			if(!icon.LoadFile(filename,type))
				icon = DevUnknownBitmap;
	}

	bool initMutIconShapes() 
	{
		DevUnknownBitmap = MutICON(wxIcon(devunknown_xpm));
		if (!DevUnknownBitmap.IsOk()) return false;
  
		initMutIcon(MidiInputDevBitmap,
			    wxGetApp().GetResourceName (_T ("InputDevice.png")),
			    wxBITMAP_TYPE_PNG);
		initMutIcon(NewInputDevBitmap,
			    wxGetApp ().GetResourceName (_T ("NewInputDevice.png")),
			    wxBITMAP_TYPE_PNG);
		initMutIcon(MidiOutputDevBitmap,
			    wxGetApp ().GetResourceName (_T ("OutputDevice.png")),
			    wxBITMAP_TYPE_PNG);
		initMutIcon(NewOutputDevBitmap,
			    wxGetApp ().GetResourceName (_T ("NewOutputDevice.png")),
			    wxBITMAP_TYPE_PNG);
		initMutIcon(MidiFileBitmap,
			    wxGetApp ().GetResourceName (_T ("MidiFile.png")),
			    wxBITMAP_TYPE_PNG);
		initMutIcon(GuidoFileBitmap,
			    wxGetApp ().GetResourceName (_T ("GuidoFile.png")),
			    wxBITMAP_TYPE_PNG);
		initMutIcon(BoxBitmap,
			    wxGetApp ().GetResourceName (_T ("TuningBox.png")),
			    wxBITMAP_TYPE_PNG);
		initMutIcon(ActiveChannelBitmap,
			    wxGetApp ().GetResourceName (_T ("ActiveChannel.png")),
			    wxBITMAP_TYPE_PNG);
		initMutIcon(PassiveChannelBitmap,
			    wxGetApp ().GetResourceName (_T ("PassiveChannel.png")),
			    wxBITMAP_TYPE_PNG);
		initMutIcon(NewBoxBitmap,
			    wxGetApp ().GetResourceName (_T ("TuningBox.png")),
			    wxBITMAP_TYPE_PNG);
		initBoxColours();	
		return true;
	}
}

/*
 * \}
 */
