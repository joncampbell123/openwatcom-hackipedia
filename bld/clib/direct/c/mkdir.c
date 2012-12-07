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


#include "variety.h"
#include "widechar.h"
#include <string.h>
#include <direct.h>
#ifdef __WIDECHAR__
    #include <mbstring.h>
#endif
#include "seterrno.h"
#include "_doslfn.h"

#ifdef _M_I86
  #ifdef __BIG_DATA__
    #define AUX_INFO    \
        parm caller     [dx ax] \
        modify exact    [ax dx];
  #else
    #define AUX_INFO    \
        parm caller     [dx] \
        modify exact    [ax];
  #endif
#else
    #define AUX_INFO    \
        parm caller     [edx] \
        modify exact    [eax];
#endif

extern unsigned __mkdir_sfn( const char *path );
#pragma aux __mkdir_sfn = \
        _SET_DSDX       \
        _MOV_AH DOS_MKDIR \
        _INT_21         \
        _RST_DS         \
        "call __doserror_" \
        AUX_INFO

#if defined( __WATCOM_LFN__ ) && !defined( __WIDECHAR__ )
static unsigned _mkdir_lfn( const char *path )
/********************************************/
{
#ifdef _M_I86
    return( __mkdir_lfn( path ) );
#else
    call_struct     dpmi_rm;

    strcpy( RM_TB_PARM1_LINEAR, path );
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.eax = 0x7139;
    dpmi_rm.flags = 1;
    if( __dpmi_dos_call( &dpmi_rm ) ) {
        return( -1 );
    }
    if( dpmi_rm.flags & 1 ) {
        return( __set_errno_dos_reterr( (unsigned short)dpmi_rm.eax ) );
    }
    return( 0 );
#endif
}
#endif

_WCRTLINK int __F_NAME(mkdir,_wmkdir)( const CHAR_TYPE *path )
/************************************************************/
{
#ifdef __WIDECHAR__
    size_t              rcConvert;
    char                mbcsPath[MB_CUR_MAX * _MAX_PATH];
    char *              p;

    /*** Convert the wide character string to a multibyte string ***/
    rcConvert = wcstombs( mbcsPath, path, sizeof( mbcsPath ) );
    p = _mbsninc( mbcsPath, rcConvert );
    *p = '\0';
    return( mkdir( mbcsPath ) );
#else
  #ifdef __WATCOM_LFN__
    unsigned    rc = 0;

    if( _RWD_uselfn && (rc = _mkdir_lfn( path )) == 0 ) {
        return( 0 );
    }
    if( IS_LFN_ERROR( rc ) ) {
        return( -1 );
    }
  #endif
    if( __mkdir_sfn( path ) ) {
        return( -1 );
    }
    return( 0 );
#endif
}
