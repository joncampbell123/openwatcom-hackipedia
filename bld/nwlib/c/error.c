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
* Description:  Message output for librarian.
*
****************************************************************************/


#include "wlib.h"

#if defined( INCL_MSGTEXT )

#undef pick
#define pick( code, e_msg, j_msg ) e_msg,

static char *msg_text_array[] = {
    #include "wlib.msg"
};

void InitMsg( void ) {}

void MsgGet( int resourceid, char *buffer )
{
    strcpy( buffer, msg_text_array[ resourceid ] );
}

void FiniMsg( void ) {}

#else

#include <wresset2.h>   /* for FileShift */

#define NIL_HANDLE      ((int)-1)

static  HANDLE_INFO     hInstance = { 0 };
static  int             Res_Flag;
static  unsigned        MsgShift;

static off_t res_seek( WResFileID handle, off_t position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seek, tell, MemAllocGlobal, MemFreeGlobal );

void InitMsg( void )
{
    int initerror;

    hInstance.filename = ImageName;
    initerror = OpenResFile( &hInstance ) == NIL_HANDLE;
    if( !initerror ) {
        initerror = FindResources( &hInstance );
        if( initerror ) {
            CloseResFile( &hInstance );
        }
    }
    if( !initerror ) {
        initerror = InitResources( &hInstance );
        if( initerror ) {
            CloseResFile( &hInstance );
        }
    }
    MsgShift = WResLanguage() * MSG_LANG_SPACING;
    if( initerror ) {
        Res_Flag = EXIT_FAILURE;
        FatalResError();
    } else {
        Res_Flag = EXIT_SUCCESS;
    }
}

void MsgGet( int resourceid, char *buffer )
{
    if( LoadString( &hInstance, resourceid + MsgShift,
                (LPSTR)buffer, 128 ) != 0 ) {
        buffer[ 0 ] = '\0';
    }
}

void FiniMsg( void )
{
    if( Res_Flag == EXIT_SUCCESS ) {
        if( CloseResFile( &hInstance ) != -1 ) {
            Res_Flag = EXIT_FAILURE;
        } else {
            longjmp( Env, 1 );
        }
    }
}
#endif
