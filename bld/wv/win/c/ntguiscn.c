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


#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <windows.h>
#include "dbgdefn.h"
#include "dbgwind.h"
#include "dbgtoggl.h"

extern void     *ExtraAlloc( unsigned );
extern void     ExtraFree( void * );
extern void     SaveMainScreen(char*);
extern void     RestoreMainScreen(char*);
extern HWND     GUIGetSysHandle( gui_window *wnd );
extern void     DebugExit();
extern void     TellHWND( HWND );

volatile int    BrkPending;
extern a_window *WndMain;
static HWND             HwndFore = NULL;
static HWND             FirstForeWnd = NULL;


void TellWinHandle()
{
    if( _IsOn( SW_POWERBUILDER ) ) return;
    TellHWND( GUIGetSysHandle( WndGui( WndMain ) ) );
}

void SetNumLines( int num )
{
    num = num;
}

void SetNumColumns( int num )
{
    num = num;
}

void RingBell()
{
    MessageBeep( 0 );
}

unsigned ConfigScreen( void )
{
    return( 0 );
}

unsigned GetSystemDir( char *buff, unsigned max )
{
    buff[ 0 ] = '\0';
    GetWindowsDirectory( buff, max );
    return( strlen( buff ) );
}

void InitScreen( void )
{
    RestoreMainScreen( "WDNT" );
    FirstForeWnd = GetForegroundWindow();
}

bool UsrScrnMode( void )
{
    return( FALSE );
}

void DbgScrnMode( void )
{
}

static enum {
    DEBUG_SCREEN,
    USER_SCREEN,
    UNKNOWN_SCREEN
} ScreenState = UNKNOWN_SCREEN;

void UnknownScreen()
{
    ScreenState = UNKNOWN_SCREEN;
}

bool DebugScreen( void )
{
    HWND        hwnd;
    HWND        fore;

    if( ScreenState == DEBUG_SCREEN ) return( FALSE );
    if( _IsOn( SW_POWERBUILDER ) ) return( FALSE );
    if( WndMain ) {
        ScreenState = DEBUG_SCREEN;
        hwnd = GUIGetSysHandle( WndGui( WndMain ) );
        fore = GetForegroundWindow();
        if( fore != hwnd ) {
            HwndFore = fore;
        }
        if( GUIIsMinimized( WndGui( WndMain ) ) ) GUIRestoreWindow( WndGui( WndMain ) );
        if( IsWindow( hwnd ) ) SetForegroundWindow( hwnd );
        if( _IsOn( SW_POWERBUILDER ) ) {
            WndRestoreWindow( WndMain );
        }
    }
    return( FALSE );
}

bool DebugScreenRecover()
{
    return( TRUE );
}


bool UserScreen()
{
    if( ScreenState == USER_SCREEN ) return( FALSE );
    if( _IsOn( SW_POWERBUILDER ) ) return( FALSE );
    if( WndMain ) {
        ScreenState = USER_SCREEN;
        if( _IsOn( SW_POWERBUILDER ) ) {
            WndMinimizeWindow( WndMain );
        } else {
            if( IsWindow( HwndFore ) ) {
                SetForegroundWindow( HwndFore );
            } else {
                HwndFore = NULL;
            }
        }
    }
    return( FALSE );
}

void SaveMainWindowPos()
{
    SaveMainScreen( "WDNT" );
}

void FiniScreen( void )
{
    if( _IsOn( SW_POWERBUILDER ) ) return;
    if( IsWindow( FirstForeWnd ) ) {
        SetForegroundWindow( FirstForeWnd );
    }
}

bool SysGUI()
{
    return( TRUE );
}
