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
* Description:  Simplified Causeway Executable Compressor
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "watcom.h"
#include "bool.h"
#include "exedos.h"

#define RepMinSize      2
#define BUFFER_LEN      0x1000
#define CWCStackSize    1024    // cannot exceed EXECopyStubLen size (1135 bytes)

#define ALLIGN_LEN_PARA(x)      ((x+15)&~15)
#define PADDING_LEN_PARA(x)     (ALLIGN_LEN_PARA(x)-x)

#include "pushpck1.h"

typedef struct cwc_info {
    unsigned_16 SourceSeg;
    unsigned_16 CopyLen;
    unsigned_16 EntryIP;
    unsigned_16 EntryCS;
    unsigned_32 ImageLen;
    unsigned_16 EntryES;
} cwc_info;

typedef struct cwc_dec_info {
    unsigned_8  IDText[3];
    unsigned_8  Bits;
    unsigned_32 Len;
    unsigned_32 Size;
} cwc_dec_info;

#include "poppck.h"

typedef struct s2 {
    struct s2   *next;
    unsigned_8  *data;
} s2;

typedef struct s1 {
    s2 *head;
    s2 *tail;
} s1;

static cwc_dec_info  cwc_dec = { "CWC", 12, 0, 0 };

static unsigned_32   EncodeLit = 0;

static unsigned_8    *OutBuffer = NULL;
static unsigned_32   OutPos = 0;
static unsigned_32   OutTotal;
static unsigned_16   OutWord;
static unsigned_32   OutBitCount;
static unsigned_32   OutCtrl;
static unsigned_32   SigOutCount;

static unsigned_8 copy_stub[ 1167 ] = {
    250,22,31,38,161,2,0,131,232,64,142,208,251,6,22,7,
    190,240,3,139,254,185,16,0,243,164,7,54,140,6,252,3,
    139,216,140,202,54,3,22,240,3,54,139,46,242,3,253,139,
    197,61,0,16,118,3,184,0,16,43,232,43,208,43,216,142,
    218,142,195,177,3,211,224,139,200,209,224,72,72,139,240,139,
    248,243,165,11,237,117,216,140,198,142,222,51,246,140,207,142,
    199,51,255,140,216,54,1,6,246,3,54,255,46,244,3,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static unsigned_8 decomp_stub[ 461 ] = {
    252,87,6,232,137,0,7,95,54,161,248,3,54,139,30,250,
    3,209,235,209,216,209,235,209,216,209,235,209,216,209,235,209,
    216,140,194,3,208,142,218,54,139,54,248,3,131,230,15,30,
    86,139,76,6,131,198,27,11,201,116,35,86,30,6,139,68,
    2,139,52,140,195,3,195,142,192,38,1,28,7,31,94,73,
    131,198,4,117,226,140,216,5,0,16,142,216,235,217,94,31,
    140,195,139,68,14,3,195,139,208,139,108,16,139,68,22,3,
    195,46,163,203,1,139,68,20,46,163,201,1,54,142,6,252,
    3,54,142,30,252,3,142,210,139,229,46,255,46,201,1,50,
    237,138,76,3,184,1,0,211,224,72,46,163,80,1,128,233,
    8,46,136,14,70,1,131,198,12,139,44,131,198,2,178,16,
    138,242,19,237,254,202,117,6,139,44,70,70,138,214,115,9,
    138,4,38,136,5,70,71,235,233,19,237,254,202,117,6,139,
    44,70,70,138,214,115,65,51,201,19,237,254,202,117,6,139,
    44,70,70,138,214,18,201,19,237,254,202,117,6,139,44,70,
    70,138,214,18,201,128,193,2,51,192,138,4,70,72,120,16,
    3,193,86,30,6,31,139,247,43,240,243,164,31,94,235,162,
    138,4,70,65,243,170,235,154,19,237,254,202,117,6,139,44,
    70,70,138,214,115,26,51,192,139,4,131,198,2,138,232,177,
    4,211,232,138,205,131,225,15,128,193,2,72,121,194,235,208,
    139,68,1,138,232,177,4,211,232,138,12,131,198,3,129,225,
    255,15,131,193,2,72,121,168,131,249,18,115,179,128,249,4,
    116,60,128,249,3,116,22,128,249,5,116,7,128,249,2,116,
    87,235,85,50,237,138,12,70,243,164,233,53,255,129,254,0,
    192,115,3,233,44,255,139,198,209,232,209,232,209,232,209,232,
    83,140,219,3,195,142,216,131,230,15,91,233,20,255,129,255,
    0,192,115,3,233,11,255,139,199,45,0,192,80,83,209,232,
    209,232,209,232,209,232,140,195,3,195,142,192,91,88,43,248,
    131,224,15,3,248,233,234,254,195,0,0,0,0
};

static char padding[ 16 ] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

bool InitOutFileBuffer( FILE *fo )
/********************************/
{
    OutTotal = 0;
    OutCtrl = 0;
    OutBitCount = 0;
    OutWord = 0;
    OutPos = 2;
    SigOutCount = 2;

    OutBuffer =  malloc( 2 * BUFFER_LEN );
    if( OutBuffer == NULL )
        return( FALSE );
    return( TRUE );
}

void FiniOutFileBuffer( FILE *fo )
/********************************/
{
    if( OutBuffer != NULL ) {
        if( OutBitCount ) {
            *(unsigned_16 *)( OutBuffer + OutCtrl ) = OutWord << ( 16 - OutBitCount );
        }
        fwrite( OutBuffer, 1, OutPos, fo );
        OutTotal += OutPos;
        OutPos = 0;
        free( OutBuffer );
        OutBuffer = NULL;
    }
}

void WriteBit( FILE *fo, int b )
/******************************/
{
    // Write a bit to output.
    OutWord <<= 1;
    OutWord |= b;
    OutBitCount++;
    if( OutBitCount >= 16 ) {
        OutBitCount = 0;
        *(unsigned_16 *)( OutBuffer + OutCtrl ) = OutWord;
        if( OutPos >= BUFFER_LEN ) {
            fwrite( OutBuffer, 1, OutPos, fo );
            OutTotal += OutPos;
            OutPos = 0;
        }
        OutCtrl = OutPos;
        OutPos += 2;
        SigOutCount += 2;
    }
}

void WriteByte( unsigned char c )
/*******************************/
{
    OutBuffer[ OutPos++ ] = c;
    SigOutCount++;
}

void WriteTerminateCmd( FILE *fo )
/********************************/
{
    WriteBit( fo, 0 );
    WriteBit( fo, 0 );
    WriteBit( fo, 0 );
    WriteByte( 0 );
    WriteByte( 0 );
    WriteByte( 0 );
}

void WriteFlushInpBufCmd( FILE *fo )
/**********************************/
{
    WriteBit( fo, 0 );
    WriteBit( fo, 0 );
    WriteBit( fo, 0 );
    WriteByte( 1 );
    WriteByte( 0 );
    WriteByte( 0 );
}

void WriteFlushOutBufCmd( FILE *fo )
/**********************************/
{
    WriteBit( fo, 0 );
    WriteBit( fo, 0 );
    WriteBit( fo, 0 );
    WriteByte( 2 );
    WriteByte( 0 );
    WriteByte( 0 );
}


void WriteStringCmd( FILE *fo )
/*****************************/
{
    WriteBit( fo, 0 );
    WriteBit( fo, 0 );
    WriteBit( fo, 0 );
    WriteByte( 3 );
    WriteByte( 0 );
    WriteByte( 0 );
}

int EncodeFile( FILE *fo, unsigned_8 *data, unsigned_32 data_size )
/*****************************************************************/
{
    unsigned_32 len;
    unsigned_32 trail_len;
    unsigned_32 xlen1;
    unsigned_32 item_len;
    unsigned_32 len1;
    unsigned_32 pos;
    unsigned_8  *p1;
    unsigned_8  *p2;
    unsigned_8  c;
    unsigned_32 SigInCount;
    unsigned_32 OutStart;
    unsigned_32 RepLength;
    unsigned_32 FoundLen;
    unsigned_8  *FoundPos;
    unsigned_32 RepMaxLen;
    unsigned_32 RepMaxSize;
    unsigned_16 CodeLast;
    unsigned_32 CodeCount;
    s1          CodeFree;
    s1          *CodeHeads;
    s1          *code;
    s2          *new;
    s2          *last;
    s2          *s;

// Encode file

    // set input data len
    cwc_dec.Len = data_size;
    RepMaxLen = ( 1 << cwc_dec.Bits ) + RepMinSize - 1;
    RepMaxSize = ( 1 << ( 24 - cwc_dec.Bits ) ) + RepMinSize - 1;

    if( !InitOutFileBuffer( fo ) )
        return( 0 );

    SigInCount = 0;

    CodeLast = 0;
    CodeCount = 0;
    CodeHeads = NULL;
    CodeFree.head = NULL;
    CodeFree.tail = NULL;

    // allocate memory blocks

    CodeHeads = malloc( 0x10000 * sizeof( s1 ) );
    if( CodeHeads == NULL ) {
        FiniOutFileBuffer( fo );
        return( 0 );
    } else {
        memset( CodeHeads, 0, 0x10000 * sizeof( s1 ) );
        CodeFree.head = malloc( ( RepMaxSize + 8 ) * sizeof( s2 ) );
        if( CodeFree.head == NULL ) {
            free( CodeHeads );
            CodeHeads = NULL;
            FiniOutFileBuffer( fo );
            return( 0 );
        } else {
            CodeFree.tail = CodeFree.head;
            for( new = CodeFree.head, len = 0; len < RepMaxSize + 7; ++len, ++new ) {
                new->next = new + 1;
            }
            new->next = 0;
        }
    }
    OutStart = ftell( fo );
    fwrite( &cwc_dec, sizeof( cwc_dec ), 1, fo );

    trail_len = 0;         // length of trailing data.
    for( xlen1 = data_size; xlen1; xlen1 -= item_len ) {
        // Search for a run of bytes.
        FoundLen = 0;
        RepLength = 0;
        if( EncodeLit == 0 ) {
            p1 = data;
            len = ( xlen1 > RepMaxLen ) ? RepMaxLen : xlen1;
            c = *p1++;
            while( --len && c == *p1++ ) ;
            RepLength = p1 - data - 1;

            // Search buffer for biggest matching string.

            if( xlen1 >= RepMinSize ) {
                len = RepMinSize;
                for( s = CodeHeads[ *(unsigned_16 *)data ].head; s != NULL; s = s->next ) {
                    p1 = data;
                    p2 = s->data;
                    len1 = p1 - p2;
                    if( len1 < len )
                        continue;
                    if( len == RepMinSize ) {
                        p1 += 2;
                        p2 += 2;
                        len1 -= 2;
                    } else if( memcmp( p1 + 2, p2 + 2, len - 2 ) != 0 ) {
                        continue;
                    } else {
                        p1 += len;
                        p2 += len;
                    }
                    do {
                        ++len;
                    } while( len <= len1  && len <= xlen1 && len <= RepMaxLen && *p1++ == *p2++ ) ;
                    len--;
                    FoundPos = s->data;
                    FoundLen = len;
                }
            }
        }
        if( RepLength >= RepMinSize + 1 && RepLength > FoundLen  ) {
            item_len = RepLength;
            len = RepLength - RepMinSize - 1;
            WriteBit( fo, 0 );
            if( RepLength < RepMinSize + 1 + 3 + 1 ) {

                // Send run as zero distance, 2 bit length, byte to run.

                WriteBit( fo, 1 );
                WriteBit( fo, ( len >> 1 ) & 1 );
                WriteBit( fo, len & 1 );
                WriteByte( 0 );
            } else if( RepLength < RepMinSize + 1 + 15 + 1 ) {

                // send run as 12 bit zero, 4 bit length, byte run.

                WriteBit( fo, 0 );
                WriteBit( fo, 1 );
                WriteByte( len );
                WriteByte( len >> 8 );
            } else {

                // Send 12-bit zero, 12 bit length, byte run.

                WriteBit( fo, 0 );
                WriteBit( fo, 0 );
                WriteByte( len );
                WriteByte( len >> 8 );
                WriteByte( len >> 16 );
            }
            WriteByte( *data );
        } else {
            item_len = FoundLen;
            pos = data - FoundPos - FoundLen + 1;
            if( FoundLen >= RepMinSize && FoundLen < RepMinSize + 3 + 1 && pos < 0x00FF + 1 ) {

                // Send repeat as byte distance, 2 bits for length.

                WriteBit( fo, 0 );
                WriteBit( fo, 1 );
                len = FoundLen - RepMinSize;
                WriteBit( fo, ( len >> 1 ) & 1 );
                WriteBit( fo, len & 1 );
                WriteByte( pos );
            } else if( FoundLen >= RepMinSize + 1 && FoundLen < RepMinSize + 15 + 1 && pos < 0x0FFF + 1 ) {

                // Send repeat as 12 bit position, 4 bit length.

                WriteBit( fo, 0 );
                WriteBit( fo, 0 );
                WriteBit( fo, 1 );
                pos <<= 4;
                pos |= FoundLen - RepMinSize;
                WriteByte( pos );
                WriteByte( pos >> 8 );
            } else if( FoundLen >= RepMinSize + 2 ) {

                // Send repeat as 12-bit position, 12 bit length.

                WriteBit( fo, 0 );
                WriteBit( fo, 0 );
                WriteBit( fo, 0 );
                pos <<= cwc_dec.Bits;
                pos |= FoundLen - RepMinSize;
                WriteByte( pos );
                WriteByte( pos >> 8 );
                WriteByte( pos >> 16 );
            } else if( EncodeLit == 0 ) {

                // Do normal single raw byte.

                WriteBit( fo, 1 );
                WriteByte( *data );
                item_len = 1;
            } else {

                // Send literal string code.

                WriteStringCmd( fo );

                // Send the length.

                if( EncodeLit > 0x00FF ) {
                    len = 0x00FF;
                } else {
                    len = EncodeLit;
                }
                if( len > xlen1 ) 
                    len = xlen1;

                WriteByte( len );
                EncodeLit -= len;

                // Send the actual string.

                item_len = len;
                for( p1 = data; len; --len ) {
                    WriteByte( *p1++ );
                }
            }
        }

        // Update search control data

        for( len = item_len; len; --len ) {
            // Lose a code

            if( trail_len < RepMaxSize ) {
                ++trail_len;
            } else {
                code = CodeHeads + *(unsigned_16 *)( data - trail_len );
                if( code->head == NULL )
                    // Error
                    return( -1 );
                s = code->head;
                if( code->head == code->tail )
                    code->tail = s->next;
                code->head = s->next;
                s->next = CodeFree.head;
                CodeFree.head = s;
            }

            // Add a code

            CodeLast >>= 8;		// rotate code buffer.
	    CodeLast |= *data << 8;
	    ++CodeCount;
            if( CodeCount >= 2 ) {
                new = CodeFree.head;
                CodeFree.head = new->next;
                new->next = NULL;
                new->data = data - 1;
                code = CodeHeads + CodeLast; 
                last = code->tail;
                if( last != NULL ) {
                    last->next = new;
                }
                code->tail = new;
                if( code->head == NULL ) {
                    code->head = new;
                }
            }
            ++data;
        }
        SigInCount += item_len;

        if( SigOutCount >= 0x2000-0x0010 ) {

            // Send input flush code.

            WriteFlushInpBufCmd( fo );
            SigOutCount = 0;
        }
        if( SigInCount >= 0x2000-0x0010 ) {

            // Send output flush code.

            WriteFlushOutBufCmd( fo );
            SigInCount = 0;
        }
    }

    // Send terminator code.

    WriteTerminateCmd( fo );

    // Make sure we sent everything.

    FiniOutFileBuffer( fo );
    cwc_dec.Size = OutTotal;
    pos = ftell( fo );
    fseek( fo, OutStart, SEEK_SET );
    fwrite( &cwc_dec, sizeof( cwc_dec ), 1, fo );
    fseek( fo, pos, SEEK_SET );
    OutTotal += sizeof( cwc_dec );
    if( CodeHeads != NULL ) {
        free( CodeHeads );
        CodeHeads = NULL;
    }
    if( CodeFree.tail != NULL ) {
        free( CodeFree.tail );
        CodeFree.tail = NULL;
    }
    // return output size.
    return( OutTotal );
}

int ProcessEXE( char *fname, char *oname )
/****************************************/
{
    FILE            *f;
    FILE            *fo;
    dos_exe_header  exe_header;
    unsigned_8      *fimg;
    unsigned_32     ImageLen;
    unsigned_32     ExeLen;
    unsigned_32     HeaderLen;
    unsigned_32     TotalLen;
    unsigned_32     CompressLen;
    signed_32       mem_req;
    cwc_info        cwc_data;
    char            temp_fname[ L_tmpnam ];

    f = fopen( fname, "rb" );
    if( f == NULL ) {
        printf( "File %s can not be opened.", fname );
        return( -1 );
    }
    // read header
    fread( &exe_header, sizeof( exe_header ), 1, f );
    HeaderLen = exe_header.hdr_size * 16;
    // calculate image size
    if( exe_header.mod_size == 0 ) {
        ExeLen = exe_header.file_size * 512L;
    } else {
        ExeLen = ( exe_header.file_size - 1 ) * 512L + exe_header.mod_size;
    }
    ImageLen = ExeLen - HeaderLen;
    fimg = malloc( ExeLen );
    // read image
    fseek( f, HeaderLen, SEEK_SET );
    fread( fimg, ImageLen, 1, f );
    // copy header
    memcpy( fimg + ImageLen, &exe_header, sizeof( exe_header ) );
    // read reloc info
    fseek( f, exe_header.reloc_offset, SEEK_SET );
    fread( fimg + ImageLen + sizeof( exe_header ) - 1, exe_header.num_relocs, 4, f );
    fclose( f );
    TotalLen = ImageLen + sizeof( exe_header ) - 1 + exe_header.num_relocs * 4;
    //
    tmpnam( temp_fname );
    fo = fopen( temp_fname, "wb" );
    if( fo == NULL ) {
        printf( "Temporary file %s can not be created.", temp_fname );
        fclose( f );
        return( -1 );
    }
    // write header
    fwrite( &exe_header, sizeof( exe_header ), 1, fo );
    // allign to para
    fwrite( padding, 1, PADDING_LEN_PARA( sizeof( exe_header ) ), fo );
    // write copy stub code
    fwrite( copy_stub, sizeof( copy_stub ), 1, fo );
    // allign to para
    fwrite( padding, 1, PADDING_LEN_PARA( sizeof( copy_stub ) ), fo );
    // encode image data
    CompressLen = EncodeFile( fo, fimg, TotalLen );
    // allign to para
    fwrite( padding, 1, PADDING_LEN_PARA( CompressLen ), fo );
    // write decompress stub code
    fwrite( decomp_stub, sizeof( decomp_stub ), 1, fo );
    // allign to para
    fwrite( padding, 1, PADDING_LEN_PARA( sizeof( decomp_stub ) ), fo );
    // update exe header
    TotalLen = ftell( fo );
    mem_req = ImageLen + exe_header.num_relocs * 4 + sizeof( exe_header ) - 1 + ALLIGN_LEN_PARA( sizeof( decomp_stub ) ) + CWCStackSize;
    mem_req -= TotalLen - 0x20;
    if( mem_req < 0 )
        mem_req = TotalLen - 0x20;
    exe_header.mod_size = TotalLen % 512;
    exe_header.file_size = TotalLen / 512;
    if( exe_header.mod_size != 0 )
        exe_header.file_size++;
    exe_header.num_relocs = 0;
    exe_header.hdr_size = 2;
    exe_header.min_16 += ( mem_req >> 4 ) + 1;
    exe_header.SS_offset = ( ALLIGN_LEN_PARA( sizeof( copy_stub ) ) - CWCStackSize ) >> 4;
    exe_header.SP = CWCStackSize - 0x0010;
    exe_header.IP = 0;
    exe_header.CS_offset = 0;
    exe_header.reloc_offset = 0x20;
    fseek( fo, 0, SEEK_SET );
    fwrite( &exe_header, sizeof( exe_header ), 1, fo );
    // update copy stub data
    fseek( fo, ALLIGN_LEN_PARA( sizeof( copy_stub ) + 0x20 ) - 16, SEEK_SET );
    cwc_data.SourceSeg = ( ALLIGN_LEN_PARA( sizeof( copy_stub ) ) + ALLIGN_LEN_PARA( CompressLen ) + ALLIGN_LEN_PARA( sizeof( decomp_stub ) ) ) >> 4;
    cwc_data.CopyLen = ( ALLIGN_LEN_PARA( CompressLen ) + ALLIGN_LEN_PARA( sizeof( decomp_stub ) ) ) >> 4;
    cwc_data.EntryIP = 0;
    cwc_data.EntryCS = ALLIGN_LEN_PARA( CompressLen ) >> 4;
    cwc_data.ImageLen = ImageLen;
    cwc_data.EntryES = 0;
    fwrite( &cwc_data, sizeof( cwc_data ), 1, fo );
    fclose( fo );
    if( oname == NULL )
        oname = fname;
    remove( oname );
    rename( temp_fname, oname);
    return( 0 );
}

int usage( void )
/***************/
{
    printf( "Causeway Executable Compressor\n" );
    printf( "Usage: bcwc <input file name> [<output file name>]\n" );
    return( -1 );
}

int main( int argc, char **argv)
/******************************/
{
    if( argc >= 2 && argc <= 3 ) {
        return( ProcessEXE( argv[ 1 ], argv[ 2 ] ) );
    } else {
        return( usage() );
    }
}
