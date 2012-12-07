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
* Description:  OS/2 single line resource semantic actions.
*
****************************************************************************/


#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "wresall.h"
#include "rcmem.h"
#include "errors.h"
#include "global.h"
#include "ytab2.gh"
#include "semantic.h"
#include "semsingl.h"
#include "reserr.h"
#include "depend.h"
#include "scan.h"
#include "iortns.h"

/**** forward references ****/
static void AddFontResources( WResID * name, ResMemFlags, char * filename );


extern void SemOS2AddSingleLineResource( WResID *name, uint_8 type,
                       FullOptFlagsOS2 *fullflags, char *filename )
/*****************************************************************/
{
    ResLocation     start;
    ResMemFlags     flags, flagsMDP, flagsMP;
    char            full_filename[ _MAX_PATH ];
    static int      firstIcon = TRUE;

    if( ErrorHasOccured ) {
        RcMemFree( name );
        RcMemFree( filename );
        return;
    }

    RcFindResource( filename, full_filename );
    if( full_filename[0] == '\0' ) {
        RcError( ERR_CANT_FIND_FILE, filename );
        goto HANDLE_ERROR;
    }

    if( AddDependency( full_filename ) ) goto HANDLE_ERROR;

    flagsMDP = MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE | MEMFLAG_PURE;
    flagsMP  = MEMFLAG_MOVEABLE | MEMFLAG_PURE;

    switch( type ) {
    case Y_DEFAULTICON:
        /* DEFAULTICON doesn't have a name, let's make our own */
        name = (WResID*)RcMemMalloc( sizeof( WResID ) );
        name->IsName = FALSE;
        name->ID.Num = 999;
        firstIcon    = TRUE;    /* Trigger a warning if we have one already */
        /* Note the fallthrough! */
    case Y_POINTER:
    case Y_ICON:
        if( fullflags != NULL ) {
            SemOS2CheckResFlags( fullflags, 0,
                              MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE, 0 );
            flags = fullflags->flags;
        } else {
            flags = flagsMDP;
        }

        /* Duplicate the first icon encountered as the default icon IFF it
           has resource ID equal to 1
        */
        if( firstIcon && !name->IsName && (name->ID.Num == 999 || name->ID.Num == 1) ) {
            WResID      *id;

            id = (WResID*)RcMemMalloc( sizeof( WResID ) );
            if( id == NULL )
                break;

            firstIcon  = FALSE;
            id->IsName = FALSE;
            id->ID.Num = 22;
            start = SemCopyRawFile( filename );
            SemAddResourceFree( name, WResIDFromNum( OS2_RT_POINTER ),
                            flags, start );

            start = SemCopyRawFile( filename );
            SemAddResourceFree( id, WResIDFromNum( OS2_RT_DEFAULTICON ),
                                flagsMDP, start );
        }
        else {
            start = SemCopyRawFile( filename );
            SemAddResourceFree( name, WResIDFromNum( OS2_RT_POINTER ),
                                flags, start );
        }
        break;

    case Y_BITMAP:
        if( fullflags != NULL ) {
            SemOS2CheckResFlags( fullflags, 0, MEMFLAG_MOVEABLE, MEMFLAG_PURE );
            flags = fullflags->flags;
        } else {
            flags = flagsMP;
        }
        start = SemCopyRawFile( filename );
        SemAddResourceFree( name, WResIDFromNum( OS2_RT_BITMAP ),
                            flags, start );
        break;

    case Y_FONT:
        if( fullflags != NULL ) {
            SemOS2CheckResFlags( fullflags, 0,
                                MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE,
                                MEMFLAG_PURE );
            flags = fullflags->flags;
        } else {
            flags = flagsMDP;
        }
        AddFontResources( name, flags, full_filename );
        break;
    default:
        RcMemFree( name );
        break;
    }

    RcMemFree( filename );

    return;

HANDLE_ERROR:
    ErrorHasOccured = TRUE;
    RcMemFree( name );
    RcMemFree( filename );
} /* SemOS2AddSingleLineResource */

static RcStatus readFontInfo( int handle, FontInfo *info, int *err_code )
/***********************************************************************/
{
    int     numread;

    numread = RcRead( handle, info, sizeof( FontInfo ) );
    if( numread != sizeof( FontInfo ) ) {
        *err_code = errno;
        if( numread == -1 ) {
            return( RS_READ_ERROR );
        } else {
            return( RS_READ_INCMPLT );
        }
    }
    return( RS_OK );
}

#define FONT_BUFFER_SIZE  0x1000

static RcStatus copyFont( FontInfo * info, int handle, WResID * name,
                                ResMemFlags flags, int *err_code )
/************************************************************************/
{
    RcStatus            error;
    char *              buffer;
    ResLocation         loc;
    long                pos;

    buffer = RcMemMalloc( FONT_BUFFER_SIZE );

    loc.start = SemStartResource();

    if( ResWriteFontInfo( info, CurrResFile.handle ) ) {
        error = RS_WRITE_ERROR;
        *err_code = LastWresErr();
    } else {
        pos = RcTell( handle );
        if( pos == -1 ) {
            error = RS_READ_ERROR;
            *err_code = errno;
        } else {
            error = SemCopyDataUntilEOF( pos, handle, buffer,
                                         FONT_BUFFER_SIZE, err_code );
        }
    }

    loc.len = SemEndResource( loc.start );
    /* add the font to the RES file directory */
    SemAddResourceFree( name, WResIDFromNum( (long)RT_FONT ), flags, loc );

    RcMemFree( buffer );

    return( error );
} /* copyFont */

typedef struct {
    int         status;
    int         err_code;
}ReadStrErrInfo;

static void * readString( int handle, long offset, ReadStrErrInfo *err )
/*************************************************************************/
{
    long    seekrc;
    char    *retstr;


    seekrc = RcSeek( handle, offset, SEEK_SET );
    if( seekrc == -1 ) {
        err->status = RS_READ_ERROR;
        err->err_code = errno;
        return( NULL );
    } else {
        retstr = ResReadString( handle, NULL );
        if( retstr == NULL ) {
            if( LastWresStatus() == WRS_READ_INCOMPLETE ) {
                err->status = RS_READ_INCMPLT;
            } else {
                err->status = RS_READ_ERROR;
                err->err_code = LastWresErr();
            }
            return( NULL );
        } else {
            return( retstr );
        }
    }
}

static FullFontDir * NewFontDir( void )
/*************************************/
{
    FullFontDir *   newdir;

    newdir = RcMemMalloc( sizeof( FullFontDir ) );
    newdir->Head = NULL;
    newdir->Tail = NULL;
    newdir->NumOfFonts = 0;

    return( newdir );
}

static FullFontDirEntry * NewFontDirEntry( FontInfo * info, char * devicename,
                        char * facename, WResID * fontid )
/****************************************************************************/
{
    FullFontDirEntry *      entry;
    int                     structextra;
    int                     devicelen;
    int                     facelen;

    devicelen = strlen( devicename ) + 1;
    facelen = strlen( facename ) + 1;
    structextra = devicelen + facelen;

    /* -1 for the 1 char in the struct already */
    entry = RcMemMalloc( sizeof( FullFontDirEntry ) + structextra - 1 );
    entry->Next = NULL;
    entry->Prev = NULL;
    /* -1 for the 1 char in the struct already */
    entry->Entry.StructSize = sizeof( FontDirEntry ) + structextra - 1;
    entry->Entry.FontID = fontid->ID.Num;
    entry->Entry.Info = *info;
    memcpy( &(entry->Entry.DevAndFaceName[ 0 ]), devicename, devicelen );
    memcpy( &(entry->Entry.DevAndFaceName[ devicelen ]), facename, facelen );
    /* set dfDevice and dfFace to be the offset of the strings from the start */
    /* of the FontInfo structure (entry->Entry.Info) */
    entry->Entry.Info.dfDevice = sizeof( FontInfo );
    entry->Entry.Info.dfFace = sizeof( FontInfo ) + devicelen;

    return( entry );
}

static void AddFontToDir( FontInfo * info, char * devicename, char * facename,
                WResID * fontid )
/****************************************************************************/
{
    FullFontDirEntry *      entry;

    entry = NewFontDirEntry( info, devicename, facename, fontid );

    if (CurrResFile.FontDir == NULL) {
        CurrResFile.FontDir = NewFontDir();
    }

    ResAddLLItemAtEnd( (void **)&(CurrResFile.FontDir->Head),
                        (void **)&(CurrResFile.FontDir->Tail), entry );
    CurrResFile.FontDir->NumOfFonts += 1;
}

static void AddFontResources( WResID * name, ResMemFlags flags,
                              char * filename )
/**************************************************************/
{
    FontInfo            info;
    char *              devicename;
    char *              facename;
    int                 handle;
    int                 error;
    int                 err_code;
    ReadStrErrInfo      readstr_err;

    if (name->IsName) {
        RcError( ERR_FONT_NAME );
        return;
    }

    handle = RcIoOpenInput( filename, O_RDONLY | O_BINARY );
    if (handle == -1) goto FILE_OPEN_ERROR;

    error = readFontInfo( handle, &info, &err_code );
    if( error != RS_OK) goto READ_HEADER_ERROR;

    error = copyFont( &info, handle, name, flags, &err_code );
    if( error != RS_OK ) goto COPY_FONT_ERROR;

    devicename = readString( handle, info.dfDevice, &readstr_err );
    if( devicename == NULL ) {
        error = readstr_err.status;
        err_code = readstr_err.err_code;
        goto READ_HEADER_ERROR;
    }

    facename = readString( handle, info.dfFace, &readstr_err );
    if (facename == NULL) {
        error = readstr_err.status;
        err_code = readstr_err.err_code;
        RcMemFree( devicename );
        goto READ_HEADER_ERROR;
    }

    AddFontToDir( &info, devicename, facename, name );

    RcMemFree( devicename );
    RcMemFree( facename );

    RcClose( handle );

    return;


FILE_OPEN_ERROR:
    RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
    ErrorHasOccured = TRUE;
    RcMemFree( name );
    return;

READ_HEADER_ERROR:
    ReportCopyError( error, ERR_READING_FONT, filename, err_code );
    ErrorHasOccured = TRUE;
    RcMemFree( name );
    RcClose( handle );
    return;

COPY_FONT_ERROR:
    ReportCopyError( error, ERR_READING_FONT, filename, err_code );
    ErrorHasOccured = TRUE;
    RcClose( handle );
    return;
}

static void FreeFontDir( FullFontDir * olddir )
/*********************************************/
{
    FullFontDirEntry *  currentry;
    FullFontDirEntry *  oldentry;

    currentry = olddir->Head;
    while (currentry != NULL) {
        oldentry = currentry;
        currentry = currentry->Next;

        RcMemFree( oldentry );
    }

    RcMemFree( olddir );
}

/* name and memory flags of the font directory resource */
#define FONT_DIR_NAME   "FONTDIR"
#define FONT_DIR_FLAGS  MEMFLAG_MOVEABLE|MEMFLAG_PRELOAD   /* not PURE */

extern void SemOS2WriteFontDir( void )
/*********************************/
{
    FullFontDirEntry *  currentry;
    ResLocation         loc;
    int                 error;

    if (CurrResFile.FontDir == NULL) {
        return;
    }

    loc.start = SemStartResource();

    error = ResWriteUint16( &(CurrResFile.FontDir->NumOfFonts),
                        CurrResFile.handle );
    if (error) goto OUTPUT_WRITE_ERROR;

    for (currentry = CurrResFile.FontDir->Head; currentry != NULL;
                currentry = currentry->Next) {
        error = ResWriteFontDirEntry( &(currentry->Entry), CurrResFile.handle );
        if (error) goto OUTPUT_WRITE_ERROR;
    }

    loc.len = SemEndResource( loc.start );

    SemAddResourceFree( WResIDFromStr( FONT_DIR_NAME ),
                WResIDFromNum( (long)RT_FONTDIR ), FONT_DIR_FLAGS, loc );

    FreeFontDir( CurrResFile.FontDir );
    CurrResFile.FontDir = NULL;

    return;


OUTPUT_WRITE_ERROR:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, LastWresErrStr() );
    ErrorHasOccured = TRUE;
    FreeFontDir( CurrResFile.FontDir );
    CurrResFile.FontDir = NULL;
    return;
}
