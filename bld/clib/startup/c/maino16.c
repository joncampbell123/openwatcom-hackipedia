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
* Description:  Main entrypoint for 16-bit OS/2.
*
****************************************************************************/


#pragma library (os2);

#include "variety.h"
#define INCL_DOSDEVICES
#define INCL_SUB
#define INCL_DOSSEMAPHORES
#define INCL_DOSINFOSEG
#define INCL_DOSMISC
#define INCL_DOSMEMMGR
#include <wos2.h>
#include <string.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stddef.h>
#include <process.h>
#include "exitwmsg.h"
#include "initfini.h"
#include "crwd.h"
#include "thread.h"

#if defined(__SW_BM)

/* semaphore control for file handles */

ULONG                   __iosemaphore[_NFILES];
int                     __iosemcount[_NFILES];
TID                     __iothreadid[_NFILES];

extern  void            *__InitThreadProcessing( void );
extern  void            __SetupThreadProcessing( int );

extern  unsigned        __MaxThreads;

extern  struct thread_data *    __MultipleThread();
#define _STACKLOW       (*(unsigned *)(__MultipleThread()))

#else

unsigned                _STACKLOW;

#endif

/* global data */

int  __far *            _threadid;
char __far *            _LpCmdLine;     /* pointer to command line */
char __far *            _LpPgmName;     /* pointer to program name */
unsigned                _dynend;
unsigned                _curbrk;
unsigned                _STACKTOP;
unsigned short          _HShift;
int                     _cbyte;     /* used by getch, getche */
int                     _cbyte2;    /* used by getch */
unsigned char __near    _osmajor;
unsigned char __near    _osminor;
unsigned char __near    _osmode;
#if defined(__SW_BD)
#include <setjmp.h>
jmp_buf                 JmpBuff;
int                     RetCode;
#endif
#if !defined(__SW_BM)
int                     _nothread;
#endif

/* End of static data - used in OS/2 DLL to find beginning of near heap */
extern char             end;

void __far __null_FPE_handler( void )
{
}

void    (__far *__FPE_handler)() = __null_FPE_handler;

extern  int _CMain( void );
#pragma aux _CMain "_*";
#pragma aux _OS2Main "_*" parm caller [ dx ax ] [ cx bx ];


int _OS2Main( char far *stklow, char far * stktop,
               unsigned envseg, unsigned cmdoff )
/*************************************************/
{
    cmdoff = cmdoff;    /* supress warnings */
    envseg = envseg;
    stktop = stktop;

    /* set up global variables */
#if defined(__SW_BD)
    _STACKTOP = 0;
    _curbrk = _dynend = (unsigned)&end;
    stklow = NULL;
#else
    _STACKTOP = FP_OFF( stktop );
    _curbrk = _dynend = _STACKTOP;
#endif
    DosGetHugeShift( (PUSHORT)&_HShift );
    DosGetMachineMode( (PBYTE)&_osmode );
    {
    unsigned short      version;

    DosGetVersion( (PUSHORT)&version );
    _osmajor = version >> 8;
    _osminor = version & 0xff;
    }

#if defined(__SW_BD)
    _LpPgmName = "";
    _LpCmdLine = "";
#else
    /* copy progname and arguments to bottom of stack */
    {
    char                far *src;
    char                far *pgmp;

    src = MK_FP( envseg, cmdoff );
    _LpPgmName = stklow;
    /* back up from the ao: pointer to the eo: pointer (see OS/2 2.0 docs)*/
    for( pgmp = src - 1; *--pgmp != '\0'; );
    ++pgmp;
    while( *stklow++ = *pgmp++ );
    while( *src ) ++src;
    ++src;
    _LpCmdLine = stklow;
    while( *stklow++ = *src++ );
    }
#endif

#if defined(__SW_BM)
    {
        SEL             globalseg;
        SEL             localseg;

        DosGetInfoSeg( &globalseg, &localseg );
        _threadid = MK_FP( localseg, offsetof( LINFOSEG, tidCurrent ) );
        if( __InitThreadProcessing() == NULL )
            __fatal_runtime_error( "Not enough memory", 1 );
        #if defined(__SW_BD)
        {
            unsigned    i;
            unsigned    j;
            j = __MaxThreads;
            for( i = 1; i <= j; i++ ) {
                __SetupThreadProcessing( i );
            }
        }
        #else
            __SetupThreadProcessing( 1 );
        #endif
        _STACKLOW = (unsigned)stklow;
    }
#else
    _nothread = getpid();
    _threadid = &_nothread;
    _STACKLOW = (unsigned)stklow; /* set bottom of stack */
#endif
//  {   /* removed JBS 99/11/10 */
//      // make sure the iomode array is of the proper length
//      // this needs to be done before the InitRtns
//      extern  void    __grow_iomode(int);

//      if( _osmode == OS2_MODE ) {
//          __grow_iomode( 100 );
//      }
//  }
    __InitRtns( 255 );
#ifdef __SW_BD
    {
        int status;
        status = setjmp( JmpBuff );
        if( status == 0 ) return( _CMain() );
        return( RetCode );
    }
#else
    return( _CMain() );
#endif
}


_WCRTLINK void  __exit( unsigned ret_code )
/*****************************************/
{
    __FiniRtns( 0, FINI_PRIORITY_EXIT-1 );
#ifdef __SW_BD
    RetCode = ret_code;
    longjmp( JmpBuff, 1 );
#else
    DosExit( EXIT_PROCESS, ret_code );
#endif
}
