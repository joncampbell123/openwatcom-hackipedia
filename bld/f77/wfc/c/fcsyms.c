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
* Description:  Symbol table processing.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "wf77defs.h"
#include "wf77aux.h"
#include "cg.h"
#include "ecflags.h"
#include "cpopt.h"
#include "segsw.h"
#include "fcgbls.h"
#include "wf77labe.h"
#include "falloc.h"
#include "nmlinfo.h"
#include "fmemmgr.h"
#include "types.h"
#include "fctypes.h"
#include "cgswitch.h"
#define  BY_CLI
#include "cgprotos.h"
#include "feprotos.h"

#include <string.h>


extern  pointer         ConstBack( sym_id );
extern  void            FCMessage( fc_msg_class, void * );
extern  segment_id      GetComSeg( sym_id,unsigned_32 );
extern  seg_offset      GetComOffset( unsigned_32 );
extern  segment_id      GetGlobalSeg( unsigned_32 );
extern  seg_offset      GetGlobalOffset( unsigned_32 );
extern  label_handle    GetLabel( label_id );
extern  label_handle    GetStmtLabel( sym_id );
extern  void            FiniLabels( uint );
extern  void            DumpFormats( void );
extern  void            FreeSFHeader( sym_id );
extern  void            MakeName( char *, char *, char * );
extern  char            *SDFName( char * );
extern  cg_type         F772CGType( sym_id );
extern  bool            EntryWithAltRets( void );
extern  bool            ChkForAltRets( entry_pt * );
extern  aux_info        *AuxLookup( sym_id );
extern  cg_type         ArrayPtrType( sym_id );
extern  void            ReverseList( void * );
extern  cg_name         SCBPtrAddr( cg_name );
extern  bool            ForceStatic( unsigned_16 );
extern  segment_id      AllocImpSegId( void );
extern  sym_id          STArgShadow( sym_id );
extern  sym_id          STAdvShadow( sym_id );
extern  cg_name         SCBLenAddr( cg_name );
extern  cg_name         SCBFlagsAddr( cg_name );
extern  cg_name         SubAltSCB( sym_id );
extern  sym_id          FindEqSetShadow( sym_id );
extern  sym_id          FindAdvShadow( sym_id );
extern  cg_type         CmplxBaseType( cg_type );
extern  bool            TypeCmplx( TYPE );
extern  cg_name         StructRef( cg_name,int );

extern  segment_id      CurrCodeSegId;

/* Forward declarations */
static  void    AssignAdv( sym_id sym );
static  void    AssignName2Adv( sym_id sym );
static  void    AssignStaticAdv( sym_id sym );
static  void    CreateAllocatableADV( sym_id sym );
static  void    DefineCommonEntry( void );
static  void    MergeCommonInfo( void );
static  void    DumpBrTable( void );
static  void    DumpLitSCBs( void );
static  void    DumpNameLists( void );
static  void    DumpSymName( sym_id sym );
static  void    DumpStaticAdv( sym_id sym, bool dmp_nam_ptr );
static  void    DbgVarInfo( sym_id sym );
static  void    DbgSubProgInfo( sym_id sym );
static  void    AssignAutoAdv( sym_id sym );
static  void    DumpAutoAdv( sym_id sym, sym_id shadow );
static  void    PostponeFreeBackHandle( back_handle data );
static  void    FreeBackHandle( void *_back );
static  void    DefineArgs( entry_pt *ep );
static  void    DeclareArg( parameter *arg, pass_by *arg_aux );

void    DefineEntryPoint( entry_pt *ep );
void    FreeUsedBacks( bool nuke );

static  back_handle     ModuleName = { NULL };

back_handle             TraceEntry;

typedef struct old_back_handle {
                struct old_back_handle  *next;
                back_handle             old;
} old_back_handle;

static old_back_handle *OldBackHandles = NULL;

/*
static  back_handle     MakeStaticSCB( int len ) {
//================================================

    back_handle scb;

    scb = BENewBack( NULL );
    DGAlign( ALIGN_DWORD );
    DGLabel( scb );
    DGIBytes( BETypeLength( T_POINTER ), 0 );
    DGInteger( len, T_INTEGER );
    return( scb );
}
*/


static  void            CheckAutoSize( sym_id sym, cg_type typ ) {
//================================================================

#if _CPU == 8086

    if( BETypeLength( typ ) <= 0x7fff ) return;

    if( sym->ns.flags & SY_IN_EQUIV ) {
        FCMessage( FCMSG_EQUIV_TOO_LARGE, sym );
    } else if( ( sym->ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) {
        FCMessage( FCMSG_RET_VAL_TOO_LARGE, sym );
    } else {
        FCMessage( FCMSG_VARIABLE_TOO_LARGE, sym );
    }
#else
    sym = sym;
    typ = typ;
#endif

}


static  temp_handle     MakeTempSCB( int len ) {
//==============================================

    temp_handle scb;

    scb = CGTemp( T_CHAR );
    // if it's character*(*), the SCB will get filled in by FCDArgInit()
    if( len != 0 ) {
        CGDone( CGAssign( SCBLenAddr( CGTempName( scb, T_CHAR ) ),
                          CGInteger( len, T_INTEGER ), T_INTEGER ) );
    }
    return( scb );
}


uint    SymAlign( sym_id sym ) {
//==============================

    switch( sym->ns.typ ) {
    case TY_LOGICAL_1 :
    case TY_LOGICAL :
    case TY_INTEGER_1 :
    case TY_INTEGER_2 :
    case TY_INTEGER :
        return( sym->ns.xt.size );
    case TY_REAL :
    case TY_COMPLEX :
        return( ALIGN_DWORD );
    case TY_DOUBLE :
    case TY_DCOMPLEX :
        return( ALIGN_QWORD );
    case TY_TRUE_EXTENDED :
    case TY_TRUE_XCOMPLEX :
        return( ALIGN_SEGMENT );
    case TY_CHAR :
        return( ALIGN_DWORD );
    default :
        return( 1 );
    }
}


static  segment_id      LocalData( sym_id sym, unsigned_32 size ) {
//=================================================================

    segment_id  seg;
    segment_id  old_seg;

    if( sym->ns.flags & SY_DATA_INIT ) {
        seg = WF77_LDATA;
    } else {
        seg = WF77_UDATA;
    }
    old_seg = BESetSeg( seg );
    DGAlign( SymAlign( sym ) );
    DGLabel( FEBack( sym ) );
    DGUBytes( size );
    BESetSeg( old_seg );
    return( seg );
}


static  unsigned_32     CheckThreshold( sym_id sym, unsigned_32 g_offset ) {
//==========================================================================

    unsigned_32 item_size;
    segment_id  old_seg;

    item_size = _SymSize( sym );
    if( sym->ns.flags & SY_SUBSCRIPTED ) {
        item_size *= sym->ns.si.va.dim_ext->num_elts;
    }
    if( item_size > DataThreshold ) {
        sym->ns.si.va.vi.seg_id = GetGlobalSeg( g_offset );
        old_seg = BESetSeg( sym->ns.si.va.vi.seg_id );
        DGSeek( GetGlobalOffset( g_offset ) );
        DGLabel( FEBack( sym ) );
        BESetSeg( old_seg );
        return( item_size );
    } else {
        sym->ns.si.va.vi.seg_id = LocalData( sym, item_size );
        return( 0 );
    }
}


static  void    DumpSCB( back_handle scb, back_handle data, int len,
                         bool allocatable, signed_32 offset ) {
//=============================================================

// Dump an SCB.

    segment_id  old_seg;

    old_seg = BESetSeg( WF77_LDATA );
    DGAlign( ALIGN_DWORD );
    DGLabel( scb );
    if( data == NULL ) {
        DGIBytes( BETypeLength( T_POINTER ), 0 );
    } else {
        DGBackPtr( data, old_seg, offset, T_POINTER );
    }
    DGInteger( len, T_INTEGER );
    if( allocatable ) {
        DGInteger( ALLOC_STRING, T_UINT_2 );
    }
    BESetSeg( old_seg );
}


static  back_handle     DumpCharVar( sym_id sym ) {
//=================================================

// Dump a character variable.

    back_handle data;

    data = BENewBack( NULL );
    DGLabel( data );
    if( (SubProgId->ns.flags & SY_SUBPROG_TYPE) != SY_BLOCK_DATA ) {
        DumpSCB( FEBack( sym ), data, sym->ns.xt.size, _Allocatable( sym ), 0 );
    }
    return( data );
}


static  void     DumpCharVarInCommon( sym_id sym, com_eq *ce_ext,
                                      segment_id seg, signed_32 offset ) {
//========================================================================

// Dump a character variable into the common block.

    if( (SubProgId->ns.flags & SY_SUBPROG_TYPE) != SY_BLOCK_DATA ) {
        seg = BESetSeg( seg );
        DumpSCB( FEBack( sym ), FEBack( ce_ext->com_blk ), sym->ns.xt.size,
                _Allocatable( sym ), GetComOffset( ce_ext->offset + offset ) );
        BESetSeg( seg );
    }
}


static  void    InitSCB( sym_id sym, cg_name data ) {
//===================================================

    CGDone( CGAssign( SCBPtrAddr( CGFEName( sym, T_CHAR ) ),
            data, T_POINTER ) );
    CGDone( CGAssign( SCBLenAddr( CGFEName( sym, T_CHAR ) ),
            CGInteger( sym->ns.xt.size, T_INTEGER ), T_INTEGER ) );
}


static  void    DumpAutoSCB( sym_id sym, cg_type typ ) {
//======================================================

    if( _Allocatable( sym ) ) {
        CGAutoDecl( sym, T_CHAR_ALLOCATABLE );
    } else {
        CGAutoDecl( sym, T_CHAR );
    }
    if( _Allocatable( sym ) ) {
        InitSCB( sym, CGInteger( 0, T_POINTER ) );
        CGDone( CGAssign( SCBFlagsAddr( CGFEName( sym, T_CHAR ) ),
                CGInteger( ALLOC_STRING, T_UINT_2 ), T_UINT_2 ) );
    } else {
        InitSCB( sym, CGTempName( CGTemp( typ ), typ ) );
    }
    sym->ns.si.va.dim_ext = NULL; // indicate NULL back handle
}


static  void    DumpGlobalSCB( sym_id sym, unsigned_32 g_offset ) {
//=================================================================

    segment_id  old_seg;

    if( _Allocatable( sym ) ) {
        DumpSCB( FEBack( sym ), NULL, 0, TRUE, 0 );
    } else {
        sym->ns.si.va.vi.seg_id = GetGlobalSeg( g_offset );
        old_seg = BESetSeg( sym->ns.si.va.vi.seg_id );
        DGSeek( GetGlobalOffset( g_offset ) );
        sym->ns.si.va.bck_hdl = DumpCharVar( sym );
        if( !(sym->ns.flags & SY_DATA_INIT) ) {
            BEFreeBack( sym->ns.si.va.bck_hdl );
        }
        BESetSeg( old_seg );
    }
}


bool    SCBRequired( sym_id sym ) {
//=================================

    if( !(Options & OPT_DESCRIPTOR) ) return( TRUE );
    if( sym->ns.flags & SY_VALUE_PARM ) return( TRUE );
    if( sym->ns.xt.size != 0 ) return( TRUE );
    return( FALSE );
}


static  unsigned_32     DumpVariable( sym_id sym, unsigned_32 g_offset ) {
//========================================================================

// Allocate space for the given variable.

    unsigned_16 flags;
    uint        size;
    segment_id  old_seg;
    TYPE        typ;
    sym_id      leader;
    signed_32   offset;
    com_eq      *ce_ext;
    cg_type     cgtyp;

    flags = sym->ns.flags;
    typ = sym->ns.typ;
    size = _SymSize( sym );
    if( flags & SY_IN_EQUIV ) {
        leader = sym;
        offset = 0;
        for(;;) {
            ce_ext = leader->ns.si.va.vi.ec_ext;
            if( ce_ext->ec_flags & LEADER ) break;
            offset += ce_ext->offset;
            leader = ce_ext->link_eqv;
        }
        if( ce_ext->ec_flags & MEMBER_IN_COMMON ) {
            if( (typ == TY_CHAR) && !(flags & SY_SUBSCRIPTED) ) {
                DumpCharVarInCommon( sym, ce_ext, GetComSeg( leader, offset ),
                                     offset );
            }
        } else if( (SubProgId->ns.flags & SY_SUBPROG_TYPE) != SY_BLOCK_DATA ) {
            if( !ForceStatic( leader->ns.flags ) && (Options & OPT_AUTOMATIC) &&
                !(ce_ext->ec_flags & MEMBER_INITIALIZED) ) {
                sym_id  eqv_set;
                eqv_set = FindEqSetShadow( leader );
                if( !(ce_ext->ec_flags & EQUIV_SET_LABELED) ) {
                    ce_ext->ec_flags |= EQUIV_SET_LABELED;
                    CheckAutoSize( leader, eqv_set->ns.si.ms.cg_typ );
                    CGAutoDecl( eqv_set, eqv_set->ns.si.ms.cg_typ );
                }
                if( (typ == TY_CHAR) && !(flags & SY_SUBSCRIPTED) ) {
                    CGAutoDecl( sym, T_CHAR );
                    InitSCB( sym, CGBinary( O_PLUS,
                                  CGFEName( eqv_set, eqv_set->ns.si.ms.cg_typ ),
                                  CGInteger( offset, T_INT_4 ), T_POINTER ) );
                }
            } else {
                if( !(ce_ext->ec_flags & EQUIV_SET_LABELED) ) {
                    ce_ext->ec_flags |= EQUIV_SET_LABELED;
                    ce_ext->offset = g_offset - ce_ext->low;
                    g_offset += ce_ext->high - ce_ext->low;
                }
                offset += ce_ext->offset;
                old_seg = BESetSeg( GetGlobalSeg( offset ) );
                DGSeek( GetGlobalOffset( offset ) );
                if( (typ == TY_CHAR) && !(flags & SY_SUBSCRIPTED) ) {
                    sym->ns.si.va.bck_hdl = DumpCharVar( sym );
                    if( sym != leader ) {
                        BEFreeBack( sym->ns.si.va.bck_hdl );
                    }
                } else if( (sym == leader) || (CGOpts & CGOPT_DB_LOCALS) ) {
                    DGLabel( FEBack( sym ) );
                }
                BESetSeg( old_seg );
            }
        }
        if( (SubProgId->ns.flags & SY_SUBPROG_TYPE) != SY_BLOCK_DATA ) {
            if( (flags & SY_SUBSCRIPTED) && (Options & OPT_BOUNDS) ) {
                AssignAdv( sym );
                AssignName2Adv( sym );
            }
        }
    } else if( flags & SY_IN_COMMON ) {
        if( (typ == TY_CHAR) && !(flags & SY_SUBSCRIPTED) ) {
            ce_ext = sym->ns.si.va.vi.ec_ext;
            DumpCharVarInCommon( sym, ce_ext, GetComSeg( sym, 0 ), 0 );
        }
        if( (SubProgId->ns.flags & SY_SUBPROG_TYPE) != SY_BLOCK_DATA ) {
            if( (flags & SY_SUBSCRIPTED) && (Options & OPT_BOUNDS) ) {
                AssignStaticAdv( sym );  // COMMON blocks are always static
                AssignName2Adv( sym );
            }
        }
    } else if( (SubProgId->ns.flags & SY_SUBPROG_TYPE) != SY_BLOCK_DATA ) {
        if( flags & SY_SUB_PARM ) {
            if( (flags & SY_CLASS) == SY_VARIABLE ) {
                if( flags & SY_SUBSCRIPTED ) {
                    if( _AdvRequired( sym->ns.si.va.dim_ext ) ||
                        // character*(*) argument with no string descriptor
                        ((size == 0) && (flags & SY_VALUE_PARM)) ||
                        (Options & OPT_BOUNDS) ) {
                        AssignAdv( sym );
                        if( Options & OPT_BOUNDS ) {
                            AssignName2Adv( sym );
                        }
                    }
                } else if( sym->ns.typ == TY_CHAR ) {
                    if( SCBRequired( sym ) ) {
                        // for dummy arguments, always allocate the SCB on
                        // the stack so we don't have any problems with
                        // recursion
                        /*
                        if( Options & OPT_AUTOMATIC ) {
                        */
                            sym->ns.si.va.vi.alt_scb = MakeTempSCB( size );
                        /*
                        } else {
                            sym->ns.si.va.vi.alt_scb = MakeStaticSCB( size );
                        }
                        */
                    }
                }
            }
        } else {
            if( flags & SY_SUBSCRIPTED ) {
                if( _Allocatable( sym ) ) {
                    CreateAllocatableADV( sym );
                } else {
                    // Consider:        DIMENSION A(1)
                    // It's considered psuedo-dimensioned until we are
                    // done compiling the subprogram at which time we
                    // know it's not a dummy argument (no more ENTRYs)
                    sym->ns.si.va.dim_ext->dim_flags &= ~DIM_PVD;
                    if( !ForceStatic( flags ) && (Options & OPT_AUTOMATIC) ) {
                        CheckAutoSize( sym, sym->ns.si.va.dim_ext->l.cg_typ );
                        CGAutoDecl( sym, sym->ns.si.va.dim_ext->l.cg_typ );
                    } else {
                        g_offset += CheckThreshold( sym, g_offset );
                    }
                    if( Options & OPT_BOUNDS ) {
                        AssignAdv( sym );
                    }
                }
                if( Options & OPT_BOUNDS ) {
                    AssignName2Adv( sym );
                }
            } else if( typ == TY_STRUCTURE ) {
                if( !ForceStatic( flags ) && (Options & OPT_AUTOMATIC) ) {
                    cgtyp = F772CGType( sym );
                    CheckAutoSize( sym, cgtyp );
                    CGAutoDecl( sym, cgtyp );
                } else {
                    g_offset += CheckThreshold( sym, g_offset );
                }
            } else if( typ == TY_CHAR ) {
                if( !ForceStatic( flags ) && (Options & OPT_AUTOMATIC) ) {
                    DumpAutoSCB( sym, sym->ns.si.va.vi.cg_typ );
                } else {
                    DumpGlobalSCB( sym, g_offset );
                    g_offset += size;
                }
            } else if( ForceStatic( flags ) || (Options & OPT_SAVE) ) {
                LocalData( sym, size );
            } else {
                cgtyp = F772CGType( sym );
                CheckAutoSize( sym, cgtyp );
                CGAutoDecl( sym, cgtyp );
            }
        }
    }
    return( g_offset );
}


static  void    SetConstDataSeg( void ) {
//=================================
#if _CPU == 8086 || _CPU == 386
    if( ( CGOpts & CGOPT_CONST_CODE ) && ( _BigDataModel( CGOpts ) ) ) {
        BESetSeg( CurrCodeSegId );
    } else {
        BESetSeg( WF77_CDATA );
    }
#else
    BESetSeg( WF77_CDATA );
#endif
}


void    GenLocalSyms( void ) {
//======================

// Generate local symbols for a subprogram.

    sym_id      sym;
    unsigned_16 flags;
    unsigned_16 sp_class;
    unsigned_16 sp_type;
    cg_type     cgtyp;

    SFSymId = NULL; // for building statement function chain
    BESetSeg( WF77_LDATA );
    sp_class = SubProgId->ns.flags & SY_SUBPROG_TYPE;
    if( sp_class != SY_BLOCK_DATA ) {
        if( CommonEntry != NULL ) {
            DefineCommonEntry();
        } else if( Entries != NULL ) {
            DefineEntryPoint( Entries );
        }
    }
    MergeCommonInfo();
    sym = NList;
    while( sym != NULL ) {
        flags = sym->ns.flags;
        if( ( flags & SY_CLASS ) == SY_VARIABLE ) {
            if( ( sym != EPValue ) && ( sym != ReturnValue ) ) {
                GSegOffset = DumpVariable( sym, GSegOffset );
            }
        } else if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
            sp_type = flags & SY_SUBPROG_TYPE;
            if( sp_type == SY_STMT_FUNC ) {
                sym->ns.si.sf.header->link = SFSymId;
                SFSymId = sym;
                if( sym->ns.typ == TY_CHAR ) {
                    CGAutoDecl( sym, T_CHAR );
                } else {
                    cgtyp = F772CGType( sym );
                    CheckAutoSize( sym, cgtyp );
                    CGAutoDecl( sym, cgtyp );
                }
            } else if( sp_type == SY_FUNCTION ) {
                if( (flags & SY_PENTRY) && (sym->ns.typ == TY_CHAR) &&
                    !(Options & OPT_DESCRIPTOR) ) {
                    if( CommonEntry != NULL ) {
                        // for function return values, always allocate the
                        // SCB on the stack so we don't have any problems
                        // with recursion
                        /*
                        if( Options & OPT_AUTOMATIC ) {
                        */
                            CommonEntry->ns.si.sp.alt_scb = MakeTempSCB( sym->ns.xt.size );
                        /*
                        } else {
                            CommonEntry->ns.si.sp.alt_scb = MakeStaticSCB( sym->ns.xt.size );
                        }
                        */
                    } else {
                        // for function return values, always allocate the
                        // SCB on the stack so we don't have any problems
                        // with recursion
                        /*
                        if( Options & OPT_AUTOMATIC ) {
                        */
                            sym->ns.si.sp.alt_scb = MakeTempSCB( sym->ns.xt.size );
                        /*
                        } else {
                            sym->ns.si.sp.alt_scb = MakeStaticSCB( sym->ns.xt.size );
                        }
                        */
                    }
                }
            }
            if( !(flags & SY_PS_ENTRY) ) {
                if( (sp_type == SY_FUNCTION) ||
                    (sp_type == SY_SUBROUTINE) ||
                    (sp_type == SY_FN_OR_SUB) ) {
                    if( flags & SY_INTRINSIC ) {
                        sym->ns.si.fi.imp_segid = AllocImpSegId();
                    } else {
                        sym->ns.si.sp.imp_segid = AllocImpSegId();
                    }
                }
            }
        }
        sym = sym->ns.link;
    }
    if( sp_class != SY_BLOCK_DATA ) {
        for( sym = MList; sym != NULL; sym = sym->ns.link ) {
            // check for shadow symbol of leader of equivalence set
            if( sym->ns.flags & SY_IN_EQUIV ) continue;
            // check for shadow symbol of ADV
            if( sym->ns.flags & SY_SUBSCRIPTED ) continue;
            // check for shadow symbol for function return value
            if( sym->ns.flags & SY_PS_ENTRY ) {
                // shadow symbol for function return value
                if( CGOpts & CGOPT_DB_LOCALS ) {
                    CGAutoDecl( sym, T_POINTER );
                }
            } else if( sym->ns.typ == TY_CHAR ) {
                if( sym->ns.xt.size == 0 ) {
                    CGAutoDecl( sym, T_CHAR );
                } else if( Options & OPT_AUTOMATIC ) {
                    DumpAutoSCB( sym, sym->ns.si.ms.cg_typ );
                } else {
                    DumpGlobalSCB( sym, GSegOffset );
                    GSegOffset += sym->ns.xt.size;
                }
            } else {
                // check for shadow argument for character argument length
                if( !(sym->ns.flags & SY_VALUE_PARM) ) {
                    cgtyp = F772CGType( sym );
                    CheckAutoSize( sym, cgtyp );
                    CGAutoDecl( sym, cgtyp );
                }
            }
        }
    }
    if( StNumbers.wild_goto ) {
        DumpBrTable();
    }
    SetConstDataSeg();
    DumpLitSCBs();
    DumpNameLists();
    DumpFormats();
    TraceEntry = NULL;
    if( Options & OPT_TRACE ) {
        if( sp_class != SY_BLOCK_DATA ) {
            if( ModuleName == NULL ) {
                // can't go in code segment since
                // there may be multiple code segments
                BESetSeg( WF77_CDATA );
                ModuleName = BENewBack( NULL );
                MakeName( SDFName( SrcName ), SrcExtn, TokenBuff );
                DGLabel( ModuleName );
                DGString( TokenBuff, strlen( TokenBuff ) );
                DGIBytes( 1, NULLCHAR );
            }
            BESetSeg( WF77_LDATA );
            TraceEntry = BENewBack( NULL );
            DGLabel( TraceEntry );
            DGIBytes( BETypeLength( T_POINTER ), 0 );
            DGInteger( 0, T_INTEGER );
            DGBackPtr( ModuleName, WF77_CDATA, 0, T_POINTER );
        }
    }
}


static  void    DumpNameLists( void ) {
//===============================

// Dump NAMELIST information.

    sym_id      nl;
    grp_entry   *ge;
    sym_id      sym;
    byte        nl_info;

    nl = NmList;
    while( nl != NULL ) {
        nl->nl.address = BENewBack( NULL );
        DGLabel( nl->nl.address );
        DGInteger( nl->nl.name_len, T_UINT_1 );
        DGBytes( nl->nl.name_len, nl->nl.name );
        ge = nl->nl.group_list;
        while( ge != NULL ) {
            sym = ge->sym;
            DumpSymName( sym );
            nl_info = 0;
            _SetNMLType( nl_info, ParmType( sym->ns.typ, sym->ns.xt.size ) );
            if( sym->ns.flags & SY_SUBSCRIPTED ) {
                _SetNMLSubScrs( nl_info,
                                _DimCount( sym->ns.si.va.dim_ext->dim_flags ) );
                DGInteger( nl_info, T_UINT_1 );
                DGInteger( sym->ns.si.va.dim_ext->num_elts, T_UINT_4 );
                DumpStaticAdv( sym, FALSE ); // we do not want a name ptr dumped
                if( sym->ns.typ == TY_CHAR ) {
                    DGInteger( sym->ns.xt.size, T_INTEGER );
                }
            } else {
                DGInteger( nl_info, T_UINT_1 );
            }
            DGIBytes( BETypeLength( T_POINTER ), 0 );
            ge = ge->link;
        }
        DGInteger( 0, T_UINT_1 );
        nl = nl->nl.link;
    }
}


void    GenLocalDbgInfo( void ) {
//=========================

// Generate local symbols for a subprogram.

    sym_id      sym;
    cg_name     loc;

    for( sym = NList; sym != NULL; sym = sym->ns.link ) {
        if( ( sym->ns.flags & SY_CLASS ) == SY_VARIABLE ) {
            if( ( sym != EPValue ) && ( sym != ReturnValue ) ) {
                DbgVarInfo( sym );
            }
        } else if( ( sym->ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) {
            if( sym->ns.flags & SY_SUB_PARM ) {
                DbgSubProgInfo( sym );
            }
        }
    }
    for( sym = BList; sym != NULL; sym = sym->ns.link ) {
        DBModSym( sym, T_DEFAULT );
    }
    for( sym = MList; sym != NULL; sym = sym->ns.link ) {
        // map all entry points to the return value so that when
        // the subprogram is entered through one entry point and
        // the symbol of a different entry point is referenced, we
        // will get the return value for the entered entry point
        if( ( sym->ns.flags & SY_PS_ENTRY ) &&
            ( ( sym->ns.flags & SY_SUBPROG_TYPE ) == SY_FUNCTION ) ) {
            // shadow symbol for function return value
            if( sym->ns.typ == TY_CHAR ) {
                // ReturnValue is an argument to character functions
                if( Options & OPT_DESCRIPTOR ) {
                    // use SCB passed as argument
                    loc = CGFEName( ReturnValue, T_POINTER );
                    loc = CGUnary( O_POINTS, loc, T_POINTER );
                } else {
                    // use allocated SCB
                    if( CommonEntry != NULL ) {
                        loc = SubAltSCB( CommonEntry );
                    } else {
                        loc = SubAltSCB( sym->ns.si.ms.sym );
                    }
                }
            } else if( CommonEntry != NULL ) {
                // ReturnValue is an argument to common entry point
                loc = CGFEName( ReturnValue, T_POINTER );
                loc = CGUnary( O_POINTS, loc, T_POINTER );
            } else {
                loc = CGFEName( ReturnValue, F772CGType( sym ) );
            }
            CGDone( CGAssign( CGFEName( sym, T_POINTER ), loc, T_POINTER ) );
            DbgVarInfo( sym );
        }
    }
}


static  void    MergeCommonInfo( void ) {
//=================================

// Merge information from global common block to local common block.

    sym_id      sym;
    sym_id      g_sym;
    int         len;

    for( sym = BList; sym != NULL; sym = sym->ns.link ) {
        for( g_sym = GList; ; g_sym = g_sym->ns.link ) {
            if( ( g_sym->ns.flags & SY_CLASS ) != SY_COMMON ) continue;
            len = sym->ns.name_len;
            if( len == g_sym->ns.name_len ) {
                if( memcmp( sym->ns.name, g_sym->ns.name, len ) == 0 ) break;
            }
        }
        sym->ns.si.cb.seg_id = g_sym->ns.si.cb.seg_id;
        sym->ns.address = g_sym->ns.address;
        if( g_sym->ns.flags & SY_EQUIVED_NAME ) {
            sym->ns.flags |= SY_EQUIVED_NAME;
        }
    }
}


static  void    DumpSymName( sym_id sym ) {
//=========================================

// Dump symbol name for run-time error messages.

    DGInteger( sym->ns.name_len, T_UINT_1 );
    DGBytes( sym->ns.name_len, sym->ns.name );
}


static  void    DbgVarInfo( sym_id sym ) {
//========================================

// Generate debugging information for for a variable.

    dbg_loc     loc;
    com_eq      *ce_ext;

    if( sym->ns.flags & SY_SUB_PARM ) {
        DBLocalSym( sym, T_POINTER );
    } else if( sym->ns.flags & SY_IN_EQUIV ) {
        sym_id  leader;
        intstar4        offset;

        offset = 0;
        leader = sym;
        for(;;) {
            ce_ext = leader->ns.si.va.vi.ec_ext;
            if( ce_ext->ec_flags & LEADER ) break;
            offset += ce_ext->offset;
            leader = ce_ext->link_eqv;
        }
        leader = FindEqSetShadow( leader );
        if( (leader == NULL) || (sym->ns.typ == TY_CHAR) ) {
            if( ( sym->ns.typ != TY_CHAR ) &&
                ( ce_ext->ec_flags & MEMBER_IN_COMMON ) ) {
                loc = DBLocInit();
                loc = DBLocSym( loc, ce_ext->com_blk );
                loc = DBLocConst( loc, ce_ext->offset + offset );
                loc = DBLocOp( loc, DB_OP_ADD, 0 );
                DBGenSym( sym, loc, TRUE );
                DBLocFini( loc );
            } else {
                DBLocalSym( sym, T_DEFAULT );
            }
        } else {
            loc = DBLocInit();
            loc = DBLocSym( loc, leader );
            loc = DBLocConst( loc, offset - ce_ext->low );
            loc = DBLocOp( loc, DB_OP_ADD, 0 );
            DBGenSym( sym, loc, TRUE );
            DBLocFini( loc );
        }
    } else {
        if( ( sym->ns.typ != TY_CHAR ) && ( sym->ns.flags & SY_IN_COMMON ) ) {
            ce_ext = sym->ns.si.va.vi.ec_ext;
            loc = DBLocInit();
            loc = DBLocSym( loc, ce_ext->com_blk );
            loc = DBLocConst( loc, ce_ext->offset );
            loc = DBLocOp( loc, DB_OP_ADD, 0 );
            DBGenSym( sym, loc, TRUE );
            DBLocFini( loc );
        } else {
            DBLocalSym( sym, T_DEFAULT );
        }
    }
}


static  void    DbgSubProgInfo( sym_id sym ) {
//============================================

// Generate debugging information for for a subprogram.

    DBLocalSym( sym, T_DEFAULT );
}


static  void    AssignAdv( sym_id sym ) {
//=======================================

// Assign an ADV to an array.

    if( !ForceStatic( sym->ns.flags ) && (Options & OPT_AUTOMATIC) ) {
        AssignAutoAdv( sym );
    } else {
        AssignStaticAdv( sym );
    }
}


static  void    CreateAllocatableADV( sym_id sym ) {
//==================================================

// Creates a allocatable ADV:
//      1. pointer to array storage
//      2. flags
//      3. actual ADV

    cg_name     adv;
    cg_name     temp;
    cg_type     typ;

    if( !ForceStatic( sym->ns.flags ) && (Options & OPT_AUTOMATIC) ) {
        if( sym->ns.si.va.dim_ext->dim_flags & DIM_EXTENDED ) {
            CGAutoDecl( sym, T_ARR_ALLOCATABLE_EXTENDED );
            adv = CGFEName( sym, T_ARR_ALLOCATABLE_EXTENDED );
            temp = CGInteger( ALLOC_EXTENDED, T_UINT_2 );
        } else {
            CGAutoDecl( sym, T_ARR_ALLOCATABLE );
            adv = CGFEName( sym, T_ARR_ALLOCATABLE );
            temp = CGInteger( 0, T_UINT_2 );
        }
        adv = StructRef( adv, BETypeLength( ArrayPtrType( sym ) ) );
        CGDone( CGAssign( adv, temp, T_UINT_2 ) );
        if( sym->ns.si.va.dim_ext->dim_flags & DIM_EXTENDED ) {
            adv = CGFEName( sym, T_ARR_ALLOCATABLE_EXTENDED );
        } else {
            adv = CGFEName( sym, T_ARR_ALLOCATABLE );
        }
        typ = ArrayPtrType( sym );
        temp = CGInteger( 0, typ );
        CGDone( CGAssign( adv, temp, typ ) );
    } else {
        DGLabel( FEBack( sym ) );
        if( sym->ns.si.va.dim_ext->dim_flags & DIM_EXTENDED ) {
            DGIBytes( BETypeLength( T_LONG_POINTER ), 0 );
            DGInteger( ALLOC_EXTENDED, T_UINT_2 );
        } else {
            DGIBytes( BETypeLength( T_POINTER ), 0 );
            DGIBytes( BETypeLength( T_INT_2 ), 0 );
        }
    }
    AssignAdv( sym );
}


static  void    AssignAutoAdv( sym_id sym ) {
//===========================================

// Assign an auto ADV to an array.

    sym_id      shadow;

    shadow = STAdvShadow( sym );
    DumpAutoAdv( sym, shadow );
    sym->ns.si.va.dim_ext->adv = NULL;
}


static  void    AssignStaticAdv( sym_id sym ) {
//=============================================

// Assign a static ADV to an array.

    sym->ns.si.va.dim_ext->adv = BENewBack( NULL );
    DGAlign( ALIGN_DWORD );
    DGLabel( sym->ns.si.va.dim_ext->adv );
    DumpStaticAdv( sym, TRUE );
}


static cg_name AdvEntryAddr( cg_name adv, int entry, cg_type part ) {
//===================================================================

    int         offset;

    offset = BETypeLength( T_ADV_ENTRY ) * ( entry - 1 );
    if( part == T_ADV_HI ) {
        offset += BETypeLength( T_ADV_LO );
    }
    return( StructRef( adv, offset ) );
}


static cg_name CVAdvEntryAddr( cg_name adv, int dims, int entry, cg_type part ){
//==============================================================================

    int         offset;

    offset = ( BETypeLength( T_ADV_ENTRY_CV ) * entry ) +
             ( BETypeLength( T_ADV_ENTRY ) * dims );
    if( part == T_ADV_HI_CV ) {
        offset += BETypeLength( T_ADV_LO );
    }
    if( Options & OPT_BOUNDS ) {
        offset += BETypeLength( T_POINTER );
    }
    return( StructRef( adv, offset ) );
}


static void AssignName2Adv( sym_id sym ) {
//========================================

    act_dim_list        *dim_ptr;
    int                 subs_no;
    cg_name             adv;
    back_handle         data;

    dim_ptr = sym->ns.si.va.dim_ext;
    subs_no = _DimCount( dim_ptr->dim_flags );
    if( dim_ptr->adv == NULL ) {
        // ADV is allocated on the stack
        adv = CGFEName( FindAdvShadow( sym ), (T_ADV_ENTRY + subs_no) );
    } else {
        adv = CGBackName( dim_ptr->adv, ( T_ADV_ENTRY + subs_no ) );
    }
    adv = StructRef( adv, ( BETypeLength( T_ADV_ENTRY ) * subs_no ) );
    data = BENewBack( NULL );
    DGLabel( data );
    DumpSymName( sym );
    CGDone( CGAssign( adv, CGBackName( data, T_POINTER ), T_POINTER ) );
    BEFiniBack( data );
    PostponeFreeBackHandle( data );
}


static  void    DumpAutoAdv( sym_id sym, sym_id shadow ) {
//========================================================

// Dump an automatic array dope vector.

    act_dim_list        *dim_ptr;
    int                 subs_no;
    int                 entry;
    intstar4            *bounds;
    intstar4            lo;
    intstar4            hi;
    cg_name             adv;
    int                 dims;


    dim_ptr = sym->ns.si.va.dim_ext;
    subs_no = _DimCount( dim_ptr->dim_flags );
    dims = subs_no;
    bounds = &dim_ptr->subs_1_lo;
    entry = 1;

    CGAutoDecl( shadow, ( T_ADV_ENTRY + subs_no ) );
    for(;;) {
        lo = *bounds;
        bounds++;
        hi = *bounds;
        bounds++;
        adv = CGFEName( shadow, ( T_ADV_ENTRY + subs_no ) );
        CGDone( CGAssign( AdvEntryAddr( adv, entry, T_ADV_LO ),
                CGInteger( lo, T_ADV_LO ), T_ADV_LO ) );
        if( ( dim_ptr->dim_flags & DIM_ASSUMED ) && ( subs_no == 1 ) ) {
            hi = 0;
        } else {
            hi = hi - lo + 1;
        }
        adv = CGFEName( shadow, ( T_ADV_ENTRY + subs_no ) );
        CGDone( CGAssign( AdvEntryAddr( adv, entry, T_ADV_HI ),
                CGInteger( hi, T_ADV_HI ), T_ADV_HI ) );
        if( --subs_no == 0 ) break;
        entry++;
    }
    if( CGOpts & CGOPT_DI_CV ) {
        entry = 1;
        subs_no = _DimCount( dim_ptr->dim_flags );
        bounds = &dim_ptr->subs_1_lo;
        for(;;) {
            lo = *bounds;
            bounds++;
            hi = *bounds;
            bounds++;
            adv = CGFEName( shadow, ( T_ADV_ENTRY + subs_no ) );
            CGDone( CGAssign( CVAdvEntryAddr( adv, dims, entry, T_ADV_LO ),
                    CGInteger( lo, T_ADV_LO ), T_ADV_LO ) );
            if( ( dim_ptr->dim_flags & DIM_ASSUMED ) && ( subs_no == 1 ) ) {
                hi = 0;
            }
            adv = CGFEName( shadow, ( T_ADV_ENTRY + subs_no ) );
            CGDone( CGAssign( CVAdvEntryAddr( adv, dims, entry, T_ADV_HI_CV ),
                    CGInteger( hi, T_ADV_HI_CV ), T_ADV_HI_CV ) );
            if( --subs_no == 0 ) break;
            entry++;
        }
    }
}


static  void    DumpStaticAdv( sym_id sym, bool dmp_nam_ptr ) {
//=============================================================

// Dump a global array dope vector.

    act_dim_list        *dim_ptr;
    int                 subs_no;
    intstar4            *bounds;
    intstar4            lo;
    intstar4            hi;

    dim_ptr = sym->ns.si.va.dim_ext;
    subs_no = _DimCount( dim_ptr->dim_flags );
    bounds = &dim_ptr->subs_1_lo;
    for(;;) {
        lo = *bounds;
        bounds++;
        hi = *bounds;
        bounds++;
        DGInteger( lo, T_ADV_LO );
        if( ( dim_ptr->dim_flags & DIM_ASSUMED ) && ( subs_no == 1 ) ) {
            DGInteger( 0, T_ADV_HI );
        } else {
            DGInteger( hi - lo + 1, T_ADV_HI );
        }
        if( --subs_no == 0 ) break;
    }
    if( ( Options & OPT_BOUNDS ) && dmp_nam_ptr ) {  // dump ptr to array name
        DGInteger( 0, T_POINTER );
    }
    if( CGOpts & CGOPT_DI_CV ) {
        subs_no = _DimCount( dim_ptr->dim_flags );
        bounds = &dim_ptr->subs_1_lo;
        for(;;) {
            lo = *bounds;
            bounds++;
            hi = *bounds;
            bounds++;
            DGInteger( lo, T_ADV_LO );
            if( ( dim_ptr->dim_flags & DIM_ASSUMED ) && ( subs_no == 1 ) ) {
                DGInteger( 0, T_ADV_HI_CV );
            } else {
                DGInteger( hi, T_ADV_HI_CV );
            }
            if( --subs_no == 0 ) break;
        }
    }
}


static  void    DumpBrTable( void ) {
//=============================

// Dump the branch table ( for wild goto's ).

    sym_id              stmt;
    sel_handle          s_handle;
    label_handle        end_sel;

    end_sel = BENewLabel();
    CGControl( O_GOTO, NULL, end_sel );
    CGControl( O_LABEL, NULL, GetLabel( StNumbers.branches ) );
    s_handle = CGSelInit();
    stmt = SList;
    while( stmt != NULL ) {
        if( ( stmt->st.flags & SN_ASSIGNED ) &&
            ( ( stmt->st.flags & SN_BAD_BRANCH ) == 0 ) ) {
            CGSelCase( s_handle, GetStmtLabel( stmt ), stmt->st.address );
        }
        stmt = stmt->st.link;
    }
    CGSelOther( s_handle, end_sel );
    CGSelect( s_handle, CGUnary( O_POINTS,
                                 CGFEName( WildLabel, T_INTEGER ),
                                 T_INTEGER ) );
    CGControl( O_LABEL, NULL, end_sel );
    BEFiniLabel( end_sel );
}


static  void    DumpLitSCBs( void ) {
//=============================

// Dump string control blocks for constant literals.

    sym_id      sym;
    back_handle data;

    for( sym = LList; sym != NULL; sym = sym->lt.link ) {
        if( !( sym->lt.flags & LT_EXEC_STMT ) ) continue;
        if( !( sym->lt.flags & ( LT_SCB_REQUIRED | LT_SCB_TMP_REFERENCE ) ) ) {
            continue;
        }
        data = BENewBack( NULL );
        DGLabel( data );
        DGString( (char *)&sym->lt.value, sym->lt.length );
        DumpSCB( ConstBack( sym ), data, sym->lt.length, FALSE, 0 );
        FreeBackHandle( &data );
    }
}


void    FreeLocalBacks( bool free_dbg_handles ) {
//===============================================

// Free back handles for local symbols in local symbol table.

    unsigned_16 flags;
    sym_id      sym;
    com_eq      *eq_ext;

    for( sym = NList; sym != NULL; sym = sym->ns.link ) {
        flags = sym->ns.flags;
        if( (flags & SY_CLASS) == SY_VARIABLE ) {
            FreeBackHandle( &sym->ns.address );
            if( (SubProgId->ns.flags & SY_SUBPROG_TYPE) != SY_BLOCK_DATA ) {
                if( flags & SY_SUB_PARM ) {
                    if( flags & SY_SUBSCRIPTED ) {
                        if( _AdvRequired( sym->ns.si.va.dim_ext ) ||
                            ((sym->ns.xt.size == 0) && (flags & SY_VALUE_PARM)) ||
                            (Options & OPT_BOUNDS) ) {
                            FreeBackHandle( &sym->ns.si.va.dim_ext->adv );
                        }
                    /* SCB for dummy arguments are always allocated on the
                       stack so we have no problems with recursion
                    } else if( sym->ns.typ == TY_CHAR ) {
                        if( SCBRequired( sym ) ) {
                            if( !(Options & OPT_AUTOMATIC) ) {
                                // if d2-level debugging information has been
                                // requested, don't free the back handle until
                                // we are told
                                if( !(CGOpts & CGOPT_DB_LOCALS) || free_dbg_handles ) {
                                    FreeBackHandle( &sym->ns.si.va.vi.alt_scb );
                                }
                            }
                        }
                    */
                    }
                } else {
                    if( flags & SY_SUBSCRIPTED ) {
                        if( _Allocatable( sym ) || (Options & OPT_BOUNDS) ) {
                            FreeBackHandle( &sym->ns.si.va.dim_ext->adv );
                        }
                    } else if( sym->ns.typ == TY_CHAR ) {
                        // character variable
                        if( (flags & SY_DATA_INIT) && !(flags & SY_IN_EC) ) {
                            FreeBackHandle( &sym->ns.si.va.bck_hdl );
                        } else if( flags & SY_IN_EQUIV ) {
                            eq_ext = sym->ns.si.va.vi.ec_ext;
                            if( eq_ext->ec_flags & LEADER ) {
                                if( !(eq_ext->ec_flags & MEMBER_IN_COMMON) ) {
                                    FreeBackHandle( &sym->ns.si.va.bck_hdl );
                                }
                            }
                        }
                    }
                }
            }
        } else if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
            if( flags & SY_SUB_PARM ) {
                FreeBackHandle( &sym->ns.address );
                if( (sym->ns.typ == TY_CHAR) && (CGOpts & CGOPT_DB_LOCALS) ) {
                    if( free_dbg_handles ) {
                        FreeBackHandle( &sym->ns.si.sp.alt_scb );
                    }
                }
            } else if( ( flags & SY_SUBPROG_TYPE ) == SY_STMT_FUNC ) {
                FreeBackHandle( &sym->ns.address );
                FreeSFHeader( sym );
            /*
            for function return values, the SCB is always allocated
            on the stack so we have no problems with recursion
            } else if( (flags & SY_SUBPROG_TYPE) == SY_FUNCTION ) {
                if( !(Options & OPT_DESCRIPTOR) &&
                    (flags & SY_PENTRY) && (sym->ns.typ == TY_CHAR) ) {
                    if( CommonEntry != NULL ) {
                        if( !(Options & OPT_AUTOMATIC) ) {
                            FreeBackHandle( &CommonEntry->ns.si.sp.alt_scb );
                        }
                    } else {
                        if( !(Options & OPT_AUTOMATIC) ) {
                            FreeBackHandle( &sym->ns.si.sp.alt_scb );
                        }
                    }
                }
            */
            }
        }
    }
    for( sym = CList; sym != NULL; sym = sym->cn.link ) {
        FreeBackHandle( &sym->cn.address );
    }
    for( sym = LList; sym != NULL; sym = sym->lt.link ) {
        FreeBackHandle( &sym->lt.address );
    }
    for( sym = MList; sym != NULL; sym = sym->ns.link ) {
        FreeBackHandle( &sym->ns.address );
    }
    for( sym = NmList; sym != NULL; sym = sym->nl.link ) {
        FreeBackHandle( &sym->nl.address );
    }
    FreeUsedBacks( TRUE );
    FiniLabels( FORMAT_LABEL );
    FreeBackHandle( &TraceEntry );
}


static void PostponeFreeBackHandle( back_handle data ) {
//======================================================

    old_back_handle     *tmp;

    tmp = FMemAlloc( sizeof( old_back_handle ) );
    tmp->next = OldBackHandles;
    OldBackHandles = tmp;
    tmp->old = data;
}


void    FreeUsedBacks( bool nuke ) {
//==================================

    old_back_handle     *tmp;

    while( OldBackHandles != NULL ) {
        tmp = OldBackHandles;
        OldBackHandles = tmp->next;
        if( nuke ) {
            BEFreeBack( tmp->old );
        }
        FMemFree( tmp );
    }
}


void    FreeGlobalData( void ) {
//========================

    if( Options & OPT_TRACE ) {
        if( ModuleName != NULL ) {
            BEFreeBack( ModuleName );
        }
    }
}


void    FreeGlobalBacks( void ) {
//=========================

// Free back handles for global symbols in local symbol table.

    unsigned_16 flags;
    sym_id      sym;

    for( sym = NList; sym != NULL; sym = sym->ns.link ) {
        flags = sym->ns.flags;
        if( ( flags & SY_CLASS ) != SY_SUBPROGRAM ) continue;
        if( ( ( flags & SY_SUBPROG_TYPE ) != SY_STMT_FUNC ) &&
            ( ( flags & SY_SUBPROG_TYPE ) != SY_REMOTE_BLOCK ) ) {
            if( sym->ns.address != NULL ) {
                BEFreeBack( sym->ns.address );
            }
        }
    }
}


static  void    FreeBackHandle( void *_back ) {
//=============================================

// Free back handles.

    back_handle     *back = _back;

    if( *back != NULL ) {
        /* no need to call BEFiniBack() since we immediately
           follow it with a call to BEFreeBack()
        BEFiniBack( *back );
        */
        BEFreeBack( *back );
        *back = NULL;
    }
}


void    DefineEntryPoint( entry_pt *ep ) {
//========================================

    sym_id      sp;
    cg_type     cgtyp;

    sp = ep->id;
    if( !(Options & OPT_DESCRIPTOR) ) {
        if( ( sp->ns.flags & SY_SUBPROG_TYPE ) == SY_FUNCTION ) {
            if( sp->ns.typ == TY_CHAR ) {
                CGParmDecl( ReturnValue, T_POINTER );
                CGParmDecl( STArgShadow( ReturnValue ), T_INTEGER );
            }
        }
    }
    DefineArgs( ep );
    if( ( sp->ns.flags & SY_SUBPROG_TYPE ) == SY_SUBROUTINE ) {
        if( ChkForAltRets( ep ) ) {
            CGAutoDecl( ReturnValue, T_INTEGER );
        }
    } else if( sp->ns.typ == TY_CHAR ) {
        if( Options & OPT_DESCRIPTOR ) {
            CGParmDecl( ReturnValue, T_POINTER );
        }
    } else {
        cgtyp = F772CGType( sp );
        CheckAutoSize( sp, cgtyp );
        CGAutoDecl( ReturnValue, cgtyp );
    }
    CGLastParm();
}


static  void    DefineCommonEntry( void ) {
//===================================

    entry_pt    *ep;
    parameter   *arg;
    aux_info    *aux;
    pass_by     *arg_aux;

    ep = Entries;
    while( ep != NULL ) {
        aux = AuxLookup( ep->id );
        arg_aux = aux->arg_info;
        for( arg = ep->parms; arg != NULL; arg = arg->link ) {
            if( !(arg->flags & (ARG_DUPLICATE | ARG_STMTNO)) ) {
                DeclareArg( arg, arg_aux );
                if( (arg->id->ns.typ == TY_CHAR) &&
                    !(Options & OPT_DESCRIPTOR) ) {
                    if( (arg_aux == NULL ) ||
                        !(arg_aux->info & (PASS_BY_VALUE | PASS_BY_DATA)) ) {
                        CGParmDecl( STArgShadow( arg->id ), T_INTEGER );
                    }
                }
            }
            if( ( arg_aux != NULL ) && ( arg_aux->link != NULL ) ) {
                arg_aux = arg_aux->link;
            }
        }
        ep = ep->link;
    }
    if( ( Entries->id->ns.flags & SY_SUBPROG_TYPE ) == SY_SUBROUTINE ) {
        CGParmDecl( EPValue, T_INTEGER );
        if( EntryWithAltRets() ) {
            CGAutoDecl( ReturnValue, T_INTEGER );
        }
    } else {
        CGParmDecl( ReturnValue, T_POINTER );
        if( (Entries->id->ns.typ == TY_CHAR) && !(Options & OPT_DESCRIPTOR) ) {
            CGParmDecl( STArgShadow( ReturnValue ), T_INTEGER );
        }
        CGParmDecl( EPValue, T_INTEGER );
    }
    CGLastParm();
}


static  void    DeclareShadowArgs( entry_pt *ep, aux_info *aux ) {
//================================================================

    parameter   *arg;
    pass_by     *arg_aux;

    arg_aux = aux->arg_info;
    for( arg = ep->parms; arg != NULL; arg = arg->link ) {
        if( !(arg->flags & ARG_STMTNO) ) {
            if( arg->id->ns.typ == TY_CHAR ) {
                if( (arg_aux == NULL) ||
                    !(arg_aux->info & (PASS_BY_VALUE | PASS_BY_DATA)) ) {
                    CGParmDecl( STArgShadow( arg->id ), T_INTEGER );
                }
            }
        }
        if( ( arg_aux != NULL ) && ( arg_aux->link != NULL ) ) {
            arg_aux = arg_aux->link;
        }
    }
}


static  void    DeclareArg( parameter *arg, pass_by *arg_aux ) {
//==============================================================

// Declare an argument.

    cg_type     arg_type;
    sym_id      arg_id;

    arg_id = arg->id;
    if( ( arg_id->ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) {
        arg_type = T_CODE_PTR;
    } else if( arg_id->ns.flags & SY_SUBSCRIPTED ) {
        arg_type = ArrayPtrType( arg_id );
        if( arg_id->ns.typ == TY_CHAR ) {
            if( arg_aux != NULL ) {
                if( arg_aux->info & (PASS_BY_VALUE | PASS_BY_DATA) ) {
                    arg_id->ns.flags |= SY_VALUE_PARM;
                }
            }
        }
    } else {
        arg_type = T_POINTER;
        if( arg_id->ns.typ == TY_CHAR ) {
            if( arg_aux != NULL ) {
                if( arg_aux->info & (PASS_BY_VALUE | PASS_BY_DATA) ) {
                    arg_id->ns.flags |= SY_VALUE_PARM;
                }
            }
        } else {
            if( arg_aux != NULL ) {
                if( arg_aux->info & PASS_BY_VALUE ) {
                    arg_type = F772CGType( arg_id );
                    arg_id->ns.flags |= SY_VALUE_PARM;
                    if( TypeCmplx( arg_id->ns.typ ) ) {
                        arg_type = CmplxBaseType( arg_type );
                        CGParmDecl( arg_id, arg_type );
                        arg_id = STArgShadow( arg_id );
                    }
                }
            }
        }
    }
    CGParmDecl( arg_id, arg_type );
}


static  void    DefineArgs( entry_pt *ep ) {
//==========================================

    parameter   *arg;
    aux_info    *aux;
    pass_by     *arg_aux;

    aux = AuxLookup( ep->id );
    if( (aux->cclass & REVERSE_PARMS) ) {
        ReverseList( &ep->parms );
        ReverseList( &aux->arg_info );
    }
    arg_aux = aux->arg_info;
    for( arg = ep->parms; arg != NULL; arg = arg->link ) {
        if( !(arg->flags & ARG_STMTNO) ) {
            DeclareArg( arg, arg_aux );
            if( ( arg_aux != NULL ) && ( arg_aux->link != NULL ) ) {
                arg_aux = arg_aux->link;
            }
        }
    }
    if( !(Options & OPT_DESCRIPTOR) ) {
        DeclareShadowArgs( ep, aux );
    }
    if( (aux->cclass & REVERSE_PARMS) ) {
        ReverseList( &ep->parms );
        ReverseList( &aux->arg_info );
    }
}
