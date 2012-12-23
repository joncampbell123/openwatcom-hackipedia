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


#include "wfilenam.hpp"
#include "wobjfile.hpp"
#include "diskos.h"

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
#ifndef __UNIX__
    #include <direct.h>
    #include <dos.h>
#endif
    #include <fcntl.h>
    #include <time.h>
#ifdef __UNIX__
    #include <sys/types.h>
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
};

typedef struct fullName {
    char        path[ _MAX_PATH + 1 ];
    char        drive[ _MAX_DRIVE + 1 ];
    char        dir[ _MAX_DIR + 1 ];
    char        fname[ _MAX_FNAME + 1 ];
    char        ext[ _MAX_EXT + 1 ];
} FullName;

#ifndef __UNIX__
static bool setdrive( const char* drive, unsigned* olddrive )
{
    if( strlen( drive ) > 0 ) {
        _dos_getdrive( olddrive );
        unsigned drv = toupper( drive[0]) - 'A' + 1;    //1='A'; 2='B'; ...
        if( *olddrive != drv ) {
            unsigned total; _dos_setdrive( drv, &total );
            unsigned newdrive; _dos_getdrive( &newdrive );
            if( drv != newdrive ) {
                return FALSE;
            }
        }
    }
    return TRUE;
}
#endif

static void makepath( char* path, const char* drive, const char* dir, const char* fname, const char* ext )
{
    path[2] = '\0';     //avoid bug in _makepath
    _makepath( path, drive, dir, fname, ext );
}

static void splitpath( const char* path, char* drive, char* dir, char* fname, char* ext )
{
    _splitpath( path, drive, dir, fname, ext );
    if( dir != NULL ) {
        int dirlen = strlen( dir );
        if( dirlen > 1 && dir[ dirlen-1 ] == '\\' ) {
            dir[ dirlen-1 ] = '\0';
        }
    }
}

static void splitref( FullName& s, const char* f )
{
    if( f != NULL ) {
        _splitpath( f, s.drive, s.dir, s.fname, s.ext );
    } else {
        char cwd[ _MAX_PATH + 1 ];
        getcwd( cwd, sizeof( cwd ) );
        int icount = strlen( cwd );
//        for( int i=0; i<icount; i++ ) {
//            cwd[i] = (char)tolower( cwd[i] );
//        }
        if( cwd[ icount-1 ] != SYS_DIR_SEP_CHAR ) {
            strcat( cwd, SYS_DIR_SEP_STR );
        }
        _splitpath( cwd, s.drive, s.dir, s.fname, s.ext );
    }
}

static FullName _x;

Define( WFileName )

WEXPORT WFileName::WFileName( const char* name )
    : WString( name )
{
}

WEXPORT WFileName::~WFileName()
{
}

#ifndef NOPERSIST
WFileName* WEXPORT WFileName::createSelf( WObjectFile& )
{
    return new WFileName();
}

void WEXPORT WFileName::readSelf( WObjectFile& p )
{
    WString::readSelf( p );
}

void WEXPORT WFileName::writeSelf( WObjectFile& p )
{
    WString::writeSelf( p );
}
#endif

WFileName& WFileName::operator=( const WFileName& f )
{
    WString::operator=( f );
    return *this;
}

void WEXPORT WFileName::merge( const char* name )
{
    _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    FullName    s;
    _splitpath( name, s.drive, s.dir, s.fname, s.ext );
    if( strchr( _x.fname, '*' ) ) {
        strcpy( _x.fname, s.fname );
    }
    if( strchr( _x.ext, '*' ) ) {
        strcpy( _x.ext, s.ext );
    }
    makepath( _x.path, _x.drive, _x.dir, _x.fname, _x.ext );
    *this = _x.path;
}

void WEXPORT WFileName::relativeTo( const char* f )
{
    int     i;

    _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    if( _x.dir[0] == SYS_DIR_SEP_CHAR ) {
        FullName        s;
        splitref( s, f );
        if( s.dir[0] == SYS_DIR_SEP_CHAR && strieq( s.drive, _x.drive ) ) {
            _x.drive[0] = '\0';
            int b = 0;
            for( i=1; _x.dir[i] != '\0' && s.dir[i] != '\0'; i++ ) {
                if( tolower( _x.dir[i] ) != tolower( s.dir[i] ) ) break;
                if( s.dir[i] == SYS_DIR_SEP_CHAR ) b = i;
            }
            if( b == 0 ) {
                strcpy( s.dir, _x.dir );
            } else {
                int n = 0;
                for( ; s.dir[i] != '\0'; i++ ) {
                    if( s.dir[i] == SYS_DIR_SEP_CHAR )  n++;
                }
                s.dir[0] = '\0';
                if( n > 0 ) {
                    for( int j=0; j<n; j++ ) {
                        strcpy( &s.dir[3*j], ".." SYS_DIR_SEP_STR );
                    }
                }
                strcpy( &s.dir[3*n], &_x.dir[ b+1 ] );
            }
            makepath( _x.path, _x.drive, s.dir, _x.fname, _x.ext );
            *this = _x.path;
        }
    }
}

void WEXPORT WFileName::absoluteTo( const char* f )
{
//
    int icount = size();
    int i, j, k;

    for( i=0; i<icount; i++ ) {
        if( strncmp( &(*this)[i], "$(", 2 ) == 0 ) {
            return;
        }
    }
//
    relativeTo( f );
    FullName    s;
    splitref( s, f );
    _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    if( _x.drive[0] == '\0' ) {
        strcpy( _x.drive, s.drive );
    }
    if( _x.dir[0] == SYS_DIR_SEP_CHAR ) {
        strcpy( s.dir, _x.dir );
    } else if( _x.dir[0] == '.' ) {
        for( i=0; strnicmp( &_x.dir[i], ".." SYS_DIR_SEP_STR, 3 )==0; i += 3 );
        int slen = strlen( s.dir );
        if( slen > 0 && s.dir[ slen-1 ] == SYS_DIR_SEP_CHAR ) {
            s.dir[ slen-1 ] = '\0';
        }
        for( j=0; j < i; j += 3 ) {
            for( k=strlen( s.dir ); k>0; k-- ) {
                if( s.dir[k] == SYS_DIR_SEP_CHAR ) break;
            }
            s.dir[k] = '\0';
        }
        strcat( s.dir, SYS_DIR_SEP_STR );
        strcat( s.dir, &_x.dir[i] );
    } else {
        strcat( s.dir, _x.dir );
    }
    makepath( _x.path, _x.drive, s.dir, _x.fname, _x.ext );
    *this = _x.path;
}

#ifdef __UNIX__
bool WEXPORT WFileName::setCWD() const
{
    splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    if( strlen( _x.dir ) > 0 ) {
        return( chdir( _x.dir ) == 0 );
    }
    return TRUE;
}
#else
bool WEXPORT WFileName::setCWD() const
{
    splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    unsigned olddrive;
    if( setdrive( _x.drive, &olddrive ) ) {
        if( strlen( _x.dir ) > 0 ) {
            int ret = chdir( _x.dir );
            if( ret == 0 ) {
                return TRUE;
            }
            unsigned total;
            _dos_setdrive( olddrive, &total );
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}
#endif

void WEXPORT WFileName::getCWD( bool slash )
{
    getcwd( _x.path, sizeof( _x.path ) );
//    int icount = strlen( _x.path );
//    for( int i=0; i<icount; i++ ) {
//        _x.path[i] = (char)tolower( _x.path[i] );
//    }
    if( slash ) {
        strcat( _x.path, SYS_DIR_SEP_STR );
    }
    *this = _x.path;
}

#ifdef __UNIX__
bool WEXPORT WFileName::makeDir() const
{
    splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    if( strlen( _x.dir ) > 0 ) {
        return mkdir( _x.dir, 0755 ) == 0;
    }
    return TRUE;
}
#else
bool WEXPORT WFileName::makeDir() const
{
    splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    if( strlen( _x.dir ) > 0 ) {
        unsigned olddrive;
        if( setdrive( _x.drive, &olddrive ) ) {
            int ret = mkdir( _x.dir );
            unsigned total;
            _dos_setdrive( olddrive, &total );
            return ret == 0;
        }
        return FALSE;
    }
    return TRUE;
}
#endif

bool WEXPORT WFileName::dirExists() const
{
    WFileName t;
    t.getCWD();
    if( setCWD() ) {
        t.setCWD();
        return TRUE;
    }
    return FALSE;
}

#ifdef __UNIX__
bool WEXPORT WFileName::attribs( char* attribs ) const
{
    /* XXX needs to be fixed: just to get it going */
    struct stat st;
    if (attribs != NULL ) {
        *attribs = 0;
    }
    return( stat( *this, &st ) == 0 );
}
#else
bool WEXPORT WFileName::attribs( char* attribs ) const
{
    struct find_t fileinfo;
    #define FIND_STYLE _A_NORMAL
    int rc = _dos_findfirst( *this, FIND_STYLE, &fileinfo );
    if( rc == 0 ) {
        if( attribs != NULL ) {
            *attribs = fileinfo.attrib;
        }
    }
    #undef FIND_STYLE
    #ifndef __WINDOWS__
    _dos_findclose( &fileinfo );
    #endif
    return rc == 0;
}
#endif

void WEXPORT WFileName::touch( time_t tm ) const
{
    if( tm == 0 ) {
        struct tm t;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_mday = 1;
        t.tm_mon = 0;
        t.tm_year = 80;
        t.tm_wday = 0;
        t.tm_yday = 0;
        t.tm_isdst = 0;
        tm = mktime( &t );
    }
    struct utimbuf ut;
    ut.actime = 0;
    ut.modtime = tm;
    utime( *this, &ut );
}

void WEXPORT WFileName::setDrive( const char* drive )
{
    _splitpath( *this, NULL, _x.dir, _x.fname, _x.ext );
    makepath( _x.path, drive, _x.dir, _x.fname, _x.ext );
    *this = _x.path;
}

const char* WEXPORT WFileName::drive() const
{
    _splitpath( *this, _x.drive, NULL, NULL, NULL );
    return _x.drive;
}

void WEXPORT WFileName::setDir( const char* dir )
{
    _splitpath( *this, _x.drive, NULL, _x.fname, _x.ext );
    makepath( _x.path, _x.drive, dir, _x.fname, _x.ext );
    *this = _x.path;
}

const char* WEXPORT WFileName::dir( bool slash ) const
{
    if( slash ) {
        _splitpath( *this, NULL, _x.dir, NULL, NULL );
    } else {
        splitpath( *this, NULL, _x.dir, NULL, NULL );
    }
    return _x.dir;
}

void WEXPORT WFileName::setFName( const char* fName )
{
    _splitpath( *this, _x.drive, _x.dir, NULL, _x.ext );
    makepath( _x.path, _x.drive, _x.dir, fName, _x.ext );
    *this = _x.path;
}

const char* WEXPORT WFileName::fName() const
{
    _splitpath( *this, NULL, NULL, _x.fname, NULL );
    return _x.fname;
}

void WEXPORT WFileName::setExt( const char* ext )
{
    _splitpath( *this, _x.drive, _x.dir, _x.fname, NULL );
    makepath( _x.path, _x.drive, _x.dir, _x.fname, ext );
    *this = _x.path;
}

const char* WEXPORT WFileName::ext() const
{
    _splitpath( *this, NULL, NULL, NULL, _x.ext );
    return _x.ext;
}

static const char legalChars[] = { "_^$~!#%&-{}()@`'." };
static bool isSpecialChar( char ch ) {
    char const        *ptr;

    ptr = legalChars;
    while( *ptr != '\0' ) {
        if( *ptr == ch ) return( TRUE );
        ptr++;
    }
    return( FALSE );
}

static const char illegalChars[] = { "\/:*?\"<>|" };
static bool isIllegalChar( char ch ) {
    char const        *ptr;

    ptr = illegalChars;
    while( *ptr != '\0' ) {
        if( *ptr == ch ) return( TRUE );
        ptr++;
    }
    return( FALSE );
}

static bool isLongName( char* fname )
{
    int len = strlen( fname );
    if( len > 0 ) {
        for( int i=0; i<len; i++ ) {
            char ch = fname[i];
            if( !isalnum( ch ) && !isSpecialChar( ch ) ) {
               return TRUE;
            }
        }
        return FALSE;
    }
    return FALSE;
}

static bool isLongDirName( char* dirNames )
{
    char* cpDirNames;
    char* aDirName;
    char* delims = { "\\/" };

    cpDirNames = strdup( dirNames );
    aDirName = strtok( cpDirNames, delims );
    while( aDirName != NULL ) {
        if( isLongName( aDirName ) ) {
            return TRUE;
        } else {
            aDirName = strtok( NULL, delims );
        }
    }
    return FALSE;
}

bool WEXPORT WFileName::needQuotes( char ch ) const
{
    if( !isMask() ) {
        int len = size();
        if( len > 0 && ( (*this)[0] != ch || (*this)[ len-1 ] != ch ) ) {
            _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
            if( isLongDirName( _x.dir ) || isLongName( _x.fname ) ) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void WEXPORT WFileName::removeQuotes( char ch )
{
    int len = size()-1;
    if( (*this)[0] == ch && (*this)[len] == ch ) {
        deleteChar( len );
        deleteChar( 0 );
    }
}

void WEXPORT WFileName::addQuotes( char ch )
{
    int len = size();
    char* quotedName = new char[ len + 3 ];
    quotedName[0] = ch;
    for( int i=0; i<len; i++ ) {
        quotedName[ i+1 ] = (*this)[i];
    }
    quotedName[ len+1 ] = ch;
    quotedName[ len+2 ] = '\0';
    (*this) = quotedName;
    delete [] quotedName;
}

bool WEXPORT WFileName::legal() const
{
    if( !isMask() ) {
        _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
        bool isLong = needQuotes();
        int len = strlen( _x.fname );
        if( len > 0 ) {
            for( int i=0; i<len; i++ ) {
                char ch = _x.fname[i];
                if( isLong ) {
                    if( isIllegalChar( ch ) ) {
                        return FALSE;
                    }
                } else {
                    if( !isalnum( ch ) && !isSpecialChar( ch ) ) {
                        return FALSE;
                    }
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

bool WEXPORT WFileName::match( const char* mask, char ctrlFlags ) const
{
    if( mask == NULL ) mask = "";
    _splitpath( *this, _x.drive, _x.dir, _x.fname, _x.ext );
    FullName    m;
    _splitpath( mask, m.drive, m.dir, m.fname, m.ext );
    bool ok = TRUE;
    if( ok && (ctrlFlags&matchDrive) ) {
        WString d( _x.drive );
        ok = ok & d.match( m.drive );
    }
    if( ok && (ctrlFlags&matchDir) ) {
        WString d( _x.dir );
        ok = ok & d.match( m.dir );
    }
    if( ok && (ctrlFlags&matchFName) ) {
        WString d( _x.fname );
        ok = ok & d.match( m.fname );
    }
    if( ok && (ctrlFlags&matchExt) ) {
        WString d( _x.ext );
        ok = ok & d.match( m.ext );
    }
    return ok;
}

void WEXPORT WFileName::fullName( WFileName& f ) const
{
    char buff[ _MAX_PATH + 1 ];
    buff[0] = '\0';
    _fullpath( buff, *this, _MAX_PATH );
    f = buff;
}

void WEXPORT WFileName::noExt( WFileName& f ) const
{
    _splitpath( *this, _x.drive, _x.dir, _x.fname, NULL );
    makepath( _x.path, _x.drive, _x.dir, _x.fname, NULL );
    f = _x.path;
}

void WEXPORT WFileName::noPath( WFileName& f ) const
{
    _splitpath( *this, NULL, NULL, _x.fname, _x.ext );
    makepath( _x.path, NULL, NULL, _x.fname, _x.ext );
    f = _x.path;
}

void WEXPORT WFileName::path( WFileName& f, bool slash ) const
{
    _splitpath( *this, _x.drive, _x.dir, NULL, NULL );
    makepath( _x.path, _x.drive, _x.dir, NULL, NULL );
    if( !slash ) {
        int len = strlen( _x.path );
        if( len > 0 && _x.path[ len-1 ] == '\\' ) {
            _x.path[ len-1 ] = '\0';
        }
    }
    f = _x.path;
}

bool WEXPORT WFileName::removeFile() const
{
    return remove( *this ) == 0;
}

bool WEXPORT WFileName::renameFile( const char* newname ) const
{
    if( rename( *this, newname ) == 0 ) {
        return TRUE;
    }
    return FALSE;
}

