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
* Description:  DDE Spy master include.
*
****************************************************************************/

#include <windows.h>
#include "win1632.h"
#include <stdlib.h>
#define DDEMLDB
#include <ddeml.h>
#include <dde.h>
#include "mem.h"
#include "font.h"
#include "srchmsg.h"
#include "log.h"
#include "pushwin.h"
#include "alias.h"
#include "savelbox.h"
#include "toolbr.h"
#include "mark.h"
#ifndef NOUSE3D
 #include "ctl3dcvr.h"
#endif
#include "rcstr.gh"
#include "ldstr.h"
#include "hint.h"
#include "cbfilt.h"
#include "msgfilt.h"

#define RCSTR_MAX_LEN   500
#define DDE_HELP_FILE   "ddespy.hlp"
#define DDE_CHM_FILE    "ddespy.chm"
#define HELP_HELP_FILE  "winhelp.hlp"
#define TRACKING_CLASS  "WDDE_TRACKING_CLASS"

/* filter type constants */

#define FILTER_MESSAGE                  10
#define FILTER_CB                       11

/* menu constants */

#define DDEMENU_EXIT                    101
#define DDEMENU_SAVE_AS                 103
#define DDEMENU_LOG_FILE                104
#define DDEMENU_LOG_PAUSE               105
#define DDEMENU_LOG_CONFIG              106
#define DDEMENU_SAVE                    107
#define DDEMENU_TOOLBAR                 108
#define DDEMENU_FONT                    109
#define DDEMENU_HINTBAR                 102

#define DDEMENU_ABOUT                   130
#define DDEMENU_HELP_CONTENTS           131
#define DDEMENU_HELP_SRCH               132
#define DDEMENU_HELP_ON_HELP            133

#define DDEMENU_CLEAR                   141
#define DDEMENU_MARK                    142
#define DDEMENU_SCROLL                  143
#define DDEMENU_SCREEN_OUT              144

#define DDEMENU_TASK_ALIAS              150
#define DDEMENU_CONV_ALIAS              151
#define DDEMENU_HWND_ALIAS              152
#define DDEMENU_NO_ALIAS                153
#define DDEMENU_ALIAS_PURGE             154

/*
 * When future monitoring options are to be added their constants should be
 * the next avalable integer and DDE_MON_LAST must be updated.  The fact
 * that the constants are consecutive is used in accessing the Monitoring
 * data structure
 */

#define DDE_MON_FIRST                   DDEMENU_MON_POST
#define DDEMENU_MON_POST                110
#define DDEMENU_MON_SENT                111
#define DDEMENU_MON_STR                 112
#define DDEMENU_MON_CB                  113
#define DDEMENU_MON_ERR                 114
#define DDEMENU_MON_LNK                 115
#define DDEMENU_MON_CONV                116
#define DDE_MON_LAST                    DDEMENU_MON_CONV

#define DDEMENU_MSG_FILTER              117
#define DDEMENU_CB_FILTER               118

#define DDE_TRK_FIRST                   DDEMENU_TRK_STR
#define DDEMENU_TRK_STR                 120
#define DDEMENU_TRK_CONV                121
#define DDEMENU_TRK_LINK                122
#define DDEMENU_TRK_SERVER              123
#define DDE_TRK_LAST                    DDEMENU_TRK_SERVER
#define NO_TRK_WND                      ( DDE_TRK_LAST - DDE_TRK_FIRST + 1 )


/* tracking push window constants */

#define PUSH_STR_HDL                    201
#define PUSH_STR_CNT                    202
#define PUSH_STR_TEXT                   203

#define PUSH_CLIENT                     204
#define PUSH_SERVER                     205
#define PUSH_SERVICE                    206
#define PUSH_TOPIC                      207
#define PUSH_TYPE                       208
#define PUSH_ITEM                       209
#define PUSH_FORMAT                     210
#define PUSH_INST                       211

/*
 * Indices for accessing the Monitoring array
 * These macros are for coding convenience ONLY.  Not all accesses to
 * the Monitoring array use them.
 */
#define MON_POST_IND            ( DDEMENU_MON_POST - DDE_MON_FIRST )
#define MON_SENT_IND            ( DDEMENU_MON_SENT - DDE_MON_FIRST )
#define MON_STR_IND             ( DDEMENU_MON_STR - DDE_MON_FIRST )
#define MON_CB_IND              ( DDEMENU_MON_CB - DDE_MON_FIRST )
#define MON_ERR_IND             ( DDEMENU_MON_ERR - DDE_MON_FIRST )
#define MON_LNK_IND             ( DDEMENU_MON_LNK - DDE_MON_FIRST )
#define MON_CONV_IND            ( DDEMENU_MON_CONV - DDE_MON_FIRST )


#ifdef __NT__
 #define HWND_FMT_LEN           8
 #define CONV_FMT_LEN           8
 #define TASK_FMT_LEN           8
 #define FMT_ID_LEN             8
 #define HSZ_FMT_LEN            8
 #define WPARAM_FMT_LEN         8
#else
 #define HWND_FMT_LEN           4
 #define CONV_FMT_LEN           8
 #define TASK_FMT_LEN           4
 #define FMT_ID_LEN             4
 #define HSZ_FMT_LEN            4
 #define WPARAM_FMT_LEN         4
#endif

typedef struct lstinfo {
    HWND        box;
    WORD        hinthite;
    RECT        old_area;
    WORD        ypos;
} ListBoxInfo;

typedef struct ddewndinfo {
    ListBoxInfo         list;
    statwnd             *hintbar;
    BOOL                hint_tmp_hide;
    LONG                horz_extent;
    LONG                char_extent;
} DDEWndInfo;

typedef struct serverinfo {
    char        *server;
    char        *instname;
} ServerInfo;

typedef struct stringinfo {
    WORD        cnt;
    char        *str;
    HSZ         hsz;
} StringInfo;

/*
 * the LinkInfo structure is used to keep track of information
 * for both the link tracking window and the conversations tracking window
 * since the conversations tracking window uses a subset of the link
 * information and this allows us to keep common sorting routines
 */

typedef struct linkinfo {
    HCONV       client;
    HCONV       server;
    char        *service;
    char        *type;
    char        *topic;
    char        *item;
    char        *format;
} LinkInfo;

typedef struct wndconfiginfo {
    HWND                hwnd;
    BOOL                visible;        /* used for tracking windows only */
    WORD                state;
    WORD                xsize;
    WORD                ysize;
    int                 xpos;
    int                 ypos;
    int                 last_xpos;
    int                 last_ypos;
}WndConfigInfo;

typedef struct {
    DWORD       textid;
    WORD        id;
    WORD        size;
} TrackHeaderInfo;

typedef struct ddetrackinfo {
    WORD                type;
    WORD                cnt;
    WORD                sorttype;
    void                *data;
    ListBoxInfo         list;
    WndConfigInfo       *config;
    WORD                hdrcnt;
    TrackHeaderInfo     *hdrinfo;
    HWND                *hdr;
} DDETrackInfo;

typedef struct ddeconfiginfo {
    BOOL                scroll;
    BOOL                alias;
    BOOL                screen_out;
    BOOL                show_tb;
    BOOL                show_hints;
} DDEConfigInfo;

extern HANDLE           Instance;
extern HWND             DDEMainWnd;
extern DWORD            DDEInstId;
extern BOOL             Monitoring[ DDE_MON_LAST - DDE_MON_FIRST + 1 ];
extern WndConfigInfo    Tracking[ NO_TRK_WND ];
extern WndConfigInfo    MainWndConfig;
extern DDEConfigInfo    ConfigInfo;
extern AliasHdl         HwndAlias;
extern AliasHdl         ConvAlias;
extern AliasHdl         TaskAlias;
extern char             *AppName;

/* ddeproc.c */
BOOL __export FAR PASCAL DDEMainWndProc( HWND hwnd, UINT msg, WPARAM wparam,
                                    DWORD lparam );
void SetMainWndDefault( void );

/* ddemsg.c */
HDDEDATA __export FAR PASCAL DDEMsgProc( UINT type, UINT fmt, HCONV hconv,
        HSZ hsz1, HSZ hsz2, HDDEDATA hdata, DWORD data1, DWORD data2 );
char *HSZToString( HSZ hsz );
char *GetFmtStr( WORD fmt, char *buf );
void RecordMsg( char *buf );
void InitAliases( void );
void RefreshAliases( void );

/* ddebox.c */
void CreateListBox( HWND parent, ListBoxInfo *info );
void ResizeListBox( WORD width, WORD height, ListBoxInfo *info );

/* ddetrack.c */
void InitTrackWind( HWND hwnd );
BOOL CreateTrackWind( void );
BOOL __export FAR PASCAL DDETrackingWndProc( HWND hwnd, UINT msg, WPARAM wparam,
                                    DWORD lparam );
void DisplayTracking( WPARAM wparam );
void TrackStringMsg( MONHSZSTRUCT *info );
void TrackLinkMsg( MONLINKSTRUCT *info );
void TrackConvMsg( MONCONVSTRUCT *info );
void TrackServerMsg( MONCBSTRUCT *info );
void FiniTrackWnd( void );
void SetTrackWndDefault( void );
void SetTrackFont( void );

/* ddefltr.c */
BOOL __export FAR PASCAL FilterDlgProc( HWND hwnd, WORD msg,
                                          UINT wparam, LONG lparam );
BOOL DoFilter( WORD msg, WORD filter_type );
void SetFilter( char *msgfilter, char *cbfilter );
void GetFilter( char *msgfilter, char *cbfilter );

/* ddecfg.c */
void ReadConfig( void );
void SaveConfigFile( void );

/* ddemisc.c */
void LogHeader( int f );
void DumpHeader( FILE *fptr );
BOOL InitGblStrings( void );
void FiniRCStrings( void );

/* ddetool.c */
void GetFixedTBRect( HWND hwnd, RECT *rect );
void MakeDDEToolBar( HWND hwnd );
void DDEToolBarFini( void );
void ResizeTB( HWND owner );
BOOL ToggleTB( HWND parent );
void DDESetStickyState( WORD id, BOOL isdown );
