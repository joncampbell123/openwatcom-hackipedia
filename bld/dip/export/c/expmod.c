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


#include "exp.h"
#include <string.h>

/*
        Stuff dealing with module handles
*/
walk_result     DIPENTRY DIPImpWalkModList( imp_image_handle *ii,
                        IMP_MOD_WKR *wk, void *d )
{
    return( wk( ii, MY_MOD_ID, d ) );
}

unsigned        DIPENTRY DIPImpModName( imp_image_handle *ii,
                        imp_mod_handle im, char *buff, unsigned max )
{

    im = im;
    if( max > 0 ) {
        --max;
        if( max > ii->len ) max = ii->len;
        memcpy( buff, ii->name, max );
        buff[ max ] = '\0';
    }
    return( ii->len );
}

char            *DIPENTRY DIPImpModSrcLang( imp_image_handle *ii, imp_mod_handle im )
{
    ii = ii; im = im;
    return( "c" );
}

dip_status      DIPENTRY DIPImpModInfo( imp_image_handle *ii,
                                imp_mod_handle im, handle_kind hk )
{
    ii = ii; im = im; hk = hk;
    return( DS_FAIL );
}

search_result   DIPENTRY DIPImpAddrMod( imp_image_handle *ii, address a,
                imp_mod_handle *im )
{
    ii = ii; a = a; im = im;
    if( FindAddrBlock( ii, a.mach ) != NULL ) {
        *im = MY_MOD_ID;
        return( SR_CLOSEST );
    }
    return( SR_NONE );
}

address         DIPENTRY DIPImpModAddr( imp_image_handle *ii,
                                imp_mod_handle im )
{
    address     addr;
    exp_block   *exp;

    im = im;
    for( exp = ii->addr; exp != NULL; exp = exp->next ) {
        if( exp->code ) {
            addr.mach = exp->start;
            DCAddrSection( &addr );
            return( addr );
        }
    }
    return( NilAddr );
}

dip_status      DIPENTRY DIPImpModDefault( imp_image_handle *ii,
                imp_mod_handle im, default_kind dk, type_info *ti )
{
     ii = ii; im = im; dk = dk; ti = ti;
     return( DS_FAIL );
}
