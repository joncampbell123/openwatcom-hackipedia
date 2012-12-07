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
* Description:  GUI library main message loop.
*
****************************************************************************/


#include "guiwind.h"
#include "guiwinlp.h"
#include "guixutil.h"

extern  WPI_INST        GUIMainHInst;

int GUIWinMessageLoop( void )
{
    WPI_PARAM1  param1;
    WPI_QMSG    msg;

    while( _wpi_getmessage( GUIMainHInst, &msg, NULLHANDLE, 0, 0 ) ) {

        _wpi_translatemessage( &msg );
        _wpi_dispatchmessage( GUIMainHInst, &msg );
    }
    _wpi_getqmsgvalues( msg, NULL, NULL, &param1, NULL, NULL, NULL );
    return( (int)param1 );
}

#if defined( WINDU )
#include <stdio.h>

// This function is used by the UNIX signal handler as an interface
// with the Wind/U windows message loop.

void GUIPostQuitMsg( void )
{
    WPI_PARAM1  param1;
    WPI_QMSG    msg;
    gui_window *wnd;

    wnd = GUIXGetRootWindow();

    if( wnd ) {
        /* Wind/U only works with the Windows API, so we know this will work */
        PostMessage( wnd->hwnd, WM_CLOSE, NULL, NULL );
    }
}
#endif
