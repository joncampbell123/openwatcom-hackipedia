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


// a lot of the contents of this file were liberally lifted from
// wlmsg.c in the WLIB project -- Wes

#include "guiwind.h"
#include "guiextnm.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "wressetr.h"

#define NIL_HANDLE      ((int)-1)

#if defined( __QNX__ ) || defined( __UNIX__ )
    #define _newline "\n"
#else
    #define _newline "\r\n"
#endif

#define NO_RES_MESSAGE_PREFIX "Error: could not open message resource file ("
#define NO_RES_MESSAGE_SUFFIX ")." _newline

#ifndef RT_DIALOG
#define RT_DIALOG       5
#endif
#ifndef RT_MENU
#define RT_MENU         4
#endif

static  HANDLE_INFO     hInstance = { 0 };
static  bool            GUIMsgInitFlag = FALSE;
extern  long            FileShift;

static off_t GUIResSeek( int handle, off_t position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

#ifdef __WATCOMC__
WResSetRtns( open,
             close,
             read,
             write,
             GUIResSeek,
             tell,
             GUIMemAlloc,
             GUIMemFree );
#else
WResSetRtns( open,
             close,
             ( int (*) (WResFileID, void *, int) )read,
             ( int (*) (WResFileID, const void *, int) )write,
             GUIResSeek,
             tell,
             ( void* (*)(unsigned long) )GUIMemAlloc,
             GUIMemFree );
#endif

bool GUIIsLoadStrInitialized( void )
{
    return( GUIMsgInitFlag );
}

bool GUILoadStrInit( char * fname )
{
    bool        error;

    hInstance.filename = fname;
    error = OpenResFile( &hInstance ) == NIL_HANDLE;
    if( !error ) {
        if( GUIGetExtName() != NULL ) {
            // we are using an external resource file so we don't have to
            // search
            FileShift = 0;
        } else {
            error = FindResources( &hInstance );
        }
    }
    if( !error ) {
        error = InitResources( &hInstance );
    }
    if( error ) {
        GUIMsgInitFlag = FALSE;
        write( fileno(stdout), NO_RES_MESSAGE_PREFIX, sizeof( NO_RES_MESSAGE_PREFIX ) - 1 );
        write( fileno(stdout), fname,                 strlen( fname ) );
        write( fileno(stdout), NO_RES_MESSAGE_SUFFIX, sizeof( NO_RES_MESSAGE_SUFFIX ) - 1 );
    } else {
        GUIMsgInitFlag = TRUE;
    }

    return( GUIMsgInitFlag );
}

bool GUILoadStrFini( void )
{
    if( GUIMsgInitFlag ) {
        if( CloseResFile( &hInstance ) != -1 ) {
            GUIMsgInitFlag = FALSE;
        } else {
            return( FALSE );
        }
    }

    return( TRUE );
}

bool GUILoadString( int string_id, char *buffer, int buffer_length )
{
    if( GUIMsgInitFlag && buffer && buffer_length ) {
        if( LoadString( &hInstance, string_id,
                        (LPSTR) buffer, buffer_length ) == 0 ) {
            return( TRUE );
        } else {
            buffer[0] = '\0';
        }
    }

    return( FALSE );
}

bool GUILoadDialogTemplate( int id, char **template, int *length )
{
    bool                ok;

    ok = ( GUIMsgInitFlag && template && length );

    if( ok ) {
        ok = ( WResLoadResource( &hInstance, (UINT)RT_DIALOG, id,
                                 (LPSTR *)template, length ) == 0 );
    }

    return( ok );
}

bool GUILoadMenuTemplate( int id, char **template, int *length )
{
    bool                ok;

    ok = ( GUIMsgInitFlag && template && length );

    if( ok ) {
        ok = ( WResLoadResource( &hInstance, (UINT)RT_MENU, id,
                                 (LPSTR *)template, length ) == 0 );
    }

    return( ok );
}

