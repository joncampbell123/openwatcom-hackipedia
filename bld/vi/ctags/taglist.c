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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include "ctags.h"

#define isWSorCtrlZ(x)  (isspace( x ) || (x == 0x1A))

static char     **tagList;

/*
 * addToTagList - add new string to tag list
 */
static void addToTagList( char *res )
{
    int         len;

    len = strlen( res ) + 1;
    tagList = realloc( tagList, (TagCount + 1) * sizeof( char * ) );
    if( tagList == NULL ) {
        ErrorMsgExit( "Out of memory!\n" );
    }
    tagList[TagCount] = malloc( len );
    if( tagList[TagCount] == NULL ) {
        ErrorMsgExit( "Out of memory!\n" );
    }
    memcpy( tagList[TagCount], res, len );
    TagCount++;

} /* addToTagList */


/*
 * AddTag - add a new tag to our list
 */
void AddTag( char *id )
{
    char        *fname;
    char        *linedata;
    char        res[MAX_STR];

    linedata = GetCurrentLineDataPtr();
    fname = GetCurrentFileName();
#ifdef __ENABLE_FNAME_PROCESSING__
    if( !strnicmp( id, "__F_NAME", 8 ) ) {
        char *ptr;

        id = strchr( id, '(' );
        if( id ) {
            ++id;
            ptr = res;

            while( *id && isspace( *id ) ) {
                ++id;
            }

            while( *id && (*id != ',') ) {
                *ptr++ = *id++;
            }

            sprintf( ptr, "\t%s\t/%s/", fname, linedata );
            addToTagList( res );

            if( *id == ',' ) {
                ++id;
                ptr = res;

                while( *id && isspace( *id ) ) {
                    ++id;
                }

                while( *id && (*id != ')') ) {
                    *ptr++ = *id++;
                }

                sprintf( ptr, "\t%s\t/%s/", fname, linedata );
                addToTagList( res );
            }
        }
    } else {
#endif //__ENABLE_FNAME_PROCESSING__
        sprintf( res, "%s\t%s\t/%s/", id, fname, linedata );
        addToTagList( res );
#ifdef __ENABLE_FNAME_PROCESSING__
    }
#endif //__ENABLE_FNAME_PROCESSING__

} /* AddTag */

/*
 * CompareStrings - quicksort comparison
 */
int CompareStrings( char **p1, char **p2 )
{
    return( strcmp( *p1, *p2 ) );

} /* CompareStrings */

/*
 * GenerateTagsFile - emit a sorted tags file
 */
void GenerateTagsFile( char *fname )
{
    FILE        *f;
    int         i;
    long        total;

    qsort( tagList, TagCount, sizeof( char * ),
           (int (*)( const void*, const void* ))CompareStrings );
    f = fopen( fname, "w" );
    if( f == NULL ) {
        ErrorMsgExit( "Could not open tags file \"%s\"\n", fname );
    }
    if( tagList == NULL ) {
        total = 0;
    } else {
        total = _msize( tagList );
    }
    for( i = 0; i < TagCount; i++ ) {
        fprintf( f, "%s\n", tagList[i] );
        total += _msize( tagList[i] );
    }
    fclose( f );
    if( VerboseFlag ) {
        printf( "Wrote %u tags.\n", TagCount );
        printf( "Used %ld bytes to store tags.\n", total );
    }

} /* GenerateTagsFile */

/*
 * ReadExtraTags - read in extra tags from tags file
 */
void ReadExtraTags( char *fname )
{
    FILE        *f;
    char        res[MAX_STR];
    int         i;

    f = fopen( fname, "r" );
    if( f == NULL ) {
        return;
    }
    while( fgets( res, sizeof( res ), f ) != NULL ) {
        for( i = strlen( res ); i && isWSorCtrlZ( res[i - 1] ); --i ) {
            res[i - 1] = '\0';
        }
        addToTagList( res );
    }
    fclose( f );

} /* ReadExtraTags */
