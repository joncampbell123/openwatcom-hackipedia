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
* Description:  Implementation of include aliasing functions.
*
****************************************************************************/

#include "plusplus.h"

#include "memmgr.h"
#include "pcheader.h"
#include "ialias.h"

typedef struct ialias_list {
    union {
        struct ialias_list  *next;
        int                 total_len;
    };
    union {
        char                *real_name;
        int                 alias_name_len;
    };
    boolean is_lib;
    char    alias_name[1];
} *IALIASPTR;

static IALIASPTR    IAliasNames;

void IAliasInit( void )
/*********************/
{
    IAliasNames = NULL;
}

void IAliasFini( void )
/*********************/
{
    IALIASPTR   aliaslist;
    
    while( (aliaslist = IAliasNames) != NULL ) {
        IAliasNames = aliaslist->next;
        CMemFree( aliaslist );
    }
}

const char *IAliasLookup( const char *filename, boolean is_lib )
/*************************************************************/
{
    IALIASPTR   alias;
    
    for( alias = IAliasNames; alias != NULL; alias = alias->next ) {
        if( (alias->is_lib == is_lib) && !strcmp( filename, alias->alias_name ) ) {
            return( alias->real_name );
        }
    }
    return( filename );
}

void IAliasAdd( const char *alias_name, const char *real_name, boolean is_lib )
/****************************************************************************/
{
    size_t      alias_size, alias_len;
    IALIASPTR   alias, old_alias;
    IALIASPTR   *lnk;

    for( lnk = &IAliasNames; (old_alias = *lnk) != NULL; lnk = &old_alias->next ) {
        if( (old_alias->is_lib == is_lib) && !strcmp( alias_name, old_alias->alias_name ) ) {
            break;
        }
    }

    alias_len = strlen( alias_name );
    alias_size = sizeof( struct ialias_list ) + alias_len + strlen( real_name ) + 1;
    alias = CMemAlloc( alias_size );
    alias->next = NULL;
    alias->is_lib = is_lib;
    strcpy( alias->alias_name, alias_name );
    alias->real_name = alias->alias_name + alias_len + 1;
    strcpy( alias->real_name, real_name );

    if( old_alias ) {
        /* Replace old alias if it exists */
        alias->next = old_alias->next;
        CMemFree( old_alias );
    }
    *lnk = alias;
}

pch_status PCHReadIncAlias( void )
{
    size_t      alias_len, real_name_len, alias_size;
    IALIASPTR   alias;

    for( ; (alias_len = PCHReadUInt()) != 0; ) {
        real_name_len = PCHReadUInt();
        alias_size = sizeof( struct ialias_list ) + alias_len + real_name_len + 1;
        alias = CMemAlloc( alias_size );
        alias->next = IAliasNames;
        PCHRead( alias->alias_name, alias_len );
        alias->alias_name[alias_len] = '\0';
        alias->real_name = alias->alias_name + alias_len + 1;
        PCHRead( alias->real_name, real_name_len );
        alias->real_name[real_name_len] = '\0';
        alias->is_lib = PCHReadUInt();
        IAliasNames = alias;
    }

    return( PCHCB_OK );
}

pch_status PCHWriteIncAlias( void )
{
    size_t      alias_len, real_name_len;
    IALIASPTR   alias;
    
    for( alias = IAliasNames; alias != NULL; alias = alias->next ) {
        alias_len = strlen( alias->alias_name );
        real_name_len = strlen( alias->real_name );
        PCHWriteUInt( alias_len );
        PCHWriteUInt( real_name_len );
        PCHWrite( alias->alias_name, alias_len );
        PCHWrite( alias->real_name, real_name_len );
        PCHWriteUInt( alias->is_lib );
    }
    PCHWriteUInt( 0 );

    return( PCHCB_OK );
}

pch_status PCHInitIncAlias( boolean writing )
{
    return( PCHCB_OK );
}

pch_status PCHFiniIncAlias( boolean writing )
{
    return( PCHCB_OK );
}

pch_status PCHRelocIncAlias( char *block, size_t size )
{
    return( PCHCB_OK );
}

