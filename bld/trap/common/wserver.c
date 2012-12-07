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
* Description:  Remote debug server mainline for Windows GUI versions.
*
****************************************************************************/


#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <process.h>
#include "wserver.h"

#include "banner.h"
#include "trpimp.h"
#include "trperr.h"
#include "packet.h"
#include "tcerr.h"

extern trap_version     TrapVersion;

char  TrapFile[ 0x400 ];
char  TrapParm[ 0x400 ];
extern char  RWBuff[ 0x400 ];
extern  char ServName[];

extern char             *LoadTrap( char *,char *,trap_version *);
extern void             KillTrap(void);
extern void             NothingToDo(void);
extern bool             Session( void );
extern bool             ParseCommandLine( char *cmdline, char *trap, char *parm, bool *oneshot );


HANDLE          Instance;
static char     ServerClass[32]="ServerClass";
static HWND     hwndMain;
static bool     SessionError;
static bool     Connected;
static bool     Linked;
static bool     OneShot;

static BOOL FirstInstance( HINSTANCE );
static BOOL AnyInstance( HINSTANCE, int, LPSTR );
#ifdef __NT__
extern void TellHWND( HWND );
#endif

#define MENU_ON (MF_ENABLED+MF_BYCOMMAND)
#define MENU_OFF (MF_DISABLED+MF_GRAYED+MF_BYCOMMAND)

extern BOOL _EXPORT FAR PASCAL OptionsDlgProc( HWND hwnd, unsigned msg,
                                UINT wparam, LONG lparam );

long _EXPORT FAR PASCAL WindowProc( HWND, unsigned, UINT, LONG );

/* Forward declarations */
void StartupErr(char *err);
void ServError( char *msg );
void Output( char *str );

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline,
                      int cmdshow )
{
    MSG         msg;

    Instance = this_inst;
    if( !prev_inst ) {
        if( !FirstInstance( this_inst ) ) return( FALSE );
    }
    if( !AnyInstance( this_inst, cmdshow, cmdline ) ) return( FALSE );

    while( GetMessage( (LPVOID)&msg, (HWND)0, 0, 0 ) ) {

        TranslateMessage( &msg );
        DispatchMessage( &msg );

    }
    if( Linked ) RemoteUnLink();
    KillTrap();

    return( msg.wParam );

} /* WinMain */

/*
 * FirstInstance - register window class for the application,
 *                 and do any other application initialization
 */
static BOOL FirstInstance( HINSTANCE this_inst )
{
    WNDCLASS    wc;
    BOOL        rc;

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = this_inst;
    wc.hIcon = LoadIcon( this_inst, "ServerIcon" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "ServerMenu";
    wc.lpszClassName = ServerClass;
    rc = RegisterClass( &wc );
    return( rc );

} /* FirstInstance */

static void EnableMenus( HWND hwnd, BOOL connected, BOOL session )
/****************************************************************/
{
    HMENU hMenu;

    hMenu = GetMenu( hwnd );
    if( hMenu == NULL ) return;
    EnableMenuItem( hMenu, MENU_EXIT, !session ? MENU_ON : MENU_OFF );
    EnableMenuItem( hMenu, MENU_CONNECT, !connected ? MENU_ON : MENU_OFF );
    EnableMenuItem( hMenu, MENU_OPTIONS, !connected ? MENU_ON : MENU_OFF );
    EnableMenuItem( hMenu, MENU_DISCONNECT, ( connected && !session ) ? MENU_ON : MENU_OFF );
    EnableMenuItem( hMenu, MENU_BREAK, session ? MENU_ON : MENU_OFF );
    DrawMenuBar( hwnd );
}
/*
 * AnyInstance - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL AnyInstance( HINSTANCE this_inst, int cmdshow, LPSTR cmdline )
{
    char        *err;

    if( !ParseCommandLine( cmdline, TrapFile, TrapParm, &OneShot ) ) {
        return( FALSE );
    }
    err = LoadTrap( TrapFile[0] == '\0' ? NULL : TrapFile, RWBuff, &TrapVersion );
    if( err != NULL ) {
        StartupErr( err );
        return( FALSE );
    }
    /*
     * create main window
     */
    hwndMain = CreateWindow(
        ServerClass,            /* class */
        ServName,
        WS_OVERLAPPEDWINDOW,    /* style */
        CW_USEDEFAULT,          /* init. x pos */
        CW_USEDEFAULT,          /* init. y pos */
        CW_USEDEFAULT,          /* init. x size */
        CW_USEDEFAULT,          /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        this_inst,              /* program handle */
        NULL                    /* create parms */
        );

    if( !hwndMain ) return( FALSE );

    #ifdef __NT__
        TellHWND( hwndMain );
    #endif

    /*
     * display window
     */
    ShowWindow( hwndMain, cmdshow );
    UpdateWindow( hwndMain );
    SendMessage( hwndMain, WM_COMMAND, MENU_CONNECT, 0 );

    return( TRUE );

} /* AnyInstance */

/*
 * AboutDlgProc - processes messages for the about dialog.
 */
BOOL _EXPORT FAR PASCAL AboutDlgProc( HWND hwnd, unsigned msg,
                                UINT wparam, LONG lparam )
{
    lparam = lparam;                    /* turn off warning */

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            EndDialog( hwnd, TRUE );
            return( TRUE );
        }
        break;
    }
    return( FALSE );

} /* AboutDlgProc */

static bool Disconnect = FALSE;
static bool Exit = FALSE;
/*
 * WindowProc - handle messages for the main application window
 */
LRESULT _EXPORT FAR PASCAL WindowProc( HWND hwnd, UINT msg,
                                     WPARAM wparam, LPARAM lparam )
{
    FARPROC     proc;
    char        *err;
    HMENU       hMenu;

    switch( msg ) {
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case MENU_ABOUT:
            proc = MakeProcInstance( (FARPROC)AboutDlgProc, Instance );
            DialogBox( Instance,"AboutBox", hwnd, (DLGPROC)proc );
            FreeProcInstance( proc );
            break;

        case MENU_DISCONNECT:
            Disconnect = TRUE;
            break;
        case MENU_CONNECT:
            Disconnect = FALSE;
            SessionError = FALSE;
            err = NULL;
            if( !Linked ) {
                HCURSOR cursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
                err = RemoteLink( TrapParm, 1 );
                SetCursor( cursor );
            }
            EnableMenus( hwnd, TRUE, FALSE );
            if( err != NULL ) {
                ServError( err );
            } else {
                Linked = TRUE;
                while( !Disconnect ) {
                    MSG         peek;
                    if( PeekMessage( &peek, (HWND)0, 0, -1, PM_NOREMOVE ) ) {
                        if( !GetMessage( (LPVOID)&peek, (HWND)0, 0, 0 ) ) {
                            Disconnect = TRUE;
                            Exit = TRUE;
                            break;
                        }
                        TranslateMessage( &peek );
                        DispatchMessage( &peek );
                    }
                    if( RemoteConnect() ) {
                        Connected = TRUE;
                        ShowWindow( hwnd, SW_MINIMIZE );
                        UpdateWindow( hwnd );
                        hMenu = GetMenu( hwnd );
                        EnableMenus( hwnd, TRUE, TRUE );
                        Session();
                        EnableMenus( hwnd, TRUE, FALSE );
                        ShowWindow( hwnd, SW_RESTORE );
                        RemoteDisco();
                        Connected = FALSE;
                        if( OneShot ) PostQuitMessage( 0 );
                        break;
                    }
                    NothingToDo();
                }
            }
            EnableMenus( hwnd, FALSE, FALSE );
            if( !Disconnect && !SessionError ) {
                SendMessage( hwndMain, WM_COMMAND, MENU_CONNECT, 0 );
            }
            if( Exit ) PostQuitMessage( 0 );

            break;
        case MENU_OPTIONS:
            proc = MakeProcInstance( (FARPROC)OptionsDlgProc, Instance );
            if( Linked ) RemoteUnLink();
            Linked = FALSE;
            DialogBox( Instance, "Options", hwnd, (DLGPROC)proc );
            FreeProcInstance( proc );
            break;
        case MENU_EXIT:
            Disconnect = TRUE;
            Exit = TRUE;
            PostQuitMessage( 0 );
            break;
        case MENU_BREAK:
#ifdef __386__
            {
                OSVERSIONINFO   osver;
                osver.dwOSVersionInfoSize = sizeof( osver );
                GetVersionEx( &osver );
                if( osver.dwPlatformId == VER_PLATFORM_WIN32s ) {
                    Output( "You must press CTRL-ALT-F11 to interrupt a program under Win32s" );
                }
            }
#else
            Output( "Press CTRL-ALT-F to interrupt the program" );
#endif
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_CLOSE:
        if( Connected ) return( 0 );
        /* fall through to default */

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0L );

} /* WindowProc */


void ServError( char *msg )
{
    ShowWindow( hwndMain, SW_RESTORE );
    MessageBox( NULL, msg, TRP_The_WATCOM_Debugger, MB_APPLMODAL+MB_OK );
    SessionError = TRUE;
}

void StartupErr(char *err)
{
    ShowWindow( hwndMain, SW_RESTORE );
    ServError( err );
}

void ServMessage(char *msg)
{
    msg = msg;
}

int WantUsage( char *ptr )
{
    return( *ptr == '?' );
}

void Output( char *str )
{
    MessageBox( NULL, str, TRP_The_WATCOM_Debugger, MB_APPLMODAL+MB_OK );
}
