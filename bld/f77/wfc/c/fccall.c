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
* Description:  F-Code processor for subroutine calls.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "wf77defs.h"
#include "wf77aux.h"
#include "wf77cg.h"
#include "tmpdefs.h"
#include "cpopt.h"
#include "fcgbls.h"
#include "iflookup.h"
#include "rtconst.h"
#include "prmcodes.h"
#include "fmemmgr.h"
#include "emitobj.h"
#include "fctypes.h"
#include "cgswitch.h"
#define  BY_CLI
#include "cgprotos.h"
#include "feprotos.h"


extern  void            GenLocalSyms(void);
extern  void            GenLocalDbgInfo(void);
extern  void            FreeLocalBacks(bool);
extern  void            FreeGlobalBacks(void);
extern  void            XPush(cg_name);
extern  cg_name         XPop(void);
extern  void            XPopCmplx(cg_cmplx *,cg_type);
extern  cg_name         GetTypedValue(void);
extern  label_handle    GetLabel(label_id);
extern  label_handle    GetStmtLabel(sym_id);
extern  void            MakeSCB(sym_id,cg_name);
extern  void            SubCodeSeg(void);
extern  void            FiniLabels(int);
extern  void            DefineEntryPoint(entry_pt *);
extern  bool            InArgList(entry_pt *,sym_id);
extern  void            SplitCmplx(cg_name,cg_type);
extern  tmp_handle      AllocTmp(cg_type);
extern  tmp_handle      MkTmp(cg_name,cg_type);
extern  cg_name         TmpPtr(tmp_handle,cg_type);
extern  void            RefStmtLabel(sym_id);
extern  void            DoneLabel(label_id);
extern  cg_name         CmplxAddr(cg_name,cg_name);
extern  aux_info        *AuxLookup(sym_id);
extern  cg_name         SCBPointer(cg_name);
extern  cg_name         SCBPtrAddr(cg_name);
extern  cg_name         SCBLength(cg_name);
extern  cg_name         SCBLenAddr(cg_name);
extern  cg_name         ArrayEltSize(sym_id);
extern  cg_name         FieldArrayEltSize(sym_id);
extern  call_handle     InitCall(RTCODE);
extern  bool            IntType(PTYPE);
extern  void            FiniTmps(void);
extern  void            CloneCGName(cg_name,cg_name *,cg_name *);
extern  sym_id          FindArgShadow(sym_id);
extern  bool            ForceStatic(unsigned_16);
extern  bool            SCBRequired(sym_id);

extern  aux_info        FortranInfo;
extern  back_handle     TraceEntry;
extern  segment_id      CurrCodeSegId;

/* Forward declarations */
static  void    GenTraceback( void );
static  void    GenReturnValue( sym_id sym );
static  void    GenCommonReturnValue( void );
static  void    DefineEntries( void );
static  void    PassCommonArgs( call_handle call, entry_pt *ep_called );



cg_type SPType( sym_id sym ) {
//============================

// Return subprogram cg type.

    if( (sym->ns.flags & SY_SUBPROG_TYPE) == SY_PROGRAM ) return( T_INTEGER );
    if( (sym->ns.flags & SY_SUBPROG_TYPE) == SY_SUBROUTINE ) return( T_INTEGER );
    // must be a function
    if( sym->ns.typ == TY_CHAR ) return( T_INTEGER );
    return( F772CGType( sym ) );
}


static  cg_type    CommonEntryType( void ) {
//====================================

// Generate return value for common entry point.

    return( T_INTEGER );
}


void    FCPrologue( void ) {
//====================

// Start a subprogram.

    sym_id              sym;
    entry_pt            *ep;
    int                 ep_count;
    sel_handle          sel;
    label_handle        main_entry_label;
    unsigned_16         sp_type;

    --NumSubProgs;
    sym = GetPtr();
    sp_type = sym->ns.flags & SY_SUBPROG_TYPE;
    SubCodeSeg();
    BESetSeg( WF77_LDATA );
    DGSeek( LDSegOffset );
    if( sp_type == SY_BLOCK_DATA ) {
        if( ( sym->ns.flags & SY_UNNAMED ) == 0 ) {
            DGLabel( FEBack( sym ) );
        }
    } else {
        if( CommonEntry == NULL ) {
            CGProcDecl( sym, SPType( sym ) );
        } else {
            CGProcDecl( CommonEntry, CommonEntryType() );
        }
        if( CGOpts & CGOPT_DB_LOCALS ) {
            DBModSym( sym, T_DEFAULT );
        }
    }
    GenLocalSyms();
    if( CGOpts & CGOPT_DB_LOCALS ) {
        if( sp_type != SY_BLOCK_DATA ) {
            GenLocalDbgInfo();
        }
    }
    if( sp_type != SY_BLOCK_DATA ) {
        GenTraceback();
    }
    if( CommonEntry != NULL ) {
        sel = CGSelInit();
        ep_count = 1;
        ep = Entries->link;
        while( ep != NULL ) {
            CGSelCase( sel, GetLabel( ep->id->ns.si.sp.entry ), ep_count );
            ep_count++;
            ep = ep->link;
        }
        main_entry_label = BENewLabel();
        CGSelOther( sel, main_entry_label );
        CGSelect( sel, CGUnary( O_POINTS, CGFEName( EPValue, T_INTEGER ),
                                T_INTEGER ) );
        CGControl( O_LABEL, NULL, main_entry_label );
        BEFiniLabel( main_entry_label );
    }
    BESetSeg( WF77_LDATA );
    LDSegOffset = DGTell();
}


static  void    GenTraceback( void ) {
//==============================

    call_handle         handle;

    if( Options & OPT_TRACE ) {
        handle = InitCall( RT_SET_MODULE );
        CGAddParm( handle, CGBackName( TraceEntry, T_POINTER ), T_POINTER );
        CGDone( CGCall( handle ) );
    }
}


void    FCEpilogue( void ) {
//====================

// End a subprogram.

    sym_id      sym;
    unsigned_16 sp_type;

    sym = GetPtr();
    sp_type = sym->ns.flags & SY_SUBPROG_TYPE;
    if( sp_type != SY_BLOCK_DATA ) {
        GenTraceback();
    }
    if( StNumbers.wild_goto ) {
        DoneLabel( StNumbers.branches );
    }
    FiniLabels( 0 );
    FiniTmps();
    if( ( sym->ns.flags & SY_SUBPROG_TYPE ) == SY_PROGRAM ) {
        CGReturn( NULL, T_INTEGER );
    } else if( sp_type != SY_BLOCK_DATA ) {
        if( CommonEntry == NULL ) {
            GenReturnValue( sym );
        } else {
            GenCommonReturnValue();
            DefineEntries();
        }
    }
    BEFlushSeg( CurrCodeSegId );
    FreeLocalBacks( TRUE );
    FreeGlobalBacks();
}


bool    ChkForAltRets( entry_pt *ep ) {
//=====================================

// Check to see if the specified entry point has an alternate return.

    parameter   *args;

    args = ep->parms;
    for(;;) {
        if( args == NULL ) break;
        if( args->flags & ARG_STMTNO ) return( TRUE );
        args = args->link;
    }
    return( FALSE );
}


bool    EntryWithAltRets( void ) {
//==========================

// Check to see if there's at least one entry with an alternate return.

    entry_pt    *ep;

    ep = Entries;
    while( ep != NULL ) {
        if( ChkForAltRets( ep ) ) return( TRUE );
        ep = ep->link;
    }
    return( FALSE );
}


static  void    GenReturnValue( sym_id sym ) {
//============================================

// Generate return value.

    cg_type     typ;

    typ = SPType( sym );
    if( ( sym->ns.flags & SY_SUBPROG_TYPE ) == SY_SUBROUTINE ) {
        if( !EntryWithAltRets() ) {
            CGReturn( NULL, T_INTEGER );
            return;
        }
    } else { // must be a function
        if( sym->ns.typ == TY_CHAR ) {
            CGReturn( NULL, T_INTEGER );
            return;
        }
    }
    CGReturn( CGUnary( O_POINTS, CGFEName( ReturnValue, typ ), typ ), typ );
}


static  void    GenCommonReturnValue( void ) {
//======================================

// Generate return value for common entry point.

    if( EntryWithAltRets() ) {
        CGReturn( CGUnary( O_POINTS, CGFEName( ReturnValue, T_INTEGER ),
                           T_INTEGER ), T_INTEGER );
    } else {
        CGReturn( NULL, T_INTEGER );
    }
}


static  void    DefineEntries( void ) {
//===============================

// Define entry points into subprogram.

    entry_pt    *ep;
    int         ep_count;
    call_handle call;
    cg_type     sp_type;
    cg_type     common_type;
    cg_name     val;

    common_type = CommonEntryType();
    ep_count = 0;
    ep = Entries;
    while( ep != NULL ) {
        FreeLocalBacks( FALSE );
        sp_type = SPType( ep->id );
        CGProcDecl( ep->id, sp_type );
        if( CGOpts & CGOPT_DB_LOCALS ) {
            DBModSym( ep->id, T_DEFAULT );
        }
        DefineEntryPoint( ep );
        call = CGInitCall( CGFEName( CommonEntry, common_type ), common_type,
                           &FortranInfo );
        PassCommonArgs( call, ep );
        if( ( ep->id->ns.flags & SY_SUBPROG_TYPE ) != SY_SUBROUTINE ) {
            val = CGFEName( ReturnValue, sp_type );
            if( ep->id->ns.typ == TY_CHAR ) {
                val = CGUnary( O_POINTS, val, T_POINTER );
                CGAddParm( call, val, T_POINTER );
                if( !(Options & OPT_DESCRIPTOR) ) {
                    val = CGFEName( FindArgShadow( ReturnValue ), T_INTEGER );
                    val = CGUnary( O_POINTS, val, T_INTEGER );
                    CGAddParm( call, val, T_INTEGER );
                }
            } else {
                CGAddParm( call, val, T_POINTER );
            }
        }
        CGAddParm( call, CGInteger( ep_count, T_INTEGER ), T_INTEGER );
        if( ( ep->id->ns.flags & SY_SUBPROG_TYPE ) == SY_SUBROUTINE ) {
            if( EntryWithAltRets() ) {
                val = CGUnary( O_POINTS, CGCall( call ), sp_type );
                CGReturn( val, sp_type );
            } else {
                CGDone( CGCall( call ) );
                CGReturn( NULL, sp_type );
            }
        } else {
            CGDone( CGCall( call ) );
            if( ep->id->ns.typ == TY_CHAR ) {
                CGReturn( NULL, T_INTEGER );
            } else {
                CGReturn( CGUnary( O_POINTS,
                                   CGFEName( ReturnValue, sp_type ),
                                   sp_type ), sp_type );
            }
        }
        ep_count++;
        ep = ep->link;
    }
}


static  bool    NeedShadowArg( pass_by *arg_aux ) {
//=================================================

    if( Options & OPT_DESCRIPTOR ) return( FALSE );
    if( arg_aux == NULL ) return( TRUE );
    if( arg_aux->info & (PASS_BY_VALUE | PASS_BY_DATA) ) return( FALSE );
    return( TRUE );
}


static  void    PassCommonArgs( call_handle call, entry_pt *ep_called ) {
//=======================================================================

// Pass arguments to common entry point.

    entry_pt    *ep;
    parameter   *arg;
    cg_name     cg;
    cg_type     arg_type;
    aux_info    *aux;
    pass_by     *arg_aux;

    ep = Entries;
    while( ep != NULL ) {
        aux = AuxLookup( ep->id );
        arg_aux = aux->arg_info;
        arg = ep->parms;
        while( arg != NULL ) {
            if( ( arg->flags & ( ARG_DUPLICATE | ARG_STMTNO ) ) == 0 ) {
                if( ( arg->id->ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) {
                    arg_type = T_CODE_PTR;
                } else if( arg->id->ns.flags & SY_SUBSCRIPTED ) {
                    arg_type = T_POINTER;
                } else if( arg->id->ns.typ == TY_CHAR ) {
                    arg_type = T_POINTER;
                } else if( ( arg_aux != NULL ) &&
                           ( arg_aux->info & PASS_BY_VALUE ) ) {
                    arg_type = F772CGType( arg->id );
                } else {
                    arg_type = T_POINTER;
                }
                if( InArgList( ep_called, arg->id ) ) {
                    cg = CGUnary( O_POINTS, CGFEName( arg->id, arg_type ),
                                  arg_type );
                    if( arg->id->ns.typ == TY_CHAR ) {
                        CGAddParm( call, cg, PromoteToBaseType( arg_type ) );
                        if( NeedShadowArg( arg_aux ) ) {
                            cg = CGFEName( FindArgShadow( arg->id ), T_INTEGER );
                            cg = CGUnary( O_POINTS, cg, T_INTEGER );
                            CGAddParm( call, cg, T_INTEGER );
                        }
                    } else {
                        CGAddParm( call, cg, PromoteToBaseType( arg_type ) );
                    }
                } else {
                    CGAddParm( call, CGInteger( 0, arg_type ),
                                                PromoteToBaseType( arg_type ) );
                    if( arg->id->ns.typ == TY_CHAR ) {
                        if( NeedShadowArg( arg_aux ) ) {
                            CGAddParm( call, CGInteger( 0, T_INTEGER ), T_INTEGER );
                        }
                    }
                }
            }
            if( ( arg_aux != NULL ) && ( arg_aux->link != NULL ) ) {
                arg_aux = arg_aux->link;
            }
            arg = arg->link;
        }
        ep = ep->link;
    }
}


static  cg_name     ArgAddr( PTYPE arg_type ) {
//===========================================

// Get address of actual argument.

    cg_type     cg_typ;
    cg_name     arg;

    arg = XPop();
    cg_typ = GetType( arg_type );
    if( !TypePointer( CGType( arg ) ) ) {
        if( ( arg_type == PT_CPLX_8 )
          || ( arg_type == PT_CPLX_16 )
          || ( arg_type == PT_CPLX_32 ) ) {
            arg = CmplxAddr( arg, XPop() );
        } else {
            arg = TmpPtr( MkTmp( arg, cg_typ ), cg_typ );
        }
    }
    return( arg );
}


static  cg_name     ArgValue( PTYPE arg_type, cg_type *new_typ ) {
//==============================================================

// Get value of actual argument.

    cg_name     arg;
    cg_type     cg_typ;

    arg = XPop();
    cg_typ = CGType( arg );
    if( TypePointer( cg_typ ) ) {
        if( DataPointer( cg_typ ) ) {
            cg_typ = GetType( arg_type );
            arg = CGUnary( O_POINTS, arg, cg_typ );
        }
    } else {
        cg_typ = GetType( arg_type );
    }
    *new_typ = cg_typ;
    return( arg );
}


static  cg_type ArgPtrType( cg_name arg ) {
//=========================================

// Get argument pointer type.

    cg_type     typ;

    typ = CGType( arg );
    if( typ >= T_USER_DEFINED ) return( T_POINTER );
    if( BETypeLength( typ ) < BETypeLength( T_POINTER ) ) return( T_POINTER );
    return( typ );
}


#if _CPU == 386

static  cg_name MkSCB16( cg_name src ) {
//======================================

    cg_name     src_2;
    cg_name     dst;
    cg_name     dst_2;

    dst = TmpPtr( AllocTmp( T_CHAR16 ), T_CHAR16 );
    CloneCGName( src, &src, &src_2 );
    CloneCGName( dst, &dst, &dst_2 );
    CGTrash( CGAssign( SCBLenAddr( dst_2 ), SCBLength( src_2 ), T_UINT_2 ) );
    // Assumption: first field in SCB is pointer to data
    return( CGLVAssign( SCBPtrAddr( dst ),
             CGUnary( O_PTR_TO_FOREIGN, SCBPointer( src ), T_GLOBAL_POINTER ),
             T_GLOBAL_POINTER ) );
}

#endif


void    FCCall( void ) {
//================

// Process CALL F-Code.

    sym_id      sp;
    sym_id      scb;
    uint        num_args;
    uint        idx;
    call_handle call;
    cg_type     sp_type;
    aux_info    *aux;
    cg_name     rtn;
    unsigned_16 arg_info;
    PTYPE       arg_type;
    PCODE       arg_code;
    pass_by     *arg_aux;
    cg_name     arg;
    cg_type     cg_typ;
    cg_type     new_typ;
    cg_cmplx    z;
    bool        chk_foreign;
    bool        pass_scb;
    bool        pass_len;
    cg_name     *arg_vec;

    sp = GetPtr();
    sp_type = SPType( sp );
    aux = AuxLookup( sp );
    arg_aux = aux->arg_info;
    if( sp->ns.flags & SY_SUB_PARM ) {
        rtn = CGUnary( O_POINTS, CGFEName( sp, T_CODE_PTR ), T_CODE_PTR );
    } else {
        rtn = CGFEName( sp, sp_type );
    }
    num_args = GetU16();
    if( num_args == 0 ) {
        arg_vec = NULL;
    } else {
        // We need num_args + 1 becausewe use senitel NULL at the end
        arg_vec = FMemAlloc( ( num_args + 1 ) * sizeof( cg_name ) );
    }
    call = CGInitCall( rtn, sp_type, aux );
    if( sp->ns.flags & SY_INTRINSIC ) {
        if( IFVarArgs( sp->ns.si.fi.index ) ) {
            CGAddParm( call, CGInteger( num_args, T_INTEGER ), T_INTEGER );
        }
    } else if( (sp->ns.flags & SY_SUBPROG_TYPE) == SY_FUNCTION ) {
        if( !(Options & OPT_DESCRIPTOR) && (sp->ns.typ == TY_CHAR) ) {
            scb = GetPtr();
            arg = SCBPointer( CGFEName( scb, T_CHAR ) );
#if _CPU == 386
            if( aux->cclass & FAR16_CALL ) {
                arg = CGUnary( O_PTR_TO_FOREIGN, arg, T_POINTER );
            }
#endif
            CGAddParm( call, arg, T_POINTER );
            CGAddParm( call, SCBLength( CGFEName( scb, T_CHAR ) ), T_INTEGER );
        }
    }
    idx = 0;
    while( num_args != 0 ) {
        arg_info = GetU16();
        arg_type = arg_info & 0x00ff;
        arg_code = (arg_info >> 8) & 0x7f;
        chk_foreign = TRUE;
        if( (arg_code == PC_PROCEDURE) || (arg_code == PC_FN_OR_SUB) ) {
            arg = XPop();
            cg_typ = T_CODE_PTR;
#if _CPU == 386 || _CPU == 8086
            if( (aux->cclass & FAR16_CALL) &&
                ((arg_info >> 8) & PC_PROC_FAR16) ) {
                chk_foreign = FALSE;
            } else if( arg_aux != NULL ) {
                if( arg_aux->info & ARG_FAR ) {
                    cg_typ = T_LONG_CODE_PTR;
                    arg = CGUnary( O_CONVERT, arg, T_LONG_CODE_PTR );
                    chk_foreign = FALSE;
                }
            }
#endif
        } else if( arg_type == PT_CHAR ) {
            // character data (including character arrays)
            if( Options & OPT_DESCRIPTOR ) {
                pass_scb = TRUE;
                pass_len = FALSE;
            } else {
                pass_scb = FALSE;
                pass_len = TRUE;
            }
            if( arg_aux != NULL ) {
                if( arg_aux->info & (PASS_BY_VALUE | PASS_BY_DATA) ) {
                    pass_scb = FALSE;
                    pass_len = FALSE;
                } else if( arg_aux->info & PASS_BY_DESCRIPTOR ) {
                    pass_scb = TRUE;
                    pass_len = FALSE;
                } else if( arg_aux->info & PASS_BY_NODESCRIPTOR ) {
                    pass_scb = FALSE;
                    pass_len = TRUE;
                }
            }
            arg = XPop();
            cg_typ = T_POINTER;
            if( pass_len ) {
                CloneCGName( arg, &arg, &arg_vec[idx] );
                ++idx;
            }
#if _CPU == 386
            if( pass_scb && (aux->cclass & FAR16_CALL) ) {
                arg = MkSCB16( arg );
            }
#endif
            if( !pass_scb ) {
                arg = SCBPointer( arg );
            }
            if( arg_aux != NULL ) {
                if( arg_aux->info & ARG_FAR ) {
                    cg_typ = T_LONG_POINTER;
                    arg = CGUnary( O_CONVERT, arg, T_LONG_POINTER );
                }
            }
        } else if( arg_code == PC_ARRAY_NAME ) {
            arg = XPop();
            cg_typ = ArgPtrType( arg );
            if( (arg_aux != NULL) &&
                (arg_aux->info & PASS_BY_REFERENCE) &&
                (arg_aux->info & ARG_FAR) ) {
                cg_typ = T_LONG_POINTER;
                arg = CGUnary( O_CONVERT, arg, T_LONG_POINTER );
                chk_foreign = FALSE;
            }
        } else if( (arg_aux != NULL) && (arg_aux->info & PASS_BY_VALUE) ) {
            chk_foreign = FALSE;
            if( arg_type == PT_CPLX_8 ) {
                XPopCmplx( &z, T_COMPLEX );
                CGAddParm( call, z.realpart, T_SINGLE );
                arg = z.imagpart;
                cg_typ = T_SINGLE;
            } else if( arg_type == PT_CPLX_16 ) {
                XPopCmplx( &z, T_DCOMPLEX );
                CGAddParm( call, z.realpart, T_DOUBLE );
                arg = z.imagpart;
                cg_typ = T_DOUBLE;
            } else if( arg_type == PT_CPLX_32 ) {
                XPopCmplx( &z, T_XCOMPLEX );
                CGAddParm( call, z.realpart, T_LONGDOUBLE );
                arg = z.imagpart;
                cg_typ = T_LONGDOUBLE;
            } else if( arg_type == PT_STRUCT ) {
                arg = ArgValue( arg_type, &cg_typ );
            } else {
                arg = ArgValue( arg_type, &cg_typ );
                new_typ = cg_typ;
                if( IntType( arg_type ) ) {
                    if( arg_aux->info & ARG_SIZE_1 ) {
                        new_typ = T_INT_1;
                    } else if( arg_aux->info & ARG_SIZE_2 ) {
                        new_typ = T_INT_2;
                    } else if( arg_aux->info & ARG_SIZE_4 ) {
                        new_typ = T_INT_4;
                    }
                } else if( ( arg_type == PT_LOG_1 ) ||
                           ( arg_type == PT_LOG_4 ) ) {
                    if( arg_aux->info & ARG_SIZE_1 ) {
                        new_typ = T_UINT_1;
                    } else if( arg_aux->info & ARG_SIZE_2 ) {
                        new_typ = T_UINT_2;
                    } else if( arg_aux->info & ARG_SIZE_4 ) {
                        new_typ = T_UINT_4;
                    }
                } else { // must be PT_REAL_4 or PT_REAL_8 or PT_REAL_16
                    if( arg_aux->info & ARG_SIZE_4 ) {
                        new_typ = T_SINGLE;
                    } else if( arg_aux->info & ARG_SIZE_8 ) {
                        new_typ = T_DOUBLE;
                    } else if( arg_aux->info & ARG_SIZE_16 ) {
                        new_typ = T_LONGDOUBLE;
                    }
                }
                if( cg_typ != new_typ ) {
                    arg = CGUnary( O_CONVERT, arg, new_typ );
                    cg_typ = new_typ;
                } else if( ( sp->ns.flags & SY_INTRINSIC ) &&
                           ( IFArgType( sp->ns.si.fi.index ) == TY_INTEGER ) ) {
                    // The following code is only useful when the intrinsic
                    // promotion switch is activated.  We don't need to check
                    // for the switch because if the switch was not set then
                    // an error would be issued before we ever reach this code.
                    if( ( cg_typ == T_INT_1 ) || ( cg_typ == T_INT_2 ) ) {
                        cg_typ = T_INT_4;
                        arg = CGUnary( O_CONVERT, arg, T_INT_4 );
                    }
                }
                cg_typ = PromoteToBaseType( cg_typ );
            }
        } else {
            arg = ArgAddr( arg_type );
            cg_typ = ArgPtrType( arg );
            if( (arg_aux != NULL) &&
                (arg_aux->info & PASS_BY_REFERENCE) &&
                (arg_aux->info & ARG_FAR) ) {
                cg_typ = T_LONG_POINTER;
                arg = CGUnary( O_CONVERT, arg, T_LONG_POINTER );
                chk_foreign = FALSE;
            }
        }
#if _CPU == 386 || _CPU == 8086
        if( (aux->cclass & FAR16_CALL) && chk_foreign ) {
            arg = CGUnary( O_PTR_TO_FOREIGN, arg, cg_typ );
        }
#endif
        CGAddParm( call, arg, cg_typ );
        if( (arg_aux != NULL) && (arg_aux->link != NULL) ) {
            arg_aux = arg_aux->link;
        }
        --num_args;
    }
    if( arg_vec != NULL ) {
        arg_vec[idx] = NULL;
        for( idx = 0; arg_vec[idx] != NULL; ++idx ) {
            CGAddParm( call, SCBLength( arg_vec[idx] ), T_INTEGER );
        }
        FMemFree( arg_vec );
    }
    if( (sp->ns.flags & SY_SUBPROG_TYPE) == SY_SUBROUTINE ) {
        XPush( CGUnary( O_POINTS, CGCall( call ), sp_type ) );
    } else {
        if( sp->ns.typ == TY_CHAR ) {
            if( (Options & OPT_DESCRIPTOR) || ( sp->ns.flags & SY_INTRINSIC) ) {
                scb = GetPtr();
                arg = CGFEName( scb, T_CHAR );
#if _CPU == 386
                if( aux->cclass & FAR16_CALL ) {
                    arg = MkSCB16( arg );
                    arg = CGUnary( O_PTR_TO_FOREIGN, arg, T_GLOBAL_POINTER );
                }
#endif
                CGAddParm( call, arg, T_POINTER );
            }
            XPush( CGBinary( O_COMMA, CGCall( call ), CGFEName( scb, T_CHAR ),
                             T_LOCAL_POINTER ) );
        } else {
            if( (sp_type == T_COMPLEX) || (sp_type == T_DCOMPLEX) ||
                        (sp_type == T_XCOMPLEX) ) {
                SplitCmplx( CGCall( call ), sp_type );
            } else if( sp_type >= T_USER_DEFINED ) { // structure
                XPush( CGCall( call ) );
            } else {
                XPush( CGUnary( O_POINTS, CGCall( call ), sp_type ) );
            }
        }
    }
}


void        FCEvalArg( void ) {
//=======================

// Force evaluation of an argument.

    XPush( CGEval( XPop() ) );
}


cg_name VarAltSCB( sym_id sym ) {
//===============================

// This function is called for character variables that are dummy arguments
// and SCBRequired() is TRUE.

    /*
    for dummy arguments, the SCB is always allocated on the stack
    so we have no problems with recursion
    if( Options & OPT_AUTOMATIC ) {
    */
        return( CGTempName( sym->ns.si.va.vi.alt_scb, T_CHAR ) );
    /*
    } else {
        return( CGBackName( sym->ns.si.va.vi.alt_scb, T_CHAR ) );
    }
    */
}


cg_name SubAltSCB( sym_id sym ) {
//===============================

// This function is called for function return values when
// the /nodescriptor option is specified

    /*
    for function return values, the SCB is always allocated on the stack
    so we have no problems with recursion
    if( Options & OPT_AUTOMATIC ) {
    */
        return( CGTempName( sym->ns.si.sp.alt_scb, T_CHAR ) );
    /*
    } else {
        return( CGBackName( sym->ns.si.sp.alt_scb, T_CHAR ) );
    }
    */
}


void    FCDArgInit( void ) {
//====================

// Initialize dummy arguments.

    sym_id      sym;
    cg_name     arg;
    cg_name     dst;

    sym = GetPtr();
    if( !(Options & OPT_DESCRIPTOR) ) {
        if( ( sym->ns.flags & SY_SUBPROG_TYPE ) == SY_FUNCTION ) {
            if( sym->ns.typ == TY_CHAR ) {
                arg = CGUnary( O_POINTS,
                               CGFEName( ReturnValue, T_POINTER ), T_POINTER );
                if( Options & OPT_DESCRIPTOR ) {
                    arg = SCBLength( arg );
                } else {
                    if( CommonEntry != NULL ) {
                        sym = CommonEntry;
                    }
                    dst = SCBPtrAddr( SubAltSCB( sym ) );
                    CGDone( CGAssign( dst, arg, T_POINTER ) );
                    arg = CGUnary( O_POINTS,
                             CGFEName( FindArgShadow( ReturnValue ), T_INTEGER ),
                             T_INTEGER );
                }
                dst = SCBLenAddr( SubAltSCB( sym ) );
                CGDone( CGAssign( dst, arg, T_INTEGER ) );
            }
        }
    }
    for(;;) {
        sym = GetPtr();
        if( sym == NULL ) break;
        if( (sym->ns.flags & SY_CLASS) != SY_VARIABLE ) continue;
        if( sym->ns.typ != TY_CHAR ) continue;
        if( sym->ns.flags & SY_SUBSCRIPTED ) continue;
        if( !SCBRequired( sym ) ) continue;
        // character variable
        arg = CGUnary( O_POINTS, CGFEName( sym, T_POINTER ), T_POINTER );
        dst = SCBPtrAddr( VarAltSCB( sym ) );
        if( !(sym->ns.flags & SY_VALUE_PARM) && (Options & OPT_DESCRIPTOR) ) {
            arg = SCBPointer( arg );
        }
        CGDone( CGAssign( dst, arg, T_POINTER ) );
        if( sym->ns.xt.size == 0 ) {
            dst = SCBLenAddr( VarAltSCB( sym ) );
            if( sym->ns.flags & SY_VALUE_PARM ) {
                arg = CGInteger( 0, T_INTEGER );
            } else if( Options & OPT_DESCRIPTOR ) {
                arg = CGFEName( sym, T_POINTER );
                arg = CGUnary( O_POINTS, arg, T_POINTER );
                arg = SCBLength( arg );
            } else {
                arg = CGFEName( FindArgShadow( sym ), T_INTEGER );
                arg = CGUnary( O_POINTS, arg, T_INTEGER );
            }
            CGDone( CGAssign( dst, arg, T_INTEGER ) );
        }
    }
}


void    FCPassFieldCharArray( void ) {
//==============================

// Pass character array, array element, or substrung array element to
// subprogram.  Character array is a field within a structure.

    sym_id      fd;
    sym_id      scb;

    fd = GetPtr();
    scb = GetPtr();
    MakeSCB( scb, FieldArrayEltSize( fd ) );
}


void    FCPassCharArray( void ) {
//=========================

// Pass character array, array element, or substrung array element to
// subprogram.

    sym_id      arr;
    sym_id      scb;

    arr = GetPtr();
    scb = GetPtr();
    MakeSCB( scb, ArrayEltSize( arr ) );
}


void    FCAltReturn( void ) {
//=====================

// Process alternate return.

    sel_handle          sel;
    int                 num_alts;
    int                 alt_ret;
    sym_id              sn;
    obj_ptr             curr_obj;
    label_handle        other;

    curr_obj = FCodeTell( 0 );
    num_alts = GetU16();
    sel = CGSelInit();
    alt_ret = 1;
    while( alt_ret <= num_alts ) {
        sn = GetPtr();
        CGSelCase( sel, GetStmtLabel( sn ), alt_ret );
        alt_ret++;
    }
    other = BENewLabel();
    CGSelOther( sel, other );
    CGSelect( sel, XPop() );
    CGControl( O_LABEL, NULL, other );
    BEFiniLabel( other );
    // mark all referenced statements
    FCodeSeek( curr_obj );
    num_alts = GetU16();
    while( num_alts != 0 ) {
        RefStmtLabel( GetPtr() );
        num_alts--;
    }
}


void    FCAssignAltRet( void ) {
//========================

// Assign alternate return value to result of subroutine.

    cg_name     alt_ret;

    alt_ret = GetTypedValue();
    if( EntryWithAltRets() ) {
        CGDone( CGAssign( CGFEName( ReturnValue, T_INTEGER ), alt_ret,
                          T_INTEGER ) );
    } else {
        CGDone( alt_ret );
    }
}
