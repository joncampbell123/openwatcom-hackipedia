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
* Description:  Linker message output and message resource low-level functions.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#ifdef __WATCOMC__
#include <process.h>
#endif
#include <unistd.h>

#include "linkstd.h"
#include "dbginfo.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "mapio.h"
#include "msg.h"
#include "fileio.h"
#include "ideentry.h"
#include "loadfile.h"
#include "wressetr.h"
#include "wreslang.h"
#include "clibint.h"

#define NO_RES_MESSAGE "could not open message resource file"

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;
static  int             Res_Flag;

static void Msg_Add_Arg( MSG_ARG *arginfo, char typech, va_list *args );


static ssize_t ResWrite( int dummy, const void *buff, size_t size )
/*****************************************************************/
/* redirect wres write to writeload */
{
    dummy = dummy;
    DbgAssert( dummy == Root->outfile->handle );
    WriteLoad( (void *) buff, size );
    return( size );
}

static off_t ResSeek( int handle, off_t position, int where )
/***********************************************************/
/* Workaround wres bug */
{
    if( ( where == SEEK_SET ) && ( handle == hInstance.handle ) ) {
        return( QLSeek( handle, position + FileShift, where, NULL ) - FileShift );
    } else {
        return( QLSeek( handle, position, where, NULL ) );
    }
}

static int ResClose( int handle )
/*******************************/
{
    return( close( handle ) );
}

static ssize_t ResRead( int handle, void *buffer, size_t len )
/************************************************************/
{
    return( QRead( handle, buffer, len, NULL ) );
}

static off_t ResPos( int handle )
/*******************************/
{
    return( QPos( handle ) );
}

WResSetRtns( ResOpen, ResClose, ResRead, ResWrite, ResSeek, ResPos, ChkLAlloc, LFree );


int InitMsg( void )
{
    char        buff[_MAX_PATH];
    int         initerror;
#if !defined( IDE_PGM )
    char        *fname;

    fname = _LpDllName;
    initerror = fname == NULL;
#else
    char        fname[_MAX_PATH];

    initerror = _cmdname( fname ) == NULL;
#endif
    Res_Flag = EXIT_SUCCESS;
    BannerPrinted = FALSE;
    if( !initerror ) {
        hInstance.filename = fname;
        OpenResFile( &hInstance );
        if( hInstance.handle == NIL_HANDLE ) {
            initerror = TRUE;
        } else {
            initerror = FindResources( &hInstance );
        }
    }
    if( !initerror ) {
        initerror = InitResources( &hInstance );
    }
    MsgShift = WResLanguage() * MSG_LANG_SPACING;
    if( !initerror && !Msg_Get( MSG_PRODUCT, buff ) ) {
        initerror = 1;
    }
    if( initerror ) {
        Res_Flag = EXIT_FAILURE;
        WriteInfoStdOut( NO_RES_MESSAGE, ERR, NULL );
    } else {
        Res_Flag = EXIT_SUCCESS;
    }
    return( Res_Flag );
}

int Msg_Get( int resourceid, char *buffer )
{
    if( Res_Flag != EXIT_SUCCESS || LoadString( &hInstance, resourceid + MsgShift,
                (LPSTR) buffer, RESOURCE_MAX_SIZE ) != 0 ) {
        buffer[0] = '\0';
        return( 0 );
    }
    return( 1 );
}

void Msg_Do_Put_Args( char rc_buff[], MSG_ARG_LIST *arg_info,
                        char *types, ... )
{
    va_list     args;

    va_start( args, types );
    Msg_Put_Args( rc_buff, arg_info, types, &args );
    va_end( args );
}

// Write arguments to put into a message and make it printf-like
void Msg_Put_Args(
    char                message[],      // Contains %s, etc. or %digit specifiers
    MSG_ARG_LIST        *arg_info,      // Arguments found
    char                *types,         // message conversion specifier types
                                        // NULL or strlen <= 3 ( arg_info->arg elements)
    va_list             *args )         // Initialized va_list
{
    int         argnum = 0;             // Index of argument found
    int         j;                      // General purpose loop index
    int         order[3];               // Mapping of args to arg_info->arg
    char        *percent;               // Position of '%' in message
    char        types_buff[1 + 3];      // readwrite copy of types
    char        specifier;              // Character following '%'

    if( types != NULL ) {
        strcpy( types_buff, types );
                                        // conversions set order[]; digits->s
        percent = message - 2;          // So strchr below can work
        while( ( percent = strchr( percent + 2, '%' ) ) != NULL ) {
            specifier = percent[1];
            for( j = 0; types_buff[j] != '\0'; j++ ) {  // Match with types
                if( types_buff[j] == specifier ) {
                    order[j] = argnum;
                    argnum++;
                    if( isdigit( specifier ) )          // Digit becomes s
                        types_buff[j] = percent[1] = 's';
                    break;
                }
            }
        }
                                        // Re-order sequential arguments
        for( j = 0; j < argnum; j++ ) {
            Msg_Add_Arg( arg_info->arg + order[j], types_buff[j], args );
        }
    }
    arg_info->index = 0;
}

static void Msg_Add_Arg( MSG_ARG *arginfo, char typech, va_list *args )
{
    switch( typech ) {
        case 's':
            arginfo->string = va_arg( *args, char * );
            break;
        case 'x':
        case 'd':
            arginfo->int_16 = va_arg( *args, unsigned int );
            break;
        case 'l':
            arginfo->int_32 = va_arg( *args, unsigned long );
            break;
        case 'A':
        case 'a':
            arginfo->address = va_arg( *args, targ_addr * );
            break;
        case 'S':
            arginfo->symb = va_arg( *args, symbol * );
            break;
    }
}

void Msg_Write_Map( int resourceid, ... )
{
    char        msg_buff[RESOURCE_MAX_SIZE];
    va_list     arglist;

    Msg_Get( resourceid, msg_buff );
    va_start( arglist, resourceid );
    DoWriteMap( msg_buff, &arglist );
    va_end( arglist );
}

int FiniMsg( void )
{
    int     retcode = EXIT_SUCCESS;

    if( Res_Flag == EXIT_SUCCESS ) {
        if( CloseResFile( &hInstance ) != -1 ) {
            Res_Flag = EXIT_FAILURE;
        } else {
            retcode = EXIT_FAILURE;
        }
    }
    return( retcode );
}
