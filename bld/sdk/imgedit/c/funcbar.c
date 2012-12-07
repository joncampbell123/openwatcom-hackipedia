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


#include "imgedit.h"
#include "funcbar.h"

static void             *functionBar;
static HWND             hFunctionBar = NULL;
static button           toolList[] = {
    { NEWBMP, IMGED_NEW, FALSE, NONE, 0, 0, WIE_TIP_NEW },
    { OPENBMP, IMGED_OPEN, FALSE, NONE, 0, 0, WIE_TIP_OPEN },
    { SAVEBMP, IMGED_SAVE, FALSE, NONE, 0, 0, WIE_TIP_SAVE },
    { GRIDBMP, IMGED_GRID, TRUE, GRIDDBMP, 0, 0, WIE_TIP_GRID },
    { MAXIMIZEBMP, IMGED_MAXIMIZE, TRUE, MAXIMIZEDBMP, 0, 0, WIE_TIP_MAXIMIZE },
    { CUTBMP, IMGED_CUT, FALSE, NONE, 0, 0, WIE_TIP_CUT },
    { COPYBMP, IMGED_COPY, FALSE, NONE, 0, 0, WIE_TIP_COPY },
    { PASTEBMP, IMGED_PASTE, FALSE, NONE, 0, 0, WIE_TIP_PASTE },
    { UNDOBMP, IMGED_UNDO, FALSE, NONE, 0, 0, WIE_TIP_UNDO },
    { REDOBMP, IMGED_REDO, FALSE, NONE, 0, 0, WIE_TIP_REDO },
    { CLEARBMP, IMGED_CLEAR, FALSE, NONE, 0, 0, WIE_TIP_CLEAR },
    { SNAPBMP, IMGED_SNAP, FALSE, NONE, 0, 0, WIE_TIP_SNAP },
    { RIGHTBMP, IMGED_RIGHT, FALSE, NONE, 0, 0, WIE_TIP_RIGHT },
    { LEFTBMP, IMGED_LEFT, FALSE, NONE, 0, 0, WIE_TIP_LEFT },
    { UPBMP, IMGED_UP, FALSE, NONE, 0, 0, WIE_TIP_UP },
    { DOWNBMP, IMGED_DOWN, FALSE, NONE, 0, 0, WIE_TIP_DOWN },
    { HFLIPBMP, IMGED_FLIPHORZ, TRUE, HFLIPDBMP, 0, 0, WIE_TIP_FLIPHORZ },
    { VFLIPBMP, IMGED_FLIPVERT, TRUE, VFLIPDBMP, 0, 0, WIE_TIP_FLIPVERT },
    { CLROTBMP, IMGED_ROTATECL, TRUE, CLROTDBMP, 0, 0, WIE_TIP_ROTATECL },
    { CCROTBMP, IMGED_ROTATECC, TRUE, CCROTDBMP, 0, 0, WIE_TIP_ROTATECC },
    { SAVEBMP, IMGED_DDE_UPDATE_PRJ, FALSE, NONE, 0, 0, -1 }
};

/*
 * addFunctionButton - adds a button to the function bar.
 */
static void addFunctionButton( button *tb, BOOL is_sticky )
{
    TOOLITEMINFO        info;

    tb->hbmp = _wpi_loadbitmap( Instance, tb->name );
    info.u.bmp = tb->hbmp;
    info.id = tb->id;
    info.flags = ITEM_DOWNBMP;

    if (is_sticky) {
        info.flags |= ITEM_STICKY;
    }
    if (tb->has_down) {
        tb->downbmp = _wpi_loadbitmap( Instance, tb->downname );
    } else {
        tb->downbmp = tb->hbmp;
    }
    if( tb->tip_id >= 0 ) {
        _wpi_loadstring( Instance, tb->tip_id, info.tip, MAX_TIP );
    } else {
        info.tip[0] = '\0';
    }

    info.depressed = tb->downbmp;
    ToolBarAddItem( functionBar, &info );

} /* addFunctionButton */

/*
 * addItems - adds the buttons to the function bar
 */
void addItems( void )
{
    TOOLITEMINFO        tii;
    int                 i;

    tii.flags = ITEM_BLANK;
    tii.u.blank_space = 5;

    if( ImgedIsDDE ) {
        addFunctionButton( &(toolList[10]), FALSE );
        addFunctionButton( &(toolList[20]), FALSE );
    } else {
        for (i=0; i < 3; ++i) {
            addFunctionButton( &(toolList[i]), FALSE );
        }
        ToolBarAddItem( functionBar, &tii );
    }

    addFunctionButton( &(toolList[3]), TRUE );
    addFunctionButton( &(toolList[4]), FALSE );
    ToolBarAddItem( functionBar, &tii );

    for (i=5; i < 8; ++i) {
        addFunctionButton( &(toolList[i]), FALSE );
    }

    ToolBarAddItem( functionBar, &tii );
    for (i=8; i < 10; ++i) {
        addFunctionButton( &(toolList[i]), FALSE );
    }

    ToolBarAddItem( functionBar, &tii );

    if( ImgedIsDDE ) {
        addFunctionButton( &(toolList[11]), FALSE );
    } else {
        for (i=10; i < 12; ++i) {
            addFunctionButton( &(toolList[i]), FALSE );
        }
    }

    ToolBarAddItem( functionBar, &tii );
    for (i=12; i < 16; ++i) {
        addFunctionButton( &(toolList[i]), FALSE );
    }

    ToolBarAddItem( functionBar, &tii );
    for (i=16; i < 20; ++i) {
        addFunctionButton( &(toolList[i]), FALSE );
    }
} /* addItems */

void FunctionBarHelpProc( HWND hwnd, WPI_PARAM1 wparam, BOOL pressed )
{
    hwnd=hwnd;
    if( pressed ) {
        ShowHintText( wparam );
    } else {
        SetHintText( " " );
    }
}

/*
 * FunctionBarProc - hook function which intercepts messages to the tool bar.
 */
BOOL FunctionBarProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                                        WPI_PARAM2 lparam )
{
    short               i;
    static BOOL         gridButtonDown = FALSE;

    hwnd = hwnd;

    switch(msg) {
    case WM_CREATE:
        break;

    case WM_USER:
        if (!lparam) {
            ShowHintText( wparam );
        }

        if( LOWORD(wparam) == IMGED_GRID ) {
            ToolBarSetState( functionBar, LOWORD(wparam), BUTTON_DOWN );
        } else {
            return 1;
        }
        break;

    case WM_COMMAND:
        if( LOWORD(wparam) == IMGED_GRID ) {
            if( !gridButtonDown ) {
                gridButtonDown  = TRUE;
            } else {
                gridButtonDown  = FALSE;
            }
            if( !HMainWindow ) {
                break;
            }
            CheckGridItem( _wpi_getmenu(_wpi_getframe(HMainWindow)) );
            return( 1 );
        }

        ToolBarSetState(functionBar, LOWORD(wparam), BUTTON_UP);
        break;

    case WM_DESTROY:
        for( i=0; i < NUMBER_OF_FUNCTIONS; ++i ) {
            _wpi_deletebitmap( toolList[i].hbmp );
            if ( toolList[i].has_down ) {
                _wpi_deletebitmap( toolList[i].downbmp );
            }
        }
        break;
    }
    return( 0 );

} /* FunctionBarProc */

/*
 * InitFunctionBar - Initializes the tool bar at the top of the image editor.
 */
void InitFunctionBar( HWND hparent )
{
    WPI_POINT           buttonsize = {FUNC_BUTTON_WIDTH, FUNC_BUTTON_HEIGHT};
    WPI_POINT           border = {FUNC_BORDER_X, FUNC_BORDER_Y};
    WPI_RECT            functionbar_loc;
    TOOLDISPLAYINFO     tdi;
    WPI_RECT            rect;
    int                 width;
    int                 height;

    _wpi_getclientrect( hparent, &rect );
    width = _wpi_getwidthrect( rect );
    height = _wpi_getheightrect( rect );

    _wpi_setwrectvalues( &(functionbar_loc), 0, 0, max(MIN_WIDTH, width),
                                                    FUNC_BUTTON_HEIGHT + 5 );
    _wpi_cvth_rect( &(functionbar_loc), height );
#ifdef __OS2_PM__
    functionbar_loc.yBottom += 1;
#endif

    functionBar = ToolBarInit(hparent);
    tdi.button_size = buttonsize;
    tdi.border_size = border;
    tdi.area = functionbar_loc;
    tdi.style = TOOLBAR_FIXED_STYLE;
    tdi.hook = (toolhook)FunctionBarProc;
    tdi.helphook = (helphook)FunctionBarHelpProc;
    tdi.background = (HBITMAP)0;
    tdi.foreground = (HBITMAP)0;
    tdi.is_fixed = 1;
    tdi.use_tips = 1;

    ToolBarDisplay(functionBar, &tdi);

    addItems();
    hFunctionBar = ToolBarWindow( functionBar );

    _wpi_showwindow( hFunctionBar, SW_SHOWNORMAL );
    _wpi_updatewindow( hFunctionBar );

    GrayEditOptions();
} /* InitFunctionBar */

/*
 * CloseFunctionBar - Call the clean up routine.
 */
void CloseFunctionBar( void )
{
    ToolBarFini( functionBar );
} /* CloseFunctionBar */

/*
 * ResizeFunctionBar - handles the resizing of the function bar.
 */
void ResizeFunctionBar( WPI_PARAM2 lparam )
{
    short       width;
    int         top;
    int         bottom;
    HWND        hwnd;

    if (!hFunctionBar) {
        return;
    }
#ifndef __OS2_PM__
    width = max( MIN_WIDTH, LOWORD( lparam ) );
    top = 0;
    bottom = FUNC_BUTTON_HEIGHT+5;
#else
    width = max( MIN_WIDTH, SHORT1FROMMP( lparam ) );

    {
        short           height;
        WPI_RECT        rect;

        _wpi_getclientrect( HMainWindow, &rect );
        height = _wpi_getheightrect( rect );
        /*
         * these are actually switched but it's necessary for the setwindowpos
         * macro
         */
        bottom = _wpi_cvth_y( 0, height );
        top = _wpi_cvth_y( FUNC_BUTTON_HEIGHT+3, height );
    }
#endif

    hwnd = ToolBarWindow( functionBar );
    _wpi_setwindowpos( hwnd, HWND_TOP, 0, top, width, bottom,
                        SWP_SHOWWINDOW | SWP_MOVE | SWP_SIZE );
} /* ResizeFunctionBar */

/*
 * PressGridButton - (de)presses the grid button as necessary.
 */
void PressGridButton( void )
{
    if (!functionBar) return;

    if (ImgedConfigInfo.grid_on) {
        ToolBarSetState(functionBar, IMGED_GRID, BUTTON_DOWN);
    } else {
        ToolBarSetState(functionBar, IMGED_GRID, BUTTON_UP);
    }
} /* PressGridButton */
