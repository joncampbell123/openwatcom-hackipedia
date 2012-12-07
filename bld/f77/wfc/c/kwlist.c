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
* Description:  list of FORTRAN statement keywords
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"

// Note: If you wish to add a new statement processor, there are four files
//       that need changing: PRDEFN.H, KWLIST.C, PROCTBL.C, CFTABLE.C

// Note: The keyword DO is contained in DOUBLEPRECISION; any time this is
//       the case, the smaller keyword must come first to enforce a match.

#ifdef pick
#undef pick
#endif

#define pick(id,text,proc,flags) text,

char    *StmtKeywords[] = {

#include "stmtdefn.h"

};
