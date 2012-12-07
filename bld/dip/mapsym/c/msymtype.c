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
* Description:  Stuff dealing with type handles.
*
****************************************************************************/


#include <stddef.h>
#include "msym.h"


/* Note: Not much to do here. Could possibly try to guess data vs. code by
 * looking at segment names, but results would not necessarily be accurate.
 */

walk_result DIPENTRY DIPImpWalkTypeList( imp_image_handle *ii,
                imp_mod_handle im, IMP_TYPE_WKR *wk, imp_type_handle *it, void *d )
{
    ii = ii; im = im; wk = wk; it = it; d = d;
    return( WR_CONTINUE );
}

imp_mod_handle DIPENTRY DIPImpTypeMod( imp_image_handle *ii, imp_type_handle *it )
{
    ii = ii; it = it;
    return( MY_MOD_ID );
}

dip_status DIPENTRY DIPImpTypeInfo( imp_image_handle *ii,
                imp_type_handle *it, location_context *lc, type_info *ti )
{
    ii = ii; lc = lc;
    ti->modifier = TM_NONE;
    ti->size = 0;
    if( it->code ) {
        ti->kind = TK_CODE;
    } else {
        ti->kind = TK_DATA;
    }
    return( DS_OK );
}

dip_status DIPENTRY DIPImpTypeBase( imp_image_handle *ii,
                        imp_type_handle *it, imp_type_handle *base,
                        location_context *lc, location_list *ll )
{
    ii = ii;
    *base = *it;
    return( DS_FAIL );
}

dip_status DIPENTRY DIPImpTypeArrayInfo( imp_image_handle *ii,
                        imp_type_handle *array, location_context *lc,
                        array_info *ai, imp_type_handle *index )
{
    ii = ii; lc = lc;
    ai->low_bound = 0;
    ai->num_elts = 0;
    ai->stride = 0;
    ai->num_dims = 0;
    ai->column_major = 0;
    if( index != NULL ) *index = *array;
    return( DS_FAIL );
}

dip_status DIPENTRY DIPImpTypeProcInfo( imp_image_handle *ii,
                imp_type_handle *proc, imp_type_handle *parm, unsigned n )
{
    ii = ii; n = n;
    *parm = *proc;
    return( DS_FAIL );
}

dip_status DIPENTRY DIPImpTypePtrAddrSpace( imp_image_handle *ii,
                    imp_type_handle *it, location_context *lc, address *a )
{
    ii = ii; it = it; lc = lc; a = a;
    return( DS_FAIL );
}

dip_status DIPENTRY DIPImpTypeThunkAdjust( imp_image_handle *ii,
                        imp_type_handle *base, imp_type_handle *derived,
                        location_context *lc, address *addr )
{
    ii = ii; base = base; derived = derived; lc = lc; addr = addr;
    return( DS_FAIL );
}

int DIPENTRY DIPImpTypeCmp( imp_image_handle *ii, imp_type_handle *it1,
                                imp_type_handle *it2 )
{
    ii = ii;
    return( it1 - it2 );
}

unsigned DIPENTRY DIPImpTypeName( imp_image_handle *ii, imp_type_handle *it,
                unsigned num, symbol_type *tag, char *buff, unsigned max )
{
    ii = ii; it = it; num = num; tag = tag; buff = buff; max = max;
    return( 0 );
}

dip_status DIPENTRY DIPImpTypeAddRef( imp_image_handle *ii, imp_type_handle *it )
{
    ii = ii; it = it;
    return( DS_OK );
}

dip_status DIPENTRY DIPImpTypeRelease( imp_image_handle *ii, imp_type_handle *it )
{
    ii = ii; it = it;
    return( DS_OK );
}

dip_status DIPENTRY DIPImpTypeFreeAll( imp_image_handle *ii )
{
    ii = ii;
    return( DS_OK );
}
