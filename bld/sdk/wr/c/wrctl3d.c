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
* Description:  Cover functions to avoid dependency on CTL3D DLL.
*
****************************************************************************/


#include <windows.h>
#include "wrglbl.h"
#include "wrctl3d.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
// #define WR_USE_3D

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef BOOL    ( WINAPI *LPFN_Ctl3dSubclassDlg ) (HWND, WORD);
typedef BOOL    ( WINAPI *LPFN_Ctl3dSubclassDlgEx ) (HWND, DWORD);
typedef WORD    ( WINAPI *LPFN_Ctl3dGetVer ) (void);
typedef BOOL    ( WINAPI *LPFN_Ctl3dEnabled ) (void);
typedef HBRUSH  ( WINAPI *LPFN_Ctl3dCtlColor ) (HDC, LONG);
typedef HBRUSH  ( WINAPI *LPFN_Ctl3dCtlColorEx ) (UINT wm, WPARAM wParam, LPARAM lParam);
typedef BOOL    ( WINAPI *LPFN_Ctl3dColorChange ) (void);
typedef BOOL    ( WINAPI *LPFN_Ctl3dSubclassCtl ) (HWND);
typedef LONG    ( WINAPI *LPFN_Ctl3dDlgFramePaint ) (HWND, UINT, WPARAM, LPARAM);
typedef BOOL    ( WINAPI *LPFN_Ctl3dAutoSubclass ) (HANDLE);
typedef BOOL    ( WINAPI *LPFN_Ctl3dRegister ) (HANDLE);
typedef BOOL    ( WINAPI *LPFN_Ctl3dUnregister ) (HANDLE);
typedef VOID    ( WINAPI *LPFN_Ctl3dWinIniChange ) (void);

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LPFN_Ctl3dSubclassDlg    wrCtl3dSubclassDlg = NULL;
static LPFN_Ctl3dSubclassDlgEx  wrCtl3dSubclassDlgEx = NULL;
static LPFN_Ctl3dGetVer         wrCtl3dGetVer = NULL;
static LPFN_Ctl3dEnabled        wrCtl3dEnabled = NULL;
static LPFN_Ctl3dCtlColor       wrCtl3dCtlColor = NULL;
static LPFN_Ctl3dCtlColorEx     wrCtl3dCtlColorEx = NULL;
static LPFN_Ctl3dColorChange    wrCtl3dColorChange = NULL;
static LPFN_Ctl3dSubclassCtl    wrCtl3dSubclassCtl = NULL;
static LPFN_Ctl3dDlgFramePaint  wrCtl3dDlgFramePaint = NULL;
static LPFN_Ctl3dAutoSubclass   wrCtl3dAutoSubclass = NULL;
static LPFN_Ctl3dRegister       wrCtl3dRegister = NULL;
static LPFN_Ctl3dUnregister     wrCtl3dUnregister = NULL;
static LPFN_Ctl3dWinIniChange   wrCtl3dWinIniChange = NULL;

static HINSTANCE                wrDLLLib = NULL;

void WRCtl3DDLLFini( void )
{
    wrCtl3dSubclassDlg = NULL;
    wrCtl3dSubclassDlgEx = NULL;
    wrCtl3dGetVer = NULL;
    wrCtl3dEnabled = NULL;
    wrCtl3dCtlColor = NULL;
    wrCtl3dCtlColorEx = NULL;
    wrCtl3dColorChange = NULL;
    wrCtl3dSubclassCtl = NULL;
    wrCtl3dDlgFramePaint = NULL;
    wrCtl3dAutoSubclass = NULL;
    wrCtl3dRegister = NULL;
    wrCtl3dUnregister = NULL;
    wrCtl3dWinIniChange = NULL;
    if( wrDLLLib != (HINSTANCE)NULL ) {
        FreeLibrary( wrDLLLib );
    }
}

static int WRCtl3DDLLInit( void )
{
#ifdef __NT__
    wrDLLLib = LoadLibrary( "CTL3D32.DLL" );
#else
    UINT    uErrMode;

    /* Use SetErrorMode to prevent annoying error popups. */
    uErrMode = SetErrorMode( SEM_NOOPENFILEERRORBOX );
    wrDLLLib = LoadLibrary( "CTL3DV2.DLL" );
    SetErrorMode( uErrMode );
#endif

    if( wrDLLLib == (HINSTANCE)NULL ) {
        return( FALSE );
    }

    wrCtl3dSubclassDlg          = (void *)GetProcAddress( wrDLLLib, (LPCSTR)2 );
    wrCtl3dSubclassDlgEx        = (void *)GetProcAddress( wrDLLLib, (LPCSTR)21 );
    wrCtl3dGetVer               = (void *)GetProcAddress( wrDLLLib, (LPCSTR)1 );
    wrCtl3dEnabled              = (void *)GetProcAddress( wrDLLLib, (LPCSTR)5 );
    wrCtl3dCtlColor             = (void *)GetProcAddress( wrDLLLib, (LPCSTR)4 );
    wrCtl3dCtlColorEx           = (void *)GetProcAddress( wrDLLLib, (LPCSTR)18 );
    wrCtl3dColorChange          = (void *)GetProcAddress( wrDLLLib, (LPCSTR)6 );
    wrCtl3dSubclassCtl          = (void *)GetProcAddress( wrDLLLib, (LPCSTR)3 );
    wrCtl3dDlgFramePaint        = (void *)GetProcAddress( wrDLLLib, (LPCSTR)20 );
    wrCtl3dAutoSubclass         = (void *)GetProcAddress( wrDLLLib, (LPCSTR)16 );
    wrCtl3dRegister             = (void *)GetProcAddress( wrDLLLib, (LPCSTR)12 );
    wrCtl3dUnregister           = (void *)GetProcAddress( wrDLLLib, (LPCSTR)13 );
    wrCtl3dWinIniChange         = (void *)GetProcAddress( wrDLLLib, (LPCSTR)22 );

    if( ( wrCtl3dSubclassDlg == NULL ) ||
        ( wrCtl3dSubclassDlgEx == NULL ) ||
        ( wrCtl3dGetVer == NULL ) ||
        ( wrCtl3dEnabled == NULL ) ||
        ( wrCtl3dCtlColor == NULL ) ||
        ( wrCtl3dCtlColorEx == NULL ) ||
        ( wrCtl3dColorChange == NULL ) ||
        ( wrCtl3dSubclassCtl == NULL ) ||
        ( wrCtl3dDlgFramePaint == NULL ) ||
        ( wrCtl3dAutoSubclass == NULL ) ||
        ( wrCtl3dRegister == NULL ) ||
        ( wrCtl3dUnregister == NULL ) ||
        ( wrCtl3dWinIniChange == NULL ) ) {
        WRCtl3DDLLFini();
        return( FALSE );
    }

    return( TRUE );
}

int WR_EXPORT WRCtl3DInit( HINSTANCE inst )
{
    DWORD       ver;
    BYTE        vm;

    _wtouch(inst);

    ver = GetVersion();
    vm = (BYTE)( ver & 0x000000FF );
    if( vm >= 0x04 ) {
        return( TRUE );
    }

    WRCtl3DDLLInit();

#if defined(WR_USE_3D)
    if( !Ctl3dRegister( inst ) ) {
        return( FALSE );
    }

    if( !Ctl3dAutoSubclass( inst ) ) {
        return( FALSE );
    }
#endif

    return( TRUE );
}

void WR_EXPORT WRCtl3DFini( HINSTANCE inst )
{
    _wtouch(inst);
#if defined(WR_USE_3D)
    Ctl3dUnregister( inst );
#endif
    WRCtl3DDLLFini();
}

BOOL WR_EXPORT WRCtl3dSubclassDlg( HWND hwnd, WORD w )
{
    if( wrCtl3dSubclassDlg ) {
        return( wrCtl3dSubclassDlg( hwnd, w ) );
    }
    return( FALSE );
}

BOOL WR_EXPORT WRCtl3dSubclassDlgEx( HWND hwnd, DWORD dw )
{
    if( wrCtl3dSubclassDlgEx ) {
        return( wrCtl3dSubclassDlgEx( hwnd, dw ) );
    }
    return( FALSE );
}

WORD WR_EXPORT WRCtl3dGetVer( void )
{
    if( wrCtl3dGetVer ) {
        return( wrCtl3dGetVer() );
    }
    return( 0 );
}

BOOL WR_EXPORT WRCtl3dEnabled( void )
{
    if( wrCtl3dEnabled ) {
        return( wrCtl3dEnabled() );
    }
    return( FALSE );
}

HBRUSH WR_EXPORT WRCtl3dCtlColor( HDC dc, LONG l )
{
    if( wrCtl3dCtlColor ) {
        return( wrCtl3dCtlColor( dc, l ) );
    }
    return( (HBRUSH)NULL );
}

HBRUSH WR_EXPORT WRCtl3dCtlColorEx(UINT wm, WPARAM wParam, LPARAM lParam)
{
    if( wrCtl3dCtlColorEx ) {
        return( wrCtl3dCtlColorEx( wm, wParam, lParam ) );
    }
    return( (HBRUSH)NULL );
}

BOOL WR_EXPORT WRCtl3dColorChange( void )
{
    if( wrCtl3dColorChange ) {
        return( wrCtl3dColorChange() );
    }
    return( FALSE );
}

BOOL WR_EXPORT WRCtl3dSubclassCtl( HWND hwnd )
{
    if( wrCtl3dSubclassCtl ) {
        return( wrCtl3dSubclassCtl( hwnd ) );
    }
    return( FALSE );
}

LONG WR_EXPORT WRCtl3dDlgFramePaint( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
    if( wrCtl3dDlgFramePaint ) {
        return( wrCtl3dDlgFramePaint( hwnd, msg, wp, lp ) );
    }
    return( 0L );
}

BOOL WR_EXPORT WRCtl3dAutoSubclass(HANDLE hndl )
{
    if( wrCtl3dAutoSubclass ) {
        return( wrCtl3dAutoSubclass( hndl ) );
    }
    return( FALSE );
}

BOOL WR_EXPORT WRCtl3dRegister( HANDLE hndl )
{
    if( wrCtl3dRegister ) {
        return( wrCtl3dRegister( hndl ) );
    }
    return( FALSE );
}

BOOL WR_EXPORT WRCtl3dUnregister( HANDLE inst )
{
    if( wrCtl3dUnregister ) {
        return( wrCtl3dUnregister( inst ) );
    }
    return( FALSE );
}

void WR_EXPORT WRCtl3dWinIniChange( void )
{
    if( wrCtl3dWinIniChange ) {
        wrCtl3dWinIniChange();
    }
}

