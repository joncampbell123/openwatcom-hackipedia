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
#include "guiscale.h"
#include <string.h>

/*
 * GUIGetStringPos
 */

extern gui_ord GUIGetStringPos( gui_window * wnd, gui_ord indent,
                                char * string, gui_ord mouse_x )

{
    int pos;
    gui_coord start;
    gui_coord mouse;

    wnd = wnd;
    start.x = indent ;
    GUIScaleToScreenR( &start );
    mouse.x = mouse_x;
    GUIScaleToScreenR( &mouse );

    if( start.x > mouse.x ) {
        return( GUI_NO_COLUMN );
    }
    pos = mouse.x - start.x;
    if( pos >= strlen( string ) ) {
        return( GUI_NO_COLUMN );
    } else {
        return( (gui_ord)pos );
    }
}
