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
* Description:  Implementation of stat().
*
****************************************************************************/


#ifdef __OSI__
#define __OS2__
#endif
#include "variety.h"
#include "widechar.h"
#undef __INLINE_FUNCTIONS__
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <direct.h>

#include <dosfunc.h>
#include <mbstring.h>
#include "_doslfn.h"
#include "dosdir.h"
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include <stdlib.h>
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#include "d2ttime.h"
#ifdef __INT64__
#include "int64.h"
#endif

#define HAS_DRIVE(x)    (__F_NAME(isalpha,iswalpha)(x[0]) && x[1]==STRING(':'))
#define ALL_ATTRIB      (_A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_SUBDIR | _A_ARCH)

#ifdef __INT64__

 _WCRTLINK int __F_NAME(_stati64,_wstati64)( CHAR_TYPE const *path, struct _stati64 *buf )
{
    struct _stat        buf32;
    int                 rc;
    INT_TYPE            tmp;

    /*** Get the info using non-64bit version ***/
    rc = __F_NAME(stat,_wstat)( path, &buf32 );
    if( rc != -1 ) {
        /*** Convert the info to 64-bit equivalent ***/
        buf->st_dev = buf32.st_dev;
        buf->st_ino = buf32.st_ino;
        buf->st_mode = buf32.st_mode;
        buf->st_nlink = buf32.st_nlink;
        buf->st_uid = buf32.st_uid;
        buf->st_gid = buf32.st_gid;
        buf->st_rdev = buf32.st_rdev;
        _clib_U32ToU64( buf32.st_size, tmp );
        buf->st_size = GET_REALINT64(tmp);
        buf->st_atime = buf32.st_atime;
        buf->st_mtime = buf32.st_mtime;
        buf->st_ctime = buf32.st_ctime;
        buf->st_btime = buf32.st_btime;
        buf->st_attr = buf32.st_attr;
        buf->st_archivedID = buf32.st_archivedID;
        buf->st_updatedID = buf32.st_updatedID;
        buf->st_inheritedRightsMask = buf32.st_inheritedRightsMask;
        buf->st_originatingNameSpace = buf32.st_originatingNameSpace;
    }
    return( rc );
}

#else

static unsigned short at2mode( int attr, char *fname )
/****************************************************/
{
    unsigned short  mode;
    char            *ext;

    if( attr & _A_SUBDIR ) {
        mode = S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    } else if( attr & AT_ISCHR ) {
        mode = S_IFCHR;
    } else {
        mode = S_IFREG;
        /* determine if file is executable, very PC specific */
        if( (ext = _mbschr( fname, '.' )) != NULL ) {
            ++ext;
            if( _mbscmp( ext, "EXE" ) == 0 || _mbscmp( ext, "COM" ) == 0 ) {
                mode |= S_IXUSR | S_IXGRP | S_IXOTH;
            }
        }
    }
    mode |= S_IRUSR | S_IRGRP | S_IROTH;
    if( !(attr & _A_RDONLY) )                   /* if file is not read-only */
        mode |= S_IWUSR | S_IWGRP | S_IWOTH;    /* - indicate writeable     */
    return( mode );
}

_WCRTLINK int __F_NAME(stat,_wstat)( CHAR_TYPE const *path,
                                              struct __F_NAME(stat,_stat) *buf )
/******************************************************************************/
{
    struct find_t       dta;
    const CHAR_TYPE     *ptr;
    unsigned            rc;
    CHAR_TYPE           fullpath[_MAX_PATH];
    int                 isrootdir = 0;

    /* reject null string and names that has wildcard */
    if( *path == NULLCHAR || __F_NAME(_mbspbrk,wcspbrk)( path, STRING( "*?" ) ) != NULL ) {
        __set_errno( ENOENT );
        return( -1 );
    }

    /*** Determine if 'path' refers to a root directory ***/
    if( __F_NAME(_fullpath,_wfullpath)( fullpath, path, _MAX_PATH ) != NULL ) {
        if( HAS_DRIVE( fullpath ) &&
            fullpath[2] == STRING( '\\' ) && fullpath[3] == NULLCHAR )
        {
            isrootdir = 1;
        }
    }

    ptr = path;
    if( HAS_DRIVE( path ) )
        ptr += 2;
    if( ( ptr[0] == STRING( '\\' ) || ptr[0] == STRING( '/' ) ) && ptr[1] == NULLCHAR || isrootdir ) {
        /* handle root directory */
        CHAR_TYPE       cwd[_MAX_PATH];

        /* save current directory */
        __F_NAME(getcwd,_wgetcwd)( cwd, _MAX_PATH );

        /* try to change to specified root */
        if( __F_NAME(chdir,_wchdir)( path ) != 0 )
            return( -1 );

        /* restore current directory */
        __F_NAME(chdir,_wchdir)( cwd );

        dta.attrib    = _A_SUBDIR;      /* fill in DTA */
        dta.wr_time   = 0;
        dta.wr_date   = 0;
        dta.size      = 0;
        dta.name[0]   = NULLCHAR;
#ifdef __WATCOM_LFN__
        SIGN_OF( &dta )   = 0;
        HANDLE_OF( &dta ) = 0;
#endif
    } else {                            /* not a root directory */
#if defined(__WIDECHAR__)
        char    mbPath[MB_CUR_MAX * _MAX_PATH];

        if( wcstombs( mbPath, path, sizeof( mbPath ) ) == -1 ) {
            mbPath[0] = '\0';
        }
#endif
        if( _dos_findfirst( __F_NAME(path,mbPath), ALL_ATTRIB, &dta ) != 0 ) {
            int         handle;
            int         canread = 0;
            int         canwrite = 0;
            int         fstatok = 0;

            // Try getting information another way.
            rc = 0;
            handle = __F_NAME(open,_wopen)( path, O_WRONLY );
            if( handle != -1 ) {
                canwrite = 1;
                if( __F_NAME(fstat,_wfstat)( handle, buf ) == -1 ) {
                    rc = errno;
                } else {
                    fstatok = 1;
                }
            }
            close( handle );
            handle = __F_NAME(open,_wopen)( path, O_RDONLY );
            if( handle != -1 ) {
                canread = 1;
                if( !fstatok ) {
                    if( __F_NAME(fstat,_wfstat)( handle, buf ) == -1 ) {
                        rc = errno;
                    }
                }
            }
            close( handle );
            if( !canread && !canwrite ) {
                __set_errno( ENOENT );
                return( -1 );
            }
            __set_errno( rc );
            if( rc != 0 ) {
                return( -1 );
            }
            if( canread ) {
                buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
            }
            if( canwrite ) {
                buf->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
            }
            return( 0 );
        }
        _dos_findclose( &dta );
    }

    /* process drive number */
    if( HAS_DRIVE( path ) ) {
        buf->st_dev = __F_NAME(tolower,towlower)( *path ) - STRING( 'a' );
    } else {
        buf->st_dev = TinyGetCurrDrive();
    }
    buf->st_rdev = buf->st_dev;

    buf->st_size = dta.size;
    buf->st_mode = at2mode( dta.attrib, dta.name );

    buf->st_mtime = _d2ttime( dta.wr_date, dta.wr_time );
    buf->st_btime = buf->st_mtime;
#ifdef __WATCOM_LFN__
    if( IS_LFN( &dta ) && CRTIME_OF( &dta ) ) {
        buf->st_atime = _d2ttime( ACDATE_OF( &dta ), ACTIME_OF( &dta ) );
        buf->st_ctime = _d2ttime( CRDATE_OF( &dta ), CRTIME_OF( &dta ) );
    } else {
#endif
        buf->st_atime = buf->st_mtime;
        buf->st_ctime = buf->st_mtime;
#ifdef __WATCOM_LFN__
    }
#endif
    buf->st_nlink = 1;
    buf->st_ino = buf->st_uid = buf->st_gid = 0;

    buf->st_attr = dta.attrib;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;

    return( 0 );
}

#endif
