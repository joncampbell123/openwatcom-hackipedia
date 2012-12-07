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
* Description:  Internal memory management with optional tracking.
*
****************************************************************************/


#include <stdio.h>
#include <stddef.h>
#include "standard.h"
#include "hostsys.h"
#include "memsydep.h"
#include "memcheck.h"
#include "zoiks.h"
#include "cg.h"
#include "spawn.h"
#include "memout.h"
#include "cgmem.h"
#if _HOST_MEMORY & _FULL_TRACKING
    #include "trmem.h"
#endif

extern    int   InOptimizer;

extern  void            CalcMemLimit( void );
extern  void            FatalError(char *);
extern  bool            GetEnvVar(char*,char*,int);


static          mem_out_action  MemOut;

#if _HOST_MEMORY & _FULL_TRACKING

extern  void            DumpChar(char);
extern  void            DumpNL(void);
_trmem_hdl              Handle;

static void Prt( int * handle, const char * buff, size_t len )
/************************************************************/
{
    size_t i;

    handle=handle;
    for( i = 0; i < len; ++i ) fputc( *buff++, stderr );
}

#elif _HOST_MEMORY & _CHUNK_TRACKING
static          uint    Chunks;
#endif

extern  void    CGMemInit( void )
/*******************************/
{
    _SysReInit();
    MemOut = MO_FATAL;
    #if _HOST_MEMORY & _FULL_TRACKING
        Handle = _trmem_open( &_SysAlloc, &_SysFree, NULL, NULL, NULL, &Prt,
                                _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
                                _TRMEM_REALLOC_NULL | _TRMEM_FREE_NULL |
                                _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
    #elif _HOST_MEMORY & _CHUNK_TRACKING
        Chunks = 0;
    #endif
    CalcMemLimit();
}

extern  void    CGMemFini( void )
/*******************************/
{
    #if _HOST_MEMORY & _FULL_TRACKING
        {
            char        buff[80];

            if( !GetEnvVar( "TRQUIET", buff, 7 ) ) {
                _trmem_prt_list( Handle );
            }
            _trmem_close( Handle );
        }
    #elif _HOST_MEMORY & _CHUNK_TRACKING
        if( Chunks != 0 ) {
            _Zoiks( ZOIKS_002 );
        }
    #endif
    MemFini();
}


extern  mem_out_action    SetMemOut( mem_out_action what )
/********************************************************/
{
    mem_out_action      old;

    old = MemOut;
    MemOut = what;
    return( old );
}

extern  pointer CGAlloc( unsigned size )
/**************************************/
{
    pointer     chunk;

    _MemLow;
    for( ;; ) {
        #if _HOST_MEMORY & _FULL_TRACKING
            chunk = _trmem_alloc( size, _trmem_guess_who(), Handle );
        #else
            chunk = _SysAlloc( size );
        #endif
        if( chunk != NULL ) {
            #if _HOST_MEMORY & _CHUNK_TRACKING
                ++Chunks;
            #endif
            _AlignmentCheck( chunk, 8 );
            return( chunk );
        }
        if( _MemCheck( size ) == FALSE ) break;
    }
    if( ( MemOut == MO_FATAL ) | ( InOptimizer != 0 ) ) {
        FatalError( "Out of memory" );
    } else if( MemOut == MO_SUICIDE ) {
        Suicide();
    }
    return( NULL );
}


extern  void    CGFree( pointer chunk )
/*************************************/
{
    #if _HOST_MEMORY & _FULL_TRACKING
        _trmem_free( chunk, _trmem_guess_who(), Handle );
    #else
        #if _HOST_MEMORY & _CHUNK_TRACKING
            --Chunks;
        #endif
        _SysFree( chunk );
    #endif
}


#if _HOST_MEMORY & _FULL_TRACKING
extern  void    DumpMem( void )
/*****************************/
{
    _trmem_prt_usage( Handle );
}
#endif
