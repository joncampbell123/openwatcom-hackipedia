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
#include "ieclrpal.h"
#include "ieprofil.h"

/*
 * Win_CreateColourPal - windows version to create the colour palette
 */
void Win_CreateColourPal( void )
{
    char        *title;

    title = IEAllocRCString( WIE_COLORPALETTETITLE );
#ifndef __NT__
    HColourPalette = CreateWindow(
            PaletteClass,                       /* Window class name */
            title,
            WS_POPUPWINDOW | WS_CAPTION,        /* Window style */
            ImgedConfigInfo.pal_xpos,           /* Initial X position */
            ImgedConfigInfo.pal_ypos,           /* Initial Y position */
            CP_WIDTH+2,                         /* Initial X size */
            CP_HEIGHT+15,                       /* Initial Y size */
            HMainWindow,                        /* Parent window handle */
            (HMENU) NULL,                       /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */
#else
    HColourPalette = CreateWindowEx(
            WS_EX_TOOLWINDOW,
            PaletteClass,                       /* Window class name */
            title,
            WS_POPUPWINDOW | WS_CAPTION,        /* Window style */
            ImgedConfigInfo.pal_xpos,           /* Initial X position */
            ImgedConfigInfo.pal_ypos,           /* Initial Y position */
            CP_WIDTH+2,                         /* Initial X size */
            CP_HEIGHT+15,                       /* Initial Y size */
            HMainWindow,                        /* Parent window handle */
            (HMENU) NULL,                       /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */
#endif

    if( title ) {
        IEFreeRCString( title );
    }

    if( HColourPalette != (HWND)NULL ) {
        RECT    rect;
        int     w,h;
        _wpi_getclientrect( HColourPalette, &rect );
        w = _wpi_getwidthrect( rect );
        h = _wpi_getheightrect( rect );
        if( w < CP_WIDTH || h < CP_HEIGHT ) {
            GetWindowRect( HColourPalette, &rect );
            w = _wpi_getwidthrect( rect ) + ( CP_WIDTH - w );
            h = _wpi_getheightrect( rect ) + ( CP_HEIGHT - h );
            SetWindowPos( HColourPalette, HWND_TOP, 0, 0, w, h,
                          SWP_SIZE | SWP_NOZORDER | SWP_NOMOVE );
        }
    }

    SendMessage( HColourPalette, WM_SETFONT, (DWORD)SmallFont, 0L );
} /* Win_CreateColourPal */

/*
 * Win_CreateCurrentDisp - creates the current display window for the
 *                         windows version
 */
HWND Win_CreateCurrentDisp( HWND hparent )
{
    HWND        hwnd;

    hwnd = CreateWindow(
            CURRENT_CLASS,                      /* Window class name */
            "",
            WS_CHILD | WS_VISIBLE,              /* control styles       */
            CUR_WND_X,                          /* Initial X position */
            CUR_WND_Y,                          /* Initial Y position */
            CUR_WND_WIDTH,                      /* Initial X size */
            CUR_WND_HEIGHT,                     /* Initial Y size */
            hparent,                            /* Parent window handle */
            (HMENU)NULL,                        /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */

    ShowWindow( hwnd, SW_NORMAL );
    UpdateWindow( hwnd );
    SendMessage( hwnd, WM_SETFONT, (DWORD) SmallFont, 0L );

    return( hwnd );
} /* Win_CreateCurrentDisp */

/*
 * Win_CreateColourCtrls - creates the controls in the colour palette for
 *                         windows version
 */
void Win_CreateColourCtrls( HWND hpar, HWND *colours, HWND *screenclrs,
                                        HWND *screentxt, HWND *inversetxt )
{
    HWND        hwnd1;
    HWND        hwnd2;
    char        *text;

    *colours = CreateWindow(
            AVAIL_CLASS,                        /* Window class name */
            "",
            WS_CHILD | WS_VISIBLE,              /* control styles       */
            COL_WND_X,                          /* Initial X position */
            COL_WND_Y,                          /* Initial Y position */
            COL_WND_WIDTH,                      /* Initial X size */
            COL_WND_HEIGHT,                     /* Initial Y size */
            hpar,                               /* Parent window handle */
            (HMENU)NULL,                        /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */

    ShowWindow( *colours, SW_NORMAL );
    UpdateWindow( *colours );
    SendMessage( *colours, WM_SETFONT, (DWORD)SmallFont, 0L );

    *screenclrs = CreateWindow(
            SCREEN_CLASS,                       /* Window class name */
            "",
            WS_CHILD | WS_VISIBLE,              /* control styles       */
            SCRN_WND_X,                         /* Initial X position */
            SCRN_WND_Y,                         /* Initial Y position */
            SCRN_WND_WIDTH,                     /* Initial X size */
            SCRN_WND_HEIGHT,                    /* Initial Y size */
            hpar,                               /* Parent window handle */
            (HMENU)NULL,                        /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */

    ShowWindow( *screenclrs, SW_HIDE );
    SendMessage( *screenclrs, WM_SETFONT, (DWORD)SmallFont, 0L );

    hwnd1 = CreateWindow(
            "STATIC",                           /* Window class name */
            "",
            SS_RIGHT | WS_CHILD | WS_VISIBLE,   /* control styles       */
            TEXT1X,                             /* Initial X position */
            TEXT1Y,                             /* Initial Y position */
            TEXT_WIDTH,                         /* Initial X size */
            TEXTHEIGHT,                         /* Initial Y size */
            hpar,                               /* Parent window handle */
            (HMENU)NULL,                        /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */

    ShowWindow( hwnd1, SW_NORMAL );
    UpdateWindow( hwnd1 );
    SendMessage( hwnd1, WM_SETFONT, (DWORD) SmallFont, 0L );

    hwnd2 = CreateWindow(
            "STATIC",                           /* Window class name */
            "",
            SS_RIGHT | WS_CHILD | WS_VISIBLE,   /* control styles       */
            TEXT1X,                             /* Initial X position */
            TEXT2Y,                             /* Initial Y position */
            TEXT_WIDTH,                         /* Initial X size */
            TEXTHEIGHT,                         /* Initial Y size */
            hpar,                               /* Parent window handle */
            (HMENU)NULL,                        /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */

    ShowWindow( hwnd2, SW_NORMAL );
    UpdateWindow( hwnd2 );
    SendMessage( hwnd2, WM_SETFONT, (DWORD) SmallFont, 0L );

    *screentxt = CreateWindow(
            "STATIC",                   /* Window class name */
            "",
            SS_RIGHT | WS_CHILD | WS_VISIBLE,   /* control styles       */
            LINE1X,                             /* Initial X position */
            LINE1Y,                             /* Initial Y position */
            LINE_WIDTH,                         /* Initial X size */
            TEXTHEIGHT,                         /* Initial Y size */
            hpar,                               /* Parent window handle */
            (HMENU)NULL,                        /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */

    ShowWindow( *screentxt, SW_NORMAL );
    UpdateWindow( *screentxt );
    SendMessage( *screentxt, WM_SETFONT, (DWORD) SmallFont, 0L );

    *inversetxt = CreateWindow(
            "STATIC",                   /* Window class name */
            "",
            SS_RIGHT | WS_CHILD | WS_VISIBLE,   /* control styles       */
            LINE1X,                             /* Initial X position */
            LINE2Y,                             /* Initial Y position */
            LINE_WIDTH,                         /* Initial X size */
            TEXTHEIGHT,                         /* Initial Y size */
            hpar,                               /* Parent window handle */
            (HMENU)NULL,                        /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */

    ShowWindow( *inversetxt, SW_NORMAL );
    UpdateWindow( *inversetxt );
    SendMessage( *inversetxt, WM_SETFONT, (DWORD) SmallFont, 0L );

    text = IEAllocRCString( WIE_DRAWTEXT );
    if( text ) {
        SetWindowText( hwnd1, text );
        IEFreeRCString( text );
    } else {
        SetWindowText( hwnd1, "Draw:" );
    }
    text = IEAllocRCString( WIE_FILLTEXT );
    if( text ) {
        SetWindowText( hwnd2, text );
        IEFreeRCString( text );
    } else {
        SetWindowText( hwnd2, "Fill:" );
    }
} /* Win_CreateColourCtrls */

/*
 * WinNewDrawPad - This function creates a new drawing pad for Windows ver.
 */
HWND WinNewDrawPad( img_node *node )
{
    MDICREATESTRUCT     mdicreate;
    short               y_adjustment;
    short               x_adjustment;
    short               pad_x;
    short               pad_y;
    int                 i;
    img_node            *temp;
    POINT               origin;
    char                filename[ _MAX_PATH ];
    HWND                drawarea;
    HMENU               sys_menu;

    node->viewhwnd = CreateViewWin( node->width, node->height );
    pad_x = 0;
    pad_y = 0;

    temp = node->nexticon;

    for(i=1; i < node->num_of_images; ++i) {
        temp->viewhwnd = node->viewhwnd;
        temp = temp->nexticon;
    }

    if (node->imgtype == BITMAP_IMG) {
        mdicreate.szClass = DrawAreaClassB;
    } else if (node->imgtype == ICON_IMG) {
        mdicreate.szClass = DrawAreaClassI;
    } else {
        mdicreate.szClass = DrawAreaClassC;
    }

    GetFnameFromPath( node->fname, filename );
    mdicreate.szTitle = filename;
    mdicreate.hOwner = Instance;

    x_adjustment = (short)(2 * GetSystemMetrics( SM_CXFRAME ));
    y_adjustment = (short)(2 * GetSystemMetrics( SM_CYFRAME ) + GetSystemMetrics( SM_CYCAPTION ) - 1);

    origin.x = 0;
    origin.y = 0;
    FindOrigin( &origin );
    CalculateDims( node->width, node->height, &pad_x, &pad_y );

    mdicreate.cx = x_adjustment + pad_x;
    mdicreate.cy = y_adjustment + pad_y;
    mdicreate.x = origin.x;
    mdicreate.y = origin.y;
    mdicreate.style = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE |
                        WS_CLIPSIBLINGS | WS_DLGFRAME | WS_MINIMIZEBOX |
                        WS_THICKFRAME;
    mdicreate.lParam = (LPARAM)(LPVOID)node;

    drawarea = (HWND)SendMessage( ClientWindow, WM_MDICREATE, 0,
                                        (LPARAM)(LPVOID)(&mdicreate) );

    if( drawarea != (HWND)NULL ) {
        RECT    rect;
        int     w,h;
        _wpi_getclientrect( drawarea, &rect );
        w = _wpi_getwidthrect( rect );
        h = _wpi_getheightrect( rect );
        if( w != pad_x || h != pad_y ) {
            GetWindowRect( drawarea, &rect );
            w = _wpi_getwidthrect( rect ) + ( pad_x - w );
            h = _wpi_getheightrect( rect ) + ( pad_y - h );
            SetWindowPos( drawarea, HWND_TOP, 0, 0, w, h,
                          SWP_SIZE | SWP_NOZORDER | SWP_NOMOVE );
        }
    }

    if( ImgedIsDDE ) {
        sys_menu = GetSystemMenu( drawarea, FALSE );
        if( sys_menu != (HMENU)NULL ) {
            EnableMenuItem( sys_menu, SC_CLOSE, MF_GRAYED );
        }
    }

    BlowupImage( NULL, NULL );
    return( drawarea );
} /* WinNewDrawPad */

/*
 * WinCreateViewWin - creates the view window for the windows version
 */
HWND WinCreateViewWin( HWND hviewwnd, BOOL foneview, int *showstate,
                                                    int width, int height )
{
    HWND        hwnd;
    RECT        location;
    int         x,y;
    int         h_adj;
    int         v_adj;

    if ((hviewwnd) && (foneview)) {
        GetWindowRect( hviewwnd, &location );
        x = location.left;
        y = location.top;
    } else {
        x = ImgedConfigInfo.view_xpos;
        y = ImgedConfigInfo.view_ypos;
    }

    width += 2*BORDER_WIDTH;
    height += 2*BORDER_WIDTH;

    h_adj = 2*GetSystemMetrics(SM_CXDLGFRAME);
    v_adj = 2*GetSystemMetrics(SM_CYDLGFRAME) +
#ifndef __NT__
                GetSystemMetrics(SM_CYCAPTION) - 1;
#else
                GetSystemMetrics(SM_CYSMCAPTION) - 1;
#endif

#ifndef __NT__
    hwnd = CreateWindow(
            ViewWinClass,                       /* Window class name */
            "View",
            WS_POPUPWINDOW | WS_CAPTION |
            WS_VISIBLE | WS_CLIPSIBLINGS |
            WS_DLGFRAME,                        /* Window style */
            x,                                  /* Initial X position */
            y,                                  /* Initial Y position */
            h_adj + width,
            v_adj + height,
            HMainWindow,                        /* Parent window handle */
            (HMENU) NULL,                       /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */
#else
    hwnd = CreateWindowEx(
            WS_EX_TOOLWINDOW,
            ViewWinClass,                       /* Window class name */
            "View",
            WS_POPUPWINDOW | WS_CAPTION |
            WS_VISIBLE | WS_CLIPSIBLINGS |
            WS_DLGFRAME,                        /* Window style */
            x,                                  /* Initial X position */
            y,                                  /* Initial Y position */
            h_adj + width,
            v_adj + height,
            HMainWindow,                        /* Parent window handle */
            (HMENU) NULL,                       /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL);                              /* Create parameters */
#endif
    ShowWindow( hwnd, SW_HIDE );

    if( hwnd != (HWND)NULL ) {
        RECT    rect;
        int     w,h;
        _wpi_getclientrect( hwnd, &rect );
        w = _wpi_getwidthrect( rect );
        h = _wpi_getheightrect( rect );
        if( w < width || h < height ) {
            GetWindowRect( hwnd, &rect );
            w = _wpi_getwidthrect( rect ) + ( width - w );
            h = _wpi_getheightrect( rect ) + ( height - h );
            SetWindowPos( hwnd, HWND_TOP, 0, 0, w, h,
                          SWP_SIZE | SWP_NOZORDER | SWP_NOMOVE );
        }
    }

    if ( ImgedConfigInfo.show_state & SET_SHOW_VIEW ) {
        *showstate = SW_SHOWNORMAL;
    } else {
        *showstate = SW_HIDE;
    }
    return( hwnd );
} /* WinCreateViewWin */

