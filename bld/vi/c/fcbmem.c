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


#include "vi.h"
#include "fcbmem.h"

/*
 * FcbAlloc - allocate an fcb and initialize it
 */
fcb *FcbAlloc( file *f )
{
    fcb *cfcb;

    /*
     * allocate fcb
     */
    cfcb = MemAlloc( FCB_SIZE );

    /*
     * thread fcb with all the others
     */
    if( FcbThreadHead == NULL ) {
        FcbThreadHead = FcbThreadTail = cfcb;
    } else {
        cfcb->thread_prev = FcbThreadTail;
        FcbThreadTail->thread_next = cfcb;
        FcbThreadTail = cfcb;
    }

    /*
     * initialize the data
     */
    cfcb->f = f;
    cfcb->non_swappable = TRUE;
    cfcb->in_memory = TRUE;
    cfcb->swapped = FALSE;
    cfcb->on_display = FALSE;
    cfcb->was_on_display = FALSE;
#ifndef NOXTD
    cfcb->in_extended_memory = FALSE;
#endif
#ifndef NOXMS
    cfcb->in_xms_memory = FALSE;
#endif
#ifndef NOEMS
    cfcb->in_ems_memory = FALSE;
#endif
    cfcb->nullfcb = FALSE;
    cfcb->dead = FALSE;
    cfcb->offset = -1L;
    FcbBlocksInUse++;

    return( cfcb );

} /* FcbAlloc */

/*
 * FcbFree - release an fcb
 */
void FcbFree( fcb *cfcb )
{
    /*
     * release swap data
     */
    if( cfcb->offset >= 0 ) {
        GiveBackSwapBlock( cfcb->offset );
    }
    if( cfcb->xmemaddr > 0 ) {
#ifndef NOEMS
        if( cfcb->in_ems_memory ) {
            GiveBackEMSBlock( cfcb->xmemaddr );
        }
#endif
#ifndef NOXTD
        if( cfcb->in_extended_memory ) {
            GiveBackXMemBlock( cfcb->xmemaddr );
        }
#endif
#ifndef NOXMS
        if( cfcb->in_xms_memory ) {
            GiveBackXMSBlock( cfcb->xmemaddr );
        }
#endif
    }

    if( cfcb == FcbThreadHead ) {
        FcbThreadHead = cfcb->thread_next;
    }
    if( cfcb == FcbThreadTail ) {
        FcbThreadTail = cfcb->thread_prev;
    }
    if( cfcb->thread_prev != NULL ) {
        cfcb->thread_prev->thread_next = cfcb->thread_next;
    }
    if( cfcb->thread_next != NULL ) {
        cfcb->thread_next->thread_prev = cfcb->thread_prev;
    }

    /*
     * release fcb data
     */
    MemFree( cfcb );
    FcbBlocksInUse--;

} /* FcbFree */

/*
 * FreeEntireFcb - free all data in an fcb
 */
void FreeEntireFcb( fcb *cfcb )
{
    line        *cline, *tline;

    if( cfcb->in_memory ) {
        for( cline = cfcb->lines.head; cline != NULL; cline = tline ) {
            tline = cline->next;
            MemFree( cline );
        }
    }
    FcbFree( cfcb );

} /* FreeEntireFcb */

/*
 * FreeFcbList - free up a list of fcbs
 */
void FreeFcbList( fcb *cfcb )
{
    fcb     *tfcb;

    for( ; cfcb != NULL; cfcb = tfcb ) {
        tfcb = cfcb->next;
        FreeEntireFcb( cfcb );
    }

} /* FreeFcbList */
