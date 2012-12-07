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
* Description:  DOS specific trap I/O.
*
****************************************************************************/


#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include "tinyio.h"
#include "trpimp.h"

extern char RWBuff[];

extern int KeyPress_pragma( void );
extern int KeyGet_pragma( void );

#pragma aux KeyPress_pragma =   \
    "mov    ah,1"               \
    "int    16h"                \
    "jnz    l1"                 \
    "xor    ax,ax"              \
    "jmp    l2"                 \
    "l1:mov ax,1"               \
    "l2:"                       \
    modify [ax];

#pragma aux KeyGet_pragma =     \
    "mov    ah,0"               \
    "int    16h"                \
    modify [ax];


void Output( char *str )
{
    TinyWrite( TINY_ERR, str, strlen( str ) );
}

void SayGNiteGracey( int return_code )
{
    TinyTerminateProcess( return_code );
}

void StartupErr( char *err )
{
    Output( err );
    Output( "\r\n" );
    SayGNiteGracey( 1 );
}

int KeyPress( void )
{
    return( KeyPress_pragma() );
}

int KeyGet( void )
{
    return( KeyGet_pragma() );
}

tiny_handle_t PathOpen( char *name, unsigned name_len, char *exts )
{

    bool        has_ext;
    bool        has_path;
    char        *ptr;
    char        *endptr;
    char        trpfile[256];
    tiny_ret_t  rc;

    has_ext = FALSE;
    has_path = FALSE;
    endptr = name + name_len;
    for( ptr = name; ptr != endptr; ++ptr ) {
        switch( *ptr ) {
        case '.':
            has_ext = TRUE;
            break;
        case '/':
        case '\\':
            has_ext = FALSE;
                /* fall through */
        case ':':
            has_path = TRUE;
            break;
        }
    }
    memcpy( trpfile, name, name_len );
    if( has_ext ) {
        trpfile[name_len] = '\0';
    } else {
        trpfile[ name_len++ ] = '.';
        memcpy( trpfile + name_len, exts, strlen( exts ) + 1 );
    }
    if( has_path ) {
        rc = TinyOpen( trpfile, TIO_READ );
    } else {
        _searchenv( trpfile, "PATH", RWBuff );
        rc = TinyOpen( RWBuff, TIO_READ );
    }
    return( TINY_ERROR( rc ) ? (-1) : TINY_INFO( rc ) );
}

unsigned long GetSystemHandle( unsigned h )
{
    return( h );
}

unsigned FileClose( unsigned h )
{
    TinyClose( h );
    return( 0 );
}


int WantUsage( char *ptr )
{
    if( (*ptr == '-') || (*ptr == '/') ) ++ptr;
    return( *ptr == '?' );
}

