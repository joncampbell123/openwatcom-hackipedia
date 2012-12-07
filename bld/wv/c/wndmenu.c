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
* Description:  Top level debugger menu.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgtoken.h"
#include "dbgwind.h"
#include "dbgerr.h"
#include "trpcore.h"
#include "madcli.h"
#include <ctype.h>
#include <string.h>

extern a_window         *WndClassInspect( wnd_class class );
extern void             DebugExit( void );
extern void             DlgSource( void );
extern char             *ReScan( char * );
extern void             ProcSearchAll( void );
extern void             ProcWndSearch( a_window * );
extern void             ProcWndFindNext( a_window * );
extern void             ProcWndFindPrev( a_window * );
extern void             DlgOptSet( void );
extern void             DlgCmd( void );
extern void             DoSystem( char *, unsigned int, int );
extern void             ExecTrace( trace_cmd_type, debug_level );
extern unsigned         Go( bool );
extern void             ReStart( void );
extern bool             SetProgStartHook( bool );
extern void             GoToReturn( void );
extern void             DlgNewProg( void );
extern bool             DlgBreak( address );
extern void             BreakSave( bool );
extern void             ReplaySave( bool );
extern void             BrkEnableAll( void );
extern void             BrkDisableAll( void );
extern void             BrkClearAll( void );
extern void             ConfigSave( bool );
extern void             ReqEOC( void );
extern bool             ScanEOC( void );
extern unsigned         ScanCmd( char *cmd_table );
extern bool             ScanItem( bool, char **, unsigned int * );
extern wnd_class        ReqWndName( void );
extern char             *StrCopy( char *, char * );
extern void             FileBrowse( void );
extern void             LastMachState( void );
extern void             LastStackPos( void );
extern void             MoveStackPos( int by );
extern void             PosMachState( int rel_pos );
extern void             Flip( unsigned );
extern void             WndAsmInspect( address );
extern void             WndSrcInspect( address );
extern void             DlgAbout( void );
extern void             FontChange( void );
extern void             ToggleHardMode( void );
extern void             WndDumpPrompt( a_window * );
extern void             WndDumpLog( a_window * );
extern char             *GetCmdName( int );
extern void             DlgWndSet( void );
extern bool             VarInfoRelease( void );
extern void             SkipToAddr( address );
extern void             GoToAddr( address );
extern address          GetCodeDot( void );
extern void             ToggleBreak( address );
extern void             FuncNewMod( a_window *wnd, mod_handle mod );
extern void             GlobNewMod( a_window *wnd, mod_handle mod );
extern void             ModNewHandle( a_window *wnd, mod_handle mod );
extern void             GoHome( void );
extern void             DoProcHelp( gui_help_actions );
extern void             DlgBreakDLL( void );
extern bool             DebugScreen( void );
extern void             UnknownScreen( void );
extern bool             DlgCodeAddr( char *title, address *value );
extern bool             DlgDataAddr( char *title, address *value );
extern void             WndAddrInspect( address addr );
extern void             RegFindData( mad_type_kind kind, mad_reg_set_data const **pdata );

extern address          NilAddr;
extern debug_level      DbgLevel;
extern void             *BrkList;
extern system_config    SysConfig;

#include "menudef.h"
static gui_menu_struct FileMenu[] = {
    #include "mmfile.h"
};

static gui_menu_struct SearchMenu[] = {
    #include "mmsearch.h"
};

static gui_menu_struct RunMenu[] = {
    #include "mmrun.h"
};

static gui_menu_struct BreakMenu[] = {
    #include "mmbreak.h"
};

static gui_menu_struct CodeMenu[] = {
    #include "mmcode.h"
};

static gui_menu_struct DataMenu[] = {
    #include "mmdat.h"
};

static gui_menu_struct UndoMenu[] = {
    #include "mmundo.h"
};

static gui_menu_struct WindowMenu[] = {
    #include "mmwind.h"
};

static gui_menu_struct HelpMenu[] = {
    #include "mmhelp.h"
};

static gui_menu_struct DummyMenu;


gui_menu_struct WndMainMenu[] = {
    MENU_CASCADE( MENU_MAIN_FILE, MainMenuFile, FileMenu )
    MENU_CASCADE( MENU_MAIN_RUN, MainMenuRun, RunMenu )
    MENU_CASCADE( MENU_MAIN_BREAK, MainMenuBreak, BreakMenu )
    MENU_CASCADE( MENU_MAIN_CODE, MainMenuCode, CodeMenu )
    MENU_CASCADE( MENU_MAIN_DATA, MainMenuData, DataMenu )
    MENU_CASCADE( MENU_MAIN_UNDO, MainMenuUndo, UndoMenu )
    MENU_CASCADE( MENU_MAIN_SEARCH, MainMenuSearch, SearchMenu )
    MENU_CASCADE( MENU_MAIN_WINDOW, MainMenuWindow, WindowMenu )
    MENU_CASCADE( MENU_MAIN_ACTION, MainMenuAction, &DummyMenu )
    MENU_CASCADE( MENU_MAIN_HELP, MainMenuHelp, HelpMenu )
};

int     WndNumMenus = { WndMenuSize( WndMainMenu ) };

wnd_info *WndInfoTab[] = {
#define pick( a,b,c,d,e,f ) &d,
#include "wndnames.h"
};

char MainTab[] = { "MAin\0" };

extern void PlayDead( bool dead )
{
    WndIgnoreAllEvents = dead;
    #if defined(__GUI__) && (defined(__WINDOWS__) || defined(__NT__))
        if( dead ) {
            UnknownScreen(); // the trap file might steal focus!
        } else {
            DebugScreen();
        }
    #endif
}


static bool StrAmpEqual( char *str, char *menu, int len )
{
    char        *p;
    char        menu_accel;

    while( *str == ' ' ) {
        ++str;
        --len;
    }
    // first look for a matching accelerator character
    menu_accel = 0;
    for( p = str; *p; ++p ) {
        if( *p == '&' ) {
            menu_accel = tolower( p[1] );
            break;
        }
    }
    if( menu_accel ) {
        for( p = menu; *p; ++p ) {
            if( *p == '&' ) {
                if( tolower( p[1] ) == menu_accel ) return( TRUE );
                break;
            }
        }
    }
    while( len > 0 && str[ len-1 ] == ' ' ) {
        --len;
    }
    while( --len >= 0 ) {
        if( *menu == '&' ) ++menu;
        if( tolower( *menu ) != tolower( *str ) ) return( FALSE );
        if( *menu == '\0' ) return( FALSE );
        ++menu;
        ++str;
    }
    return( TRUE );
}

static gui_menu_struct *FindMainMenu( gui_menu_struct *main, int size )
{
    char                *start;
    unsigned            len;

    if( !ScanItem( TRUE, &start, &len ) ) return( NULL );
    while( --size >= 0 ) {
        if( StrAmpEqual( start, main->label, len ) ) break;
        ++main;
    }
    if( size < 0 ) return( NULL );
    return( main );
}


char *GetMenuLabel( unsigned size,
                    gui_menu_struct *menu, unsigned id, char *buff, bool strip_amp )
{
    char        *p;

    while( size != 0 ) {
        if( menu->id == id ) {
            for( p = menu->label; *p != '\0'; ++p ) {
                if( *p == '&' && strip_amp ) continue;
                if( *p == '\t' ) break;
                *buff++ = *p;
            }
            *buff = '\0';
            return( buff );
        }
        if( menu->num_child_menus != 0 ) {
            p = GetMenuLabel( menu->num_child_menus, menu->child, id, buff, strip_amp );
            if( p != NULL ) return( p );
        }
        --size;
        ++menu;
    }
    return( NULL );
}

static gui_menu_struct *FindSubMenu( char *start, unsigned len, gui_menu_struct *child, int size )
{
    gui_menu_struct     *sub;

    while( --size >= 0 ) {
        if( StrAmpEqual( start, child->label, len ) ) {
            return( child );
        }
        if( child->num_child_menus != NULL ) {
            sub = FindSubMenu( start, len, child->child, child->num_child_menus );
            if( sub != NULL ) return( sub );
        }
        ++child;
    }
    return( NULL );
}

#ifdef DEADCODE
int FindMenuLen( gui_menu_struct *child )
{
    char        *p;

    p = child->label + strlen( child->label ) - 1;
    while( *p == ' ' ) --p;
    if( *p == ')' ) {
        while( *p != '(' ) --p;
        --p;
        while( *p == ' ' ) --p;
    }
    return( p - child->label + 1 );
}
#endif


void AccelMenuItem( gui_menu_struct *menu, bool is_main )
{
    a_window    *wnd = WndFindActive();

    if( is_main ) {
        WndMainMenuProc( wnd, menu->id );
    } else {
        WndKeyPopUp( wnd, menu );
        WndNoSelect( wnd );
    }
}

static bool DoProcAccel( bool add_to_menu, gui_menu_struct **menu,
                  gui_menu_struct **parent, int *num_siblings, wnd_class class )
{
    gui_menu_struct     *main;
    gui_menu_struct     *child;
    wnd_info            *info;
    a_window            *wnd;
    char                *start;
    unsigned            len;

    *menu = *parent = NULL;
    child = NULL;
    if( ScanCmd( MainTab ) ) {
        main = FindMainMenu( WndMainMenu, ArraySize( WndMainMenu ) );
        if( main == NULL ) {
            if( add_to_menu ) return( TRUE );
            Error( ERR_NONE, LIT( ERR_WANT_MENU_ITEM ) );
        }
        if( ScanItem( TRUE, &start, &len ) ) {
            child = FindSubMenu( start, len, main->child, main->num_child_menus );
        }
        if( child == NULL ) {
            if( add_to_menu ) return( TRUE );
            Error( ERR_NONE, LIT( ERR_WANT_MENU_ITEM ) );
        }
        *menu = child;
        *parent = main->child;
        *num_siblings = main->num_child_menus;
        if( add_to_menu ) return( TRUE );
        ReqEOC();
        AccelMenuItem( child, TRUE );
    } else {
        info = WndInfoTab[ class ];
        if( ScanItem( TRUE, &start, &len ) ) {
            child = FindSubMenu( start, len, info->popupmenu, info->num_popups );
        }
        if( child == NULL ) {
            if( add_to_menu ) return( FALSE );
            Error( ERR_NONE, LIT( ERR_WANT_MENU_ITEM ) );
        }
        *menu = child;
        *parent = info->popupmenu;
        *num_siblings = info->num_popups;
        if( add_to_menu ) return( FALSE );
        wnd = WndFindActive();
        if( WndClass( wnd ) != class ) Error( ERR_NONE, LIT( ERR_MACRO_NOT_VALID ) );
        ReqEOC();
        AccelMenuItem( child, FALSE );
    }
    return( FALSE );
}

extern void ProcAccel( void )
{
    gui_menu_struct     *menu,*parent;
    int                 num_sibs;

    DoProcAccel( FALSE, &menu, &parent, &num_sibs, WndClass( WndFindActive() ));
}

static void FreeLabels( gui_menu_struct *menu, int num_menus )
{
    while( --num_menus >= 0 ) {
        if( menu->id != MENU_MAIN_ACTION && menu->child != NULL ) {
            FreeLabels( menu->child, menu->num_child_menus );
        }
        if( menu->style & WND_MENU_ALLOCATED ) {
            menu->style &= ~WND_MENU_ALLOCATED;
            WndFree( menu->label );
            WndFree( menu->hinttext );
        }
        ++menu;
    }
}


static void LoadLabels( gui_menu_struct *menu, int num_menus )
{
    while( --num_menus >= 0 ) {
        if( menu->child != NULL ) {
            LoadLabels( menu->child, menu->num_child_menus );
        }
        if( !( menu->style & (GUI_SEPARATOR|WND_MENU_ALLOCATED) ) ) {
            menu->label = WndLoadString( (int)menu->label );
            menu->hinttext = WndLoadString( (int)menu->hinttext );
            menu->style |= WND_MENU_ALLOCATED;
        }
        ++menu;
    }
}

void SetBrkMenuItems( void )
{
    bool        on;

    on = ( BrkList != NULL );
    WndEnableMainMenu( MENU_MAIN_BREAK_CLEAR_ALL, on );
    WndEnableMainMenu( MENU_MAIN_BREAK_DISABLE_ALL, on );
    WndEnableMainMenu( MENU_MAIN_BREAK_ENABLE_ALL, on );
    WndEnableMainMenu( MENU_MAIN_BREAK_SAVE_ALL, on );
}

void SetLogMenuItems( bool active )
{
    WndEnableMainMenu( MENU_MAIN_WINDOW_LOG, !active );
}

void SetMADMenuItems( void )
{
    const mad_reg_set_data      *rsd;

    RegFindData( MTK_FLOAT, &rsd );
    WndEnableMainMenu( MENU_MAIN_OPEN_FPU, rsd != NULL );
    RegFindData( MTK_CUSTOM, &rsd );
    WndEnableMainMenu( MENU_MAIN_OPEN_MMX, rsd != NULL );
    RegFindData( MTK_XMM, &rsd );
    WndEnableMainMenu( MENU_MAIN_OPEN_XMM, rsd != NULL );
}

void SetTargMenuItems( void )
{
    WndEnableMainMenu( MENU_MAIN_BREAK_ON_DLL, _IsOn( SW_HAVE_RUNTIME_DLLS ) );
#if defined(__GUI__) && defined(__OS2__)
    WndEnableMainMenu( MENU_MAIN_FILE_FONT, FALSE );
#endif
    SetMADMenuItems();
}

static void ForAllMenus( void (*rtn)( gui_menu_struct *menu, int num_menus ) )
{
    int         i;

    rtn( WndMainMenu, ArraySize( WndMainMenu ) );
    for( i = 0; i < WND_NUM_CLASSES; ++i ) {
        rtn( WndInfoTab[i]->popupmenu, WndInfoTab[i]->num_popups );
    }
}

extern void InitMenus( void )
{
    int         i;

    for( i = 0; i < ArraySize( WndMainMenu ); ++i ) {
        if( WndMainMenu[i].id == MENU_MAIN_ACTION ) {
            WndMainMenu[i].style |= WND_MENU_POPUP;
        }
        if( WndMainMenu[i].id == MENU_MAIN_WINDOW ) {
            WndMainMenu[i].style |= GUI_MDIWINDOW;
        }
    }
    ForAllMenus( LoadLabels );
    SetBrkMenuItems();
}

extern void FiniMenus( void )
{
    ForAllMenus( FreeLabels );
}

void WndMenuSetHotKey( gui_menu_struct *menu, bool is_main, char *key )
{
    char                *p;
    char                *new;

    if( menu == NULL ) return;
    for( p = menu->label; *p != '\0'; ++p ) {
        if( *p == '\t' ) {
            *p = '\0';
            break;
        }
    }
    new = WndAlloc( p - menu->label + 1 + strlen( key ) + 1 );
    p = StrCopy( menu->label, new );
    *p++ = '\t';
    StrCopy( key, p );
    if( menu->style & WND_MENU_ALLOCATED ) WndFree( menu->label );
    menu->style |= WND_MENU_ALLOCATED;
    menu->label = new;
    if( is_main ) {
        WndSetMainMenuText( menu );
    }
}


extern gui_menu_struct *AddMenuAccel( char *key, char *cmd,
                                      wnd_class class, bool *is_main )
{
    char                *old;
    gui_menu_struct     *menu,*parent;
    int                 num_sibs;

    old = ReScan( cmd );
    menu = NULL;
    if( ScanCmd( GetCmdName( CMD_ACCEL ) ) ) {
        *is_main = DoProcAccel( TRUE, &menu, &parent, &num_sibs, class );
    }
    ReScan( old );
    if( menu == NULL || !ScanEOC() ) return( NULL );
    WndMenuSetHotKey( menu, *is_main, key );
    return( menu );
}


static void     DoMatch( void )
{
    a_window    *wnd;

    wnd = WndFindActive();
    if( wnd == NULL ) return;
    if( WndKeyPiece( wnd ) == WND_NO_PIECE ) {
        Error( ERR_NONE, LIT( ERR_MATCH_NOT_SUPPORTED ) );
    } else {
        WndSetSwitches( wnd, WSW_CHOOSING );
    }
}


static  void    ExamMemAt( void )
{
    address     addr;

    addr = NilAddr;
    if( DlgDataAddr( LIT( Mem_Addr ), &addr ) ) {
        WndAddrInspect( addr );
    }
}


static  void    GoToPromptedAddr( void )
{
    address     addr;

    addr = NilAddr;
    if( DlgCodeAddr( LIT( GoWhere ), &addr ) ) {
        GoToAddr( addr );
    }
}

extern bool     WndMainMenuProc( a_window *wnd, unsigned id )
{
    bool        save;

    wnd=wnd;
    switch( id ) {
#if defined(__GUI__)
    case MENU_MAIN_FILE_FONT:
        FontChange();
        break;
#endif
    case MENU_MAIN_FILE_VIEW:
        FileBrowse();
        break;
#if !defined(__GUI__) && !defined(__WINDOWS__) && !defined(__NT__)
    case MENU_MAIN_FILE_SYSTEM:
        DoSystem( NULL, 0, 0 );
        break;
#endif
    case MENU_MAIN_FILE_OPTIONS:
        DlgOptSet();
        break;
    case MENU_MAIN_FILE_COMMAND:
        DlgCmd();
        break;
    case MENU_MAIN_FILE_EXIT:
        DebugExit();
        break;
    case MENU_MAIN_FILE_ABOUT:
        DlgAbout();
        VarInfoRelease();
        break;
    case MENU_MAIN_SEARCH_ALL:
        ProcSearchAll();
        break;
    case MENU_MAIN_SEARCH_FIND:
        ProcWndSearch(WndFindActive());
        break;
    case MENU_MAIN_SEARCH_NEXT:
        ProcWndFindNext(WndFindActive());
        break;
    case MENU_MAIN_SEARCH_PREV:
        ProcWndFindPrev(WndFindActive());
        break;
    case MENU_MAIN_SEARCH_MATCH:
        DoMatch();
        break;
    case MENU_TOOL_GO:
    case MENU_MAIN_RUN_GO:
        Go( TRUE );
        break;
    case MENU_MAIN_RUN_SKIP_TO_CURSOR:
        SkipToAddr( GetCodeDot() );
        break;
    case MENU_MAIN_RUN_TO_CURSOR:
        GoToAddr( GetCodeDot() );
        break;
    case MENU_MAIN_BREAK_TOGGLE:
        ToggleBreak( GetCodeDot() );
        break;
    case MENU_MAIN_BREAK_AT_CURSOR:
        DlgBreak( GetCodeDot() );
        break;
    case MENU_MAIN_RUN_EXECUTE_TO:
        GoToPromptedAddr();
        break;
    case MENU_TOOL_HOME:
    case MENU_MAIN_HOME:
        GoHome();
        break;
    case MENU_TOOL_REDO:
    case MENU_MAIN_REDOIT:
        PosMachState( 1 );
        break;
    case MENU_TOOL_UNDO:
    case MENU_MAIN_UNDOIT:
        PosMachState( -1 );
        break;
    case MENU_TOOL_UP_STACK:
    case MENU_MAIN_UP_STACK:
        MoveStackPos( -1 );
        break;
    case MENU_TOOL_DOWN_STACK:
    case MENU_MAIN_DOWN_STACK:
        MoveStackPos( 1 );
        break;
    case MENU_TOOL_TRACE_OVER:
    case MENU_MAIN_RUN_TRACE_OVER:
        ExecTrace( TRACE_OVER, DbgLevel );
        break;
    case MENU_TOOL_STEP_INTO:
    case MENU_MAIN_RUN_STEP_INTO:
        ExecTrace( TRACE_INTO, DbgLevel );
        break;
    case MENU_MAIN_RUN_TRACE_NEXT:
        ExecTrace( TRACE_NEXT, DbgLevel );
        break;
    case MENU_TOOL_RETURN_TO_CALLER:
    case MENU_MAIN_RUN_RETURN_TO_CALLER:
        GoToReturn();
        break;
    case MENU_MAIN_FILE_OPEN:
        DlgNewProg();
        break;
    case MENU_MAIN_RUN_RESTART:
        ReStart();
        break;
    case MENU_MAIN_DEBUG_STARTUP:
        {
            bool old;

            old = SetProgStartHook( FALSE );
            ReStart();
            SetProgStartHook( old );
        }
        break;
    case MENU_MAIN_BREAK_CREATE_NEW:
        DlgBreak( NilAddr );
        break;
    case MENU_MAIN_BREAK_VIEW_ALL:
        WndClassInspect( WND_BREAK );
        break;
    case MENU_MAIN_BREAK_ON_DLL:
        DlgBreakDLL();
        break;
    case MENU_MAIN_BREAK_ON_DEBUG_MESSAGE:
        _SwitchToggle( SW_BREAK_ON_DEBUG_MESSAGE );
        WndCheckMainMenu( MENU_MAIN_BREAK_ON_DEBUG_MESSAGE, _IsOn( SW_BREAK_ON_DEBUG_MESSAGE ) );
        break;
    case MENU_MAIN_BREAK_CLEAR_ALL:
        BrkClearAll();
        break;
    case MENU_MAIN_BREAK_DISABLE_ALL:
        BrkDisableAll();
        break;
    case MENU_MAIN_BREAK_ENABLE_ALL:
        BrkEnableAll();
        break;
    case MENU_MAIN_BREAK_SAVE_ALL:
        BreakSave( TRUE );
        break;
    case MENU_MAIN_BREAK_RESTORE_ALL:
        BreakSave( FALSE );
        break;
    case MENU_MAIN_SAVE_REPLAY:
        ReplaySave( TRUE );
        break;
    case MENU_MAIN_RESTORE_REPLAY:
        ReplaySave( FALSE );
        break;
    case MENU_MAIN_FILE_SAVE_CONFIGURATION:
        ConfigSave( TRUE );
        break;
    case MENU_MAIN_FILE_LOAD_CONFIGURATION:
        save = WndStopRefresh( TRUE );
        ConfigSave( FALSE );
        WndStopRefresh( save );
        break;
    case MENU_MAIN_FILE_SOURCE_PATH:
        DlgSource();
        break;
    case MENU_MAIN_WINDOW_ZOOM:
        wnd = WndFindActive();
        if( wnd == NULL ) break;
        if( WndIsMaximized( wnd ) ) {
            WndRestoreWindow( wnd );
        } else {
            WndMaximizeWindow( wnd );
        }
        break;
    case MENU_MAIN_WINDOW_NEXT:
        WndChooseNew();
        break;
    case MENU_MAIN_WINDOW_PROGRAM:
        Flip( 0 );
        break;
    case MENU_MAIN_WINDOW_DUMP:
        WndDumpPrompt( WndFindActive() );
        break;
    case MENU_MAIN_WINDOW_LOG:
        WndDumpLog( WndFindActive() );
        break;
    case MENU_MAIN_WINDOW_SETTINGS:
        DlgWndSet();
        break;
#if 0 // defined(__GUI__) && (defined(__WINDOWS__) || defined(__OS2__)
    case MENU_MAIN_WINDOW_HARD_MODE:
        ToggleHardMode();
        break;
#endif
    case MENU_MAIN_OPEN_ASSEMBLY:
        WndAsmInspect( NilAddr );
        break;
    case MENU_MAIN_OPEN_CALL:
        WndClassInspect( WND_CALL );
        break;
    case MENU_MAIN_OPEN_LOG:
        WndClassInspect( WND_DIALOGUE );
        break;
    case MENU_MAIN_REPLAY:
        WndClassInspect( WND_REPLAY );
        break;
    case MENU_MAIN_OPEN_IMAGE:
        WndClassInspect( WND_IMAGE );
        break;
    case MENU_MAIN_OPEN_FPU:
        WndClassInspect( WND_FPU );
        break;
    case MENU_MAIN_OPEN_THREADS:
        WndClassInspect( WND_THREAD );
        break;
    case MENU_MAIN_OPEN_FUNCTIONS:
        wnd = WndClassInspect( WND_GBLFUNCTIONS );
        if( wnd == NULL ) break;
        FuncNewMod( wnd, NO_MOD );
        break;
    case MENU_MAIN_OPEN_FILESCOPE:
        WndClassInspect( WND_FILESCOPE );
        break;
    case MENU_MAIN_OPEN_GLOBALS:
        wnd = WndClassInspect( WND_GLOBALS );
        if( wnd == NULL ) break;
        GlobNewMod( wnd, NO_MOD );
        break;
    case MENU_MAIN_OPEN_LOCALS:
        WndClassInspect( WND_LOCALS );
        break;
    case MENU_MAIN_OPEN_MODULES:
        wnd = WndClassInspect( WND_MODULES );
        if( wnd == NULL ) break;
        ModNewHandle( wnd, NO_MOD );
        break;
    case MENU_MAIN_OPEN_REGISTER:
        WndClassInspect( WND_REGISTER );
        break;
    case MENU_MAIN_OPEN_MMX:
        WndClassInspect( WND_MMX );
        break;
    case MENU_MAIN_OPEN_XMM:
        WndClassInspect( WND_XMM );
        break;
    case MENU_MAIN_OPEN_SOURCE:
        WndClassInspect( WND_SOURCE );
//      WndSrcInspect( NilAddr );
        break;
    case MENU_MAIN_OPEN_WATCH:
        WndClassInspect( WND_WATCH );
        break;
    case MENU_MAIN_OPEN_MACROS:
        WndClassInspect( WND_MACRO );
        break;
    case MENU_MAIN_OPEN_IO:
        WndClassInspect( WND_IO );
        break;
    case MENU_MAIN_OPEN_STACK:
        WndClassInspect( WND_STACK );
        break;
    case MENU_MAIN_OPEN_MEMORY_AT:
        ExamMemAt();
        break;
    case MENU_MAIN_HELP_CONTENTS:
        DoProcHelp( GUI_HELP_CONTENTS );
        break;
#if defined(__GUI__)
    case MENU_MAIN_HELP_ON_HELP:
        DoProcHelp( GUI_HELP_ON_HELP );
        break;
    case MENU_MAIN_HELP_SEARCH:
        DoProcHelp( GUI_HELP_SEARCH );
        break;
#endif
    default:
        return( FALSE );
    }
    return( TRUE );
}
