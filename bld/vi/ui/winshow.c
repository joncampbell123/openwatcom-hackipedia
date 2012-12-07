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


#include "vi.h"
#include "win.h"


/*
 * reDisplayWindow - redisplay the saved window text
 */
static void reDisplayWindow( window_id wn )
{
    wind                *w;
    char_info           *txt;
    char                *over;
    char_info           _FAR *scr;
#ifdef __VIO__
    unsigned            oscr;
#endif
    int                 j, i;

    if( EditFlags.Quiet ) {
        return;
    }
    w = AccessWindow( wn );

    /*
     * re-display text area
     */
    txt = (char_info *) w->text;
    over = w->overlap;
    for( j = w->y1; j <= w->y2; j++ ) {
        scr = (char_info _FAR *) &Scrn[(w->x1 + j * WindMaxWidth) * sizeof( char_info )];
#ifdef __VIO__
        oscr = (unsigned) ((char *)scr - Scrn);
#endif
        for( i = w->x1; i <= w->x2; i++ ) {
            if( *over++ == NO_CHAR ) {
                WRITE_SCREEN( *scr, *txt );
            }
            scr++;
            txt++;
        }
#ifdef __VIO__
        MyVioShowBuf( oscr, w->width );
#endif
    }

    DrawBorder( wn );
    ReleaseWindow( w );

} /* reDisplayWindow */

/*
 * MoveWindowToFront - bring a window forward
 */
void MoveWindowToFront( window_id wn )
{
    if( !TestOverlap( wn ) ) {
        return;
    }
    MoveWindowToFrontDammit( wn, TRUE );

} /* MoveWindowToFront */

/*
 * MoveWindowToFrontDammit - bring a window forward
 */
void MoveWindowToFrontDammit( window_id wn, bool scrflag )
{
    wind        *w;

    if( wn == NO_WINDOW ) {
        return;
    }
    w = Windows[wn];

    RestoreOverlap( wn, scrflag );
    ResetOverlap( w );
    MarkOverlap( wn );
    reDisplayWindow( wn );

} /* MoveWindowToFrontDammit */

/*
 * InactiveWindow - display a window as inactive
 */
void InactiveWindow( window_id wn )
{
    wind        *w;
    vi_color    c;

    if( wn == NO_WINDOW ) {
        return;
    }

    w = Windows[wn];
    if( w == NULL ) {
        return;
    }

    if( !w->has_border ) {
        return;
    }

    /*
     * change the border color
     */
    c = w->border_color1;
    w->border_color1 = InactiveWindowColor;
    DrawBorder( wn );
    w->border_color1 = c;

} /* InactiveWindow */

void ActiveWindow( window_id a ) { a = a; }

/*
 * WindowTitleAOI - set the title of a window, active or inactive
 */
void WindowTitleAOI( window_id wn, char *title, bool active )
{
    wind        *w;

    w = Windows[wn];

    MemFree( w->title );
    if( title == NULL ) {
        w->title = NULL;
    } else {
        AddString( &(w->title), title );
    }
    if( active ) {
        DrawBorder( wn );
    } else {
        InactiveWindow( wn );
    }

} /* WindowTitleAOI */


/*
 * WindowTitle - set window title, active border
 */
void WindowTitle( window_id id, char *name )
{
    WindowTitleAOI( id, name, TRUE );

} /* WindowTitle */

/*
 * WindowTitleInactive - set window title, inactive border
 */
void WindowTitleInactive( window_id id, char *name )
{
    WindowTitleAOI( id, name, FALSE );

} /* WindowTitleInactive */

/*
 * ClearWindow - do just that
 */
void ClearWindow( window_id wn )
{
    wind                *w;
    char                *over;
    char_info           *txt;
    char_info           _FAR *scr;
    int                 j, i, shift, addr;
    char_info           what;
#ifdef __VIO__
    unsigned            oscr;
#endif

    if( EditFlags.Quiet ) {
        return;
    }
    w = AccessWindow( wn );
    shift = (int) w->has_border;

    /*
     * clear text area
     */
    what.ch = ' ';
    what.attr = MAKE_ATTR( w, w->text_color, w->background_color );
    addr = shift * w->width + shift;
    for( j = w->y1 + shift; j <= w->y2 - shift; j++ ) {
        scr = (char_info _FAR *) &Scrn[(w->x1 + shift + j * WindMaxWidth) *
                                       sizeof( char_info )];
#ifdef __VIO__
        oscr = (unsigned) ((char *) scr - Scrn);
#endif
        txt = (char_info *) &(w->text[sizeof( char_info ) * addr]);
        over = &(w->overlap[addr]);
        addr += w->width;
        for( i = w->x1 + shift; i <= w->x2 - shift; i++ ) {
            if( *over++ == NO_CHAR ) {
                WRITE_SCREEN( *scr, what );
            }
            WRITE_SCREEN_DATA( *txt, what );
            txt++;
            scr++;
        }
#ifdef __VIO__
        MyVioShowBuf( oscr, w->width - 2 * shift ); // inside of window only
#endif
    }

    ReleaseWindow( w );

} /* ClearWindow */

/*
 * InsideWindow - test if coordinates are in window or on border
 */
bool InsideWindow( window_id id, int x, int y )
{
    wind        *w;

    w = Windows[id];
    if( !w->has_border ) {
        return( TRUE );
    }
    if( x == 0 || y == 0 ) {
        return( FALSE );
    }
    if( x == w->width - 1 ) {
        return( FALSE );
    }
    if( y == w->height - 1 ) {
        return( FALSE );
    }
    return( TRUE );

} /* InsideWindow */
