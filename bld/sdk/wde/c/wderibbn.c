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

#include "wdeglbl.h"
#include "wdemain.h"
#include "wdetoolb.h"
#include "wdestat.h"
#include "wdeopts.h"
#include "wdemain.h"
#include "wdehints.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wde_rc.h"
#include "wderibbn.h"
#include "wde_rc.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define BLANK_PAD    8
#define BUTTONX      22
#define BUTTONY      18
#define BUTTON_PAD   4
#define TOOL_BORDERX 4
#define TOOL_BORDERY 2

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL WdeRibbonHook       ( HWND, UINT, WPARAM, LPARAM );
extern void WdeRibbonHelpHook   ( HWND hwnd, WPARAM wParam, BOOL pressed );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    char    *up;
    char    *down;
    UINT    menu_id;
    WORD    flags;
    int     tip_id;
} WdeRibbonName;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static Bool WdeDoInitRibbon( HINSTANCE, WdeRibbonName *, int );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
WdeRibbonName WdeRibbonNames[] =
{
    { "New"     , NULL , IDM_NEW_RES     , 0            , WDE_TIP_NEW        }
,   { "Open"    , NULL , IDM_OPEN_RES    , 0            , WDE_TIP_OPEN       }
,   { "Save"    , NULL , IDM_SAVE_RES    , 0            , WDE_TIP_SAVE       }
,   { NULL      , NULL , BLANK_PAD       , 0            , -1                 }
,   { "Cut"     , NULL , IDM_CUTOBJECT   , 0            , WDE_TIP_CUT        }
,   { "Copy"    , NULL , IDM_COPYOBJECT  , 0            , WDE_TIP_COPY       }
,   { "Paste"   , NULL , IDM_PASTEOBJECT , 0            , WDE_TIP_PASTE      }
,   { NULL      , NULL , BLANK_PAD       , 0            , -1                 }
,   { "SzToTxt" , NULL , IDM_SIZETOTEXT  , 0            , WDE_TIP_SIZETOTEXT }
,   { NULL      , NULL , BLANK_PAD       , 0            , -1                 }
,   { "Test"    , NULL , IDM_TEST_MODE   , ITEM_STICKY  , WDE_TIP_TEST_MODE  }
,   { NULL      , NULL , BLANK_PAD       , 0            , -1                 }
,   { "Order"   , NULL , IDM_SET_ORDER   , ITEM_STICKY  , WDE_TIP_SET_ORDER  }
,   { "Tabs"    , NULL , IDM_SET_TABS    , ITEM_STICKY  , WDE_TIP_SET_TABS   }
,   { "Groups"  , NULL , IDM_SET_GROUPS  , ITEM_STICKY  , WDE_TIP_SET_GROUPS }
};
#define NUM_TOOLS (sizeof(WdeRibbonNames)/sizeof(WdeRibbonName))

WdeRibbonName WdeDDERibbonNames[] =
{
    { "Clear"   , NULL , IDM_DDE_CLEAR   , 0            , -1 }
,   { "Save"    , NULL , IDM_DDE_UPDATE_PRJ , 0         , -1 }
,   { NULL      , NULL , BLANK_PAD       , 0            , -1 }
,   { "Cut"     , NULL , IDM_CUTOBJECT   , 0            , -1 }
,   { "Copy"    , NULL , IDM_COPYOBJECT  , 0            , -1 }
,   { "Paste"   , NULL , IDM_PASTEOBJECT , 0            , -1 }
,   { NULL      , NULL , BLANK_PAD       , 0            , -1 }
,   { "SzToTxt" , NULL , IDM_SIZETOTEXT  , 0            , -1 }
,   { NULL      , NULL , BLANK_PAD       , 0            , -1 }
,   { "Test"    , NULL , IDM_TEST_MODE   , ITEM_STICKY  , -1 }
,   { NULL      , NULL , BLANK_PAD       , 0            , -1 }
,   { "Order"   , NULL , IDM_SET_ORDER   , ITEM_STICKY  , -1 }
,   { "Tabs"    , NULL , IDM_SET_TABS    , ITEM_STICKY  , -1 }
,   { "Groups"  , NULL , IDM_SET_GROUPS  , ITEM_STICKY  , -1 }
};
#define NUM_DDE_TOOLS (sizeof(WdeDDERibbonNames)/sizeof(WdeRibbonName))

static WdeToolBarInfo *WdeRibbonInfo   = NULL;
static WdeToolBar     *WdeRibbon       = NULL;
static int             WdeRibbonHeight = 0;
static int             WdeNumRibbonTools = 0;

int WdeGetRibbonHeight ( void )
{
    return ( WdeRibbonHeight );
}

Bool WdeInitRibbon ( HINSTANCE inst )
{
    Bool        ret;

    if( WdeIsDDE() ) {
        ret = WdeDoInitRibbon( inst, WdeDDERibbonNames, NUM_DDE_TOOLS );
    } else {
        ret = WdeDoInitRibbon( inst, WdeRibbonNames, NUM_TOOLS );
    }

    return( ret );
}

Bool WdeDoInitRibbon( HINSTANCE inst, WdeRibbonName *tools, int num_tools )
{
    int i;

    WdeNumRibbonTools = num_tools;

    WdeRibbonInfo = WdeAllocToolBarInfo( num_tools );

    if( !WdeRibbonInfo ) {
        return ( FALSE );
    }

    for( i=0; i<num_tools; i++ ) {
        if( tools[i].up ) {
            WdeRibbonInfo->items[i].bmp =
                LoadBitmap ( inst, tools[i].up );
            WdeRibbonInfo->items[i].id     = tools[i].menu_id;
            WdeRibbonInfo->items[i].flags  = tools[i].flags;
            WdeRibbonInfo->items[i].flags |= ITEM_DOWNBMP;
            if ( tools[i].down ) {
                WdeRibbonInfo->items[i].depressed =
                    LoadBitmap ( inst, tools[i].down );
            } else {
                WdeRibbonInfo->items[i].depressed =
                    WdeRibbonInfo->items[i].bmp;
            }
            if( tools[i].tip_id >= 0 ) {
                LoadString( inst, tools[i].tip_id, WdeRibbonInfo->items[i].tip,
                            MAX_TIP );
            } else {
                WdeRibbonInfo->items[i].tip[0] = '\0';
            }
        } else {
            WdeRibbonInfo->items[i].flags = ITEM_BLANK;
            WdeRibbonInfo->items[i].blank_space = tools[i].menu_id;
        }
    }

    WdeRibbonInfo->dinfo.button_size.x = BUTTONX + BUTTON_PAD;
    WdeRibbonInfo->dinfo.button_size.y = BUTTONY + BUTTON_PAD;
    WdeRibbonInfo->dinfo.border_size.x = TOOL_BORDERX;
    WdeRibbonInfo->dinfo.border_size.y = TOOL_BORDERY;
    WdeRibbonInfo->dinfo.style         = TOOLBAR_FIXED_STYLE;
    WdeRibbonInfo->dinfo.hook          = WdeRibbonHook;
    WdeRibbonInfo->dinfo.helphook      = WdeRibbonHelpHook;
    WdeRibbonInfo->dinfo.foreground    = NULL;
    WdeRibbonInfo->dinfo.background    = NULL;
    WdeRibbonInfo->dinfo.is_fixed      = TRUE;
    WdeRibbonInfo->dinfo.use_tips      = TRUE;

    return ( TRUE );
}

void WdeShutdownRibbon ( void )
{
    int  i;

    WdeDestroyRibbon ( );

    if ( !WdeRibbonInfo ) {
        return;
    }

    for ( i=0; i<WdeNumRibbonTools; i++ ) {
        if ( WdeRibbonInfo->items[i].flags != ITEM_BLANK ) {
            if ( WdeRibbonInfo->items[i].bmp ==
                 WdeRibbonInfo->items[i].depressed ) {
                WdeRibbonInfo->items[i].depressed = (HBITMAP) NULL;
            }
            if ( WdeRibbonInfo->items[i].bmp ) {
                DeleteObject ( WdeRibbonInfo->items[i].bmp );
            }
            if ( WdeRibbonInfo->items[i].depressed ) {
                DeleteObject ( WdeRibbonInfo->items[i].depressed );
            }
        }
    }

    WdeFreeToolBarInfo ( WdeRibbonInfo );
}

Bool WdeCreateRibbon ( HWND parent )
{
    if ( WdeRibbon || !WdeRibbonInfo || ( parent == (HWND)NULL ) ) {
        return ( FALSE );
    }

    GetClientRect ( parent, &WdeRibbonInfo->dinfo.area );

    WdeRibbonHeight = 2 * WdeRibbonInfo->dinfo.border_size.y +
                      WdeRibbonInfo->dinfo.button_size.y +
                      2 * GetSystemMetrics(SM_CYBORDER);
    WdeRibbonInfo->dinfo.area.bottom = WdeRibbonHeight;

    WdeRibbon = WdeCreateToolBar ( WdeRibbonInfo, parent );

    WdeResizeWindows ();

    return ( WdeRibbon != NULL );
}

Bool WdeResizeRibbon ( RECT *prect )
{
    if ( !WdeRibbon || !WdeRibbonHeight ||
         ( WdeRibbon->win == (HWND)NULL ) || !prect ) {
        return ( FALSE );
    }

    MoveWindow ( WdeRibbon->win, 0, 0, ( prect->right - prect->left ),
                 WdeRibbonHeight, TRUE );

    return ( TRUE );
}

void WdeShowRibbon( void )
{
    Bool        created;
    char        *mtext;

    if( WdeRibbonHeight ) {
        ShowWindow( WdeRibbon->win, SW_HIDE );
        WdeRibbonHeight = 0;
        WdeResizeWindows();
        mtext = WdeAllocRCString( WDE_SHOWTOOLBAR );
    } else {
        if( !WdeRibbon ) {
            WdeCreateRibbon( WdeGetMainWindowHandle () );
            created = TRUE;
        } else {
            created = FALSE;
        }
        if( !created ) {
            WdeRibbonHeight = 2 * WdeRibbonInfo->dinfo.border_size.y +
                              WdeRibbonInfo->dinfo.button_size.y +
                              2 * GetSystemMetrics(SM_CYBORDER);
            WdeResizeWindows();
        }
        mtext = WdeAllocRCString( WDE_HIDETOOLBAR );
        ShowWindow( WdeRibbon->win, SW_SHOW );
    }

    WdeSetOption( WdeOptIsRibbonVisible, ( WdeRibbonHeight != 0 ) );

    ModifyMenu( WdeGetInitialMenuHandle(), IDM_SHOW_RIBBON,
                MF_BYCOMMAND | MF_STRING, IDM_SHOW_RIBBON, mtext );
    ModifyMenu( WdeGetResMenuHandle(), IDM_SHOW_RIBBON,
                MF_BYCOMMAND | MF_STRING, IDM_SHOW_RIBBON, mtext );

    if( mtext ) {
        WdeFreeRCString( mtext );
    }
}

void WdeDestroyRibbon( void )
{
    if( WdeRibbon ) {
        WdeDestroyToolBar( WdeRibbon );
    }

    WdeRibbonHeight = 0;

    WdeResizeWindows();
}

void WdeSetRibbonItemState( WORD item, int state )
{
    if( WdeRibbon ) {
        WdeSetToolBarItemState( WdeRibbon, item, state );
    }
}

void WdeRibbonHelpHook( HWND hwnd, WPARAM wParam, BOOL pressed )
{
    _wde_touch(hwnd);

    WdeHandleToolHint( wParam, pressed );
}

BOOL WdeRibbonHook( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    Bool         ret;

    _wde_touch(hwnd);
    _wde_touch(wParam);
    _wde_touch(lParam);

    if( !WdeRibbon ) {
        return( FALSE );
    }

    ret = FALSE;

    switch( msg ) {
        case WM_DESTROY:
            WdeCloseToolBar ( WdeRibbon );
            WdeRibbon = NULL;
            break;
    }

    return ( ret );
}

