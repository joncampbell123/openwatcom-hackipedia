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
#include "guixhook.h"
#include <stdlib.h>

extern gui_window       *GUIMDIGetWindow( int id );
extern void             GUIMDIMoreWindows(void);

static void BringWindowToFront( gui_window *wnd )
{
    if( wnd != NULL ) {
        if( GUIIsMinimized( wnd ) ) {
            GUIRestoreWindow( wnd );
        }
        GUIBringToFront( wnd );
    }
}

static bool FakeMDIProcessMessage( gui_window *wnd, HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                               WPI_PARAM2 lparam, WPI_MRESULT *ret )
{
    WORD        param;

    ret=ret;
    if( wnd->root == hwnd ) {
        if( msg == WM_COMMAND ) {
            param = _wpi_getid( wparam );
            if( _wpi_ismenucommand( wparam, lparam ) ) {
                if( param == GUI_MDI_MORE_WINDOWS ) {
                    GUIMDIMoreWindows();
                    return( TRUE );
                }
                if( ( param >= GUI_MDI_FIRST_WINDOW ) &&
                        ( param <= GUI_MDI_FIRST_WINDOW + MAX_NUM_MDI_WINDOWS ) ) {
                    BringWindowToFront( GUIMDIGetWindow( param ) );
                    return( TRUE );
                }
            }
        }
    }
    return( FALSE );
}

bool XFakeInitMDI( gui_window *wnd )
{
    GUISetMDIProcessMessage( &FakeMDIProcessMessage );
    return( wnd->root == NULLHANDLE );
}
