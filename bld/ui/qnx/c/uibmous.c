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


#include "uidef.h"
#include "uimouse.h"
#include "uivirt.h"

#define OFF_SCREEN      200

extern MOUSEORD MouseRow, MouseCol;

static MOUSEORD OldMouseRow, OldMouseCol = OFF_SCREEN;
static bool     MouseOn = FALSE;
static ATTR     OldAttr;

static char __FAR *RegenPos( unsigned row, unsigned col )
/*******************************************************/
{
    char        __FAR *pos;

    pos = (char __FAR *)UIData->screen.origin
          + (row*UIData->screen.increment+col)*sizeof(PIXEL) + 1;
    return( pos );
}

static void uisetmouseoff( void )
/*******************************/
{
    char                __FAR *old;
    SAREA               area;

    if( MouseOn ) {
        old = RegenPos( OldMouseRow, OldMouseCol );
        *old = OldAttr;
        area.row = OldMouseRow;
        area.col = OldMouseCol;
        area.width = 1;
        area.height = 1;
        _physupdate( &area );
    }
}

static void uisetmouseon( row, col )
/**********************************/
    register MOUSEORD        row;
    register MOUSEORD        col;
{
    char                __FAR *new;
    SAREA               area;

    if( MouseOn ){
        new = RegenPos( row, col );
        OldAttr = *new;
        if( UIData->colour == M_MONO ){
            *new = (OldAttr & 0x79) ^ 0x71;
        } else {
            *new = (OldAttr & 0x7f) ^ 0x77;
        }
        area.row = row;
        area.col = col;
        area.width = 1;
        area.height = 1;
        _physupdate( &area );
        OldMouseRow = row;
        OldMouseCol = col;
    }
}



void global uisetmouse( row, col )
/********************************/
    register MOUSEORD        row;
    register MOUSEORD        col;
{
    if( OldMouseRow == row && OldMouseCol == col ) return;
    uisetmouseoff();
    uisetmouseon( row, col );
}


void global uimouse( func )
/*************************/
{
    if( func == MOUSE_ON ) {
        MouseOn = TRUE;
        uisetmouseon( OldMouseRow, OldMouseCol );
    } else {
        uisetmouseoff();
        MouseOn = FALSE;
    }
}
