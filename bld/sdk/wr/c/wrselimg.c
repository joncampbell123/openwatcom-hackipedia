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
#include <stdlib.h>
#include <string.h>
#include "win1632.h"
#include "wrglbl.h"
#include "wrinfo.h"
#include "wrmsg.h"
#include "wrcmsg.gh"
#include "wrctl3d.h"
#include "wrmaini.h"
#include "wrdmsgi.h"
#include "wrmem.h"
#include "wrfindt.h"
#include "wrrnames.h"
#include "wrselimg.h"
#include "selimage.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL WR_EXPORT WRSelectImageProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static  void            WRSetEntries    ( HWND, WRSelectImageInfo * );
static  BOOL            WRSetWinInfo    ( HWND, WRSelectImageInfo * );
static  BOOL            WRGetWinInfo    ( HWND, WRSelectImageInfo * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

void WR_EXPORT WRFreeSelectImageInfo( WRSelectImageInfo *info )
{
    if( info != NULL ) {
        WRMemFree( info );
    }
}

WRSelectImageInfo * WR_EXPORT WRSelectImage( HWND parent, WRInfo *rinfo,
                                             FARPROC hcb )
{
    DLGPROC             proc;
    HINSTANCE           inst;
    BOOL                modified;
    WRSelectImageInfo   *info;

    if( rinfo == NULL ) {
        return( NULL );
    }

    info = (WRSelectImageInfo *)WRMemAlloc( sizeof(WRSelectImageInfo) );
    if( info == NULL ) {
        return( NULL );
    }
    memset( info, 0, sizeof(WRSelectImageInfo) );

    info->hcb  = hcb;
    info->info = rinfo;

    inst = WRGetInstance();

    proc = (DLGPROC) MakeProcInstance( (FARPROC)WRSelectImageProc, inst );

    modified = JDialogBoxParam( inst, "WRSelectImage", parent,
                                proc, (LPARAM) info );

    FreeProcInstance( (FARPROC) proc );

    if( ( modified == -1 ) || ( modified == IDCANCEL ) ) {
        WRMemFree( info );
        info = NULL;
    }

    return( info );
}

void WRSetEntries( HWND hdlg, WRSelectImageInfo *info )
{
    HWND                lbox;
    WResTypeNode        *tnode;
    char                *empty_str;

    if( !info || !info->info || ( hdlg == (HWND)NULL ) ) {
        return;
    }

    lbox = GetDlgItem( hdlg, IDM_SELIMGLBOX );
    if( lbox == (HWND)NULL ) {
        return;
    }
    SendMessage( lbox, LB_RESETCONTENT, 0, 0 );

    tnode = WRFindTypeNode( info->info->dir, info->type, NULL );
    if( tnode == NULL ) {
        empty_str = WRAllocRCString( WR_EMPTY );
        if( empty_str ) {
            WRSetLBoxWithStr( lbox, empty_str, NULL );
            WRFreeRCString( empty_str );
        }
        return;
    }

    WRSetResNamesFromTypeNode( lbox, tnode );
    SendMessage( lbox, LB_SETCURSEL, 0, 0 );
}

BOOL WRSetWinInfo( HWND hdlg, WRSelectImageInfo *info )
{
    WResTypeNode        *tnode;
    BOOL                lbox_set;

    if( !info || ( hdlg == (HWND)NULL ) ) {
        return( FALSE );
    }

    lbox_set = FALSE;

    tnode = WRFindTypeNode( info->info->dir, (uint_16)RT_BITMAP, NULL );
    if( tnode != NULL ) {
        CheckDlgButton( hdlg, IDM_SELIMGBMP, 1);
        info->type = (uint_16)RT_BITMAP;
        WRSetEntries( hdlg, info );
        lbox_set = TRUE;
    } else {
        EnableWindow( GetDlgItem( hdlg, IDM_SELIMGBMP ), FALSE );
    }

    tnode = WRFindTypeNode( info->info->dir, (uint_16)RT_GROUP_CURSOR, NULL );
    if( tnode != NULL ) {
        if( !lbox_set ) {
            CheckDlgButton( hdlg, IDM_SELIMGCUR, 1);
            info->type = (uint_16)RT_GROUP_CURSOR;
            WRSetEntries( hdlg, info );
            lbox_set = TRUE;
        }
    } else {
        EnableWindow( GetDlgItem( hdlg, IDM_SELIMGCUR ), FALSE );
    }

    tnode = WRFindTypeNode( info->info->dir, (uint_16)RT_GROUP_ICON, NULL );
    if( tnode != NULL ) {
        if( !lbox_set ) {
            CheckDlgButton( hdlg, IDM_SELIMGICO, 1);
            info->type = (uint_16)RT_GROUP_ICON;
            WRSetEntries( hdlg, info );
            lbox_set = TRUE;
        }
    } else {
        EnableWindow( GetDlgItem( hdlg, IDM_SELIMGICO ), FALSE );
    }

    if( !lbox_set ) {
        WRDisplayErrorMsg( WR_PRJNOIMAGES );
    }

    return( lbox_set );
}

BOOL WRGetWinInfo( HWND hdlg, WRSelectImageInfo *info )
{
    HWND        lbox;
    LRESULT     index;

    if( !info || !info->info || ( hdlg == (HWND)NULL ) ) {
        return( FALSE );
    }

    lbox = GetDlgItem( hdlg, IDM_SELIMGLBOX );
    if( lbox == (HWND)NULL ) {
        return( FALSE );
    }

    index = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
    if( index == LB_ERR ) {
        return( FALSE );
    }

    info->lnode = (WResLangNode *)
        SendMessage( lbox, LB_GETITEMDATA, (WPARAM) index, 0 );
    if( info->lnode == NULL ) {
        return( FALSE );
    }

    return( TRUE );
}

BOOL WR_EXPORT WRSelectImageProc( HWND hDlg, UINT message,
                                  WPARAM wParam, LPARAM lParam )
{
    WRSelectImageInfo   *info;
    BOOL                ret;

    ret = FALSE;

    switch( message ) {

        case WM_DESTROY:
            WRUnregisterDialog( hDlg );
            break;

        case WM_INITDIALOG:
            info = (WRSelectImageInfo *) lParam;
            if( info == NULL ) {
                EndDialog( hDlg, FALSE );
            }
            SetWindowLong( hDlg, DWL_USER, (LONG) info );
            WRRegisterDialog( hDlg );
            if( !WRSetWinInfo( hDlg, info ) ) {
                EndDialog( hDlg, FALSE );
            }
            ret = TRUE;
            break;

        case WM_SYSCOLORCHANGE:
            WRCtl3dColorChange();
            break;

        case WM_COMMAND:
            info = (WRSelectImageInfo *) GetWindowLong( hDlg, DWL_USER );
            switch( LOWORD(wParam) ) {
                case IDM_SELIMGHELP:
                    if( info && info->hcb ) {
                        (*info->hcb)();
                    }
                    break;

                case IDOK:
                    if( info == NULL ) {
                        EndDialog( hDlg, FALSE );
                        ret  = TRUE;
                    } else if( WRGetWinInfo( hDlg, info ) ) {
                        EndDialog( hDlg, TRUE );
                        ret  = TRUE;
                    }
                    break;

                case IDCANCEL:
                    EndDialog( hDlg, FALSE );
                    ret  = TRUE;
                    break;

                case IDM_SELIMGBMP:
                    if(GET_WM_COMMAND_CMD(wParam,lParam) != BN_CLICKED) {
                        break;
                    }
                    if( info->type != (uint_16)RT_BITMAP ) {
                        info->type = (uint_16)RT_BITMAP;
                        WRSetEntries( hDlg, info );
                    }
                    break;

                case IDM_SELIMGCUR:
                    if(GET_WM_COMMAND_CMD(wParam,lParam) != BN_CLICKED) {
                        break;
                    }
                    if( info->type != (uint_16)RT_GROUP_CURSOR ) {
                        info->type = (uint_16)RT_GROUP_CURSOR;
                        WRSetEntries( hDlg, info );
                    }
                    break;

                case IDM_SELIMGICO:
                    if(GET_WM_COMMAND_CMD(wParam,lParam) != BN_CLICKED) {
                        break;
                    }
                    if( info->type != (uint_16)RT_GROUP_ICON ) {
                        info->type = (uint_16)RT_GROUP_ICON;
                        WRSetEntries( hDlg, info );
                    }
                    break;
            }
    }

    return( ret );
}


