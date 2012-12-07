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


#include <rdos.h>
#include <stdio.h>
#include <stdlib.h>
#include "uidef.h"
#include "uiforce.h"

extern EVENT    Event;
extern int      WaitHandle;

void global uiflush( void )
{
    Event = EV_NO_EVENT;
    flushkey();
}

unsigned long global uiclock( void )
{
    unsigned long msb;
    unsigned long lsb;

    RdosGetSysTime( &msb, &lsb );
    return( lsb );
}

EVENT global uieventsource( int update )
{
    EVENT                   ev;
    static      int         ReturnIdle = 1;
    unsigned long           start;
    EVENT                   ( *proc )();

    start = uiclock();
    for( ; ; ) {
        ev = forcedevent();
        if( ev > EV_NO_EVENT ) break;

        proc = RdosWaitTimeout( WaitHandle, 25 );
        if( proc != 0) {
            ev = (*proc)();
            if( ev > EV_NO_EVENT ) break;
        }
        
        if( ReturnIdle ) {
            ReturnIdle--;
            return( EV_IDLE );
        } else {
            if( update ) uirefresh();
            if( uiclock() - start >= UIData->tick_delay ) {
                return( EV_CLOCK_TICK );
            } else if( UIData->busy_wait ) {
                return( EV_SINK );
            }
        }

        proc = RdosWaitForever( WaitHandle );
        if( proc != 0) {
            ev = (*proc)();
            if( ev > EV_NO_EVENT ) break;
        }

    }
    ReturnIdle = 1;
    return( ev );
}


EVENT global uiget( void )
{
    return( uieventsource( 1 ) );
}
