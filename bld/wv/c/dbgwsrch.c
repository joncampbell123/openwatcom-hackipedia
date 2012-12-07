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


#include "dbgdefn.h"
#include "dbginfo.h"
#include "dbgwind.h"
#include "dbgadget.h"
#include "wndregx.h"
#include "sortlist.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

extern char             *TxtBuff;
extern void             *SrchHistory;

extern cue_file_id      CueFileId( cue_handle * );
extern unsigned         CueFile( cue_handle *ch, char *file, unsigned max );
extern unsigned long    CueLine( cue_handle *ch );
extern void             WndFuncInspect( mod_handle mod );
extern a_window         *WndModInspect(mod_handle);
extern void             *OpenSrcFile(cue_handle *);
extern int              FReadLine(void   *,int ,int ,char *,int );
extern void             FDoneSource(void         *);
extern char             *DupStr(char*);
extern unsigned int     InfoSize(mod_handle ,unsigned int, unsigned );
extern char             *StrCopy(char*,char*);
extern int              ModCompare( mod_handle const *a, mod_handle const *b );
extern bool             DlgSearchAll(char**,void*);
extern bool             OpenGadget( a_window *, wnd_line_piece *, mod_handle, bool );
extern bool             CheckOpenGadget( a_window*, wnd_row, bool, mod_handle, bool, int );

#include "menudef.h"
static gui_menu_struct SrchMenu[] = {
    #include "menusrch.h"
};

enum {
    PIECE_OPENER,
    PIECE_MODULE,
    PIECE_SOURCE,
};


typedef struct {
    char        *source_line;
    mod_handle  mod;
    cue_file_id file_id;
    unsigned    open : 1;
} found_item;

typedef struct srch_window      srch_window;

typedef void (SRCH_WALKER)( srch_window * );

typedef struct a_cue {
    struct a_cue        *next;
    cue_handle          *ch;
    char                name[1];
} a_cue;

struct srch_window {
    void        *rx;
    char        *expr;
    int         max_mod_name;
    wnd_row     num_rows;
    char        magic_str[MAX_MAGIC_STR];
    found_item  *found;
    SRCH_WALKER *walk;
    void        *cookie;
    a_cue       *file_list;
    bool        ignore_case : 1;
    bool        use_rx : 1;
};

#define WndSrch( wnd ) ( (srch_window*)WndExtra( wnd ) )

static WNDNUMROWS SrchNumRows;
static int SrchNumRows( a_window *wnd )
{
    return( WndSrch( wnd )->num_rows );
}


OVL_EXTERN walk_result AddSrcFile( cue_handle *ch, void *d )
{
    a_cue       *file;
    srch_window *srch = d;
    int         len;

    len = CueFile( ch, NULL, 0 ) + 1;
    file = WndMustAlloc( sizeof( a_cue ) + cue_SIZE + len );
    file->ch = (cue_handle*)((char*)file + sizeof( a_cue ) + len  );
    CueFile( ch, file->name, len );
    HDLAssign( cue, file->ch, ch );
    file->next = srch->file_list;
    srch->file_list = file;
    return( WR_CONTINUE );
}

OVL_EXTERN walk_result SearchSrcFile( srch_window *srch, cue_handle *ch )
{
    void        *viewhndl;
    char        *pos,*endpos;
    found_item  *found;
    unsigned    i;
    int         len;

    viewhndl = OpenSrcFile( ch );
    if( viewhndl == NULL ) return( WR_CONTINUE );
    CueFile( ch, TxtBuff, TXT_LEN );
    WndStatusText( TxtBuff );
    for( i = 1;; ++i ) {
        len = FReadLine( viewhndl, i, 0, TxtBuff, TXT_LEN );
        if( len < 0 ) break;
        TxtBuff[ len ] = '\0';
        pos = TxtBuff;
        endpos = NULL;
        if( WndRXFind( srch->rx, &pos, &endpos ) ) {
            found = WndRealloc( srch->found,
                                (srch->num_rows+1)*sizeof( found_item ) );
            if( found == NULL ) break;
            srch->found = found;
            found[ srch->num_rows ].mod = CueMod( ch );
            found[ srch->num_rows ].file_id = CueFileId( ch );
            found[ srch->num_rows ].open = FALSE;
            found[ srch->num_rows ].source_line = DupStr( TxtBuff );
            srch->num_rows++;
            len = ModName( CueMod( ch ), NULL, 0 );
            if( len > srch->max_mod_name ) srch->max_mod_name = len;
            break;
        }
    }
    FDoneSource( viewhndl );
    return( WR_CONTINUE );
}

OVL_EXTERN walk_result BuildFileList( mod_handle mh, void *d )
{
    if( ModHasInfo( mh, HK_CUE ) == DS_OK ) {
        WalkFileList( mh, AddSrcFile, d );
    }
    return( WR_CONTINUE );
}


static int CueCompare( void *pa, void *pb )
{
    return( strcmp( (*(a_cue **)pa)->name, (*(a_cue **)pb)->name ) );
}

OVL_EXTERN void GlobalModWalker( srch_window *srch )
{
    a_cue       *file,*next;

    WalkModList( NO_MOD, BuildFileList, srch );
    srch->file_list = SortLinkedList( srch->file_list,
                offsetof( a_cue, next ), CueCompare, WndAlloc, WndFree );
    for( file = srch->file_list; file != NULL; file = file->next ) {
        if( file->next != NULL && strcmp( file->name, file->next->name ) == 0 ) continue;
        SearchSrcFile( srch, file->ch );
    }
    for( file = srch->file_list; file != NULL; file = next ) {
        next = file->next;
        WndFree( file );
    }
    srch->file_list = NULL;
}


OVL_EXTERN void NoModWalker( srch_window *srch )
{
    srch = srch;
}


OVL_EXTERN int FoundCompare( const void *a, const void *b )
{
    return( ModCompare( &((found_item const *)a)->mod, &((found_item const *)b)->mod ) );
}

static  void    SrchFreeFound( srch_window *srch )
{
    int         i;

    for( i = 0; i < srch->num_rows; ++i ) {
        WndFree( srch->found[ i ].source_line );
    }
    WndFree( srch->found );
    srch->found = NULL;
    srch->num_rows = 0;
}

static void     SrchInit( a_window *wnd )
{
    srch_window *srch = WndSrch( wnd );

    SrchFreeFound( srch );
    srch->max_mod_name = 0;
    srch->ignore_case = SrchIgnoreCase;
    srch->use_rx = SrchRX;
    strcpy( srch->magic_str, SrchMagicChars );
    srch->walk( srch );
    if( srch->num_rows != 0 ) {
        qsort( srch->found, srch->num_rows, sizeof( found_item ), FoundCompare );
    }
    WndZapped( wnd );
}


static  WNDMENU SrchMenuItem;
static void     SrchMenuItem( a_window *wnd, unsigned id, int row, int piece )
{
    srch_window *srch = WndSrch( wnd );
    a_window    *new;

    piece=piece;
    switch( id ) {
    case MENU_INITIALIZE:
        WndMenuEnable( wnd, MENU_SEARCH_SOURCE,
                row != WND_NO_ROW && row <= srch->num_rows );
        break;
    case MENU_SEARCH_SOURCE:
        new = WndModInspect( srch->found[ row ].mod );
        if( new == NULL ) break;
        SrchIgnoreCase = srch->ignore_case;
        WndSetMagicStr( srch->use_rx ? srch->magic_str : LIT( Empty ) );
        WndSetSrchItem( new, srch->expr );
        WndSearch( new, TRUE, 1 );
        break;
    }
}


static WNDGETLINE SrchGetLine;
static  bool    SrchGetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    srch_window *srch = WndSrch( wnd );
    found_item  *found;

    if( row >= srch->num_rows ) return( FALSE );
    if( srch->found == NULL ) return( FALSE );
    found = &srch->found[ row ];
    switch( piece ) {
    case PIECE_OPENER:
        found->open = OpenGadget( wnd, line, found->mod, TRUE );
        return( TRUE );
    case PIECE_MODULE:
        ModName( found->mod, TxtBuff, TXT_LEN );
        line->text = TxtBuff;
        line->indent = MaxGadgetLength;
        line->extent = WND_MAX_EXTEND;
        return( TRUE );
    case PIECE_SOURCE:
        line->indent = MaxGadgetLength;
        line->indent += ( srch->max_mod_name + 2 ) * WndAvgCharX( wnd );
        line->tabstop = FALSE;
        line->use_prev_attr = TRUE;
        if( found->source_line == NULL ) return( FALSE );
        line->text = found->source_line;
        return( TRUE );
    default:
        return( FALSE );
    }
}



extern wnd_info SrchInfo;
static WNDREFRESH SrchRefresh;
static void     SrchRefresh( a_window *wnd )
{
    srch_window *srch = WndSrch( wnd );
    found_item  *found;
    int         i;

    if( ( WndFlags & ~UP_OPEN_CHANGE ) & SrchInfo.flags ) {
        if( WndFlags & UP_SYMBOLS_LOST ) {
            srch->walk = NoModWalker;
        }
        SrchInit( wnd );
    } else {
        for( i = 0; i < srch->num_rows; ++i ) {
            found = &srch->found[ i ];
            found->open = CheckOpenGadget( wnd, i, found->open,
                           found->mod, TRUE, PIECE_OPENER );
        }
    }
}


static WNDCALLBACK SrchEventProc;
static bool SrchEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    srch_window *srch = WndSrch( wnd );

    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        SrchInit( wnd );
        WndSetKey( wnd, PIECE_MODULE );
        return( TRUE );
    case GUI_DESTROY :
        WndFreeRX( srch->expr );
        WndFreeRX( srch->rx );
        SrchFreeFound( srch );
        WndFree( srch );
        return( TRUE );
    }
    return( FALSE );
}

wnd_info SrchInfo = {
    SrchEventProc,
    SrchRefresh,
    SrchGetLine,
    SrchMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    WndFirstMenuItem,
    SrchNumRows,
    NoNextRow,
    NoNotify,
    UP_SYMBOLS_LOST+UP_OPEN_CHANGE,
    DefPopUp( SrchMenu ),
};

static a_window *DoWndSrchOpen( char *expr, SRCH_WALKER *walk, void *cookie )
{
    srch_window *srch;
    void        *rx;

    srch = WndMustAlloc( sizeof( srch_window ) );
    srch->file_list = NULL;
    srch->expr = DupStr( expr );
    srch->cookie = cookie;
    srch->found = NULL;
    srch->num_rows = 0;
    srch->walk = walk;
    rx = WndCompileRX( expr );
    if( srch->expr == NULL || rx == NULL ) {
        WndFree( srch->expr );
        WndFree( rx );
        WndFree( srch );
        return( NULL );
    }
    srch->rx = rx;
    return( DbgWndCreate( LIT( WindowSearch ), &SrchInfo, WND_ALL, srch, &SrchIcon ) );
}

a_window *WndSrchOpen( char *expr )
{
    return( DoWndSrchOpen( expr, GlobalModWalker, NULL ) );
}
