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


#include "guiwind.h"
#include "guiextnm.h"

extern  WPI_INST        GUIMainHInst;
extern  WPI_INST        GUIResHInst;

static  bool            GUIMsgInitFlag = FALSE;

static  bool            external = FALSE;

bool GUIIsLoadStrInitialized( void )
{
    return( GUIMsgInitFlag );
}

bool GUILoadStrInit( char * fname )
{
#ifndef __OS2_PM__
    WPI_INST    library;
#endif
    char *      ext_name;

    fname = fname;
    if( !GUIMsgInitFlag ) {
        ext_name = GUIGetExtName();
        external = ( ext_name != NULL );
#ifndef __OS2_PM__
        if( external ) {
        // if an external resource DLL is specified, then load it
            library = _wpi_loadlibrary( GUIMainHInst, ext_name );
            if( library != NULL ) {
                memcpy( &GUIResHInst, &library, sizeof( WPI_INST ) ) ;
            } else {
                return( FALSE );
            }
        }
#endif
        GUIMsgInitFlag = TRUE;
        return( TRUE );
    } else {
        return( FALSE );
    }
}

bool GUILoadStrFini( void )
{
    if( GUIMsgInitFlag ) {
#ifndef __OS2_PM__
        if( external ) {
            _wpi_freelibrary( GUIMainHInst, GUIResHInst );
        }
#endif
        GUIMsgInitFlag = FALSE;
    }

    return( TRUE );
}

bool GUILoadString( int string_id, char *buffer, int buffer_length )
{
    if( buffer && buffer_length ) {
        if( _wpi_loadstring( GUIResHInst, string_id,
                             (LPSTR) buffer, buffer_length ) != 0 ) {
            return( TRUE );
        } else {
            buffer[0] = '\0';
        }
    }

    return( FALSE );
}

