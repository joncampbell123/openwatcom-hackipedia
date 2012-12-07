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
* Description:  Signal handling related globals.
*
****************************************************************************/


#if defined(__NT__) || defined(__OS2__)
    #if defined( __SW_BM ) && (defined(__386__) || defined(__AXP__) || defined(__PPC__))
        #include "osthread.h"
        #define __SIGNALTABLE   (__THREADDATAPTR->signal_table)
        #define __XCPTHANDLER   (__THREADDATAPTR->xcpt_handler)
    #else
        #include "sigdefn.h"
        extern struct _EXCEPTIONREGISTRATIONRECORD *__XcptHandler;
        #define __SIGNALTABLE   _SignalTable
        #define __XCPTHANDLER   __XcptHandler
    #endif
    _WCRTLINK extern void       (*__sig_init_rtn)( void );
    _WCRTLINK extern void       (*__sig_fini_rtn)( void );
#elif defined(__NETWARE__)
    #define __SIGNALTABLE       (__THREADDATAPTR->signal_table)
#elif defined(__RDOS__)
    #include "osthread.h"
    #define __SIGNALTABLE   (__THREADDATAPTR->signal_table)
    #define __XCPTHANDLER   (__THREADDATAPTR->xcpt_handler)
    _WCRTLINK extern void       (*__sig_init_rtn)( void );
    _WCRTLINK extern void       (*__sig_fini_rtn)( void );
#else
    #define __SIGNALTABLE       _SignalTable
#endif
