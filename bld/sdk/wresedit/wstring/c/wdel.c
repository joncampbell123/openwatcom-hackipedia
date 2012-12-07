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
#include <ctype.h>
#include <string.h>
#include "wglbl.h"
#include "wmem.h"
#include "wedit.h"
#include "wdel.h"
#include "sys_rc.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

Bool WDeleteStringEntry ( WStringEditInfo *einfo )
{
    HWND         lbox;
    Bool         ok;
    LRESULT      ret;

    ok = ( einfo && einfo->edit_dlg );

    if ( ok ) {
        lbox = GetDlgItem ( einfo->edit_dlg, IDM_STREDLIST );
        ok = ( lbox != NULL );
    }

    if ( ok ) {
        ret = SendMessage ( lbox, LB_GETCURSEL, 0, 0 );
        ok = ( ret != LB_ERR );
    }

    if ( ok ) {
        ok = WDeleteEditWinLBoxEntry ( einfo, (int) ret, TRUE );
    }

    return ( ok );
}

Bool WDeleteStringData( WStringEditInfo *einfo, WStringBlock *block,
                        uint_16 id, Bool *bdel )
{
    Bool        ok;

    ok = ( einfo && einfo->tbl && block && bdel );

    if( ok ) {
        *bdel = FALSE;
        if( block->block.String[ id & 0xf ] != NULL ) {
            WMemFree( block->block.String[ id & 0xf ] );
            block->block.String[ id & 0xf ] = NULL;
            if( WIsBlockEmpty( block ) ) {
                ok = WRemoveStringBlock( einfo->tbl, block );
                *bdel = TRUE;
            }
        }
    }

    return( ok );
}

Bool WDeleteEditWinLBoxEntry ( WStringEditInfo *einfo, int pos, Bool free_it )
{
    HWND                lbox;
    Bool                ok;
    Bool                bdel;
    WStringBlock        *block;
    uint_16             string_id;
    LRESULT             ret, max;

    ok = ( einfo && einfo->edit_dlg && einfo->tbl );

    if( ok ) {
        lbox = GetDlgItem ( einfo->edit_dlg, IDM_STREDLIST );
        ok = ( lbox != NULL );
    }

    if( ok ) {
        ret = SendMessage ( lbox, LB_GETCOUNT, 0, 0 );
        max = ret;
        ok = ( ret && ( ret != LB_ERR ) && ( pos < (int)ret ) );
    }

    if( ok ) {
        string_id = (uint_16 )(void *)
            SendMessage ( lbox, LB_GETITEMDATA, (WPARAM) pos, 0 );
        block = WFindStringBlock( einfo->tbl, string_id );
        ok = ( block != NULL );
    }

    if( ok ) {
        if( free_it ) {
            ok = WDeleteStringData( einfo, block, string_id, &bdel );
            if( bdel ) {
                block = NULL;
            }
        }
    }

    if ( ok ) {
        einfo->info->modified = TRUE;
        ret = SendMessage ( lbox, LB_DELETESTRING, (WPARAM) pos, 0 );
        ok = ( ret != LB_ERR );
    }

    if ( ok ) {
        einfo->current_block  = NULL;
        einfo->current_string = 0;
        einfo->current_pos    = -1;
        pos = min ( max-2, pos );
        ret = SendMessage ( lbox, LB_SETCURSEL, (WPARAM) pos, 0 );
        ok = ( ret != LB_ERR );
        if ( ok ) {
            WHandleSelChange ( einfo );
        }
    }

    return ( ok );
}

