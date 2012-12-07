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


#include "dbgdefn.h"
#include <string.h>
#include <stddef.h>
#include "dbgtoggl.h"
#include "dbglit.h"
#include "dbginfo.h"
#include <rdos.h>
#include <stdui.h>

extern void         *ExtraAlloc( unsigned );
extern void         ExtraFree( void * );
extern int          GUIInitMouse( int );
extern void         GUIFiniMouse( void );
extern image_entry  *ImagePrimary(void);

static unsigned ScrnLines=25;
static unsigned ScrnColumns=80;

static char my_key = 0;
static char debug_key = 0;


void SetNumLines( int num )
{
    ScrnLines = num;
}

void SetNumColumns( int num )
{
    ScrnColumns=num;
}

void RingBell()
{
}

unsigned ConfigScreen( void )
{
    return( 0 );
}

void InitScreen( void )
{
    if( uistart() ) {
        GUIInitMouse( 1 );
    }
}

bool UsrScrnMode( void )
{
    return( FALSE );
}

bool DebugScreen( void )
{
    int handle;
    
    if( my_key == 0) {
        handle = RdosGetModuleHandle();
        my_key = RdosGetModuleFocusKey( handle );
    }
    
    if( my_key )
        RdosSetFocus( my_key );
        
    return( FALSE );
}

bool DebugScreenRecover()
{
    DebugScreen();
    return( TRUE );
}

bool UserScreen()
{
    int handle;
    image_entry *img;
        
    if( debug_key == 0 ) {
        img = ImagePrimary();
        if( img ) {
            handle = img->system_handle;
            debug_key = RdosGetModuleFocusKey( handle );
        }        
    }

    if( debug_key )
        RdosSetFocus( debug_key );

    return( FALSE );
}

void SaveMainWindowPos()
{
}

void FiniScreen( void )
{
    DebugScreen();
    GUIFiniMouse();
    uistop();
}

bool SysGUI()
{
    return( FALSE );
}

int mygetlasterr()
{
    return( 0 );
}

void PopErrBox( char *buff )
{
    RdosWriteString( buff );
}
