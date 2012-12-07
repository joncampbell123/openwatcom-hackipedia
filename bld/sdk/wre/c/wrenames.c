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
#include <stdio.h>
#include <string.h>
#include "wrdll.h"
#include "wreglbl.h"
#include "wretoolb.h"
#include "wrestrdp.h"
#include "wreres.h"
#include "wremain.h"
#include "wremem.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wrehints.h"
#include "wreseted.h"
#include "wrestat.h"
#include "wreftype.h"
#include "wrenames.h"
#include "wrrnames.h"
#include "wre_rc.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definition                                                          */
/****************************************************************************/

WRETypeName WRETypeNames[] =
{
    { 12 /* RT_GROUP_CURSOR */  , WRE_CURSORGROUPNAME   , FALSE }
,   { 14 /* RT_GROUP_ICON */    , WRE_ICONGROUPNAME     , FALSE }
,   { (uint_16)RT_BITMAP        , WRE_BITMAPNAME        , FALSE }
,   { (uint_16)RT_MENU          , WRE_MENUNAME          , FALSE }
,   { (uint_16)RT_DIALOG        , WRE_DIALOGNAME        , FALSE }
,   { (uint_16)RT_STRING        , WRE_STRINGNAME        , FALSE }
,   { (uint_16)RT_FONT          , WRE_FONTNAME          , FALSE }
,   { (uint_16)RT_ACCELERATOR   , WRE_ACCELNAME         , FALSE }
,   { (uint_16)RT_RCDATA        , WRE_USERDATANAME      , FALSE }
,   { (uint_16)RT_FONTDIR       , WRE_FONTDIRNAME       , FALSE }
,   { (uint_16)RT_CURSOR        , WRE_CURSORNAME        , TRUE  }
,   { (uint_16)RT_ICON          , WRE_ICONNAME          , TRUE  }
,   { (uint_16)RT_FONTDIR       , WRE_FONTDIRNAME       , FALSE }
,   { (uint_16)RT_MESSAGETABLE  , WRE_MESSAGETABLENAME  , FALSE }
,   { (uint_16)RT_VERSION       , WRE_VERSIONNAME       , FALSE }
,   { (uint_16)RT_DLGINCLUDE    , WRE_DLGINCLUDENAME    , FALSE }
,   { 0                         , 0                     , FALSE }
};

static Bool WRESetResNamesFromTypeNode( HWND lbox, WResTypeNode *tnode )
{
    return( WRSetResNamesFromTypeNode( lbox, tnode ) );
}

static void WREResetListbox( HWND lbox )
{
    if( lbox != (HWND)NULL ) {
        SendMessage( lbox, LB_RESETCONTENT, 0, 0 );
    }
}

static int WREFindTypeLBoxIndex( HWND lbox, uint_16 type,
                                 WResTypeNode **typeNode )
{
    WResTypeNode        *tnode;
    LRESULT             count;
    int                 i;
    Bool                ok;

    ok = ( lbox != (HWND)NULL );

    if( ok ) {
        count = SendMessage( lbox, LB_GETCOUNT, 0, 0 );
        ok = ( count != LB_ERR );
    }

    if( ok ) {
        for( i = 0; i<count; i++ ) {
            tnode = (WResTypeNode *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)i, 0 );
            if( tnode != NULL ) {
                if( (uint_16)tnode->Info.TypeName.ID.Num == type ) {
                    if( typeNode != NULL ) {
                        *typeNode = tnode;
                    }
                    return( i );
                }
            }
        }
    }

    return( -1 );
}

char *WREGetResName( WResResNode *rnode, uint_16 type )
{
    return( WRGetResName( rnode, type ) );
}

Bool WREAddToTypeListBox( HWND lbox, WResTypeNode *tnode )
{
    char                *text;
    WRETypeName         *tn;
    Bool                ok;

    text = NULL;

    ok = ( tnode && lbox != (HWND)NULL );

    if( ok ) {
        if( tnode->Info.TypeName.IsName ) {
            text = WResIDToStr( &(tnode->Info.TypeName) );
        } else {
            tn = WREGetTypeNameFromRT( (uint_16)tnode->Info.TypeName.ID.Num );
            if( tn && ( tn->name != -1 ) && !tn->exclude ) {
                text = tn->typeName;
            }
        }
        ok = ( text != NULL );
    }

    if( ok ) {
        ok = WRESetLBoxWithStr( lbox, text, tnode );
    }

    if( text && tnode->Info.TypeName.IsName ) {
        WREMemFree( text );
    }

    if( !ok && tn != NULL && tn->exclude ) {
        return( TRUE );
    }

    return( ok );
}

static Bool WREInitTypeListBox( HWND hDlg, WResDir dir )
{
    WResTypeNode        *tnode;
    HWND                lbox;
    int                 count;
    Bool                ok;

    if( hDlg == (HWND)NULL ) {
        return( FALSE );
    }

    WREResetListbox( GetDlgItem( hDlg, IDM_RNRES ) );

    lbox = GetDlgItem( hDlg, IDM_RNTYPE );
    if( lbox == (HWND)NULL ) {
        return( FALSE );
    }

    WREResetListbox( lbox );

    if( dir == NULL ) {
        return( TRUE );
    }

    ok = TRUE;
    count = 0;
    for( tnode = dir->Head; tnode && ok; tnode = tnode->Next ) {
        if( !tnode->Info.TypeName.IsName ) {
            WREAddToTypeListBox( lbox, tnode );
            ++count;
        }
    }

    ok = ( count != 0 );

    return( ok );
}

Bool WREInitResourceWindow( WREResInfo *info, uint_16 type )
{
    Bool           ok;

    ok = ( info != NULL && info->info != NULL );

    if( ok ) {
        ok = WREInitTypeListBox( info->info_win, info->info->dir );
    }

    if( ok ) {
        ok = WRESetResNamesFromType( info, type, FALSE, NULL, 0 );
    }

    return ( ok );
}

Bool WRESetResNamesFromType( WREResInfo *info, uint_16 type, Bool force,
                             WResID *name, int index )
{
    HWND                resLbox;
    HWND                typeLbox;
    int                 typeIndex;
    WResTypeNode        *tnode;
    LRESULT             max_index;
    char                *str;
    Bool                ok;

    tnode = NULL;

    ok = ( info != NULL );

    if( ok && type && ( info->current_type == type ) && !force ) {
        return( TRUE );
    }

    if( ok ) {
        resLbox  = GetDlgItem( info->info_win, IDM_RNRES );
        typeLbox = GetDlgItem( info->info_win, IDM_RNTYPE );
        ok = ( ( resLbox != (HWND)NULL ) && ( typeLbox != (HWND)NULL ) );
    }

    if( ok ) {
        WREResetListbox( resLbox );
        typeIndex = 0;
        if( type != 0 ) {
            typeIndex = WREFindTypeLBoxIndex( typeLbox, type, &tnode );
        } else {
            LRESULT count;
            count = SendMessage( typeLbox, LB_GETCOUNT, 0, 0 );
            if( count != 0 && count != LB_ERR ) {
                tnode = (WResTypeNode *)
                    SendMessage( typeLbox, LB_GETITEMDATA,
                                 (WPARAM)typeIndex, 0 );
            }
        }
        if( typeIndex == -1 ) {
            return( TRUE );
        }
        info->current_type = type;
        SendMessage( typeLbox, LB_SETCURSEL, (WPARAM)typeIndex, 0 );
    }

    if( ok ) {
        if( tnode != NULL ) {
            info->current_type = tnode->Info.TypeName.ID.Num;
            ok = WRESetResNamesFromTypeNode( resLbox, tnode );
        } else {
            info->current_type = 0;
        }
    }

    if( ok ) {
        if( name ) {
            index = LB_ERR;
            str = WResIDToStr( name );
            if( str ) {
                index = (int)
                    SendMessage( resLbox, LB_FINDSTRING, 0, (LPARAM) str );
                WREMemFree( str );
            }
            if( index == LB_ERR ) {
                index = 0;
            }
        }
        max_index = SendMessage( resLbox, LB_GETCOUNT, 0, 0 );
        if( max_index == LB_ERR ) {
            max_index = 0;
        }
        index = min( index, max_index-1 );
        SendMessage( resLbox, LB_SETCURSEL, (WPARAM)index, 0 );
        WRESetTotalText( info );
        if( GetActiveWindow() == WREGetMainWindowHandle() ) {
            SetFocus( resLbox );
        }
    }

    return( ok );
}

Bool WREAddResNames( WREResInfo *info )
{
    HWND                resLbox;
    HWND                typeLbox;
    WResTypeNode        *tnode;
    Bool                redrawOff;
    LRESULT             index;
    Bool                ok;

    redrawOff = FALSE;

    ok = ( info != NULL );

    if( ok ) {
        typeLbox = GetDlgItem( info->info_win, IDM_RNTYPE );
        resLbox = GetDlgItem( info->info_win, IDM_RNRES );
        ok = ( ( typeLbox != (HWND)NULL ) && ( resLbox != (HWND)NULL ) );
    }

    if( ok ) {
        SendMessage( resLbox, WM_SETREDRAW, FALSE, 0 );
        redrawOff = TRUE;
        WREResetListbox( resLbox );
        index = SendMessage( typeLbox, LB_GETCURSEL, 0, 0 );
        ok = ( index != LB_ERR );
    }

    if( ok ) {
        tnode = (WResTypeNode *)
            SendMessage( typeLbox, LB_GETITEMDATA, (WPARAM)index, 0 );
        ok = ( tnode != NULL );
    }

    if( ok ) {
        info->current_type = tnode->Info.TypeName.ID.Num;
        ok = WRESetResNamesFromTypeNode( resLbox, tnode );
    }

    if( redrawOff ) {
        SendMessage( resLbox, WM_SETREDRAW, TRUE, 0 );
        InvalidateRect( resLbox, NULL, TRUE );
        WRESetTotalText( info );
    }

    return( ok );
}

static char     *WRETotalText           = NULL;
static char     *WRETotalTextOne        = NULL;
static char     *WRETotalTextNone       = NULL;

void WREFiniTotalText( void )
{
    if( WRETotalText != NULL ) {
        WREFreeRCString( WRETotalText );
        WRETotalText = NULL;
    }
    if( WRETotalTextOne != NULL ) {
        WREFreeRCString( WRETotalTextOne );
        WRETotalTextOne = NULL;
    }
    if( WRETotalTextNone != NULL ) {
        WREFreeRCString( WRETotalTextNone );
        WRETotalTextNone = NULL;
    }
}

Bool WREInitTotalText( void )
{
    WRETotalText = WREAllocRCString( WRE_TOTALTEXT );
    WRETotalTextOne = WREAllocRCString( WRE_TOTALTEXT_ONE );
    WRETotalTextNone = WREAllocRCString( WRE_TOTALTEXT_NONE );
    if( !WRETotalText || !WRETotalTextOne || !WRETotalTextNone ) {
        return( FALSE );
    }
    return( TRUE );
}

void WRESetTotalText( WREResInfo *info )
{
    HWND        total;
    HWND        lbox;
    LRESULT     count;
    char        *buf;

    if( !info || !WRETotalText || !WRETotalTextOne || !WRETotalTextNone ) {
        return;
    }

    lbox = GetDlgItem( info->info_win, IDM_RNRES );
    total = GetDlgItem( info->info_win, IDM_RNTOTALTEXT );
    if( ( lbox == (HWND)NULL ) || ( total == (HWND)NULL ) ) {
        return;
    }

    count = SendMessage( lbox, LB_GETCOUNT, 0, 0 );
    if( count == LB_ERR ) {
        count = 0;
    }

    if( count == 0 ) {
        SendMessage( total, WM_SETTEXT, 0, (LPARAM)(LPCSTR)WRETotalTextNone );
    } else if( count == 1 ) {
        SendMessage( total, WM_SETTEXT, 0, (LPARAM)(LPCSTR)WRETotalTextOne );
    } else {
        buf = WREMemAlloc( strlen( WRETotalText ) + 20 + 1 );
        if( buf ) {
            sprintf( buf, WRETotalText, count );
            SendMessage( total, WM_SETTEXT, 0, (LPARAM)(LPCSTR)buf );
            WREMemFree( buf );
        }
    }

    return;
}

WRETypeName *WREGetTypeNameFromRT( uint_16 type )
{
    int i;

    for ( i = 0; WRETypeNames[i].name; i++ ) {
        if( WRETypeNames[i].type == type ) {
            return ( &WRETypeNames[i] );
        }
    }
    return ( FALSE );
}

void WREInitTypeNames( void )
{
    int i;

    for( i = 0; WRETypeNames[i].name; i++ ) {
        WRETypeNames[i].typeName = WREAllocRCString( WRETypeNames[i].name );
    }
}

void WREFiniTypeNames( void )
{
    int i;

    for( i = 0; WRETypeNames[i].name; i++ ) {
        if( WRETypeNames[i].typeName != NULL ) {
            WREFreeRCString( WRETypeNames[i].typeName );
        }
    }
}

