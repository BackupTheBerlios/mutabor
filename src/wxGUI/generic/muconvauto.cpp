/** \file 
 ********************************************************************
 * Automatic Character set detection and conversion
 *
 * Copyright:   (c) 2008 TU Dresden
 * \author  Tobias Schlemmer <keinstein@users.berlios.de>
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
 * \addtogroup GUI
 * \{
 * \todo remove this file: a character set error implies that the whole file has to be reconverted.
 ********************************************************************/
///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/convauto.cpp
// Purpose:     implementation of wxConvAuto
// Author:      Vadim Zeitlin
// Created:     2006-04-04
// RCS-ID:      $Id: muconvauto.cpp,v 1.7 2011/11/02 14:32:01 keinstein Exp $
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "src/kernel/Defs.h"

#if wxUSE_WCHAR_T

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "muconvauto.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// ============================================================================
// implementation
// ============================================================================

/* static */

muConvAuto::BOMType muConvAuto::DetectBOM(const char *src, size_t srcLen)
{
	DEBUGLOGTYPE(other,muConvAuto,_T("Detecting BOM at length %d"),(int)srcLen);
	if (srcLen == wxNO_LEN) { // zero terminated string
		for (srcLen = 0; src[srcLen] && srcLen <= 4; srcLen++); 
	}
	if ( srcLen < 2 ) {
		// minimal BOM is 2 bytes so bail out immediately and simplify the code
		// below which wouldn't need to check for length for UTF-16 cases
		return BOM_None;
	}

	// examine the buffer for BOM presence
	//
	// see http://www.unicode.org/faq/utf_bom.html#BOM
	switch ( *(src++) ) {
	case '\0':
		// could only be big endian UTF-32 (00 00 FE FF)
		if ( srcLen >= 4 &&
		                src[0] == '\0' &&
		                src[1] == '\xfe' &&
		                src[2] == '\xff' ) {
			return BOM_UTF32BE;
		}
		break;

	case '\xfe':
		// could only be big endian UTF-16 (FE FF)
		if ( *src++ == '\xff' ) {
			return BOM_UTF16BE;
		}
		break;

	case '\xff':
		// could be either little endian UTF-16 or UTF-32, both start
		// with FF FE
		if ( *(src++) == '\xfe' ) {
			return srcLen >= 4 && src[0] == '\0' && src[1] == '\0'
			       ? BOM_UTF32LE
			       : BOM_UTF16LE;
		}
		break;

	case '\xef':
		// is this UTF-8 BOM (EF BB BF)?
		if ( srcLen >= 3 && src[0] == '\xbb' && src[1] == '\xbf' ) {
			return BOM_UTF8;
		}
		break;
	}

	return BOM_None;
}

void muConvAuto::InitFromBOM(BOMType bomType)
{
	TRACEC;
	m_consumedBOM = false;

	switch ( bomType ) {
	case BOM_UTF32BE:
		m_conv = new wxMBConvUTF32BE;
		m_ownsConv = true;
		break;

	case BOM_UTF32LE:
		m_conv = new wxMBConvUTF32LE;
		m_ownsConv = true;
		break;

	case BOM_UTF16BE:
		m_conv = new wxMBConvUTF16BE;
		m_ownsConv = true;
		break;

	case BOM_UTF16LE:
		m_conv = new wxMBConvUTF16LE;
		m_ownsConv = true;
		break;

	case BOM_UTF8:
		m_conv = &wxConvUTF8;
		m_ownsConv = false;
		break;

	default:
		wxFAIL_MSG( _T("unexpected BOM type") );
		// fall through: still need to create something

	case BOM_None:
		InitWithDefault();
		m_consumedBOM = true; // as there is nothing to consume
	}
}

void muConvAuto::SkipBOM(const char **src, size_t *len) const
{
	TRACEC;
	int ofs;
	switch ( m_bomType ) {
	case BOM_UTF32BE:
	case BOM_UTF32LE:
		ofs = 4;
		break;

	case BOM_UTF16BE:
	case BOM_UTF16LE:
		ofs = 2;
		break;

	case BOM_UTF8:
		ofs = 3;
		break;

	default:
		wxFAIL_MSG( _T("unexpected BOM type") );
		// fall through: still need to create something

	case BOM_None:
		ofs = 0;
	}

	*src += ofs;
	if ( *len != wxNO_LEN )
		*len -= ofs;
}

void muConvAuto::InitFromInput(const char **src, size_t *len)

{
	TRACEC;
	m_bomType = DetectBOM(*src, *len);
	InitFromBOM(m_bomType);
	SkipBOM(src, len);
}

size_t muConvAuto::ToWChar(wchar_t *dst, size_t dstLen,
                    const char *src, size_t srcLen) const
{
	TRACEC;
	// we check BOM and create the appropriate conversion the first time we're
	// called but we also need to ensure that the BOM is skipped not only
	// during this initial call but also during the first call with non-NULL
	// dst as typically we're first called with NULL dst to calculate the
	// needed buffer size
	muConvAuto *self = wx_const_cast(muConvAuto *, this);
	if ( !m_conv ) {
		self->InitFromInput(&src, &srcLen);
		if ( dst )
			self->m_consumedBOM = true;
	}

	if ( !m_consumedBOM && dst ) {
		self->m_consumedBOM = true;
		SkipBOM(&src, &srcLen);
	}

	size_t result = m_conv->ToWChar(dst, dstLen, src, srcLen);
	if (result != wxCONV_FAILED) return result;

	self->m_conv = m_fallback; // save for further use.
	self->m_ownsConv = false; // if we own fallback, it will be destroyed as fallback.
	return m_conv->ToWChar(dst, dstLen, src, srcLen);
}

size_t muConvAuto::FromWChar(char *dst, size_t dstLen,
                      const wchar_t *src, size_t srcLen) const
{
	TRACEC;
	if ( !m_conv ) {
		// default to UTF-8 for the multibyte output
		wx_const_cast(muConvAuto *, this)->InitWithDefault();
	}
	return m_conv->FromWChar(dst, dstLen, src, srcLen);
}

#endif // wxUSE_WCHAR_T

wxMBConv * muConvAutoFallback = &wxConvISO8859_1;

///\}
