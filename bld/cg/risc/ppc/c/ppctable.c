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
* Description:  Table translating generic cg "assembler" to PowerPC
*               specific instructions.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "pattern.h"
#include "regset.h"
#include "model.h"
#include "vergen.h"
#include "tables.h"
#include "funits.h"

opcode_entry    StubUnary[] = {
/*************************/
/*       op1   op2   res           verify          gen           reg      fu */
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_NO,         RG_DWORD, FU_NO,
};


opcode_entry    StubBinary[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg      fu */
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           G_NO,         RG_DWORD, FU_NO,
};

opcode_entry    StubSide[] = {
/******************************/
/*       op1   op2                 verify          gen           reg      fu */
_Side(  ANY,  ANY ),               V_NO,           G_NO,         RG_DWORD, FU_NO,
};

#define LOAD_TABLE( name, reg ) \
static  opcode_entry    name[] = {                                                      \
/********************************/                                                      \
/*       op    res   eq          verify          gen             reg fu*/               \
_UnPP(  M,    R,    NONE ),     V_NO,           G_LOAD_ADDR,    RG_##reg,FU_ALU,                \
_Un(    M,    M,    NONE ),     V_NO,           R_MOVRESTEMP,   RG_##reg,FU_NO,         \
_Un(    M,    ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_##reg##_NEED,FU_NO,  \
_Un(    ANY,  ANY,  NONE ),     V_NO,           R_FORCEOP1MEM,  RG_##reg,FU_NO,         \
};

LOAD_TABLE( LoadAddr2, WORD );
LOAD_TABLE( LoadAddr8, QWORD );

static  opcode_entry    LoadAddr4[] = {
/*************************************/
/*       op    res   eq          verify          gen             reg fu*/
_Un(    M,    ANY,  NONE ),     V_OFFSETZERO,   R_MOVEINDEX,    RG_DWORD,FU_NO,
_UnPP(  M,    R,    NONE ),     V_NO,           G_LOAD_ADDR,    RG_DWORD,FU_ALU,
_Un(    M,    M,    NONE ),     V_NO,           R_MOVRESTEMP,   RG_DWORD,FU_NO,
_Un(    M,    ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_DWORD_NEED,FU_NO,
_Un(    ANY,  ANY,  NONE ),     V_NO,           R_FORCEOP1MEM,  RG_DWORD,FU_NO,
};

opcode_entry    Conv[] = {
/************************/
/*       op1   res  eq             verify          gen           reg      fu */
_Un(     ANY,  ANY, NONE ),        V_NO,           R_DOCVT,      RG_,     FU_NO,
};

opcode_entry    NegF[] = {
/************************/
/*       op1   res   eq            verify          gen           reg      fu */
_Un(    R,    R,    NONE ),        V_NO,           G_UNARY,      RG_FLOAT,FU_NO,
_Un(    C|M,  ANY,  NONE ),        V_NO,           R_MOVOP1TEMP, RG_FLOAT,FU_NO,
_Un(    ANY,  M,    NONE ),        V_NO,           R_MOVRESTEMP, RG_FLOAT,FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_FLOAT_NEED, FU_NO,
};

opcode_entry    MoveXX[] = {
/**************************/
/*       op    res   eq            verify          gen           reg      fu*/
_Un(    ANY,  ANY,  NONE  ),       V_REG_SIZE,     R_CHANGETYPE, RG_,     FU_NO,
_Un(    M|U,  M|U,  EQ_R1 ),       NVI(V_NO),      G_NO,         RG_,     FU_NO,
_UnPP(  M,    M,    NONE  ),       NVI(V_SAME_LOCN),G_NO,        RG_,     FU_NO,
_Un(    U,    ANY,  NONE ),        V_NO,           R_FORCEOP1MEM,RG_,     FU_NO,
_Un(    ANY,  U,    NONE ),        V_NO,           R_FORCERESMEM,RG_,     FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_OP1_RES_AL4,  R_MOVEXX_4,   RG_DWORD_NEED,FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           R_MOVEXX,     RG_,     FU_NO,
};

#define MOVE_TABLE( t_name, reg, load, store ) \
opcode_entry    t_name[] = {                                                            \
/**************************/                                                            \
/*       op1   res   eq            verify          gen           reg      fu */         \
_Un(     ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      G_NO,         RG_,     FU_NO,        \
_UnPP(   M,    M,    NONE  ),      NVI(V_SAME_LOCN),G_NO,         RG_,     FU_NO,       \
_Un(     R,    R,    NONE ),       V_NO,           G_MOVE,       RG_##reg,FU_NO,        \
_Un(     R,    M,    NONE ),       V_NO,           store,        RG_##reg,FU_MEM,       \
_Un(     C,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_##reg,FU_NO,        \
_Un(     M,    R,    NONE ),       V_NO,           load,         RG_##reg,FU_MEM,       \
_Un(     C,    R,    NONE ),       V_OP1HIGHADDR,  G_LEA_HIGH,   RG_##reg,FU_NO,        \
_Un(     C,    R,    NONE ),       V_HALFWORDCONST1,G_LEA,       RG_##reg,FU_NO,        \
_Un(     C,    R,    NONE ),       V_UHALFWORDCONST1,G_MOVE_UI,  RG_##reg,FU_NO,        \
_Un(     C,    R,    NONE ),       V_NO,           R_CONSTLOAD,  RG_##reg,FU_NO,        \
_Un(     M,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_,     FU_NO,        \
_Un(     ANY,  ANY,  NONE ),       V_NO,           G_UNKNOWN,    RG_##reg##_NEED,FU_NO, \
};

MOVE_TABLE( Move1, BYTE,  G_LOAD, G_STORE );
MOVE_TABLE( Move2, WORD,  G_LOAD, G_STORE );
MOVE_TABLE( Move4, DWORD, G_LOAD, G_STORE );

opcode_entry    Move8[] = {
/**************************/
/*       op1   res   eq            verify          gen           reg      fu */
_Un(     ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      G_NO,         RG_,     FU_NO,
_UnPP(   M,    M,    NONE  ),      NVI(V_SAME_LOCN),G_NO,        RG_,     FU_NO,
_Un(     ANY,  ANY,  NONE ),       V_NO,           R_SPLITMOVE,  RG_,     FU_NO,
};

#define BINARY_TABLE( name, reg ) \
opcode_entry    name[] = {                                                              \
/***************************/                                                           \
/*       op1   op2   res   eq      verify          gen           reg      fu */         \
_Bin(    R,    R,    R,    NONE ), V_NO,           G_BINARY,     RG_##reg, FU_ALU,      \
_Bin(    R,    C,    R,    NONE ), V_HALFWORDCONST2,G_BINARY_IMM, RG_##reg, FU_ALU,     \
_Bin(    R,    C,    R,    NONE ), V_NO,           R_MOVOP2TEMP, RG_##reg, FU_NO,       \
_Bin(    C,    C,    R,    NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg, FU_NO,       \
_Bin(    C,    ANY,  R,    NONE ), V_SYMMETRIC,    R_SWAPOPS,    RG_##reg, FU_NO,       \
_Bin(    C,    ANY,  R,    NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg, FU_NO,       \
_Bin(    M,    ANY,  ANY,  NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg, FU_NO,       \
_Bin(    ANY,  M,    ANY,  NONE ), V_NO,           R_MOVOP2TEMP, RG_##reg, FU_NO,       \
_Bin(    ANY,  ANY,  M,    NONE ), V_NO,           R_MOVRESTEMP, RG_##reg, FU_NO,       \
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_##reg##_NEED, FU_NO,\
};

BINARY_TABLE( Binary1, BYTE  );
BINARY_TABLE( Binary2, WORD  );
BINARY_TABLE( Binary4, DWORD );

#define U_BINARY_TABLE( name, reg ) \
opcode_entry    name[] = {                                                              \
/***************************/                                                           \
/*       op1   op2   res   eq      verify          gen           reg      fu */         \
_Bin(    R,    R,    R,    NONE ), V_NO,           G_BINARYS,    RG_##reg, FU_ALU,      \
_Bin(    R,    C,    R,    NONE ), V_UHALFWORDCONST2,G_BINARYS_IMM,RG_##reg, FU_ALU,    \
_Bin(    R,    C,    R,    NONE ), V_NO,           R_MOVOP2TEMP, RG_##reg, FU_NO,       \
_Bin(    C,    C,    R,    NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg, FU_NO,       \
_Bin(    C,    ANY,  R,    NONE ), V_SYMMETRIC,    R_SWAPOPS,    RG_##reg, FU_NO,       \
_Bin(    C,    ANY,  R,    NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg, FU_NO,       \
_Bin(    M,    ANY,  ANY,  NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg, FU_NO,       \
_Bin(    ANY,  M,    ANY,  NONE ), V_NO,           R_MOVOP2TEMP, RG_##reg, FU_NO,       \
_Bin(    ANY,  ANY,  M,    NONE ), V_NO,           R_MOVRESTEMP, RG_##reg, FU_NO,       \
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_##reg##_NEED, FU_NO,\
};

U_BINARY_TABLE( UBinary1, BYTE  );
U_BINARY_TABLE( UBinary2, WORD  );
U_BINARY_TABLE( UBinary4, DWORD );

#define N_BINARY_TABLE( name, reg ) \
opcode_entry    name[] = {                                                              \
/***************************/                                                           \
/*       op1   op2   res   eq      verify          gen           reg      fu */         \
_Bin(    R,    R,    R,    NONE ), V_NO,           G_BINARY,     RG_##reg, FU_ALU,      \
_Bin(    R,    C,    R,    NONE ), V_NO,           R_MOVOP2TEMP, RG_##reg, FU_NO,       \
_Bin(    C,    C,    R,    NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg, FU_NO,       \
_Bin(    C,    ANY,  R,    NONE ), V_SYMMETRIC,    R_SWAPOPS,    RG_##reg, FU_NO,       \
_Bin(    C,    ANY,  R,    NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg, FU_NO,       \
_Bin(    M,    ANY,  ANY,  NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg, FU_NO,       \
_Bin(    ANY,  M,    ANY,  NONE ), V_NO,           R_MOVOP2TEMP, RG_##reg, FU_NO,       \
_Bin(    ANY,  ANY,  M,    NONE ), V_NO,           R_MOVRESTEMP, RG_##reg, FU_NO,       \
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_##reg##_NEED, FU_NO,\
};

N_BINARY_TABLE( NBinary1, BYTE  );
N_BINARY_TABLE( NBinary2, WORD  );
N_BINARY_TABLE( NBinary4, DWORD );

static  opcode_entry    Binary8[] = {
/***********************************/
/*       op1   op2   res  eq      verify      gen           reg         fu*/
_Bin(   ANY,  ANY,  ANY, NONE ),  V_NO,       R_SPLITOP,    RG_QWORD,   FU_NO,
};

opcode_entry    Push[] = {
/************************/
/*       op1   op2   res           verify          gen           reg      fu */
_Un(     ANY,  ANY,  NONE ),       V_NO,           R_PUSHTOMOV,  RG_DWORD, FU_NO,
};

opcode_entry    Pop[] = {
/***********************/
/*       op1   op2   res           verify          gen           reg      fu */
_Un(     ANY,  ANY,  NONE ),       V_NO,           R_POPTOMOV,   RG_DWORD, FU_NO,
};

opcode_entry    Un1[] = {
/***********************/
/*      op1   res   eq             verify          gen           reg           fu */
_Un(    R,    R,    NONE ),        V_NO,           G_UNARY,      RG_BYTE,      FU_ALU,
_Un(    C|M,  ANY,  NONE ),        V_NO,           R_MOVOP1TEMP, RG_BYTE,      FU_NO,
_Un(    ANY,  M,    NONE ),        V_NO,           R_MOVRESTEMP, RG_BYTE,      FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_BYTE_NEED, FU_NO,
};

opcode_entry    Un2[] = {
/***********************/
/*      op1   res   eq             verify          gen           reg           fu */
_Un(    R,    R,    NONE ),        V_NO,           G_UNARY,      RG_WORD,      FU_ALU,
_Un(    C|M,  ANY,  NONE ),        V_NO,           R_MOVOP1TEMP, RG_WORD,      FU_NO,
_Un(    ANY,  M,    NONE ),        V_NO,           R_MOVRESTEMP, RG_WORD,      FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_WORD_NEED, FU_NO,
};

opcode_entry    Un4[] = {
/***********************/
/*      op1   res   eq             verify          gen           reg           fu */
_Un(    R,    R,    NONE ),        V_NO,           G_UNARY,      RG_DWORD,      FU_ALU,
_Un(    C|M,  ANY,  NONE ),        V_NO,           R_MOVOP1TEMP, RG_DWORD,      FU_NO,
_Un(    ANY,  M,    NONE ),        V_NO,           R_MOVRESTEMP, RG_DWORD,      FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_DWORD_NEED, FU_NO,
};

opcode_entry    VaStart[] = {
/***************************/
/*      op1   res   eq             verify          gen           reg           fu */
_Un(    R,    ANY,  NONE ),        V_NO,           G_VASTART,    RG_DWORD,      FU_ALU,
_Un(    M,    ANY,  NONE ),        V_NO,           R_MOVOP1TEMP, RG_DWORD,      FU_ALU,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_DWORD_NEED, FU_NO,
};

opcode_entry    MoveF[] = {
/*************************/
/*       op1   res   eq            verify          gen           reg      fu */
_Un(     ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      G_NO,         RG_,     FU_NO,
_Un(     M,    M,    NONE ),       V_SAME_LOCN,    G_NO,         RG_,     FU_NO,
_Un(     R,    R,    NONE ),       V_NO,           G_MOVE_FP,    RG_FLOAT,FU_NO,
_Un(     R,    M,    NONE ),       V_NO,           G_STORE,      RG_FLOAT,FU_NO,
_Un(     M,    R,    NONE ),       V_NO,           G_LOAD,       RG_FLOAT,FU_NO,
_Un(     C,    ANY,  NONE ),       V_NO,           R_FORCEOP1CMEM,RG_,    FU_NO,
_Un(     M,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_,     FU_NO,
_Un(     ANY,  ANY,  NONE ),       V_NO,           G_UNKNOWN,    RG_FLOAT_NEED,FU_NO,
};

opcode_entry    FloatBinary[] = {
/*******************************/
/*       op1   op2   res   eq      verify          gen           reg      fu */
_Bin(    R,    R,    R,    NONE ), V_NO,           G_BINARY_FP,  RG_FLOAT,FU_ALU,
_Bin(    C,    ANY,  ANY,  NONE ), V_NO,           R_FORCEOP1CMEM,RG_FLOAT, FU_NO,
_Bin(    ANY,  C,    ANY,  NONE ), V_NO,           R_FORCEOP2CMEM,RG_FLOAT, FU_NO,
_Bin(    M,    ANY,  ANY,  NONE ), V_NO,           R_MOVOP1TEMP, RG_FLOAT, FU_NO,
_Bin(    ANY,  M,    ANY,  NONE ), V_NO,           R_MOVOP2TEMP, RG_FLOAT, FU_NO,
_Bin(    ANY,  ANY,  M,    NONE ), V_NO,           R_MOVRESTEMP, RG_FLOAT, FU_NO,
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_FLOAT_NEED, FU_NO,
};


opcode_entry    DoNop[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen           reg      fu */
_BinPP( ANY,  ANY,  ANY,  NONE ),  V_NO,           G_NO,         RG_,     FU_NO,
};

opcode_entry    Set4[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg      fu */
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,           R_DOSET,      RG_,     FU_ALU,
};

opcode_entry    Test4[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen           reg      fu */
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,           R_DOTEST,     RG_,     FU_ALU,
};

opcode_entry    Cmp4[] = {
/************************/
/*       op1   op2       verify          gen             reg fu*/
_Side(  R,    R ),      V_NO,           G_CMP,          RG_DWORD,FU_NO,
_Side(  R,    C ),      V_HALFWORDCONST2,G_CMP_I,       RG_DWORD,FU_NO,
_Side(  R,    C ),      V_NO,           R_MOVOP2TEMP,   RG_DWORD,FU_NO,
_Side(  C,    C ),      V_NO,           R_MOVOP1TEMP,   RG_DWORD,FU_NO,
_Side(  C,    R ),      V_NO,           R_SWAPCMP,      RG_DWORD,FU_NO,
_Side(  M,    ANY ),    V_NO,           R_MOVOP1TEMP,   RG_DWORD,FU_NO,
_Side(  ANY,  M ),      V_NO,           R_MOVOP2TEMP,   RG_DWORD,FU_NO,
_Side(  ANY,  ANY ),    V_NO,           G_UNKNOWN,      RG_DWORD_NEED,FU_NO,
};

opcode_entry    CmpF[] = {
/************************/
/*       op1   op2       verify          gen             reg fu*/
_Side(  R,    R ),      V_NO,           G_CMP_FP,       RG_FLOAT,FU_NO,
_Side(  R,    C ),      V_NO,           R_FORCEOP2CMEM, RG_FLOAT,FU_NO,
_Side(  C,    R ),      V_NO,           R_SWAPCMP,      RG_FLOAT,FU_NO,
_Side(  M,    ANY ),    V_NO,           R_MOVOP1TEMP,   RG_FLOAT,FU_NO,
_Side(  ANY,  M ),      V_NO,           R_MOVOP2TEMP,   RG_FLOAT,FU_NO,
_Side(  ANY,  ANY ),    V_NO,           G_UNKNOWN,      RG_FLOAT_NEED,FU_NO,
};

opcode_entry    Call[] = {
/************************/
/*       op    op2,  res   eq          verify          gen             reg fu*/
_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,            G_CALL,         RG_,FU_NO,
};

opcode_entry    CallI[] = {
/*************************/
/*       op    op2,  res   eq          verify          gen             reg fu*/
_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,            G_CALLI,        RG_,FU_NO,
};

opcode_entry    Rtn[] = {
/***********************/
/*       op    op2,  res   eq          verify          gen             reg fu*/
_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,            R_MAKECALL,      RG_,FU_NO,
};

opcode_entry    Promote[] = {
/***************************/
/*       op    op2,  res   eq          verify          gen             reg fu*/
_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,            R_BIN2INT,      RG_,FU_NO,
};

opcode_entry    Mod4[] = {
/************************/
/*       op    op2,  res   eq          verify          gen             reg fu*/
_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,            R_MOD2DIV,      RG_,FU_NO,
};


static  opcode_entry    *OpcodeList[] = {
        StubUnary,              /* NO   */
        StubUnary,              /* NYI  */
        Binary1,                /* BIN1 */
        Binary2,                /* BIN2 */
        Binary4,                /* BIN4 */
        Binary8,                /* BIN8 */
        UBinary1,               /* UBIN1 */
        UBinary2,               /* UBIN2 */
        UBinary4,               /* UBIN4 */
        NBinary1,               /* NBIN1 */
        NBinary2,               /* NBIN2 */
        NBinary4,               /* NBIN4 */
        Un1,                    /* UN1  */
        Un2,                    /* UN2  */
        Un4,                    /* UN4  */
        StubUnary,              /* UN8  */
        Move1,                  /* MOV1 */
        Move2,                  /* MOV2 */
        Move4,                  /* MOV4 */
        Move8,                  /* MOV8 */
        MoveXX,                 /* MOVXX */
        Conv,                   /* CONV */
        Conv,                   /* FCONV */
        Call,                   /* CALL */
        CallI,                  /* CALLI */
        Push,                   /* PUSH */
        Pop,                    /* POP */
        LoadAddr2,              /* LA2 */
        LoadAddr4,              /* LA4 */
        LoadAddr8,              /* LA8 */
        Cmp4,                   /* CMP4 */
        StubBinary,             /* CMP8 */
        Test4,                  /* TEST4 */
        StubBinary,             /* TEST8 */
        Set4,                   /* SET4 */
        StubBinary,             /* SET8 */
        Binary4,                /* ZAP */
        Binary4,                /* EXT4 */
        FloatBinary,            /* FBINS */
        FloatBinary,            /* FBIND */
        MoveF,                  /* MOVS */
        MoveF,                  /* MOVD */
        CmpF,                   /* CMPS */
        CmpF,                   /* CMPD */
        Rtn,                    /* RTN  */
        NegF,                   /* NEGF */
        Promote,                /* PROM */
        VaStart,                /* VASTR */
        Mod4,                   /* MOD4 */
        DoNop,                  /* DONOTHING */
        NULL };                 /* BAD*/

extern  opcode_entry    *OpcodeTable( table_def i )
/*************************************************/
{
    return( OpcodeList[ i ] );
}
