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
* Description:  C compiler memory management routines.
*
****************************************************************************/


#include "cvars.h"
#include "cgdefs.h"
#define BY_CLI
#include "feprotos.h"

/*
 * RISC platforms are likely to take a big hit for misaligned accesses.
 * Memory blocks are ordinarily aligned to sizeof( unsigned ) which is
 * OK for many platforms, but notably on Alpha we need 8-byte alignment
 * to make sure doubles won't get us in trouble.
 * NB: MEM_ALIGN must be at least int-sized
 */

#ifdef __AXP__
    #define MEM_ALIGN   sizeof( double )
#else
    #define MEM_ALIGN   sizeof( int )
#endif

typedef struct  mem_block {
        size_t              len;    /* length of stg */
        struct mem_block    *prev;  /* pointer to previous free memory block */
        struct mem_block    *next;  /* pointer to next     free memory block */
} MCB;

typedef struct mem_blk {
    struct mem_blk  *next;
    char            *ptr;   // old perm pointer
    long            size;   // old perm size
#ifdef __AXP__
    unsigned        pad;    // padding to get quadword aligned size
#endif
} mem_blk;

/* Size of permanent area. Needs to be reasonably big to satisfy
 * large allocation requests.
 */
#define MAX_PERM_SIZE   0xffff0         /* was 0xfff0 */

/* Mask to get real allocation size */
#define SIZE_MASK       ~1u             /* was 0xfffe */

/*  variables used:
 *      char *PermArea;         pointer to start of permanent area
 *      char *PermPtr;          next free byte in permanent area
 *      unsigned PermSize;      total size of permanent area
 *      unsigned PermAvail;     number of bytes available in permanent area
 *
 *  Permanent memory is allocated from low to high.
 *  Memory allocated using CPermAlloc is never given back.
 *  Temporary allocations are made at the high end of permanent memory.
*/

static  char        *PermPtr;   /* next free byte in PermArea */
static  size_t      PermSize;   /* total size of permanent memory block */
static  size_t      PermAvail;  /* # of bytes available in PermArea */

static  MCB         CFreeList;
static  mem_blk     *Blks;

// local functions
static void Ccoalesce( MCB *p1 );

static void InitPermArea( void )
{
    Blks = NULL;
    PermAvail = 0;
    PermPtr = NULL;
    PermSize = 0;
}

static void FiniPermArea( void )
{
    mem_blk *curr, *next;
    curr = Blks;
    while( curr != NULL ){
        next = curr->next;
        free( curr );
        curr = next;
    }
    Blks = NULL;
}

static void AllocPermArea( void )
{
    char    *perm_area;

    PermSize = MAX_PERM_SIZE;
    perm_area = NULL;
    for( ;PermSize; ) {                                     /* 05-apr-91 */
        mem_blk *blk;
        blk = calloc( 1, sizeof( mem_blk )+ PermSize + sizeof( int ) );
        if( blk != NULL ) {
            blk->next = Blks;
            blk->ptr = PermPtr;
            blk->size = PermSize;
            Blks = blk;
            perm_area = (char*)blk + sizeof( mem_blk );
            *(int *)(perm_area + PermSize) = -1;     /* null length tag */
            break;
        }
        if( PermSize < 32 )  PermSize = 32;
        PermSize -= 32;
    }
    PermPtr = perm_area;
    PermAvail = PermSize;
}


void CMemInit( void )
{
    InitPermArea();
    CFreeList.len = 0;
    CFreeList.next = &CFreeList;
    CFreeList.prev = &CFreeList;
}

void CMemFini( void )
{
    CFreeList.len = 0;
    CFreeList.next = &CFreeList;
    CFreeList.prev = &CFreeList;
    FiniPermArea();
}

static void *CFastAlloc( unsigned size )
/**************************************/
{
    size_t      amount;
    MCB         *p1;
    MCB         *pnext;
    MCB         *pprev;

    amount = (size + MEM_ALIGN + MEM_ALIGN - 1) & - MEM_ALIGN;
    if( amount < sizeof( MCB ) )  amount = sizeof( MCB );

/*      search free list before getting memory from PermArea */

    for( p1 = CFreeList.prev; p1 != &CFreeList; p1 = p1->prev ) {
        Ccoalesce( p1 );
        if( p1->len >= amount ) {
            if( p1->len - amount > sizeof( MCB ) ) {
                /* block is big enough to split it */
                p1->len -= amount;
                p1 = (MCB *)( (char *)p1 + p1->len );
                p1->len = amount;
            } else {
                /* remove block from free list */
                pnext = p1->next;
                pprev = p1->prev;
                pprev->next = pnext;
                pnext->prev = pprev;
            }
            p1->len |= 1;           /* indicate block allocated */
            return( (char *)p1 + MEM_ALIGN );
        }
    }
    if( amount > PermAvail ) return( NULL ); 
    PermAvail -= amount;
    p1 = (MCB *) (PermPtr + PermAvail);
    p1->len = amount | 1;
    return( (char *)p1 + MEM_ALIGN );
}


static void Ccoalesce( MCB *p1 )
{
    MCB *p2;
    MCB *pnext;
    MCB *pprev;

    for( ;; ) {
        p2 = (MCB *)( (char *)p1 + p1->len );
        if( p2->len & 1 )  break;   /* quit if next block not free */
        /* coalesce p1 and p2 and remove p2 from free list */
        p1->len += p2->len;
        pnext = p2->next;
        pprev = p2->prev;
        pprev->next = pnext;
        pnext->prev = pprev;
    }
}


void *CMemAlloc( unsigned size )
/******************************/
{
    void    *p;

    p = CFastAlloc( size );
    if( p == NULL ) {
        AllocPermArea();            /* allocate another permanent area */
        p = CFastAlloc( size );
        if( p == NULL ) {
            CErr1( ERR_OUT_OF_MEMORY );
            CSuicide();
        }
    }
    /* make sure pointer is properly aligned */
    assert( ((unsigned)p & (MEM_ALIGN - 1)) == 0 );

    return( memset( p, 0, size ) );
}


void *CMemRealloc( void *loc, unsigned size )
/*******************************************/
{
    void            *p;
    MCB             *p1;
    size_t          len;

    if( loc == NULL )
        return( CMemAlloc( size ) );

    p = loc;
    p1 = (MCB *) ( (char *)loc - MEM_ALIGN );
    len = (p1->len & SIZE_MASK) - MEM_ALIGN;
    if( size > len ) {
        p = CMemAlloc( size );
        memcpy( p, loc, len );
        CMemFree( loc );
    } /* else the current block is big enough -- nothing to do
         (very lazy realloc) */
    return( p );
}

enum cmem_kind {
    CMEM_PERM,
    CMEM_MEM,
    CMEM_NONE,
};

static enum cmem_kind CMemKind( void *loc )
{
    char            *ptr;
    size_t          size;
    mem_blk         *blk;

    ptr  = PermPtr;
    size = PermSize;
    blk  = Blks;
    while( blk != NULL ) {
        if( (mem_blk *)loc > blk ) {
            if( (char *)loc < ptr ) {
                return( CMEM_PERM );
            }
            if( ((mem_blk *)loc < blk) + sizeof( mem_blk ) + size ) {
                return( CMEM_MEM );
            }
        }
        ptr = blk->ptr;
        size = blk->size;
        blk = blk->next;
    }
    return( CMEM_NONE );
}


void CMemFree( void *loc )
/************************/
{
    size_t      len;
    MCB         *p1;
    MCB         *pprev;
    MCB         *pnext;

    if( loc == NULL ) { //Should try and get rid of these error cases
        return;
    }
    if( ((char *) loc >= PCH_Start)  &&  ((char *) loc < PCH_End) ) {
        return;  // 29-dec-93
    }
    switch( CMemKind( loc ) ) {
    case CMEM_PERM:
        return;
    case CMEM_MEM:
        p1 = (MCB *) ( (char *)loc - MEM_ALIGN );
        len = p1->len;
        len &= SIZE_MASK;
        if( (char *)p1 == PermPtr + PermAvail ) {
            PermAvail += len;
            if( CFreeList.next != &CFreeList ) {
                Ccoalesce( CFreeList.next );
                p1 = CFreeList.next;
                if( (char *)p1 == PermPtr + PermAvail ) {
                    /* remove block from free list */
                    pnext = p1->next;
                    pprev = p1->prev;
                    pprev->next = pnext;
                    pnext->prev = pprev;
                    len = p1->len;
                    PermAvail += len;
                }
            }
        } else {
            p1->len = len;
            pprev = &CFreeList;
            for( ;; ) {         /* insert in sorted order */
                pnext = pprev;
                pprev = pprev->prev;
                if( pprev == &CFreeList ) break;
                if( pprev < p1 ) break;
            }
            pnext->prev = p1;
            pprev->next = p1;
            p1->prev = pprev;
            p1->next = pnext;
            Ccoalesce( p1 );
        }
        break;
    case CMEM_NONE:
        len = 0;
        return;
    }
}


void *CPermAlloc( unsigned amount )
{
    char        *p;

    amount = (amount + MEM_ALIGN - 1) & -MEM_ALIGN;
    if( amount > PermAvail ) {
        AllocPermArea();            /* allocate another permanent area */
        if( amount > PermAvail ) {
            return( CMemAlloc( amount ) );
        }
    }
    PermAvail -= amount;
    p = PermPtr;
    PermPtr += amount;
    return( memset( p, 0, amount ) );       /* 27-jul-88 AFS */
}


void *FEmalloc( unsigned size )                 /* 16-jan-90 */
{
    void    *p;

    p = malloc( size );
    if( p == NULL ) {
        CErr1( ERR_OUT_OF_MEMORY );
        CSuicide();
    }
    return( p );
}


void FEfree( void *p )
{
    if( p != NULL )  free( p );
}


int FEMoreMem( int size )
{
    return( 0 );
}
