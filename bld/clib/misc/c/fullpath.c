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
* Description:  Implementation of fullpath() - returns fully qualified
*               pathname of a file.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#if defined(__UNIX__)
    #include <dirent.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <fcntl.h>
#if defined(__QNX__)
    #include <sys/io_msg.h>
#endif
#else
    #include <direct.h>
    #if defined(__OS2__)
        #include <wos2.h>
    #elif defined(__NT__)
        #include <windows.h>
        #include "libwin32.h"
    #elif defined(__DOS__)
        #include "_doslfn.h"
    #elif !defined(__NETWARE__)
        #include "tinyio.h"
    #endif
#endif
#include "liballoc.h"
#include "rtdata.h"
#include "seterrno.h"
#if defined(__WIDECHAR__) || defined(__WARP__)
    #include <mbstring.h>
#endif

#define _WILL_FIT( c )  if(( (c) + 1 ) > size ) {       \
                            __set_errno( ERANGE );      \
                            return( NULL );             \
                        }                               \
                        size -= (c);

#ifdef __UNIX__
#define _IS_SLASH( c )  ((c) == '/')
#else
#define _IS_SLASH( c )  ((c) == STRING('/') || (c) == STRING('\\'))
#endif

#if !defined( __NT__ ) && !defined( __NETWARE__ ) && !defined( __UNIX__ )
#pragma on (check_stack);
#endif

#ifdef __NETWARE__
extern char *ConvertNameToFullPath( const char *, char * );
#endif

#if defined(__QNX__)

static char *__qnx_fullpath( char *fullpath, const char *path )
/*************************************************************/
{
    struct {
            struct _io_open _io_open;
            char            m[ _QNX_PATH_MAX ];
    } msg;
    int             fd;

    msg._io_open.oflag = _IO_HNDL_INFO;
    fd = __resolve_net( _IO_HANDLE, 1, &msg._io_open, path, 0, fullpath );
    if( fd != -1 ) {
        close( fd );
    } else if( errno != ENOENT ) {
        return( NULL );
    } else {
        __resolve_net( 0, 0, &msg._io_open, path, 0, fullpath );
    }
    return( fullpath );
}

#elif defined( __DOS__ )

#ifdef _M_I86
  #ifdef __BIG_DATA__
    #define AUX_INFO    \
        parm caller     [si ax] [dl] \
        modify exact    [ax si];
  #else
    #define AUX_INFO    \
        parm caller     [si] [dl] \
        modify exact    [ax];
  #endif
#else
    #define AUX_INFO    \
        parm caller     [esi] [dl] \
        modify exact    [eax];
#endif

extern unsigned __getdcwd_sfn( const char *buff, unsigned char drv );
#pragma aux __getdcwd_sfn = \
        _SET_DSSI       \
        _MOV_AH DOS_GETCWD \
        _INT_21         \
        _RST_DS         \
        "call __doserror_" \
        AUX_INFO

#if defined( __WATCOM_LFN__ )
static unsigned _getdcwd_lfn( const char *buff, unsigned char drv )
/*****************************************************************/
{
  #ifdef _M_I86
    return( __getdcwd_lfn( buff, drv ) );
  #else
    call_struct     dpmi_rm;

    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.esi = RM_TB_PARM1_OFFS;
    dpmi_rm.edx = drv;
    dpmi_rm.eax = 0x7147;
    dpmi_rm.flags = 1;
    if( __dpmi_dos_call( &dpmi_rm ) ) {
        return( -1 );
    }
    if( dpmi_rm.flags & 1 ) {
        return( __set_errno_dos_reterr( (unsigned short)dpmi_rm.eax ) );
    }
    strcpy( (char *)buff, RM_TB_PARM1_LINEAR );
    return( 0 );
  #endif
}
#endif

static unsigned __getdcwd( const char *buff, unsigned char drv )
/**************************************************************/
{
#if defined( __WATCOM_LFN__ )
    unsigned        rc = 0;

    if( _RWD_uselfn && (rc = _getdcwd_lfn( buff, drv )) == 0 ) {
        return( rc );
    }
    if( IS_LFN_ERROR( rc ) ) {
        return( rc );
    }
#endif
    return( __getdcwd_sfn( buff, drv ) );
}

#endif

_WCRTLINK CHAR_TYPE *__F_NAME(_sys_fullpath,_sys_wfullpath)
                ( CHAR_TYPE *buff, const CHAR_TYPE *path, size_t size )
/*********************************************************************/
{

#if defined(__NT__)
    CHAR_TYPE       *filepart;
    DWORD           rc;

    if( __F_NAME(stricmp,_wcsicmp)( path, STRING( "con" ) ) == 0 ) {
        _WILL_FIT( 3 );
        return( __F_NAME(strcpy,wcscpy)( buff, STRING( "con" ) ) );
    }

    /*** Get the full pathname ***/
    rc = __F_NAME(GetFullPathNameA,__lib_GetFullPathNameW)( path, size, buff, &filepart );
    // If the buffer is too small, the return value is the size of
    // the buffer, in TCHARs, required to hold the path.
    // If the function fails, the return value is zero. To get extended error
    // information, call GetLastError.
    if( ( rc == 0 ) || ( rc > size ) ) {
        __set_errno_nt();
        return( NULL );
    }

    return( buff );
#elif defined(__WARP__)
    APIRET      rc;
    char        root[ 4 ];      /* SBCS: room for drive, ':', '\\', and null */
  #ifdef __WIDECHAR__
    char        mbBuff[ _MAX_PATH * MB_CUR_MAX ];
    char        mbPath[ _MAX_PATH * MB_CUR_MAX ];
  #endif

    if( __F_NAME(isalpha,iswalpha)( path[ 0 ] ) && ( path[ 1 ] == STRING( ':' ) ) && ( path[ 2 ] == STRING( '\\' ) ) ) {
        int i;
        i = __F_NAME(strlen,wcslen)( path );
        _WILL_FIT( i );
        __F_NAME(strcpy,wcscpy)( buff, path );
        return( buff );
    }

    /*
     * Check for x:filename.ext when drive x doesn't exist.  In this
     * case, return x:\filename.ext, not NULL, to be consistent with
     * MS and with the NT version of _fullpath.
     */
    if( __F_NAME(isalpha,iswalpha)( path[ 0 ] ) && path[ 1 ] == STRING( ':' ) ) {
        /*** We got this far, so path can't start with letter:\ ***/
        root[ 0 ] = (char)path[ 0 ];
        root[ 1 ] = ':';
        root[ 2 ] = '\\';
        root[ 3 ] = '\0';
        rc = DosQueryPathInfo( root, FIL_QUERYFULLNAME, buff, size );
        if( rc != NO_ERROR ) {
            /*** Drive does not exist; return x:\filename.ext ***/
            _WILL_FIT( __F_NAME(strlen,wcslen)( &path[ 2 ] ) + 3 );
            buff[ 0 ] = root[ 0 ];
            buff[ 1 ] = STRING( ':' );
            buff[ 2 ] = STRING( '\\' );
            __F_NAME(strcpy,wcscpy)( &buff[ 3 ], &path[ 2 ] );
            return( buff );
        }
    }

  #ifdef __WIDECHAR__
    if( wcstombs( mbPath, path, sizeof( mbPath ) ) == (size_t)-1 ) {
        return( NULL );
    }
    rc = DosQueryPathInfo( (PSZ)mbPath, FIL_QUERYFULLNAME, mbBuff, sizeof( mbBuff ) );
  #else
    rc = DosQueryPathInfo( (PSZ)path, FIL_QUERYFULLNAME, buff, size );
  #endif
    if( rc != 0 ) {
        __set_errno_dos( rc );
        return( NULL );
    }
  #ifdef __WIDECHAR__
    if( mbstowcs( buff, mbBuff, size ) != (size_t)-1 ) {
        return( buff );
    } else {
        return( NULL );
    }
  #else
    return( buff );
  #endif
#elif defined(__QNX__) || defined( __NETWARE__ )
    size_t len;
    char temp_dir[ _MAX_PATH ];

  #if defined(__NETWARE__)
    if( ConvertNameToFullPath( path, temp_dir ) != 0 ) {
        return( NULL );
    }
  #else
    if( __qnx_fullpath( temp_dir, path ) == NULL ) {
        return( NULL );
    }
  #endif
    len = strlen( temp_dir );
    if( len >= size ) {
        __set_errno( ERANGE );
        return( NULL );
    }
    return( strcpy( buff, temp_dir ) );
#elif defined(__UNIX__)
    const char  *p;
    char        *q;
    size_t      len;
    char        curr_dir[ _MAX_PATH ];

    p = path;
    q = buff;
    if( ! _IS_SLASH( p[ 0 ] ) ) {
        if( getcwd( curr_dir, sizeof( curr_dir ) ) == NULL ) {
            __set_errno( ENOENT );
            return( NULL );
        }
        len = strlen( curr_dir );
        _WILL_FIT( len );
        strcpy( q, curr_dir );
        q += len;
        if( q[ -1 ] != '/' ) {
            _WILL_FIT( 1 );
            *(q++) = '/';
        }
        for( ;; ) {
            if( p[ 0 ] == '\0' )
                break;
            if( p[ 0 ] != '.' ) {
                _WILL_FIT( 1 );
                *(q++) = *(p++);
                continue;
            }
            ++p;
            if( _IS_SLASH( p[ 0 ] ) ) {
                /* ignore "./" in directory specs */
                if( ! _IS_SLASH( q[ -1 ] ) ) {
                    *q++ = '/';
                }
                ++p;
                continue;
            }
            if( p[ 0 ] == '\0' )
                break;
            if( p[ 0 ] == '.' && _IS_SLASH( p[ 1 ] ) ) {
                /* go up a directory for a "../" */
                p += 2;
                if( ! _IS_SLASH( q[ -1 ] ) ) {
                    return( NULL );
                }
                q -= 2;
                for( ;; ) {
                    if( q < buff ) {
                        return( NULL );
                    }
                    if( _IS_SLASH( *q ) )
                        break;
                    --q;
                }
                ++q;
                *q = '\0';
                continue;
            }
            _WILL_FIT( 1 );
            *(q++) = '.';
        }
        *q = '\0';
    } else {
        len = strlen( p );
        _WILL_FIT( len );
        strcpy( q, p );
    }
    return( buff );
#else
    const CHAR_TYPE     *p;
    CHAR_TYPE           *q;
    size_t              len;
    int                 path_drive_idx;
    char                curr_dir[ _MAX_PATH ];

    p = path;
    q = buff;
    _WILL_FIT( 2 );
    if( __F_NAME(isalpha,iswalpha)( p[ 0 ] ) && p[ 1 ] == STRING( ':' ) ) {
        path_drive_idx = ( __F_NAME(tolower,towlower)( p[ 0 ] ) - STRING( 'a' ) ) + 1;
        q[ 0 ] = p[ 0 ];
        q[ 1 ] = p[ 1 ];
        p += 2;
    } else {
  #if defined(__OS2__)
        ULONG   drive_map;
        OS_UINT os2_drive;

        if( DosQCurDisk( &os2_drive, &drive_map ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
        path_drive_idx = os2_drive;
  #else
        path_drive_idx = TinyGetCurrDrive() + 1;
  #endif
        q[ 0 ] = STRING( 'A' ) + ( path_drive_idx - 1 );
        q[ 1 ] = STRING( ':' );
    }
    q += 2;
    if( ! _IS_SLASH( p[ 0 ] ) ) {
  #if defined(__OS2__)
        OS_UINT dir_len = sizeof( curr_dir );

        if( DosQCurDir( path_drive_idx, curr_dir, &dir_len ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
  #elif defined(__DOS__)
        if( __getdcwd( curr_dir, path_drive_idx ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
  #else
        tiny_ret_t rc;

        rc = TinyGetCWDir( curr_dir, path_drive_idx );
        if( TINY_ERROR( rc ) ) {
            __set_errno( ENOENT );
            return( NULL );
        }
  #endif
        len = strlen( curr_dir );
        if( curr_dir[ 0 ] != '\\' ) {
            _WILL_FIT( 1 );
            *(q++) = STRING( '\\' );
        }
        _WILL_FIT( len );
  #ifdef __WIDECHAR__
        if( mbstowcs( q, curr_dir, len + 1 ) == (size_t)-1 ) {
            return( NULL );
        }
  #else
        strcpy( q, curr_dir );
  #endif
        q += len;
        if( q[ -1 ] != STRING( '\\' ) ) {
            _WILL_FIT( 1 );
            *(q++) = STRING( '\\' );
        }
        for( ;; ) {
            if( p[ 0 ] == NULLCHAR )
                break;
            if( p[ 0 ] != STRING( '.' ) ) {
                _WILL_FIT( 1 );
                *(q++) = *(p++);
                continue;
            }
            ++p;     // at least '.'
            if( _IS_SLASH( p[ 0 ] ) ) {
                /* ignore "./" in directory specs */
                if( ! _IS_SLASH( q[ -1 ] ) ) {
                    *q++ = STRING( '\\' );
                }
                ++p;
                continue;
            }
            if( p[ 0 ] == NULLCHAR )
                break;
            if( p[ 0 ] == STRING( '.' ) ) {  /* .. */
                ++p;
                if( _IS_SLASH( p[ 0 ] ) ) {   /* "../" */
                    ++p;
                }
                if( ! _IS_SLASH( q[ -1 ] ) ) {
                    return( NULL );
                }
                q -= 2;
                for( ;; ) {
                    if( q < buff ) {
                        return( NULL );
                    }
                    if( _IS_SLASH( *q ) )
                        break;
                    if( *q == STRING( ':' ) ) {
                        ++q;
                        *q = STRING( '\\' );
                        break;
                    }
                    --q;
                }
                ++q;
                *q = NULLCHAR;
                continue;
            }
            _WILL_FIT( 1 );
            *(q++) = STRING( '.' );
        }
        *q = NULLCHAR;
    } else {
        len = __F_NAME(strlen,wcslen)( p );
        _WILL_FIT( len );
        __F_NAME(strcpy,wcscpy)( q, p );
    }
    /* force to all backslashes */
    for( q = buff; *q != NULLCHAR; ++q ) {
        if( *q == STRING( '/' ) ) {
            *q = STRING( '\\' );
        }
    }
    return( buff );
#endif
}

_WCRTLINK CHAR_TYPE *__F_NAME(_fullpath,_wfullpath)
                ( CHAR_TYPE *buff, const CHAR_TYPE *path, size_t size )
/*********************************************************************/
{
    CHAR_TYPE *ptr = NULL;

    if( buff == NULL ) {
        size = _MAX_PATH;
        ptr = lib_malloc( size * CHARSIZE );
        if( ptr == NULL )
            __set_errno( ENOMEM );
        buff = ptr;
    }
    if( buff != NULL ) {
        buff[ 0 ] = NULLCHAR;
        if( path == NULL || path[ 0 ] == NULLCHAR ) {
            buff = __F_NAME(getcwd,_wgetcwd)( buff, size );
        } else {
            buff = __F_NAME(_sys_fullpath,_sys_wfullpath)( buff, path, size );
        }
        if( buff == NULL ) {
            if( ptr != NULL ) {
                lib_free( ptr );
            }
        }
    }
    return( buff );
}
