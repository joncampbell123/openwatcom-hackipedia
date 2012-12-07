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
* Description:  Message/resources support for the Execution Sampler.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef __WATCOMC__
    #include <process.h>
#else
    #include "clibext.h"
#endif

#if defined (__NETWARE__)
/*
// process.h exists for netware !!!
*/
_WCRTLINK extern char *_cmdname( char *__name );
#endif

#include "wmsg.h"
#include "wreslang.h"
#if defined(__WINDOWS__)
 #include "windows.h"
#else
 #include "wressetr.h"
 #include "watcom.h"

#define NIL_HANDLE      ((int)-1)
#ifndef STDOUT_HANDLE
 #define STDOUT_HANDLE   ((int)1)
#endif
static  HANDLE_INFO     hInstance = { 0 };
extern  long            FileShift;

#define NO_RES_MESSAGE "Error: could not open message resource file.\r\n"
#define NO_RES_SIZE (sizeof(NO_RES_MESSAGE)-1)
#endif

char    FAR_PTR         *MsgArray[ERR_LAST_MESSAGE-ERR_FIRST_MESSAGE+1];
extern void             Output( char FAR_PTR * );
extern void             FAR_PTR *alloc( int size );
extern void             mfree( void FAR_PTR *chunk );

#if !defined(__WINDOWS__)
static long res_seek( int handle, long position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

#ifndef __NETWARE__
WResSetRtns( open, close, read, write, res_seek, tell, malloc, free );
#else
WResSetRtns( open, close, ( int (*)( WResFileID, void *, size_t)) read, ( int (*)( WResFileID, const void *, size_t)) write, res_seek, tell, malloc, free );
#endif

int MsgInit()
{
    int         initerror;
    int         i;
    unsigned    msg_shift;
    char        buffer[_MAX_PATH];
#if defined(_PLS)
    char        *fname;
    char        fullpath[_MAX_PATH];
#endif

    if( _cmdname( buffer ) == NULL ) {
        initerror = 1;
    } else {
        hInstance.filename = buffer;
        OpenResFile( &hInstance );
#if defined(_PLS)
        if( hInstance.handle == NIL_HANDLE ) {
            _splitpath2( buffer, fullpath, NULL, NULL, &fname, NULL );
            _makepath( buffer, NULL, NULL, fname, ".exp" );
            _searchenv( buffer, "PATH", fullpath );
            if( fullpath[0] != '\0' ) {
                hInstance.filename = fullpath;
                OpenResFile( &hInstance );
            }
        }
#endif
        if( hInstance.handle == NIL_HANDLE ) {
            initerror = 1;
        } else {
            initerror = FindResources( &hInstance );
            if( !initerror ) {
                initerror = InitResources( &hInstance );
            }
        }
        if( !initerror ) {
            msg_shift = WResLanguage() * MSG_LANG_SPACING;
            for( i = ERR_FIRST_MESSAGE; i <= ERR_LAST_MESSAGE; i++ ) {
                if( LoadString( &hInstance, i + msg_shift, (LPSTR) buffer, 128 ) == -1 ) {
                    if( i == ERR_FIRST_MESSAGE ) {
                        initerror = 1;
                        break;
                    }
                    buffer[0] = '\0';
                }
                MsgArray[i-ERR_FIRST_MESSAGE] = alloc( strlen( buffer ) + 1 );

                if( MsgArray[i-ERR_FIRST_MESSAGE] == NULL ) break;
#if defined( __I86__ )
                _fstrcpy( MsgArray[i-ERR_FIRST_MESSAGE], buffer );
#else
                strcpy( MsgArray[i-ERR_FIRST_MESSAGE], buffer );
#endif
            }
            CloseResFile( &hInstance );
        }
    }
    if( initerror ) {
        write( STDOUT_HANDLE, NO_RES_MESSAGE, NO_RES_SIZE );
        return( 0 );
    }
    return( 1 );
}
#else

int MsgInit( HANDLE inst )
{
    int         i;
    char        buffer[128];
    unsigned    msg_shift;

    msg_shift = WResLanguage() * MSG_LANG_SPACING;

    for( i = ERR_FIRST_MESSAGE; i <= ERR_LAST_MESSAGE; i++ ) {
        if( LoadString( inst, i + msg_shift, (LPSTR) buffer, 128 ) == -1 ) {
            if( i == ERR_FIRST_MESSAGE ) {
                return( 0 );
                break;
            }
            buffer[0] = '\0';
        }
        MsgArray[i-ERR_FIRST_MESSAGE] = alloc( strlen( buffer ) + 1 );
        if( MsgArray[i-ERR_FIRST_MESSAGE] == NULL ) return( 0 );
        _fstrcpy( MsgArray[i-ERR_FIRST_MESSAGE], buffer );
    }
    return( 1 );
}
#endif

void MsgFini()
{
    int          i;

    for( i = ERR_FIRST_MESSAGE; i <= ERR_LAST_MESSAGE; i++ ) {
        mfree( MsgArray[i-ERR_FIRST_MESSAGE] );
    }
}

void MsgPrintfUsage( int first_ln, int last_ln )
{
    for( ; first_ln <= last_ln; first_ln++ ) {
        Output( MsgArray[first_ln-ERR_FIRST_MESSAGE] );
        Output( "\r" );
    }
}
