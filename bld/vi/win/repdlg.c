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


#include "vi.h"
#include "repdlg.h"

static fancy_find findData =
    { TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, 0, -1, -1, NULL, 0, NULL, 0 };

/*
 * RepDlgProc - callback routine for find & replace dialog
 */
BOOL WINEXP RepDlgProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    int                 curr;
    int                 i;
    int                 cmd;
    DWORD               index;
    char                find[MAX_INPUT_LINE];
    history_data        *h;
    char                *ptr;
    RECT                pos;

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        if( findData.posx == -1 && findData.posy == -1 ) {
            CenterWindowInRoot( hwnd );
        } else {
            SetWindowPos( hwnd, (HWND)NULLHANDLE, findData.posx, findData.posy, 
                0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOZORDER );
        }
        EditSubClass( hwnd, REP_FIND, &FindHist );
        CheckDlgButton( hwnd, REP_IGNORE_CASE, findData.case_ignore );
        CheckDlgButton( hwnd, REP_REGULAR_EXPRESSIONS, findData.use_regexp );
        // CheckDlgButton( hwnd, REP_SEARCH_BACKWARDS, !findData.search_forward );
        CheckDlgButton( hwnd, REP_SEARCH_WRAP, findData.search_wrap );
        CheckDlgButton( hwnd, REP_PROMPT_ON_REPLACE, findData.prompt );
        // CheckDlgButton( hwnd, REP_SELECTION_ONLY, findData.selection );
        SetDlgItemText( hwnd, REP_FIND, findData.find );
        SetDlgItemText( hwnd, REP_REPLACE, findData.replace );
        curr = FindHist.curr + FindHist.max - 1;
        for( i = 0; i < FindHist.max; i++ ) {
            if( FindHist.data[curr % FindHist.max] != NULL ) {
                SendDlgItemMessage( hwnd, REP_LISTBOX, LB_ADDSTRING, 0,
                                    (LONG) FindHist.data[curr % FindHist.max] );
            }
            curr--;
            if( curr < 0 ) {
                break;
            }
        }
        return( TRUE );
    case WM_CLOSE:
        GetWindowRect( hwnd, &pos );
        findData.posx = pos.left;
        findData.posy = pos.top;
        PostMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( IDCANCEL, 0, 0 ) );
        return( TRUE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case REP_LISTBOX:
            cmd = GET_WM_COMMAND_CMD( wparam, lparam );
            if( cmd == LBN_SELCHANGE || cmd == LBN_DBLCLK ) {
                index = SendDlgItemMessage( hwnd, REP_LISTBOX, LB_GETCURSEL, 0, 0L );
                if( index == LB_ERR ) {
                    break;
                }
                SendDlgItemMessage( hwnd, REP_LISTBOX, LB_GETTEXT, index, (LONG) find );
                SetDlgItemText( hwnd, REP_FIND, find );
                if( cmd == LBN_DBLCLK ) {
                    PostMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( IDOK, 0, 0 ) );
                }
            }
            break;
        case IDCANCEL:
            GetWindowRect( hwnd, &pos );
            findData.posx = pos.left;
            findData.posy = pos.top;
            RemoveEditSubClass( hwnd, REP_FIND );
            EndDialog( hwnd, FALSE );
            break;
        case IDOK:
            GetDlgItemText( hwnd, REP_FIND, findData.find, findData.findlen );
            GetDlgItemText( hwnd, REP_REPLACE, findData.replace, findData.replacelen );
            findData.case_ignore = IsDlgButtonChecked( hwnd, REP_IGNORE_CASE );
            findData.use_regexp = IsDlgButtonChecked( hwnd, REP_REGULAR_EXPRESSIONS );
            // findData.search_forward = !IsDlgButtonChecked( hwnd, REP_SEARCH_BACKWARDS );
            findData.search_wrap = IsDlgButtonChecked( hwnd, REP_SEARCH_WRAP );
            findData.prompt = IsDlgButtonChecked( hwnd, REP_PROMPT_ON_REPLACE );
            // findData.selection = IsDlgButtonChecked( hwnd, REP_SELECTION_ONLY );
            h = &FindHist;
            curr = h->curr + h->max - 1;
            ptr = NULL;
            if( curr >= 0 ) {
                ptr = h->data[curr % h->max];
            }
            if( ptr == NULL || strcmp( ptr, findData.find ) ) {
                AddString2( &(h->data[h->curr % h->max]), findData.find );
                h->curr += 1;
            }
            GetWindowRect( hwnd, &pos );
            findData.posx = pos.left;
            findData.posy = pos.top;
            RemoveEditSubClass( hwnd, REP_FIND );
            EndDialog( hwnd, TRUE );
            break;
        default:
            return( FALSE );
        }
        return( TRUE );
    }
    return( FALSE );

} /* RepDlgProc */

/*
 * GetReplaceStringDialog - create dialog settings
 */
bool GetReplaceStringDialog( fancy_find *ff )
{
    DLGPROC     proc;
    bool        rc;

    findData.find = ff->find;
    findData.findlen = ff->findlen;
    findData.replace = ff->replace;
    findData.replacelen = ff->replacelen;
    proc = (DLGPROC) MakeProcInstance( (FARPROC) RepDlgProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "REPDLG", Root, proc );
    FreeProcInstance( (FARPROC) proc );
    SetWindowCursor();

    if( strlen( findData.find ) == 0 ) {
        // no find string so pretend user cancelled
        rc = FALSE;
    }

    if( rc ) {
        ff->case_ignore = findData.case_ignore;
        ff->use_regexp = findData.use_regexp;
        // ff->search_forward = findData.search_forward;
        ff->search_wrap = findData.search_wrap;
        ff->prompt = findData.prompt;
        // ff->selection = findData.selection;
    }
    return( rc );

} /* GetReplaceStringDialog */
