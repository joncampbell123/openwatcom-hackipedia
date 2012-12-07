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
* Description:  Interface to MDI window implementation.
*
****************************************************************************/


typedef struct {
    HWND        root;
    HWND        container;
    DWORD       reg_style;
    DWORD       max_style;
    UINT        data_off;
    char        *main_name;
    void        (*start_max_restore)( HWND );
    void        (*end_max_restore)( HWND );
    void        (*set_window_title)( HWND );
    void        (*set_style)( HWND, int );
    HANDLE      hinstance;
} mdi_info;

void MDIInit( mdi_info * );
void MDIInitMenu( void );
int  MDINewWindow( HWND hwnd );
void MDISetMainWindowTitle( char *fname );
void MDIClearMaximizedMenuConfig( void );
int  MDIIsMaximized( void );
int  MDIIsWndMaximized( HWND );
int  MDIUpdatedMenu( void );
void MDISetMaximized( int setting );
void MDITile( int is_horz );
void MDICascade( void );
int  MDIChildHandleMessage( HWND hwnd, UINT msg, UINT wparam, LONG lparam, LONG *lrc );
int  MDIHitClose( HWND hwnd, UINT msg, UINT wparam, LONG lparam );
int  MDIIsSysCommand( HWND hwnd, UINT msg, UINT wparam, LONG lparam );
void MDIResizeContainer( void );
void MDIContainerResized( void );
void MDISetOrigSize( HWND hwnd, RECT *rect );
