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
#include "guicontr.h"

bool GUIClearText( gui_window *wnd, unsigned id )
{
    HWND                control;

    control = _wpi_getdlgitem( wnd->hwnd, id );
    if( control != (HWND)NULL ) {
        _wpi_setwindowtext( control, NULL );
    }
    return( TRUE );
}

bool GUIEnableControl( gui_window *wnd, unsigned id, bool enable )
{
    HWND                control;

    control = _wpi_getdlgitem( wnd->hwnd, id );
    if( control != (HWND)NULL ) {
        _wpi_enablewindow( control, enable );
        return( TRUE );
    }
    return( FALSE );
}

bool GUIIsControlEnabled( gui_window *wnd, unsigned id )
{
    HWND                control;

    control = _wpi_getdlgitem( wnd->hwnd, id );
    if( control != (HWND)NULL ) {
        if( _wpi_iswindowenabled( control ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

