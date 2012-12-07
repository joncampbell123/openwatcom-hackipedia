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
* Description:  Generate calls to runtime support routines.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "rtclass.h"
#include "regset.h"
#include "rttable.h"
#include "model.h"
#include "zoiks.h"
#include "conflict.h"
#include "seldef.h"
#include "cgaux.h"


extern    hw_reg_set      *RegSets[];

extern  name            *GenFloat( name *, type_class_def );
extern  name            *GenConstData( byte *, type_class_def );
extern  void            UpdateLive( instruction *, instruction * );
extern  void            DoNothing( instruction * );
extern  name            *AllocIntConst( int );
extern  void            ReplIns( instruction *, instruction * );
extern  void            SuffixIns( instruction *, instruction * );
extern  void            MoveSegRes( instruction *, instruction * );
extern  instruction     *MakeBinary( opcode_defs, name *, name *, name *, type_class_def );
extern  name            *AllocMemory( pointer, type_length, cg_class, type_class_def );
extern  bool            SegIsSS( name * );
extern  name            *GetSegment( name * );
extern  void            DelSeg( instruction * );
extern  instruction     *MakeUnary( opcode_defs, name *, name *, type_class_def );
extern  instruction     *MakeConvert( name *, name *, type_class_def, type_class_def );
extern  void            PrefixIns( instruction *, instruction * );
extern  void            MoveSegOp( instruction *, instruction *, int );
extern  instruction     *MakeMove( name *, name *, type_class_def );
extern  name            *AllocRegName( hw_reg_set );
extern  rt_class        AskHow( type_class_def, type_class_def );
extern  label_handle    AskRTLabel( sym_handle * );
extern  instruction     *NewIns( int );
extern  conflict_node   *NameConflict( instruction *, name * );
extern  conflict_node   *InMemory( conflict_node * );
extern  int             NumOperands( instruction * );
extern  void            AddIns( instruction * );
extern  name            *AllocTemp( type_class_def );
extern  name            *AllocIndex( name *, name *, type_length, type_class_def );
extern  name            *AddrConst( name *, int, constant_class );
extern  seg_id          AskCodeSeg( void );
extern  void            LookupRoutine( instruction * );
extern  label_handle    RTLabel( int );
extern  int             FindRTLabel( label_handle );
extern  hw_reg_set      ReturnReg( type_class_def, bool );

/*
 * If you add a new routine, let John know as the debugger recognizes
 * these.
 */

/*
    OK, so I'm going to go to hell for this, it works don't it?
*/
#define RL_SI   RL_TEMP_INDEX

rtn_info RTInfo[RT_NOP-BEG_RTNS+1] = {
/*  name    op            class   left            right           result*/
{"__U4FS",   OP_CONVERT,   U4,     RL_DX_AX,       RL_,            RL_DX_AX},
{"__I4FS",   OP_CONVERT,   I4,     RL_DX_AX,       RL_,            RL_DX_AX},
{"__U4FD",   OP_CONVERT,   U4,     RL_DX_AX,       RL_,            RL_8},
{"__I4FD",   OP_CONVERT,   I4,     RL_DX_AX,       RL_,            RL_8},
{"__U8FS",   OP_CONVERT,   U8,     RL_8,           RL_,            RL_DX_AX},
{"__I8FS",   OP_CONVERT,   I8,     RL_8,           RL_,            RL_DX_AX},
{"__U8FD",   OP_CONVERT,   U8,     RL_8,           RL_,            RL_8},
{"__I8FD",   OP_CONVERT,   I8,     RL_8,           RL_,            RL_8},
{"__FSFD",   OP_CONVERT,   FS,     RL_DX_AX,       RL_,            RL_8},
{"__FSI4",   OP_CONVERT,   FS,     RL_DX_AX,       RL_,            RL_DX_AX},
{"__RSI4",   OP_ROUND,     FS,     RL_DX_AX,       RL_,            RL_DX_AX},
{"__FSU4",   OP_CONVERT,   FS,     RL_DX_AX,       RL_,            RL_DX_AX},
{"__RSU4",   OP_ROUND,     FS,     RL_DX_AX,       RL_,            RL_DX_AX},
{"__FSI8",   OP_CONVERT,   FS,     RL_DX_AX,       RL_,            RL_8},
{"__RSI8",   OP_ROUND,     FS,     RL_DX_AX,       RL_,            RL_8},
{"__FSU8",   OP_CONVERT,   FS,     RL_DX_AX,       RL_,            RL_8},
{"__RSU8",   OP_ROUND,     FS,     RL_DX_AX,       RL_,            RL_8},
{"__FDI4",   OP_CONVERT,   FD,     RL_8,           RL_,            RL_DX_AX},
{"__RDI4",   OP_ROUND,     FD,     RL_8,           RL_,            RL_DX_AX},
{"__FDU4",   OP_CONVERT,   FD,     RL_8,           RL_,            RL_DX_AX},
{"__RDU4",   OP_ROUND,     FD,     RL_8,           RL_,            RL_DX_AX},
{"__FDI8",   OP_CONVERT,   FD,     RL_8,           RL_,            RL_8},
{"__RDI8",   OP_ROUND,     FD,     RL_8,           RL_,            RL_8},
{"__FDU8",   OP_CONVERT,   FD,     RL_8,           RL_,            RL_8},
{"__RDU8",   OP_ROUND,     FD,     RL_8,           RL_,            RL_8},

{"__U8FS7",  OP_CONVERT,   U8,     RL_8,           RL_,            RL_DX_AX},
{"__U8FD7",  OP_CONVERT,   U8,     RL_8,           RL_,            RL_8},
{"__FSU87",  OP_CONVERT,   FS,     RL_DX_AX,       RL_,            RL_8},
{"__FDU87",  OP_CONVERT,   FD,     RL_8,           RL_,            RL_8},

{"__FDFS",   OP_CONVERT,   FD,     RL_8,           RL_,            RL_DX_AX},
{"__RDFS",   OP_ROUND,     FD,     RL_8,           RL_,            RL_DX_AX},

{"__FSA",    OP_ADD,       FS,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__FSS",    OP_SUB,       FS,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__FSM",    OP_MUL,       FS,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__FSD",    OP_DIV,       FS,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__FSC",    OP_CMP,       FS,     RL_DX_AX,       RL_CX_BX,       RL_},
{"__FSN",    OP_NEGATE,    FS,     RL_DX_AX,       RL_,            RL_DX_AX},
{"__FDA",    OP_ADD,       FD,     RL_8,           RL_8,           RL_8},
{"__EDA",    OP_ADD,       FD,     RL_8,           RL_8,           RL_8},
{"__FDS",    OP_SUB,       FD,     RL_8,           RL_8,           RL_8},
{"__EDS",    OP_SUB,       FD,     RL_8,           RL_8,           RL_8},
{"__FDM",    OP_MUL,       FD,     RL_8,           RL_8,           RL_8},
{"__EDM",    OP_MUL,       FD,     RL_8,           RL_8,           RL_8},
{"__FDD",    OP_DIV,       FD,     RL_8,           RL_8,           RL_8},
{"__EDD",    OP_DIV,       FD,     RL_8,           RL_8,           RL_8},
{"__FDC",    OP_CMP,       FD,     RL_8,           RL_8,           RL_},
{"__EDC",    OP_CMP,       FD,     RL_8,           RL_8,           RL_},
{"__FDN",    OP_NEGATE,    FD,     RL_8,           RL_,            RL_8},
{"__I4M",    OP_MUL,       I4,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__I4D",    OP_DIV,       I4,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__I4D",    OP_MOD,       I4,     RL_DX_AX,       RL_CX_BX,       RL_CX_BX},
{"__I8M",    OP_MUL,       I8,     RL_8,           RL_8,           RL_8},
{"__I8ME",   OP_MUL,       I8,     RL_8,           RL_8,           RL_8},
{"__I8DQ",   OP_DIV,       I8,     RL_8,           RL_8,           RL_8},
{"__I8DQE",  OP_DIV,       I8,     RL_8,           RL_8,           RL_8},
{"__I8DR",   OP_MOD,       I8,     RL_8,           RL_8,           RL_8},
{"__I8DRE",  OP_MOD,       I8,     RL_8,           RL_8,           RL_8},
{"__U4M",    OP_MUL,       U4,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__U4D",    OP_DIV,       U4,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__U4D",    OP_MOD,       U4,     RL_DX_AX,       RL_CX_BX,       RL_CX_BX},
{"__U8M",    OP_MUL,       U8,     RL_8,           RL_8,           RL_8},
{"__U8ME",   OP_MUL,       U8,     RL_8,           RL_8,           RL_8},
{"__U8DQ",   OP_DIV,       U8,     RL_8,           RL_8,           RL_8},
{"__U8DQE",  OP_DIV,       U8,     RL_8,           RL_8,           RL_8},
{"__U8DR",   OP_MOD,       U8,     RL_8,           RL_8,           RL_8},
{"__U8DRE",  OP_MOD,       U8,     RL_8,           RL_8,           RL_8},
{"__I8LS",   OP_LSHIFT,    I8,     RL_8,           RL_SI,          RL_8},
{"__I8RS",   OP_RSHIFT,    I8,     RL_8,           RL_SI,          RL_8},
{"__U8LS",   OP_LSHIFT,    U8,     RL_8,           RL_SI,          RL_8},
{"__U8RS",   OP_RSHIFT,    U8,     RL_8,           RL_SI,          RL_8},
{"__PTS",    OP_SUB,       PT,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__PTC",    OP_CMP,       PT,     RL_DX_AX,       RL_CX_BX,       RL_},
{"__PIS",    OP_SUB,       PT,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__PIA",    OP_ADD,       PT,     RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"__STK",    OP_CALL,       0,     RL_,            RL_,            RL_},
{"__CHP",    OP_CALL,       0,     RL_,            RL_,            RL_},
{"__SCN1",   OP_SELECT,     0,     RL_AL,          RL_,            RL_},
{"__SCN2",   OP_SELECT,     0,     RL_AX,          RL_,            RL_},
{"__SCN4",   OP_SELECT,     0,     RL_DX_AX,       RL_,            RL_},
{"__EPI",    OP_CALL,       0,     RL_,            RL_,            RL_},
{"__PRO",    OP_CALL,       0,     RL_,            RL_,            RL_},

{"IF@DP5DIV",OP_P5DIV,      FD,    RL_8,           RL_,            RL_8},
{"IF@P5DIV", OP_P5DIV,      FS,    RL_DX_AX,       RL_CX_BX,       RL_DX_AX},

{"IF@DPOW",  OP_POW,        FD,    RL_8,           RL_,            RL_8},
{"IF@DPOWI", OP_POW,        FD,    RL_8,           RL_,            RL_8},
{"IF@POW",   OP_POW,        FS,    RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"IF@POWI",  OP_POW,        FS,    RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"IF@IPOW",  OP_POW,        I4,    RL_DX_AX,       RL_CX_BX,       RL_DX_AX},

{"IF@DATAN2",OP_ATAN2,      FD,    RL_8,           RL_,            RL_8},
{"IF@DFMOD", OP_FMOD,       FD,    RL_8,           RL_,            RL_8},
{"IF@DLOG",  OP_LOG,        FD,    RL_8,           RL_,            RL_8},
{"IF@DCOS",  OP_COS,        FD,    RL_8,           RL_,            RL_8},
{"IF@DSIN",  OP_SIN,        FD,    RL_8,           RL_,            RL_8},
{"IF@DTAN",  OP_TAN,        FD,    RL_8,           RL_,            RL_8},
{"IF@DSQRT", OP_SQRT,       FD,    RL_8,           RL_,            RL_8},
{"IF@DFABS", OP_FABS,       FD,    RL_8,           RL_,            RL_8},
{"IF@DACOS", OP_ACOS,       FD,    RL_8,           RL_,            RL_8},
{"IF@DASIN", OP_ASIN,       FD,    RL_8,           RL_,            RL_8},
{"IF@DATAN", OP_ATAN,       FD,    RL_8,           RL_,            RL_8},
{"IF@DCOSH", OP_COSH,       FD,    RL_8,           RL_,            RL_8},
{"IF@DEXP",  OP_EXP,        FD,    RL_8,           RL_,            RL_8},
{"IF@DLOG10",OP_LOG10,      FD,    RL_8,           RL_,            RL_8},
{"IF@DSINH", OP_SINH,       FD,    RL_8,           RL_,            RL_8},
{"IF@DTANH", OP_TANH,       FD,    RL_8,           RL_,            RL_8},

{"IF@ATAN2",OP_ATAN2,       FS,    RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"IF@FMOD", OP_FMOD,        FS,    RL_DX_AX,       RL_CX_BX,       RL_DX_AX},
{"IF@LOG",  OP_LOG,         FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@COS",  OP_COS,         FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@SIN",  OP_SIN,         FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@TAN",  OP_TAN,         FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@SQRT", OP_SQRT,        FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@FABS", OP_FABS,        FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@ACOS", OP_ACOS,        FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@ASIN", OP_ASIN,        FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@ATAN", OP_ATAN,        FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@COSH", OP_COSH,        FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@EXP",  OP_EXP,         FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@LOG10",OP_LOG10,       FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@SINH", OP_SINH,        FS,    RL_DX_AX,       RL_,            RL_DX_AX},
{"IF@TANH", OP_TANH,        FS,    RL_DX_AX,       RL_,            RL_DX_AX},

{"__chipbug",OP_NOP,        0,     RL_,            RL_,            RL_},
{"__fdiv_m32",OP_NOP,       0,     RL_,            RL_,            RL_},
{"__fdiv_m64",OP_NOP,       0,     RL_,            RL_,            RL_},
{"__fdiv_m32r",OP_NOP,      0,     RL_,            RL_,            RL_},
{"__fdiv_m64r",OP_NOP,      0,     RL_,            RL_,            RL_},
{"__fdiv_fpr",OP_NOP,       0,     RL_,            RL_,            RL_},

{"__NOP",    OP_NOP,        0,     RL_,            RL_,            RL_ }};

#include "cgnoalgn.h"
typedef struct {
        call_class      class;
        byte_seq_len    length;
        byte            data[];
}rt_aux_info;


static  rt_aux_info Scn1 = {
                        0, 6,
                       {0xF2,                   /*       repne*/
                        0xAE,                   /*       scasb*/
                        0xD1, 0xE1,             /*       shl     cx,1*/
                        0x89, 0xCF}             /*       mov     di,cx*/
                        };

static  rt_aux_info Scn2 = {
                        0, 2,
                       {0xF2,                   /*       repne*/
                        0xAF}                   /*       scasw*/
                        };

static  rt_aux_info Scn4 = {
                        0, 18,
                       {0x83, 0xC7, 0x02,       /* L1:   add     d1,2*/
                        0x49,                   /* L2:   dec     cx*/
                        0x74, 0x08,             /*       je      L3*/
                        0xAF,                   /*       scasw*/
                        0x75, 0xF7,             /*       jne     L1*/
                        0x92,                   /*       xchg    dx,ax*/
                        0xAF,                   /*       scasw*/
                        0x92,                   /*       xchg    dx,ax*/
                        0x75, 0xF5,             /*       jne     L2*/
                        0xD1, 0xE1,             /* L3:   shl     cx,1*/
                        0x89, 0xCF}             /*       mov     di,cx*/
                        };

#include "cgrealgn.h"

extern  char    *AskRTName( int rtindex )
/***************************************/
{
    return( RTInfo[  rtindex  ].nam );
}


static  hw_reg_set      FirstReg( reg_set_index index )
/******************************************************
    the tables above use RL_ consts rather that hw_reg_sets cause
    it cheaper. This just picks off the first register from a
    register list and returns it.
*/
{
    hw_reg_set  *list;

    list = RegSets[  index  ];
    return( *list );
}


extern  name    *Addressable( name *cons, type_class_def class )
/***************************************************************
    make sure a floating point constant is addressable (dropped
    it into memory if it isnt)
*/
{
    unsigned_64         buffer;

    if( cons->n.class == N_CONSTANT ) {
        switch( class ) {
        case FD:
        case FS:
            return( GenFloat( cons, class ) );
        case U8:
        case I8:
            buffer.u._32[ I64LO32 ] = cons->c.int_value;
            buffer.u._32[ I64HI32 ] = cons->c.int_value_2;
            return( GenConstData( (byte *)&buffer, class ) );
        default:
            Zoiks( ZOIKS_138 );
            break;
        }
        return( GenFloat( cons, class ) );
    }
    return( cons );
}


static void CheckForPCS( instruction *ins )
/******************************************
    check to see if pointer subtract is really pointer - pointer
    or pointer - integer (PCS = Pointer Constant Subtract)
*/
{
    if( RoutineNum + BEG_RTNS == RT_PTS ) {
        if( ins->operands[ 1 ]->n.name_class != PT
         && ins->operands[ 1 ]->n.name_class != CP ) {
            RoutineNum = RT_PCS - BEG_RTNS;
        }
    }
}


extern  bool    RTLeaveOp2( instruction *ins )
/*********************************************
    return true if it's a bad idea to put op2 into a temporary since we're
    gonna take the bugger's address in rMAKECALL anyway for FDD, FDC, EDA, etc
*/
{
    switch( ins->type_class ) {
    case FD:
        if( _FPULevel( FPU_87 ) ) return( FALSE );
        break;
/* -- This is not true now - I8 math and parameters are kept in registers -- [RomanT]
    case I8:
    case U8:
        break;
*/
    default:
        return( FALSE );
    }
    if( NumOperands( ins ) != 2 ) return( FALSE );
    return( TRUE );
}


static  void    FlipIns( instruction *ins )
/******************************************
   maybe flip the const/mem into the second operand so it goes in local data
*/
{
    name        *temp;

    switch( ins->head.opcode ) {
    case OP_ADD:
    case OP_MUL:
        break;
    default:
        return;
    }
    switch( ins->type_class ) {
    case FD:
    case I8:
    case U8:
        break;
    default:
        return;
    }
    temp = ins->operands[ 0 ];
    if( temp->n.class == N_CONSTANT
     || temp->n.class == N_MEMORY
     || temp->n.class == N_INDEXED ) {
        ins->operands[ 0 ] = ins->operands[ 1 ];
        ins->operands[ 1 ] = temp;
    }
}


extern  instruction     *rMAKECALL( instruction *ins )
/*****************************************************
    turn an instruction into the approprate runtime call sequence, using
    the tables above to decide where parms go.
*/
{
    rtn_info            *info;
    label_handle        lbl;
    instruction         *left_ins;
    instruction         *new_ins;
    instruction         *la_ins;
    instruction         *last_ins;
    name                *reg_name;
    hw_reg_set          regs;
    hw_reg_set          all_regs;
    name                *temp;
    name                *also_used;
    conflict_node       *conf;
    hw_reg_set          tmp;
    type_class_def      parm2_class;

    if( !_IsConvert( ins ) ) {
        LookupRoutine( ins );
        CheckForPCS( ins );
    } else { /* RoutineNum might be wrong if we ran out of memory in ExpandIns*/
        RoutineNum = AskHow( ins->base_type_class, ins->type_class )
                   - BEG_RTNS;
    }
    FlipIns( ins );
    info = &RTInfo[  RoutineNum  ];
    regs = FirstReg( info->left );
    all_regs = regs;
    tmp = ReturnReg( WD, FALSE );
    HW_TurnOn( all_regs, tmp );
    left_ins = MakeMove( ins->operands[ 0 ], AllocRegName( regs ),
                          info->operand_class );
    ins->operands[ 0 ] = left_ins->result;
    MoveSegOp( ins, left_ins, 0 );
    PrefixIns( ins, left_ins );
    regs = FirstReg( info->right );
    also_used = NULL;
    if( !HW_CEqual( regs, HW_EMPTY ) ) {
        if( info->right == RL_8 ) {
            temp = ins->operands[ 1 ];
            if( temp->n.class==N_TEMP && ( temp->t.temp_flags & CONST_TEMP ) ) {
                temp = temp->v.symbol;
            }
            if( temp->n.class == N_CONSTANT ) {
                temp = Addressable( temp, info->operand_class );
                la_ins = MakeUnary( OP_CAREFUL_LA, temp,
                                      AllocRegName( HW_SI ), U2 );
                also_used = la_ins->operands[ 0 ];
                ins->operands[ 1 ] = la_ins->result;
                PrefixIns( ins, la_ins );
                if( !SegIsSS( temp ) ) {
                    new_ins = MakeMove( GetSegment( temp ),
                                        AllocRegName(HW_ES), U2 );
                    HW_CTurnOn( all_regs, HW_ES );
                    PrefixIns( ins, new_ins );
                    ++RoutineNum;
                }
            } else if( temp->n.class == N_TEMP ) {
                la_ins = MakeUnary( OP_CAREFUL_LA,temp,
                                    AllocRegName( HW_SI ),U2 );
                also_used = temp;
                ins->operands[ 1 ] = la_ins->result;
                MoveSegOp( ins, la_ins, 0 );
                DelSeg( la_ins );
                PrefixIns( ins, la_ins );
            } else if( ins->num_operands == 3 ) {
                new_ins = MakeMove( ins->operands[ 2 ],
                                    AllocRegName(HW_ES), U2 );
                ins->operands[ 2 ] = new_ins->result;
                PrefixIns( ins, new_ins );
                la_ins = MakeUnary( OP_CAREFUL_LA, temp,
                                    AllocRegName( HW_SI ), U2 );
                also_used = temp;
                ins->operands[ 1 ] = la_ins->result;
                MoveSegOp( ins, la_ins, 0 );
                DelSeg( la_ins );
                PrefixIns( ins, la_ins );
                HW_CTurnOn( all_regs, HW_ES );
                ++RoutineNum;
            } else if( ( temp->n.class == N_MEMORY && !SegIsSS( temp ) ) ||
                       ( temp->n.class == N_INDEXED && temp->i.base != NULL &&
                         !SegIsSS( temp->i.base ) ) ) {
                la_ins = MakeUnary( OP_CAREFUL_LA, ins->operands[ 1 ],
                                      AllocRegName( HW_ES_SI ), PT );
                also_used = ins->operands[ 1 ];
                ins->operands[ 1 ] = la_ins->result;
                PrefixIns( ins, la_ins );
                HW_CTurnOn( all_regs, HW_ES );
                ++RoutineNum;
            } else {
                la_ins = MakeUnary( OP_CAREFUL_LA, temp,
                                    AllocRegName( HW_SI ), U2 );
                also_used = temp;
                ins->operands[ 1 ] = la_ins->result;
                MoveSegOp( ins, la_ins, 0 );
                DelSeg( la_ins );
                PrefixIns( ins, la_ins );
            }
            HW_CTurnOn( all_regs, HW_SI );
        } else {
            /* If I knew how to turn a register list index into a type class,
               I'd do that, and avoid this if */
            if( info->right == RL_SI ) {
                new_ins = MakeMove( ins->operands[ 1 ], AllocRegName( regs ),
                                        U2 );
            } else {
                new_ins = MakeMove( ins->operands[ 1 ], AllocRegName( regs ),
                                        info->operand_class );
            }
            ins->operands[ 1 ] = new_ins->result;
            MoveSegOp( ins, new_ins, 0 );
            HW_TurnOn( all_regs, regs );
            PrefixIns( ins, new_ins );
        }
    } else if( NumOperands( ins ) == 2 ) {
        if( RoutineNum == RT_DPOWI ) {
            parm2_class = I4;
        } else {
            parm2_class = ins->type_class;
        }
        new_ins = MakeUnary( OP_PUSH, ins->operands[ 1 ], NULL, parm2_class );
        MoveSegOp( ins, new_ins, 0 );
        PrefixIns( ins, new_ins );
    }
    reg_name = AllocRegName( all_regs );
    lbl = RTLabel( RoutineNum );
    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = ins->type_class;
    new_ins->operands[ CALL_OP_USED ] = reg_name;
    if( also_used != NULL ) {
        switch( also_used->n.class ) {
        case N_REGISTER:
        case N_CONSTANT:
            _Zoiks( ZOIKS_047 );
            break;
        case N_MEMORY:
        case N_TEMP:
            also_used->v.usage |= ( USE_MEMORY | NEEDS_MEMORY );
            conf = also_used->v.conflict;
            if( conf != NULL ) {
                InMemory( conf );
            }
            break;
        case N_INDEXED:            /* Dec-20-88*/
            also_used = reg_name;  /* - the index doesn't need an index register!*/
        default:
            break;
        }
        new_ins->operands[ CALL_OP_USED2 ] = also_used;
    } else {
        new_ins->operands[ CALL_OP_USED2 ] = reg_name;
    }
    new_ins->operands[ CALL_OP_ADDR ]= AllocMemory( lbl, 0, CG_LBL,
                                                    ins->type_class );
    new_ins->result = NULL;
    new_ins->num_operands = 2;         /* special case for OP_CALL*/
    new_ins->zap = &AllocRegName( all_regs )->r;/* all parm regs could be zapped*/
    last_ins = new_ins;
    if( ins->result != NULL ) {
        regs = FirstReg( info->result );
        tmp = regs;
        HW_TurnOn( tmp, new_ins->zap->reg );
        new_ins->zap = &AllocRegName( tmp )->r;
        reg_name = AllocRegName( regs );
        new_ins->result = reg_name;
        last_ins = MakeMove( reg_name, ins->result, ins->type_class );
        ins->result = last_ins->operands[ 0 ];
        MoveSegRes( ins, last_ins );
        SuffixIns( ins, last_ins );
        ReplIns( ins, new_ins );
    } else {                /* comparison, still need conditional jumps*/
        ins->operands[ 0 ] = AllocIntConst( 0 );
        ins->operands[ 1 ] = AllocIntConst( 1 );
        DelSeg( ins );
        DoNothing( ins );               /* just conditional jumps for ins*/
        PrefixIns( ins, new_ins );
        new_ins->ins_flags |= INS_CC_USED;
        last_ins = ins;
    }
    UpdateLive( left_ins, last_ins );
    return( left_ins );
}


extern  name    *ScanCall( tbl_control *table, name *value,
                           type_class_def tipe )
/**********************************************************
    generates a fake call to a rutime routine that looks up "value" in a table
    and jumps to the appropriate case, using either a pointer or index
    returned by the "routine". The "routine" will be generated inline later.
    See BEAuxInfo for the code sequences generated. That will explain
    how the jump destination is determined as well.
*/
{
    instruction *new_ins;
    name        *reg_name;
    name        *result;
    name        *label;
    hw_reg_set  tmp;
    name        *temp_result;

    switch( tipe ) {
    case U1:
        RoutineNum = RT_SCAN1 - BEG_RTNS;
        break;
    case U2:
        RoutineNum = RT_SCAN2 - BEG_RTNS;
        break;
    case U4:
        RoutineNum = RT_SCAN4 - BEG_RTNS;
        break;
    default:
        break;
    }

    reg_name = AllocRegName( FirstReg( RTInfo[  RoutineNum  ].left ) );
    new_ins = MakeConvert( value, reg_name, tipe, value->n.name_class );
    AddIns( new_ins );

    reg_name = AllocRegName( HW_CX );
    new_ins = MakeMove( AllocRegName( HW_CS ), reg_name, U2 );
    AddIns( new_ins );
    new_ins = MakeMove( reg_name, AllocRegName( HW_ES ), U2 );
    AddIns( new_ins );
    new_ins = MakeMove( AllocIntConst( table->size + 1 ), reg_name, U2 );
    AddIns( new_ins );

    reg_name = AllocRegName( HW_DI );
    if( tipe == U4 ) {
        label = AllocMemory( table, -2, CG_VTB, U2 );
    } else {
        label = AllocMemory( table, 0, CG_VTB, U2 );
    }
    label = AddrConst( label, AskCodeSeg(), CONS_OFFSET );
    new_ins = MakeMove( label, reg_name, U2 );
    AddIns( new_ins );

    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = U2;
    tmp = FirstReg( RTInfo[ RoutineNum ].left );
    HW_CTurnOn( tmp, HW_ES_DI );
    HW_CTurnOn( tmp, HW_CX );
    new_ins->operands[ CALL_OP_USED ] = AllocRegName( tmp );
    new_ins->operands[ CALL_OP_USED2 ] = new_ins->operands[ CALL_OP_USED ];
    new_ins->operands[ CALL_OP_ADDR ] = AllocMemory( RTLabel(RoutineNum),
                                             0, CG_LBL, U2 );
    new_ins->result = NULL;
    new_ins->num_operands = 2;
    new_ins->zap = &AllocRegName( HW_CX_DI )->r;
    new_ins->result = reg_name;
    AddIns( new_ins );

    result = AllocMemory( table, 0, CG_TBL, U2 ); /* so table gets freed!*/
    if( tipe == U2 ) {
        result = AllocRegName( HW_ES_DI );
        result = AllocIndex( result, NULL, ( table->size - 1 )*2, U2 );
    } else {
        result = AllocIndex( reg_name, result, 0, U2 );
    }
    // this is here because we can get ourselves into trouble
    // by hoisting expressions into the spot between the call and
    // the OP_SELECT instruction at the end of the block if those
    // expressions expand to instructions which require ECX.
    // Same goes for EDI in the U4 case.        BBB - July, 1996
    temp_result = AllocTemp( WD );
    new_ins = MakeMove( result, temp_result, WD );
    AddIns( new_ins );
    return( temp_result );
}


extern  instruction     *rMAKEFNEG( instruction *ins )
/*****************************************************
    negating a floating point value which is in the 386 registers only
    needs to change the register containing the exponent, so this is
    handled as a special case rather than using rMAKERTCALL that would
    assume all of the registers containing the number were used
    and modified by the call.
*/
{
    rtn_info            *info;
    label_handle        lbl;
    instruction         *left_ins;
    instruction         *new_ins;
    instruction         *last_ins;
    name                *reg_name;
    name                *exp_reg;

    LookupRoutine( ins );
    CheckForPCS( ins );
    lbl = RTLabel( RoutineNum );
    info = &RTInfo[  RoutineNum  ];
    reg_name = AllocRegName( FirstReg( info->left ) );
    left_ins = MakeMove( ins->operands[ 0 ], reg_name,
                          info->operand_class );
    ins->operands[ 0 ] = left_ins->result;
    MoveSegOp( ins, left_ins, 0 );
    PrefixIns( ins, left_ins );
    if( ins->type_class == FD ) { /* exponent in AX*/
        exp_reg = AllocRegName( HW_AX );
    } else {                           /* exponent in DX*/
        exp_reg = AllocRegName( HW_DX );
    }
    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = U2;
    new_ins->operands[ CALL_OP_USED ] = exp_reg;
    new_ins->operands[ CALL_OP_USED2 ] = exp_reg;
    new_ins->operands[ CALL_OP_ADDR ] = AllocMemory( lbl, 0, CG_LBL, U2 );
    new_ins->result = NULL;
    new_ins->num_operands = 2;
    new_ins->zap = &exp_reg->r;
    last_ins = new_ins;
    new_ins->result = exp_reg;
    last_ins = MakeMove( reg_name, ins->result, info->operand_class );
    ins->result = last_ins->operands[ 0 ];
    MoveSegRes( ins, last_ins );
    SuffixIns( ins, last_ins );
    ReplIns( ins, new_ins );
    UpdateLive( left_ins, last_ins );
    return( left_ins );
}


extern  pointer BEAuxInfo( pointer hdl, aux_class request )
/**********************************************************
    see ScanCall for explanation
*/
{
    pointer     info = NULL;

    switch( request ) {
    case AUX_LOOKUP:
        switch( FindRTLabel( hdl ) ) {
        case RT_SCAN1:
            info = &Scn1;
            break;
        case RT_SCAN2:
            info = &Scn2;
            break;
        case RT_SCAN4:
            info = &Scn4;
            break;
        default:
            info = NULL;
            break;
        }
        break;
    case CALL_CLASS:
        info = hdl;
        info = &((rt_aux_info *)info)->class;
        break;
    case CALL_BYTES:
        info = hdl;
        info = &((rt_aux_info *)info)->length;
        break;
    default:
        break;
    }
    return( info );
}
