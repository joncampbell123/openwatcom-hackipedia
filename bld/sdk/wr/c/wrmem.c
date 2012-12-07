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
* Description:  WR memory manipulation routines
*
****************************************************************************/


#include <windows.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include "wrglbl.h"
#include "wrmemi.h"
#include "wrmem.h"

#ifdef TRMEM

#include "trmem.h"

static _trmem_hdl   TRMemHandle;
static int          TRFileHandle;   /* stream to put output on */
static void TRPrintLine( int *, const char * buff, size_t len );

/* extern to avoid problems with taking address and overlays */
extern void TRPrintLine( int * handle, const char * buff, size_t len )
/********************************************************************/
{
    write( *handle, buff, len );
}

#endif

void WRMemOpen ( void )
{
#ifdef TRMEM
    TRFileHandle = STDERR_FILENO;
    TRMemHandle = _trmem_open( malloc, free, realloc, _expand,
            &TRFileHandle, TRPrintLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
#endif
}

void WRMemClose ( void )
{
#ifdef TRMEM
    _trmem_close( TRMemHandle );
#endif
}

void *WRWResMemAlloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

void WRWResMemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
#else
    free( ptr );
#endif
}

void * WR_EXPORT WRMemAlloc( size_t size )
{
#ifdef TRMEM
    return( _trmem_alloc( size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( malloc( size ) );
#endif
}

void WR_EXPORT WRMemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
#else
    free( ptr );
#endif
}

void * WR_EXPORT WRMemRealloc ( void *ptr, size_t size )
{
#ifdef TRMEM
    return( _trmem_realloc( ptr, size, _trmem_guess_who(), TRMemHandle ) );
#else
    return( realloc( ptr, size ) );
#endif
}

int WR_EXPORT WRMemValidate ( void *ptr )
{
#ifdef TRMEM
    return( _trmem_validate( ptr, _trmem_guess_who(), TRMemHandle ) );
#else
    _wtouch(ptr);
    return ( TRUE );
#endif
}

int WR_EXPORT WRMemChkRange ( void *start, size_t len )
{
#ifdef TRMEM
    return( _trmem_chk_range( start, len, _trmem_guess_who(), TRMemHandle ) );
#else
    _wtouch(start);
    _wtouch(len);
    return ( TRUE );
#endif
}

void WR_EXPORT WRMemPrtUsage ( void )
{
#ifdef TRMEM
    _trmem_prt_usage( TRMemHandle );
#endif
}

unsigned WR_EXPORT WRMemPrtList ( void )
{
#ifdef TRMEM
    return( _trmem_prt_list( TRMemHandle ) );
#else
    return ( 0 );
#endif
}

/* functions to replace those in mem.c in SDK/MISC */

void MemStart( void )
{
#ifndef __386__
#ifndef __ALPHA__
    __win_alloc_flags   = GMEM_MOVEABLE | GMEM_SHARE;
    __win_realloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
#endif
#endif
}

void *MemAlloc( size_t size )
{
    void *p;

#ifdef TRMEM
    p = _trmem_alloc( size, _trmem_guess_who(), TRMemHandle );
#else
    p = malloc( size );
#endif

    if( p ) {
        memset( p, 0, size );
    }

    return( p );
}

void *MemReAlloc( void *ptr, size_t size )
{
    void *p;

#ifdef TRMEM
    p = _trmem_realloc( ptr, size, _trmem_guess_who(), TRMemHandle );
#else
    p = realloc( ptr, size );
#endif

    return( p );
}

void MemFree( void *ptr )
{
#ifdef TRMEM
    _trmem_free( ptr, _trmem_guess_who(), TRMemHandle );
#else
    free( ptr );
#endif
}

