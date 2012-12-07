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

#ifdef __OS2__
#define INCL_DOS
#include <os2.h>
#else
#include <windows.h>
// this is defined by windows.h - MS are such idiots
#undef IGNORE
#endif

#include "standard.h"
#include "coderep.h"
#include "targsys.h"
#include "cgdefs.h"
#include "model.h"
#include "cgmisc.h"
#include "cgdllcli.h"

#define BY_CLI
#include "cgprotos.h"
#include "feprotos.h"

static fe_interface FERtnTable = {
    #define CGCALLBACKDEF( a, b, c )    a,
    #include "cgfertns.h"
    #undef  CGCALLBACKDEF
};

char *defaultDLLName =
#if _TARGET & _TARG_80386
"cg386.dll"
#elif _TARGET & _TARG_IAPX86
"cgi86.dll"
#elif _TARGET & _TARG_AXP
"cgaxp.dll"
#elif _TARGET & _TARG_PPC
"cgppc.dll"
#elif _TARGET & _TARG_MIPS
"cgmps.dll"
#else
#error Unknown target.
#endif
;

cg_interface *CGFuncTable;

#ifdef __OS2__
static HMODULE  dllHandle;
#else
static HANDLE   dllHandle;
#endif

int _CGAPI BEDLLLoad( char *dll_name )
/************************************/
{
#ifdef __OS2__
#define SIZE 32
    unsigned char badfile[SIZE];
#endif
    int retval;

    if( dll_name == NULL ) {
        dll_name = defaultDLLName;
    }
    CGFuncTable = NULL;
#ifdef __OS2__
    retval = DosLoadModule( (PSZ)badfile, SIZE, (PSZ)dll_name, &dllHandle );
#else
    dllHandle = LoadLibrary( dll_name );
    retval = ( dllHandle == 0 );
#endif
    if( retval == 0 ) {
        cg_interface * _CGDLLEXPORT (*func_ptr)( fe_interface * );
#ifdef __OS2__
        retval = DosQueryProcAddr( dllHandle, 0, (PSZ)"BEDLLInit", (PFN*)&func_ptr );
#else
        func_ptr = (cg_interface * _CGDLLEXPORT(*)( fe_interface * ))GetProcAddress( dllHandle, "_BEDLLInit@4" );
        retval = ( func_ptr == 0 );
#endif
        if( retval == 0 ) {
            CGFuncTable = func_ptr( &FERtnTable );
            return( TRUE );
        }
    }
    return( FALSE );
}

void _CGAPI BEDLLUnload( void )
/*****************************/
{
#ifdef __OS2__
    DosFreeModule( dllHandle );
#else
    FreeLibrary( dllHandle );
#endif
}

bool _CGAPI TBreak( void )
/************************/
{
    return( FALSE );
}

void _CGAPI CauseTBreak( void )
/*****************************/
{
}

