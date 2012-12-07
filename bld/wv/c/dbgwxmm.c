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
#include "dbgwind.h"
#include "dbgreg.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgitem.h"
#include "madcli.h"
#include <string.h>

extern void             RegFindData( mad_type_kind kind, mad_reg_set_data const **pdata );
extern a_window *WndMadRegOpen( mad_type_kind kind, wnd_class class, gui_resource *icon );
extern void MadRegChangeOptions( a_window *wnd );

void XMMChangeOptions()
{
    WndForAllClass( WND_XMM, MadRegChangeOptions );
}

extern WNDOPEN WndXMMOpen;
extern a_window *WndXMMOpen()
{
    const mad_reg_set_data      *rsd;

    RegFindData( MTK_XMM, &rsd );
    if( rsd == NULL ) return( NULL );
    return( WndMadRegOpen( MTK_XMM, WND_XMM, &XMMIcon ) );
}
