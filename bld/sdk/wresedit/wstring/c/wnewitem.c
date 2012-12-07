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
#include "win1632.h"
#include "wglbl.h"
#include "wmem.h"
#include "wmsg.h"
#include "rcstr.gh"
#include "wsetedit.h"
#include "wedit.h"
#include "wstrdup.h"
#include "wnewitem.h"
#include "sys_rc.h"
#include "widn2str.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define DEFAULT_STRING_ID       101

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

static Bool WQueryReplaceString( HWND parent )
{
    char        *title;
    char        *text;
    int         ret;

    text = WAllocRCString( W_QUERYREPLACESTRING );
    title = WAllocRCString( W_INSERTNEWSTRING );

    ret = MessageBox( parent, text, title,
                      MB_APPLMODAL | MB_ICONEXCLAMATION | MB_YESNO );
    if( text ) {
        WFreeRCString( text );
    }

    if( title ) {
        WFreeRCString( title );
    }

    return( ret == IDYES );
}

WStringBlock *WInsertStringData( WStringEditInfo *einfo, uint_16 id,
                                 char *text, char *symbol, Bool *replace )
{
    Bool                ok;
    WStringBlock        *block;

    ok = ( einfo && einfo->tbl && einfo->win && replace );

    if( ok ) {
        *replace = FALSE;
        block = WGetOrMakeStringBlock( einfo->tbl, id );
        ok = ( block != NULL );
    }

    if( ok ) {
        if( block->block.String[ id & 0xf ] == NULL ) {
            block->block.String[ id & 0xf ] = WResIDNameFromStr( text );
            block->symbol[ id & 0xf ] = WStrDup( symbol );
            einfo->info->modified = TRUE;
        } else {
            if( WQueryReplaceString( einfo->edit_dlg ) ) {
                WMemFree( block->block.String[ id & 0xf ] );
                block->block.String[ id & 0xf ] = WResIDNameFromStr( text );
                block->symbol[ id & 0xf ] = WStrDup( symbol );
                einfo->info->modified = TRUE;
                *replace = TRUE;
            } else {
                if( WIsBlockEmpty( block ) ) {
                    WRemoveStringBlock( einfo->tbl, block );
                }
                ok = FALSE;
            }
        }
    }

    if( !ok ) {
        return( NULL );
    }

    return( block );
}

Bool WInsertStringEntry ( WStringEditInfo *einfo )
{
    HWND                lbox;
    Bool                ok;
    Bool                replace;
    WStringBlock        *block;
    uint_16             id;
    char                *text;
    char                *symbol;
    int                 pos;

    text = NULL;
    symbol = NULL;
    replace = FALSE;

    ok = ( einfo && einfo->tbl && einfo->edit_dlg );

    if( ok ) {
        lbox = GetDlgItem ( einfo->edit_dlg, IDM_STREDLIST );
        ok = ( lbox != NULL );
    }

    if( ok ) {
        ok = WGetEditWindowText( einfo->edit_dlg, &text );
    }

    if ( ok ) {
        ok = WGetEditWindowID( einfo->edit_dlg, &symbol, &id,
                               einfo->info->symbol_table,
                               einfo->combo_change );
    }

    if( ok ) {
        if( id == 0 ) {
            id = DEFAULT_STRING_ID;
        }
        block = WInsertStringData( einfo, id, text, symbol, &replace );
        ok = ( block != NULL );
    }

    if( ok ) {
        pos = WFindStringPos( einfo->tbl, id );
        ok = ( pos != -1 );
    }

    if( ok ) {
        if( replace ) {
            SendMessage( lbox, LB_DELETESTRING, pos, 0 );
        }
        ok = WAddEditWinLBoxEntry( einfo, block, id, pos );
    }


    if ( ok ) {
        WSetEditWindowID( einfo->edit_dlg, id, block->symbol[ id & 0xf ] );
        ok = ( SendMessage ( lbox, LB_SETCURSEL, pos, 0 ) != LB_ERR );
        if ( ok ) {
            einfo->current_block  = block;
            einfo->current_string = id;
            einfo->current_pos    = pos;
#if 0
            einfo->current_block  = NULL;
            einfo->current_string = 0;
            einfo->current_pos    = -1;
            WHandleSelChange( einfo );
#endif
        }
    }

    if( ok ) {
        SetFocus( GetDlgItem( einfo->edit_dlg, IDM_STREDTEXT ) );
        SendDlgItemMessage( einfo->edit_dlg, IDM_STREDTEXT, EM_SETSEL, GET_EM_SETSEL_MPS( 0, -1 ));
    }

    if( symbol ) {
        WMemFree( symbol );
    }

    if( text ) {
        WMemFree( text );
    }

    return ( ok );
}

Bool WAddEditWinLBoxBlock( WStringEditInfo *einfo, WStringBlock *block,
                           int pos )
{
    int         i;

    if( block ) {
        for( i=0; i<STRTABLE_STRS_PER_BLOCK; i++ ) {
            if( block->block.String[i] != NULL ) {
                if( !WAddEditWinLBoxEntry( einfo, block,
                                           (block->blocknum & 0xfff0) + i,
                                           pos ) ) {
                    return( FALSE );
                }
                if( pos != -1 ) {
                    pos ++;
                }
            }
        }
    }

    return( TRUE );
}

Bool WAddEditWinLBoxEntry ( WStringEditInfo *einfo, WStringBlock *block,
                            uint_16 string_id, int pos )
{
    Bool    ok;
    char   *n;
    char   *lbtext;
    char   *text;
    char    idtext[35];
    int     tlen, idlen;
    HWND    lbox;

    text = NULL;
    lbtext = NULL;
    n = NULL;

    ok = ( einfo && einfo->edit_dlg && block );

    if ( ok ) {
        lbox = GetDlgItem ( einfo->edit_dlg, IDM_STREDLIST );
        ok = ( lbox != NULL );
    }

    if ( ok ) {
        text = WResIDNameToStr( block->block.String[ string_id & 0xf ] );
        ok = ( text != NULL );
    }

#define MAX_ID_CHARS    14
#define TAB_SIZE        8

    if( ok ) {
        if( block->symbol[ string_id & 0xf ] != NULL ) {
            strncpy( idtext, block->symbol[ string_id & 0xf ], MAX_ID_CHARS );
            idtext[MAX_ID_CHARS] = '\0';
        } else {
            utoa( (int)string_id, idtext, 10 );
        }
        idlen = strlen(idtext);
        idtext[ idlen ] = '\t';
        ++idlen;
        idtext[ idlen ] = '\0';
        tlen = strlen(text);
        lbtext = (char *) WMemAlloc( tlen + idlen + 4);
        ok = ( lbtext != NULL );
    }

    if( ok ) {
        memcpy( lbtext, idtext, idlen );
        lbtext[idlen] = ' ';
        lbtext[idlen+1] = '\"';
        memcpy( lbtext+idlen+2, text, tlen );
        lbtext[ tlen + idlen + 2 ] = '\"';
        lbtext[ tlen + idlen + 3 ] = '\0';
        n = WConvertStringFrom( lbtext, "\n", "n" );
        ok = WInsertLBoxWithStr( lbox, pos, n, (void *)string_id );
    }

    if( n ) {
        WMemFree( n );
    }

    if( text ) {
        WMemFree( text );
    }

    if( lbtext ) {
        WMemFree( lbtext );
    }

    return( ok );
}

