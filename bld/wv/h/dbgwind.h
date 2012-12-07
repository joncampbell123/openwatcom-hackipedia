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


#include "aui.h"
#include "dbglit.h"
#include "dbgmenu.h"
#include "dlgbutn.h"
#include "dbgicon.h"

#define WND_MENU_ALLOCATED      GUI_UTIL_2

enum {
    WND_PLAIN = WND_FIRST_UNUSED,
    WND_TABSTOP,
    WND_SELECTED,
    WND_STANDOUT,
    WND_HOTSPOT,
    WND_STANDOUT_TABSTOP,
};

typedef enum wnd_controls {
    COMBOBOX_CONTROL
} wnd_controls;

#define MAX_TAB         80

enum {
    #define pick( a,b,c,d,e,f ) b,
    #include "wndnames.h"
};

#define pick( a,b,c,d,e,f ) extern wnd_info d;
#include "wndnames.h"

#define pick( a,b,c,d,e,f ) extern WNDOPEN c;
#include "wndnames.h"

typedef enum macro_type {
    MACRO_POPUP_MENU,
    MACRO_MAIN_MENU,
    MACRO_COMMAND
} macro_type;

typedef struct wnd_macro {
        struct wnd_macro        *link;
        void                    *cmd;
        unsigned                key;
        wnd_class               class;
        int                     menu_item;
        gui_menu_struct         *menu;
        macro_type              type;
} wnd_macro;

#define pick( a,b,c,d,e,f ) extern wnd_metrics e;
#include "wndnames.h"

extern wnd_metrics *WndMetrics[];

extern void             WndInitScrnState(void);
extern void             WndNewProg(void);
extern void             WndStrt(void);
extern void             WndShow(void);
extern void             WndStop(void);
extern void             WndUser(void);
extern void             WndDebug(void);

extern void             WndSysStart(void);
extern void             WndSysEnd(bool );
extern void             WndRedraw(wnd_class );
extern void             WndCodeBrk(address ,bool );

extern void             WndDoInput(void);

extern wnd_macro        *WndMacroList;

extern char             *WndClipItem;

extern void             WndUpdate(update_list );

extern a_window *DbgWndCreate( char * text, wnd_info *info,
                               wnd_class class, void * extra,
                               gui_resource *icon );

extern a_window *DbgTitleWndCreate( char * text, wnd_info *wndinfo,
                                    wnd_class class, void * extra,
                                    gui_resource * icon,
                                    int title_size, bool vdrag );
