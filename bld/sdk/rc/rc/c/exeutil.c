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
* Description:  Resource Compiler executable processing utility functions.
*
****************************************************************************/


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "global.h"
#include "exeutil.h"
#include "iortns.h"

/*
 * CopyExeData
 * NB When an error occurs the function MUST return without altering errno
 */
extern RcStatus CopyExeData( int inhandle, int outhandle, uint_32 length )
/************************************************************************/
{
    uint    numio;      /* number of bytes read or wrote */

    if (length == 0) {
        return( RS_PARAM_ERROR );
    }

    while (length > IO_BUFFER_SIZE) {
        numio = RcRead( inhandle, Pass2Info.IoBuffer, IO_BUFFER_SIZE );
        if (numio != IO_BUFFER_SIZE) {
            if( numio == -1 ) {
                return( RS_READ_ERROR );
            } else {
                return( RS_READ_INCMPLT );
            }
        }
        length -= IO_BUFFER_SIZE;
        numio = RcWrite( outhandle, Pass2Info.IoBuffer, IO_BUFFER_SIZE );
        if (numio != IO_BUFFER_SIZE) {
            return( RS_WRITE_ERROR );
        }
    }

    numio = RcRead( inhandle, Pass2Info.IoBuffer, length );
    if (numio != length) {
        if( numio == -1 ) {
            return( RS_READ_ERROR );
        } else {
            return( RS_READ_INCMPLT );
        }
    }
    numio = RcWrite( outhandle, Pass2Info.IoBuffer, length );
    if (numio != length) {
        return( RS_WRITE_ERROR );
    }

    return( RS_OK );
} /* CopyExeData */

/*
 * CopyExeDataTilEOF
 * NB when an error occurs this function MUST return without altering errno
 */
extern RcStatus CopyExeDataTilEOF( int inhandle, int outhandle )
/***************************************************************/
{
    uint    numread;
    uint    numwrote;

    numread = RcRead( inhandle, Pass2Info.IoBuffer, IO_BUFFER_SIZE );
    if (numread == -1) {
        return( RS_READ_ERROR );
    }

    while (numread > 0) {
        numwrote = RcWrite( outhandle, Pass2Info.IoBuffer, numread );
        if (numwrote != numread) {
            return( RS_WRITE_ERROR );
        }

        numread = RcRead( inhandle, Pass2Info.IoBuffer, IO_BUFFER_SIZE );
        if (numread == -1) {
            return( RS_READ_ERROR );
        }
    }

    return( RS_OK );
} /* CopyExeDataTilEOF */

extern uint_32 AlignAmount( uint_32 offset, uint_16 shift_count )
/***************************************************************/
{
    uint_32     low_bits;       /* low shift_count bits of offset */

    low_bits = offset & (0xfffffffful >> (32 - shift_count));
    if (low_bits == 0) {
        return( 0 );
    } else {
        return( (0x1ul << shift_count) - low_bits );
    }
} /* AlignAmount */

static uint_32 FloorLog2( uint_32 value )
/***************************************/
/* This calculates the floor of the log base 2 of value. */
/* modified from binary_log function in wlink */
{
    uint_32 log;

    if( value == 0 ) {
        return( 0 );
    }
    log = 31;
    for(;;) {
        if( value & 0x80000000ul ) {  /* done if high bit on */
            break;
        }
        value <<= 1;            /* shift left and decrease possible log. */
        log--;
    }
    return( log );
} /* FloorLog2 */

extern uint_16 FindShiftCount( uint_32 filelen, uint_16 numobjs )
/***************************************************************/
/* filelen is the length of the file without any padding, numobjs is the */
/* number of objects that must appear on an alignment boundary */
{
    uint_16     shift_old;
    uint_16     shift;

    if (filelen < 0x10000L) {
        return( 0 );
    }

    shift_old = 16;
    shift = FloorLog2( filelen + numobjs * (1L << shift_old) ) - 15;
    /* It is possible for the algorithm to blow up so don't check for != use <*/
    while (shift < shift_old) {
        shift_old = shift;
        shift = FloorLog2( filelen + numobjs * (1L << shift_old) ) - 15;
    }

    /* In event of the rare case that the algorithm blew up take the min */
    return( min( shift, shift_old ) );
} /* FindShiftCount */

/*
 *PadExeData
 * NB When an error occurs the function MUST return without altering errno
 */
extern RcStatus PadExeData( int handle, uint_32 length )
/*************************************************/
{
    uint    numio;      /* number of bytes read or wrote */

    memset( Pass2Info.IoBuffer, 0, IO_BUFFER_SIZE );

    while (length > IO_BUFFER_SIZE) {
        length -= IO_BUFFER_SIZE;
        numio = RcWrite( handle, Pass2Info.IoBuffer, IO_BUFFER_SIZE );
        if (numio != IO_BUFFER_SIZE) {
            return( RS_WRITE_ERROR );
        }
    }

    if (length > 0) {
        numio = RcWrite( handle, Pass2Info.IoBuffer, length );
        if (numio != length) {
            return( RS_WRITE_ERROR );
        }
    }

    return( FALSE );
} /* PadExeData */

extern void CheckDebugOffset( ExeFileInfo * info )
/************************************************/
{
    uint_32     curroffset;

    curroffset = RcTell( info->Handle );
    if (curroffset > info->DebugOffset) {
        info->DebugOffset = curroffset;
    }
} /* CheckDebugOffset */

extern unsigned_32 OffsetFromRVA( ExeFileInfo *info, pe_va rva ) {
/*****************************************************************/

    pe_object           *objects;
    unsigned_16         obj_cnt;
    unsigned            i;

    obj_cnt = info->u.PEInfo.WinHead->num_objects;
    objects = info->u.PEInfo.Objects;
    for( i = 0; i < obj_cnt; i++ ) {
        if( objects[i].rva == rva ) break;
        if( objects[i].rva > rva ) {
            if( i != 0 ) i--;
            break;
        }
    }
    if( i == obj_cnt ) i--;
    if( objects[i].rva > rva ) return( 0xFFFFFFFF );
    return( objects[i].physical_offset + rva - objects[i].rva );
}

/*
 * SeekRead
 * NB When an error occurs the function MUST return without altering errno
 */
RcStatus SeekRead( int handle, unsigned_32 newpos, void *buff,
                   unsigned_16 size )
/**************************************************************************/
/* seek to a specified spot in the file, and read some data */
{
    long        rc;
    int         bytes_read;

    rc = RcSeek( handle, newpos, SEEK_SET );
    if( rc == -1 ) return( RS_READ_ERROR );
    bytes_read = RcRead( handle, buff, size );
    if( bytes_read != size ) {
        if( bytes_read == -1 ) {
            return( RS_READ_ERROR );
        } else {
            return( RS_READ_INCMPLT );
        }
    }
    return( RS_OK );

} /* SeekRead */

/* location within a windows executable of the offset of the os2_exe_header */
#define WIN_EXE_HEADER_OFFSET           0x3cL
#define DOS_RELOCATION_OFFSET           0x18L
#define DOS_EXE_SIGNATURE               0x5a4d

/* If the value at DOS_RELOCATION_ADDRESS_OFFSET < */
/* WIN_EXE_HEADER_OFFSET + sizeof(uint_32) then the DOS reloction */
/* information starts before the end of the address of the os2_exe_header */
/* so this is not a valid windows EXE file. */

ExeType FindNEPELXHeader( int handle, unsigned_32 *nh_offset )
/************************************************************/
/* Determine type of executable */
{
    os2_exe_header  ne_header;
    unsigned_16     data;
    RcStatus        rc;

    rc = SeekRead( handle, 0x00, &data, sizeof( data ) );
    if( rc != RS_OK ) return( FALSE );
    if( data != DOS_EXE_SIGNATURE ) return( EXE_TYPE_UNKNOWN );

    rc = SeekRead( handle, DOS_RELOCATION_OFFSET, &data, sizeof( data ) );
    if( rc != RS_OK ) return( EXE_TYPE_UNKNOWN );

    if( data < WIN_EXE_HEADER_OFFSET + sizeof(uint_32) ) {
        return( EXE_TYPE_UNKNOWN );
    }

    rc = SeekRead( handle, WIN_EXE_HEADER_OFFSET, nh_offset, sizeof(uint_32) );
    if( rc != RS_OK ) return( EXE_TYPE_UNKNOWN );

    rc = SeekRead( handle, *nh_offset, &data, sizeof( unsigned_16 ) );
    if( rc != RS_OK ) return( EXE_TYPE_UNKNOWN );

    switch( data ) {
    case OS2_SIGNATURE_WORD:
        rc = SeekRead( handle, *nh_offset, &ne_header, sizeof( ne_header ) );
        if( rc != RS_OK )
            return( EXE_TYPE_UNKNOWN );
        if( ne_header.target == TARGET_OS2 )
            return( EXE_TYPE_NE_OS2 );
        if( ne_header.target == TARGET_WINDOWS || ne_header.target == TARGET_WIN386 )
            return( EXE_TYPE_NE_WIN );
        return( EXE_TYPE_UNKNOWN );
        break;
    case PE_SIGNATURE:
        return( EXE_TYPE_PE );
        break;
    case OSF_FLAT_LX_SIGNATURE:
        return( EXE_TYPE_LX );
        break;
    default:
        return( EXE_TYPE_UNKNOWN );
        break;
    }
} /* FindNEPEHeader */
