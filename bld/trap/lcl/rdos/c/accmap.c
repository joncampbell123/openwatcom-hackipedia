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
* Description:  Routines to keep track of loaded modules and address maps.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdrdos.h"
#include "debug.h"

unsigned ReqMap_addr( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;
    struct TDebug       *obj;
	struct TDebugModule *mod = 0;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    obj = GetCurrentDebug();

	if (obj) {
        mod = LockModule( obj, acc->handle );
        if( mod ) {
            if( acc->in_addr.segment == MAP_FLAT_CODE_SELECTOR || acc->in_addr.segment == MAP_FLAT_DATA_SELECTOR ) {
                if( acc->in_addr.segment == MAP_FLAT_CODE_SELECTOR) {
                    ret->out_addr.segment = 0x1B3;
                    ret->out_addr.offset = mod->ImageBase + mod->ObjectRva + acc->in_addr.offset;
                }

                if( acc->in_addr.segment == MAP_FLAT_DATA_SELECTOR) {
                    ret->out_addr.segment = 0x1BB;
                    ret->out_addr.offset = mod->ImageBase + mod->ObjectRva + acc->in_addr.offset;
                }

                ret->lo_bound = 0;
                ret->hi_bound = mod->ImageSize - 1;            
            }
        }
        UnlockModule( obj );
    }        

    if( mod )
        return( sizeof( *ret ) );
    else
        return( 0 );
}

/*
 * AccGetLibName - get lib name of current module
 */
unsigned ReqGet_lib_name( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    struct TDebug       *obj;
    struct TDebugModule *mod;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    name = GetOutPtr( sizeof( *ret ) );

    ret->handle = 0;

    obj = GetCurrentDebug();

	if (obj) {
		ret->handle = GetNextModule( obj, acc->handle );

        if( ret->handle ) {
            mod = LockModule( obj, ret->handle );
            if( mod )
                strcpy( name, mod->ModuleName );
            else
                name[0] = 0;
            UnlockModule( obj );
        }
	}

	if( mod )
        return( sizeof( *ret ) + strlen( name ) + 1 );
    else
        return( sizeof( *ret ) );
}
