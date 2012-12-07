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


#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define _IN_ICMASK      // for ic.h to include everything

#include "wcpp.h"
#include "icodes.h"
#include "inames.h"

static char *usageMsg[] = {
    "icmask <ic-h> <use-ic-source> <use-ic-source> ...",
    "where:",
    "    <ic-h> is ic.h",
    "    <use-ic-source> is source code that selectively reads IC codes",
    NULL
};

#define BUFF_SIZE 1024

static char *file;
static unsigned line;
static FILE *ic_h;
static FILE *src_c;
static FILE *out_h;
static char buff[BUFF_SIZE];

typedef struct name NAME;
struct name {
    NAME        *next;
    char        name[1];
};
static NAME *icMaskTable[IC_END+1];
static NAME *icPreProcTable[IC_END+1];

static void addName( NAME **t, unsigned ic, char *name )
{
    NAME *m;
    size_t len;

    for( m = t[ic]; m != NULL; m = m->next ) {
        if( strcmp( m->name, name ) == 0 ) {
            return;
        }
    }
    len = strlen( name );
    m = malloc( sizeof( *m ) + len );
    m->next = t[ic];
    t[ic] = m;
    strcpy( m->name, name );
}

static void fail( char *msg, ... )
{
    va_list args;

    if( line ) {
        fprintf( stderr, "error on line %u of %s\n", line, file );
    }
    va_start( args, msg );
    vfprintf( stderr, msg, args );
    va_end( args );
    exit( EXIT_FAILURE );
}

static void dumpUsage( void )
{
    char **p;

    for( p = usageMsg; *p; ++p ) {
        fprintf( stderr, "%s\n", *p );
    }
}

static char *skipNonSpace( char *p )
{
    while( *p && !isspace( *p ) ) {
        ++p;
    }
    return( p );
}

static char *skipMacroName( char *p )
{
    if( *p && ( isalpha( *p ) || *p == '_' )) {
        ++p;
    }
    while( *p && ( isalnum( *p ) || *p == '_') ) {
        ++p;
    }
    return( p );
}

static char *skipSpace( char *p )
{
    while( *p && isspace( *p ) ) {
        ++p;
    }
    return( p );
}

static unsigned whatICAmI( char *p )
{
    char *s;
    char **f;
    char c;

    s = p;
    while( *p && ( isalnum( *p ) || *p == '_' ) ) {
        ++p;
    }
    c = *p;
    *p = '\0';
    for( f = ic_names; f <= &ic_names[ IC_END ]; ++f ) {
        if( strcmp( *f, s ) == 0 ) {
            *p = c;
            return( f - ic_names );
        }
    }
    fail( "cannot find IC '%s'\n", s );
    return( -1 );
}

static void addICMasks( unsigned ic )
{
    char *p;
    char *s;
    char c;

    p = buff;
    p = skipNonSpace( p );
    for(;;) {
        p = skipSpace( p );
        if( *p == '\0' ) break;
        s = p;
        while( *p && ( isalnum( *p ) || *p == '_' ) ) {
            ++p;
        }
        c = *p;
        *p = '\0';
        addName( icMaskTable, ic, s );
        *p = c;
    }
}

static void processIC_H( char *fname )
{
    char *p;
    char *ok;
    char *ic_start;
    unsigned ic_idx;

    file = fname;
    line = 0;
    ic_idx = -1;
    for(;;) {
        ok = fgets( buff, sizeof(buff), ic_h );
        if( ok == NULL ) break;
        ++line;
        p = skipSpace( buff );
        if( memcmp( p, "//", 2 ) == 0 ) {
            if( p[2] != '|' ) {
                continue;
            }
        }
        ic_start = strstr( p, "IC(" );
        if( ic_start != NULL ) {
            p = skipNonSpace( ic_start );
            p = skipSpace( p );
            ic_idx = whatICAmI( p );
            continue;
        }
        if( ic_idx != -1 ) {
            if( memcmp( p, "//| ", 4 ) == 0 ) {
                addICMasks( ic_idx );
            }
            if( buff[0] == '#' ) {
                addName( icPreProcTable, ic_idx, buff );
            }
        }
    }
}

static void dumpHeader( char **argv )
{
    fprintf( out_h, "/* generated by ICMASK reading" );
    while( *argv ) {
        fprintf( out_h, " %s", *argv );
        ++argv;
    }
    fprintf( out_h, " */\n" );
}

static void outputIMASK_H( char **argv )
{
    NAME **h;
    NAME *m;
    NAME *n;

    // output name cannot match icmask.* because makefile execs del icmask.*
    out_h = fopen( "ic_mask.gh", "w" );
    if( out_h == NULL ) fail( "cannot open 'icmask.gh' for output\n" );
    dumpHeader( argv );
    for( h = &icMaskTable; h <= &icMaskTable[IC_END]; ++h ) {
        if( *h != NULL ) {
            for( m = *h; m != NULL; m = n ) {
                n = m->next;
                if( n != NULL ) {
                    fprintf( out_h, "ICOPM_%s|", m->name );
                } else {
                    fprintf( out_h, "ICOPM_%s, /* %s */\n", m->name, ic_names[ h - icMaskTable ] );
                }
            }
        } else {
            fprintf( out_h, "0, /* %s */\n", ic_names[ h - icMaskTable ] );
        }
        if( icPreProcTable[ h - icMaskTable ] != NULL ) {
            fprintf( out_h, "%s\n", icPreProcTable[ h - icMaskTable ]->name );
        }
    }
    fclose( out_h );
}

static void scanSource( char *fname )
{
    char *mask_id;
    char *ok;
    char *t;
    char *p;
    char *s;
    char c;
    int begin_found;
    unsigned ic_idx;

    file = fname;
    line = 0;
    begin_found = 0;
    mask_id = NULL;
    for(;;) {
        ok = fgets( buff, sizeof(buff), src_c );
        if( ok == NULL ) break;
        ++line;
        if( begin_found ) {
            t = strstr( buff, "// ICMASK END" );
            if( t != NULL ) {
                begin_found = 0;
                continue;
            }
            p = skipSpace( buff );
            if( memcmp( p, "case", 4 ) == 0 ) {
                p = skipNonSpace( p );
                p = skipSpace( p );
                if( memcmp( p, "IC_", 3 ) == 0 ) {
                    ic_idx = whatICAmI( p );
                    addName( icMaskTable, ic_idx, mask_id );
                }
            }
        } else {
            t = strstr( buff, "// ICMASK BEGIN" );
            if( t != NULL ) {
                p = skipNonSpace( t );
                p = skipSpace( p );
                p = skipNonSpace( p );
                p = skipSpace( p );
                p = skipNonSpace( p );
                p = skipSpace( p );
                s = p;
                p = skipMacroName( p );
                c = *p;
                *p = '\0';
                mask_id = strdup( s );
                *p = c;
                begin_found = 1;
            }
        }
    }
    if( begin_found ) {
        fail( "// ICMASK BEGIN found without matching // ICMASK END\n" );
    }
}

int main( int argc, char **argv ) {
    char **src_file;

    if( argc < 3 ) {
        dumpUsage();
        fail( "invalid arguments\n" );
    }
    ic_h = fopen( argv[1], "r" );
    if( ic_h == NULL ) fail( "cannot open '%s' for input\n", argv[1] );
    processIC_H( argv[1] );
    fclose( ic_h );
    for( src_file = &argv[2]; *src_file; ++src_file ) {
        src_c = fopen( *src_file, "r" );
        if( src_c == NULL ) fail( "cannot open '%s' for input\n", *src_file );
        scanSource( *src_file );
        fclose( src_c );
    }
    outputIMASK_H( &argv[1] );
    return( EXIT_SUCCESS );
}
