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
* Description:  Accessor routine for address of _pgmptr global.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>

/* Temp Hack Alert! The conditional below needs to be removed once 1.8
 * is released (same with wpgmptr.c). ../register.def and ../stack.def
 * need to be modified to uncomment _pgmptr and _wpgmptr references.
 */
#if __WATCOMC__ < 1280
_WCRTDATA extern char _WCI86FAR *_LpPgmName;
#undef  _pgmptr
#define _pgmptr _LpPgmName
#endif

_WCRTLINK char _WCI86FAR **__get_pgmptr_ptr( void )
{
    return( &_pgmptr );
}
