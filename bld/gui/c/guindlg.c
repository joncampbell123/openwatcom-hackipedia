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


#include "guiwind.h"
#include "guidlg.h"
#include <string.h>

#if !defined( __NT__ ) || (GUI_IS_GUI == FALSE)
static gui_colour_set DlgColours[GUI_NUM_ATTRS+1] = {
  /* Fore              Back        */
  { GUI_BRIGHT_BLUE,  GUI_BRIGHT_WHITE },   /* GUI_MENU_PLAIN     */
  { GUI_BRIGHT_WHITE, GUI_BLUE },           /* GUI_MENU_STANDOUT  */
  { GUI_WHITE,        GUI_BRIGHT_WHITE },   /* GUI_BACKGROUND     */
  { GUI_RED,          GUI_WHITE },          /* GUI_TITLE_ACTIVE   */
  { GUI_GREY,         GUI_WHITE },          /* GUI_TITLE_INACTIVE */
  { GUI_BRIGHT_BLUE,  GUI_WHITE },          /* GUI_FRAME_ACTIVE   */
  { GUI_GREY,         GUI_WHITE },          /* GUI_FRAME_INACTIVE */
  { GUI_BRIGHT_CYAN,  GUI_CYAN }            /* GUI_FIRST_UNUSED   */
};
#endif

static gui_create_info DlgControl = {
    NULL,
    { 0, 0, 0, 0 },
    GUI_SCROLL_BOTH,
    GUI_SCROLL_EVENTS | GUI_ALL,
    NULL,
    0,
    NULL,
#if !defined( __NT__ ) || (GUI_IS_GUI == FALSE)
    GUI_NUM_ATTRS + 1,
    DlgColours,
#else
    0,
    NULL,
#endif
    NULL,
    NULL
};

static bool DlgModal = FALSE;

extern unsigned GUIDlgBuffGetText( gui_window *gui, unsigned id,
                             char *buff, unsigned max_len )
{
    char        *str;

    str = GUIGetText( gui, id );
    if( str == NULL ) {
        buff[0] = '\0';
    } else {
        strncpy( buff, str, max_len-1 );
        buff[max_len-1] = '\0';
        GUIMemFree( str );
    }
    return( strlen( buff ) );
}


static bool DlgRelocNum( gui_ord *pnum, int adjust, gui_coord *charuse )
{
    gui_ord     num;

    num = *pnum;
    if( num >= DLG_ROW_0 && num <= DLG_ROW_n ) {
        num -= DLG_ROW( 0 );
        num += adjust;
        num *= charuse->y;
    } else if( num >= DLG_COL_0 && num <= DLG_COL_n ) {
        num -= DLG_COL( 0 );
        num += adjust;
        num *= charuse->x;
    } else {
        return( FALSE );
    }
    *pnum = num;
    return( TRUE );
}


static void GetHalfAndAdjust( gui_coord *charuse,
                              gui_coord *half, gui_ord *char_ui_adjust )
{
    half->x = charuse->x / 2;
    half->y = charuse->y / 2;
    GUITruncToPixel( half );
    if( half->y == 0 ) {
        *char_ui_adjust = charuse->y;
    } else {
        *char_ui_adjust = 0;
    }
}

static void DlgSetCtlSizes( gui_control_info *control,
                            int num, gui_coord *charuse, gui_coord *charspace )
{
    gui_coord   half;
    gui_ord     char_ui_adjust;
    bool        reloc;

    charspace=charspace;
    GetHalfAndAdjust( charuse, &half, &char_ui_adjust );
    while( --num >= 0 ) {
        if( DlgRelocNum( &control->rect.x, 1, charuse ) ) {
            control->rect.x += half.x;
        }
        if( DlgRelocNum( &control->rect.y, 0, charuse ) ) {
            control->rect.y += half.y + char_ui_adjust;
        }
        reloc = FALSE;
        if( DlgRelocNum( &control->rect.width, 0, charuse ) ) {
            reloc = TRUE;
        }
        if( DlgRelocNum( &control->rect.height, 0, charuse ) ) {
            reloc = TRUE;
        }
        if( reloc ) {
            switch( control->control_class ) {
#ifdef __OS2_PM__
            // brutal hack to get OS/2 and Windows dialogs to look the same.
            case GUI_EDIT:
                control->rect.height = charspace->y;
                break;
#endif
            case GUI_GROUPBOX:
                control->rect.width -= half.x;
                control->rect.height -= half.y;
                break;
            default :
                break;
            }
        }
        ++control;
    }
}

static void DlgSetSize( gui_window *parent, gui_create_info *dlg, int rows,
                        int cols, gui_coord *charuse )
{
    gui_rect            max_size;
    gui_rect            rect;
    gui_system_metrics  metrics;
    gui_coord           half;
    gui_ord             char_ui_adjust;

    GetHalfAndAdjust( charuse, &half, &char_ui_adjust );
    GUIGetSystemMetrics( &metrics );
    GUIGetScale( &max_size );
    dlg->rect.width = (cols+3) * charuse->x + metrics.dialog_top_left_size.x +
                      metrics.dialog_bottom_right_size.x;
    dlg->rect.height= rows * charuse->y + metrics.dialog_top_left_size.y +
                      metrics.dialog_bottom_right_size.y + char_ui_adjust;
    if( dlg->rect.width  > max_size.width ) {
        dlg->rect.width  = max_size.width;
    }
    if( dlg->rect.height > max_size.height ) {
        dlg->rect.height = max_size.height;
    }
    dlg->rect.x = max_size.x + ( max_size.width - dlg->rect.width ) / 2;
    dlg->rect.y = max_size.y + ( max_size.height - dlg->rect.height ) / 2;
    if( parent != NULL ) {
        GUIGetClientRect( parent, &rect );
        dlg->rect.x -= rect.x;
        dlg->rect.y -= rect.y;
    }
}

static void DlgOpen( gui_window *parent, char *name, int rows, int cols,
                     gui_control_info *ctl, int num_controls, GUICALLBACK *rtn,
                     void *extra, bool sys )
{
    gui_coord           charuse;
    gui_coord           charspace;
    gui_text_metrics    metrics;

    DlgControl.text = name;
    DlgControl.call_back = rtn;
    DlgControl.extra = extra;
    DlgControl.parent = parent;
    GUIGetDlgTextMetrics( &metrics );
    charspace.x = metrics.max.x;
    charspace.y = metrics.max.y;
#ifdef __OS2_PM__
    // brutal hack to get OS/2 and Windows dialogs to look the same.
    charuse.x = ( 5 * metrics.avg.x ) / 4;
    charuse.y = ( 3 * metrics.avg.y ) / 2;
#else
    charuse.x = metrics.avg.x;
    charuse.y = ( 7 * metrics.avg.y ) / 4;
#endif
    GUITruncToPixel( &charuse );
    DlgSetSize( parent, &DlgControl, rows, cols, &charuse );
    DlgSetCtlSizes( ctl, num_controls, &charuse, &charspace );
    if( sys || DlgModal ) {
        GUICreateSysModalDialog( &DlgControl, num_controls, ctl );
    } else {
        GUICreateDialog( &DlgControl, num_controls, ctl );
    }
}

void GUISetModalDlgs( bool modal )
{
    DlgModal = modal;
}

void GUIDlgOpen( char *name, int rows, int cols, gui_control_info *ctl,
                 int num_controls, GUICALLBACK *rtn, void *extra )
{
    DlgOpen( NULL, name, rows, cols, ctl, num_controls, rtn, extra, FALSE );
}

void GUIModalDlgOpen( gui_window *parent, char *name, int rows, int cols,
                      gui_control_info *ctl, int num_controls,
                      GUICALLBACK *rtn, void *extra )
{
    DlgOpen( parent, name, rows, cols, ctl, num_controls, rtn, extra, FALSE );
}

void GUISysModalDlgOpen( char *name, int rows, int cols,
                         gui_control_info *ctl, int num_controls,
                         GUICALLBACK *rtn, void *extra )
{
    DlgOpen( NULL, name, rows, cols, ctl, num_controls, rtn, extra, TRUE );
}
