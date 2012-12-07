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
#include <commdlg.h>
#include <dlgs.h>
#include <direct.h>

#include <string.h>
#include <stdlib.h>
#include "wglbl.h"
#include "winst.h"
#include "wmsg.h"
#include "rcstr.gh"
#include "wctl3d.h"
#include "wstrdup.h"
#include "wgetfn.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern UINT WINEXPORT WOpenHookProc ( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    OPENFILE
,   SAVEFILE
} WGetFileAction;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static char *WGetFileName ( WGetFileStruct *, HWND, DWORD, WGetFileAction );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static char  WFnTitle[_MAX_PATH];
static char  WFn[_MAX_PATH];
static char  WInitialDir[_MAX_PATH] = { 0 };
static int   WFileFilter = 1;

static char *WFindFileFilterFromIndex( char *filter, int index )
{
    int ind;
    int i;

    if( filter != NULL ) {
        ind = 1;
        for( i=0; ; i++ ) {
            if( filter[i] == '\0' ) {
                if( filter[i+1] == '\0' ) {
                    break;
                }
                ind++;
                if( ( ind % 2 == 0 ) && ( ind / 2 == index ) ) {
                    return( &filter[i+1] );
                }
            }
        }
    }

    return( "*.*" );
}

int WGetFileFilter ( void )
{
    return ( WFileFilter );
}

void WSetFileFilter ( int filter )
{
    WFileFilter = filter;
}

char *WGetInitialDir ( void )
{
    if ( *WInitialDir ) {
        return ( WInitialDir );
    }
    return ( NULL );
}

void WSetInitialDir ( char *dir )
{
    int len;

    if ( dir && *dir ) {
        len = strlen ( dir );
        if ( len >= _MAX_PATH ) {
            return;
        }
        strcpy ( WInitialDir, dir );
    }

}

char *WGetOpenFileName ( HWND parent, WGetFileStruct *gf )
{
    return ( WGetFileName ( gf, parent, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                        OFN_FILEMUSTEXIST, OPENFILE ) );
}

char *WGetSaveFileName ( HWND parent, WGetFileStruct *gf )
{
    return ( WGetFileName ( gf, parent, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST |
                                        OFN_OVERWRITEPROMPT, SAVEFILE ) );
}

char *WGetFileName ( WGetFileStruct *gf, HWND owner, DWORD flags,
                     WGetFileAction action )
{
    OPENFILENAME    wofn;
    Bool            ret;
    DWORD           error;
    int             len;
    char            fn_drive[_MAX_DRIVE];
    char            fn_dir[_MAX_DIR];
    char            fn_name[_MAX_FNAME];
    char            fn_ext[_MAX_EXT+1];
    HINSTANCE       app_inst;

    if ( !gf ) {
        return ( NULL );
    }

    app_inst = WGetEditInstance ();

    if ( !app_inst || !owner ) {
        return ( NULL );
    }

    if ( gf->title ) {
        len = strlen ( gf->title );
        if ( len < _MAX_PATH ) {
            memcpy ( WFnTitle, gf->title, len + 1 );
        } else {
            memcpy ( WFnTitle, gf->title, _MAX_PATH );
            WFnTitle[_MAX_PATH-1] = 0;
        }
    } else {
        WFnTitle[0] = 0;
    }

    if ( gf->file_name && *gf->file_name ) {
        _splitpath ( gf->file_name, fn_drive, fn_dir, fn_name, fn_ext );
        if ( *fn_drive || *fn_dir ) {
            _makepath  ( WInitialDir, fn_drive, fn_dir, NULL, NULL );
        }
        _makepath  ( WFn, NULL, NULL, fn_name, fn_ext );
    } else {
        WFn[0] = 0;
    }

    /* set the initial directory */
    if ( !*WInitialDir ) {
        getcwd ( WInitialDir, _MAX_PATH );
        WFileFilter = 1;
    }

#if !defined (__NT__)
    //ctl3d no longer requires this
    flags |= OFN_ENABLEHOOK;
#endif

    /* initialize the OPENFILENAME struct */
    memset (&wofn, 0, sizeof(OPENFILENAME));

    /* fill in non-variant fields of OPENFILENAME struct */
    wofn.lStructSize       = sizeof(OPENFILENAME);
    wofn.hwndOwner         = owner;
    wofn.hInstance         = app_inst;
    wofn.lpstrFilter       = gf->filter;
    wofn.lpstrCustomFilter = NULL;
    wofn.nMaxCustFilter    = 0;
    wofn.nFilterIndex      = WFileFilter;
    wofn.lpstrFile         = WFn;
    wofn.nMaxFile          = _MAX_PATH;
    wofn.lpstrFileTitle    = WFnTitle;
    wofn.nMaxFileTitle     = _MAX_PATH;
    wofn.lpstrInitialDir   = WInitialDir;
    wofn.lpstrTitle        = WFnTitle;
    wofn.Flags             = flags;
#if !defined (__NT__)
    wofn.lpfnHook          = (LPVOID) MakeProcInstance
                                ( (LPVOID) WOpenHookProc, app_inst );
#endif

#if 0
    wofn.nFileOffset       = 0L;
    wofn.nFileExtension    = 0L;
    wofn.lpstrDefExt       = NULL;
    wofn.lCustData         = NULL;
    wofn.lpfnHook            = NULL;
    wofn.lpTemplateName    = NULL;
#endif

    if ( action == OPENFILE ) {
        ret = GetOpenFileName ((LPOPENFILENAME)&wofn);
    } else if ( action == SAVEFILE ) {
        ret = GetSaveFileName ((LPOPENFILENAME)&wofn);
    } else {
        return ( NULL );
    }

#ifndef __NT__
    if ( wofn.lpfnHook ) {
        FreeProcInstance ( (FARPROC) wofn.lpfnHook );
    }
#endif

    if( !ret ) {
        error = CommDlgExtendedError();
        if( error ) {
            WDisplayErrorMsg( W_ERRORSELFILE );
        }
        return( NULL );
    } else {
        memcpy(WInitialDir,WFn,wofn.nFileOffset);
        if ( ( WInitialDir[wofn.nFileOffset-1] == '\\' ) &&
             ( WInitialDir[wofn.nFileOffset-2] != ':' )  ) {
            WInitialDir[wofn.nFileOffset-1] = '\0';
        } else {
            WInitialDir[wofn.nFileOffset] = '\0';
        }
        WFileFilter = wofn.nFilterIndex;
        _splitpath( WFn, NULL, NULL, NULL, (fn_ext+1) );
        if( !fn_ext[1] ) {
            char *out_ext;
            out_ext = WFindFileFilterFromIndex( gf->filter,
                                                wofn.nFilterIndex );
            if( out_ext[2] != '*' ) {
                strcat( WFn, &out_ext[1] );
            }
        }
    }

    UpdateWindow( owner );

    return( WStrDup( WFn ) );
}

UINT WINEXPORT WOpenHookProc( HWND hwnd, UINT msg,
                              WPARAM wparam, LPARAM lparam )
{
    _wtouch(wparam);
    _wtouch(lparam);

    switch( msg ) {
        case WM_INITDIALOG:
            // We must call this to subclass the directory listbox even
            // if the app calls Ctl3dAutoSubclass (commdlg bug)
            //WCtl3dSubclassDlg( hwnd, CTL3D_ALL );
            {
                char    *title;

                title = WAllocRCString( W_GETFNCOMBOTITLE );
                if( title ) {
                    SendDlgItemMessage( hwnd, stc2, WM_SETTEXT, 0, (LPARAM)title );
                    WFreeRCString( title );
                }
            }

            return( TRUE );
    }

    return( FALSE );
}

Bool WGetInternalRESName( char *filename, char *newname )
{
    char                fn_drive[_MAX_DRIVE];
    char                fn_dir[_MAX_DIR];
    char                fn_name[_MAX_FNAME];
    char                fn_ext[_MAX_EXT+1];

    if( filename && newname ) {
        _splitpath( filename, fn_drive, fn_dir, fn_name, fn_ext );
        strcpy( fn_ext, ".res" );
        _makepath( newname, fn_drive, fn_dir, fn_name, fn_ext );
        return( TRUE );
    }

    return( FALSE );
}

void WMassageFilter( char *filter )
{
    if( filter ) {
        for( ; *filter; filter++ ) {
            if( *filter == '\t' ) {
                *filter = '\0';
            }
        }
    }
}

