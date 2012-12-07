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
* Description:  FORTRAN compiler main line
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "cpopt.h"
#include "fcgbls.h"
#include "boot77.h"
#include "errrtns.h"

#include <stdlib.h>
#include <process.h>
#include <string.h>

extern  bool            MainCmdLine(char **,char **,char **,char *);
extern  char            *SDSrcExtn(char *);
extern  void            InitPredefinedMacros(void);


static  char            CmdBuff[2*128];

#if _CPU == 386
    #define _WFC "wfc386"
#else
    #define _WFC "wfc"
#endif

#if defined( _M_IX86 )
    unsigned char   _8087   = 0;
    unsigned char   _real87 = 0;
#endif


int     main( int argc, char *argv[] ) {
//======================================

// FORTRAN compiler main line.

    int         ret_code;
    char        *opts[MAX_OPTIONS+1];
    char        *p;

    argc = argc; argv = argv;
#if defined( __INCL_ERRMSGS__ )
    {
        extern  void    __InitError(void);

        __InitError();
        __ErrorInit( NULL );
    }
#else
    {
        extern  void    __InitResource(void);

        __InitResource();
        __ErrorInit( argv[0] );
    }
#endif
#if defined( _M_IX86 )
    _real87 = _8087 = 0;
#endif
    p = getenv( _WFC );
    if( p != NULL ) {
        strcpy( CmdBuff, p );
        p = &CmdBuff[ strlen( p ) ];
        *p = ' ';
        ++p;
    } else {
        p = CmdBuff;
    }
    getcmd( p );
    ret_code = 0;
    InitCompMain();
    if( MainCmdLine( &SrcName, &CmdPtr, opts, CmdBuff ) ) {
        SrcExtn = SDSrcExtn( SrcName ); // parse the file name in case we get
        ProcOpts( opts );               // an error in ProcOpts() so error
        InitPredefinedMacros();         // file can be created
        ret_code = CompMain( CmdBuff );
    } else {
        ShowUsage();
    }
    FiniCompMain();
    __ErrorFini();
    return( ret_code );
}
