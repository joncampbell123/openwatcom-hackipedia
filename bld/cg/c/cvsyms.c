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
* Description:  Emit CodeView symbol information.
*
****************************************************************************/


#include "standard.h"
#include <string.h>
#include "hostsys.h"
#include "coderep.h"
#include "pattern.h"
#include "procdef.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "symdbg.h"
#include "model.h"
#include "ocentry.h"
#include "objrep.h"
#include "zoiks.h"
#include "cgaux.h"
#include "typedef.h"
#include "dbgstrct.h"
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include "cvdbg.h"
#define BY_CG
#include "feprotos.h"
#include "cgprotos.h"

extern  void            FEPtrBaseOffset(sym_handle,offset);
extern  type_def        *TypeAddress(cg_type);
extern  seg_id          AskOP(void);
extern  bck_info        *BENewBack(sym_handle);
extern  void            BEFreeBack(bck_info*);
extern  void            OutLabel(label_handle);
extern  void            SetUpObj(bool);
extern  void            BackPtr( bck_info*, seg_id, offset, type_def* );
extern  name            *DeAlias(name*);
extern  seg_id          SetOP(seg_id);
extern  seg_id          AskCodeSeg();
extern  segment_id      AskSegID(pointer,cg_class);
extern  sym_handle      AskForLblSym(label_handle);
extern  void            SetBigLocation( long_offset loc );
extern  long_offset     AskBigLocation(void);
extern  offset          AskMaxSize(void);
extern  offset          AskAddress(label_handle);
extern  void            DataInt(short_offset);
extern  void            DataLong( long );
extern  char            GetMemModel( void );
#if _TARGET & _TARG_IAPX86
extern  hw_reg_set      Low32Reg(hw_reg_set);
#elif _TARGET & _TARG_80386
extern  hw_reg_set      Low64Reg(hw_reg_set);
#endif
extern  void            DataBytes(unsigned_32,byte*);
extern  void            DoBigLblPtr(sym_handle);
extern  void            DBLocFini( dbg_loc loc );
extern  void            CVPutStr( cv_out *, char * );
extern  void            CVPutINum( cv_out *out, signed_32 num );
extern  void            CVPutINum64( cv_out *out, signed_64 num );
extern  void            CVPutNullStr( cv_out * );
extern  void            CVEndType( cv_out *out );
extern  sym_handle      LocSimpStatic(dbg_loc);
extern  type_length     NewBase(name*);

extern  source_line_number  SrcLine;
extern  proc_def            *CurrProc;
extern  uint                TypeIdx;

// global variables
seg_id                  CVSyms;
seg_id                  CVTypes;

static  void             DumpLocals( dbg_local *local );

typedef struct block_patch {
    struct block_patch  *link;
    dbg_patch_handle    handle;
} block_patch;

#define CurrProc_debug ((dbg_rtn *)CurrProc->targ.debug)


struct sf_info {
    char       size;
    s_values   code;
} sf_info;

static struct sf_info SInfo[SG_LAST] = {
    #define SLMAC( n, N, c )    { sizeof( s_##n ), c },
    #include "cv4syms.h"
    #undef SLMAC
};

static  void    NewBuff( cv_out *out, seg_id seg )
/************************************************/
{
    out->ptr = &out->buff[0];
    out->beg = &out->buff[0];
    out->seg = seg;
}

static void BuffPatchSet( seg_id seg, dbg_patch_handle *patch )
/*************************************************************/
{
    long_offset         off;
    seg_id              old;

   old = SetOP(seg );
   off = AskBigLocation();
   SetOP( old );
   patch->segment = seg;
   patch->offset = off;
}

static  void    BuffWrite( cv_out *out, void *to )
/************************************************/
{
    int     len;
    seg_id  old;

    len = (byte *)to - out->beg;
    old = SetOP( out->seg );
    DataBytes( len, out->beg );
    out->beg = to;
    SetOP( old );
}

static  void   BuffSkip( cv_out *out, void *to )
/**********************************************/
{
    out->beg = to;
}

static  void    BuffEnd( cv_out *out )
/************************************/
{
    int     len;
    seg_id  old;

    len = out->ptr - out->beg;
    old = SetOP( out->seg );
    DataBytes( len, out->beg );
    SetOP( old );
}

static  void  *StartSym( cv_out *out, sg_index what )
{
    s_common    *ptr;

    ptr = (s_common *)out->beg;
    out->ptr += SInfo[what].size;
    ptr->code = SInfo[what].code;
    ++ptr; /* skip to start of cs_... */
    return( ptr );
}

static  void    EndSym( cv_out *out )
/***********************************/
{
    int         len;
    s_common    *com;
    byte        *ptr;

    com = (s_common *) out->beg; /* assume ptr marks end of a sym */
    ptr = out->ptr;
    len = ptr - out->beg;
#if CVSIZE == 32
    while( (len & CV_ALIGN-1)  != 0 ){
        *ptr = '\0';
        ++len;
        ++ptr;
    }
    out->ptr = ptr;
#endif
    com->length  = len-sizeof( com->length );  /*don't count length field */
}

extern  void    CVInitDbgInfo( void )
/***********************************/
/* called after ObjInit */
{
    TypeIdx   = CV_FIRST_USER_TYPE-1;
}

#define MAX_LANG 4
struct lang_map{
     uint       lang;
     char       name[10];
};

static struct lang_map LangNames[MAX_LANG] = {
    {LANG_C,       "C"},
    {LANG_CPP,     "CPP"},
    {LANG_FORTRAN, "FORTRAN"},
    {LANG_FORTRAN, "FORTRAN77"},
};

static int SetLang( void )
{
    int     ret;
    char    *name;
    int     index;

    ret = LANG_C;
    name =  FEAuxInfo( NULL, SOURCE_LANGUAGE );
    for( index = 0; index < MAX_LANG; ++index ){
        if( strcmp( name, LangNames[index].name ) == 0 ){
            ret = LangNames[index].lang;
            break;
        }
    }
    return( ret );
}


static  void    InitSegBck( void )
/********************************/
{
    seg_id       old;

    if( _IsModel( DBG_LOCALS ) ) {
        old = SetOP( CVSyms );
        DataLong( CV_OMF_SIG );
    }
    if( _IsModel( DBG_TYPES ) ) {
        SetOP( CVTypes );
        DataLong( CV_OMF_SIG );
        SetOP( old );
    }
}


extern  void    CVObjInitInfo( void )
/***********************************/
/* called by objinit to init segments and for codeview */
{
    cv_out      out[1];
    cs_objname  *optr;
    cs_compile  *cptr;
    char        *name;

    InitSegBck();
    if( _IsModel( DBG_LOCALS ) ) {
        NewBuff( out, CVSyms );
        optr = StartSym(  out, SG_OBJNAME );
        optr->signature = 0;
        name =  FEAuxInfo( NULL, OBJECT_FILE_NAME );
        CVPutStr( out, name );
        EndSym( out );
        BuffEnd( out );
        NewBuff( out, CVSyms );
        cptr = StartSym(  out, SG_COMPILE );
        cptr->language  = SetLang();
        cptr->flags.s = 0; /* set default */
    #if _TARGET & _TARG_IAPX86
        cptr->flags.f.Mode32 = FALSE;
        cptr->machine = MACH_INTEL_8080;
    #elif _TARGET & _TARG_80386
        cptr->machine = MACH_INTEL_80386;
        cptr->flags.f.Mode32 = TRUE;
    #elif _TARGET & _TARG_AXP
        cptr->machine = MACH_DECALPHA;
        cptr->flags.f.Mode32 = TRUE;
        cptr->flags.f.FloatPrecision = 1;
    #endif
        switch( GetMemModel() ){
            case 'h':
                cptr->flags.f.AmbientData = AMBIENT_HUGE;
                cptr->flags.f.AmbientCode = AMBIENT_FAR;
                break;
            case 'l':
                cptr->flags.f.AmbientData = AMBIENT_FAR;
                cptr->flags.f.AmbientCode = AMBIENT_FAR;
                break;
            case 'f':
            case 's':
                cptr->flags.f.AmbientData = AMBIENT_NEAR;
                cptr->flags.f.AmbientCode = AMBIENT_NEAR;
                break;
            case 'c':
                cptr->flags.f.AmbientData = AMBIENT_FAR;
                cptr->flags.f.AmbientCode = AMBIENT_NEAR;
                break;
            case 'm':
                cptr->flags.f.AmbientData = AMBIENT_NEAR;
                cptr->flags.f.AmbientCode = AMBIENT_FAR;
                break;
            default:
                break;
        }
        CVPutStr( out, "WATCOM CV 10.5   " );
        EndSym( out );
        BuffEnd( out );
    }
}


extern  void    CVFiniDbgInfo( void )
/***********************************/
{
}


extern  void    CVObjFiniDbgInfo( void )
/**************************************/
{
}



static  void    SymReloc(  seg_id seg,  sym_handle sym, offset lc )
/*****************************************************************/
{
    seg_id      old;

    old = SetOP( seg );
    FEPtrBaseOffset( sym, lc );
    SetOP( old );
}

static void LabelReloc( seg_id seg, bck_info *bck, long disp )
/************************************************************/
{
    type_def   *ptr_type;
    seg_id      id;
    seg_id      old;

    old = SetOP( seg );
    id = AskSegID( bck, CG_BACK );
    ptr_type = TypeAddress( T_LONG_POINTER );
    BackPtr( bck, id, disp, ptr_type );
    SetOP( old );
}

extern  void    CVOutBck( cv_out *out, bck_info *bck, offset add, dbg_type tipe )
/*** Put a back sym out*********************************************************/
{
    cs_ldata    *ptr;
    void        *ptr1;

    ptr1 = out->beg;
    out->beg = out->ptr;
    ptr = StartSym(  out, SG_LDATA );
    ptr->offset = 0;
    ptr->segment = 0;
    ptr->type = tipe;
    CVPutStr( out, "__bck" );
    EndSym( out );
    out->beg = ptr1;
    CVEndType( out );
    BuffWrite( out, &ptr->offset );
    LabelReloc( out->seg, bck, add );
    BuffSkip( out, &ptr->type );
    BuffEnd( out );
}

static  void FrameVar( cv_out  *out,  char        *nm,
                       dbg_type tipe, long        disp )
/***  local rel to  frame  ****************************/
#if 1     // it seems like BPREL works for AXP so I'll give it a try
{
//#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    cs_bprel   *ptr;

    ptr = StartSym(  out, SG_BPREL );
    ptr->offset = disp;
    ptr->type = tipe;
    CVPutStr( out, nm );
    EndSym( out );
#elif 0
//#elif _TARGET & _TARG_AXP
    cs_regrel    *ptr;

    ptr = StartSym(  out, SG_REGREL   );
    ptr->offset = disp;
    ptr->type = tipe;
    ptr->reg = 0x48;   // not wired in need real frame reg.
    CVPutStr( out, nm );
    EndSym( out );
#endif
}

extern  void    CVOutSym( cv_out *out, sym_handle sym )
/*** Put a sym in out ********************************/
{
    dbg_type    tipe;
    fe_attr     attr;
    char        *nm;

    attr = FEAttr( sym );
    tipe = FEDbgType( sym );
    nm = FEName( sym );
    if( attr & FE_STATIC ) {
        cs_gdata     *ptr;
        sg_index    kind;

        if( attr & FE_GLOBAL ) {
            kind = SG_GDATA;
        } else {
            kind = SG_LDATA;
        }
        ptr = StartSym(  out, kind );
        ptr->offset = 0;
        ptr->segment = 0;
        ptr->type = tipe;
        CVPutStr( out, nm );
        EndSym( out );
    } else {
        FrameVar( out, nm, tipe, 0 );
    }
}

extern  void    CVGenStatic( sym_handle sym, dbg_loc loc, bool mem )
/******************************************************************/
{
    dbg_type    tipe;
    cv_out      out[1];
    fe_attr     attr;
    cs_gdata     *ptr;
    sg_index     kind;
    char        *name;

    attr = FEAttr( sym );
    tipe = FEDbgType( sym );
    NewBuff( out, CVSyms );
    attr = FEAttr( sym );
    if( attr & FE_GLOBAL ) {
        kind = SG_GDATA;
    } else {
        kind = SG_LDATA;
    }
    ptr = StartSym(  out, kind );
    ptr->offset = 0;
    ptr->segment = 0;
    ptr->type = tipe;
    if( mem ){
        name = FEAuxInfo( sym, CLASS_APPENDED_NAME );
    } else {
        name = FEName( sym );
    }
    CVPutStr( out, name );
    EndSym( out );
    if( LocSimpStatic( loc ) == sym ) {
        BuffWrite( out, &ptr->offset );
        SymReloc( CVSyms, sym, 0 );
        BuffSkip( out, &ptr->type );
    } else {
        /*TODO:can't handle locs */
    }
    BuffEnd( out );
}


extern  void    CVTypedef( char *nm, dbg_type tipe )
/*** emit UDT**************************************/
{
    cv_out      out[1];
    cs_udt       *ptr;

    NewBuff( out, CVSyms );
    ptr = StartSym(  out, SG_UDT );
    ptr->type = tipe;
    CVPutStr( out, nm );
    EndSym( out );
    BuffEnd( out );
}

extern  void    CVOutSymICon( cv_out *out, char *nm, long val, dbg_type tipe )
/*** Put a const sym to out**************************************************/
{
    cs_constant *ptr;
    void        *ptr1;

    ptr1 = out->beg;
    out->beg = out->ptr;
    ptr = StartSym(  out, SG_CONSTANT );
    ptr->type = tipe;
    CVPutINum( out, val );
    CVPutStr( out, nm );
    EndSym( out );
    out->beg = ptr1;
    CVEndType( out );
    BuffEnd( out );
}

extern  void    CVSymIConst( char *nm, long val, dbg_type tipe )
/*** emit UDT**************************************************/
{
    cv_out      out[1];
    cs_constant *ptr;

    NewBuff( out, CVSyms );
    ptr = StartSym(  out, SG_CONSTANT );
    ptr->type = tipe;
    CVPutINum( out, val );
    CVPutStr( out, nm );
    EndSym( out );
    BuffEnd( out );

}

extern  void    CVSymIConst64( char *nm, signed_64 val, dbg_type tipe )
/*** emit UDT*********************************************************/
{
    cv_out      out[1];
    cs_constant *ptr;

    NewBuff( out, CVSyms );
    ptr = StartSym(  out, SG_CONSTANT );
    ptr->type = tipe;
    CVPutINum64( out, val );
    CVPutStr( out, nm );
    EndSym( out );
    BuffEnd( out );

}

extern  void    CVSetBase( void )
/*******************************/
/* don't know about this yet */
{
}

/**/
/* Coming out of optimizer queue*/
/**/

extern  void    CVRtnBeg( dbg_rtn *rtn, offset lc )
/*************************************************/
{
    rtn = rtn;
    lc  = lc;
}

static dbg_patch_handle RtnPatch[1];

static  name    *LocSymBP( dbg_loc loc )
/**************************************/
{
    if( loc == NULL ) return( NULL );
    if( (loc->class & 0xf0) != LOC_BP_OFFSET ){
        return( NULL );
    }
    return( loc->u.be_sym );
}


static  dbg_local *UnLinkLoc( dbg_local **owner, sym_handle sym )
/***************************************************************/
// unlink dbg_local with sym from owner
{
    dbg_local           *curr;

    while( (curr = *owner) != NULL ) {
        if( curr->sym == sym ){
            *owner = curr->link;
            break;
        }
        owner = &(*owner)->link;
    }
    return( curr );
}

static  void DumpParms( dbg_local *parm, dbg_local **locals )
{
    dbg_local   *alt;
    cv_out      out[1];
    dbg_local   *junk;

    while( parm != NULL ) {  /* find and unlink from locals */
        alt = UnLinkLoc( locals, parm->sym );
        if( alt != NULL ){
            dbg_type    tipe;
            type_length offset;
            name       *t;

            if( alt->kind == DBG_SYM_VAR ){
                tipe = FEDbgType( alt->sym );
                t = LocSymBP( alt->loc );
                if( t != NULL ){
                    offset = NewBase( t );
                    NewBuff( out, CVSyms );
                    FrameVar( out, FEName( alt->sym ), tipe, offset );
                    BuffEnd( out );
                }else{
                    CVGenStatic( alt->sym, alt->loc, FALSE );
                }
            }
            DBLocFini( alt->loc );
           CGFree( alt );
        }
        DBLocFini( parm->loc );
        junk = parm;
        parm = parm->link;
        CGFree( junk );
    }
//#if _TARGET & _TARG_AXP
#if 0 // seems like it screws CVPACK on intel
    NewBuff( out, CVSyms );
    StartSym(  out, SG_ENDARG );
    EndSym( out );
    BuffEnd( out );
#endif
}

extern  void    CVProEnd( dbg_rtn *rtn, offset lc )
/*************************************************/
{
    sym_handle          sym;
    dbg_type            tipe;
    fe_attr             attr;
    char               *name;
    call_class         *class_ptr;
    cs_gproc           *ptr;
    sg_index            kind;
    cv_out              out[1];

    lc = lc;
    sym = AskForLblSym( CurrProc->label );
    class_ptr = FEAuxInfo( FEAuxInfo( sym, AUX_LOOKUP ), CALL_CLASS );
    attr = FEAttr( sym );
    if( attr & FE_GLOBAL ){
        kind = SG_GPROC;
    }else{
        kind = SG_LPROC;
    }
    NewBuff( out, CVSyms );
    ptr = StartSym(  out, kind );
    ptr->pParent = 0;
    ptr->pEnd = 0;
    ptr->pNext = 0;
    ptr->proc_length = 0;
    ptr->debug_start = rtn->pro_size;
    ptr->debug_end = 0;
    ptr->offset = 0;
    ptr->segment = 0;
    tipe = FEDbgType( sym );
    ptr->proctype = tipe;
    ptr->flags.s = 0;
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    if( *class_ptr & FAR ) {
        ptr->flags.f.far_ret = TRUE;
    }
#endif
    if( rtn->obj_type != DBG_NIL_TYPE ) {
        name = FEAuxInfo( sym, CLASS_APPENDED_NAME );
    } else {
        name = FEName( sym );
    }
    CVPutStr( out, name );
    EndSym( out );
    BuffPatchSet( CVSyms, RtnPatch );
    BuffWrite( out, &ptr->offset );
    SymReloc( CVSyms, sym, 0 );
    BuffSkip( out, &ptr->proctype );
    BuffEnd( out );
    DBLocFini( rtn->reeturn );
    DBLocFini( rtn->obj_loc );
    if( rtn->parms != NULL ){
        DumpParms( rtn->parms, &rtn->blk->locals );
    }
    DumpLocals( rtn->blk->locals );
}

extern  void    CVBlkBeg( dbg_block *blk, offset lc )
/***************************************************/
{
    block_patch        *patch;
    dbg_patch_handle   *handle;
    cv_out             out[1];
    offset             start;
    sym_handle         sym;
    cs_block           *ptr;
    byte               *nm;


    patch = CGAlloc( sizeof( block_patch ) );
    blk->patches = patch;
    NewBuff( out, CVSyms );
    ptr = StartSym(  out, SG_BLOCK );
    ptr->pParent = 0;
    ptr->pEnd = 0;
    ptr->length = 0;
    ptr->offset = 0;
    ptr->segment = 0;
    nm = out->ptr;     /* mark name */
    CVPutNullStr( out );
    EndSym( out );
    handle = &((block_patch *)(blk->patches))->handle;
    BuffPatchSet( CVSyms, handle );
    BuffWrite( out, &ptr->offset );
    sym = AskForLblSym( CurrProc->label );
    start = lc - CurrProc_debug->blk->start;
    SymReloc( CVSyms, sym, start );
    BuffSkip( out, nm );      /* skip addr */
    BuffEnd( out );
    DumpLocals( blk->locals );
}

extern  void    CVBlkEnd( dbg_block *blk, offset lc )
/***************************************************/
{
    fsize               length;
    long_offset         here;
    seg_id              old;
    dbg_patch_handle   *handle;
    cv_out              out[1];

    handle = &((block_patch *)(blk->patches))->handle;
    old = SetOP( handle->segment );
    here = AskBigLocation();
    SetBigLocation( handle->offset + offsetof( s_block, f.length ) );
    length = lc - blk->start;
    DataBytes( sizeof( fsize ), (byte *)&length );
    SetBigLocation( here );
    SetOP( old );
    NewBuff( out, CVSyms );
    StartSym(  out, SG_END );
    EndSym( out );
    BuffEnd( out );
   CGFree( blk->patches );
}

extern  void    CVEpiBeg( dbg_rtn *blk, offset lc )
/*************************************************/
{
    blk = blk;
    lc  = lc;
}

extern  void    CVRtnEnd( dbg_rtn *rtn, offset lc )
/*************************************************/
{
    cv_out              out[1];
    fsize               proc_length;
    fsize               debug_end;
    dbg_patch_handle   *handle;
    long_offset         here;
    seg_id              old;

    handle = RtnPatch;
    old = SetOP( handle->segment );
    here = AskBigLocation();
    SetBigLocation( handle->offset + offsetof( s_gproc, f.proc_length ) );
    proc_length = lc - rtn->blk->start;
    DataBytes( sizeof( fsize ), (byte *)&proc_length );
    SetBigLocation( handle->offset+ offsetof( s_gproc, f.debug_end ) );
    debug_end   = rtn->epi_start - rtn->blk->start;
    DataBytes( sizeof( fsize ), (byte *)&debug_end );
    SetBigLocation( here );
    SetOP( old );
    NewBuff( out, CVSyms );
    StartSym(  out, SG_END );
    EndSym( out );
    BuffEnd( out );
}


static  void    DumpLocals( dbg_local *local )
/********************************************/
{
    dbg_local  *old;
    type_length offset;
    cv_out      out[1];
    dbg_type    tipe;
    name       *t;

    while( local != NULL ) {
        switch( local->kind ){
        case DBG_SYM_VAR:
            tipe = FEDbgType( local->sym );
            t = LocSymBP( local->loc );
            if( t != NULL ){
                offset = NewBase( t );
                NewBuff( out, CVSyms );
                FrameVar( out, FEName( local->sym ), tipe, offset );
                BuffEnd( out );
            }else{
                CVGenStatic( local->sym, local->loc, FALSE );
            }
            break;
        case DBG_SYM_TYPE:
            break;
        case DBG_SYM_TYPEDEF:
            tipe = FEDbgType( local->sym );
            CVTypedef( FEName( local->sym ), tipe );
            break;
        }
        DBLocFini( local->loc );
        old = local;
        local = local->link;
        CGFree( old );
    }

}


extern  void    CVOutLocal( cv_out *out, name *t, int disp,  dbg_type tipe )
/*** Put a local back sym out**********************************************/
{
    type_length offset;
    void        *ptr1;


    offset = NewBase( t );
    ptr1 = out->beg;
    out->beg = out->ptr;
    FrameVar( out, "__bck", tipe, offset+disp );
    out->beg = ptr1;
    CVEndType( out );
    BuffEnd( out );
}
