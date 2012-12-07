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
#include <stdlib.h>
#include <direct.h>
#include <io.h>
#include "bdiff.h"
#include "wpatchio.h"

#define FALSE 0

struct {
    int origSrcDirLen;
    int origTgtDirLen;
} glob;

extern void     GetMsg( char *, int );
extern void     MsgPrintf( int resourceid, va_list arglist );

int     cmpStrings( const void *, const void * );
void    WPatchApply( char *PatchName, char *TgtPath );
void    DirDelete( char *tgtDir );
void    DirDelFiles( char *tgtDir, char *tgtFiles[], int Dirflag );
void    DirGetFiles( DIR *dirp, char *Files[], char *Dirs[] );

void main( int argc, char *argv[] ) 
{
    if( argc != 3 ) {
        printf( "Usage: WPATCH patchfile target-dir\n" );
        printf( "where target-dir is the directory containing files to be modified,\n" );
        printf( "and patchfile contains patch information for modifying target-dir.\n" );
        printf( "(as created by WCPATCH)\n\n" );
        exit( -2 );
    } else {
        printf( "Watcom Patch version 11.0\n" );
        printf( "Copyright (c) 1996 by Sybase, Inc., and its subsidiaries.\n");
        printf( "All rights reserved.  Watcom is a trademark of Sybase, Inc.\n\n");
    }
    WPatchApply( argv[ 1 ], argv[ 2 ] );
}

void WPatchApply( char *PatchName, char *TgtPath )
{
    short   flag;
    char    RelPath[ PATCH_MAX_PATH_SIZE ];
    char    FullPath[ PATCH_MAX_PATH_SIZE ];

    PatchReadOpen( PatchName );
    while( 1 ) {
        PatchReadFile( &flag, RelPath );
        if ( flag == PATCH_EOF ) break;
        strcpy( FullPath, TgtPath );
        strcat( FullPath, "\\" );
        strcat( FullPath, RelPath );
        switch( flag ) {
            case PATCH_FILE_PATCHED:
                printf( "Patching file %s\n", FullPath );
                DoPatch( "", 0,0,0, FullPath );
                break;
            case PATCH_DIR_DELETED:
                printf( "Deleting directory %s\n", FullPath );
                DirDelete( FullPath );
                break;
            case PATCH_FILE_DELETED:
                printf( "Deleting file %s\n", FullPath );
                remove( FullPath );
                break;
            case PATCH_DIR_ADDED:
                printf( "Adding directory %s\n", FullPath );
                mkdir( FullPath );
                break;
            case PATCH_FILE_ADDED:
                printf( "Adding file %s\n", FullPath );
                PatchGetFile( FullPath );
                break;
        }
    }
    PatchReadClose();
}

void DirDelete( char *tgtDir ) 
{
    DIR     *tgtdirp;

    char **tgtFiles = malloc ( 1000 * sizeof( char *));
    char **tgtDirs = malloc ( 500 * sizeof( char *));

    tgtdirp = opendir( tgtDir );
    if( tgtdirp == NULL ) {
        perror( "" );
    }
    DirGetFiles( tgtdirp, tgtFiles, tgtDirs );
    closedir( tgtdirp );
    DirDelFiles( tgtDir, tgtFiles, 0 );
    DirDelFiles( tgtDir, tgtDirs,  1 );
    remove( tgtDir );
}

void DirDelFiles( char *tgtDir, char *tgtFiles[], int Dirflag ) 
{
    int     indexTgt = 0;
    char    FullTgtPath[ PATCH_MAX_PATH_SIZE ];

    while( tgtFiles[ indexTgt ] != NULL ){
        strcpy( FullTgtPath, tgtDir );
        strcat( FullTgtPath, "\\" );
        strcat( FullTgtPath, tgtFiles[ indexTgt ] );
        if( Dirflag == 1 ) {
            DirDelete( FullTgtPath );
        } else {
            remove( FullTgtPath );
        }
        indexTgt += 1;
    }
}

void DirGetFiles( DIR *dirp, char *Files[], char *Dirs[] ) 
{
    struct dirent   *direntp;
    int             file = 0;
    int             dir  = 0;

    for( ;; ) {
        direntp = readdir( dirp );
        if( direntp == NULL ) break;
        if(( direntp->d_attr & _A_SUBDIR ) == 0 ) {
            /* must be a file */
            Files[ file ] = (char *)malloc( strlen( direntp->d_name ) + 1 );
            strcpy( Files[ file ], direntp->d_name );
            file += 1;
            if( file >= 1000 ) perror( "File limit in directory is 1000." );
        } else {
            /* must be a directory */
            Dirs[ dir ] = (char *)malloc( strlen( direntp->d_name ) + 1 );
            strcpy( Dirs[ dir ], direntp->d_name );
            if( strcmp( Dirs[ dir ], "." ) != 0 && strcmp( Dirs[ dir ], ".." ) != 0 ) {
                dir += 1;
            }
            if( dir >= 500 ) perror( "Subdirectory limit is 500." );
        }
    }
    Files[file] = NULL;
    Dirs[dir] = NULL;
    qsort( Files, file, sizeof(char *), cmpStrings );
    qsort( Dirs, dir, sizeof(char *), cmpStrings );
}

int cmpStrings( const void *op1, const void *op2 )
{
    const char **p1 = (const char **) op1;
    const char **p2 = (const char **) op2;
    return( strcmp( *p1, *p2 ) );
}

static void Err( int format, va_list args )
{
    char        msgbuf[80];

    GetMsg( msgbuf, MSG_ERROR );
    printf( msgbuf );
    MsgPrintf( format, args);
}

void PatchError( int format, ... )
{
    va_list     args;

    va_start( args, format );
    Err( format, args );
    printf( "\n" );
    va_end( args );
    MsgFini();
    exit( EXIT_FAILURE );
}

void FilePatchError( int format, ... )
{
    va_list     args;
    int         err;

    va_start( args, format );
    err = errno;
    Err( format, args );
    printf( ": %s\n", strerror( err ) );
    va_end( args );
    MsgFini();
    exit( EXIT_FAILURE );
}
