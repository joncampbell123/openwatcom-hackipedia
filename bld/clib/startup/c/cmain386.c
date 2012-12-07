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
* Description:  386 implementation of __CMain().
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "widechar.h"
#include "initarg.h"
#include "rtdata.h"

extern  void    __CommonInit( void );

#ifdef __WIDECHAR__
    extern      int     wmain( int, wchar_t ** );
    #if defined(_M_IX86)
        #pragma aux     __wCMain  "*";
    #endif
    void __wCMain( void )
    {
        #if !defined(__OSI__)
            /* allocate alternate stack for F77 */
            __ASTACKPTR = (char *)__alloca( __ASTACKSIZ ) + __ASTACKSIZ;
        #endif
        __CommonInit();
        exit( wmain( ___wArgc, ___wArgv ) );
    }
#else
    extern      int     main( int, char ** );
    #if defined(_M_IX86)
        #pragma aux     __CMain  "*";
    #endif
    void __CMain( void )
    {
        #if !defined(__OSI__)
            /* allocate alternate stack for F77 */
            __ASTACKPTR = (char *)__alloca( __ASTACKSIZ ) + __ASTACKSIZ;
        #endif
        __CommonInit();
        exit( main( ___Argc, ___Argv ) );
    }
#endif
