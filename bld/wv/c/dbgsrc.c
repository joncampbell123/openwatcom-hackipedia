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
* Description:  Source file access.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbglit.h"
#include "dbgtoken.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbginfo.h"
#include "dbgio.h"
#include "dui.h"
#include <string.h>


extern cue_file_id      CueFileId( cue_handle * );
extern unsigned         CueFile( cue_handle *ch, char *file, unsigned max );
extern unsigned long    CueLine( cue_handle *ch );
extern bool             ScanItem( bool ,char **,unsigned int *);
extern unsigned int     ScanCmd( char * );
extern void             Scan( void );
extern struct browser   *FOpenSource( char *, mod_handle, cue_file_id );
extern void             ConfigLine( char * );
extern char             *StrCopy(char *, char *);
extern char             *GetCmdName( int );
extern void             DbgUpdate( update_list );
extern bool             IsAbsolutePath( char *path );


extern char_ring *SrcSpec;
extern char      *TxtBuff;
extern tokens     CurrToken;

static char AddTab[] = { "Add\0" };


void FreeRing( char_ring *p )
{
    char_ring   *old;

    while( p != NULL ) {
        old = p;
        p = p->next;
        _Free( old );
    }
}

/*
 * InitSource -- initialize the source path and extension
 */

void InitSource( void )
{
    SrcSpec = NULL;
}


void FiniSource( void )
{
    FreeRing( SrcSpec );
    SrcSpec = NULL;
}


void DeleteRing( char_ring **owner, char *start, unsigned len )
{
    char_ring *p;

    if( len != 0 ) {
        for( ;; ) {
            p = *owner;
            if( p == NULL ) break;
            if( strnicmp( p->name, start, len ) == 0 ) {
                *owner = p->next;
                _Free( p );
                break;
            }
            owner = &p->next;
        }
    }
}


void InsertRing( char_ring **owner, char *start, unsigned len )
{
    char_ring *path;

    if( len != 0 ) {
        path = DbgMustAlloc( sizeof( char_ring ) + len );
        memcpy( path->name, start, len );
        path->name[ len ] = NULLCHAR;
        path->next = NULL;
        *owner = path;
        owner = &path->next;
    }
}


char_ring **RingEnd( char_ring **owner )
{
    while( *owner != NULL ) {
        owner = &(*owner)->next;
    }
    return( owner );
}


void AddSourceSpec( char *str )
{
    InsertRing( RingEnd( &SrcSpec ), str, strlen( str ) );
}

char *SourceName( char_ring *src )
{
    return( src->name );
}

char_ring *NextSourceSpec( char_ring *curr )
{
    if( curr == NULL ) return( SrcSpec );
    return( curr->next );
}


/*
 * SourceSet -- set the source path prefix and extension
 */

void SourceSet( void )
{
    char_ring **owner;
    char      *start;
    unsigned   len;

    if( CurrToken == T_DIV ) {
        Scan();
        if( ScanCmd( AddTab ) == 0 ) {
            Error( ERR_LOC, LIT( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_SET ) );
        }
        owner = RingEnd( &SrcSpec );
    } else {
        owner = &SrcSpec;
        FiniSource();
    }
    while( ScanItem( TRUE, &start, &len ) ) {
        while( len > 0 && *start == ' ' ) {
            ++start;
            --len;
        }
        InsertRing( owner, start, len );
    }
    DbgUpdate( UP_NEW_SRC );
}

void SourceConf( void )
{
    char_ring *curr;
    char *ptr;

    ptr = TxtBuff;
    for( curr = SrcSpec; curr != NULL; curr = curr->next ) {
        if( ptr != TxtBuff ) {
            *ptr = NULLCHAR;
            ConfigLine( TxtBuff );
            ptr = StrCopy( "/add ", TxtBuff );
        }
        *ptr++ = '{';
        ptr = StrCopy( curr->name, ptr );
        *ptr++ = '}';
    }
    *ptr = NULLCHAR;
    ConfigLine( TxtBuff );
}


/*
 * OpenSrcFile -- given a cue_handle, open the source file
 */

void *OpenSrcFile( cue_handle *ch )
{
    void        *hndl;
    char_ring   *path;
    char        *p;
    char        *d;
    char        *rem_name;
    bool        used_star;
    unsigned    len;
    char        *buff;

    len = CueFile( ch, NULL, 0 ) + 1;
    _AllocA( buff, len );
    CueFile( ch, buff, len );
    hndl = FOpenSource( buff, CueMod( ch ), CueFileId( ch ) );
    if( hndl != NULL ) return( hndl );
    for( path = SrcSpec; path != NULL; path = path->next ) {
        used_star = FALSE;
        d = TxtBuff;
        for( p = path->name; *p != '\0'; ++p ) {
            if( *p == '*' ) {
                used_star = TRUE;
                d += ModName( CueMod( ch ), d, TXT_LEN );
            } else {
                *d++ = *p;
            }
        }
        *d = NULLCHAR;
        if( !used_star ) {
            #if 0
                /*
                    John can't remember why he put this code in, and it
                    screws up when the user sets a source path of ".".
                    If we find some case where it's required, we'll have to
                    think harder about things.
                */
            if( *ExtPointer( TxtBuff, 0 ) != '\0' ) {
                *SkipPathInfo( TxtBuff, 0 ) = '\0';
            }
            #endif
            d = AppendPathDelim( TxtBuff, 0 );
            if( !IsAbsolutePath( buff ) ) {
                StrCopy( buff, d );
                hndl = FOpenSource( TxtBuff, CueMod( ch ), CueFileId( ch ) );
                if( hndl != NULL ) return( hndl );
            }
            /*
                We have a small problem here. We want to strip off the
                path information for the source file name, but we don't
                know if the file was compiled on the local system or the
                remote one. We'll kludge things by doing a local skip
                and then a remote one and seeing who takes off the most stuff.
                Don't even think about the case where the file has been
                compiled on a third, different type of file system.
            */
            p = SkipPathInfo( buff, OP_LOCAL );
            rem_name = SkipPathInfo( buff, OP_REMOTE );
            if( rem_name > p ) p = rem_name;
            d = StrCopy( p, d );
            *d = NULLCHAR;
        }
        hndl = FOpenSource( TxtBuff, CueMod( ch ), CueFileId( ch ) );
        if( hndl != NULL ) return( hndl );
    }
    return( NULL );
}
