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
#include "guihot.h"
#include "guixhot.h"
#include "guihook.h"

        int             GUINumHotSpots;
extern  hotspot_info    *GUIHotSpots;


static void Cleanup( void )
{
    GUIXCleanupHotSpots();
    GUIMemFree( GUIHotSpots );
    GUIHotSpots = NULL;
    GUINumHotSpots = 0;
}

bool GUIInitHotSpots( int num_hot_spots, gui_resource *hot )
{
    GUINumHotSpots = num_hot_spots;
    if( num_hot_spots == 0 ) {
        GUIHotSpots = NULL;
        return( TRUE );
    }
    GUIHotSpots = (hotspot_info *)GUIMemAlloc( sizeof( hotspot_info ) * num_hot_spots );
    if( GUIHotSpots == NULL ) {
        GUINumHotSpots = 0;
        return( FALSE );
    }
    GUISetHotSpotCleanup( &Cleanup );
    if( !GUIXInitHotSpots( num_hot_spots, hot ) ) {
        Cleanup();
        return( FALSE );
    } else {
        GUISetHotSpotCleanup( &Cleanup );
        return( TRUE );
    }
}

int GUIGetNumHotSpots( void )
{
    return( GUINumHotSpots );
}

bool GUIGetHotSpotSize( int hot_spot, gui_coord *size )
{
    if( ( size == NULL ) || ( hot_spot > GUINumHotSpots ) ||
        ( hot_spot < 1 ) ) {
        return( FALSE );
    }
    size->x = GUIHotSpots[hot_spot-1].size.x;
    size->y = GUIHotSpots[hot_spot-1].size.y;
    GUIScreenToScaleR( size );
    return( TRUE );
}
