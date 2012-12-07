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
* Description:  Perform binary patching.
*
****************************************************************************/


#include "bdiff.h"
#include "bool.h"

extern  void    Message( int, ... );

extern void PatchError( int, ... );
extern void FilePatchError( int, ... );
extern void PatchingFile( char *patchname, char *filename );

static char CurrLevel[ sizeof( LEVEL )  ];

void GetLevel( char *name )
{
    int         io;
    char        buffer[ sizeof( LEVEL ) ];

    CurrLevel[0] = '\0';
    io = open( name, O_BINARY+O_RDONLY );
    if( io == -1 ) return;
    if( lseek( io, -(long)sizeof( LEVEL ), SEEK_END ) != -1L &&
        read( io, buffer, sizeof( LEVEL ) ) == sizeof( LEVEL ) &&
        memcmp( buffer, LEVEL, LEVEL_HEAD_SIZE ) == 0 ) {
        strcpy( CurrLevel, buffer + LEVEL_HEAD_SIZE );
    }
    close( io );
}

void FileCheck( int fd, char *name )
{
    if( fd == -1 ) {
    FilePatchError( ERR_CANT_OPEN, name );
    }
}

void SeekCheck( long pos, char *name )
{
    if( pos == -1 ) {
    FilePatchError( ERR_IO_ERROR, name );
    }
}

#if !defined(BDIFF) && !defined(__386__) && !defined(BDUMP)

    /* Real mode version. Buffer up the holes and apply in sorted order */

    typedef struct {
            foff        offset;
            foff        diff;
    } save_hole;

    int         NumHoles;
    save_hole   *HoleArray = NULL;
    int         HoleArraySize;

    PATCH_RET_CODE InitHoles( void )
    {
        NumHoles = 0;
        HoleArraySize = (64*1024L) / sizeof( save_hole ) - 1;
        for( ;; ) {
            HoleArray = _allocate( HoleArraySize*sizeof(save_hole) );
            if( HoleArray != NULL ) break;
            HoleArraySize /= 2;
            if( HoleArraySize < 100 ) {
        PatchError( ERR_MEMORY_OUT );
        return( PATCH_NO_MEMORY );
            }
        }
    return( PATCH_RET_OKAY );
    }

    void FreeHoleArray( void )
    {
    if( HoleArray != NULL ) {
        _free( HoleArray );
        HoleArray = NULL;
    }
    }

    void AddHole( foff offset, foff diff )
    {
        if( NumHoles == HoleArraySize ) {
            FlushHoles();
        }
        HoleArray[ NumHoles ].offset = offset;
        HoleArray[ NumHoles ].diff = diff;
        NumHoles++;
    }

    int HoleCompare( const void *_h1, const void *_h2 )
    {
        const save_hole *h1 = _h1;
        const save_hole *h2 = _h2;

        if( h1->offset < h2->offset ) return( -1 );
        if( h1->offset > h2->offset ) return( 1 );
        return( 0 );
    }

    void FlushHoles( void )
    {
        extern MY_FILE          NewFile;
        extern void Input( MY_FILE *file, void *tmp, foff off, size_t len );
        extern void Output( MY_FILE *file, void *tmp, foff off, size_t len );

        save_hole       *end;
        save_hole       *curr;
        hole    tmp;

        if( NumHoles != 0 ) {
            qsort( HoleArray, NumHoles, sizeof( save_hole ), HoleCompare );
            end = HoleArray + NumHoles;
            for( curr = HoleArray; curr < end; ++curr ) {
                Input( &NewFile, &tmp, curr->offset, sizeof(hole) );
                tmp += curr->diff;
                Output( &NewFile, &tmp, curr->offset, sizeof( hole ) );
            }
            NumHoles = 0;
        }
    }

#else

    #define InitHoles()
    #define AddHole( off, diff ) OutNew( (off), InNew( (off) )+(diff), hole );
    #define FlushHoles()
    #define FreeHoleArray()

#endif

#ifdef BDIFF
    extern      char *PatchFile;
    extern      char *OldFile;

    #define PatchName ""

    static char *pat;

    #define OpenPatch()                 (pat=PatchFile,PATCH_RET_OKAY)
    #define InPatch( type )             (tmp=pat,pat+=sizeof(type),*(type*)tmp)
    #define ClosePatch()

    #define OpenNew( len )      PATCH_RET_OKAY
    #define InNew( offset )             (*(hole*)(dest+offset))
    #define OutNew( offset, x, type )   *(type*)(dest+(offset))=(x)
    #define CloseNew( len, checksum, havenew )  *havenew = TRUE, PATCH_RET_OKAY

    #define FindOld( name ) NULL
    #define SetOld( name ) NULL
    #define OpenOld( len, prompt, newsize, newsum ) PATCH_RET_OKAY
    #define InOld( offset )             (*(byte*)(OldFile+offset))
    #define CloseOld( havenew, dobackup )

    #define Dump( x )
    #define DOPROMPT    1
    #define DOBACKUP    1

#else  /* Not BDIFF */

    char            *PatchName;
    char            *NewName;
    int             DoPrompt;
    int             DoBackup;
    int             PrintLevel;

    #define InPatch( type )             ( InputPatch( tmp, sizeof( type ) ), \
                                          *(type*)tmp )

    extern PATCH_RET_CODE OpenPatch( void );
    extern PATCH_RET_CODE InputPatch( void *tmp, size_t len );
    extern PATCH_RET_CODE OpenOld( foff len, int prompt, foff newsize, foff newsum );
    extern PATCH_RET_CODE CloseOld( int havenew, int dobackup );
    extern void ClosePatch( void );
    extern char *FindOld( char * );
    extern char *SetOld( char * );
    extern byte InOld( foff );

    extern PATCH_RET_CODE OpenNew( foff len );
    extern PATCH_RET_CODE CloseNew( foff len, foff actual_sum, int *havenew );

    #define Dump( x )
    #define DOPROMPT    1
    #define DOBACKUP    DoBackup
#endif

PATCH_RET_CODE InitPatch( char **target_given )
{
    char            *p;
    int             compare_sig;
    char            target[FILENAME_MAX];
    char            ch;
    char            *temp;
    PATCH_RET_CODE  ret;
#ifdef BDIFF
    char            *tmp;
#else
    char            tmp[4];
#endif
    ret = OpenPatch();
    if( ret != PATCH_RET_OKAY ) {
    return( ret );
    }
    p = SIGNATURE;
    compare_sig = 1;
    for( ;; ) {
        ch = InPatch( char );
        if( ch == EOF_CHAR ) break;
        if( compare_sig ) {
            if( ch != *p ) {
        PatchError( ERR_NOT_PATCHFILE, PatchName );
        return( PATCH_RET_OKAY );
            }
            ++p;
            if( ch == END_SIG_CHAR ) {
                compare_sig = 0;
            }
        }
    }
    p = target;
    for( ;; ) {
        *p = ch = InPatch( char );
        ++p;
        if( ch == '\0' ) break;
    }
    if( (*target_given) != NULL ) {
        temp = SetOld( (*target_given) );
    }
    else {
    temp = FindOld( target );
    }
    if( temp ) {
    *target_given = temp;
    return( PATCH_RET_OKAY );
    } else {
    *target_given = NULL;
    ClosePatch();
    return( PATCH_CANT_OPEN_FILE );
    }
}

#ifdef BDIFF

PATCH_RET_CODE Execute( byte *dest )
{
    char        *tmp;

#else

PATCH_RET_CODE Execute( void )
{
    char        tmp[4];


#if defined(__386__)


#if defined(_WPATCH)
    extern MY_FILE NewFile;
    #define InNew( offset )             ( Input( &NewFile, tmp, offset, \
                                                 sizeof(hole)), \
                                          *(hole*)tmp )
    #define OutNew( off, x, type )      *(type*)tmp = (x); \
                                                 Output( &NewFile, tmp, \
                                                         off, sizeof( type ) );
#else
  extern byte         *NewFile;
  #define OutNew( off, x, type )      *(type*)(NewFile+off) = (x);
  #define InNew( off )                *(hole*)(NewFile+off)
#endif
#elif defined(BDUMP)

    #define InNew( offset )             1
    #define OutNew( off, x, type )      ( x )

    #undef Dump
    #define Dump( x ) printf x
    #undef DOPROMPT
    #undef DOBACKUP
    #define DOPROMPT    0
    #define DOBACKUP    0

#else

    extern MY_FILE      NewFile;

    extern void Input( MY_FILE *file, void *tmp, foff off, size_t len );
    #define InNew( offset )             ( Input( &NewFile, tmp, offset, \
                                                 sizeof(hole)), \
                                          *(hole*)tmp )


    extern void Output( MY_FILE *file, void *tmp, foff off, size_t len );
    #define OutNew( off, x, type )      *(type*)tmp = (x); \
                                                 Output( &NewFile, tmp, \
                                                         off, sizeof( type ) );

#endif

#endif

    patch_cmd   cmd;
    byte        next;
    hole        diff;
    foff        size;
    foff        incr;
    foff        iters;
    foff        old_size;
    foff        new_size;
    foff        checksum;
    foff        new_offset;
    foff        old_offset;
    char        ch;
    int     havenew;
    PATCH_RET_CODE  ret;
    PATCH_RET_CODE  ret2;
#ifdef BDIFF
    char        *dummy = NULL;
#endif

    havenew = 1;
#ifdef BDIFF
    InitPatch( &dummy );
#endif
    old_size = InPatch( foff );
    new_size = InPatch( foff );
    checksum = InPatch( foff );
    ret = OpenOld( old_size, DOPROMPT, new_size, checksum );
    if( ret != PATCH_RET_OKAY ) goto error1;
    ret = OpenNew( new_size );
    if( ret != PATCH_RET_OKAY ) goto error2;
    InitHoles();
    for( ;; ) {
    #if defined( INSTALL_PROGRAM )
        #if defined( WINNT ) || defined( WIN ) || defined( OS2 )
        if( StatusCancelled() ) {
        ret = PATCH_RET_CANCEL;
        goto error3;
        }
        #endif
    #endif
        cmd = InPatch( patch_cmd );
        if( cmd == CMD_DONE ) break;
        switch( cmd ) {
        case CMD_DIFFS:
            new_offset = InPatch( foff );
            size = InPatch( foff );
            Dump(( "Different  new-%8.8lx size-%8.8lx\n", new_offset, size ));
            while( size != 0 ) {
                OutNew( new_offset, InPatch( byte ), byte );
                ++new_offset;
                --size;
            }
            break;
        case CMD_SAMES:
            new_offset = InPatch( foff );
            old_offset = InPatch( foff );
            size = InPatch( foff );
            Dump(( "Similar    new-%8.8lx old-%8.8lx size-%8.8lx\n",
                   new_offset, old_offset, size));
            while( size != 0 ) {
                OutNew( new_offset, InOld( old_offset ), byte );
                ++new_offset;
                ++old_offset;
                --size;
            }
            break;
        case CMD_ITER_HOLES:
            new_offset = InPatch( foff );
            diff = InPatch( hole );
            iters = InPatch( foff );
            incr = InPatch( foff );
            ch = InPatch( byte );
            Dump(( "IterHole   new-%8.8lx diff-%8.8lx iter-%8.8lx inc-%8.8lx\n",
                   new_offset, diff, iters, incr ));
            while( iters != 0 ) {
                AddHole( new_offset, diff );
                new_offset += incr;
                --iters;
            }
            break;
        case CMD_HOLES:
            new_offset = InPatch( foff );
            diff = InPatch( hole );
            for( ;; ) {
                Dump(( "Hole       new-%8.8lx diff-%8.8lx\n",new_offset,diff));
                AddHole( new_offset, diff );
                next = InPatch( byte );
                if( next == 0 ) break;
                if( ( next & 0x80 ) == 0  ) {
                    new_offset += (foff)next & 0x7f;
                } else if( ( next & 0x40 ) == 0 ) {
                    new_offset += ( (foff)next & 0x3f ) << 8;
                    new_offset += (foff)InPatch( byte );
                } else {
                    new_offset += ( (foff)next & 0x3f ) << 16;
                    new_offset += (foff)InPatch(byte) << 8;
                    new_offset += (foff)InPatch(byte);
                }
            }
            break;
        default:
        PatchError( ERR_BAD_PATCHFILE, PatchName );
        ret = PATCH_BAD_PATCH_FILE;
        goto error3;
        }
    }
    ret = PATCH_RET_OKAY;
    FlushHoles();
error3:
    FreeHoleArray();
    ret2 = CloseNew( new_size, checksum, &havenew );
    if( ret == PATCH_RET_OKAY ) {
    ret = ret2;
    }
error2:
    CloseOld( havenew && DOPROMPT, DOBACKUP );
error1:
    ClosePatch();
    return( ret );
}

#if !defined( BDIFF )

extern PATCH_RET_CODE DoPatch( char *patchname,
                   int doprompt,
                   int dobackup,
                   int printlevel,
                   char *outfilename )
{
    char        buffer[ sizeof( LEVEL ) ];
#ifndef _WPATCH
    char        *target = NULL;
#endif
    PATCH_RET_CODE  ret;

    outfilename=outfilename;
    PatchName = patchname;
    DoPrompt = doprompt;
    DoBackup = dobackup;
    PrintLevel = printlevel;
    NewName = tmpnam( NULL );
#ifndef _WPATCH
    if( access( PatchName, R_OK ) != 0 ) {
        PatchError( ERR_CANT_FIND, PatchName );
        return( PATCH_CANT_FIND_PATCH );
    }
#endif
#if !defined( INSTALL_PROGRAM )
    if( PrintLevel ) {
        GetLevel( PatchName );
        if( CurrLevel[ 0 ] == '\0' ) {
        Message( MSG_NOT_PATCHED, PatchName );
        } else {
            Message( MSG_PATCHED_TO_LEVEL, PatchName, CurrLevel );
        }
    return( PATCH_RET_OKAY );
    }
#endif
    _splitpath( PatchName, NULL, NULL, NULL, buffer );
#ifndef _WPATCH
    ret = InitPatch( &target );
#else
    ret = InitPatch( &outfilename );
#endif
    #if defined( INSTALL_PROGRAM )
        if( ret != PATCH_RET_OKAY ) {
        return( ret );
        }
    if( outfilename != NULL ) {
        strcpy( outfilename, target );
        PatchingFile( PatchName, outfilename );
    }
    #endif
#ifndef _WPATCH
    GetLevel( target );
    if( stricmp( buffer, CurrLevel ) <= 0 ) {
    ClosePatch();
    #if !defined( INSTALL_PROGRAM )
        Message( MSG_ALREADY_PATCHED, target, CurrLevel );
    #endif
    return( PATCH_ALREADY_PATCHED );
    } else {
#endif
    ret = Execute();
    if( ret != PATCH_RET_OKAY ) {
        return( ret );
    }
#ifndef _WPATCH
    }
#endif
    #if !defined( INSTALL_PROGRAM ) && !defined( _WPATCH )
        Message( MSG_SUCCESSFULLY_PATCHED, target, buffer );
    #endif
    return( PATCH_RET_OKAY );
}

#endif

