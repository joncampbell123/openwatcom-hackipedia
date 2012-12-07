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
* Description:  Source code string literal processing.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "scan.h"
#include "asciiout.h"
#include "unicode.h"                                    /* 05-jun-91 */
#include <unistd.h>
#include <fcntl.h>

#ifndef _MAX_PATH
    #define _MAX_PATH (PATH_MAX+1)
#endif
#ifndef O_BINARY
    #define O_BINARY 0
#endif

extern TREEPTR         CurFuncNode;

static int RemoveEscapes( char *buf, const char *inbuf, size_t length );

static int OpenUnicodeFile( const char *filename )
{
    int         handle;
    char        fullpath[ _MAX_PATH ];

#if defined(__QNX__)
    _searchenv( filename, "ETC_PATH", fullpath );
    if( fullpath[0] == '\0' ) {
        #define ETC "/etc/"
        strcpy( fullpath, ETC );
        strcpy( &fullpath[ sizeof( ETC ) - 1 ], filename );
    }
#else
    _searchenv( filename, "PATH", fullpath );
#endif
    handle = -1;
    if( fullpath[0] != '\0' ) {
        handle = open( fullpath, O_RDONLY | O_BINARY );
    }
    return( handle );
}

static void ReadUnicodeFile( int handle )
{
    int             i;
    unsigned short  unicode_table[256];

    read( handle, unicode_table, 256 * sizeof( unsigned short ) );
    /* UniCode might be a FAR table */
    for( i = 0; i < 256; i++ ) {
        UniCode[i] = unicode_table[i];
    }
}

void LoadUnicodeTable( unsigned codePage )
{
    int         handle;
    char        filename[ 20 ];

    sprintf( filename, "unicode.%3.3u", codePage );
    if( filename[ 11 ] != '\0' ) {
        filename[ 7 ] = filename[ 8 ];
        filename[ 8 ] = '.';
    }
    handle = OpenUnicodeFile( filename );
    if( handle != -1 ) {
        ReadUnicodeFile( handle );
        close( handle );
    } else {
        CBanner();
        CErr2p( ERR_CANT_OPEN_FILE, filename );
    }
    return;
}

void StringInit( void )
{
    int     i;

    for( i = 0; i < STRING_HASH_SIZE; ++i ) {
        StringHash[i] = 0;
    }
}

void FreeLiteral( STRING_LITERAL *str_lit )
{
    CMemFree( str_lit->literal );
    CMemFree( str_lit );
}

STRING_LITERAL *GetLiteral( void )
{
    unsigned            len, len2;
    char                *s;
    STRING_LITERAL      *str_lit;
    STRING_LITERAL      *p;
    STRING_LITERAL      *q;
    int                 is_wide;

    /* first we store the whole string in a linked list to see if
       the end result is wide or not wide */
    p = str_lit = CMemAlloc( sizeof( STRING_LITERAL ) );
    q = NULL;
    is_wide = 0;
    do {
        /* if one component is wide then the whole string is wide */
        if( CompFlags.wide_char_string )
            is_wide = 1;
        if( q != NULL ) {
            p = CMemAlloc( sizeof( STRING_LITERAL ) );
            q->next_string = p;
        }
        q = p;
        p->length = CLitLength;
        p->next_string = NULL;
        p->literal = Buffer;
        Buffer = CMemAlloc( BufSize );
    } while( NextToken() == T_STRING );
    CompFlags.wide_char_string = is_wide;
    /* then remove escapes (C99: translation phase 5), and only then
       concatenate (translation phase 6), not the other way around! */
    len = 1;
    s = NULL;
    q = str_lit;
    do {
        len2 = RemoveEscapes( NULL, q->literal, q->length );
        --len;
        if( is_wide && len != 0 ) {
            --len;
        }
        s = CMemRealloc( s, len + len2 + 1 );
        RemoveEscapes( &s[len], q->literal, q->length );
        len += len2;
        p = q->next_string;
        if( q != str_lit )
            FreeLiteral( q );
        q = p;
    } while ( q );
    CLitLength = len;
    CMemFree( str_lit->literal );
    str_lit->literal = s;
    str_lit->length = len;
    str_lit->flags = 0;
    str_lit->cg_back_handle = 0;
    str_lit->ref_count = 0;
    return( str_lit );
}

static int RemoveEscapes( char *buf, const char *inbuf, size_t length )
{
    int                 c;
    size_t              j;
    bool                error;
    const unsigned char *end;
    const unsigned char *p = (const unsigned char *)inbuf;

    j = 0;
    error = FALSE;
    end = p + length;
    while( p < end ) {
        c = *p++;
        if( c == '\\' ) {
            c = ESCChar( *p, &p, &error );
            if( CompFlags.wide_char_string ) {
                if( buf != NULL )
                    buf[ j ] = c;
                ++j;
                c = c >> 8;                     /* 31-may-91 */
            }
        } else {
            if( CharSet[ c ] & C_DB ) {       /* if double-byte character */
                if( CompFlags.jis_to_unicode &&
                    CompFlags.wide_char_string ) {      /* 15-jun-93 */
                    c = (c << 8) + *p;
                    c = JIS2Unicode( c );
                    if( buf != NULL )
                        buf[ j ] = c;
                    c = c >> 8;
                } else {
                    if( buf != NULL )
                        buf[ j ] = c;
                    c = *p;
                }
                ++j;
                ++p;
            } else if( CompFlags.wide_char_string ) {
                if( CompFlags.use_unicode ) {   /* 05-jun-91 */
                    c = UniCode[ c ];
                } else if( CompFlags.jis_to_unicode ) {
                    c = JIS2Unicode( c );
                }
                if( buf != NULL )
                    buf[ j ] = c;
                ++j;
                c = c >> 8;
            } else {
                _ASCIIOUT( c );
            }
        }
        if( buf != NULL )
            buf[ j ] = c;
        ++j;
    }
    if( error && buf != NULL ) {                   /* 16-nov-94 */
        if( NestLevel == SkipLevel ) {
            CErr1( ERR_INVALID_HEX_CONSTANT );
        }
    }
    return( j );
}

static TYPEPTR StringLeafType( void )
{
    TYPEPTR     typ;

    if( CompFlags.wide_char_string ) {          /* 01-aug-91 */
        typ = ArrayNode( GetType( TYPE_USHORT ) );
        typ->u.array->dimension = CLitLength >> 1;
    } else if( StringArrayType != NULL ) {
        typ = StringArrayType;
        StringArrayType->u.array->dimension = CLitLength;
        StringArrayType = NULL;
    } else {
        typ = ArrayNode( GetType( TYPE_PLAIN_CHAR ) );  /* 25-nov-94 */
        typ->u.array->dimension = CLitLength;
    }
    return( typ );
}


static unsigned CalcStringHash( STRING_LITERAL *lit )
{
    return( hashpjw( lit->literal ) % STRING_HASH_SIZE );
}

TREEPTR StringLeaf( int flags )
{
    STRING_LITERAL      *new_lit;
    STRING_LITERAL      *strlit;
    TREEPTR             leaf_index;
    unsigned            hash;

    strlit = NULL;
    new_lit = GetLiteral();
    if( TargetSwitches & BIG_DATA ) {                   /* 06-oct-88 */
        if( !CompFlags.strings_in_code_segment ) {      /* 01-sep-89 */
            if( new_lit->length > DataThreshold ) {
                flags |= FLAG_FAR;
            }
        }
    }
    if( CompFlags.wide_char_string )
        flags |= STRLIT_WIDE;
    if( flags & FLAG_FAR )
        CompFlags.far_strings = 1;
    hash = CalcStringHash( new_lit );
    if( Toggles & TOGGLE_REUSE_DUPLICATE_STRINGS ) {    /* 24-mar-92 */
        for( strlit = StringHash[ hash ]; strlit != NULL; strlit = strlit->next_string ) {
            if( strlit->length == new_lit->length && strlit->flags == flags ) {
                if( memcmp( strlit->literal, new_lit->literal, new_lit->length ) == 0 ) {
                    break;
                }
            }
        }
    }
    if( strlit == NULL ) {
        new_lit->flags = flags;
        ++LitCount;
        LitPoolSize += CLitLength;
        new_lit->next_string = StringHash[ hash ];
        StringHash[ hash ] = new_lit;
    } else {            // we found a duplicate
        FreeLiteral( new_lit );
        new_lit = strlit;
    }

    leaf_index = LeafNode( OPR_PUSHSTRING );
    leaf_index->op.string_handle = new_lit;
    // set op.flags field
    leaf_index->expr_type = StringLeafType();

    if( CurFunc != NULL ) {                             /* 22-feb-92 */
        CurFuncNode->op.func.flags &= ~FUNC_OK_TO_INLINE;
    }
    return( leaf_index );
}
