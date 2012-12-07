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
* Description:  GUI library message loop.
*
****************************************************************************/


#include "guiwind.h"
#include "guix.h"
#include "guiscale.h"
#include "guixloop.h"
#include "guixutil.h"
#include "guicolor.h"
#include "guixmain.h"
#include "guisysfi.h"
#include "guimenu.h"
#include "guiwhole.h"
#include "guiwnclr.h"
#include "guihook.h"
#include "guiutil.h"
#include "guigadgt.h"
#include "guizlist.h"
#include "guideath.h"
#include "guidead.h"
#include "guistr.h"
#include "guiextnm.h"
#include <string.h>
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include <stdlib.h>

extern bool GUIFirstCrack( void );

extern bool GUIMainTouched;

extern EVENT GUIUIProcessEvent( EVENT );
extern bool GUIIsInit( void );
extern void GUIDoneEventProcessing( void );
extern void GUIStartEventProcessing( void );

gui_window      *GUICurrWnd     =       NULL;

/* include from the app */
extern  void GUImain( void );

EVENT GUIAllEvents[] = {
    EV_FIRST_EVENT, LAST_EVENT,
    FIRST_GUI_EVENT, LAST_GUI_EVENT,
    EV_NO_EVENT,
    EV_NO_EVENT
};

/*
 * GUIWndGetEvent -- get ad event (other than EV_NO_EVENT) from UI
 */

EVENT GUIWndGetEvent( VSCREEN * screen )
{
    EVENT ev;

    do {
        ev = uivgetevent( screen );
        ev = GUIUIProcessEvent( ev );
    } while( ev == EV_NO_EVENT );
    return( ev );
}

void uistartevent( void )
{
    GUIStartEventProcessing();
}

void uidoneevent( void )
{
    GUIDoneEventProcessing();
}

/*
 * MessageLoop -- get events and process them
 */

static void MessageLoop( void )
{
    EVENT ev;

    uipushlist( GUIAllEvents );
    while( GUIGetFront() != NULL ) {
        if( GUICurrWnd != NULL ) {
            ev = GUIWndGetEvent( &GUICurrWnd->screen );
        } else {
            ev = GUIWndGetEvent( NULL );
        }
        if( !GUIProcessEvent( ev ) ) {
            break;
        }
    }
    uipoplist( /* GUIAllEvents */ );
}

static void GUICleanupInit( bool intstringtable_cleanup,
                            bool loadstrings_cleanup )
{
    if( intstringtable_cleanup ) {
        GUIFiniInternalStringTable();
    }
    if( loadstrings_cleanup ) {
        GUILoadStrFini();
    }
}

void GUICleanup( void )
{
    GUIDeath();                 /* user replaceable stub function */
    uirefresh();
    uiswap();
    uirestorebackground();      /* must be after uiswap */
    GUICleanupHotSpots();
    GUICleanupInit( TRUE, TRUE );
    GUISysFini();
}

static bool LoadStrings( void )
{
    char *      resource_name;
    char        fname[_MAX_PATH];

    resource_name = GUIGetExtName();
    if( resource_name != NULL ) {
        return( GUILoadStrInit( resource_name ) );
    } else {
        _cmdname( fname );
        if( fname == NULL ) return( FALSE );
        return( GUILoadStrInit( fname ) );
    }
}

static void MainLoop( void )
{
    bool        loadstrings_ok;
    bool        intstringtable_ok;
    #ifdef __WINDOWS__
        SAREA area;
    #endif

    loadstrings_ok    = LoadStrings();
    intstringtable_ok = GUIInitInternalStringTable();
    if( loadstrings_ok  &&  intstringtable_ok ) {
        GUImain();
        if( GUIIsInit() ) {
            #ifdef __WINDOWS__
                area.row = 0;
                area.col = 0;
                area.width = UIData->width;
                area.height = UIData->height;
                uidirty( area );
                uirefresh();
            #endif

            MessageLoop();
            GUICleanup();
            loadstrings_ok    = FALSE;
            intstringtable_ok = FALSE;
        }
    }
    GUICleanupInit( intstringtable_ok, loadstrings_ok );
    GUIDead();                 /* user replaceable stub function */
}

/*
 * GUIXMain
 */

int GUIXMain( int argc, char * argv[] )
{
    GUIMainTouched = TRUE;
    GUIMemOpen();
    if( GUIFirstCrack() ) {
        GUIStoreArgs( argv, argc );
        MainLoop();
    }
    GUIMemClose();
    return( 0 );
}

/*
 * GUIXSetupWnd - initializes the gui_window struture
 */

void GUIXSetupWnd( gui_window *wnd )
{
    wnd->screen.event = EV_NO_EVENT;
    wnd->screen.flags = V_UNFRAMED | V_NO_ZOOM | GUI_WINDOW;
    wnd->screen.cursor = C_OFF;
    wnd->flags = CHECK_CHILDREN_ON_RESIZE;
    wnd->background = ' ';
}

bool GUISetBackgroundChar( gui_window *wnd, char background )
{
    wnd->background = background;
    return( TRUE );
}


/*
 * GUISetupStruct - sets up the gui_window structure
 */

bool GUISetupStruct1( gui_window *wnd, gui_create_info *info, bool dialog )
{
    wnd->style = info->style;
    if( !dialog ) {
        if( !GUICreateMenus( wnd, info ) ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

bool GUISetupStruct2( gui_window *wnd, gui_create_info *info, bool dialog )
{
    if( !GUIJustSetWindowText( wnd, info->text ) ) {
        return( FALSE );
    }
    if( !GUISetArea( &wnd->screen.area, &info->rect, info->parent, TRUE, dialog ) ) {
        return( FALSE );
    }
    GUISetUseWnd( wnd );
    if( info->scroll & GUI_VSCROLL ) {
        if( !GUICreateGadget( wnd, VERTICAL, wnd->use.width, wnd->use.row,
                           wnd->use.height, &wnd->vgadget, info->scroll ) ) {
            return( FALSE );
        }
    }
    if( info->scroll & GUI_HSCROLL ) {
        if( !GUICreateGadget( wnd, HORIZONTAL, wnd->use.height, wnd->use.col,
                           wnd->use.width, &wnd->hgadget, info->scroll ) ) {
            return( FALSE );
        }
    }
    if( wnd->style & GUI_CURSOR ) {
        wnd->screen.cursor = C_NORMAL;
        GUISetCursor( wnd );
    }
    if( !GUISetColours( wnd, info->num_attrs, info->colours ) ) {
        return( FALSE );
    }
    return( TRUE );
}

bool GUISetupStruct( gui_window *wnd, gui_create_info *info, bool dialog )
{
    if( GUISetupStruct1( wnd, info, dialog ) ) {
        return( GUISetupStruct2( wnd, info, dialog ) );
    }
    return( FALSE );
}

/*
 * GUIXCreateWindow - create a UI window
 */

bool GUIXCreateWindow( gui_window *wnd, gui_create_info *info,
                       gui_window *parent )
{
    if( parent != NULL ) {
        wnd->sibling = parent->child;
        parent->child = wnd;
        wnd->parent = parent;
    } else {
        if( !( info->style & GUI_POPUP ) ) {
            wnd->flags |= IS_ROOT;
        }
    }
    if( !GUISetupStruct1( wnd, info, FALSE ) ) {
        return( FALSE );
    }
    if( wnd->vbarmenu != NULL ) {
        uimenubar( wnd->vbarmenu );
        GUISetScreen( XMIN, YMIN, XMAX-XMIN, YMAX-YMIN );
    }
    if( !GUISetupStruct2( wnd, info, FALSE ) ) {
        return( FALSE );
    }
    GUIFrontOfList( wnd );
    GUISetIcon( wnd, info->icon );
    if( uivopen( &wnd->screen ) != NULL ) {
        if( info->style & GUI_INIT_MAXIMIZED ) {
            GUIMaximizeWindow( wnd );
        } else if( info->style & GUI_INIT_MINIMIZED ) {
            GUIMinimizeWindow( wnd );
        }
        if( info->style & GUI_INIT_INVISIBLE ) {
            uivhide( &wnd->screen );
        }
        if( wnd->vgadget != NULL ) {
            uiinitgadget( wnd->vgadget );
        }
        if( wnd->hgadget != NULL ) {
            uiinitgadget( wnd->hgadget );
        }
        if( !GUIEVENTWND( wnd, GUI_INIT_WINDOW, NULL ) ) {
            return( FALSE );
        }
        GUIBringToFront( wnd );
        GUIWholeWndDirty( wnd );
        uisetmouse( wnd->screen.area.row, wnd->screen.area.col );
        return( TRUE );
    } else {
        return( FALSE );
    }
}
