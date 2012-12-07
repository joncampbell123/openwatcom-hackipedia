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
* Description:  Local and remote file access routines.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbglit.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "dbgtoggl.h"
#include "trpfile.h"
#include "dui.h"
#include <string.h>

extern int              DUIEnvLkup( char *, char *, int );
extern char             *StrCopy( char const *, char * );
extern void             FreeRing( char_ring * );
extern unsigned         RemoteStringToFullName( bool, char *, char *, unsigned );
extern void             StartupErr( char * );
extern bool             HaveRemoteFiles( void );

extern unsigned         RemoteErase( char const * );
extern void             RemoteErrMsg( sys_error, char * );
extern unsigned         RemoteRead( sys_handle, void *, unsigned );
extern unsigned         RemoteWrite( sys_handle, const void *, unsigned );
extern unsigned long    RemoteSeek( sys_handle, unsigned long, seek_method );
extern sys_handle       RemoteOpen( char const *, open_access );
extern unsigned         RemoteClose( sys_handle );
extern unsigned         RemoteWriteConsole( void *, unsigned );

extern unsigned         LocalErase( char const * );
extern void             LocalErrMsg( sys_error, char * );
extern unsigned         LocalRead( sys_handle, void *, unsigned );
extern unsigned         LocalWrite( sys_handle, const void *, unsigned );
extern unsigned long    LocalSeek( sys_handle, unsigned long, seek_method );
extern sys_handle       LocalOpen( char const *, open_access );
extern unsigned         LocalClose( sys_handle );
extern sys_handle       LocalHandle( handle );
extern void             DUIWndUser( void );


extern file_components  RemFile;
extern file_components  LclFile;
extern char             LclPathSep;


static char_ring *LclPath;


#define LOC_ESCAPE      '@'
#define REMOTE_LOC      'r'
#define LOCAL_LOC       'l'
#define REMOTE_IND      0x8000
#define MAX_OPENS       100
#define MAX_ERRORS      10


static sys_handle       SysHandles[MAX_OPENS];
static sys_error        SysErrors[MAX_ERRORS];
static unsigned         ErrRover;
static unsigned         LastErr;

handle PathOpen( char const *name, unsigned len, char *ext );

char  *RealFName( char const *name, open_access *loc )
{
    *loc &= ~(OP_REMOTE|OP_LOCAL);
    if( name[0] == LOC_ESCAPE ) {
        if( (name[1] | 0x20) == REMOTE_LOC ) {
            *loc |= OP_REMOTE;
            name += 2;
        } else if( (name[1] | 0x20) == LOCAL_LOC ) {
            *loc |= OP_LOCAL;
            name += 2;
        } else if( name[1] == LOC_ESCAPE ) {
            name += 1;
        }
    }
    return( (char*)name );
}

static open_access DefaultLoc( open_access loc )
{
    if( (loc & (OP_REMOTE|OP_LOCAL) ) == 0 ) {
        if( _IsOn( SW_REMOTE_FILES ) ) {
            loc |= OP_REMOTE;
        } else {
            loc |= OP_LOCAL;
        }
    }
    if( (loc & OP_REMOTE) && !HaveRemoteFiles() ) {
        loc &= ~OP_REMOTE;
        loc |= OP_LOCAL;
    }
    return( loc );
}

char    *FileLoc( char const *name, open_access *loc )
{
    open_access ind;

    ind = 0;
    name = RealFName( name, &ind );
    if( ind != 0 ) {
        *loc &= ~(OP_LOCAL|OP_REMOTE);
        *loc |= ind;
    }
    *loc = DefaultLoc( *loc );
    return( (char*)name );
}


static file_components *PathInfo( char const *path, open_access loc )
{
    file_components     *info;

    FileLoc( path, &loc );
    if( loc & OP_LOCAL ) {
        info = &LclFile;
    } else {
        info = &RemFile;
    }
    return( info );
}

static handle FindFreeHandle( void )
{
    handle      i;

    for( i = 0; i < MAX_OPENS; ++i ) {
        if( SysHandles[i] == NIL_SYS_HANDLE ) return( i );
    }
    return( NIL_HANDLE );
}

unsigned ReadStream( handle h, void *b, unsigned l )
{
    sys_handle  sys     = SysHandles[ h & ~REMOTE_IND ];

    if( h & REMOTE_IND ) {
        return( RemoteRead( sys, b, l ) );
    } else {
        return( LocalRead( sys, b, l ) );
    }
}

unsigned ReadText( handle h, void *b, unsigned l )
{
    return( ReadStream( h, b, l ) );
}

unsigned WriteStream( handle h, const void *b, unsigned l)
{
    sys_handle  sys     = SysHandles[ h & ~REMOTE_IND ];

    if( h & REMOTE_IND ) {
        return( RemoteWrite( sys, b, l ) );
    } else {
        return( LocalWrite( sys, b, l ) );
    }
}

unsigned WriteText( handle h, const void *b, unsigned len )
{
    char    *nl;

    len = WriteStream( h, b, len );
    if( h & REMOTE_IND ) {
        nl = RemFile.newline;
    } else {
        nl = LclFile.newline;
    }
    WriteStream( h, nl, (nl[1] != NULLCHAR) ? 2 : 1 );
    return( len );   /* not including the newline sequence */
}

unsigned long SeekStream( handle h, long p, seek_method m )
{
    sys_handle  sys     = SysHandles[ h & ~REMOTE_IND ];

    if( h & REMOTE_IND ) {
        return( RemoteSeek( sys, p, m ) );
    } else {
        return( LocalSeek( sys, p, m ) );
    }
}

handle FileOpen( char const *name, open_access o )
{
    sys_handle  sys;
    handle      h;

    if( o & OP_SEARCH ) {
        return( PathOpen( name, strlen( name ), "" ) );
    }
    name = FileLoc( name, &o );
    h = FindFreeHandle();
    if( h == NIL_HANDLE ) return( NIL_HANDLE );
    if( o & OP_REMOTE ) {
        h |= REMOTE_IND;
        sys = RemoteOpen( name, o );
    } else {
        sys = LocalOpen( name, o );
    }
    if( sys == NIL_SYS_HANDLE ) return( NIL_HANDLE );
    SysHandles[ h & ~REMOTE_IND ] = sys;
    if( o & OP_APPEND ) SeekStream( h, 0, DIO_SEEK_END );
    return( h );
}

unsigned FileClose( handle h )
{
    sys_handle  sys     = SysHandles[ h & ~REMOTE_IND ];

    SysHandles[ h & ~REMOTE_IND ] = NIL_SYS_HANDLE;
    if( h & REMOTE_IND ) {
        return( RemoteClose( sys ) );
    } else {
        return( LocalClose( sys ) );
    }
}


unsigned FileRemove( char const *name, open_access loc )
{
    name = FileLoc( name, &loc );
    if( loc & OP_REMOTE ) {
        return( RemoteErase( name ) );
    } else {
        return( LocalErase( name ) );
    }
}

void WriteToPgmScreen( void *buff, unsigned len )
{
#if !defined( BUILD_RFX )
    DUIWndUser();
#endif
    RemoteWriteConsole( buff, len );
}

open_access FileHandleInfo( handle h )
{
    if( h & REMOTE_IND ) return( OP_REMOTE );
    return( OP_LOCAL );
}

char *SysErrMsg( unsigned code, char *buff )
{
    sys_error   sys = SysErrors[ (code & ~REMOTE_IND) - 1];

    if( code & REMOTE_IND ) {
        RemoteErrMsg( sys, buff );
    } else {
        LocalErrMsg( sys, buff );
    }
    return( &buff[ strlen( buff ) ] );
}

unsigned StashErrCode( sys_error sys, open_access loc )
{
    unsigned    code;

    if( sys == 0 ) return( 0 );
    if( ++ErrRover >= MAX_ERRORS ) ErrRover = 0;
    code = ErrRover;
    SysErrors[code] = sys;
    ++code;
    if( loc & OP_REMOTE ) code |= REMOTE_IND;
    LastErr = code;
    return( code );
}

/* for RFX */
unsigned GetLastErr( void )
{
    return( LastErr );
}

/* for RFX */
sys_error GetSystemErrCode( unsigned code )
{
    if( code == 0 ) return( 0 );
    return( SysErrors[ (code & ~REMOTE_IND) - 1] );
}

/* for RFX */
sys_handle GetSystemHandle( unsigned h )
{
    return( SysHandles[ h & ~REMOTE_IND ] );
}

bool IsAbsolutePath( char *path )
{
    file_components     *info;
    char                *p;
    open_access         loc;

    p = RealFName( path, &loc );
    info = PathInfo( p, loc );
    if( strlen( p ) == 0 ) return( FALSE );
    return( p[0] == info->path_separator[0]
         || p[0] == info->path_separator[1]
         || p[0] == info->path_separator[2] );
}

char *AppendPathDelim( char *path, open_access loc )
{
    file_components     *info;
    unsigned            len;
    char                *end;

    info = PathInfo( path, loc );
    len = strlen( path );
    end = &path[len];
    if( len == 0 ||
        (   end[-1] != info->path_separator[0]
         && end[-1] != info->path_separator[1]
         && end[-1] != info->path_separator[2] ) ) {
        *end++ = info->path_separator[0];
    }
    return( end );
}

char  *SkipPathInfo( char const *path, open_access loc )
{
    file_components     *info;
    char      const     *name;
    char                c;

    name = path;
    info = PathInfo( path, loc );
    for( ;; ) {
        c = *path++;
        if( c == NULLCHAR ) break;
        if( c == info->path_separator[0]
         || c == info->path_separator[1]
         || c == info->path_separator[2] ) {
            name = path;
        }
    }
    return( (char*)name );
}


char  *ExtPointer( char const *path, open_access loc )
{
    file_components     *info;
    char                *p,*end;
    char                c;

    end = (char *)path + strlen( path );
    info = PathInfo( path, loc );
    p = end;
    for( ;; ) {
        c = *--p;
        if( p < path ) return( end );
        if( c == info->path_separator[0]
         || c == info->path_separator[1]
         || c == info->path_separator[2] ) return( end );
        if( c == info->ext_separator ) return( p );
    }
}


unsigned MakeFileName( char *result, char *name, char *ext, open_access loc )
{
    file_components     *info;
    char                *p;

    p = StrCopy( name, result );
    if( *ExtPointer( result, loc ) == NULLCHAR ) {
        info = PathInfo( name, loc );
        *p++ = info->ext_separator;
        p = StrCopy( ext, p );
    }
    return( p - result );
}

static unsigned MakeNameWithPath( open_access loc,
                                char *path, unsigned plen,
                                char const *name, unsigned nlen, char *res )
{
    file_components     *info;
    char                *p;

    p = res;
    if( loc & OP_REMOTE ) {
        *p++ = LOC_ESCAPE;
        *p++ = REMOTE_LOC;
    } else if( loc & OP_LOCAL ) {
        *p++ = LOC_ESCAPE;
        *p++ = LOCAL_LOC;
    } else {
        loc = DefaultLoc( loc );
    }
    if( path != NULL ) {
        memcpy( p, path, plen );
        p += plen;
        if( loc & OP_LOCAL ) {
            info = &LclFile;
        } else {
            info = &RemFile;
        }
        if( plen > 0 &&
            p[-1] != LclFile.path_separator[0] &&
            p[-1] != LclFile.path_separator[1] &&
            p[-1] != LclFile.path_separator[2] ) {
            p[0] = LclFile.path_separator[0];
            ++p;
        }
    }
    memcpy( p, name, nlen );
    p += nlen;
    *p = NULLCHAR;
    return( p - res );
}


handle LclStringToFullName( char *name, unsigned len, char *full )
{
    char_ring   *curr;
    handle      hndl;
    int         plen;

    MakeNameWithPath( OP_LOCAL, NULL, 0, name, len, full );
    curr = LclPath;
    for( ;; ) {
        hndl = FileOpen( full, OP_READ );
        if( hndl != NIL_HANDLE ) return( hndl );
        if( curr == NULL ) return( NIL_HANDLE );
        plen = curr->name[0];
        MakeNameWithPath( OP_LOCAL, &curr->name[1], plen, name, len, full );
        curr = curr->next;
    }
}


/*
 *
 */
static handle FullPathOpenInternal( char const *name, char *ext, char *result,
                                    unsigned max_result, bool force_local )
{
    char        buffer[TXT_LEN];
    char        *p;
    open_access loc;
    bool        have_ext;
    bool        have_path;
    file_components *file;
    handle      f;

    loc = 0;
    p = FileLoc( name, &loc );
    name = p;
    have_ext = FALSE;
    have_path = FALSE;
    if( force_local ) {
        loc &= ~OP_REMOTE;
        loc |= OP_LOCAL;
    }
    if( loc & OP_LOCAL ) {
        file = &LclFile;
    } else {
        file = &RemFile;
    }
    p = buffer;
    while( *name != '\0' ) {
        *p = *name;
        if( *p == file->path_separator[0] ||
            *p == file->path_separator[1] ||
            *p == file->path_separator[2] ) {
            have_ext = FALSE;
            have_path = TRUE;
        } else if( *p == file->ext_separator ) {
            have_ext = TRUE;
        }
        ++p;
        ++name;
    }
    *p = *name;
    if( !have_ext ) {
        *p++ = file->ext_separator;
        p = StrCopy( ext, p );
    }
    *p = NULLCHAR;
    if( loc & OP_REMOTE ) {
        RemoteStringToFullName( FALSE, buffer, result, max_result );
        f = FileOpen( result, OP_READ | OP_REMOTE );
    } else if( have_path ) {
        StrCopy( buffer, result );
        f = FileOpen( buffer, OP_READ );
    } else {
        f = LclStringToFullName( buffer, p - buffer, result );
    }
    if( f == NIL_HANDLE ) {
        strcpy( result, buffer );
    } else {
        p = RealFName( result, &loc );
        memmove( result, p, strlen( p ) + 1 );
    }
    return( f );
}

handle FullPathOpen( char const *name, char *ext, char *result, unsigned max_result )
{
    return( FullPathOpenInternal( name, ext, result, max_result, FALSE ) );
}

handle LocalFullPathOpen( char const *name, char *ext, char *result, unsigned max_result )
{
    return( FullPathOpenInternal( name, ext, result, max_result, TRUE ) );
}

#if !defined( BUILD_RFX )
static bool IsWritable( char const *name, open_access loc )
{
    handle      h;

    h = FileOpen( name, OP_READ | loc );
    if( h == NIL_HANDLE ) {
        h = FileOpen( name, OP_WRITE | OP_CREATE | loc );
        if( h != NIL_HANDLE ) {
            FileClose( h );
            FileRemove( name, loc );
            return( TRUE );
        }
    } else {
        FileClose( h );
        h = FileOpen( name, OP_WRITE | loc );
        if( h != NIL_HANDLE ) {
            FileClose( h );
            return( TRUE );
        }
    }
    return( FALSE );
}

bool FindWritable( char const *src, char *dst )
{
    char        buffer[TXT_LEN];
    unsigned    plen;
    unsigned    nlen;
    char const *name;
    open_access loc;

    loc = 0;
    src = RealFName( src, &loc );
    if( IsWritable( src, loc ) ) {
        StrCopy( src, dst );
        return( TRUE );
    }
    name = SkipPathInfo( src, loc );
    nlen = strlen( name );
    if( DefaultLoc( loc ) & OP_LOCAL ) {
        plen = DUIEnvLkup( "HOME", buffer, sizeof( buffer ) );
        if( plen > 0 ) {
            MakeNameWithPath( loc, buffer, plen, name, nlen, dst );
            if( IsWritable( dst, loc ) ) return( TRUE );
        }
    }
    MakeNameWithPath( loc, NULL, 0, name, nlen, dst );
    return( IsWritable( dst, loc ) );
}
#endif

static handle PathOpenInternal( char const *name, unsigned len, char *ext, bool force_local )
{
    char        result[TXT_LEN];
    char        *p;

    _AllocA( p, len + 1 );
    memcpy( p, name, len );
    p[ len ] = '\0';
    if( force_local )
        return( LocalFullPathOpen( p, ext, result, TXT_LEN ) );
    else
        return( FullPathOpen( p, ext, result, TXT_LEN ) );
}

handle PathOpen( char const *name, unsigned len, char *ext )
{
    return( PathOpenInternal( name, len, ext, FALSE ) );
}

handle LocalPathOpen( char const *name, unsigned len, char *ext )
{
    return( PathOpenInternal( name, len, ext, TRUE ) );
}

void SysFileInit( void )
{
    unsigned    i;

    for( i = 0; i < MAX_OPENS; ++i ) {
        SysHandles[i] = NIL_SYS_HANDLE;
    }
    SysHandles[STD_IN ] = LocalHandle( STD_IN  );
    SysHandles[STD_OUT] = LocalHandle( STD_OUT );
    SysHandles[STD_ERR] = LocalHandle( STD_ERR );
}

#if !defined( BUILD_RFX )

void PathFini( void )
{
    FreeRing( LclPath );
}

/*
 * EnvParse -- parse environment string into separate pieces
 */

static void EnvParse( char_ring **owner, char *src )
{
    char       *start, *end;
    int         len;
    char_ring  *new;

    for( start = end = src;; ++end ) {
        if( *end == LclPathSep || *end == '\0' ) {
            while( *start == ' ' ) {
                ++start;
            }
            len = end - start;
            while( len > 0 && start[len-1] == ' ' ) {
                --len;
            }
            _Alloc( new, sizeof( char_ring ) + len );
            if( new == NULL ) break;
            *owner = new;
            owner = &new->next;
            new->next = NULL;
            new->name[0] = len;
            memcpy( &new->name[1], start, len );
            if( *end == NULLCHAR ) return;
            start = end + 1;
        }
    }
}
#endif

void PathInit( void )
{
#if !defined( BUILD_RFX )
    #define BSIZE 2048
    char        *buff;

    _Alloc( buff, BSIZE ); /* allocate enough room for a very long PATH */
    if( buff == NULL ) {
        StartupErr( LIT( ERR_NO_MEMORY ) );
    }
    if( DUIEnvLkup( "PATH", buff, BSIZE ) != 0 ) {
        EnvParse( &LclPath, buff );
    }
    _Free( buff );
#endif
}
