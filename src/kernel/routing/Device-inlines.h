/** \file               -*- C++ -*-
********************************************************************
* Inline routitnes for the device class
*
* $Header: /home/tobias/macbookbackup/Entwicklung/mutabor/cvs-backup/mutabor/mutabor/src/kernel/routing/Device-inlines.h,v 1.2 2011/11/02 14:31:57 keinstein Exp $
* Copyright:   (c) 2011 TU Dresden
* \author  Tobias Schlemmer <keinstein@users.berlios.de>
* \date 
* $Date: 2011/11/02 14:31:57 $
* \version $Revision: 1.2 $
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
*
********************************************************************
* \addtogroup route
* \{
********************************************************************/
// availlable groups: GUI, route, kernel, src/wxGUI, debug, docview

/* we guard a little bit complicated to ensure the references are set right
 */

#if (!defined(HEADERFILENAME) && !defined(PRECOMPILE))	\
	|| (!defined(HEADERFILENAME_PRECOMPILED))
#ifndef PRECOMPILE
#define HEADERFILENAME
#endif

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "src/kernel/Defs.h"
#include "src/kernel/routing/Device.h"

#ifndef HEADERFILENAME_PRECOMPILED
#define HEADERFILENAME_PRECOMPILED

// system headers which do seldom change

/// not for headers
#ifdef __BORLANDC__
#pragma hdrstop
#endif

namespace mutabor {

	template <class T, class P, class L>
	void CommonTypedDeviceAPI<T,P,L>::Destroy()  {
		TRACEC;
		DevicePtr self(static_cast<thistype *>(this));
		TRACEC;

		Route route (NULL);
		routeListType::iterator R;
			
		while ( (R = routes.begin()) != routes.end() ) {
			TRACEC;
			route = (*R);
			TRACEC;
			disconnect(route,self);
			TRACEC;
		}
			
		route = NULL;
		TRACEC;
		debug_destroy_class(this);
		RemoveFromDeviceList(static_cast<thistype *>(this));
		TRACEC;
	}

}

#endif // precompiled
#endif // header loaded


///\}
