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
* Description:  Status line window.
*
****************************************************************************/


#include "vi.h"
#include "color.h"
#include "font.h"
#include "utils.h"
#include "statwnd.h"
#include "wstatus.h"
#include "watcom.h"
#include <assert.h>

static BOOL Init( window *, void * );
static BOOL Fini( window *, void * );

static BOOL     capIndex = -1;
static short    *sections;

window StatusBar = {
    &statusw_info,
    { 0, 0, 0, 0 },
    Init,
    Fini
};

void StatusWndSetSeparatorsWithArray( short *source, int num )
{
    status_block_desc   *list;
    int                 i;

    assert( num > 0 );
    list = MemAlloc( num * sizeof( status_block_desc ) );
    for( i = 0; i < num; i++ ) {
        list[i].separator_width = SEPERATOR_WIDTH;
        list[i].width = source[i];
        list[i].width_is_pixels = TRUE;
    }
    StatusWndSetSeparators( num, list );
    MemFree( list );
}

BOOL StatusWindowProc( HWND, unsigned, UINT, LONG );
static BOOL Init( window *w, void *parm )
{
    BOOL    rc;

    parm = parm;
    w = w;

    rc = StatusWndInit( InstanceHandle, StatusWindowProc, sizeof( LPVOID ),
                        (HCURSOR)NULLHANDLE );
#if defined( __NT__ )
    StatusWndChangeSysColors( GetSysColor( COLOR_BTNFACE ),
                              GetSysColor( COLOR_BTNTEXT ),
#else
    StatusWndChangeSysColors( GetRGB( statusw_info.text.background ),
                              GetRGB( statusw_info.text.foreground ),
#endif
                              GetSysColor( COLOR_BTNHIGHLIGHT ),
                              GetSysColor( COLOR_BTNSHADOW ) );
    if( NumStatusSections > 0 ) {
        StatusWndSetSeparatorsWithArray( StatusSections, NumStatusSections );
    }
    return( rc );
}

static BOOL Fini( window *w, void *parm )
{
    w = w;
    parm = parm;
    StatusWndFini();
    return( FALSE );
}

int setCursor( short x )
{
    int     i;
    for( i = 0; i < NumStatusSections; i++ ) {
        if( abs( x - (StatusSections[i]) ) < MOUSE_ALLOWANCE ) {
            CursorOp( COP_STATMOVE );
            return( i );
        }
    }
    CursorOp( COP_ARROW );
    return( -1 );
}

void processMouseMove( UINT w, LONG l )
{
    int         deep, delta, maxmove, movedby, i, next;
    short       x;
    int         secIndex;

    x = (signed_16)LOWORD( l ) - CURSOR_CORRECT;
    w = w;

    if( capIndex == -1 ) {
        setCursor( x );
        return;
    }

    // algorithm simpler if we index into array w/ 'stops' at either end
    secIndex = capIndex + 1;

    // shove bars to left
    deep = 0;
    delta = sections[secIndex] - x;
    while( delta > 0 && secIndex - deep > 0 ) {
        next = secIndex - deep;
        maxmove = sections[next] - sections[next - 1] - BOUNDARY_WIDTH;
        movedby = 0;
        if( maxmove > 0 ) {
            movedby = min( maxmove, delta );
            delta -= movedby;
        }
        for( i = next; i <= secIndex; i++ ) {
            sections[i] -= movedby;
        }
        deep++;
    }

    // shove bars to right
    deep = 0;
    delta = x - sections[secIndex];
    while( delta > 0 && secIndex + deep <= NumStatusSections ) {
        next = secIndex + deep;
        maxmove = sections[next + 1] - sections[next] - BOUNDARY_WIDTH;
        movedby = 0;
        if( maxmove > 0 ) {
            movedby = min( maxmove, delta );
            delta -= movedby;
        }
        for( i = secIndex; i <= next; i++ ) {
            sections[i] += movedby;
        }
        deep++;
    }

    StatusWndSetSeparatorsWithArray( sections + 1, NumStatusSections );
    InvalidateRect( StatusWindow, NULL, TRUE );
    UpdateWindow( StatusWindow );
}

void processLButtonDown( HWND hwnd, UINT w, LONG l )
{
    RECT        rect;

    w = w;
    capIndex = setCursor( (signed_16)LOWORD( l ) - CURSOR_CORRECT );
    if( capIndex != -1 ) {
        SetCapture( hwnd );
        sections = MemAlloc( (NumStatusSections + 2) * sizeof( short ) );
        GetClientRect( StatusWindow, &rect );
        memcpy( sections + 1, StatusSections,
                NumStatusSections * sizeof( short ) );
        sections[0] = 0;
        sections[NumStatusSections + 1] = rect.right - BOUNDARY_WIDTH +
                                          CURSOR_CORRECT;
    }
}

void processLButtonUp( void )
{
    if( capIndex != -1 ) {
        CursorOp( COP_ARROW );
        ReleaseCapture();
        capIndex = -1;
        memcpy( StatusSections, sections + 1,
                NumStatusSections * sizeof( short ) );
        MemFree( sections );
    }
}

/*
 * StatusWindowProc - handle messages for the status window
 */
BOOL StatusWindowProc( HWND hwnd, unsigned msg, UINT w, LONG l )
{
    w = w;
    l = l;
    switch( msg ) {
    case WM_CREATE:
        SetWindowLong( hwnd, 0, (LONG)(LPVOID)&StatusBar );
        break;
    case WM_SETFOCUS:
        SetFocus( Root );
        return( TRUE );
    case WM_MOUSEMOVE:
        processMouseMove( w, l );
        return( TRUE );
    case WM_LBUTTONDOWN:
        processLButtonDown( hwnd, w, l );
        return( TRUE );
    case WM_LBUTTONUP:
        processLButtonUp();
        return( TRUE );
    }
    return( FALSE );

} /* StatusWindowProc */

/*
 * NewStatWindow - create a new status window
 */
window_id NewStatWindow( void )
{
    window_id   stat;
    RECT        size;

    size = StatusBar.area;
    size.left -= 1;
    size.right += 1;
    size.bottom += 1;
    stat = StatusWndCreate( Root, &size, InstanceHandle, NULL );
    GetWindowRect( stat, &size );
    return( stat );

} /* NewStatWindow */

/*
 * StatusLine - display text on a status line
 */
void StatusLine( int line, char *str, int format )
{
    HDC         hdc;
    font_type   font;
    HFONT       hfont;

    format = format;

    if( line != 1 ) {
        return;
    }
    if( !AllowDisplay || BAD_ID( StatusWindow ) ) {
        return;
    }
    hdc = TextGetDC( StatusWindow, WIN_STYLE( &StatusBar ) );
    font = WIN_FONT( &StatusBar );
    hfont = FontHandle( font );
    StatusWndDrawLine( hdc, hfont, str, (UINT) -1 );
    TextReleaseDC( StatusWindow, hdc );

} /* StatusLine */
