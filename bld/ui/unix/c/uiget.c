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
#include "uiforce.h"
#include <sys/types.h>
#include "uivirt.h"
extern EVENT Event;

extern EVENT uieventsourcehook( EVENT );
extern int   kb_wait( int secs, int usecs );

void global uiflush( void )
/*************************/
{
    Event = EV_NO_EVENT;
    flushkey();
}


static EVENT doget( int update )
/******************************/
{
    register    EVENT                   ev;
    static      short                   ReturnIdle = 1;

    for( ;; ) {
        ev = forcedevent();
        if( ev > EV_NO_EVENT ) break;
        ev = _uievent();
        if( ev > EV_NO_EVENT ) break;
        if( ReturnIdle ) {
            --ReturnIdle;
            return( EV_IDLE );
        } else {
            if( update ) uirefresh();
            if( UIData->busy_wait ) {
                return( EV_SINK );
            }
        }
        kb_wait( 60, 0 );
    }
    ReturnIdle = 1;
    if( ev==EV_REDRAW_SCREEN ){
        extern bool     UserForcedTermRefresh;
        SAREA           screen={ 0, 0, 0, 0 };

        screen.height= UIData->height;
        screen.width=  UIData->width;

        uidirty( screen );
        UserForcedTermRefresh= TRUE;
        physupdate( &screen );
    }
    return( ev );
}

EVENT global uieventsource( int update )
/**************************************/
{
    extern void intern  stopmouse(void);
    extern void intern  stopkeyboard(void);
    EVENT   ev;

    ev = doget( update );
    stopmouse();
    stopkeyboard();
    return( uieventsourcehook( ev ) );
}


EVENT global uiget( void )
/************************/
{
    return( uieventsource( 1 ) );
}
