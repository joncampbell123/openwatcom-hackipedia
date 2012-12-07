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


#ifndef __DBGMEM_INCLUDED
#define __DBGMEM_INCLUDED

#include "walloca.h"
#ifndef __alloca
#define __alloca(s)     _alloca( __ALLOCA_ALIGN( s ) )
#endif

extern void     *DbgRealloc( void *, unsigned );
extern void     *ChkAlloc( unsigned, char * );
extern void     *DbgAlloc( unsigned );
extern void     *DbgMustAlloc( unsigned );
extern void     DbgFree( void * );
extern void     MemInit( void );
extern void     MemFini( void );

#define _ChkAlloc( res, size, type )    res = ChkAlloc( size, type )
#define _Realloc( res, size )           res = DbgRealloc( res, size )
#define _Alloc( res, size )             res = DbgAlloc( size )
#define _AllocA( res, size )            res = __alloca( size )
#define _Free( ptr )                    DbgFree( ptr )

#endif
