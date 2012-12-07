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
* Description:  Linker binary i/o interface routines (ZDOS flavour).
*
****************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>
#include <unistd.h>
#include <zdos.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "wressetr.h"
#include "objio.h"
#include "fileio.h"

typedef enum {
    NOT_HIT,
    BREAK_DETECTED,
    BREAK_HANDLED
} break_status;

static bool         BreakCond;
static break_status CaughtBreak = NOT_HIT;
static int          OpenFiles   = 0;                    // the number of open files
static unsigned     LastResult;
static char         Rc_Buffer[RESOURCE_MAX_SIZE];

#define     TOOMANY             4
#define     ACCESS_DENIED       5

extern void         CheckBreak( void );

extern void LnkFilesInit( void )
/******************************/
{
}

extern void PrintIOError( unsigned msg, char *types, char *name )
/***************************************************************/
{
    char    rc_buff[RESOURCE_MAX_SIZE];

    Msg_Get( MSG_IOERRLIST_0 + LastResult, rc_buff );
    LnkMsg( msg, types, name, rc_buff );
}

static int DoOpen( char *name, bool create, unsigned mode )
/****************************************************************/
{
    int h;

    CheckBreak();
    for( ;; ) {
        if( OpenFiles >= MAX_OPEN_FILES ) CleanCachedHandles();
        if( create ) {
            h  = DosCreateFile( mode, name );
        } else {
            h = DosOpenFile( mode, name );
        }
        if( h  > 0 ) {
            OpenFiles++;
            break;
        }
        if( h != -TOOMANY )
            break;
        if( !CleanCachedHandles() )
            break;
    }
    return( h );
}


static char *QErrMsg( unsigned status )
/*************************************/
{
    Msg_Get( MSG_IOERRLIST_0 + ( status & 0x7fffffff ), Rc_Buffer );
    return( Rc_Buffer );
}

extern f_handle QOpenR( char *name )
/**********************************/
{
    int h;

    h = DoOpen( name, FALSE, MODE_READ_ONLY );
    if( h > 0 )
        return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, QErrMsg( -h ) );
    return( NIL_HANDLE );
}

extern f_handle QOpenRW( char *name )
/***********************************/
{
    int h;

    h = DoOpen( name, TRUE, _A_NORMAL );
    if( h > 0 )
        return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, QErrMsg( -h ) );
    return( NIL_HANDLE );
}

extern unsigned QRead( f_handle file, void *buffer, unsigned len, char *name )
/****************************************************************************/
{
/* read into far memory */
    int h;

    CheckBreak();
    h = DosReadFile( len, buffer, file );
    if( h < 0 ) {
        if( name != NULL ) {
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
        } else {
            return( -1 );
        }
    }
    return( h );
}

static unsigned TestWrite( f_handle file, void *buffer, unsigned len, char *name )
/*****************************************************************************/
{
    int     h;
    char    rc_buff[RESOURCE_MAX_SIZE];

    CheckBreak();
    if( len == 0 )
        return( 0 );

    h = DosWriteFile( len, buffer, file );
    if( name != NULL ) {
        if( h < 0 ) {
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
            return( ~0 );
        } else if( h != len ) {
            Msg_Get( MSG_IOERRLIST_7, rc_buff );
            LnkMsg( (FTL+MSG_IO_PROBLEM) & ~OUT_MAP, "12", name, rc_buff );
        }
    }
    return( h );
}

extern unsigned QWrite( f_handle file, void *buffer, unsigned len, char *name )
/*****************************************************************************/
{
    while( len > (16*1024) ) {
        if( TestWrite( file, buffer, 16*1024, name ) != 16*1024 ) return 0;
        len -= 16*1024;
        buffer = ((char *) buffer) + 16*1024;
    }
    return TestWrite( file, buffer, len, name );
}

char    NLSeq[] = { "\r\n" };

extern void QWriteNL( f_handle file, char *name )
/***********************************************/
{
    QWrite( file, NLSeq, sizeof( NLSeq ) - 1, name );
}

extern void QClose( f_handle file, char *name )
/*********************************************/
{
/* file close */
    int h;

    CheckBreak();
    h = DosCloseFile( file );
    OpenFiles--;
    if( h >= 0  || name == NULL )
        return;
    LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
}

extern long QLSeek( f_handle file, long position, int start, char *name )
/***********************************************************************/
{
    int         h;
    unsigned    pos;

    CheckBreak();

    h = DosSetFilePosition ( start, position, file, &pos );
    if( h < 0 && name != NULL ) {
        LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
        return( -1 );   /* for xmem checking */
    }
    return( pos );
}

extern void QSeek( f_handle file, long position, char *name )
/***********************************************************/
{
    QLSeek( file, position, SEEK_FROM_START, name );
}

extern unsigned long QPos( f_handle file )
/****************************************/
{
    unsigned    pos;

    DosSetFilePosition ( SEEK_FROM_CURRENT, 0, file, &pos );
    return( pos );
}

extern unsigned long QFileSize( f_handle file )
/*********************************************/
{
    unsigned    curpos, size;

    curpos = QPos( file );
    DosSetFilePosition ( SEEK_FROM_END, 0, file, &size );
    DosSetFilePosition ( SEEK_FROM_START, curpos, file, &curpos );
    return( size );
}

extern void QDelete( char *name )
/*******************************/
{
    int h;

    if( name == NULL )
        return;
    h = DosDeleteFile( name );
    if( h < 0 ) {
        if( h != -2 ) {  /* file not found is OK */
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
        }
    }
}

extern bool QReadStr( f_handle file, char *dest, unsigned size, char *name )
/**************************************************************************/
{
/* quick read string (for reading directive file) */
    bool    eof;
    char    ch;

    eof = FALSE;
    while( --size > 0 ) {
        if( QRead( file, &ch, 1, name ) == 0 ) {
            eof = TRUE;
            break;
        } else if( ch != '\r' ) {
            *dest++ = ch;
        }
        if( ch == '\n' ) break;
    }
    *dest = '\0';
    return( eof );
}

extern bool QIsDevice( f_handle file )
/************************************/
{
    if ( DosIoctlGetDeviceInfo ( file ) & 0x80 )
        return( TRUE );
    else
        return( FALSE );  // don't write the prompt if input not from stdin
}

extern f_handle ExeCreate( char *name )
/*************************************/
{
    int h;

    h = DoOpen( name, TRUE, _A_NORMAL );
    LastResult = h & 0x7fffffff;
    if( h > 0 )
        return( h );
    return( NIL_HANDLE );
}

static f_handle NSOpen( char *name, unsigned mode )
/*************************************************/
{
    int h;

    h = DoOpen( name, FALSE, mode );
    LastResult = h & 0x7fffffff;
    if( h > 0 )
        return( h );
    return( NIL_HANDLE );
}

extern f_handle ExeOpen( char *name )
/***********************************/
{
    return( NSOpen( name, MODE_READ_AND_WRITE ) );
}

extern f_handle QObjOpen( char *name )
/************************************/
{
    return( NSOpen( name, MODE_READ_ONLY ) );
}

extern f_handle TempFileOpen( char *name )
/****************************************/
{
// open without suiciding. Don't create the file
    return( NSOpen( name, MODE_READ_ONLY ) );
}

extern int QMakeFileName( char **pos, char *name, char *fname )
/*************************************************************/
{
    char        *pathptr;
    unsigned    path_len;
    char        *file_ptr;

    pathptr = *pos;
    if( pathptr == NULL )
        return( 0 );
    while( *pathptr != '\0' ) {
        if( IS_PATH_LIST_SEP( *pathptr ) )
            *pos = ++pathptr;
        for( ;; ) {
            if( *pathptr == '\0' )
                break;
            if( IS_PATH_LIST_SEP( *pathptr ) )
                break;
            pathptr++;
        }
        path_len = pathptr - *pos;
        if( path_len != 0 ) {
            memcpy( fname, *pos, path_len );
            file_ptr = fname + path_len;
            switch( file_ptr[ -1 ] ) {
            CASE_PATH_SEP:
                break;
            default:
                *file_ptr++ = PATH_SEP;
                break;
            }
            strcpy( file_ptr, name );
            *pos = pathptr;
            return( 1 );
        }
        *pos = pathptr;
    }
    return( 0 );
}

extern bool QHavePath( char *name )
/*********************************/
{
    return( *name == '\\' || *name == '/' || *(name + 1) == ':' );
}

extern bool QSysHelp( char **cmd_ptr )
{
    cmd_ptr = cmd_ptr;
    return( FALSE );
}

extern bool QModTime( char *name, time_t *time )
/**********************************************/
{
    int         result;
    struct stat buf;

    result = stat( name, &buf );
    *time = buf.st_mtime;
    return result != 0;
}

extern time_t QFModTime( int handle )
/***********************************/
{
    struct stat buf;

    fstat( handle, &buf );
    return buf.st_mtime;
}

int ResOpen( const char *name, int access, ... )
/******************************************/
{
/* this just ignores the access parameter, and assumes it knows what the
 * resource stuff really wants to do */
    access = access;  /* to avoid a warning */
    return( NSOpen( (char *) name, MODE_READ_ONLY ) );
}

extern char WaitForKey( void )
/****************************/
{
    return getch();
}

extern void GetCmdLine( char *buff )
/**********************************/
{
    getcmd( buff );
}

extern void TrapBreak( int sig_num )
/**********************************/
{
    sig_num = sig_num;          // to avoid a warning, will be optimized out.
    if( CaughtBreak != BREAK_HANDLED ) {
        CaughtBreak = BREAK_DETECTED;
    }
}

extern void CheckBreak( void )
/****************************/
{
    if( CaughtBreak == BREAK_DETECTED ) {
        CaughtBreak = BREAK_HANDLED;
        LnkMsg( FTL+MSG_BREAK_HIT, NULL );    /* suicides */
    }
}

extern void SetBreak( void )
/**************************/
{
    BreakCond = DosGetBreakFlag();
    DosSetBreakFlag( 1 );
}

extern void RestoreBreak( void )
/******************************/
{
    DosSetBreakFlag( BreakCond );
}
