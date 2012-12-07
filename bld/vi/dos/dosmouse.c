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
* Description:  DOS mouse help routines for VI
*
****************************************************************************/


#include "vi.h"
#include <dos.h>
#include "mouse.h"
#include "dosmouse.h"

/*
 * SetMouseSpeed - set mouse movement speed
 */
void SetMouseSpeed( int speed )
{
    SetMickeysToPixelsRatio( speed, speed * 2 );
    MouseSpeed = speed;

} /* SetMouseSpeed */

/*
 * SetMousePosition - set the mouse position
 */
void SetMousePosition( int row, int col )
{
    MouseRow = row;
    MouseCol = col;
    SetMousePointerPosition( col * MOUSE_SCALE, row * MOUSE_SCALE );

} /* SetMousePosition */

/*
 * PollMouse - poll the mouse for it's state
 */
void PollMouse( int *status, int *row, int *col )
{
    mouse_status        ms;

    GetMousePositionAndButtonStatus( &ms );
    *status = ms.button_status;
    *col = ms.x / MOUSE_SCALE;
    *row = ms.y / MOUSE_SCALE;

} /* PollMouse */

/*
 * InitMouse - initialize the mouse
 */
void InitMouse( void )
{
    int                 and_mask, or_mask;
    unsigned short _FAR *vector;
    unsigned char _FAR  *intrtn;

    if( !EditFlags.UseMouse ) {
        return;
    }

#if defined( __4G__ )
    vector = (unsigned short *)(MOUSE_INT * 4);
    intrtn = (unsigned char *)((((unsigned) vector[1]) << 4) + vector[0]);
#elif defined(__386__)
    vector = MK_FP( 0x34, MOUSE_INT * 4 );
    intrtn = MK_FP( 0x34, (((unsigned) vector[1]) << 4) + vector[0]);
#else
    vector = MK_FP( 0, MOUSE_INT * 4 );
    intrtn = MK_FP( vector[1], vector[0] );
#endif
    if( !((intrtn != NULL) && (*intrtn != 0xcf)) ) {
        EditFlags.UseMouse = FALSE;
        return;
    }

    MouseFunction( RESET_MOUSE_DRIVER );

    SetHorizontalLimitsForPointer( 0, (WindMaxWidth - 1) * MOUSE_SCALE );
    SetVerticalLimitsForPointer( 0, (WindMaxHeight - 1) * MOUSE_SCALE );

    if( EditFlags.Monocolor ) {
        and_mask = 0x79ff;
        or_mask = 0x7100;
    } else {
        and_mask = 0x7fff;
        or_mask = 0x7700;
    }
    SetTextPointerType( SOFTWARE_CURSOR, and_mask, or_mask );
    SetMousePointerExclusionArea( 0, 0, 0, 0 );
    SetMousePosition( WindMaxWidth / 2 - 1, WindMaxHeight / 2 - 1 );
    SetMouseSpeed( MouseSpeed );
    PollMouse( &MouseStatus, &MouseRow, &MouseCol );

} /* InitMouse */

/*
 * FiniMouse - done with the mouse
 */
void FiniMouse( void )
{
} /* FiniMouse */
