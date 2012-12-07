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


#include <windows.h>
#include <string.h>
#include <limits.h>
#include "wreglbl.h"
#include "wrestat.h"
#include "wremain.h"
#include "wremem.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wrehints.h"
#include "wrelist.h"
#include "wretoolb.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define OUTLINE_AMOUNT    4
#define WDE_TOOL_BORDER_X 1
#define WDE_TOOL_BORDER_Y 1

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL WREToolBarHook ( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WREToolBar *WREFindToolBar         ( HWND );
static WREToolBar *WREAllocToolBar        ( void );
static void        WREAddToolBar          ( WREToolBar * );
static void        WRERemoveToolBar       ( WREToolBar * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST       *WREToolBarList      = NULL;

WREToolBar *WRECreateToolBar ( WREToolBarInfo *info, HWND parent )
{
    WREToolBar  *tbar;
    int         i;
    HMENU       sys_menu;
    char        *text;

    if( !info ) {
        return( NULL );
    }

    tbar = WREAllocToolBar();
    if( tbar == NULL ) {
        return( NULL );
    }

    tbar->info   = info;
    tbar->parent = parent;
    tbar->tbar   = (toolbar) ToolBarInit ( parent );

    ToolBarDisplay ( tbar->tbar, &info->dinfo );

    for ( i = 0; i < info->num_items; i++ ) {
        ToolBarAddItem ( tbar->tbar, &info->items[i] );
    }

    tbar->win = ToolBarWindow ( tbar->tbar );

    if( (info->dinfo.style & TOOLBAR_FLOAT_STYLE) == TOOLBAR_FLOAT_STYLE ) {
        sys_menu = GetSystemMenu ( tbar->win, FALSE );
        i = GetMenuItemCount ( sys_menu );
        for ( ; i>0; i-- ) {
            DeleteMenu ( sys_menu, i, MF_BYPOSITION );
        }
        text = WREAllocRCString( WRE_SYSMENUMOVE );
        AppendMenu( sys_menu, MF_STRING , SC_MOVE,  (text) ? text : "Move" );
        if( text ) {
            WREFreeRCString( text );
        }
        text = WREAllocRCString( WRE_SYSMENUSIZE );
        AppendMenu( sys_menu, MF_STRING , SC_SIZE,  (text) ? text : "Size" );
        if( text ) {
            WREFreeRCString( text );
        }
        text = WREAllocRCString( WRE_SYSMENUHIDE );
        AppendMenu( sys_menu, MF_STRING , SC_CLOSE, (text) ? text : "Hide" );
        if( text ) {
            WREFreeRCString( text );
        }
    }

    ShowWindow ( tbar->win, SW_SHOWNORMAL );

    UpdateWindow ( tbar->win );

    WREAddToolBar ( tbar );

    return ( tbar );
}

BOOL WREToolBarHook ( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    MINMAXINFO  *minmax;
    WREToolBar  *tbar;
    int          bstate;
    Bool         ret;

    if ( !( tbar = WREFindToolBar ( hwnd ) ) || ( tbar->win == NULL ) ) {
        return( FALSE );
    }

    ret = FALSE;

    switch( msg ) {

        case WM_USER:
            if ( lParam ) {
                WRESetStatusText ( NULL, "", TRUE );
            } else {
                WREDisplayHint ( wParam );
            }
            if ( lParam ) {
                bstate = BUTTON_UP;
            } else {
                bstate = BUTTON_DOWN;
            }
            WRESetToolBarItemState ( tbar, wParam, bstate );
            break;

        case WM_SIZE:
            if ( ( wParam != SIZE_MAXIMIZED ) &&
                 ( wParam != SIZE_MINIMIZED ) ) {
                GetWindowRect ( hwnd, &tbar->last_pos );
            }
            break;

        case WM_MOVE:
            if ( !IsZoomed ( hwnd ) ) {
                GetWindowRect ( hwnd, &tbar->last_pos );
            }
            break;

        case WM_GETMINMAXINFO:
            minmax = (MINMAXINFO *) lParam;
            minmax->ptMinTrackSize.x =
                2 * GetSystemMetrics(SM_CXFRAME) +
                tbar->info->dinfo.border_size.x +
                tbar->info->dinfo.button_size.x - 1;
            minmax->ptMinTrackSize.y =
                2 * GetSystemMetrics(SM_CYFRAME) +
                tbar->info->dinfo.border_size.y +
                GetSystemMetrics(SM_CYCAPTION) +
                tbar->info->dinfo.button_size.y - 1;
            break;

        case WM_DESTROY:
            WRECloseToolBar ( tbar );
            break;

    }

    return ( ret );
}

WREToolBar *WREFindToolBar ( HWND win )
{
    WREToolBar *tbar;
    LIST       *tlist;

    for ( tlist = WREToolBarList; tlist; tlist = ListNext ( tlist ) ) {
        tbar = ListElement ( tlist );
        if ( tbar->win == win ) {
            return ( tbar );
        }
    }

    return ( NULL );
}

Bool WRECloseToolBar ( WREToolBar *tbar )
{
    if ( tbar ) {
        tbar->win = (HWND) NULL;
        WRERemoveToolBar ( tbar );
        WREFreeToolBar ( tbar );
    }

    return( TRUE );
}

void WREFreeToolBarInfo ( WREToolBarInfo *info )
{
    if ( info ) {
        if ( info->items ) {
            WREMemFree ( info->items );
        }
        if ( info->dinfo.background ) {
            DeleteObject ( info->dinfo.background );
        }
        WREMemFree ( info );
    }
}

WREToolBarInfo *WREAllocToolBarInfo( int num )
{
    WREToolBarInfo *info;

    info = (WREToolBarInfo *) WREMemAlloc( sizeof (WREToolBarInfo) );

    if( info ) {
        memset( info, 0, sizeof ( WREToolBarInfo ) );
        info->items = (TOOLITEMINFO *) WREMemAlloc ( sizeof(TOOLITEMINFO) * num );
        if( info->items ) {
            memset( info->items, 0, sizeof(TOOLITEMINFO) * num );
            info->num_items = num;
        } else {
            WREMemFree( info );
            info = NULL;
        }
    }

    return( info );
}

WREToolBar *WREAllocToolBar ( void )
{
    WREToolBar *tbar;

    tbar = (WREToolBar *) WREMemAlloc ( sizeof ( WREToolBar ) );
    if ( tbar ) {
        memset ( tbar, 0, sizeof ( WREToolBar ) );
    }

    return ( tbar );
}

void WREFreeToolBar ( WREToolBar *tbar )
{
    if ( tbar ) {
        WREMemFree ( tbar );
    }
}

void WREAddToolBar ( WREToolBar *tbar )
{
    WREInsertObject ( &WREToolBarList, (void *)tbar );
}

void WREDestroyToolBar ( WREToolBar *tbar )
{
    ToolBarDestroy ( tbar->tbar );
}

void WRERemoveToolBar ( WREToolBar *tbar )
{
    ListRemoveElt ( &WREToolBarList, (void *)tbar );
}

void WREShutdownToolBars ( void )
{
    WREToolBar  *tbar;
    LIST        *tlist;

    tlist = WREListCopy ( WREToolBarList );
    for ( ; tlist; tlist = ListConsume ( tlist ) ) {
        tbar = ListElement ( tlist );
        ToolBarDestroy ( tbar->tbar );
    }
    ToolBarFini ( NULL );

    ListFree ( WREToolBarList );
}

void WRESetToolBarItemState ( WREToolBar *tbar, UINT id, UINT state )
{
    if ( tbar /*&& ( ToolBarGetState ( tbar->tbar, id ) != state )*/ ) {
        ToolBarSetState ( tbar->tbar, id, state );
    }
}

