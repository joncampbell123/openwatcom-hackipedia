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
// FCSTRING  : string processing F-Codes
//

#include "ftnstd.h"
#include "global.h"
#include "rtconst.h"
#include "wf77defs.h"
#include "cg.h"
#include "cpopt.h"
#include "inline.h"
#include "emitobj.h"
#include "fctypes.h"
#include "cgswitch.h"
#define  BY_CLI
#include "cgprotos.h"


extern  cg_name         XPop(void);
extern  cg_name         XPopValue(cg_type);
extern  void            XPush(cg_name);
extern  cg_name         SymAddr(sym_id);
extern  call_handle     InitCall(RTCODE);
extern  call_handle     InitInlineCall(int);
extern  cg_name         StructRef(cg_name,int);
extern  cg_name         StkElement(int);
extern  void            CloneCGName(cg_name,cg_name *,cg_name *);
extern  void            PopStkElements(int);
extern  void            MakeSCB(sym_id,cg_name);
extern  cg_name         GetChOp(cg_type);
extern  sym_id          FindArgShadow(sym_id);
extern  cg_name         GetTypedValue(void);

#define CAT_TEMP        0x8000


cg_name SCBPtrAddr( cg_name scb ) {
//=================================

// Get pointer to pointer in SCB.

    return( scb );
}


cg_name SCBPointer( cg_name scb ) {
//=================================

// Get pointer from SCB.

    return( CGUnary( O_POINTS, SCBPtrAddr( scb ), T_GLOBAL_POINTER ) );
}


cg_name SCBLenAddr( cg_name scb ) {
//=================================

// Get pointer to length in SCB.

    return( StructRef( scb, BETypeLength( T_GLOBAL_POINTER ) ) );
}


cg_name SCBFlagsAddr( cg_name scb ) {
//===================================

// Get pointer to flags in SCB.

    return( StructRef( scb, BETypeLength( T_CHAR ) ) );
}


cg_name SCBLength( cg_name scb ) {
//================================

// Get length from SCB.

    return( CGUnary( O_POINTS, SCBLenAddr( scb ), T_UNSIGNED ) );
}


cg_name Concat( uint num_args, cg_name dest ) {
//=============================================

// Do concatenation operation.

    int         count;
    call_handle call;
    cg_name     dest_1;
    cg_name     dest_2;

    if( num_args & CAT_TEMP ) {
        call = InitCall( RT_TCAT );
        num_args &= ~CAT_TEMP;
    } else if( num_args == 1 ) {
        call = InitCall( RT_MOVE );
    } else {
        call = InitCall( RT_CAT );
    }
    count = num_args;
    while( count > 0 ) {
        CGAddParm( call, StkElement( count ), T_LOCAL_POINTER );
        --count;
    }
    PopStkElements( num_args );
    CloneCGName( dest, &dest_1, &dest_2 );
    CGAddParm( call, dest_1, T_LOCAL_POINTER );
    if( num_args != 1 ) {
        CGAddParm( call, CGInteger( num_args, T_UNSIGNED ), T_UNSIGNED );
    }
    return( CGBinary( O_COMMA, CGCall( call ), dest_2, T_LOCAL_POINTER ) );
}


void    FCCat( void ) {
//===============

// Do concatenation operation.

    XPush( Concat( GetU16(), XPop() ) );
}


void    FCChar1Move( void ) {
//=====================

// Perform single character assignment.

    cg_type     typ;
    cg_name     dest;

    typ = GetType( GetU16() );
    dest = XPop();
    XPush( CGLVAssign( SCBPointer( dest ), GetChOp( typ ), typ ) );
}

#if _CPU == 8086
 #define TAIL_MASK      1
 #define TAIL_SHIFT     1
#else
 #define TAIL_MASK      3
 #define TAIL_SHIFT     2
#endif

void    FCCharNMove( void ) {
//=====================

// Perform N character assignment of non optimal lengths.

    int         src_len;
    int         dst_len;
    cg_name     dst;
    cg_name     dst2;
    call_handle call;
    bool        equal = FALSE;

    src_len = GetInt();
    dst_len = GetInt();
    if( src_len < dst_len ) {
        call = InitInlineCall( INLINE_STRBLAST_NE );
    } else {
        src_len = dst_len;
        equal = TRUE;
        call = InitInlineCall( INLINE_STRBLAST_EQ );
    }
    dst = XPop();
    CloneCGName( dst, &dst, &dst2 );

    if( OZOpts & OZOPT_O_SPACE || !equal ) {
        CGAddParm( call, CGInteger( src_len, T_INTEGER ), T_INTEGER );
    } else {
        // Special but common case, so we optimize it.
        CGAddParm( call, CGInteger( src_len & TAIL_MASK, T_INTEGER ),
                        T_INTEGER );
        CGAddParm( call, CGInteger( src_len >> TAIL_SHIFT, T_INTEGER ),
                        T_INTEGER );
    }

    CGAddParm( call, SCBPointer( XPop() ), T_LOCAL_POINTER );
    if( !equal ) {
        CGAddParm( call, CGInteger( dst_len - src_len, T_INTEGER ), T_INTEGER );
    }
    CGAddParm( call, SCBPointer( dst ), T_LOCAL_POINTER );
    XPush( CGBinary( O_COMMA, CGCall( call ), dst2, T_LOCAL_POINTER ) );
}


static cg_name CharArrLength( sym_id sym ) {
//==========================================

// Get element size for character*(*) arrays.

    if( sym->ns.flags & SY_VALUE_PARM ) {
        return( CGInteger( 0, T_INTEGER ) );
    } else if( Options & OPT_DESCRIPTOR ) {
        return( SCBLength( CGUnary( O_POINTS, CGFEName( sym, T_POINTER ),
                                    T_POINTER ) ) );
    } else {
        return( CGUnary( O_POINTS, CGFEName( FindArgShadow( sym ), T_INTEGER ),
                T_INTEGER ) );
    }
}


cg_name CharItemLen( sym_id sym ) {
//=================================

// Get element size for character*(*) variables, functions and arrays.

    if( sym->ns.flags & SY_SUBSCRIPTED ) {
        return( CharArrLength( sym ) );
    } else {
        return( SCBLength( SymAddr( sym ) ) );
    }
}


void    FCSubString( void ) {
//=====================

// Do substring operation.

    sym_id      char_var;
    sym_id      dest;
    cg_name     src;
    cg_name     first_1;
    cg_name     first_2;
    cg_name     last;
    unsigned_16 typ_info;
    cg_name     len;
    cg_name     ptr;
    call_handle call;

    char_var = GetPtr();
    typ_info = GetU16();
    src = XPop();
    first_1 = XPopValue( GetType1( typ_info ) );
    if( char_var == NULL ) { // i.e. chr(i:i)
        len = CGInteger( GetInt(), T_INTEGER );
        if( Options & OPT_BOUNDS ) {
            CloneCGName( first_1, &first_1, &last );
            last = CGBinary( O_PLUS, last, len, T_INTEGER );
            last = CGBinary( O_MINUS, last, CGInteger( 1, T_INTEGER ),
                             T_INTEGER );
        }
    } else {
        last = XPop();
        if( last == NULL ) {
            if( char_var->ns.xt.size == 0 ) {
                last = CharItemLen( char_var );
            } else {
                last = CGInteger( char_var->ns.xt.size, T_INTEGER );
            }
        } else {
            XPush( last );
            last = XPopValue( GetType2( typ_info ) );
        }
        if( !( Options & OPT_BOUNDS ) ) {
            CloneCGName( first_1, &first_1, &first_2 );
            len = CGBinary( O_MINUS, last, first_2, T_INTEGER );
            len = CGBinary( O_PLUS, len, CGInteger( 1, T_INTEGER ), T_INTEGER );
        }
    }
    dest = GetPtr();
    if( Options & OPT_BOUNDS ) {
        call = InitCall( RT_SUBSTRING );
        CGAddParm( call, CGFEName( dest, T_CHAR ), T_LOCAL_POINTER );
        CGAddParm( call, last, T_INT_4 );
        CGAddParm( call, first_1, T_INT_4 );
        CGAddParm( call, src, T_LOCAL_POINTER );
        XPush( CGBinary( O_COMMA, CGCall( call ), CGFEName( dest, T_CHAR ),
                         T_LOCAL_POINTER ) );
    } else {
        ptr = CGBinary( O_PLUS, SCBPointer( src ),
                        CGBinary( O_MINUS, first_1, CGInteger( 1, T_INTEGER ),
                                  T_INTEGER ),
                        T_GLOBAL_POINTER );
        CGTrash( CGAssign( SCBLenAddr( CGFEName( dest, T_CHAR ) ),
                           len, T_INTEGER ) );
        // Assumption is that the pointer in the SCB is the first field in
        // the SCB so that when we push the cg_name returned by CGAssign()
        // it is a pointer to the SCB.  We must leave the assignment of the
        // pointer into the SCB in the tree so that the aliasing information
        // is not lost.
        XPush( CGLVAssign( SCBPtrAddr( CGFEName( dest, T_CHAR ) ),
                           ptr, T_GLOBAL_POINTER ) );
// Don't do it the following way:
//        CGTrash( CGAssign( SCBPtrAddr( CGFEName( dest, T_CHAR ) ),
//                           ptr, T_GLOBAL_POINTER ) );
//        XPush( CGFEName( dest, T_CHAR ) );
    }
}


void    FCPushSCBLen( void ) {
//======================

// NULL "last" means we need the length from the SCB in the character*(*) case.
// See FCSubString().

    XPush( NULL );
}


void    FCMakeSCB( void ) {
//===================

    cg_name     len;
    cg_name     ptr;

    ptr = XPop();
    len = XPop();
    XPush( ptr );
    MakeSCB( GetPtr(), len );
}


void    FCSetSCBLen( void ) {
//=====================

// Fill scb length

    sym_id              scb;
    cg_name             len;

    // Get general information
    scb = GetPtr();
    len = GetTypedValue();
    CGTrash( CGAssign( SCBLenAddr( CGFEName( scb, T_CHAR ) ), len,
                       T_INTEGER ) );
}
