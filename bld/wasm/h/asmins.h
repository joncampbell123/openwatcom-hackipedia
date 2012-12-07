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

#ifndef ASMINS_H
#define ASMINS_H

#ifdef _M_I86

#define ASMFAR far

#else

#define ASMFAR

#endif

#include "asmopnds.h"
#include "asmsym.h"
#include "asmops.h"

enum prefix_reg {
    PREFIX_EMPTY = EMPTY,
    PREFIX_ES = 0x26,
    PREFIX_CS = 0x2E,
    PREFIX_SS = 0x36,
    PREFIX_DS = 0x3E,
    PREFIX_FS = 0x64,
    PREFIX_GS = 0x65
};

#if defined( _STANDALONE_ )
    struct asm_ins {
        unsigned short      token;                  /* T_ADD, etc */
        unsigned            allowed_prefix  : 4;    /* allowed prefix */
        unsigned            byte1_info      : 4;    /* flags for 1st byte */
        unsigned            rm_info         : 2;    /* info on r/m byte */
        unsigned            opnd_type_3rd   : 4;    /* info on 3rd operand */
        unsigned            opnd_dir        : 1;    /* operand direction */
        enum asm_cpu        cpu;                    /* CPU type */
        OPNDTYPE            opnd_type[2];           /* asm_opnds */
        unsigned char       opcode;                 /* opcode byte */
        unsigned char       rm_byte;                /* mod_rm_byte */
    };
#else
    struct asm_ins {
        unsigned            token           : 10;   /* T_ADD, etc */
        unsigned            allowed_prefix  : 4;    /* allowed prefix */
        unsigned            byte1_info      : 4;    /* flags for 1st byte */
        unsigned            rm_info         : 2;    /* info on r/m byte */
        unsigned            opnd_type_3rd   : 4;    /* info on 3rd operand */
        unsigned            opnd_dir        : 1;    /* operand direction */
        enum asm_cpu        cpu;                    /* CPU type */
        OPNDTYPE            opnd_type[2];           /* asm_opnds */
        unsigned char       opcode;                 /* opcode byte */
        unsigned char       rm_byte;                /* mod_rm_byte */
    };
#endif

struct asm_code {
    struct {
        signed short    ins;           // prefix before instruction, e.g. lock
        enum prefix_reg seg;           // segment register override
        unsigned        adrsiz:1;      // address size prefix
        unsigned        opsiz:1;       // operand size prefix
    } prefix;
    memtype         mem_type;       // byte / word / etc. NOT near/far
    long            data[3];
    struct {
#if defined( _STANDALONE_ )
        unsigned short      token;
        enum asm_cpu        cpu;
        OPNDTYPE            opnd_type[3];
        unsigned char       opcode;
        unsigned char       rm_byte;
#else
        unsigned            token           : 10;
        enum asm_cpu        cpu;
        OPNDTYPE            opnd_type[3];
        unsigned char       opcode;
        unsigned char       rm_byte;
#endif
    } info;
    signed char     extended_ins;
    unsigned char   sib;
    unsigned        use32:1;
    unsigned        indirect:1;     // CALL/JMP indirect jump
    unsigned        mem_type_fixed:1;
};

#define NO_PREFIX   0x00
#define LOCK        0x01
#define REP         0x02
#define REPxx       0x03
#define FWAIT       0x04
#define NO_FWAIT    0x05
#define K3D_SET     0x06   // AMD 3DNow instruction set
#define MMX_SET     0x07   // MMX instruction set
#define SSE_SET     0x08   // XMM instruction set
#define SSE2_SET    0x09   // XMM instruction set
#define SSE3_SET    0x0A   // XMM instruction set

#define F_16        0x1
#define F_32        0x2
#define F_0F        0x3
#define F_F3        0x4
#define F_0F0F      0x5    // AMD 3DNow prefix
#define F_660F      0x6    // SSEx prefix 1
#define F_F20F      0x7    // SSEx prefix 2
#define F_F30F      0x8    // SSEx prefix 3

#define no_RM       0x1
#define no_WDS      0x2
#define R_in_OP     0x3

#define IREG    1       /* indicates INDEX register for an OP_REGISTER entry*/

/* Note on the byte_1_info
   10 ( + F_0F ) -> the first byte is 0x0F, follow by opcode and rm_byte
   01 ( + F_16 ) -> the first byte is OPSIZ prefix if in use32 segment
   11 ( + F_32 ) -> the first byte is OPSIZ prefix if in use16 segment
   00            -> the first byte is opcode, follow by rm_byte      */

/* Note on the rm_info:
   01 ( + no_RM   ) -> no rm_byte
   10 ( + no_WDS  ) -> has rm_byte, but w-bit, d-bit, s-bit of opcode are absent
   11 ( + R_in_OP ) -> no rm_byte, reg field is included in opcode
   00               -> has rm_byte with w-, d- and/or s-bit in opcode  */

/* NOTE: The order of table is IMPORTANT !! */
/* OP_A should put before OP_R16 & OP_R
   OP_R16   "     "    "    OP_R
   OP_I8    "     "    "    OP_I
   OP_M ( without extension ) should follow OP_M_x
   OP_R ( without extension ) should follow OP_Rx
   OP_I ( without extension ) should follow OP_Ix  */

extern const struct asm_ins ASMFAR  AsmOpTable[];
extern const struct AsmCodeName     AsmOpcode[];
extern const char                   AsmChars[];

#if defined( _STANDALONE_ )

extern void     find_frame( struct asm_sym *sym );
extern void     GetInsString( asm_token, char *, int );

#endif

extern int      check_override( int *i );
extern int      OperandSize( OPNDTYPE opnd );
extern int      InRange( unsigned long val, unsigned bytes );
extern int      cpu_directive( int i );
extern int      AsmParse( void );
extern int      NextArrayElement( void );
extern int      data_init( int, int );

#endif
