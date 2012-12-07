/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


// RTFUNNAM -- define names for run-time functions
//
// 91/11/04 -- J.W.Welch        -- defined

#define QSTRING( name ) __STR( name )
#define RTFUN( code, name ) QSTRING(CPPLIB(name))
#if _INTEL_CPU
#define RTDAT( code, name ) "_" QSTRING(CPPDATA(name))
#elif _CPU == _AXP
#define RTDAT( code, name ) QSTRING(CPPDATA(name))
#endif
#define RTFNC( code, name ) QSTRING(name)
#define RTFNP( code, name ) #name

static char *runTimeCodeString[] =
#include "rtfuns.h"

#undef QSTRING
