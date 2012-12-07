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


#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "wdebug.h"
#include "packet.h"
#include "trperr.h"

static _dword _id;
#ifdef DEBUG_ME
static int _aa=10,_bb=12;
static int _a=0,_b=2;
void Blip( int *a, int *b, char a1 )
{
char far *s1 = (char far *) 0xb8000000;
char far *s2 = (char far *) 0xb0000000;
int c;

    s1[*a] = a1;
    s1[*b] = ' ';
    s2[*a] = a1;
    s2[*b] = ' ';
    c = *a;
    *a = *b;
    *b = c;

}
#endif

unsigned RemoteGet( char *rec, unsigned len )
{
unsigned long rc;

#ifdef __WINDOWS__
    while( 1 ) {
        rc = ConvGet( _id, rec, len, NO_BLOCK );
        if( (rc & 0xffff) == BLOCK ) SetExecutionFocus( _id );
        else break;
    }
#else
#ifdef DEBUG_ME
    Blip( &_a, &_b, 'G' );
#endif
    rc = ConvGet( _id, rec, len, BLOCK );
#ifdef DEBUG_ME
    Blip( &_a, &_b, 'g' );
#endif
#endif
    return( rc >> 16 );
}

unsigned RemotePut( char *rec, unsigned len )
{
#ifdef __WINDOWS__
int rc;
    while( 1 ) {
        rc = ConvPut( _id, rec, len, NO_BLOCK );
        if( rc == BLOCK ) SetExecutionFocus( _id );
        else break;
    }
#else
#ifdef DEBUG_ME
    Blip( &_aa, &_bb, 'T' );
#endif
    ConvPut( _id, rec, len, BLOCK );
#ifdef DEBUG_ME
    Blip( &_aa, &_bb, 't' );
#endif
#endif
    return( len );
}


char RemoteConnect( void )
{
int rc;
#ifdef SERVER
    rc = LookForConv( &_id );
    if( rc == 1 ) {
        return( 1 );
    } else if( rc == 0 ) {
        TimeSlice();
    }
    return( 0 );
#else
static int _first=1;
    if( _first ) {

        _first = 0;
        rc = StartConv( _id );
        if( rc != 0 ) {
        }
    }
    for( ;; ) {
        rc = IsConvAck( _id );
        if( !rc ) {
            TimeSlice();
        } else if( rc < 0 ) {
        } else {
            return( 1 );
        }
    }
#endif
}

void RemoteDisco( void )
{
#ifdef SERVER
#else
    EndConv( _id );
#endif
}

char    LinkName[80];
char    DefLinkName[] = "WinLink";

char *RemoteLink( char *name, char server )
{
    int     i;
    int     rc;

    server = server;
    if( name == NULL || *name == '\0' ) name = DefLinkName;
    i = 0;
    for( ;; ) {
        if( i >= sizeof( LinkName ) ) break;
        if( *name == '\0' ) break;
        LinkName[i++] = *name++;
    }
    LinkName[i] = '\0';
#ifdef SERVER
    rc = RegisterName( LinkName );
    if( rc < 0 ) {
        UnregisterName( LinkName );
        rc = RegisterName( LinkName );
        if( rc < 0 ) {
            return( TRP_ERR_CANT_REGISTER_SERVER );
        }
    }
#else
    rc = AccessName( LinkName, &_id );
    if( rc < 0 ) return( TRP_ERR_CANT_FIND_SERVER );
#endif
    return( NULL );
}


void RemoteUnLink( void )
{
#ifdef SERVER
        UnregisterName( LinkName );
#else
        UnaccessName( LinkName );
#endif
}
