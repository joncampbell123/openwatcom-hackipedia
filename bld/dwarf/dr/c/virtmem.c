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
* Description:  Virtual memory support implementation.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include "drpriv.h"
/*
 * this virtual memory system has been set up in such a way that when
 * running under a 32-bit operating system with effective paging, the
 * vm read routines can be replaced with a simple pointer dereference,
 * and the vm alloc can be replaced with a call to dwralloc
*/

/* the following structures are for virtual memory allocation */

typedef struct {
    char        *mem;
    unsigned_8  sect:4;         // the type of section this page is in
    unsigned_8  refd:1;         // true if page not referenced
    unsigned_8  inmem:1;        // true if page in memory
} page_entry;

/* the page_entrys are referenced by an array of pointers, something like
PageTab --> 0 1 2 3 4 5 6 ...       <-- array elements are pointers
            | | | | | | |
            v v v v v v v
            1 1 1 1 1 1 1    <- array elements are page_entrys
            2 2 2 2 2 2 2   these arrays are referred to as "branches",
            3 3 3 3 3 3 3   and the individual elements are called "leaves".

in other words, it is effectively a dynamically allocated 2 dimensional array.
All virtual memory locations are split into 4K pages.

a virtual memory address is split into three parts:
offset                   leaf id   branch id
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   12 bits             | 4 bits|  16 bits                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 low order word                 high order word.
the leaf id chooses which element of the branch is the correct seg_table.
Note that the branch id can only reach about 16200 due to only being able
to allocate < 64k hunks of memory.
*/

// this structure is used for picking the high order word off a long

#ifdef __BIG_ENDIAN__
typedef struct wordpick {
    unsigned_16 high;
    unsigned_16 low;
} wordpick;
#else
typedef struct wordpick {
    unsigned_16 low;
    unsigned_16 high;
} wordpick;
#endif

// this is used instead of the virt_mem type inside this module, since it is
// desirable to be able to get the high order word without having to do a
// 16-bit shift. MAKE SURE THAT VIRT_MEM IS THE SAME SIZE AS THIS STRUCTURE!
typedef union {
    unsigned_32     l;
    wordpick        w;
} virt_struct;

#define OFFSET_SHIFT      12
#define MAX_NODE_SIZE     (1U << OFFSET_SHIFT)
#define MAX_LEAFS         16            // maximum # of leafs per branch
#define SEG_LIMIT         16200         // maximum # of 4K pages.

/* find the node for MEM_ADDR or FILE_ADDR */
#define NODE( stg )        (&PageTab[ stg.w.high ][ stg.w.low >> OFFSET_SHIFT ])

#define OFFSET_MASK        (MAX_NODE_SIZE - 1)
#define NODE_OFF( stg )    ( stg.w.low & OFFSET_MASK )

static page_entry       **PageTab   = NULL;
static unsigned_16      NumBranches = 15;
// start with branch # 1 so an address of zero can be illegal.
static unsigned_16      CurrBranch = 1;
static unsigned_16      NextLeaf = 0;   // next leaf # to be allocated.
static unsigned_16      SwapBranch = 1; // next branch # to be swapped.
static unsigned_16      SwapLeaf = 0;   // next leaf # to be swapped
static bool             DontSwap = TRUE; // TRUE if we can't swap now.
static unsigned_16      PageCount;

extern void DWRVMInit( void )
/***************************/
// Allocate space for the branch pointers.
{
    PageTab = DWRALLOC( NumBranches * sizeof( page_entry * ) );
    memset( PageTab, 0, NumBranches * sizeof( page_entry * ) );
    PageTab[1] = DWRALLOC( sizeof( page_entry ) * MAX_LEAFS );
    memset( PageTab[1], 0, sizeof( page_entry ) * MAX_LEAFS );
    DontSwap = FALSE;
    PageCount = 0;
}

extern void DWRVMReset( void )
/****************************/
// Reset VM state without having to destroy/init
// NOTE: This will ensure that allocations start from the lowest VM
// address again, without actually freeing the memory that we have
// allocated. Any existing memory will be reused.
{
    CurrBranch = 1;
    NextLeaf = 0;
}

static void GetMoreBranches( void )
/*********************************/
// make a larger array to hold branch pointers in.
{
    page_entry      **branches;
    unsigned        alloc_size;

    alloc_size = NumBranches * sizeof( page_entry * );
    NumBranches = NumBranches * 2;   // double the # of pointers.
    if( NumBranches > SEG_LIMIT ) {
        DWREXCEPT( DREXCEP_OUT_OF_VM );
    }
    branches = DWRALLOC( alloc_size * 2 );
    memcpy( branches, PageTab, alloc_size );
    memset( (char *)branches + alloc_size, 0, alloc_size ); // null pointers
    DWRFREE( PageTab );
    PageTab = branches;
}

static virt_struct GetPage( dr_section sect )
/*******************************************/
{
    page_entry      *entry;
    unsigned        alloc_size;
    virt_struct     vmem;

    if( NextLeaf >= MAX_LEAFS ) {
        DontSwap = TRUE;
        NextLeaf = 0;
        CurrBranch++;
        if( CurrBranch >= NumBranches ) {
            GetMoreBranches();
        }
        alloc_size = sizeof( page_entry ) * MAX_LEAFS;
        entry = DWRALLOC( alloc_size );
        PageTab[ CurrBranch ] = entry;
        memset( entry, 0, alloc_size ); //set all flags FALSE.
        DontSwap = FALSE;
    } else {
        entry = &PageTab[ CurrBranch ][ NextLeaf ];
    }
    entry->sect = sect;
    vmem.w.high = CurrBranch;
    vmem.w.low = NextLeaf << OFFSET_SHIFT;
    NextLeaf++;
    return( vmem );
}

extern dr_handle DWRVMAlloc( unsigned long size, int sect )
/*********************************************************/
{
    virt_struct ret;

    ret.l = 0;          // assume 0 not a valid vm address
    if( size > 0 ) {
        ret = GetPage( sect );
        while( size > MAX_NODE_SIZE ) {
            size -= MAX_NODE_SIZE;
            GetPage( sect );
        }
    }
    return( ret.l );
}

extern void DWRVMFree( dr_handle hdl )
/************************************/
{
    hdl = hdl;
}

extern int DRSwap( void )
/***********************/
// this uses the second-chance cyclic algorithm for page replacement.
// NOTE: this tends to degenerate into FIFO under very tight memory
// requirements, which is rather bad for the current usage.  Any better ideas?
{
    unsigned_16         startbranch;
    unsigned_16         startleaf;
    page_entry          *entry;
    bool                passtwo;

    if( DontSwap ) return( FALSE );
    passtwo = FALSE;
    startbranch = SwapBranch;
    startleaf = SwapLeaf;
    for( ;; ) {
        SwapLeaf++;
        if( SwapLeaf >= MAX_LEAFS ) {
            SwapLeaf = 0;
            SwapBranch++;
            if( SwapBranch > CurrBranch ) {
                SwapBranch = 1;
            }
        }
        entry = &PageTab[SwapBranch][SwapLeaf];
        if( entry != NULL ) {
            if( entry->refd ) {
                entry->refd = 0;
            } else if( entry->inmem ) {
                DWRFREE( entry->mem );
                PageCount--;
                entry->inmem = 0;
                return( TRUE );
            }
        }
        if( SwapLeaf == startleaf && SwapBranch == startbranch ) {
            if( passtwo ) break;        // nothing to swap;
            passtwo = TRUE;
        }
    }
    return( FALSE );
}

extern void DWRVMDestroy( void )
/******************************/
/* this frees all virtual memory */
{
    unsigned        branch;
    unsigned        leaf;
    page_entry      *entry;

    DontSwap = TRUE;
    if( PageTab == NULL ) return;
    for( branch = 1; branch < NumBranches; branch++ ) {
        entry = PageTab[ branch ];
        if( entry != NULL ) {
            for( leaf = 0; leaf < MAX_LEAFS; leaf++ ) {
                if( entry->inmem ) {
                    DWRFREE( entry->mem );
                }
                entry++;
            }
            DWRFREE( PageTab[branch] );
        }
    }
    DWRFREE( PageTab );
}

static void ReadPage( page_entry * node, virt_struct vm )
/*******************************************************/
/* read a page in from the dwarf file */
{
    unsigned long size;
    unsigned long base;
    unsigned long offset;
    dr_section    sect;

    sect = node->sect;
    size = DWRCurrNode->sections[sect].size;
    base = DWRCurrNode->sections[sect].base;
    offset = (vm.l - base) & ~((unsigned long)OFFSET_MASK);
    size -= offset;
    if( size > MAX_NODE_SIZE ) {
        size = MAX_NODE_SIZE;
    }
    node->mem = DWRALLOC( size );
    node->inmem = TRUE;
    ++PageCount;
    DWRSEEK( DWRCurrNode->file, sect, offset );
    DWRREAD( DWRCurrNode->file, sect, node->mem, size );
}

extern void DWRVMSwap( dr_handle base, unsigned_32 size, int *ret )
/*****************************************************************/
// Swap out base for length size
// If memory was freed set *ret
{
    volatile virt_struct         vm; // cg bug workaround
    page_entry          *entry;
    int                 ret_val;

    vm.l = base;
    ret_val = FALSE;
    if( size > 0 ) {
        for( ;; ) {
            entry = NODE( vm );
            entry->refd = 0;
            if( entry->inmem ) {
                --PageCount;
                DWRFREE( entry->mem );
                entry->inmem = 0;
                ret_val = TRUE;
            }
            if( size <= MAX_NODE_SIZE ) break;
            size -= MAX_NODE_SIZE;
            vm.l += MAX_NODE_SIZE;
        }
    }
    if( ret_val == TRUE ) {
        *ret = ret_val;
    }
}

extern int DWRVMSectDone( dr_handle base, unsigned_32 size )
/**********************************************************/
{
    int ret;

    DWRVMSwap( base, size, &ret );
    return( ret );
}

#if 1 // ITB

#define ACCESSPAGE( __nd, __vm )    \
    __nd = NODE( (__vm) );          \
    __nd->refd = 1;                 \
    if( !__nd->inmem ) {            \
        ReadPage( (__nd), (__vm) ); \
    }

#else

static page_entry * AccessPage( virt_struct vm )
/**********************************************/
{
    page_entry  *node;

    node = NODE( vm );
    node->refd = TRUE;
    if( !node->inmem ) {
        ReadPage( node, vm );
    }
    return node;
}

#endif

extern void DWRVMRead( dr_handle hdl, void * info, unsigned len )
/***************************************************************/
/* go through the virtual memory nodes, reading data */
{
    unsigned    end_off;
    unsigned    off;
    page_entry  *node;
    unsigned    amt;
    virt_struct vm;

    vm.l = hdl;
    off = NODE_OFF( vm );
    for( ;; ) {
        //node = AccessPage( vm );
        ACCESSPAGE( node, vm );                     // ITB
        end_off = (unsigned long)off + len;
        if( end_off <= MAX_NODE_SIZE ) break;
        amt = MAX_NODE_SIZE - off;
        memcpy( info, node->mem + off, amt );
        len -= amt;
        vm.l += amt;
        info = (char *)info + amt;
        off = 0;
    }
    memcpy( info, node->mem + off, len );
}

extern unsigned_8 DWRVMReadByte( dr_handle hdl )
/**********************************************/
{
    page_entry  *node;
    virt_struct vm;

    vm.l = hdl;
    //node = AccessPage( vm );
    ACCESSPAGE( node, vm );                         // ITB
    return( *(node->mem + NODE_OFF(vm)) );
}

static unsigned_32 ReadLEB128( dr_handle *vmptr, bool issigned )
/**************************************************************/
/* read and advance the vm pointer */
{
    virt_struct vm;
    page_entry  *node;
    unsigned_16 off;
    unsigned_32 result;
    unsigned_8  inbyte;
    unsigned    shift;

    shift = 0;
    vm.l = *vmptr;
    //node = AccessPage( vm );
    ACCESSPAGE( node, vm );                         // ITB
    off = NODE_OFF( vm );
    vm.l++;
    inbyte = *(node->mem + off);
    result = inbyte & 0x7F;
    while( inbyte & 0x80 ) {
        off++;
        if( off == MAX_NODE_SIZE ) {
            off = 0;
            //node = AccessPage( vm );
            ACCESSPAGE( node, vm );                 // ITB
        }
        shift += 7;
        inbyte = *(node->mem + off);
        result |= (unsigned_32)(inbyte & 0x7F) << shift;
        vm.l++;
    }
    *vmptr = vm.l;
    if( issigned ) {
        if( inbyte & 0x40 ) {   // we have to sign extend
            result |= - ((signed_32)(1 << (shift + 7)));
        }
    }
    return( result );
}

extern signed_32 DWRVMReadSLEB128( dr_handle *vmptr )
/***************************************************/
{
    return( (signed_32) ReadLEB128( vmptr, TRUE ) );
}

extern unsigned_32 DWRVMReadULEB128( dr_handle *vmptr )
/*****************************************************/
{
    page_entry  *node;
    char *      walk;
    unsigned    off;
    virt_struct vm;
    unsigned_32 result = 0;
    unsigned    shift = 0;
    char        b;

    vm.l = *vmptr;
    //node = AccessPage( vm );
    ACCESSPAGE( node, vm );                         // ITB
    off = NODE_OFF(vm);
    if( off <= MAX_NODE_SIZE - 5 ) {  // we can read whole uleb from buffer
        walk = node->mem + off;
        while( 1 ) {
            b = *walk++;
            result |= ( b & 0x7f ) << shift;
            if( ( b & 0x80 ) == 0 ) break;
            shift += 7;
        }
        *vmptr += walk - node->mem - off;
    } else {
        result = ReadLEB128( vmptr, FALSE );
    }

    return( result );
}

extern void DWRVMSkipLEB128( dr_handle *hdl )
/*******************************************/
// just advance the vm pointer past the leb128 (works on both signed & unsigned)
{
    page_entry  *node;
    unsigned_16 off;
    unsigned_8  inbyte;
    virt_struct vm;
    char        *walk;
    char        b;

    vm.l = *hdl;
    //node = AccessPage( vm );
    ACCESSPAGE( node, vm );                         // ITB

    off = NODE_OFF( vm );
    if( off <= MAX_NODE_SIZE - 5 ) {  // we can read whole leb from buffer
        walk = node->mem + off;
        while( 1 ) {
            b = *walk++;
            if( (b & 0x80) == 0 ) break;
        }
        vm.l += walk - node->mem - off;
    } else {
        vm.l++;
        inbyte = *(node->mem + off);
        while( inbyte & 0x80 ) {
            off++;
            if( off == MAX_NODE_SIZE ) {
                off = 0;
                //node = AccessPage( vm );
                ACCESSPAGE( node, vm );             // ITB
            }
            inbyte = *(node->mem + off);
            vm.l++;
        }
    }
    *hdl = vm.l;
}

extern unsigned_16 DWRVMReadWord( dr_handle hdl )
/***********************************************/
{
    page_entry  *node;
    char        *target;
    unsigned_16 off;
    virt_struct vm;

    vm.l = hdl;
    //node = AccessPage( vm );
    ACCESSPAGE( node, vm );                         // ITB

    off = NODE_OFF(vm);
    target = node->mem + off;
    if( off != MAX_NODE_SIZE - 1 ) {  // we can read both bytes now.
        // must not swap bytes in source buffer!
        off = *((unsigned_16 *)target);
    } else {
        off = *target;
        vm.l++;
        //node = AccessPage( vm );
        ACCESSPAGE( node, vm );                     // ITB
        off |= ((unsigned_16)*node->mem) << 8;
    }
    if( DWRCurrNode->byte_swap ) {
        SWAP_16( off );
    }
    return( off );
}

extern unsigned_32 DWRVMReadDWord( dr_handle hdl )
/************************************************/
{
    page_entry  *node;
    char        *target;
    unsigned    off;
    unsigned_32 result;
    virt_struct vm;

    vm.l = hdl;
    //node = AccessPage( vm );
    ACCESSPAGE( node, vm );                         // ITB
    off = NODE_OFF(vm);
    target = node->mem + off;
    if( off <= MAX_NODE_SIZE - 4 ) {  // we can read both bytes now.
        // must not swap bytes in source buffer!
        result = *((unsigned_32 *)target);
    } else {
        off = MAX_NODE_SIZE - off;
        memcpy( &result, target, off );
        vm.l += 4;
        //node = AccessPage( vm );
        ACCESSPAGE( node, vm );                     // ITB
        memcpy( (char *)&result + off, node->mem, 4 - off );
    }
    if( DWRCurrNode->byte_swap ) {
        SWAP_32( result );
    }
    return( result );
}

extern unsigned DWRStrLen( dr_handle hdl )
/****************************************/
{
    unsigned    off;
    unsigned    start_off;
    page_entry  *node;
    virt_struct vm;
    int         length = 0;

    vm.l = hdl;
    start_off = NODE_OFF( vm );

    off = start_off;

    for( ;; ) {
        //node = AccessPage( vm );
        ACCESSPAGE( node, vm );                     // ITB
        while( off < MAX_NODE_SIZE ) {
             if( node->mem[ off++ ] == '\0' ) {
                 goto end;
             }
        }
        length += off - start_off;

        vm.l += MAX_NODE_SIZE - start_off;
        off = 0;
        start_off = 0;
    }

    end:
    length += off - start_off;
    return( length - 1 );   // remove '\0' terminator's contrib
}

extern void DWRGetString( char * buf, dr_handle * hdlp )
/******************************************************/
{
    unsigned    off;
    page_entry  *node;
    virt_struct vm;
    char        *wlkBuf = buf;

    vm.l = *hdlp;
    off = NODE_OFF( vm );

    for( ;; ) {
        //node = AccessPage( vm );
        ACCESSPAGE( node, vm );                     // ITB
        while( off < MAX_NODE_SIZE ) {
            *wlkBuf = node->mem[ off++ ];
            vm.l++;
            if( *wlkBuf++ == '\0' ) goto end;
        }
        off = 0;
    }

    end:
    *hdlp = vm.l;
}

extern unsigned DWRGetStrBuff( dr_handle drstr, char *buf, unsigned max )
/***********************************************************************/
{
    unsigned    off;
    page_entry  *node;
    virt_struct vm;
    char        curr;
    unsigned    len;

    vm.l = drstr;
    off = NODE_OFF( vm );
    len = 0;
    for( ;; ) {
        //node = AccessPage( vm );
        ACCESSPAGE( node, vm );                     // ITB
        while( off < MAX_NODE_SIZE ) {
            curr = node->mem[ off++ ];
            if( len < max ) {
               *buf++ = curr;
            }
            ++len;
            if( curr == '\0' ) goto end;
            vm.l++;
        }
        off = 0;
    }

    end:
    return( len );
}
