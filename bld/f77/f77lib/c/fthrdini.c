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
* Description:  thread initialization
*
****************************************************************************/


#include "ftnstd.h"
#include "ftextfun.h"
#include "ftextvar.h"
#include "fthread.h"
#include "errcod.h"

#if defined( __OS2__ )

#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#include <os2.h>

ULONG                   __fio_sem;

#elif defined( __NETWARE__ )

long                    __fio_sem;
extern long             OpenLocalSemaphore( long );
extern int              CloseLocalSemaphore( long );

#elif defined( __NT__ )

#include <windows.h>

HANDLE                  __fio_sem;

#elif defined( __LINUX__ )

// TODO: semaphore support for Linux!

#endif

int     __InitFThreadProcessing( void ) {
//=======================================

// Setup for multiple threads.

#if defined( __OS2__ )
    DosCreateMutexSem( NULL, &__fio_sem, 0, FALSE );
#elif defined( __NETWARE__ )
    __fio_sem = OpenLocalSemaphore( 1 );
#elif defined( __NT__ )
    __fio_sem = CreateMutex( NULL, FALSE, NULL );
#elif defined( __LINUX__ )
// TODO: semaphore support for Linux!
#endif
    _AccessFIO  = &__AccessFIO;
    _ReleaseFIO = &__ReleaseFIO;
    _PartialReleaseFIO = &__PartialReleaseFIO;
    __InitMultiThreadIO();
    return( 0 );
}

void            __FiniFThreadProcessing( void ) {
//===============================================

#if defined( __OS2__ )
    DosCloseMutexSem( __fio_sem );
#elif defined( __NETWARE__ )
    CloseLocalSemaphore( __fio_sem );
#elif defined( __NT__ )
    CloseHandle( __fio_sem );
#elif defined( __LINUX__ )
// TODO: semaphore support for Linux!
#endif
}
