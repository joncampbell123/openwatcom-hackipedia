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
* Description:  for error messages in resource files
*
****************************************************************************/


#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include "cioconst.h"
#include "wressetr.h"
#include "wreslang.h"

static  HANDLE_INFO     hInstance = { 0 };
static  char            ResFlags = { 0 };
static  unsigned        MsgShift;

#define RF_OPENED       0x01            // Resource file opened
#define RF_INITIALIZED  0x02            // Resource system initialized

static  long    res_seek( int handle, long position, int where ) {
// Fool the resource compiler into thinking that the resource information
// starts at offset 0.
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

// Client routines setup for wres library
WResSetRtns( open, close, read, write, res_seek, tell, malloc, free );

int     LoadMsg( unsigned int msg, char *buffer, int buff_size ) {
// Load a message into the specified buffer.  This function is called
// by WLINK when linked with 16-bit version of WATFOR-77.
    if( !(ResFlags & RF_INITIALIZED) ) return( 0 );
    return( !LoadString( &hInstance, msg+MsgShift, buffer, buff_size ) );
}


static  void    BldErrMsg( unsigned int err, char *buffer, va_list args ) {
// Build error message.
    *buffer = NULLCHAR;
    if( LoadMsg( err, &buffer[1], ERR_BUFF_SIZE-1 ) ) {
        buffer[0] = ' ';
        Substitute( buffer, buffer, args );
    }
}

static  void    ErrorInit( char *pgm_name ) {
    hInstance.filename = pgm_name;
    if( OpenResFile( &hInstance ) == -1 ) return;
    ResFlags |= RF_OPENED;
    if( FindResources( &hInstance ) ) return;
    if( InitResources( &hInstance ) ) return;
    MsgShift = WResLanguage() * MSG_LANG_SPACING;
    ResFlags |= RF_INITIALIZED;
}

static  void    ErrorFini( void ) {
    if( ResFlags & RF_OPENED ) {
        CloseResFile( &hInstance );
    }
}

void    __InitResource( void ) {
    __ErrorInit = &ErrorInit;
    __ErrorFini = &ErrorFini;
    __BldErrMsg = &BldErrMsg;
}
