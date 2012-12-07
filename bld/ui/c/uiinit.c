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


#include "uidef.h"
#ifdef __UNIX__
#include "uivirt.h"
#endif
#include "uiforce.h"
#include "stdui.h"

void intern DBCSCharacterMap( void );

bool global uistart( void )
/*************************/
{
    UIMemOpen();
    if( initbios() ) {
        DBCSCharacterMap();
        /* need for LUI and DUI apps to avoid divide by zero    */
        /* when no mouse is found                               */
        UIData->mouse_yscale = 1;
        UIData->mouse_xscale = 1;
        uiattrs();
        openbackground();
        forceevinit();
        return( TRUE );
    }
    return( FALSE );
}

bool global uiinit( int mouse )
/*****************************/
{
    register int initialized;

    initialized = uistart();

    if( initialized ) {
#ifdef __UNIX__
        _initmouse( mouse );
#else
        initmouse( mouse );
#endif
    }
    return( initialized );
}

void global uistop( void )
{
    forceevfini();
    closebackground();
    finibios();
}

void global uifini( void )
/************************/
{
#ifdef __UNIX__
    _finimouse();
#else
    finimouse();
#endif
    uistop();
    UIMemClose();
}


void global uiswap( void )
/************************/
{
    uiswapcursor();
    uiswapmouse();
}
