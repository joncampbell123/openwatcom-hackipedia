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
* Description:  Take the instruction stream from the code generator and
*               writes the instructions into the object file.
*
****************************************************************************/


#include "standard.h"
#include "cgdefs.h"
#include "coderep.h"
#include "opcodes.h"
#include "pattern.h"
#include "ocentry.h"
#include "vergen.h"
#include "pccode.h"
#include "system.h"
#include "escape.h"
#include "model.h"
#include "pcencode.h"
#include "objrep.h"
#include "regset.h"
#include "rttable.h"
#include "rtclass.h"
#include "zoiks.h"
#include "zeropage.h"
#include "fppatch.h"
#include "cfloat.h"
#include "cgaux.h"
#include "feprotos.h"
#include "p5prof.h"
#include "procdef.h"

extern  void            DoAbsPatch(abspatch_handle*,int);
extern  void            DoFunnyRef(int);
extern  hw_reg_set      High32Reg(hw_reg_set);
extern  hw_reg_set      Low32Reg(hw_reg_set);
extern  void            DoSegRef(seg_id);
extern  hw_reg_set      CalcSegment(sym_handle,cg_class);
extern  void            EmitDbgInfo(instruction*);
extern  void            DoCall(label_handle,bool,bool,oc_class);
extern  void            RTCall( rt_class rtn, oc_class pop_bit );
extern  label_handle    RTLabel(int);
extern  void            GenMJmp(instruction*);
extern  void            GenRJmp(instruction*);
extern  void            GenICall(instruction*);
extern  void            GenRCall(instruction*);
extern  void            GenCall(instruction*);
extern  abspatch_handle *NextFramePatch( void );
extern  bool            AskIsFrameIndex(name*);
extern  void            GenCondJump(instruction*);
extern  int             NumOperands(instruction*);
extern  void            InputOC(any_oc*);
extern  void            DoRepOp( instruction *ins );
extern  void            Do4CXShift(instruction *,void (*)(instruction*) );
extern  void            LayLeaOp(instruction*);
extern  void            LayModRM(name*);
extern  byte            DoMDisp(name*,bool);
extern  void            DoMAddr(name*);
extern  void            DoRelocConst(name*,type_class_def);
extern  void            Do4Shift(instruction*);
extern  void            Do4RShift(instruction*);
extern  void            Gen4RNeg(instruction*);
extern  void            Gen4Neg(instruction*);
extern  void            Pow2Div(instruction*);
extern  void            By2Div(instruction*);
extern  void            Pow2Div286(instruction*);
extern  void            LayLeaRegOp(instruction*);
extern  pointer         Copy(pointer,pointer,uint);
extern  void            GenUnkLea(pointer);
extern  name            *IntEquivalent(name*);
extern  void            LookupRoutine(instruction*);
extern  void            AdjustStackDepth(instruction*);
extern  void            AdjustStackDepthDirect(int adjust);

extern  hw_reg_set      FullReg(hw_reg_set);
extern  bool            BaseIsSP(name*);
extern  type_length     TmpLoc(name*,name*);
extern  name            *DeAlias(name*);
extern  seg_id          AskBackSeg( void );
extern  seg_id          AskCodeSeg( void );
extern  void            DoLblRef(label_handle,seg_id,offset,escape_class);
extern  void            AddWData(signed_32,type_class_def );
extern  label_handle    AskForNewLabel( void );
extern  name            *LowPart(name *,type_class_def);
extern  name            *HighPart(name *,type_class_def);
extern  void            CodeLabel(label_handle, unsigned);
extern  int             OptInsSize(oc_class,oc_dest_attr);
extern  void            GenJumpLabel( label_handle );
extern  void            GenKillLabel( label_handle );
extern  segment_id      GenP5ProfileData( char *fe_name, label_handle *data, label_handle *stack );
extern  void            EndBlockProfiling( void );
extern  void            LayOpbyte( opcode op );
extern  void            LayOpword( opcode op );
extern  void            LayW( type_class_def class );
extern  void            GenSeg( hw_reg_set regs );
extern  void            GenLoadDS(void);
extern  void            LayRMRegOp( name *r );
extern  void            AddWCons( name *op, type_class_def kind );
extern  void            LayReg( hw_reg_set r );
extern  void            GCondFwait(void);
extern  void            GFldz(void);
extern  void            AddSData( signed_32 value, type_class_def kind );
extern  void            GFwait(void);
extern  void            GCondFwait(void);
extern  void            GFldM( pointer what );

static  void            AddSWCons( opcode_defs op, name *opnd, type_class_def class );
static  void            TransferIns(void);
static  void            LayST( name *op );
static  void            LayMF( name *op );
static  void            AddSCons( name *op, type_class_def kind );
static  void            LayACRegOp( name *r );
static  void            LayRegOp( name *r );
static  void            LaySROp( name *r );
static  void            SetCC(void);
static  void            CallMathFunc( instruction *ins );
static  void            MathFunc( instruction *ins );
static  void            PopSeg( hw_reg_set reg );
static  void            PushSeg( hw_reg_set reg );
static  void            GFld1(void);
static  int             FPRegTrans( hw_reg_set reg );


extern    pccode_def            PCCodeTable[];
extern    name                  *FPStatWord;
extern    byte                  OptForSize;
extern    bool                  Used87;
extern    proc_def              *CurrProc;

          template              Temp;   /* template for oc_entries*/
          byte                  Inst[INSSIZE];  /* template for instructions*/

static    int   ICur;           /* cursor for writing into Inst*/
static    int   IEsc;           /* number of initial bytes that must be*/
                                /* checked for escapes when copied into Temp*/
          int   ILen;           /* length of object instruction*/
          fp_patches    FPPatchType;

static  hw_reg_set RegTab[] = {
#define REGS 24
        HW_D( HW_AL ),          HW_D( HW_AX ),          HW_D( HW_EAX ),
        HW_D( HW_CL ),          HW_D( HW_CX ),          HW_D( HW_ECX ),
        HW_D( HW_DL ),          HW_D( HW_DX ),          HW_D( HW_EDX ),
        HW_D( HW_BL ),          HW_D( HW_BX ),          HW_D( HW_EBX ),
        HW_D( HW_AH ),          HW_D( HW_SP ),          HW_D( HW_SP ),
        HW_D( HW_CH ),          HW_D( HW_BP ),          HW_D( HW_BP ),
        HW_D( HW_DH ),          HW_D( HW_SI ),          HW_D( HW_ESI ),
        HW_D( HW_BH ),          HW_D( HW_DI ),          HW_D( HW_EDI ) };

static  hw_reg_set SegTab[] = {
#define SEGS 6
        HW_D( HW_ES ),          HW_D( HW_CS ),
        HW_D( HW_SS ),          HW_D( HW_DS ),
        HW_D( HW_FS ),          HW_D( HW_GS ) };

static  fp_patches SegPatchTab[] = {
        FPP_ES,         FPP_CS,
        FPP_SS,         FPP_DS,
        FPP_FS,         FPP_GS };

        hw_reg_set FPRegs[] = {
        HW_D( HW_ST0 ),
        HW_D( HW_ST1 ),
        HW_D( HW_ST2 ),
        HW_D( HW_ST3 ),
        HW_D( HW_ST4 ),
        HW_D( HW_ST5 ),
        HW_D( HW_ST6 ),
        HW_D( HW_ST7 ) };

/* routines that maintain instruction buffers*/

extern  void    Format( oc_class class ) {
/*****************************************
    Get Temp and Inst ready to accept instructions.  Each instruction is
    formatted into Inst, transferred into Temp (as an opcode_entry) and
    then dumped into the peephole optimizer.
*/

    FPPatchType = FPP_NONE;
    Temp.oc.class = class;
    Temp.oc.objlen = 0;
    Temp.oc.reclen = sizeof( oc_header );
    ICur = 0;
    ILen = 0;
    IEsc = 0;
}

extern  void    ReFormat( oc_class class ) {
/*******************************************
    Change the class of Temp
*/

    Temp.oc.class = class;
}

extern  void    AddByte( byte b ) {
/**********************************
    Add a byte to Inst[]
*/

    if( b == ESC ) {
        Inst[ICur++] = b;
    }
    Inst[ICur++] = b;
    ILen++;
}

extern  void    AddToTemp( byte b ) {
/************************************
    Add a byte to the end of Temp
*/

    if( b == ESC ) {
        Temp.data[Temp.oc.reclen++ - sizeof( oc_header )] = b;
    }
    Temp.data[Temp.oc.reclen++ - sizeof( oc_header )] = b;
    Temp.oc.objlen++;
}

extern  void    InsertByte( byte b ) {
/*************************************
    Insert a byte into the beginning of Temp. It may not be ESC!
*/

    int         i;
    byte        *src;
    byte        *dst;

    i = Temp.oc.reclen - sizeof( oc_header );
    dst = &Temp.data[i];
    src = &Temp.data[i - 1];
    while( --i >= 0 ) {
        *dst = *src;
        --dst;
        --src;
    }
    Temp.data[0] = b;
    Temp.oc.reclen++;
}


extern  void    EmitByte( byte b ) {
/***********************************
    Plop a byte into Inst[]
*/

    Inst[ICur++] = b;
}

extern  void    EmitPtr( pointer p ) {
/*************************************
    Plop a pointer into Inst[]
*/

    *(pointer *)(Inst + ICur) = p;
    ICur += sizeof( pointer );
}

extern  void    EmitSegId( seg_id seg ) {
/****************************************
    Plop a seg_id into Inst[]
*/

    *(seg_id *)(Inst + ICur) = seg;
    ICur += sizeof( seg_id );
}

extern  void    EmitOffset( offset i ) {
/***************************************
    Plop an "offset" int Inst[] (a machine word)
*/

    *(offset *)(Inst + ICur) = i;
    ICur += sizeof( offset );
}

extern  void    EjectInst( void ) {
/**********************************
    Dump the current instruction into the peephole optimizer
*/

    TransferIns();
    ICur = 0;
    ILen = 0;
    IEsc = 0;
}

static  void    TransferIns( void ) {
/************************************
    Transfer an instruction from Inst[] to Temp
*/

    int i;
    int j;

    i = 0;
    j = Temp.oc.reclen - sizeof( oc_header );
    while( i < IEsc ) {
        if( Inst[i] == ESC ) {
            Temp.data[j++] = ESC;
            Temp.oc.reclen++;
        }
        Temp.data[j++] = Inst[i++];
    }
    Copy( &Inst[i], &Temp.data[j], ICur - i );
    Temp.oc.reclen += ICur;
    Temp.oc.objlen += ILen;
}

extern  void    Finalize( void ) {
/*********************************
    Invoked by macro _Emit. Spits Temp into the peephole optimizer
*/

    EjectInst();
    if( FPPatchType != FPP_NONE ) {
        DoFunnyRef( FPPatchType );
        FPPatchType = FPP_NONE;
    }
    if( Temp.oc.objlen != 0 ) {
        InputOC( (any_oc *)&Temp.oc );
    }
}


static  void    LayInitial( instruction *ins, gentype gen ) {
/************************************************************
    Do some really magical jiggery pokery based on "gen" to get the
    right opcode int Inst[].  See PCCodeTable if you want, but this is
    not for the faint of heart!
*/

    int         index;
    pccode_def  *table;

    table = PCCodeTable;
    for(;;) {
        if( gen < table->low_gen )
            break;
        if( table->width == 0 )
            return;
        ++ table;
    }
    -- table;
    index = 0;
    for(;;) {
        if( table->opcode_list[index] == ins->head.opcode )
            break;
        if( table->opcode_list[index] == OP_NOP )
            break;
        ++ index;
    }
    index = index * table->width + gen - table->low_gen;
    if( table->flags & NEED_WAIT ) {
        Used87 = TRUE;
#if !( _TARGET & _TARG_80386 )
        if( gen == G_FINIT || !_CPULevel( CPU_286 ) || _IsEmulation() ) {
            if( _IsEmulation() ) {
                FPPatchType = FPP_NORMAL;
            }
            LayOpbyte( 0x9b );
            _Next;
        }
#endif
    }
    if( table->flags & BYTE_OPCODE ) {
        LayOpbyte( table->opcode_table[index] );
    } else {
        LayOpword( table->opcode_table[index] );
    }
    if( table->flags & BYTE_WORD ) {
        LayW( ins->type_class );
    }
    if( table->flags & SIGN_UNSIGN ) {
        if( ins->type_class == I1
         || ins->type_class == I2
         || ins->type_class == I4 ) {
            if( ins->head.opcode >= OP_MUL && ins->head.opcode <= OP_MOD ) {
                Inst[RMR] |= B_RMR_MUL_SGN;
            } else if( ins->head.opcode == OP_RSHIFT ) {
                Inst[RMR] |= B_RMR_SHR_SAR;
            }
        }
    }
}

static  byte    SegTrans( hw_reg_set regs ) {
/********************************************
    Return the encoding of a segment register name
*/

    int i;

    HW_COnlyOn( regs, HW_SEGS );
    i = 0;
    while( i < SEGS ) {
        if( HW_Equal( regs, SegTab[i] ) )
            break;
        i++;
    }
    if( i >= SEGS ) {
        _Zoiks( ZOIKS_032 );
    }
    return( i );
}

static  byte    RegTrans( hw_reg_set regs ) {
/********************************************
    Return the encoding of a register name
*/

    int i;

    HW_CTurnOff( regs, HW_SEGS );
    i = 0;
    while( i < REGS ) {
        if( HW_Equal( regs, RegTab[i] ) )
            break;
        i++;
    }
    if( i >= REGS ) {
        _Zoiks( ZOIKS_031 );
    }
    i = i / 3;
    return( i );
}

#if _TARGET & _TARG_80386
static  bool    NeedOpndSize( instruction *ins ) {
/*************************************************
        Do we REALLY, REALLY need the operand size override.
*/
    int         i;
    hw_reg_set  result_reg;
    hw_reg_set  full_reg;
    instruction *next;

    switch( ins->head.opcode ) {
    case OP_MOD:
    case OP_DIV:
    case OP_RSHIFT:
        return( TRUE );
    default:
        break;
    }
    if( _OpIsCondition( ins->head.opcode ) )
        return( TRUE );
    if( ins->ins_flags & INS_CC_USED )
        return( TRUE );
    if( ins->result == NULL )
        return( TRUE );
    switch( ins->result->n.class ) {
    case N_TEMP:
        /* it's OK to store a DWORD into a WORD temp because there's always
           two bytes of slack on the stack */
        if( ins->result->t.alias != ins->result )
            return( TRUE );
        break;
    case N_REGISTER:
        /* check that it's OK to trash high word of register */
        next = ins->head.next;
        if( next == NULL )
            return( TRUE );
        result_reg = ins->result->r.reg;
        full_reg = FullReg( result_reg );
        HW_TurnOff( full_reg, result_reg );
        if( HW_Ovlap( full_reg, next->head.live.regs ) )
            return( TRUE );
        break;
    default:
        return( TRUE );
    }
    for( i = ins->num_operands-1; i >= 0; --i ) {
        /* Could be smarter here and allow N_MEMORY,N_INDEXED if
           they're in DGROUP */
        switch( ins->operands[i]->n.class ) {
        case N_TEMP:
        case N_REGISTER:
        case N_CONSTANT:
            break;
        default:
            return( TRUE );
        }
    }
    ins->type_class = U4; /* get correct size of constants */
    return( FALSE );
}
#endif


static  bool    LayOpndSize( instruction *ins, gentype gen ) {
/*************************************************************
    Generate an operand size prefix if the instruction needs it
    (Eg: MOV    AX,DX need it in a USE32 segment)
*/

#if _TARGET & _TARG_80386
    switch( ins->head.opcode ) {
    case OP_CONVERT: /* these ones handle themselves */
    case OP_CALL:
    case OP_CALL_INDIRECT:
    case OP_SELECT:
        return( FALSE );
        break;
    default:
        switch( gen ) {
#if _TARGET & _TARG_IAPX86
        case G_POW2DIV_286:
#endif
        case G_SR:
        case G_RS:
        case G_MS1:
        case G_SM1:
        case G_SEG_SEG:
        case G_SEGR1:
        case G_POW2DIV:
        case G_DIV2:
            return( FALSE );
        default:
            break;
        }
        if( _IsTargetModel( USE_32 ) ) {
            if( ins->type_class == U2 || ins->type_class == I2 ) {
                if( NeedOpndSize( ins ) ) {
                    AddToTemp( M_OPND_SIZE );
                    return( TRUE );
                }
            }
        } else {
            if( ins->type_class == U4
             || ins->type_class == I4
             || ins->type_class == FS ) {
                AddToTemp( M_OPND_SIZE );
                return( TRUE );
            }
        }
        return( FALSE );
    }
#else
    gen = gen;
    switch( ins->head.opcode ) {
    case OP_PUSH:
        if( ( gen == G_C1 || gen == G_M1 ) && ins->type_class == I4 ) {
            AddToTemp( M_OPND_SIZE );
            return( TRUE );
        }
        break;
    case OP_MOV:
        if( ins->type_class == U4 || ins->type_class == I4 ) {
            if( ( gen == G_MOVMC ) ) {
                AddToTemp( M_OPND_SIZE );
                return( TRUE );
            }
        }
        break;
    default:
        break;
    }
    return( FALSE );
#endif
}

#if 1
static  void    DoP5RegisterDivide( instruction *ins ) {
/******************************************************/

    int                 i;
    byte                reverse;
    byte                pop;
    byte                dest;
    int                 ins_key;
    label_handle        lbl;
    label_handle        lbl_2;
    oc_jcond            temp;

    lbl = AskForNewLabel();
    lbl_2 = AskForNewLabel();

    temp.op.class = OC_JCOND;
    temp.op.class |= ATTR_SHORT;
    temp.op.objlen = OptInsSize( OC_JCOND, OC_DEST_NEAR );
    temp.op.reclen = sizeof( oc_jcond );
    temp.cond = 4;
    temp.handle = lbl;
    InputOC( (any_oc *)&temp );
    i = FPRegTrans( ins->operands[0]->r.reg );
    reverse = FALSE;
    pop = FALSE;
    dest = FALSE;
    switch( ins->u.gen_table->generate ) {
    case G_RRFBIN:
        reverse = TRUE;
        break;
    case G_RNFBIN:
        break;
    case G_RRFBINP:
        reverse = TRUE;
        pop = TRUE;
        dest = TRUE;
        break;
    case G_RNFBINP:
        pop = TRUE;
        dest = TRUE;
        break;
    case G_RRFBIND:
        reverse = TRUE;
        dest = TRUE;
        break;
    case G_RNFBIND:
        dest = TRUE;
        break;
    }
    ins_key = ( pop & 1 ) | ( ( reverse & 1 ) << 1 ) | ( ( dest & 1 ) << 2 );
    ins_key |= ( i & 0x07 ) << 3;
    // je ok
    _Code;
    _Next;
    // push [e]ax
    LayOpbyte( 0x50 );
    _Next;
    // mov #cons -> [e]ax
    LayOpbyte( 0xb8 );
#if _TARGET & _TARG_80386
    AddWData( ins_key, U4 );
#else
    AddWData( ins_key, U2 );
#endif
    _Emit;
    // call __fdiv_fpr
    RTCall( RT_FDIV_FPREG, 0 );
    _Code;
    // pop [e]ax
    LayOpbyte( 0x58 );
    _Emit;
    GenJumpLabel( lbl_2 );
    CodeLabel( lbl, 0 );
    _Code;
    LayInitial( ins, ins->u.gen_table->generate );
    LayOpndSize( ins, ins->u.gen_table->generate );
    LayST( ins->operands[0] );
    _Emit;
    CodeLabel( lbl_2, 0 );
    GenKillLabel( lbl );
    GenKillLabel( lbl_2 );
}

#if _TARGET & _TARG_80386
    extern type_length StackDepth;
#endif

static  void    DoP5MemoryDivide( instruction *ins ) {
/****************************************************/

    oc_jcond            temp;
    int                 rtindex;
    label_handle        lbl;
    label_handle        lbl_2;
    name                *high;
    name                *low;
#if !(_TARGET & _TARG_80386)
    name                *h;
    name                *l;
#endif
    name                *seg;

    lbl = AskForNewLabel();
    lbl_2 = AskForNewLabel();

    temp.op.class = OC_JCOND;
    temp.op.class |= ATTR_SHORT;
    temp.op.objlen = OptInsSize( OC_JCOND, OC_DEST_NEAR );
    temp.op.reclen = sizeof( oc_jcond );
    temp.cond = 4;
    temp.handle = lbl;
    InputOC( (any_oc *)&temp );

    seg = NULL;
    if( ins->num_operands > NumOperands( ins ) ) {
        seg = ins->operands[ins->num_operands - 1];
    }

    _Code;
    switch( ins->operands[0]->n.name_class ) {
    case FS:
#if _TARGET & _TARG_80386
        if( seg != NULL )
            GenSeg( seg->r.reg );
        LayOpword( 0x30ff );
        LayModRM( ins->operands[0] );
#else
        high = HighPart( ins->operands[0], U2 );
        low = LowPart( ins->operands[0], U2 );
        if( seg != NULL )
            GenSeg( seg->r.reg );
        LayOpword( 0x30ff );
        LayModRM( high );
        _Next;
        if( seg != NULL )
            GenSeg( seg->r.reg );
        LayOpword( 0x30ff );
        LayModRM( low );
#endif
        if( ins->u.gen_table->generate == G_MRFBIN ) {
            rtindex = RT_FDIV_MEM32R;
        } else {
            rtindex = RT_FDIV_MEM32;
        }
        break;
    case FD:
        high = HighPart( ins->operands[0], U4 );
        low = LowPart( ins->operands[0], U4 );
#if _TARGET & _TARG_80386
        if( seg != NULL )
            GenSeg( seg->r.reg );
        LayOpword( 0x30ff );
        LayModRM( high );
        _Next;
        StackDepth += WORD_SIZE;
        if( seg != NULL )
            GenSeg( seg->r.reg );
        LayOpword( 0x30ff );
        LayModRM( low );
        StackDepth -= WORD_SIZE;
#else
        h = HighPart( high, U2 );
        l = LowPart( high, U2 );
        if( seg != NULL )
            GenSeg( seg->r.reg );
        LayOpword( 0x30ff );
        LayModRM( h );
        _Next;
        if( seg != NULL )
            GenSeg( seg->r.reg );
        LayOpword( 0x30ff );
        LayModRM( l );
        _Next;
        h = HighPart( low, U2 );
        l = LowPart( low, U2 );
        if( seg != NULL )
            GenSeg( seg->r.reg );
        LayOpword( 0x30ff );
        LayModRM( h );
        _Next;
        if( seg != NULL )
            GenSeg( seg->r.reg );
        LayOpword( 0x30ff );
        LayModRM( l );
        _Next;
#endif
        if( ins->u.gen_table->generate == G_MRFBIN ) {
            rtindex = RT_FDIV_MEM64R;
        } else {
            rtindex = RT_FDIV_MEM64;
        }
        break;
    default:
        rtindex = RT_FDIV_MEM64;
        _Zoiks( ZOIKS_114 );
    }
    _Emit;
    RTCall( rtindex, 0 );
    GenJumpLabel( lbl_2 );
    CodeLabel( lbl, 0 );
    _Code;
    if( seg != NULL ) {
        GenSeg( seg->r.reg );
    }
    LayInitial( ins, ins->u.gen_table->generate );
    LayOpndSize( ins, ins->u.gen_table->generate );
    LayMF( ins->operands[0] );
    LayModRM( ins->operands[0] );
    _Emit;
    CodeLabel( lbl_2, 0 );
    GenKillLabel( lbl );
    GenKillLabel( lbl_2 );
}

static  void    DoP5Divide( instruction *ins ) {
/***********************************************
    Emit a most disgusting sequence of code to
    test for the Pentium FDIV bug and call a
    runtime routine to correct it.
*/
    bool        used_ds = FALSE;

    _Code;
    LayOpbyte( 0x9c );  /* pushf */
    _Emit;
#if _TARGET & _TARG_80386
    StackDepth += WORD_SIZE;
#endif
    _Code;
    if( _IsTargetModel( FLOATING_DS ) ) {
        if( _IsTargetModel( FLOATING_SS ) ) {
            LayOpbyte( 0x1e );  // push ds
            AddByte( 0x50 );    // push ax
            _Emit;
            GenLoadDS();
            _Code;
            used_ds = TRUE;
        } else {
            AddToTemp( 0x36 );
        }
    }
#if ( _TARGET & _TARG_80386 )
    LayOpword( 0x05f6 );        // test byte ptr L1,1
#else
    LayOpword( 0x06f6 );        // test byte ptr L1,1
#endif
    ILen += WORD_SIZE;
    DoLblRef( RTLabel( RT_BUGLIST - BEG_RTNS ), AskBackSeg(), 0, OFST );
    AddWData( 1, U1 );
    _Emit;
    if( used_ds ) {
        _Code;
        LayOpbyte( 0x58 );      // pop ax
        AddByte( 0x1f );        // pop ds
        _Emit;
    }
    switch( ins->u.gen_table->generate ) {
    case G_RRFBIN:
    case G_RNFBIN:
    case G_RRFBIND:
    case G_RNFBIND:
    case G_RRFBINP:
    case G_RNFBINP:
        // assuming G_RXFBINZ means
        //      X -> R (reverse) and N (normal)
        //      Z -> D (destination is argument)
        //        -> P (argument is popped)
        DoP5RegisterDivide( ins );
        break;
    case G_MRFBIN:
    case G_MNFBIN:
        DoP5MemoryDivide( ins );
    }
    _Code;
    LayOpbyte( 0x9d );  /* popf */
    _Emit;
#if _TARGET & _TARG_80386
    StackDepth -= WORD_SIZE;
#endif
}
#endif

extern  void    GenObjCode( instruction *ins ) {
/***********************************************
    Generate object code for the instruction "ins" based on gen_table->generate
*/

    gentype     gen;
    name        *result;
    name        *left = NULL;
    name        *right = NULL;
    int         i;
    bool        opnd_size;

    gen = ins->u.gen_table->generate;
    if( gen != G_NO ) {
#if 1
        // fixme - should be _IsTargetModel
        if( _IsTargetModel( P5_DIVIDE_CHECK ) ) {
            if( ins->head.opcode == OP_DIV && _IsFloating( ins->type_class ) ) {
                DoP5Divide( ins );
                return;
            }
        }
#endif
        result = ins->result;
        if( ins->num_operands != 0 ) {
            left = ins->operands[0];
            if( ins->num_operands != 1 ) {
                right = ins->operands[1];
            }
        }

        if (gen == G_MFSTRND)
        {
            /*
            68 3F 0C 00 00            push        0x00000c3f
            D9 7C 24 02               fnstcw      word ptr 0x2[esp]
            D9 2C 24                  fldcw       word ptr [esp]
            */

            _Code;
            AddByte(0x68);
            AddByte(0x3f);
            AddByte(0x0c);
            AddByte(0x00);
            AddByte(0x00);
            _Emit;

            _Code;
            AddByte(0xd9);
            AddByte(0x7c);
            AddByte(0x24);
            AddByte(0x02);
            _Emit;

            _Code;
            AddByte(0xd9);
            AddByte(0x2c);
            AddByte(0x24);
            _Emit;
        }

        _Code;
        LayInitial( ins, gen );
        i = ins->num_operands;
        --i;
        if( i >= NumOperands( ins ) ) {
            if( ins->operands[i]->n.class == N_REGISTER ) {
                GenSeg( ins->operands[i]->r.reg );
            } else {
                _Zoiks( ZOIKS_027 );
            }
        }
        opnd_size = LayOpndSize( ins, gen );
        switch( gen ) {
        case G_RC:
            LayRMRegOp( left );
            AddSWCons( ins->head.opcode, right, ins->type_class );
            break;
        case G_AC:
            ICur--;
            ILen--;
            IEsc--;
            AddWCons( right, ins->type_class );
            break;
        case G_MC:
            LayModRM( left );
            AddSWCons( ins->head.opcode, right, ins->type_class );
            break;
        case G_RR2:
            LayRegOp( right );
            LayRMRegOp( left );
            break;
        case G_0FRR2:
            LayOpword( M_386MUL );
            LayRegOp( left );
            LayRMRegOp( right );
            AddToTemp( M_SECONDARY );
            break;
        case G_0FRM2:
            LayOpword( M_386MUL );
            LayModRM( right );
            LayRegOp( left );
            AddToTemp( M_SECONDARY );
            break;
        case G_PUSHFS:
            LayOpbyte( M_PUSHI );
            AddSCons( IntEquivalent( left ), U4 );
            break;
        case G_RM2:
            LayModRM( right );
            LayRegOp( left );
            break;
        case G_MR2:
            LayModRM( left );
            LayRegOp( right );
            break;
        case G_WORDR1:
            if( ins->head.opcode == OP_POP ) {
                LayACRegOp( result );
                break;
            }
            LayACRegOp( left );
            if( NumOperands( ins ) != 1 && right->c.int_value == 2 ) {
                /* never address*/
                TransferIns();
                if( opnd_size ) {
                    AddToTemp( M_OPND_SIZE );
                }
            }
            break;
        case G_R1:
            LayRMRegOp( left );
            if( ins->num_operands != 1 && right->c.int_value == 2 ) {
                TransferIns();
                if( opnd_size ) {
                    AddToTemp( M_OPND_SIZE );
                }
            }
            break;
        case G_R1SHIFT:
        case G_RCLSHIFT:
            LayRMRegOp( left );
            break;
        case G_M1:
        case G_1SHIFT:
        case G_CLSHIFT:
            LayModRM( left );
            break;
        case G_RNSHIFT:
            LayRMRegOp( left );
            AddByte( right->c.int_value ); /* never address*/
            break;
        case G_NSHIFT:
            LayModRM( left );
            AddByte( right->c.int_value ); /* never address*/
            break;
        case G_R2:
            LayRMRegOp( right );
            break;
        case G_M2:
            LayModRM( right );
            break;
        case G_RR1:
            LayRegOp( left );
            LayRMRegOp( result );
            break;
        case G_RM1:
            LayRegOp( result );
            LayModRM( left );
            break;
        case G_MR1:
            LayRegOp( left );
            LayModRM( result );
            break;
        case G_LEA:
            LayRegOp( result );
            if( left->n.class == N_REGISTER ) {  /* add/sub transformed to LEA*/
                LayLeaRegOp( ins );
            } else {
                LayModRM( left );
            }
            break;
        case G_LDSES:
            LayRegOp( result );
            LayModRM( left );
            if( HW_COvlap( result->r.reg, HW_FS_GS ) ) {
                Inst[KEY] -= B_KEY_FS;      /* load ES or FS */
                AddToTemp( M_SECONDARY );
            }
            if( HW_COvlap( result->r.reg, HW_DS_GS ) ) {
                Inst[KEY] |= B_KEY_DS;      /* indicate load to DS GS*/
            }
            break;
        case G_MS1:
            LaySROp( left );
            LayModRM( result );
            break;
        case G_SM1:
            LaySROp( result );
            LayModRM( left );
            break;
        case G_RS:
            LaySROp( left );
            LayRMRegOp( result );
            break;
        case G_SR:
            LaySROp( result );
            LayRMRegOp( left );
            break;
        case G_MOVMC:
            LayW( ins->type_class );
            LayModRM( result );
            AddWCons( left, ins->type_class );
            break;
        case G_MADDR:
            LayModRM( result );
            DoMAddr( left );
            break;
        case G_TEST:
            LayW( ins->type_class );
            LayRegOp( left );
            LayRMRegOp( left );
            break;
        case G_SEGR1:
            {
                uint    ins_loc;
                byte    extra_bits;
                byte    reg_index;

                ins_loc = KEY;
                if( ins->head.opcode == OP_POP ) {
                    extra_bits = B_KEY_POPSEG;
                    reg_index = SegTrans( result->r.reg );
                } else {
                    extra_bits = 0;
                    reg_index = SegTrans( left->r.reg );
                }
                if( reg_index > 3 ) {
                    ins_loc += 1;
                    Inst[KEY] = 0x0f;
                    AddByte( 0x80 );
                }
                Inst[ins_loc] |= extra_bits;
                Inst[ins_loc] |= reg_index << S_KEY_SR;
            }
            break;
        case G_MOVAM:
            LayW( ins->type_class );
            DoMDisp( left, FALSE );
            break;
        case G_MOVMA:
            LayW( ins->type_class );
            DoMDisp( result, FALSE );
            break;
        case G_MOVRC:
            if( ins->type_class == U2 || ins->type_class == I2
             || ins->type_class == U4 || ins->type_class == I4
             || ins->type_class == FS ) {
                Inst[KEY] |= B_KEY_AW;
            }
            LayACRegOp( result );
            AddWCons( left, ins->type_class );
            break;
        case G_RADDR:
            LayACRegOp( result );
            DoMAddr( left );
            break;
        case G_REPOP:
            DoRepOp( ins );
            break;
        case G_LOADADDR:
            if( AskIsFrameIndex( left ) ) {
                GenUnkLea( NextFramePatch() );
            } else {
                /* turn "LEA EAX, 0[ESP]" into "MOV EAX,ESP" */
#if _TARGET & _TARG_80386
                if((left->n.class==N_TEMP) && TmpLoc(DeAlias(left),left)==0) {
                    LayOpword( M_MOVRR | B_KEY_W );
                    if( BaseIsSP( left ) ) {
                        LayReg( HW_SP );
                    } else {
                        LayReg( HW_BP );
                    }
                    LayRMRegOp( result );
                } else
#endif
                {
                    LayOpword( M_LEA );
                    LayRegOp( result );
                    LayModRM( left );
                }
            }
            break;
        case G_4SHIFT:
            Do4Shift( ins );
            break;
        case G_4RSHIFT:
            Do4RShift( ins );
            break;
        case G_CXSHIFT:
            Do4CXShift( ins, &Do4Shift );
            break;
        case G_RCXSHIFT:
            Do4CXShift( ins, &Do4RShift );
            break;
        case G_4NEG:
            Gen4Neg( ins );
            break;
        case G_4RNEG:
            Gen4RNeg( ins );
            break;
        case G_ENTER:
            /*   do nothing, prolog should already be generated*/
            break;
        case G_CALL:
            GenCall( ins );
            AdjustStackDepth( ins );
            if( _IsTargetModel( NEW_P5_PROFILING ) ) {
                label_handle lbl;
                segment_id seg;

                seg = GenP5ProfileData( "", &lbl, &CurrProc->targ.routine_profile_data );
                _Code;
                LayOpword( 0xc4f7 ); // test esp, offset L1
                ILen += WORD_SIZE;
                DoLblRef( lbl, seg, 0, OFST );
                _Emit;
            }
            return;
        case G_ICALL:
            if( ins->operands[CALL_OP_ADDR]->n.class == N_REGISTER ) {
                GenRCall( ins );
            } else {
                GenICall( ins );
            }
            AdjustStackDepth( ins );
            if( _IsTargetModel( NEW_P5_PROFILING ) ) {
                label_handle lbl;
                label_handle junk;
                segment_id seg;
                char    c[2];

                seg = GenP5ProfileData( "", &lbl, &CurrProc->targ.routine_profile_data );
                GenP5ProfileData( "", &junk, &CurrProc->targ.routine_profile_data );
                c[0] = PROFILE_FLAG_END_GROUP;
                c[1] = 0;
                GenP5ProfileData( c, &junk, &CurrProc->targ.routine_profile_data );
                _Code;
                LayOpword( 0xc5f7 ); // test ebp, offset L1
                ILen += WORD_SIZE;
                DoLblRef( lbl, seg, 0, OFST );
                _Emit;
            }
            return;
        case G_RJMP:
            GenRJmp( ins );
            AdjustStackDepth( ins );
            return;
        case G_MJMP:
            GenMJmp( ins );
            break;
        case G_SIGNEX:
            switch( ins->type_class ) {
#if _TARGET & _TARG_80386
            case U8:
            case I8:
                LayOpbyte( M_CWD );
                if( _IsntTargetModel( USE_32 ) )
                    AddToTemp( M_OPND_SIZE );
                break;
#endif
            case U4:
            case I4:
#if _TARGET & _TARG_IAPX86
                LayOpbyte( M_CWD );
#elif _TARGET & _TARG_80386
                switch( ins->base_type_class ) {
                case U1:
                case I1:
                    if( _IsTargetModel( USE_32 ) )
                        AddToTemp( M_OPND_SIZE );
                    AddToTemp( M_CBW );
                    break;
                default:
                    break;
                }
                if( HW_CEqual( ins->result->r.reg, HW_DX_AX ) ) {
                    LayOpbyte( M_CWD );
                    if( _IsTargetModel( USE_32 ) ) {
                        AddToTemp( M_OPND_SIZE );
                    }
                } else {
                    LayOpbyte( M_CBW );
                    if( _IsntTargetModel( USE_32 ) ) {
                        AddToTemp( M_OPND_SIZE );/*CWDE*/
                    }
                }
#endif
                break;
            case U2:
            case I2:
                LayOpbyte( M_CBW );
                if( _IsTargetModel( USE_32 ) )
                    AddToTemp( M_OPND_SIZE );
                break;
            default:
                break;
            }
            break;
        case G_C1:
            AddSCons( left, ins->type_class );
            break;
        case G_186RMUL:
            LayOpword( 0xc069 );
            LayRegOp( result );
            LayRMRegOp( left );
            AddSCons( right, ins->type_class );
            break;
        case G_186MUL:
            LayOpword( 0x0069 );
            LayRegOp( result );
            LayModRM( left );
            AddSCons( right, ins->type_class );
            break;
        case G_MRFBIN:
        case G_MNFBIN:
        case G_MFLD:
            LayMF( left );
            LayModRM( left );
            break;
        case G_RFLD:
        case G_RRFBIN:
        case G_RRFBINP:
        case G_RRFBIND:
        case G_RNFBIN:
        case G_RNFBINP:
        case G_RNFBIND:
            LayST( left );
            break;
        case G_RFST:
        case G_RFSTNP:
            LayST( result );
            break;
        case G_MFST:
        case G_MFSTNP:
            LayMF( result );
            LayModRM( result );
            break;

        case G_MFSTRND:
            /* store with rounding */

            AdjustStackDepthDirect( WORD_SIZE );
            LayMF( result );
            LayModRM( result );
            AdjustStackDepthDirect( -WORD_SIZE );
            _Emit;

            /*
            D9 6C 24 02               fldcw       word ptr 0x2[esp]
            8D 64 24 04               lea         esp,0x4[esp]
            */
            _Code;
            AddByte(0xd9);
            AddByte(0x6c);
            AddByte(0x24);
            AddByte(0x02);
            _Emit;

            _Code;
            AddByte(0x8d);
            AddByte(0x64);
            AddByte(0x24);
            AddByte(0x04);
            break;

        case G_FCHS:
        case G_FLD1:
        case G_FLDZ:
        case G_FINIT:
            break;
        case G_IFUNC:
            CallMathFunc( ins );
            break;
        case G_FMATH:
            MathFunc( ins );
            break;
        case G_FSINCOS:
            GCondFwait();
            LayOpword( 0xFBD9 );
            break;
        case G_FCHOP:
            DoCall( RTLabel( RT_CHOP - BEG_RTNS ),
                TRUE, _IsTargetModel( BIG_CODE ), EMPTY );
            break;
        case G_FTST:
        case G_FCOMPP:
            Used87 = TRUE;
            SetCC();
            break;
        case G_FWAIT:
            Used87 = TRUE;
            if( _IsEmulation() ) {
                FPPatchType = FPP_WAIT;
                LayOpword( 0x9b90 );
            } else {
                LayOpbyte( 0x9b );
            }
            break;
        case G_FXCH:
            Used87 = TRUE;
            GCondFwait();
            LayOpword( 0xC8D9 ); /* fxch st(1) */
            LayST( result );
            break;
        case G_MCOMP:
            LayMF( left );
            LayModRM( left );
            SetCC();
            break;
        case G_RCOMP:
            LayST( left );
            SetCC();
            break;
        case G_DEBUG:
            EmitDbgInfo( ins );
            break;
        case G_SEG_SEG:
            PushSeg( left->r.reg );
            _Next;
            PopSeg( result->r.reg );
            break;
        case G_POW2DIV:
            Pow2Div( ins );
            break;
        case G_DIV2:
            By2Div( ins );
            break;
#if _TARGET & _TARG_IAPX86
        case G_POW2DIV_286:
            Pow2Div286( ins );
            break;
#endif
        case G_MOVSX:
        case G_MOVZX:
            if( gen == G_MOVSX ) {
                LayOpword( M_MOVSX );
            } else {
                LayOpword( M_MOVZX );
            }
#if _TARGET & _TARG_80386
            if( ins->operands[0]->n.size == 2 ) {
                Inst[KEY] |= B_KEY_W;
            } else { /* base is byte */
                switch( ins->type_class ) {
                case U2:
                case I2:
                    if( _IsTargetModel( USE_32 ) )
                        AddToTemp( M_OPND_SIZE );
                    break;
                case U4:
                case I4:
                    if( _IsntTargetModel( USE_32 ) )
                        AddToTemp( M_OPND_SIZE );
                    break;
                default:
                    break;
                }
            }
#endif
            LayModRM( left );
            LayRegOp( result );
            AddToTemp( M_SECONDARY );
            break;
        default:
            _Zoiks( ZOIKS_028 );
            break;
        }
        _Emit;
    }
    AdjustStackDepth( ins );
    if( _OpIsCondition( ins->head.opcode ) ) {
        EndBlockProfiling();
        GenCondJump( ins );
    }
}


static  void    SetCC(void) {
/******************************
    Generate code to set the condition codes based on an 8087 FCMP instruction
*/

    if( _IsEmulation() ) {
        _Emit;
        _Code;
        FPPatchType = FPP_NORMAL;
    }
    if( _CPULevel( CPU_386 ) ) {
        if( _IsEmulation() ) {
            AddByte( 0x9b );            /*% FWAIT - needed for FIDRQQ to work*/
        }
        AddByte( 0xdf );            /*% FSTSW  AX*/
        AddByte( 0xe0 );            /*% ..*/
    } else {
        if( !_CPULevel( CPU_386 ) ) {
            AddByte( 0x9b );            /*% FWAIT*/
        }
        if( _CPULevel( CPU_286 ) && !_IsEmulation() ) {
            AddByte( 0xdf );            /*% FSTSW  AX*/
            AddByte( 0xe0 );            /*% ..*/
        } else {
            _Next;                     /*%*/
            LayOpword( 0x38dd );        /*% FSTSW  temp*/
            LayModRM( FPStatWord );    /*%*/
            if( _IsEmulation() ) {
                _Emit;                 /*%*/
                GFwait();              /*% FWAIT*/
                _Code;                 /*%*/
            } else {
                AddByte( 0x9b );        /*% FWAIT*/
                _Next;                 /*%*/
            }
            LayOpword( 0x008a );        /*% MOV    AX,temp*/
            LayReg( HW_AX );           /*%*/
            LayW( U2 );                /*%*/
            LayModRM( FPStatWord );    /*%*/
        }
    }
    AddByte( 0x9e );                /*% SAHF*/
}


/* Lay Routines*/

extern  void    LayOpbyte( opcode op ) {
/***************************************
    Add a one byte opcode to Inst[]
*/

    Inst[KEY] = op & 0xff;
    ICur = 1;
    ILen = 1;
    IEsc = 1;
}

extern  void    LayOpword( opcode op ) {
/***************************************
    Add a 2 byte opcode to Inst[]
*/

    Inst[KEY] = op & 0xff;
    Inst[RMR] = (op >> 8) & 0xff;
    ICur = 2;
    ILen = 2;
    IEsc = 2;
}

extern  void    LayW( type_class_def class ) {
/********************************************/

    switch( class ) {
    case U2: case I2: case U4: case I4: case FS:
        Inst[KEY] |= B_KEY_W;       /* turn on the W bit*/
        break;
    default:
        break;
    }
}

static  void    LayRegOp( name *r ) {
/************************************
    Add the register op to the instruction
*/

    Inst[RMR] |= RegTrans( r->r.reg ) << S_RMR_REG;
}

extern  void    LayReg( hw_reg_set r ) {
/***************************************
    Add the register op to the instruction
*/

    Inst[RMR] |= RegTrans( r ) << S_RMR_REG;
}

extern  void    LayRMRegOp( name *r ) {
/**************************************
    Add the register op to a MOD/RM instruction
*/

    Inst[RMR] |= ( RegTrans( r->r.reg ) << S_RMR_RM ) + RMR_MOD_REG;
}

extern  void    LayRegRM( hw_reg_set r ) {
/**************************************
    Add the register op to a MOD/RM instruction
*/

    Inst[RMR] |= ( RegTrans( r ) << S_RMR_RM ) + RMR_MOD_REG;
}

static  void    LayACRegOp( name *r ) {
/*************************************
    Add the other register op to a REG/AX (Accumulator) operation
*/

    Inst[KEY] |= RegTrans( r->r.reg ) << S_KEY_REG;
}

extern  void    LayRegAC( hw_reg_set r ) {
/*****************************************
    Add the other register op to a REG/AX (Accumulator) operation
*/

    Inst[KEY] |= RegTrans( r ) << S_KEY_REG;
}

static  void    LaySROp( name *r ) {
/***********************************
    Add a segment register op
*/

    Inst[RMR] |= SegTrans( r->r.reg ) << S_RMR_SR;
}

static  int     FPRegTrans( hw_reg_set reg ) {
/********************************************/

    int         i;
    hw_reg_set  *table;

    table = FPRegs;
    i = 0;
    while( !HW_Equal( reg, *table ) ) {
        table++;
        i++;
    }
    return( i );
}

static  void    LayST( name *op ) {
/**********************************
    add the ST(i) operand to a floating point instruction
*/

    Inst[RMR] |= FPRegTrans( op->r.reg );
}


static  void    LayMF( name *op ) {
/**********************************
    For a floating point instruction, add the qword ptr stuff
*/

    if( op->n.class != N_CONSTANT ) {
        switch( op->n.name_class ) {
        case FS:
            Inst[KEY] |= MF_FS;
            break;
        case FD:
            Inst[KEY] |= MF_FD;
            break;
        case FL:
            Inst[KEY] |= MF_FL;
            Inst[RMR] |= B_RMR_FMT_FL;
            break;
        case U2:
        case I2:
            Inst[KEY] |= MF_I2;
            break;
        case U4:
        case I4:
            Inst[KEY] |= MF_I4;
            break;
        case U8:
        case I8:
        case XX:
            Inst[KEY] |= MF_I8;
            Inst[RMR] |= B_RMR_FMT_I8;
            break;
        default:
            _Zoiks( ZOIKS_029 );
            break;
        }
    }
}

extern  void    GenSeg( hw_reg_set regs ) {
/******************************************
    Generate a segment override if we need one. regs if the full address.
    For example, if regs is DS:BP, we need an override. SS:BP wouldn't
*/

    hw_reg_set  segreg;
    int         i;

    segreg = regs;
    HW_COnlyOn( segreg, HW_SEGS );
    if( HW_CEqual( segreg, HW_EMPTY ) )
        return;
    if( HW_COvlap( regs, HW_BP ) ) {
        if( HW_CEqual( segreg, HW_SS ) )
            return;
        if( HW_CEqual( segreg, HW_DS )
         && _IsntTargetModel(FLOATING_DS|FLOATING_SS) ) {
            return;
        }
    } else {
        if( HW_CEqual( segreg, HW_DS ) ) {
            return;
        }
    }
    /* produce segment override prefix*/
    if( HW_COvlap( regs, HW_FS ) ) {
        AddToTemp( M_SEGFS );
    } else if ( HW_COvlap( regs, HW_GS ) ) {
        AddToTemp( M_SEGGS );
    } else {
        AddToTemp( M_SEGOVER | ( SegTrans( regs ) << S_KEY_SR ) );
    }
    if( _IsEmulation() ) {
        i = 0;
        while( i < SEGS ) {
            if( HW_Equal( segreg, SegTab[i] ) )
                break;
            i++;
        }
        if( FPPatchType != FPP_NONE ) {
            FPPatchType = SegPatchTab[i];
        }
    }
}

extern  type_class_def  OpndSize( hw_reg_set reg ) {
/***************************************************
    Generate an operand size prefix if we need it and return the
    type_class of the register "reg"
*/

    if( HW_COvlap( reg, HW_SEGS ) )
        return( U2 );
#if _TARGET & _TARG_80386
    if( HW_COvlap( reg, HW_32_BP_SP ) ) {
        if( _IsntTargetModel( USE_32 ) )
            AddToTemp( M_OPND_SIZE );
        return( U4 );
    } else {
        if( _IsTargetModel( USE_32 ) )
            AddToTemp( M_OPND_SIZE );
        return( U2 );
    }
#else
    if( _IsTargetModel( USE_32 ) )
        AddToTemp( M_OPND_SIZE );
    return( U2 );
#endif
}


extern  void    QuickSave( hw_reg_set reg, opcode_defs op ) {
/************************************************************
    PUSH/POP    reg     - based on "op"
*/

/* assume register is valid for PUSH/POP*/

    _Code;
    if( HW_COvlap( reg, HW_SEGS ) ) {
        if( op == OP_PUSH ) {
            PushSeg( reg );
        } else {
            PopSeg( reg );
        }
    } else {
        if( op == OP_PUSH ) {
            LayOpbyte( M_PUSH | ( RegTrans( reg ) << S_KEY_REG ) );
        } else {
            LayOpbyte( M_POP | ( RegTrans( reg ) << S_KEY_REG ) );
        }
        OpndSize( reg );
    }
    _Emit;
}


extern  void    GenRegXor( hw_reg_set src, hw_reg_set dst ) {
/************************************************************
    XOR         dst,src
*/

    _Code;
    LayOpword( M_XORRR | B_KEY_W );
    OpndSize( src );
    LayReg( src );
    LayRegRM( dst );
    _Emit;
}


extern  void    GenRegNeg( hw_reg_set src ) {
/********************************************
    NEG         src
*/

    _Code;
    LayOpword( M_NEGR | B_KEY_W );
    OpndSize( src );
    LayRegRM( src );
    _Emit;
}


extern  void    GenRegMove( hw_reg_set src, hw_reg_set dst ) {
/*************************************************************
    MOV         dst,src
*/

    _Code;
    LayOpword( M_MOVRR | B_KEY_W );
    OpndSize( src );
    LayReg( src );
    LayRegRM( dst );
    _Emit;
}


static  void    AddSWData( opcode_defs op, signed_32 val,
                           type_class_def class ) {
/**************************************************
    Add a const (signed_32) to Inst[] with sign extension if the opcode
    allows it
*/

    switch( op ) {
    case OP_ADD:
    case OP_EXT_ADD:
    case OP_SUB:
    case OP_EXT_SUB:
    case OP_AND:
    case OP_OR:
    case OP_XOR:
    case OP_MUL:
    case OP_CMP_EQUAL:
    case OP_CMP_NOT_EQUAL:
    case OP_CMP_GREATER:
    case OP_CMP_LESS_EQUAL:
    case OP_CMP_LESS:
    case OP_CMP_GREATER_EQUAL:
        AddSData( val, class );
        break;
    default:
        AddWData( val, class );
        break;
    }
}


static  void    AddSWCons( opcode_defs op, name *opnd, type_class_def class ) {
/******************************************************************************
    Add a const (name *) to Inst[] with sign extension if the opcode allows it
*/

    if( opnd->c.const_type == CONS_ABSOLUTE ) {
        AddSWData( op, opnd->c.int_value, class );
    } else {
        switch( class ) {
        case U2:
        case I2:
        case U4:
        case I4:
            DoRelocConst( opnd, class );
            break;
        default:
            Zoiks( ZOIKS_045 );
            break;
        }
    }
}


extern  void    AddWData( signed_32 value, type_class_def kind ) {
/****************************************************************/

    AddByte( value );
    if( kind == U1 || kind == I1 )
        return;
    value >>= 8;
    AddByte( value );
    if( kind == U2 || kind == I2 )
        return;
    value >>= 8;
    AddByte( value );
    value >>= 8;
    AddByte( value );
}

extern  void    AddWCons( name *op, type_class_def kind ) {
/**********************************************************
    Add a WORD constant to Inst[]
*/

    if( op->c.const_type == CONS_ABSOLUTE ) {
        AddWData( op->c.int_value, kind );
    } else  {
        switch( kind ) {
        case U2:
        case I2:
        case U4:
        case I4:
            DoRelocConst( op, kind );
            break;
        default:
            Zoiks( ZOIKS_045 );
            break;
        }
    }
}

extern  void    AddSData( signed_32 value, type_class_def kind ) {
/*****************************************************************
    Add a constant (signed_32) to Inst[], with possible sign extension
*/

    if( ( kind == U2 || kind == I2 )
        && ( ( value & 0xff80 ) == 0xff80
          || ( value & 0xff80 ) == 0 ) ) {
        Inst[KEY] |= B_KEY_S;
        AddByte( _IntToByte( value ) );
    } else if( ( kind == U4 || kind == I4 )
        && ( ( value & 0xffffff80 ) == 0xffffff80
          || ( value & 0xffffff80 ) == 0 ) ) {
        Inst[KEY] |= B_KEY_S;
        AddByte( _IntToByte( value ) );
    } else {
        AddWData( value, kind );
    }
}

static  void    AddSCons( name *op, type_class_def kind ) {
/**********************************************************
    Add a constant (name *) to Inst[], with possible sign extension
*/

    if( op->c.const_type == CONS_ABSOLUTE ) {
        AddSData( op->c.int_value, kind );
    } else {
        switch( kind ) {
        case U2:
        case I2:
        case U4:
        case I4:
            DoRelocConst( op, kind );
            break;
        default:
            Zoiks( ZOIKS_045 );
            break;
        }
    }
}


static  void    GenRegOp( hw_reg_set dst, type_length value, unsigned op ) {
/**************************************************************************/

    type_class_def      kind;

    _Code;
    LayOpword( op | B_KEY_W );
    kind = OpndSize( dst );
    LayRegRM( dst );
    AddSData( value, kind );
    _Emit;
}


extern  void    GenRegAdd( hw_reg_set dst, type_length value ) {
/***************************************************************
    ADD dst,value
*/

    GenRegOp( dst, value, M_ADDRC );
}


extern  void    GenRegSub( hw_reg_set dst, type_length value ) {
/***************************************************************
    SUB         dst,value
*/

    GenRegOp( dst, value, M_SUBRC );
}


extern  void    GenRegAnd( hw_reg_set dst, type_length value ) {
/***************************************************************
    SUB         dst,value
*/

    GenRegOp( dst, value, M_ANDRC );
}


extern  void    GFldMorC( name *what ) {
/***************************************
    FLD         what, where what is either 0,1,or a memory location
*/

    if( what->n.class == N_MEMORY || what->n.class == N_TEMP ) {
        GFldM( what );
    } else if( what->c.int_value == 0 ) {
        GFldz();
    } else {
        GFld1();
    }
}

extern  void    GFldM( pointer what ) {
/**************************************
    FLD         tbyte ptr what
*/

    GCondFwait();
    LayOpword( 0x00d9 );
    LayMF( what );
    LayModRM( what );
    _Emit;
}


extern  void    GFstpM( pointer what ) {
/***************************************
    FSTP        tbyte ptr what
*/

    GCondFwait();
    LayOpword( 0x18d9 );
    LayMF( what );
    LayModRM( what );
    _Emit;
}


extern  void    GFstp( int i ) {
/*******************************
    FSTP        ST(i)
*/

    GCondFwait();
    LayOpword( 0xd8dd );
    Inst[RMR] |= i;
    _Emit;
}


extern  void    GFxch( int i ) {
/*******************************
    FXCH        ST(i)
*/

    GCondFwait();
    LayOpword( 0xc8d9 );
    Inst[RMR] |= i;
    _Emit;
}


extern  void    GFldz( void ) {
/******************************
    FLDZ
*/

    GCondFwait();
    LayOpword( 0xeed9 );
    _Emit;
}


static  void    GFld1( void ) {
/******************************
    FLD1
*/

    GCondFwait();
    LayOpword( 0xe8d9 );
    _Emit;
}


extern  void    GFld( int i ) {
/******************************
    FLD         ST(i)
*/

    GCondFwait();
    LayOpword( 0xc0d9 );
    Inst[RMR] |= i;
    _Emit;
}


extern  void    GCondFwait( void ) {
/***********************************
    FWAIT, but only if we need one
*/

    _Code;
    Used87 = TRUE;
#if !( _TARGET & _TARG_80386 )
    if( !_CPULevel( CPU_286 ) || _IsEmulation() ) {
        if( _IsEmulation() ) {
            FPPatchType = FPP_NORMAL;
        }
        LayOpbyte( 0x9b );
        _Next;
    }
#endif
}


extern  void    GFwait( void ) {
/*******************************
    FWAIT
*/

    if( _CPULevel( CPU_386 ) )
        return;
    _Code;
    Used87 = TRUE;
    if( _IsEmulation() ) {
        FPPatchType = FPP_WAIT;
        LayOpword( 0x9b90 );
    } else {
        LayOpbyte( 0x9b );
    }
   _Emit;
}

extern  void    Gpusha( void ) {
/*******************************
    PUSHA{d}
*/

    _Code;
    LayOpbyte( 0x60 );
   _Emit;
}

extern  void    Gpopa( void ) {
/******************************
    POPA{d}
*/

    _Code;
    LayOpbyte( 0x61 );
   _Emit;
}

extern  void    Gcld( void ) {
/*****************************
    CLD
*/

    _Code;
    LayOpbyte( 0xfc );
   _Emit;
}

extern  void    GenLeave( void ) {
/*********************************
    LEAVE
*/

    _Code;
    LayOpbyte( 0xc9 );
    OpndSize( HW_BP );
    _Emit;
}


extern  void    GenTouchStack( bool sp_might_point_at_something ) {
/******************************************************************
    MOV         [esp],eax
*/

#if _TARGET & _TARG_80386
    if( sp_might_point_at_something || OptForSize == 100 ) {
        QuickSave( HW_EAX, OP_PUSH );
        QuickSave( HW_EAX, OP_POP );
    } else {
        _Code;
        LayOpword( 0x0489 );
        OpndSize( HW_SP );
        AddByte( 0x24 );
        _Emit;
    }
#else
    sp_might_point_at_something=sp_might_point_at_something;
#endif
}


extern  void    GenEnter(  int size,  int level  ) {
/***************************************************
    ENTER       size,level
*/

    _Code;
    LayOpbyte( 0xc8 );
    OpndSize( HW_BP );
    AddWData( size, U2 );
    AddByte( level );
    _Emit;
}



extern  void    GenPushOffset( byte offset ) {
/*********************************************
    PUSH        word ptr offset[BP]
*/

    _Code;
    LayOpword( M_PUSHATBP );
    AddWData( offset, I1 );
    _Emit;
}

static  void    PushSeg( hw_reg_set reg ) {
/******************************************
    PUSH segreg
*/

    if( HW_COvlap( reg, HW_FS ) ) {
        LayOpword( M_PUSHFS );
    } else if ( HW_COvlap( reg, HW_GS ) ) {
        LayOpword( M_PUSHGS );
    } else {
        LayOpbyte( M_PUSHSEG | ( SegTrans( reg ) << S_KEY_SR ) );
    }
}

static  void    PopSeg( hw_reg_set reg ) {
/*****************************************
    POP segreg
*/

    if( HW_COvlap( reg, HW_FS ) ) {
        LayOpword( M_POPFS );
    } else if ( HW_COvlap( reg, HW_GS ) ) {
        LayOpword( M_POPGS );
    } else {
        LayOpbyte( M_POPSEG | ( SegTrans( reg ) << S_KEY_SR ) );
    }
}


extern  void    GenUnkSub( hw_reg_set dst, pointer value ) {
/***********************************************************
    SUB         dst,??? - to be patched later
*/

    _Code;
    LayOpword( M_SUBRC | B_KEY_W );
    OpndSize( dst );
    LayRegRM( dst );
    ILen += WORD_SIZE;
    DoAbsPatch( value, WORD_SIZE );
    _Emit;
}


extern  void    GenUnkMov( hw_reg_set dst, pointer value ) {
/***********************************************************
    MOV         dst,??? - to be patched
*/
    dst = dst;
    _Code;
    LayOpbyte( 0xb8 );
    OpndSize( dst );
    ILen += WORD_SIZE;
    DoAbsPatch( value, WORD_SIZE );
    _Emit;
}


extern  void    GenUnkEnter( pointer value, int level ) {
/********************************************************
    ENTER       ??,level - to be patched later
*/

    _Code;
    LayOpbyte( 0xc8 );
    DoAbsPatch( value, 2 );
    ILen += 2;
    AddByte( level );
    _Emit;
}

extern  void    GenWindowsProlog( void ) {
/****************************************/

    _Code;
    if( _IsTargetModel( SMART_WINDOWS ) ) {
        LayOpbyte( 0x8c ); AddByte( 0xd0 ); /*  mov     ax, ss  */
    } else {
        LayOpbyte( 0x1e );              /*      push    ds      */
        AddByte( 0x58 );                /*      pop     ax      */
        AddByte( 0x90 );                /*      nop             */
    }
    if( !_CPULevel( CPU_386 ) ) {
        AddByte( 0x45 );                /*      inc     bp      */
    }
    AddByte( 0x55 );                    /*      push    bp      */
    AddByte( 0x89 ); AddByte( 0xe5 );   /*      mov     bp,sp   */
    AddByte( 0x1e );                    /*      push    ds      */
    AddByte( 0x8e ); AddByte( 0xd8 );   /*      mov     ds,ax   */
    _Emit;
}

extern  void    GenCypWindowsProlog( void ) {
/********************************************
    Generate a "cheap" windows prolog
*/

    _Code;
    if( !_CPULevel( CPU_386 ) ) {
        LayOpbyte( 0x45 );              /*      inc     bp      */
    }
    AddByte( 0x55 );                    /*      push    bp      */
    AddByte( 0x89 ); AddByte( 0xe5 );   /*      mov     bp,sp   */
    _Emit;
}

extern  void    GenWindowsEpilog( void ) {
/****************************************/

    _Code;
    LayOpbyte( 0x1f );                  /*      pop     ds      */
    AddByte( 0x5d );                    /*      pop     bp      */
    if( !_CPULevel( CPU_386 ) ) {
        AddByte( 0x4d );                /*      dec     bp      */
    }
    _Emit;
}

extern  void    GenCypWindowsEpilog( void ) {
/********************************************
    Generate a "cheap" windows epilog
*/

    _Code;
    LayOpbyte( 0x5d );                  /*      pop     bp      */
    if( !_CPULevel( CPU_386 ) ) {
        AddByte( 0x4d );                /*      dec     bp      */
    }
    _Emit;
}


extern  void    GenLoadDS( void )
/*******************************/
{
    _Code;
    LayOpbyte( 0xb8 );                  /*      mov     ax,DGROUP */
    OpndSize( HW_AX );
    ILen += WORD_SIZE;
    DoSegRef( AskBackSeg() );
    AddByte( 0x8e ); AddByte( 0xd8 );   /*      mov     ds,ax   */
    _Emit;
}


static  void    OutputFP( opcode op ) {
/**************************************
*/

    GCondFwait();
    LayOpword( op );
}

static  void    MathFunc( instruction *ins ) {
/*********************************************
    Generate inline code for a math function instruction
*/

    switch( ins->head.opcode ) {
    case OP_EXP:
        OutputFP( 0xE8D9 ); _Emit;      /*   FLD1    */
        OutputFP( 0xEAD9 ); _Emit;      /*   FLDL2E    */
        OutputFP( 0xCAD8 ); _Emit;      /*   FMUL ST,ST(2) */
        OutputFP( 0xD2DD ); _Emit;      /*   FST  ST(2) */
        OutputFP( 0xF8D9 ); _Emit;      /*   FPREM */
        OutputFP( 0xF0D9 ); _Emit;      /*   F2XM1 */
        OutputFP( 0xC1DE ); _Emit;      /*   FADDP ST(1),ST */
        OutputFP( 0xFDD9 ); _Emit;      /*   FSCALE         */
        OutputFP( 0xD9DD );             /*   FSTP ST(1)     */
        break;
    case OP_LOG:
        OutputFP( 0xEDD9 ); _Emit;      /*   FLDLN2    */
        OutputFP( 0xC9D9 ); _Emit;      /*   FXCH    ST(1)    */
        OutputFP( 0xF1D9 );             /*   FYL2X    */
        break;
    case OP_LOG10:
        OutputFP( 0xECD9 ); _Emit;      /*   FLDLG2    */
        OutputFP( 0xC9D9 ); _Emit;      /*   FXCH    ST(1)    */
        OutputFP( 0xF1D9 );             /*   FYL2X    */
        break;
    case OP_COS:
        OutputFP( 0xffD9 );             /*   FCOS    */
        break;
    case OP_SIN:
        OutputFP( 0xfeD9 );             /*   FSIN    */
        break;
    case OP_TAN:
        OutputFP( 0xF2D9 ); _Emit;      /*   FPTAN    */
        OutputFP( 0xD8DD );             /*   FSTP    ST(0)    */
        break;
    case OP_ATAN:
        OutputFP( 0xE8D9 ); _Emit;      /*   FLD1    */
        OutputFP( 0xF3D9 );             /*   FPATAN    */
        break;
    case OP_SQRT:
        OutputFP( 0xFAD9 );             /*   FSQRT   */
        break;
    case OP_FABS:
        OutputFP( 0xE1D9 );             /*   FABS    */
        break;
    default:
        break;
    }
}
static  void    CallMathFunc( instruction *ins ) {
/*************************************************
    Call a runtime routine for a math function instructions
*/
    LookupRoutine( ins );
    DoCall( RTLabel( RoutineNum ),
            TRUE, _IsTargetModel( BIG_CODE ), EMPTY );
}
