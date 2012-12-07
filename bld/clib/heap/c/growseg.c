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


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#if defined(__OS2__)
    #include <dos.h>
    #include "tinyos2.h"
    #define INCL_DOSMEMMGR
    #include <wos2.h>
#elif defined(__QNX__)
    #include <sys/types.h>
    #include <sys/seginfo.h>
    #include <i86.h>
#elif defined(__WINDOWS__) && defined( _M_I86 )
    #include <dos.h>
    #include "windows.h"
#else
    #include <dos.h>
    #include "tinyio.h"
#endif
#include "heap.h"

unsigned __GrowSeg( unsigned short seg, unsigned int amount )
    {
        unsigned n;             /* number of paragraphs desired   */
        unsigned int old_heaplen;
        unsigned int old_heap_paras;
        struct heapblk _WCFAR *p;
        struct freelist _WCFAR *pfree;
        struct freelist _WCFAR *pnew;
        tag _WCFAR *last_tag;

        if( !__heap_enabled ) return( 0 );
        p = (struct heapblk _WCFAR *)MK_FP( seg, 0 );
        old_heaplen = p->heaplen;
        if( old_heaplen != 0 ) {                /* if not already 64K */
            amount += TAG_SIZE;                      /* 25-feb-91 */
            if( amount < TAG_SIZE ) amount = ~0;
            if( amount < _amblksiz )  amount = _amblksiz;
            n = ( amount + 0x0f ) >> 4;
            if( n == 0 )  n = PARAS_IN_64K;     /* 23-may-89 */
            old_heap_paras = old_heaplen >> 4;
            n += old_heap_paras;
            /*
                We shouldn't extend segments to 64k if we are not going to
                use the space for this allocation.  In protected-mode
                environments, it should be possible to extend segments
                later on when we know we can use the space.
            */
#if defined(__QNX__)
            if( n > PARAS_IN_64K ) return( 0 );
#elif defined(__OS2__) || defined(__WINDOWS_286__)
            if( n > PARAS_IN_64K ) {
                if( _osmode != DOS_MODE ) {
                    /* protected-mode */
                    return( 0 );
                } else {
                    n = PARAS_IN_64K;
                }
            }
#else
            if( n > PARAS_IN_64K ) n = PARAS_IN_64K;
#endif
#if defined(__OS2__)
            if( DosReallocSeg( n << 4, seg ) != 0 )             return( 0 );
#elif defined(__QNX__)
            if( qnx_segment_realloc( seg, ((long)n) << 4 ) == -1 ) return( 0 );
#elif defined(__WINDOWS_286__)
            if( old_heap_paras < (PARAS_IN_64K-2) && n == PARAS_IN_64K ) {
                n = PARAS_IN_64K - 2;
            } else if( n > ( PARAS_IN_64K - 2 ) ) {
                /*
                  in Standard mode, GlobalRealloc may change selectors
                  if size > 65519 (64k-17)! (p. 4-246 Windows Ref) AFS 23-apr-91
                */
                return( 0 );
            }
            {
                HANDLE hmem;

                hmem = (HANDLE)GlobalHandle( seg );
                if( hmem == NULL ) {
                    return( 0 );
                }
                if( GlobalReAlloc( hmem, ((long)n) << 4,
                        __win_realloc_flags) == NULL ){
                    return( 0 );
                }
            }
#else
            if( TINY_ERROR( TinySetBlock( n, seg ) ) )          return( 0 );
#endif
            p->heaplen = n << 4;        /* put in new heap length */
            pfree = MK_FP( seg, p->freehead.prev );
            if( FP_OFF(pfree) + pfree->len != old_heaplen - TAG_SIZE*2 ) {
                /* last free entry not at end of the heap */
                /* add a new free entry to end of list */
                pnew = MK_FP( seg, old_heaplen - TAG_SIZE*2 );
                pnew->prev = FP_OFF(pfree);
                pnew->next = pfree->next;
                pfree->next = FP_OFF(pnew);
                p->freehead.prev = FP_OFF(pnew);
                p->numfree++;
                pfree = pnew;
            }
            pfree->len = p->heaplen - FP_OFF(pfree) - TAG_SIZE*2;
            if( pfree->len > p->largest_blk )  p->largest_blk = pfree->len;
            last_tag = MK_FP( seg, p->heaplen - TAG_SIZE*2 );
            *last_tag = END_TAG;
            last_tag[1] = 0;            /* link to next piece of near heap */
            return( 1 );                /* indicate segment was grown */
        }
        return( 0 );    /* indicate failed to grow the segment */
    }
