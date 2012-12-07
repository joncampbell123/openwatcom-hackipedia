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


#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "wglbl.h"
#include "wrglbl.h"
#include "wmem.h"
#include "wmsg.h"
#include "winst.h"
#include "wsetedit.h"
#include "wrename.h"
#include "wctl3d.h"
#include "w_rc.h"
#include "wresall.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WResRenameInfo {
    HELP_CALLBACK       *hcb;
    WResID              *old_name;
    WResID              *new_name;
} WResRenameInfo;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL WINEXPORT WResRenameProc ( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void         WSetWinInfo   ( HWND, WResRenameInfo * );
static void         WGetWinInfo   ( HWND, WResRenameInfo * );
static Bool         WGetNewName   ( HWND, WResRenameInfo * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

Bool WRenameResource( HWND parent, WResID **name, HELP_CALLBACK *hcb )
{
    WResRenameInfo   info;
    Bool               ok;

    info.old_name = NULL;
    info.new_name = NULL;

    ok = ( name != NULL );

    if ( ok )  {
        ok = FALSE;
        info.hcb = hcb;
        info.old_name = *name;
        if ( WGetNewName ( parent, &info ) && info.new_name ) {
            if ( *name ) {
                WMemFree ( *name );
            }
            *name = info.new_name;
            ok = TRUE;
        }
    }

    return ( ok );
}

Bool WGetNewName ( HWND parent, WResRenameInfo *info )
{
    DLGPROC     proc_inst;
    HINSTANCE   app_inst;
    Bool        modified;

    app_inst      = WGetEditInstance();

    proc_inst = (DLGPROC)
        MakeProcInstance ( (FARPROC) WResRenameProc, app_inst );

    modified = JDialogBoxParam( app_inst, "WRenameResource", parent,
                                proc_inst, (LPARAM) info );

    FreeProcInstance ( (FARPROC) proc_inst );

    return  ( ( modified != -1 ) && ( modified == IDOK ) );
}

void WSetWinInfo ( HWND hDlg, WResRenameInfo *info )
{
    if ( info && info->old_name ) {
        WSetEditWithWResID ( GetDlgItem ( hDlg, IDM_RENOLD), info->old_name );
        WSetEditWithWResID ( GetDlgItem ( hDlg, IDM_RENNEW), info->old_name );
    }
}

void WGetWinInfo ( HWND hDlg, WResRenameInfo *info )
{
    Bool mod;

    if ( info ) {
        info->new_name =
            WGetWResIDFromEdit ( GetDlgItem ( hDlg,IDM_RENNEW ), &mod );
    }
}

BOOL WINEXPORT WResRenameProc ( HWND hDlg, UINT message,
                                WPARAM wParam, LPARAM lParam )
{
    WResRenameInfo *info;
    BOOL   ret;

    ret = FALSE;

    switch( message ) {

        case WM_SYSCOLORCHANGE:
            WCtl3dColorChange();
            break;

        case WM_INITDIALOG:
            info = (WResRenameInfo *) lParam;
            SetWindowLong( hDlg, DWL_USER, (LONG) info );
            WSetWinInfo( hDlg, info );
            ret = TRUE;
            break;

        case WM_COMMAND:
            info = (WResRenameInfo *) GetWindowLong( hDlg, DWL_USER );
            switch( LOWORD(wParam) ) {
                case IDM_HELP:
                    if( info && info->hcb ) {
                        (*info->hcb)();
                    }
                    break;

                case IDOK:
                    WGetWinInfo( hDlg, info );
                    EndDialog( hDlg, TRUE );
                    ret  = TRUE;
                    break;

                case IDCANCEL:
                    EndDialog( hDlg, FALSE );
                    ret  = TRUE;
                    break;
            }
    }

    return( ret );
}

