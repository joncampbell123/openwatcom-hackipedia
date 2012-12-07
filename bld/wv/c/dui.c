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


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbginp.h"
#include "dbgtoken.h"
#include "dbgerr.h"
#include "dbgwind.h"
#include "dbgadget.h"
#include "dbghook.h"
#include "dbgio.h"
#include "dbgbreak.h"
#include "dui.h"

extern void             *CmdHistory;
extern void             *SrchHistory;
extern a_window         *WndMain;
extern char             WndNameTab[];
extern char             *TxtBuff;

extern unsigned long    CueLine( cue_handle *ch );
extern GUICALLBACK      WndMainEventProc;
extern void             TellWinHandle( void );
extern void             DlgNewProg( void );
extern void             InitToolBar( void );
extern void             InitMemWindow( void );
extern void             InitAboutMessage( void );
extern void             InitIOWindow( void );
extern void             InitMenus( void );
extern void             InitHelp( void );
extern void             InitGadget( void );
extern void             InitPaint( void );
extern void             InitBrowse( void );
extern void             InitFont( void );
extern void             InitFileMap( void );
extern void             FiniMacros( void );
extern void             FiniBrowse( void );
extern void             FiniPaint( void );
extern void             FiniGadget( void );
extern void             FiniFont( void );
extern void             FiniAboutMessage( void );
extern void             FiniMenus( void );
extern void             FiniIOWindow( void );
extern void             FiniMemWindow( void );
extern void             FiniToolBar( void );
extern void             FiniFileMap( void );
extern void             WndDlgFini( void );
extern void             DoProcPending( void );
extern void             SetTargMenuItems( void );
extern void             SetBrkMenuItems( void );
extern void             SetIOMenuItems( void );
extern void             DoInput( void );
extern void             *OpenSrcFile(cue_handle *);
extern int              FReadLine( void *, int, int, char *, int );
extern void             FDoneSource( void * );
extern bool             GUIIsDBCS( void );
extern int              EnvLkup( char *src, char *dst, int );
extern void             PopErrBox( char *buff );
extern void             KillDebugger( int ret_code );
extern char             *Format( char *, char *, ... );

extern void             InitSuppServices( void );


void DUIUpdate( update_list flags )
{
    if( flags & UP_NEW_PROGRAM ) {
        SetTargMenuItems();
        SetIOMenuItems();
        SetBrkMenuItems();
    } else if( flags & UP_BREAK_CHANGE ) {
        SetBrkMenuItems();
    }
}

void DUIStatusText( char *text )
{
    WndStatusText( text );
}

extern void WndMsgBox( char *text );
void DUIMsgBox( char *text )
{
    WndMsgBox( text );
}

extern bool WndDlgTxt( char *text );
bool DUIDlgTxt( char *text )
{
    return( WndDlgTxt( text ) );
}

extern void WndInfoBox( char *text );
void DUIInfoBox( char *text )
{
    WndInfoBox( text );
}

void DUIStop( void )
{
    WndStop();
}

bool DUIClose( void )
{
    return( WndFini() );
}

void DUIInit( void )
{
    CmdHistory = WndInitHistory();
    SrchHistory = WndInitHistory();
    InitToolBar();
    InitMemWindow();
    InitAboutMessage();
    InitIOWindow();
    InitMenus();
    WndInit( LIT( The_WATCOM_Debugger ) );
    _SwitchOff( SW_ERROR_STARTUP );
#if defined(__GUI__)
    TellWinHandle();
#endif
    if( WndMain != NULL ) WndSetIcon( WndMain, &MainIcon );
    InitHelp();
    InitGadget();
    InitPaint();
    InitBrowse();
    InitFont();
}

void DUIFini( void )
{
    WndFiniHistory( SrchHistory );
    WndFiniHistory( CmdHistory );
    FiniMacros();
    FiniBrowse();
    FiniPaint();
    FiniGadget();
    FiniFont();
    FiniAboutMessage();
    FiniMenus();
    FiniIOWindow();
    FiniMemWindow();
    FiniToolBar();
    WndDlgFini();
}

extern void DUIFreshAll( void )
{
    WndFreshAll();
}

extern bool DUIStopRefresh( bool ok )
{
    return( WndStopRefresh( ok ) );
}

extern void DUIShow( void )
{
    WndDebug();
    WndShowAll();
    WndShowWndMain();
    WndMainEventProc( WndGui( WndMain ), GUI_NO_EVENT, NULL );
    if( _IsOff( SW_HAVE_TASK ) && _IsOff( SW_PROC_ALREADY_STARTED ) ) {
        DlgNewProg();
    }
}

extern void WndUser( void );
extern void DUIWndUser( void )
{
    WndUser();
}

extern void WndDebug( void );
extern void DUIWndDebug( void )
{
    WndDebug();
}

extern a_window         *WndClassInspect( wnd_class class );
extern void DUIShowLogWindow( void )
{
    WndClassInspect( WND_DIALOGUE );
}

unsigned ScanCmd( char *cmd_table );
wnd_class ReqWndName( void )
{
    wnd_class   class;

    class = ScanCmd( &WndNameTab );
    if( class == 0 ) Error( ERR_LOC, LIT( ERR_BAD_WIND_NAME ) );
    return( class-1 );
}

extern int DUIGetMonitorType( void )
{
    if( GUIIsGUI() ) {
        return( 1 );
    } else {
        gui_system_metrics  metrics;
        GUIGetSystemMetrics( &metrics );
        return( metrics.colour != 0 );
    }
}

extern int DUIScreenSizeY( void )
{
    return( WndScreen.y );
}

extern int DUIScreenSizeX( void )
{
    return( WndScreen.x );
}

void DUIErrorBox( char *buff )
{
    WndDisplayMessage( buff, LIT( Error ), GUI_INFORMATION + GUI_SYSTEMMODAL );
}

extern void DUIArrowCursor( void )
{
    WndArrowCursor();
}

extern char *WndLoadString( int i );
extern char *DUILoadString( int i )
{
    return( WndLoadString( i ) );
}

bool DUIAskIfAsynchOk( void )
{
    return( WndDisplayMessage( LIT( WARN_Asynch_Event ), LIT( Empty ),
                             GUI_YES_NO ) == GUI_RET_YES );
}

extern void WndFlushKeys( void );
extern void DUIFlushKeys( void )
{
    WndFlushKeys();
}

extern void PlayDead( bool );
extern void DUIPlayDead( bool dead )
{
    PlayDead( dead );
}

extern void WndSysEnd( bool pause );
extern void DUISysEnd( bool pause )
{
    WndSysEnd( pause );
}

extern void WndSysStart( void );
extern void DUISysStart( void )
{
    WndSysStart();
}

extern void RingBell( void );
extern void DUIRingBell( void )
{
    RingBell();
}

extern void ProcPendingPaint( void );
extern void DUIProcPendingPaint( void )
{
    ProcPendingPaint();
}

extern bool DlgInfoRelease( void );
extern bool VarInfoRelease( void );
extern bool DUIInfoRelease( void )
{
    if( DlgInfoRelease() ) return( TRUE );
    if( VarInfoRelease() ) return( TRUE );
    return( FALSE );
}

extern void *DUIHourGlass( void *x )
{
    return( WndHourGlass( x ) );
}

extern void WndDoInput( void )
{
    InitSuppServices();
    DoInput();
}

void DUIEnterCriticalSection( void )
{
}

void DUIExitCriticalSection( void )
{
}

bool DUIGetSourceLine( cue_handle *ch, char *buff, unsigned len )
{
    void        *viewhndl;

    viewhndl = OpenSrcFile( ch );
    if( viewhndl == NULL ) return( FALSE );
    buff[ FReadLine( viewhndl, CueLine( ch ), 0, buff, len )] = '\0';
    FDoneSource( viewhndl );
    return( TRUE );
}

bool DUIIsDBCS( void )
{
    return( GUIIsDBCS() );
}

int DUIEnvLkup( char *src, char *dst, int max_len )
{
    return( EnvLkup( src, dst, max_len ) );
}

void DUIDirty( void )
{
    WndDirty( NULL );
}

extern void WndSrcOrAsmInspect( address );
void DUISrcOrAsmInspect( address addr )
{
    WndSrcOrAsmInspect( addr );
}

extern void WndAddrInspect( address );
void DUIAddrInspect( address addr )
{
    WndAddrInspect( addr );
}

extern void RemovePoint( brkp *bp );
extern void DUIRemoveBreak( brkp *bp )
/***********************************/
{
    RemovePoint( bp );
}


void StartupErr( char *err )
/**************************/
{
    PopErrBox( err );
    KillDebugger(1);
}

void DUICopySize( void *cookie, long size )
/*****************************************/
{
    size = size;
    cookie = cookie;
}

void DUICopyCopied( void *cookie, long size )
/*******************************************/
{
    size = size;
    cookie = cookie;
}

bool DUICopyCancelled( void * cookie )
/************************************/
{
    cookie = cookie;
    return( FALSE );
}

bool DUISymbolsCancelled( void )
/******************************/
{
    return( FALSE );
}

bool DUIImageLoaded( image_entry *image, bool load,
                     bool already_stopping, bool *force_stop )
/************************************************************/
{
    already_stopping=already_stopping;
    force_stop= force_stop;
    if( load ) {
        Format( TxtBuff, "%s '%s'", LIT( DLL_Loaded ), image->image_name );
    } else {
        Format( TxtBuff, "%s '%s'", LIT( DLL_UnLoaded ), image->image_name );
    }
    DUIDlgTxt( TxtBuff );
    return( FALSE );
}
