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


//
// FCTYPES :    Get typing information
//

#include "ftnstd.h"
#include "symtypes.h"
#include "types.h"
#include "wf77defs.h"
#include "symbol.h"
#include "fctypes.h"

#define NUM_TYPES       12
#define L1              T_UINT_1
#define L4              T_UINT_4
#define I1              T_INT_1
#define I2              T_INT_2
#define I4              T_INT_4
#define R4              T_SINGLE
#define R8              T_DOUBLE
#define R10             T_LONGDOUBLE
#define C8              T_COMPLEX
#define C16             T_DCOMPLEX
#define C20             T_XCOMPLEX
#define CH              T_CHAR

static  byte            MapCGTypes[] = {

// L1   L4  I1   I2   I4   R4   R8   R10  C8   C16  C20  CH

   L1,  L4, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // L1
   L4,  L4, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // L4
   0,   0,  I1,  I2,  I4,  R4,  R8,  R10, C8,  C16, C20, 0,   // I1
   0,   0,  I2,  I2,  I4,  R4,  R8,  R10, C8,  C16, C20, 0,   // I2
   0,   0,  I4,  I4,  I4,  R4,  R8,  R10, C8,  C16, C20, 0,   // I4
   0,   0,  R4,  R4,  R4,  R4,  R8,  R10, C8,  C16, C20, 0,   // R4
   0,   0,  R8,  R8,  R8,  R8,  R8,  R10, C16, C16, C20, 0,   // R8
   0,   0,  R10, R10, R10, R10, R10, R10, C20, C20, C20, 0,   // R10
   0,   0,  C8,  C8,  C8,  C8,  C16, C20, C8,  C16, C20, 0,   // C8
   0,   0,  C16, C16, C16, C16, C16, C20, C16, C16, C20, 0,   // C16
   0,   0,  C20, C20, C20, C20, C20, C20, C20, C20, C20, 0,   // C20
   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   CH   // CH
};

#ifdef pick
#undef pick
#endif
#define pick(id,type,dbgtype,cgtype,typnam) cgtype,

static  cg_type         CGTypesMap[] = {
#include "ptypdefn.h"
};

cg_type MkCGType( PTYPE typ ) {
//===========================

// Map FORTRAN parameter type to CG-type.

    return( CGTypesMap[ typ] );
}


cg_type         GetType( unsigned_16 typ_info ) {
//===============================================

    return( MkCGType( typ_info ) );
}


cg_type         GetType1( unsigned_16 typ_info ) {
//================================================

    return( MkCGType( ( typ_info & 0xff00 ) >> 8 ) );
}


cg_type         GetType2( unsigned_16 typ_info ) {
//================================================

    return( MkCGType( typ_info & 0x00ff ) );
}


cg_type         F772CGType( sym_id sym ) {
//========================================

// Map a WATFOR-77 type to a CG type.

    if( sym->ns.typ == TY_STRUCTURE ) return( sym->ns.xt.record->cg_typ );
    return( MkCGType( ParmType( sym->ns.typ, sym->ns.xt.size ) ) );
}


static  int     CGIndex( cg_type typ ) {
//======================================

// Return index for a CG-type.

    if( typ == T_UINT_1 ) return( 0 );
    if( typ == T_UINT_2 ) return( 1 );
    if( typ == T_INT_1 ) return( 2 );
    if( typ == T_INT_2 ) return( 3 );
    if( typ == T_INT_4 ) return( 4 );
    if( typ == T_SINGLE ) return( 5 );
    if( typ == T_DOUBLE ) return( 6 );
    if( typ == T_LONGDOUBLE ) return( 7 );
    if( typ == T_COMPLEX ) return( 8 );
    if( typ == T_DCOMPLEX ) return( 9 );
    if( typ == T_XCOMPLEX ) return( 10 );
    return( 11 ); // typ == T_CHAR
}


cg_type         ResCGType( cg_type typ1, cg_type typ2 ) {
//=======================================================

// For a binary operation of two operands of the given CG-types, return
// the result CG-type.

    return( MapCGTypes[ CGIndex( typ1 ) * NUM_TYPES + CGIndex( typ2 ) ] );
}


bool                DataPointer( cg_type typ ) {
//==============================================

// Is CG-type a pointer?

    return( ( typ == T_NEAR_POINTER )  || ( typ == T_LONG_POINTER ) ||
            ( typ == T_HUGE_POINTER )  || ( typ == T_LOCAL_POINTER ) ||
            ( typ == T_COMPLEX )       || ( typ == T_DCOMPLEX ) ||
            ( typ == T_XCOMPLEX )      ||
            ( typ == T_CHAR )          || ( typ >= T_USER_DEFINED ) );
}


bool                TypeCGInteger( cg_type typ ) {
//================================================

// Is CG-type an integer?

    return( ( typ == T_UINT_1 ) || ( typ == T_INT_1 ) ||
            ( typ == T_UINT_2 ) || ( typ == T_INT_2 ) ||
            ( typ == T_UINT_4 ) || ( typ == T_INT_4 ) ||
            ( typ == T_UINT_8 ) || ( typ == T_INT_8 ) ||
            ( typ == T_INTEGER ) );
}


bool                TypePointer( cg_type typ ) {
//==============================================

// Is CG-type a pointer?

    return( DataPointer( typ ) || ( typ == T_CODE_PTR ) ||
            ( typ == T_LONG_CODE_PTR ) || ( typ == T_NEAR_CODE_PTR ) );
}


cg_type             PromoteToBaseType( cg_type typ ) {
//====================================================

// if type is integer T_INT_1, T_INT_2 under the _AXP or _PPC, we must promote
// it in order to make a call

#if _CPU == _AXP || _CPU == _PPC
    if( ( typ == T_INT_1 ) || ( typ == T_INT_2 )  ) {
        typ = T_INT_4;
    }
#endif
    return( typ );
}
