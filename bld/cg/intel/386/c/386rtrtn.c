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

extern  name            *GenFloat( name *, type_class_def );
extern  void            UpdateLive( instruction *, instruction * );
extern  name            *AllocIntConst( int );
extern  void            ReplIns( instruction *, instruction * );
extern  void            SuffixIns( instruction *, instruction * );
extern  void            MoveSegRes( instruction *, instruction * );
extern  instruction     *MakeBinary( opcode_defs, name *, name *, name *, type_class_def );
extern  name            *AllocMemory( pointer, type_length, cg_class, type_class_def );
extern  bool            SegIsSS( name * );
extern  void            DelSeg( instruction * );
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
extern  seg_id          AskBackSeg( void );
extern  void            LookupRoutine( instruction * );
extern  label_handle    RTLabel( int );
extern  int             FindRTLabel( label_handle );
extern  instruction     *rMAKECALL( instruction * );
extern  hw_reg_set      FirstReg( reg_set_index );

/*
 * If you add a new routine, let John know as the debugger recognizes
 * these.
 */

rtn_info RTInfo[RT_NOP-BEG_RTNS+1] = {
/* name    op            class   left            right           result*/
"__U4FS", OP_CONVERT,   U4,     RL_EAX,         RL_,            RL_EAX,
"__I4FS", OP_CONVERT,   I4,     RL_EAX,         RL_,            RL_EAX,
"__U4FD", OP_CONVERT,   U4,     RL_EAX,         RL_,            RL_EDX_EAX,
"__I4FD", OP_CONVERT,   I4,     RL_EAX,         RL_,            RL_EDX_EAX,
"__FSFD", OP_CONVERT,   FS,     RL_EAX,         RL_,            RL_EDX_EAX,
"__FSI4", OP_CONVERT,   FS,     RL_EAX,         RL_,            RL_EAX,
"__RSI4", OP_ROUND,     FS,     RL_EAX,         RL_,            RL_EAX,
"__FSU4", OP_CONVERT,   FS,     RL_EAX,         RL_,            RL_EAX,
"__RSU4", OP_ROUND,     FS,     RL_EAX,         RL_,            RL_EAX,
"__FDI4", OP_CONVERT,   FD,     RL_EDX_EAX,     RL_,            RL_EAX,
"__RDI4", OP_ROUND,     FD,     RL_EDX_EAX,     RL_,            RL_EAX,
"__FDU4", OP_CONVERT,   FD,     RL_EDX_EAX,     RL_,            RL_EAX,
"__RDU4", OP_ROUND,     FD,     RL_EDX_EAX,     RL_,            RL_EAX,
"__FDFS", OP_CONVERT,   FD,     RL_EDX_EAX,     RL_,            RL_EAX,
"__RDFS", OP_ROUND,     FD,     RL_EDX_EAX,     RL_,            RL_EAX,

"__U8FS", OP_CONVERT,   U8,     RL_EDX_EAX,     RL_,            RL_EAX,
"__I8FS", OP_CONVERT,   I8,     RL_EDX_EAX,     RL_,            RL_EAX,
"__U8FD", OP_CONVERT,   U8,     RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"__I8FD", OP_CONVERT,   I8,     RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"__FSI8", OP_CONVERT,   FS,     RL_EAX,         RL_,            RL_EDX_EAX,
"__RSI8", OP_ROUND,     FS,     RL_EAX,         RL_,            RL_EDX_EAX,
"__FSU8", OP_CONVERT,   FS,     RL_EAX,         RL_,            RL_EDX_EAX,
"__RSU8", OP_ROUND,     FS,     RL_EAX,         RL_,            RL_EDX_EAX,
"__FDI8", OP_CONVERT,   FD,     RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"__RDI8", OP_ROUND,     FD,     RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"__FDU8", OP_CONVERT,   FD,     RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"__RDU8", OP_ROUND,     FD,     RL_EDX_EAX,     RL_,            RL_EDX_EAX,

"__U8FS7",OP_CONVERT,   U8,     RL_EDX_EAX,     RL_,            RL_EAX,
"__U8FD7",OP_CONVERT,   U8,     RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"__FSU87",OP_CONVERT,   FS,     RL_EAX,         RL_,            RL_EDX_EAX,
"__FDU87",OP_CONVERT,   FD,     RL_EDX_EAX,     RL_,            RL_EDX_EAX,

"__I8M",    OP_MUL,        I8,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__I8D",    OP_DIV,        I8,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__I8D",    OP_MOD,        I8,    RL_EDX_EAX,     RL_FPPARM2,     RL_FPPARM2,
"__I8RS",   OP_RSHIFT,     I8,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__I8LS",   OP_LSHIFT,     I8,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,

"__U8M",    OP_MUL,        U8,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__U8D",    OP_DIV,        U8,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__U8D",    OP_MOD,        U8,    RL_EDX_EAX,     RL_FPPARM2,     RL_FPPARM2,
"__U8RS",   OP_RSHIFT,     U8,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__U8LS",   OP_LSHIFT,     U8,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,


"__FSA",  OP_ADD,       FS,     RL_EAX,         RL_EDX,         RL_EAX,
"__FSS",  OP_SUB,       FS,     RL_EAX,         RL_EDX,         RL_EAX,
"__FSM",  OP_MUL,       FS,     RL_EAX,         RL_EDX,         RL_EAX,
"__FSD",  OP_DIV,       FS,     RL_EAX,         RL_EDX,         RL_EAX,
"__FSC",  OP_CMP,       FS,     RL_EAX,         RL_EDX,         RL_,
"__FSN",  OP_NEGATE,    FS,     RL_EAX,         RL_,            RL_EAX,
"__FDA",  OP_ADD,       FD,     RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__FDS",  OP_SUB,       FD,     RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__FDM",  OP_MUL,       FD,     RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__FDD",  OP_DIV,       FD,     RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__FDC",  OP_CMP,       FD,     RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"__FDN",  OP_NEGATE,    FD,     RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"__CHK",  OP_CALL,       0,     RL_,            RL_,            RL_,
"__GRO",  OP_CALL,       0,     RL_,            RL_,            RL_,
"__TNK",  OP_CALL,       0,     RL_,            RL_,            RL_,
"__TNC",  OP_CALL,       0,     RL_,            RL_,            RL_,
"__CHP",  OP_CALL,       0,     RL_,            RL_,            RL_,
"__SCN1", OP_SELECT,     0,     RL_AL,          RL_,            RL_,
"__SCN2", OP_SELECT,     0,     RL_AX,          RL_,            RL_,
"__SCN4", OP_SELECT,     0,     RL_EAX,         RL_,            RL_,
"__GETDS",OP_CALL,       0,     RL_,            RL_,            RL_,
"__EPI",  OP_CALL,       0,     RL_,            RL_,            RL_,
"__PRO",  OP_CALL,       0,     RL_,            RL_,            RL_,
"__PON",  OP_CALL,       0,     RL_,            RL_,            RL_,
"__POFF", OP_CALL,       0,     RL_,            RL_,            RL_,

"__FlatToFar16",  OP_PTR_TO_FOREIGN,   U4,    RL_EAX,  RL_,     RL_EAX,
"__Far16ToFlat",  OP_PTR_TO_NATIVE,    U4,    RL_EAX,  RL_,     RL_EAX,
"__Far16Func2",   OP_CALL,             U4,    RL_EAX,  RL_,     RL_EAX,
"__Far16Cdecl2",  OP_CALL,             U4,    RL_EAX,  RL_,     RL_EAX,
"__Far16Pascal2", OP_CALL,             U4,    RL_EAX,  RL_,     RL_EAX,
"__Far32Func",    OP_CALL,             0,     RL_,     RL_,     RL_,

"IF@DP5DIV",OP_P5DIV,      FD,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"IF@P5DIV", OP_P5DIV,      FS,    RL_EAX,         RL_EDX,         RL_EAX,

"IF@DPOW",  OP_POW,        FD,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"IF@DPOWI", OP_POW,        FD,    RL_EDX_EAX,     RL_ECX,         RL_EDX_EAX,
"IF@POW",   OP_POW,        FS,    RL_EAX,         RL_EDX,         RL_EAX,
"IF@POWI",  OP_POW,        FS,    RL_EAX,         RL_EDX,         RL_EAX,
"IF@IPOW",  OP_POW,        I4,    RL_EAX,         RL_EDX,         RL_EAX,

"IF@DATAN2",OP_ATAN2,      FD,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"IF@DFMOD", OP_FMOD,       FD,    RL_EDX_EAX,     RL_FPPARM2,     RL_EDX_EAX,
"IF@DLOG",  OP_LOG,        FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DCOS",  OP_COS,        FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DSIN",  OP_SIN,        FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DTAN",  OP_TAN,        FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DSQRT", OP_SQRT,       FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DFABS", OP_FABS,       FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DACOS", OP_ACOS,       FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DASIN", OP_ASIN,       FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DATAN", OP_ATAN,       FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DCOSH", OP_COSH,       FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DEXP",  OP_EXP,        FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DLOG10",OP_LOG10,      FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DSINH", OP_SINH,       FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,
"IF@DTANH", OP_TANH,       FD,    RL_EDX_EAX,     RL_,            RL_EDX_EAX,

"IF@ATAN2", OP_ATAN2,      FS,    RL_EAX,         RL_EDX,         RL_EAX,
"IF@FMOD",  OP_FMOD,       FS,    RL_EAX,         RL_EDX,         RL_EAX,
"IF@LOG",   OP_LOG,        FS,    RL_EAX,         RL_,            RL_EAX,
"IF@COS",   OP_COS,        FS,    RL_EAX,         RL_,            RL_EAX,
"IF@SIN",   OP_SIN,        FS,    RL_EAX,         RL_,            RL_EAX,
"IF@TAN",   OP_TAN,        FS,    RL_EAX,         RL_,            RL_EAX,
"IF@SQRT",  OP_SQRT,       FS,    RL_EAX,         RL_,            RL_EAX,
"IF@FABS",  OP_FABS,       FS,    RL_EAX,         RL_,            RL_EAX,
"IF@ACOS",  OP_ACOS,       FS,    RL_EAX,         RL_,            RL_EAX,
"IF@ASIN",  OP_ASIN,       FS,    RL_EAX,         RL_,            RL_EAX,
"IF@ATAN",  OP_ATAN,       FS,    RL_EAX,         RL_,            RL_EAX,
"IF@COSH",  OP_COSH,       FS,    RL_EAX,         RL_,            RL_EAX,
"IF@EXP",   OP_EXP,        FS,    RL_EAX,         RL_,            RL_EAX,
"IF@LOG10", OP_LOG10,      FS,    RL_EAX,         RL_,            RL_EAX,
"IF@SINH",  OP_SINH,       FS,    RL_EAX,         RL_,            RL_EAX,
"IF@TANH",  OP_TANH,       FS,    RL_EAX,         RL_,            RL_EAX,

/* Following are special runtime routines called from the FAST -od codegen */

"__87LDI2", OP_NOP,        0,     RL_,            RL_,            RL_,
"__87LDI4", OP_NOP,        0,     RL_,            RL_,            RL_,
"__COPY",   OP_NOP,        0,     RL_,            RL_,            RL_,
"__FADD",   OP_NOP,        0,     RL_,            RL_,            RL_,
"__FSUB",   OP_NOP,        0,     RL_,            RL_,            RL_,
"__FSUBR",  OP_NOP,        0,     RL_,            RL_,            RL_,
"__FMUL",   OP_NOP,        0,     RL_,            RL_,            RL_,
"__FDIV",   OP_NOP,        0,     RL_,            RL_,            RL_,
"__FDIVR",  OP_NOP,        0,     RL_,            RL_,            RL_,
"__FCMP",   OP_NOP,        0,     RL_,            RL_,            RL_,
"__FCMPR",  OP_NOP,        0,     RL_,            RL_,            RL_,
"__TryInit",OP_NOP,        0,     RL_,            RL_,            RL_,
"__TryUnwind",OP_NOP,      0,     RL_,            RL_,            RL_,

/* End of special runtime routines called from the FAST -od codegen */

"__chipbug",OP_NOP,        0,     RL_,            RL_,            RL_,
"__fdiv_m32",OP_NOP,       0,     RL_,            RL_,            RL_,
"__fdiv_m64",OP_NOP,       0,     RL_,            RL_,            RL_,
"__fdiv_m32r",OP_NOP,      0,     RL_,            RL_,            RL_,
"__fdiv_m64r",OP_NOP,      0,     RL_,            RL_,            RL_,
"__fdiv_fpr",OP_NOP,       0,     RL_,            RL_,            RL_,
"__tls_index",OP_NOP,      0,     RL_,            RL_,            RL_,
"__tls_array",OP_NOP,      0,     RL_,            RL_,            RL_,
"__tls_region",OP_NOP,     0,     RL_,            RL_,            RL_,

"__NOP",    OP_NOP,        0,     RL_,            RL_,            RL_ };


#include "cgnoalgn.h"
typedef struct {
        call_class      class;
        byte_seq_len    length;
        byte            data[];
} rt_aux_info;

static  rt_aux_info Scn1 = {
                        0, 2,
                       {0xF2,                   /*       repne     */
                        0xAE}                   /*       scasb     */
                        };

static  rt_aux_info Scn1ES = {
                        0, 6,
                       {0x06,                   /*      push    es */
                        0x0e,                   /*      push    cs */
                        0x07,                   /*      pop     es */
                        0xF2,                   /*      repne      */
                        0xAE,                   /*      scasb      */
                        0x07}                   /*      pop     es */
                        };

static  rt_aux_info Scn2 = {            /* or Scn4 in USE16 */
                        0, 3,
                       {0xF2,                   /*       repne     */
                        0x66, 0xAF}             /*       scasw     */
                        };

static  rt_aux_info Scn2ES = {          /* or Scn4 in USE16 */
                        0, 7,
                       {0x06,                   /*      push    es */
                        0x0e,                   /*      push    cs */
                        0x07,                   /*      pop     es */
                        0xF2,                   /*      repne      */
                        0x66, 0xAF,             /*      scasw      */
                        0x07}                   /*      pop     es */
                        };

static  rt_aux_info Scn4 = {            /* or Scn2 in USE16 */
                        0, 2,
                       {0xF2,                   /*       repne     */
                        0xAF}                   /*       scasd     */
                        };

static  rt_aux_info Scn4ES = {          /* or Scn2 in USE16 */
                        0, 6,
                       {0x06,                   /*      push    es */
                        0x0e,                   /*      push    cs */
                        0x07,                   /*      pop     es */
                        0xF2,                   /*      repne      */
                        0xAF,                   /*      scasd      */
                        0x07}                   /*      pop     es */
                        };
#include "cgrealgn.h"


extern  char    *AskRTName( int rtindex )
/***************************************/
{
    if( _IsTargetModel( INDEXED_GLOBALS ) ) {
        switch( rtindex + BEG_RTNS ) {
        case RT_FDA:
            return( "__FXA" );
        case RT_FDS:
            return( "__FXS" );
        case RT_FDM:
            return( "__FXM" );
        case RT_FDD:
            return( "__FXD" );
        case RT_FDC:
            return( "__FXC" );
        case RT_DPOW:
            return( "RT@XPOW" );
        case RT_DATAN2:
            return( "IF@XATAN2" );
        case RT_DFMOD:
            return( "IF@XFMOD" );
        }
    }
    return( RTInfo[  rtindex  ].nam );
}


extern  bool    RTLeaveOp2( instruction *ins )
/*********************************************
    return true if it's a bad idea to put op2 into a temporary since we're
    gonna take the bugger's address in rMAKECALL.
*/
{
    ins = ins;
    return( FALSE );
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
    }

    reg_name = AllocRegName( FirstReg( RTInfo[  RoutineNum  ].left ) );
    new_ins = MakeConvert( value, reg_name, tipe, value->n.name_class );
    AddIns( new_ins );

    reg_name = AllocRegName( HW_ECX );
    new_ins = MakeMove( AllocIntConst( table->size + 1 ), reg_name, U4 );
    AddIns( new_ins );

    reg_name = AllocRegName( HW_EDI );
    label = AllocMemory( table, 0, CG_VTB, U4 );
    label = AddrConst( label, AskBackSeg(), CONS_OFFSET );
    new_ins = MakeMove( label, reg_name, U4 );
    AddIns( new_ins );

    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = U2;
    tmp = FirstReg( RTInfo[ RoutineNum ].left );
    HW_CTurnOn( tmp, HW_ECX );
    HW_CTurnOn( tmp, HW_EDI );
    HW_CTurnOn( tmp, HW_ES );
    new_ins->operands[ CALL_OP_USED ] = AllocRegName( tmp );
    new_ins->operands[ CALL_OP_USED2 ] = new_ins->operands[ CALL_OP_USED ];
    new_ins->operands[ CALL_OP_ADDR ] = AllocMemory( RTLabel(RoutineNum),
                                             0, CG_LBL, U4 );
    new_ins->result = NULL;
    new_ins->num_operands = 2;
    AddIns( new_ins );

    result = AllocMemory( table, 0, CG_TBL, U4 ); /* so table gets freed!*/
    if( tipe == U4 ) {
        HW_CAsgn( tmp, HW_ECX );
        HW_CTurnOn( tmp, HW_EDI );
        new_ins->zap = &AllocRegName( tmp )->r;
        new_ins->result = AllocRegName( HW_EDI );
        HW_CAsgn( tmp, HW_CS );
        HW_CTurnOn( tmp, HW_EDI );
        result = AllocRegName( tmp );
        result = AllocIndex( result, NULL, ( table->size - 1 )*4, U4 );
        new_ins = MakeMove( result, AllocTemp( WD ), WD );
        AddIns( new_ins );
        result = new_ins->result;
    } else {
        HW_CAsgn( tmp, HW_ECX );
        HW_CTurnOn( tmp, HW_EDI );
        new_ins->zap = &AllocRegName( tmp )->r;
        new_ins->result = AllocRegName( HW_ECX );
        new_ins = MakeMove( new_ins->result, AllocTemp( WD ), WD );
        AddIns( new_ins );
        result = AllocIndex( new_ins->result, result, 0, U4 );
        result->i.scale = 2; /* 2**2 == 4 */
    }
    return( result );
}


extern  name    *Addressable( name *cons, type_class_def class )
/***************************************************************
    make sure a floating point constant is addressable (dropped
    it into memory if it isnt)
*/
{
    if( cons->n.class == N_CONSTANT ) return( GenFloat( cons, class ) );
    return( cons );
}


extern  pointer BEAuxInfo( pointer hdl, aux_class request )
/**********************************************************
    see ScanCall for explanation
*/
{
    pointer     info;

    switch( request ) {
    case AUX_LOOKUP:
        switch( FindRTLabel( hdl ) ) {
        case RT_SCAN1:
            if( _IsntTargetModel( FLAT_MODEL ) ) return( &Scn1ES );
            return( &Scn1 );
        case RT_SCAN2:
            if( _IsntTargetModel( USE_32 ) ) {
                if( _IsntTargetModel( FLAT_MODEL ) ) return( &Scn4ES );
                return( &Scn4 );
            } else {
                if( _IsntTargetModel( FLAT_MODEL ) ) return( &Scn2ES );
                return( &Scn2 );
            }
        case RT_SCAN4:
            if( _IsntTargetModel( USE_32 ) ) {
                if( _IsntTargetModel( FLAT_MODEL ) ) return( &Scn2ES );
                return( &Scn2 );
            } else {
                if( _IsntTargetModel( FLAT_MODEL ) ) return( &Scn4ES );
                return( &Scn4 );
            }
        default:
            return( NULL );
        }
    case CALL_CLASS:
        info = hdl;
        return( &((rt_aux_info *)info)->class );
    case CALL_BYTES:
        info = hdl;
        return( &((rt_aux_info *)info)->length );
    default:
        _Zoiks( ZOIKS_128 );
        return( NULL );
    }
}

extern  instruction     *rMAKEFNEG( instruction *ins )
/*****************************************************
    this is intentionally a stub for the 386.
*/
{
    return( ins );
}
