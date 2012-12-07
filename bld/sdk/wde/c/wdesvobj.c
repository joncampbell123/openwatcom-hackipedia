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
#include "wdeglbl.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdemem.h"
#include "wdei2mem.h"
#include "wdewait.h"
#include "wrdll.h"
#include "wdeselft.h"
#include "wdegetfn.h"
#include "wdesdup.h"
#include "wdesvdlg.h"
#include "wdesvobj.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static Bool WdeSaveObjectAs   ( WdeResInfo *, WdeDialogBoxInfo *, char **,
                                WResID *, void *, int, WResLangType *, Bool );
static Bool WdeSaveObjectInto ( WdeResInfo *, WdeDialogBoxInfo *, WResID *,
                                void *, int, WResLangType * );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/
extern char     *WdeDlgSaveIntoTitle;
extern char     *WdeDlgSaveAsTitle;
extern char     *WdeDlgSaveTitle;
extern char     *WdeResSaveFilter;

extern Bool WdeCreateDLGName( char *filename, char *newname );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

Bool WdeSaveObject( WdeResInfo *rinfo, WdeDialogBoxInfo *dbi,
                    char **fname, WResID *rname, WResLangType *langtype,
                    Bool save_into, Bool get_name )
{
    void                *rdata;
    uint_32             size;
    Bool                ok;
    WResLangType        lang;


    WdeSetWaitCursor ( TRUE );

    ok = ( rinfo && dbi );

    if( ok ) {
        if( langtype ) {
            lang = *langtype;
        } else {
            lang.lang    = DEF_LANG;
            lang.sublang = DEF_SUBLANG;
        }
        ok = WdeDBI2Mem( dbi, (unsigned_8 **)&rdata, &size );
    }

    if( ok ) {
        if( save_into ) {
            ok = WdeSaveObjectInto( rinfo, dbi, rname, rdata, size, &lang );
        } else {
            ok = WdeSaveObjectAs( rinfo, dbi, fname, rname, rdata, size,
                                  &lang, get_name );
        }
    }

    if( rdata ) {
        WdeMemFree( rdata );
    }

    WdeSetWaitCursor( FALSE );

    return( ok );
}

Bool WdeSaveObjectAs( WdeResInfo *rinfo, WdeDialogBoxInfo *dbi,
                      char **file_name, WResID *name, void *rdata, int size,
                      WResLangType *lang, Bool get_name )
{
    char                *fname;
    WRFileType          ftype;
    WdeGetFileStruct    gf;
    WRSaveIntoData      idata;
    WdeResDlgItem       ditem;
    Bool                is_rc;
    Bool                ok;

    fname = NULL;
    idata.type = NULL;

    ok = ( rinfo && dbi && file_name && name && rdata );

    if( ok ) {
        if( get_name || !*file_name ) {
            gf.file_name = *file_name;
            gf.title     = WdeDlgSaveTitle;
            if( get_name ) {
                gf.title     = WdeDlgSaveAsTitle;
            }
            gf.filter    = WdeResSaveFilter;
            fname        = WdeGetSaveFileName ( &gf );
        } else {
            fname        = WdeStrDup ( *file_name );
        }
        ok = ( fname && *fname );
    }

    if( ok ) {
        ftype = WdeSelectFileType( fname, rinfo->is32bit );
        ok = ( ftype != WR_DONT_KNOW );
    }

    if( ok ) {
        is_rc = WdeIsFileAnRCFile( fname );
        ok = ( (idata.type = WResIDFromNum( (long)RT_DIALOG )) != NULL );
    }

    if( ok ) {
        memset( &ditem, 0, sizeof(WdeResDlgItem) );
        ditem.dialog_info = dbi;
        ditem.dialog_name = name;
        if( is_rc ) {
            ok = WdeSaveObjectToRC( fname, rinfo, &ditem, FALSE );
        }
    }

    // if we are saving a .RES file then auto create a dlg script
    if( ok ) {
        if( ( ftype == WR_WIN16M_RES ) ||
            ( ftype == WR_WIN16W_RES ) ||
            ( ftype == WR_WINNTM_RES ) ||
            ( ftype == WR_WINNTW_RES ) ) {
            char        dlgName[_MAX_PATH];
            if( WdeCreateDLGName( fname, dlgName ) ) {
                ok = WdeSaveObjectToRC( dlgName, rinfo, &ditem, FALSE );
            }
        }
    }

    if( ok ) {
        idata.next = NULL;
        idata.name = name;
        idata.data = rdata;
        idata.lang = *lang;
        idata.size = size;
        idata.MemFlags = dbi->MemoryFlags;
        ok = WRSaveObjectAs ( fname, ftype, &idata );
    }

    if( idata.type ) {
        WResIDFree( idata.type );
    }

    if( ok ) {
        if( fname ) {
            if( *file_name ) {
                WdeMemFree( *file_name );
            }
            *file_name = fname;
        }
    } else {
        if( fname ) {
            WdeMemFree( fname );
        }
    }

    return( ok );
}

Bool WdeSaveObjectInto( WdeResInfo *rinfo, WdeDialogBoxInfo *dbi,
                        WResID *name, void *data, int size,
                        WResLangType *lang )
{
    char                *fname;
    WdeGetFileStruct    gf;
    int                 dup;
    WRSaveIntoData      idata;
    WdeResDlgItem       ditem;
    Bool                is_rc;
    Bool                ok;

    idata.type = NULL;
    fname = NULL;
    dup   = FALSE;

    ok = ( rinfo && dbi && name && data );

    if ( ok ) {
        gf.file_name = NULL;
        gf.title     = WdeDlgSaveIntoTitle;
        gf.filter    = WdeResSaveFilter;
        fname        = WdeGetOpenFileName ( &gf );
        ok = ( fname && *fname );
    }

    if( ok ) {
        is_rc = WdeIsFileAnRCFile( fname );
        if( !is_rc ) {
            ok = ( (idata.type = WResIDFromNum( (long)RT_DIALOG )) != NULL );
        }
    }

    if ( ok ) {
        if( is_rc ) {
            memset( &ditem, 0, sizeof(WdeResDlgItem) );
            ditem.dialog_info = dbi;
            ditem.dialog_name = name;
            ok = WdeSaveObjectToRC( fname, rinfo, &ditem, TRUE );
        } else {
            idata.next = NULL;
            idata.name = name;
            idata.data = data;
            idata.lang = *lang;
            idata.size = size;
            idata.MemFlags = dbi->MemoryFlags;
            ok = WRSaveObjectInto ( fname, &idata, &dup ) && !dup;
        }
    }

    if( dup ) {
        WdeDisplayErrorMsg( WDE_DUPRESNAMEINFILE );
    }

    if ( fname ) {
        WdeMemFree ( fname );
    }

    if( idata.type ) {
        WResIDFree( idata.type );
    }

    return ( ok );
}

