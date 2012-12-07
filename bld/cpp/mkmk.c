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

#include <stdio.h>
#if !defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <conio.h>
#endif
#include <stdlib.h>
#include "preproc.h"
#include <malloc.h>

extern char PreProcChar;

void MkMkDependency( char *filename, char *fullname, char *delim )
{
    printf( "#include %c%s%c  ==> ", delim[0], filename, delim[1] );
    printf( "%c%s%c\n", delim[0], fullname, delim[1] );
}

int main( int argc, char *argv[] )
{
    if( argc < 2 ) {
        cprintf( "Usage: mkmk filename\r\n" );
        exit( 1 );
    }
    if( argv[2] != NULL ) {
        PreProcChar = argv[2][0];
    }
    if( PP_Init( argv[1], PPFLAG_DEPENDENCIES, NULL ) != 0 ) {
        cprintf( "Unable to open '%s'\r\n", argv[1] );
        exit( 1 );
    }
    // call PP_Define here to predefine any desired macros
    PP_Dependency_List( MkMkDependency );
    PP_Fini();
    return( 0 );
}
