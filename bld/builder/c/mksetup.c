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
* Description:  Utility to create setup.inf files for Watcom installer.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "diskos.h"
#ifdef __UNIX__
    #include <unistd.h>
#else
    #include <direct.h>
    #include <dos.h>
#endif

#define RoundUp( size, limit )  ( ( ( size + limit - 1 ) / limit ) * limit )


enum {
    FALSE, TRUE
};


typedef struct path_info {
    char                *path;
    int                 target;
    int                 parent;
    struct path_info    *next;
} PATH_INFO;

typedef struct size_list size_list;

struct size_list {
    size_list   *next;
    long        size;
    long        stamp;
    char        type;
    char        redist;
    char        *dst_var;
    char        name[1];
};

typedef struct file_info {
//    char                *file;
    char                *pack;
    char                *condition;
    int                 path;
    int                 old_path;
    int                 num_files;
    size_list           *sizes;
    struct file_info    *next;
} FILE_INFO;


typedef struct list {
    char                *item;
    struct list         *next;
    int                 type;
} LIST;

enum {
    DELETE_DIALOG,
    DELETE_FILE,
    DELETE_DIR
};

static char                 *Product;
static long                 DiskSize;
static int                  BlockSize;
static char                 *RelRoot;
static char                 *Setup;
static FILE_INFO            *FileList = NULL;
static PATH_INFO            *PathList = NULL;
static LIST                 *AppSection = NULL;
static LIST                 *IconList = NULL;
static LIST                 *SupplimentList = NULL;
static LIST                 *IniList = NULL;
static LIST                 *AutoList = NULL;
static LIST                 *CfgList = NULL;
static LIST                 *EnvList = NULL;
static LIST                 *DialogList = NULL;
static LIST                 *BootTextList = NULL;
static LIST                 *ExeList = NULL;
static LIST                 *TargetList = NULL;
static LIST                 *LabelList = NULL;
static LIST                 *UpgradeList = NULL;
static LIST                 *AutoSetList = NULL;
static LIST                 *AfterList = NULL;
static LIST                 *BeforeList = NULL;
static LIST                 *EndList = NULL;
static LIST                 *DeleteList = NULL;
static LIST                 *ForceDLLInstallList = NULL;
static LIST                 *AssociationList = NULL;
static LIST                 *ErrMsgList = NULL;
static LIST                 *SetupErrMsgList = NULL;
static unsigned             MaxDiskFiles;
static int                  FillFirst = 1;
static int                  Lang = 1;
static int                  Upgrade = FALSE;
static int                  Verbose = FALSE;
static int                  IgnoreMissingFiles = FALSE;
static int                  CreateMissingFiles = FALSE;
static char                 *Include;
static const char           MksetupInf[] = "mksetup.inf";


static char *mygets( char *buf, unsigned len, FILE *fp )
/******************************************************/
{
    char        *p,*q,*start;
    int         lang;
    unsigned    got;

    /* syntax is //nstring//mstring//0 */

    p = buf;
    for( ;; ) {
        if( fgets( p, len, fp ) == NULL ) return( NULL );
        q = p;
        while( *q == ' ' || *q == '\t' ) ++q;
        if( p != q ) strcpy( p, q );
        got = strlen( p );
        if( got <= 1 ) break;
        got-=2;
        if( p[got] != '\\' || p[got+1] != '\n' ) break;
        p += got;
        len -= got;
    }
    p = buf;
    while( *p ) {
        if( p[0] == '/' && p[1] == '/' && isdigit( p[2] ) ) {
            start = p;
            lang = p[2] - '0';
            if( lang == 0 ) {
                strcpy( start, start+3 );
                continue;
            }
            p += 3;
            while( p[0] != '/' || p[1] != '/' ) {
                ++p;
            }
            if( lang == Lang ) {
                strcpy( start, start+3 );
                p -= 3;
            } else {
                strcpy( start, p );
                p = start;
            }
        } else {
            ++p;
        }
    }
    return( buf );
}


static void AddToList( LIST *new, LIST **list )
/*********************************************/
{
    while( *list != NULL ) {
        list = &((*list)->next);
    }
    *list = new;
}


static long FileSize( char *file )
/********************************/
{
    struct stat         stat_buf;

    if( (file == NULL) || (stat( file, &stat_buf ) != 0) ) {
        printf( "Can't find '%s'\n", file );
        return( 0 );
    } else {
        return( RoundUp( stat_buf.st_size, BlockSize ) );
    }
}


int CheckParms( int *pargc, char **pargv[] )
/******************************************/
{
    struct stat         stat_buf;
    char                **argv;
    int                 argc;
    LIST                *new;

    FillFirst = 1;
    if( *pargc > 1 ) {
        while( ((*pargv)[1] != NULL) && ((*pargv)[1][0] == '-') ) {
            if( (*pargv)[1][1] == '0' ) {
                FillFirst = 0;
            } else if( tolower( (*pargv)[1][1] ) == 'l' ) {
                Lang = (*pargv)[1][2] - '0';
            } else if( tolower( (*pargv)[1][1] ) == 'd' ) {
                new = malloc( sizeof( LIST ) );
                if( new == NULL ) {
                    printf( "\nOut of memory\n" );
                    exit( 1 );
                }
                new->next = NULL;
                new->item = strdup( &(*pargv)[1][2] );
                AddToList( new, &AppSection );
            } else if( tolower( (*pargv)[1][1] ) == 'i' ) {
                Include = (*pargv)[1]+2;
            } else if( tolower( (*pargv)[1][1] ) == 'u' ) {
                Upgrade = TRUE;
            } else if( tolower( (*pargv)[1][1] ) == 'v' ) {
                Verbose = TRUE;
            } else if( tolower( (*pargv)[1][1] ) == 'f' ) {
                IgnoreMissingFiles = TRUE;
            } else if( tolower( (*pargv)[1][1] ) == 'x' ) {
                CreateMissingFiles = TRUE;
            } else {
                printf( "Unrecognized option %s\n", (*pargv)[1] );
            }
            ++*pargv;
            --*pargc;
        }
    }
    argc = *pargc;
    argv = *pargv;
    if( argc != 4 ) {
        printf( "Usage: mksetup [-options] <product> <file_list> <rel_root>\n\n" );
        printf( "Supported options (case insensitive):\n" );
        printf( "-v         verbose operation\n" );
        printf( "-i<path>   include path for setup scripts\n" );
        printf( "-u         create upgrade setup script\n" );
        printf( "-d<string> specify string to add to Application section\n" );
        printf( "-f         force script creation if files missing (testing only)\n" );
        printf( "-x         force creation of missing files (testing only)\n" );
        return( FALSE );
    }
    Product = argv[ 1 ];
    DiskSize = (1457664L-4096);
    MaxDiskFiles = 215;
    BlockSize = 512;

    RelRoot  = argv[ 3 ];
    if( stat( RelRoot, &stat_buf ) != 0 ) {  // exists
        printf( "\nDirectory '%s' does not exist\n", RelRoot );
        return( FALSE );
    }
    return( TRUE );
}


int AddTarget( char *target )
/***************************/
{
    int                 count;
    LIST                *new, *curr;

    count = 1;
    for( curr = TargetList; curr != NULL; curr = curr->next ) {
        if( stricmp( target, curr->item ) == 0 ) {
            return( count );
        }
        ++count;
    }

    new = malloc( sizeof( LIST ) );
    if( new == NULL ) {
        printf( "Out of memory\n" );
        return( 0 );
    } else {
        new->item = strdup( target );
        if( new->item == NULL ) {
            printf( "Out of memory\n" );
            return( 0 );
        }
        new->next = NULL;
        count = 1;
        if( TargetList == NULL ) {
            TargetList = new;
        } else {
            curr = TargetList;
            while( curr->next != NULL ) {
                curr = curr->next;
                ++count;
            }
            curr->next = new;
            ++count;
        }
        return( count );
    }
}


static char *GetBracketedString( const char *src, char **end )
/************************************************************/
{
    const char      *s = src;
    char            *ret;
    int             len;

    if( *s++ != '<' ) {
        return( NULL );
    }
    ret = strchr( s, '>' );
    if( ret == NULL ) {
        return( NULL );
    }
    len = ret - s;
    *end = (char *)src + len + 2;
    if( (ret = malloc( len + 1 )) == NULL ) {
        return( NULL );
    }
    strncpy( ret, s, len );
    ret[len] = '\0';
    return( ret );
}


int AddPath( char *path, int target, int parent )
/***********************************************/
{
    int                 count;
    PATH_INFO           *new, *curr;

    count = 1;
    for( curr = PathList; curr != NULL; curr = curr->next ) {
        if( stricmp( path, curr->path ) == 0 && target == curr->target ) {
            return( count );
        }
        ++count;
    }

    new = malloc( sizeof( PATH_INFO ) );
    if( new == NULL ) {
        printf( "Out of memory\n" );
        return( 0 );
    } else {
        new->path = strdup( path );
        if( new->path == NULL ) {
            printf( "Out of memory\n" );
            return( 0 );
        }
        new->target = target;
        new->parent = parent;
        new->next = NULL;
        count = 1;
        if( PathList == NULL ) {
            PathList = new;
        } else {
            curr = PathList;
            while( curr->next != NULL ) {
                curr = curr->next;
                ++count;
            }
            curr->next = new;
            ++count;
        }
        return( count );
    }
}


int AddPathTree( char *path, int target )
/***************************************/
{
    int         parent;
    char        *p;

    if( path == NULL ) return( -1 );
    parent = AddPath( ".", target, -1 );
    p = strchr( path, '/' );
    while( p != NULL ) {
        *p = '\0';
        parent = AddPath( path, target, parent );
        if( parent == 0 ) return( FALSE );
        *p = '/';
        p = strchr( p + 1, '/' );
    }
    return( AddPath( path, target, parent ) );
}

static int mkdir_nested( char *path )
/***********************************/
{
#ifdef __UNIX__
    struct stat sb;
#else
    unsigned    attr;
#endif
    char        pathname[ FILENAME_MAX ];
    char        *p;
    char        *end;

    p = pathname;
    strncpy( pathname, path, FILENAME_MAX );
    end = pathname + strlen( pathname );

#ifndef __UNIX__
    /* special case for drive letters */
    if( p[0] && p[1] == ':' ) {
        p += 2;
    }
#endif
    /* skip initial path separator if present */
    if( (p[0] == '/') || (p[0] == '\\') )
        ++p;

    /* find the next path component */
    while( p < end ) {
        while( (p < end) && (*p != '/') && (*p != '\\') )
            ++p;
        *p = '\0';

        /* check if pathname exists */
#ifdef __UNIX__
        if( stat( pathname, &sb ) == -1 ) {
#else
        if( _dos_getfileattr( pathname, &attr ) != 0 ) {
#endif
            int rc;

#ifdef __UNIX__
            rc = mkdir( pathname, S_IRWXU | S_IRWXG | S_IRWXO );
#else
            rc = mkdir( pathname );
#endif
            if( rc != 0 ) {
                printf( "Can not create directory '%s': %s\n", pathname, strerror( errno ) );
                return( -1 );
            }
        } else {
            /* make sure it really is a directory */
#ifdef __UNIX__
            if( !S_ISDIR( sb.st_mode ) ) {
#else
            if( (attr & _A_SUBDIR) == 0 ) {
#endif
                printf( "Can not create directory '%s': file with the same name already exists\n", pathname );
                return( -1 );
            }
        }
        /* put back the path separator - forward slash always works */
        *p++ = '/';
    }
    return( 0 );
}

int AddFile( char *path, char *old_path, char type, char redist, char *file, char *rel_file, char *dst_var, char *cond )
/***********************************************************************************************************/
{
    int                 path_dir, old_path_dir, target;
    FILE_INFO           *new, *curr;
    long                act_size;
    long                time;
    struct stat         stat_buf;
    char                *p;
    char                *root_file;
    char                *archive;
    char                src[ _MAX_PATH ];
    size_list           *ns,*sl;
    static char         archive_name[16] = "pck00000";
    static int          pack_num = 1;

    archive = archive_name;

    if( strcmp( rel_file, "." ) != 0 ) {
        if( strchr( rel_file, ':' ) != NULL
        ||  *rel_file == '\\'
        ||  *rel_file == '/' ) {
            strcpy( src, rel_file );
        } else {
            char        c;

            strcpy( src, RelRoot );
            c = src[ strlen( src ) - 1 ];
            if( (c != '/') && (c != '\\') ) {
                strcat( src, SYS_DIR_SEP_STR );
            }
            strcat( src, rel_file );
        }
    } else if( strchr( path, ':' ) != NULL ) {
        // path is absolute. don't use RelRoot
        strcpy( src, path );
        strcat( src, SYS_DIR_SEP_STR );
        strcat( src, file );
    } else {
        char    c;

        strcpy( src, RelRoot );
        c = src[ strlen( src ) - 1 ];
        if( (c != '\\') && (c != '/') ) {
            strcat( src, SYS_DIR_SEP_STR );
        }
        if( path[ 0 ] != '.' ) {
            strcat( src, path );
            strcat( src, SYS_DIR_SEP_STR );
        }
        strcat( src, file );
    }
    if( stat( src, &stat_buf ) != 0 ) {
        printf( "'%s' does not exist\n", src );
        if( IgnoreMissingFiles ) {
            act_size = 1024;
            time = 0;
        } else if( CreateMissingFiles ) {
            FILE    *fp;
            char    c;

            fp = fopen( src, "w" );
            if( fp == NULL ) {
                for( p = src + strlen( src ); p > src; p-- ) {
                    if( (*(p - 1) == '\\') || (*(p - 1) == '/') ) {
                        c = *p;
                        *p = '\0';
                        mkdir_nested( src );
                        *p = c;
                        break;
                    }
                }
                fp = fopen( src, "w" );
                if( fp == NULL ) {
                    printf( "Cannot create '%s'\n", src );
                    return( FALSE );
                }
            }
            fclose( fp );
            act_size = 0;
            time = 0;
        } else {
            return( FALSE );
        }
    } else {
        act_size = stat_buf.st_size;
        time = stat_buf.st_mtime;
    }
#if 0
    printf( "\r%s                              \r", file );
    fflush( stdout );
#endif
    act_size = RoundUp( act_size, 512 );

    // strip target off front of path
    if( *path == '%' ) {
        p = strchr( ++path, '%' );
        if( p == NULL ) {
            printf( "Invalid path(%s)\n", path );
            return( 0 );
        }
        *p = '\0';
        target = AddTarget( path );
        path = p + 1;
        if( *path == '\0' ) {
            path = ".";
        } else if( (*path == '\\') || (*path == '/') ) {
            ++path;
        } else {
            printf( "Invalid path (%s)\n", path );
            return( 0 );
        }
    } else {
        target = AddTarget( "DstDir" );
    }
    if( target == 0 ) {
        return( 0 );
    }

    // handle sub-directories in path before full path
    path_dir = AddPathTree( path, target );
    old_path_dir = AddPathTree( old_path, target );
    if( path_dir == 0 ) {
        return( FALSE );
    }
    root_file = p = file;
    while( *p != '\0' ) {
        switch( *p ) {
        case '/':
        case ':':
        case '\\':
            root_file = p + 1;
            break;
        }
        ++p;
    }

    // see if the pack_file has been seen already
    curr = FileList;
    while( curr != NULL ) {
        if( stricmp( curr->pack, archive ) == 0 ) {
            // this file is already in the current pack file

            // if the path differs, start a new pack file
            if( curr->path != path_dir ) {
                sprintf( archive_name, "pck%05d", pack_num++ );
                archive = strdup( archive_name );
                if( Verbose ) {
                    printf( "\nPath for archive '%s' changed to '%s'\n",
                        curr->pack, path );
                    printf( "Changing archive to '%s'\n", archive );
                }
                curr = curr->next;
                break;
            }
            // if the condition differs, start a new pack file
            if( strcmp( curr->condition, cond ) != 0 ) {
                sprintf( archive_name, "pck%05d", pack_num++ );
                archive = strdup( archive_name );
                if( Verbose ) {
                    printf( "\nCondition for archive '%s' changed:\n", curr->pack );
                    printf( "Old: <%s>\n", curr->condition );
                    printf( "New: <%s>\n", cond );
                    printf( "Changing archive to '%s'\n", archive );
                }
                curr = curr->next;
                break;
            }

            curr->num_files++;
            ns = malloc( sizeof( size_list ) + strlen( root_file ) );
            if( ns == NULL ) {
                printf( "Out of memory\n" );
                return( FALSE );
            }
            strcpy( ns->name, root_file );
            for( sl = curr->sizes; sl != NULL; sl = sl->next ) {
                if( stricmp( sl->name, ns->name ) == 0 ) {
                    printf( "file '%s' included in archive '%s' more than once\n", sl->name, curr->pack );
                    return( FALSE );
                }
            }
            ns->size = act_size;
            ns->stamp = time;
            ns->type = type;
            ns->redist = redist;
            ns->dst_var = dst_var;
            ns->next = curr->sizes;
            curr->sizes = ns;
            return( TRUE );
        }
        curr = curr->next;
    }

    // add to list
    new = malloc( sizeof( FILE_INFO ) );
    if( new == NULL ) {
        printf( "Out of memory\n" );
        return( FALSE );
    } else {
        new->pack = strdup( archive );
        new->condition = strdup( cond );
        if( new->pack == NULL || new->condition == NULL ) {
            printf( "Out of memory\n" );
            return( FALSE );
        }
        new->path = path_dir;
        new->old_path = old_path_dir;
        new->num_files = 1;
        ns = malloc( sizeof( size_list ) + strlen( root_file ) );
        if( ns == NULL ) {
            printf( "Out of memory\n" );
            return( FALSE );
        }
        strcpy( ns->name, root_file );
        ns->size = act_size;
        ns->stamp = time;
        ns->next = NULL;
        ns->type = type;
        ns->redist = redist;
        ns->dst_var = dst_var;
        new->sizes = ns;
        new->next = NULL;
        if( FileList == NULL ) {
            FileList = new;
        } else {
            curr = FileList;
            while( curr->next != NULL ) {
                curr = curr->next;
            }
            curr->next = new;
        }
        return( TRUE );
    }
}


int ReadList( FILE *fp )
/**********************/
{
    char        *p, *s;
    char        *path;
    char        *old_path;
    char        *file;
    char        *rel_fil;
    char        *condition;
    char        *desc;
    char        *dst_var;
    char        *where;
    char        buf[ 1024 ];
    char        redist;
    char        type;
    int         no_error;

    printf( "Checking files...\n" );
    no_error = TRUE;
    while( fgets( buf, sizeof( buf ), fp ) != NULL ) {
        s = GetBracketedString( buf, &p );
        if( s == NULL ) {
            printf( "Invalid list file format - 'type' not found\n" );
            exit( 2 );
        }
        type = s[0];
        free( s );
        s = GetBracketedString( p, &p );
        if( s == NULL ) {
            printf( "Invalid list file format - 'redist' not found\n" );
            exit( 2 );
        }
        redist = s[0];
        free( s );
        path = GetBracketedString( p, &p );
        if( path == NULL ) {
            printf( "Invalid list file format - 'dir' not found\n" );
            exit( 2 );
        }
        old_path = GetBracketedString( p, &p );
        if( old_path == NULL ) {
            printf( "Invalid list file format - 'old dir' not found\n" );
            exit( 2 );
        }
        if( (stricmp( path, old_path ) == 0) || (*old_path == '\0') ) {
            free( old_path );
            old_path = NULL;
        }
        file = GetBracketedString( p, &p );
        if( file == NULL ) {
            printf( "Invalid list file format - 'file' not found\n" );
            exit( 2 );
        }
        rel_fil = GetBracketedString( p, &p );
        if( rel_fil == NULL ) {
            printf( "Invalid list file format - 'rel file' not found\n" );
            exit( 2 );
        }
        if( *rel_fil == '\0' ) {
            free( rel_fil );
            rel_fil = strdup( "." );
        }
        where = GetBracketedString( p, &p );
        if( where == NULL ) {
            printf( "Invalid list file format - 'product' not found\n" );
            exit( 2 );
        }
        dst_var = GetBracketedString( p, &p );
        if( dst_var == NULL ) {
            printf( "Invalid list file format - 'dst_var' not found\n" );
            exit( 2 );
        }
        if( *dst_var == '\0' ) {
            free( dst_var );
            dst_var = NULL;
        }
        condition = GetBracketedString( p, &p );
        if( condition == NULL ) {
            printf( "Invalid list file format - 'condition' not found\n" );
            exit( 2 );
        }
        desc = GetBracketedString( p, &p );
        if( desc == NULL ) {
            printf( "Invalid list file format - 'description' not found\n" );
            exit( 2 );
        }
        if( ( dst_var != NULL ) && strcmp( dst_var, "." ) == 0 ) {
            free( dst_var );
            dst_var = NULL;
        }
        if( !AddFile( path, old_path, type, redist, file, rel_fil, dst_var, condition ) ) {
            no_error = FALSE;
        }
        free( path );
        free( old_path );
        free( file );
        free( rel_fil );
        free( condition );
        free( desc );
    }
    printf( ".\n" );
    return( no_error );
}


#define STRING_include          "include="
#define STRING_icon             "icon="
#define STRING_supplimental     "supplimental="
#define STRING_ini              "ini="
#define STRING_auto             "auto="
#define STRING_cfg              "cfg="
#define STRING_autoset          "autoset="
#define STRING_spawnafter       "spawnafter="
#define STRING_spawnbefore      "spawnbefore="
#define STRING_spawnend         "spawnend="
#define STRING_env              "env="
#define STRING_dialog           "dialog="
#define STRING_boottext         "boottext="
#define STRING_exe              "exe="
#define STRING_label            "label="
#define STRING_deletedialog     "deletedialog="
#define STRING_deletefile       "deletefile="
#define STRING_deletedir        "deletedir="
#define STRING_language         "language="
#define STRING_upgrade          "upgrade="
#define STRING_forcedll         "forcedll="
#define STRING_assoc            "assoc="
#define STRING_errmsg           "errmsg="
#define STRING_setuperrmsg      "setuperrmsg="

#define STRING_IS( buf, new, string ) \
        ( strnicmp( buf, string, sizeof( string ) - 1 ) == 0 && \
          ( new->item = strdup( buf + sizeof( string ) - 1 ) ) != NULL )


static FILE *PathOpen( char *name )
/*********************************/
{
    FILE        *fp;
    char        buf[_MAX_PATH];

    fp = fopen( name, "r" );
    if( fp == NULL && Include != NULL ) {
        _makepath( buf, NULL, Include, name, NULL );
        fp = fopen( buf, "r" );
    }
    if( fp == NULL ) {
        printf( "\nCannot open include file '%s'\n", name );
        exit( 1 );
    }
    return( fp );
}

static char *ReplaceEnv( char *file_name )
/****************************************/
// if file_name is of the form $env_var$\name, replace with
// value of the environment variable
{
    char                *p, *q, *e, *var;
    char                buff[ _MAX_PATH ];

    // copy and make changes into 'buff'
    q = buff;
    p = file_name;
    for( ;; ) {
        if( *p == '$' ) {
            e = strchr( p + 1, '$' );
            if( e == NULL ) {
                strcpy( q, p );
                break;
            } else {
                *e = '\0';
                var = getenv( p + 1 );
                if( var == NULL ) {
                    printf( "Error: environment variable '%s' not found\n", p + 1 );
                } else {
                    strcpy( q, var );
                    q += strlen( var );
                }
                p = e + 1;
            }
        } else {
            *q = *p;
            if( *p == '\0' ) break;
            ++p;
            ++q;
        }
    }
    if( strcmp( buff, file_name ) == 0 ) {
        // no environment variables found
        return( file_name );
    } else {
        return( strdup( buff ) );
    }
}

#define SECTION_BUF_SIZE 8192   // allow long text strings

static char             SectionBuf[ SECTION_BUF_SIZE ];

void ReadSection( FILE *fp, char *section, LIST **list )
/******************************************************/
{
    LIST                *new;
    int                 file_curr = 0;
    FILE                *file_stack[20];

    Setup = "setup.exe";
    for( ;; ) {
        if( mygets( SectionBuf, sizeof( SectionBuf ), fp ) == NULL ) {
            printf( "%s section not found in '%s'\n", section, MksetupInf );
            return;
        }
        if( SectionBuf[ 0 ] == '#' || SectionBuf[ 0 ] == '\0' ) continue;
        SectionBuf[ strlen( SectionBuf ) - 1 ] = '\0';
        if( stricmp( SectionBuf, section ) == 0 ) break;
    }
    for( ;; ) {
        if( mygets( SectionBuf, sizeof( SectionBuf ), fp ) == NULL ) {
            if( --file_curr >= 0 ) {
                fclose( fp );
                fp = file_stack[file_curr];
                continue;
            } else {
                break;
            }
        }
        if( SectionBuf[ 0 ] == '#' || SectionBuf[ 0 ] == '\0' ) continue;
        SectionBuf[ strlen( SectionBuf ) - 1 ] = '\0';
        if( SectionBuf[ 0 ] == '\0' ) break;
        if( strnicmp( SectionBuf, "setup=", 6 ) == 0 ) {
            Setup = strdup( &SectionBuf[6] );
            Setup = ReplaceEnv( Setup );
            continue;
        }
        new = malloc( sizeof( LIST ) );
        if( new == NULL ) {
            printf( "\nOut of memory\n" );
            exit( 1 );
        }
        new->next = NULL;
        if( STRING_IS( SectionBuf, new, STRING_include ) ) {
            file_stack[file_curr++] = fp;
            fp = PathOpen( new->item );
            free( new->item );
            free( new );
        } else if( STRING_IS( SectionBuf, new, STRING_icon ) ) {
            AddToList( new, &IconList );
        } else if( STRING_IS( SectionBuf, new, STRING_supplimental ) ) {
            AddToList( new, &SupplimentList );
        } else if( STRING_IS( SectionBuf, new, STRING_ini ) ) {
            AddToList( new, &IniList );
        } else if( STRING_IS( SectionBuf, new, STRING_auto ) ) {
            AddToList( new, &AutoList );
        } else if( STRING_IS( SectionBuf, new, STRING_cfg ) ) {
            AddToList( new, &CfgList );
        } else if( STRING_IS( SectionBuf, new, STRING_autoset ) ) {
            AddToList( new, &AutoSetList );
        } else if( STRING_IS( SectionBuf, new, STRING_spawnafter ) ) {
            AddToList( new, &AfterList );
        } else if( STRING_IS( SectionBuf, new, STRING_spawnbefore ) ) {
            AddToList( new, &BeforeList );
        } else if( STRING_IS( SectionBuf, new, STRING_spawnend ) ) {
            AddToList( new, &EndList );
        } else if( STRING_IS( SectionBuf, new, STRING_env ) ) {
            AddToList( new, &EnvList );
        } else if( STRING_IS( SectionBuf, new, STRING_dialog ) ) {
            AddToList( new, &DialogList );
        } else if( STRING_IS( SectionBuf, new, STRING_boottext ) ) {
            AddToList( new, &BootTextList );
        } else if( STRING_IS( SectionBuf, new, STRING_exe ) ) {
            AddToList( new, &ExeList );
            new->item = ReplaceEnv( new->item );
        } else if( STRING_IS( SectionBuf, new, STRING_label ) ) {
            AddToList( new, &LabelList );
        } else if( STRING_IS( SectionBuf, new, STRING_upgrade ) ) {
            AddToList( new, &UpgradeList );
        } else if( STRING_IS( SectionBuf, new, STRING_deletedialog ) ) {
            new->type = DELETE_DIALOG;
            AddToList( new, &DeleteList );
        } else if( STRING_IS( SectionBuf, new, STRING_deletefile ) ) {
            new->type = DELETE_FILE;
            AddToList( new, &DeleteList );
        } else if( STRING_IS( SectionBuf, new, STRING_deletedir ) ) {
            new->type = DELETE_DIR;
            AddToList( new, &DeleteList );
        } else if( STRING_IS( SectionBuf, new, STRING_language ) ) {
            Lang = new->item[0] - '0';
            free( new->item );
            free( new );
        } else if( STRING_IS( SectionBuf, new, STRING_forcedll ) ) {
            AddToList( new, &ForceDLLInstallList );
        } else if( STRING_IS( SectionBuf, new, STRING_assoc ) ) {
            AddToList( new, &AssociationList );
        } else if( STRING_IS( SectionBuf, new, STRING_errmsg ) ) {
            AddToList( new, &ErrMsgList );
        } else if( STRING_IS( SectionBuf, new, STRING_setuperrmsg ) ) {
            AddToList( new, &SetupErrMsgList );
        } else {
            new->item = strdup( SectionBuf );
            AddToList( new, list );
        }
    }
}


void ReadInfFile( void )
/**********************/
{
    FILE                *fp;
    char                ver_buf[ 80 ];

    fp = fopen( MksetupInf, "r" );
    if( fp == NULL ) {
        printf( "Cannot open '%s'\n", MksetupInf );
        return;
    }
    sprintf( ver_buf, "[%s]", Product );
    ReadSection( fp, ver_buf, &AppSection );
    fclose( fp );
}


static void fput36( FILE *fp, long value )
/****************************************/
{
    char        buff[30];

    if( value < 0 ) {
        fprintf( fp, "-" );
        value = -value;
    }
    ltoa( value, buff, 36 );
    fprintf( fp, "%s", buff );
}

void DumpSizes( FILE *fp, FILE_INFO *curr )
/*****************************************/
{
    size_list   *csize;

    fput36( fp, curr->num_files );
    fprintf( fp, "," );
    if( curr->num_files > 1 ) {
        fprintf( fp, "\\\n" );
    }
    for( csize = curr->sizes; csize != NULL; csize = csize->next ) {
        fprintf( fp, "%s!", csize->name );
        fput36( fp, csize->size/512 );
        fprintf( fp, "!" );
        if( csize->redist != '\0' ) {
            fput36( fp, csize->stamp );
        }
        fprintf( fp, "!" );
        if( csize->dst_var != NULL ) {
            fprintf( fp, "%s", csize->dst_var );
        }
        fprintf( fp, "!" );
        if( csize->type != '\0' ) {
            fprintf( fp, "%c", tolower( csize->type ) );
        }
        if( csize->redist != '\0' ) {
            // 'o' for ODBC file
            // 's' for supplemental file (not deleted)
            fprintf( fp, "!%c", tolower( csize->redist ) );
        }
        fprintf( fp, "," );
        if( curr->num_files > 1 ) {
            fprintf( fp, "\\\n" );
        }
    }
}

int CheckForDuplicateFiles( void )
/********************************/
{
    FILE_INFO           *file1,*file2;
    size_list           *name1,*name2;
    int                 ok = TRUE;

    if( FileList == NULL ) return( TRUE );
    for( file1 = FileList; file1->next != NULL; file1 = file1->next ) {
        for( file2 = file1->next; file2 != NULL; file2 = file2->next ) {
            if( file1->path != file2->path ) continue;
            if( file1->condition != file2->condition ) continue;
            for( name1 = file1->sizes; name1 != NULL; name1 = name1->next ) {
                for( name2 = file2->sizes; name2 != NULL; name2 = name2->next ) {
                    if( stricmp( name1->name, name2->name ) == 0 ) {
                        printf( "'%s' is in 2 pack files (%s) (%s)\n",
                                name1->name,
                                file1->pack,
                                file2->pack );
                        ok = FALSE;
                    }
                }
            }
        }
    }
    return( ok );
}


void DumpFile( FILE *out, char *fname )
/*************************************/
{
    FILE                *in;
    char                *buf;
    int                 len;

    in = PathOpen( fname );
    if( in == NULL ) {
        printf( "Cannot open '%s'\n", fname );
        return;
    }
    buf = malloc( SECTION_BUF_SIZE );
    if( buf == NULL ) {
        printf( "Out of memory\n" );
        return;
    }
    for( ;; ) {
        if( mygets( buf, SECTION_BUF_SIZE, in ) == NULL ) {
            break;
        }
        if( strnicmp( buf, "include=", 8 ) == 0 ) {
            len = strlen( buf );
            if( buf[len-1] == '\n' ) buf[len-1] = '\0';
            DumpFile( out, buf+8 );
        } else {
            fputs( buf, out );
        }
    }
    free( buf );
    fclose( in );
}


int CreateScript( long init_size, unsigned padding )
/**************************************************/
{
    int                 disk;
    FILE                *fp;
    FILE_INFO           *curr;
    PATH_INFO           *path;
    LIST                *list;
    LIST                *list2;
    unsigned            nfiles;
    int                 i;

    fp = fopen( "setup.inf", "w" );
    if( fp == NULL ) {
        printf( "Cannot create file 'setup.inf'\n" );
        fp = stdout;
    }
    fprintf( fp, "[Application]\n" );
    for( list = AppSection; list != NULL; list = list->next ) {
        fprintf( fp, "%s\n", list->item );
    }
    fprintf( fp, "DisketteSize=%ld\n", DiskSize );
    if( Upgrade ) {
        fprintf( fp, "IsUpgrade=1\n" );
    }
    fprintf( fp, "\n[Targets]\n" );
    for( list = TargetList; list != NULL; list = list->next ) {
        fprintf( fp, "%s", list->item );
        for( list2 = SupplimentList; list2 != NULL; list2 = list2->next ) {
            if( stricmp( list->item, list2->item ) == 0 ) {
                fprintf( fp, ",supplimental" );
            }
        }
        fprintf( fp, "\n" );
    }

    fprintf( fp, "\n[Dirs]\n" );
    for( path = PathList; path != NULL; path = path->next ) {
        fprintf( fp, "%s,%d,%d\n", path->path, path->target, path->parent );
    }

    disk = 1;
    nfiles = 0;
    fprintf( fp, "\n[Files]\n" );
    for( curr = FileList; curr != NULL; curr = curr->next ) {
        if( ++nfiles > MaxDiskFiles ) {
            nfiles = 0;
            ++disk;
        }
        fprintf( fp, "%s,", curr->pack );
        DumpSizes( fp, curr );
        fput36( fp, curr->path );
        fprintf( fp, "," );
        fput36( fp, curr->old_path );
        fprintf( fp, "," );
        fput36( fp, disk );
        fprintf( fp, ",.,%s\n", curr->condition );
    }

    if( DeleteList != NULL ) {
        fprintf( fp, "\n[DeleteFiles]\n" );
        for( list = DeleteList; list != NULL; list = list->next ) {
            switch( list->type ) {
            case DELETE_DIALOG:
                fprintf( fp, "dialog=" );
                break;
            case DELETE_FILE:
                fprintf( fp, "file=" );
                break;
            case DELETE_DIR:
                fprintf( fp, "directory=" );
                break;
            }
            fprintf( fp, "%s\n", list->item );
        }
    }

    fprintf( fp, "\n[Disks]\n" );          // do this after # of disks determined
    for( i = 1; i <= disk; ++i ) {
        fprintf( fp, "Disk %d\n", i );
    }

    if( IconList != NULL ) {
        fprintf( fp, "\n[PM Info]\n" );
        for( list = IconList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( IniList != NULL ) {
        fprintf( fp, "\n[Profile]\n" );
        for( list = IniList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( AutoList != NULL ) {
        fprintf( fp, "\n[Autoexec]\n" );
        for( list = AutoList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( CfgList != NULL ) {
        fprintf( fp, "\n[Config]\n" );
        for( list = CfgList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( AutoSetList != NULL ) {
        fprintf( fp, "\n[AutoSet]\n" );
        for( list = AutoSetList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( BeforeList != NULL || AfterList != NULL || EndList != NULL ) {
        fprintf( fp, "\n[Spawn]\n" );
        for( list = BeforeList; list != NULL; list = list->next ) {
            fprintf( fp, "before=%s\n", list->item );
        }
        for( list = AfterList; list != NULL; list = list->next ) {
            fprintf( fp, "after=%s\n", list->item );
        }
        for( list = EndList; list != NULL; list = list->next ) {
            fprintf( fp, "end=%s\n", list->item );
        }
    }

    if( EnvList != NULL ) {
        fprintf( fp, "\n[Environment]\n" );
        for( list = EnvList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( DialogList != NULL ) {
        for( list = DialogList; list != NULL; list = list->next ) {
            fprintf( fp, "\n[Dialog]\n" );
            DumpFile( fp, list->item );
        }
    }

    if( LabelList != NULL ) {
        fprintf( fp, "\n[Labels]\n" );
        for( list = LabelList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( UpgradeList != NULL ) {
        fprintf( fp, "\n[Upgrade]\n" );
        for( list = UpgradeList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( ForceDLLInstallList != NULL ) {
        fprintf( fp, "\n[ForceDLLInstall]\n" );
        for( list = ForceDLLInstallList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( AssociationList != NULL ) {
        fprintf( fp, "\n[Associations]\n" );
        for( list = AssociationList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( ErrMsgList != NULL ) {
        fprintf( fp, "\n[ErrorMessage]\n" );
        for( list = ErrMsgList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( SetupErrMsgList != NULL ) {
        fprintf( fp, "\n[SetupErrorMessage]\n" );
        for( list = SetupErrMsgList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    fprintf( fp, "\n[End]\n" );

    while( padding != 0 ) {
        /* add some padding to bring the size up to old size */
        fputc( ' ', fp );
        --padding;
    }

    fclose( fp );
    return( disk );
}


int MakeScript( void )
/********************/
{
    int                 disks;
    FILE_INFO           *curr;
    size_list           *csize;
    long                act_size;
    long                size, old_size, inf_size;
    LIST                *list;

    act_size = 0;
    for( curr = FileList; curr != NULL; curr = curr->next ) {
        for( csize = curr->sizes; csize != NULL; csize = csize->next ) {
            act_size += csize->size;
        }
    }
    printf( "Installed size = %ld\n", act_size );

//  place SETUP.EXE, *.EXE on the 1st disk
    size = FileSize( Setup );
    for( list = ExeList; list != NULL; list = list->next ) {
        size += FileSize( list->item );
    }
    inf_size = 0;
    old_size = 0;
    if( !FillFirst ) size = DiskSize;
    for ( ;; ) {
        /* keep creating script until size stabilizes */
        disks = CreateScript( size+inf_size, 0 );
        inf_size = FileSize( "setup.inf" );
        if( old_size > inf_size ) {
            /*
                If the new size of the install script is less than the
                old size, we can create the script with some padding
                to bring it up to the old size. This prevents us from
                going into an oscillation between two sizes.
            */
            disks = CreateScript( size+old_size, old_size - inf_size );
            inf_size = old_size;
        }
        if( old_size == inf_size ) break;
        old_size = inf_size;
    }
    printf( "Installation will require %d disks\n", disks );
    return( disks );
}


char * GetPath( int pos )
/***********************/
{
    int                 count;
    PATH_INFO           *curr;

    count = 1;
    for( curr = PathList; curr != NULL; curr = curr->next ) {
        if( count == pos )
            return( curr->path );
        ++count;
    }
    return( NULL );
}


void CreateFileList( FILE *fp )
/*****************************/
{
    FILE_INFO           *curr;
    size_list           *list;
    char                *path;

    for( curr = FileList; curr != NULL; curr = curr->next ) {
        path = GetPath( curr->path );
        for( list = curr->sizes; list != NULL; list = list->next ) {
            if( path != NULL && strcmp( path, "." ) )
                fprintf( fp, "%s/", path );
            fprintf( fp, "%s\n", list->name );
        }
    }
}


void MakeLaundryList( void )
/**************************/
{
    FILE                *fp;

    // create archive.lst
    fp = fopen( "instarch.lst", "w" );
    if( fp == NULL ) {
        printf( "Cannot create file 'instarch.lst'\n" );
        fp = stdout;
    }
    // always add setup.inf to file list
    fprintf( fp, "setup.inf\n" );
    CreateFileList( fp );
    if( fp != stdout ) {
        fclose( fp );
    }
}


int main( int argc, char *argv[] )
/********************************/
{
    int                 ok;
    FILE                *fp;

    if( !CheckParms( &argc, &argv ) ) {
        return( 1 );
    }
    fp = fopen( argv[ 2 ], "r" );
    if( fp == NULL ) {
        printf( "Cannot open '%s'\n", argv[ 2 ] );
        return( 1 );
    }
    printf( "Reading Info File...\n" );
    ReadInfFile();
    ok = ReadList( fp );
    if( !ok ) return( 1 );
    printf( "Checking for duplicate files...\n" );
    ok = CheckForDuplicateFiles();
    if( !ok ) return( 1 );
    fclose( fp );
    printf( "Making script...\n" );
    MakeScript();
    MakeLaundryList();
    return( 0 );
}
