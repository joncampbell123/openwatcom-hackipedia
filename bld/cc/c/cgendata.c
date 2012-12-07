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
* Description:  Interface to Optimizing code generator for DATA
*
****************************************************************************/


#include "cvars.h"
#include "cg.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "standard.h"
#define BY_CLI
#include "cgprotos.h"
#include "feprotos.h"

static void EmitDQuad( DATA_QUAD *dq );

void EmitZeros( unsigned long amount )
{
    while( amount > 8*1024 ) {
        DGIBytes( 8*1024, 0 );
        amount -= 8*1024;
    }
    DGIBytes( amount, 0 );
}


void AlignIt( TYPEPTR typ )
{
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    if( OptSize == 0 ) {        /* optimize for time */
        DGAlign( GetTypeAlignment( typ ) );
    }
#else
    DGAlign( max( 4, GetTypeAlignment( typ ) ) );
#endif
}


void EmitDataQuads( void )
{
    DATA_QUAD   *dq;

    if( StartDataQuadAccess() != NULL ) {
        for(;;) {
            dq = NextDataQuad();
            if( dq == NULL ) break;
            for(;;) {
                EmitDQuad( dq );
                if( ! (dq->flags & Q_REPEATED_DATA) ) break; /* 06-apr-92 */
                dq->u.long_values[1]--;
                if( dq->u.long_values[1] == 0 ) break;
            }
        }
    }
}

static void EmitDQuad( DATA_QUAD *dq )
{
    cg_type             data_type;
    int                 size_of_item;
    unsigned long       amount;
    auto SYM_ENTRY      sym;

    static int          segment;
    static unsigned long size = 0;

    if( dq->flags & Q_NEAR_POINTER ) {
        data_type = T_NEAR_POINTER;
        size_of_item = TARGET_NEAR_POINTER;
    } else if( dq->flags & Q_FAR_POINTER ) {
        data_type = T_LONG_POINTER;
        size_of_item = TARGET_FAR_POINTER;
    } else if( dq->flags & Q_CODE_POINTER ) {
        data_type = T_CODE_PTR;
        size_of_item = TARGET_POINTER;
#if _CPU == 8086
        if( TargetSwitches & BIG_CODE ) {
            size_of_item = TARGET_FAR_POINTER;
        }
#endif
    } else {
        data_type = T_POINTER;
        size_of_item = TARGET_POINTER;
#if _CPU == 8086
        if( TargetSwitches & BIG_DATA ) {
            size_of_item = TARGET_FAR_POINTER;
        }
#endif
    }
#if _CPU == 8086
    if( size >= 0x10000 ) {
        if( segment != SEG_CONST  &&  segment != SEG_DATA ) {
            ++segment;
            BESetSeg( segment );
            size -= 0x10000;
        }
    }
#endif
    switch( dq->type ) {
    case QDT_STATIC:
        SymGet( &sym, dq->u.var.sym_handle );
        segment = sym.u.var.segment;
        BESetSeg( segment );
        AlignIt( sym.sym_type );
        DGLabel( FEBack( dq->u.var.sym_handle ) );
        size = 0;
        break;
    case QDT_CHAR:
    case QDT_UCHAR:
    case QDT_BOOL:
        DGInteger( dq->u.long_values[0], T_UINT_1 );
        size += sizeof( char );
        if( dq->flags & Q_2_INTS_IN_ONE ) {
            DGInteger( dq->u.long_values[1], T_UINT_1 );
            size += sizeof( char );
        }
        break;
    case QDT_SHORT:
    case QDT_USHORT:
        DGInteger( dq->u.long_values[0], T_UINT_2 );
        size += sizeof( target_short );
        if( dq->flags & Q_2_INTS_IN_ONE ) {
            DGInteger( dq->u.long_values[1], T_UINT_2 );
            size += sizeof( target_short );
        }
        break;
    case QDT_INT:
    case QDT_UINT:
        DGInteger( dq->u.long_values[0], T_INTEGER );
        size += sizeof( target_int );
        if( dq->flags & Q_2_INTS_IN_ONE ) {
            DGInteger( dq->u.long_values[1], T_INTEGER );
            size += sizeof( target_int );
        }
        break;
    case QDT_LONG:
    case QDT_ULONG:
        DGInteger( dq->u.long_values[0], T_UINT_4 );
        size += sizeof( target_long );
        if( dq->flags & Q_2_INTS_IN_ONE ) {
            DGInteger( dq->u.long_values[1], T_UINT_4 );
            size += sizeof( target_long );
        }
        break;
    case QDT_LONG64:
    case QDT_ULONG64:
        DGInteger64( dq->u.long64, T_UINT_8 );
        size += sizeof( int64 );
        break;

    case QDT_FLOAT:
    case QDT_FIMAGINARY:
//      ftoa( dq->u.double_value, Buffer );
//      DGFloat( Buffer, T_SINGLE );
        {
            DATA_QUAD   local_dq;
            float       float_value;

            // dq->u.double_value may not have proper alignment on Alpha
            // so copy pieces to local copy on stack which will have
            // proper alignment
            local_dq.u.long_values[0] = dq->u.long_values[0];
            local_dq.u.long_values[1] = dq->u.long_values[1];
            float_value = (float)local_dq.u.double_value;
            DGBytes( sizeof(float), (char *)&float_value );
        }
        size += sizeof( float );
        break;
    case QDT_DOUBLE:
    case QDT_DIMAGINARY:
//      ftoa( dq->u.double_value, Buffer );
//      DGFloat( Buffer, TY_DOUBLE );
        DGBytes( sizeof(double), (char *)&dq->u.double_value );
        size += sizeof( double );
        break;
    case QDT_LONG_DOUBLE:
    case QDT_LDIMAGINARY:
        DGBytes( sizeof(long_double), (char *)&dq->u.long_double_value );
        size += sizeof( long_double );
        break;
    case QDT_STRING:
        EmitStrPtr( dq->u.string_leaf, data_type );
        size += size_of_item;
        break;
    case QDT_POINTER:
    case QDT_ID:
        if( dq->u.var.sym_handle == 0 ) {
            DGInteger( dq->u.var.offset, data_type );
        } else {
            DGFEPtr( dq->u.var.sym_handle, data_type, dq->u.var.offset );
        }
        size += size_of_item;
        break;
    case QDT_CONST:                       /* array of characters */
        size += EmitBytes( dq->u.string_leaf );
        break;
    case QDT_CONSTANT:
#if _CPU == 8086
        for( amount = dq->u.long_values[0]; amount != 0; ) {
            if( amount + size >= 0x00010000 ) {
                EmitZeros( 0x10000 - size );
                amount -= ( 0x10000 - size );
                size = 0;
                if( segment != SEG_CONST  &&  segment != SEG_DATA ) {
                    ++segment;
                    BESetSeg( segment );
                }
            } else {
                EmitZeros( amount );
                size += amount;
                amount = 0;
            }
        }
#else
        amount = dq->u.long_values[0];
        EmitZeros( amount );
        size += amount;
#endif
        break;
    }
}
