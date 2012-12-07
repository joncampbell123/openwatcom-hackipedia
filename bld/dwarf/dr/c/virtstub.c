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
* Description:  Stubbed virtual memory support functions (no VM support).
*
****************************************************************************/


#include <string.h>

#include "drpriv.h"

typedef struct alloc_struct {
    struct alloc_struct *next;
    char                data[ 1 ];
} alloc_struct;

static alloc_struct *AllocHead;       /* head of list of allocated chunks */

extern void DWRVMInit( void )
/***************************/
{
    /* no action */
    AllocHead = NULL;
}

extern void DWRVMReset( void )
/***************************/
{
    /* nothing to do here */
}

extern dr_handle DWRVMAlloc( unsigned long len, int sect )
/********************************************************/
{
    alloc_struct * nChunk;

    if( len == 0 ) {
        return( 0 );
    }

    nChunk = (alloc_struct *) DWRALLOC( len - 1 + sizeof( alloc_struct ) );
    if( nChunk == NULL ) {
        DWREXCEPT( DREXCEP_OUT_OF_MMEM );
        return( 0 );
    }

    nChunk->next = AllocHead;
    AllocHead = nChunk;

    DWRSEEK( DWRCurrNode->file, sect, 0 );
    DWRREAD( DWRCurrNode->file, sect, nChunk->data, len );

    return( (dr_handle) nChunk->data );
}

extern int DWRVMSectDone( dr_handle base, unsigned_32 size )
/**********************************************************/
{
    alloc_struct    *walk;
    alloc_struct    **lnk;
    int             ret;

    lnk = &AllocHead;
    ret = FALSE;
    while( (walk = *lnk) != NULL ) {
        if( (dr_handle)walk->data == base ) {
            *lnk = walk->next;
            DWRFREE( walk );
            ret = TRUE;
            break;
        }
        lnk = &walk->next;
    }
    return( ret );
}


extern void DWRVMDestroy( void )
/******************************/
{
    alloc_struct    *walk;
    alloc_struct    *prev;

    for( walk = AllocHead; walk != NULL; ) {
        prev = walk;
        walk = walk->next;
        DWRFREE( prev );
    }
    AllocHead = NULL;
}

extern int DRSwap( void )
/***********************/
{
    // swap requests are ignored.

    return( FALSE );
}


extern dr_handle ReadLEB128( dr_handle *vmptr, bool issigned )
/************************************************************/
// works for signed or unsigned
{
    char            *buf = (char *) *vmptr;
    long            result = 0;
    unsigned        shift = 0;
    char            b;

    for( ;; ) {
        b = *buf++;
        result |= ( b & 0x7f ) << shift;
        if( ( b & 0x80 ) == 0 ) break;
        shift += 7;
    }

    *vmptr = (dr_handle) buf;
    if( issigned && (b & 0x40) ) {      // we have to sign extend
        result |= - ((long)(1 << (shift + 7)));
    }

    return( result );
}


extern unsigned DWRStrLen( dr_handle hdl )
/****************************************/
{
    return( strlen( (const char *) hdl ) );
}

extern void DWRGetString( char * buf, dr_handle * hdlp )
/******************************************************/
{
    uint len;

    len = DWRStrLen( (*hdlp) ) + 1;
    memcpy( buf, (const char *)(*hdlp), len );
    *hdlp = (dr_handle) ((const char *) *hdlp + len );
}

extern unsigned DWRGetStrBuff( dr_handle drstr, char *buf, unsigned max )
/***********************************************************************/
{
    unsigned    len;
    char        *curr;

    curr = (char *)drstr;
    len = 0;
    for( ;; ) {
        if( len < max ) {
           *buf++ = *curr;
        }
        ++len;
        if( *curr == '\0' ) break;
        ++curr;
    }
    return( len );
}

extern unsigned_16 DWRVMReadWord( dr_handle hdl )
/***********************************************/
{
    unsigned_16     word = *((unsigned_16 _WCUNALIGNED *)(hdl));

    if( DWRCurrNode->byte_swap ) {
        SWAP_16( word );
    }
    return( word );
}

extern unsigned_32 DWRVMReadDWord( dr_handle hdl )
/************************************************/
{    unsigned_32    dword = *((unsigned_32 _WCUNALIGNED *)(hdl));
    if( DWRCurrNode->byte_swap ) {
        SWAP_32( dword );
    }
    return( dword );
}
