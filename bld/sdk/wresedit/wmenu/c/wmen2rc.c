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

#include "wglbl.h"
#include "wsetedit.h"
#include "wmem.h"
#include "wmen2rc.h"

#define DEPTH_MULT      4

typedef struct {
    MenuFlags   flag;
    char        *flagtext;
} FlagItem;

static FlagItem FlagItems[] =
{
    { MENU_GRAYED,      "GRAYED"        }
,   { MENU_INACTIVE,    "INACTIVE"      }
,   { MENU_BITMAP,      "BITMAP"        }
,   { MENU_CHECKED,     "CHECKED"       }
,   { MENU_MENUBARBREAK,"MENUBARBREAK"  }
,   { MENU_MENUBREAK,   "MENUBREAK"     }
//,   { MENU_OWNERDRAWN,        "OWNERDRAWN"    }
,   { MENU_HELP,        "HELP"          }
,   { 0,                NULL            }
};

static Bool WSetFlagsText( uint_16 flags, char **text )
{
    int         i;
    int         tlen;

    if( text == NULL ) {
        return( FALSE );
    }

    tlen = 0;
    *text = NULL;

    for( i=0; FlagItems[i].flagtext != NULL ; i++ ) {
        if( FlagItems[i].flag & flags ) {
            tlen += strlen( FlagItems[i].flagtext ) + 2;
        }
    }

    *text = (char *)WMemAlloc( tlen + 1 );
    if( *text == NULL ) {
        return( FALSE );
    }

    (*text)[0] = '\0';

    if( tlen == 0 ) {
        return( TRUE );
    }

    for( i=0; FlagItems[i].flagtext != NULL ; i++ ) {
        if( FlagItems[i].flag & flags ) {
            strcat( *text, ", " );
            strcat( *text, FlagItems[i].flagtext );
        }
    }

    return( TRUE );
}

static Bool WWriteMenuEntryItem( WMenuEntry *entry, FILE *fp, int depth )
{
    Bool        ok;
    char        *itemname;
    char        *text;
    char        *flagtext;

    text = NULL;
    flagtext = NULL;
    itemname = NULL;

    ok = ( entry && fp );

    if( ok ) {
        depth++;
        fprintf( fp, "%*s", DEPTH_MULT*depth, "" );
    }

    if( ok ) {
        if( !( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) ) {
            ok = FALSE;
            itemname = WGETMENUITEMTEXT(entry->item);
            if( itemname != NULL ) {
                text = WConvertStringFrom( itemname, "\t\x8\"\\", "ta\"\\" );
                ok = ( text != NULL );
            }
        }
    }

    if( ok ) {
        if( !( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) ) {
            ok = WSetFlagsText( entry->item->Item.Normal.ItemFlags, &flagtext );
        }
    }

    if( ok ) {
        if( entry->item->Item.Normal.ItemFlags & MENU_POPUP ) {
            fprintf( fp, "POPUP \"%s\"%s\n", text, flagtext );
        } else if( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) {
            fprintf( fp, "MENUITEM SEPARATOR\n" );
        } else {
            if( entry->symbol ) {
                fprintf( fp, "MENUITEM \"%s\"\t%s%s\n", text, entry->symbol,
                         flagtext );
            } else {
                fprintf( fp, "MENUITEM \"%s\"\t%u%s\n", text,
                         entry->item->Item.Normal.ItemID, flagtext );
            }
        }
    }

    if( flagtext != NULL ) {
        WMemFree( flagtext );
    }

    if( text != NULL ) {
        WMemFree( text );
    }

    return( ok );
}

static Bool WWriteDummyItem( FILE *fp, int depth )
{
    if( fp == NULL ) {
        return( FALSE );
    }

    if( depth != 0 ) {
        fprintf( fp, "%*s", DEPTH_MULT*depth, "" );
    }
    fwrite( "BEGIN\n", sizeof(char), 6, fp );
    fprintf( fp, "%*sMENUITEM SEPARATOR\t/* dummy menu entry */\n",
             DEPTH_MULT*(depth+1), "" );
    if( depth != 0 ) {
        fprintf( fp, "%*s", DEPTH_MULT*depth, "" );
    }
    fwrite( "END\n", sizeof(char), 4, fp );

    return( TRUE );
}

static Bool WWriteMenuPopupItem( WMenuEntry *entry, FILE *fp )
{
    Bool        ok;
    int         depth;

    ok = ( entry && fp );

    if( ok ) {
        depth = WGetMenuEntryDepth( entry );
        ok = ( depth != -1 );
    }

    if( ok ) {
        if( depth != 0 ) {
            fprintf( fp, "%*s", DEPTH_MULT*depth, "" );
        }
        fwrite( "BEGIN\n", sizeof(char), 6, fp );
    }

    if( ok ) {
        while( entry && ok ) {
            ok = WWriteMenuEntryItem( entry, fp, depth );
            if( ok && entry->item->IsPopup ) {
                if( entry->child ) {
                    ok = WWriteMenuPopupItem( entry->child, fp );
                } else {
                    ok = WWriteDummyItem( fp, depth+1 );
                }
            }
            entry = entry->next;
        }
    }

    if( ok ) {
        if( depth != 0 ) {
            fprintf( fp, "%*s", DEPTH_MULT*depth, "" );
        }
        fwrite( "END\n", sizeof(char), 4, fp );
    }

    return( ok );
}

Bool WWriteMenuToRC( WMenuEditInfo *einfo, char *file, Bool append )
{
    FILE        *fp;
    char        *rname;
    Bool        ok;

    rname = NULL;

    ok = ( einfo && einfo->menu );

    if( ok ) {
        if( append ) {
            fp = fopen( file, "at" );
        } else {
            fp = fopen( file, "wt" );
        }
        ok = ( fp != NULL );
    }

    if( ok ) {
        ok = ( ( rname = WResIDToStr( einfo->info->res_name ) ) != NULL );
    }

    if( ok ) {
        fprintf( fp, "%s MENU\n", rname );
    }

    if( ok ) {
        ok = WWriteMenuPopupItem( einfo->menu->first_entry, fp );
    }

    if( rname ) {
        WMemFree( rname );
    }

    if( fp ) {
        fclose( fp );
    }

    return( ok );
}

