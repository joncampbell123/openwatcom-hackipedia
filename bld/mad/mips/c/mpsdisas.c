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
* Description:  MIPS instruction decoding.
*
****************************************************************************/


#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "walloca.h"
#include "mips.h"
#include "mpstypes.h"
#include "madregs.h"

static dis_handle DH;

mad_status DisasmInit()
{
    bool        swap_bytes;

#ifdef __BIG_ENDIAN__
    swap_bytes = TRUE;
#else
    swap_bytes = FALSE;
#endif
    if( DisInit( DISCPU_mips, &DH, swap_bytes ) != DR_OK ) {
        return( MS_ERR | MS_FAIL );
    }
    return( MS_OK );
}

void DisasmFini()
{
    DisFini( &DH );
}

dis_return DisCliGetData( void *d, unsigned off, unsigned size, void *data )
{
    mad_disasm_data     *dd = d;
    address             addr;

    addr = dd->addr;
    addr.mach.offset += off;
    if( MCReadMem( addr, size, data ) == 0 ) return( DR_FAIL );
    return( DR_OK );
}

unsigned DisCliValueString( void *d, dis_dec_ins *ins, unsigned op, char *buff )
{
    mad_disasm_data     *dd = d;
    char                *p;
    unsigned            max;
    mad_type_info       mti;
    address             val;

    p = buff;
    p[0] = '\0';
    val = dd->addr;
    switch( ins->op[op].type & DO_MASK ) {
    case DO_RELATIVE:
        val.mach.offset += ins->op[op].value;
        //NYI: 64 bit
        MCAddrToString( val, MIPST_N32_PTR, MLK_CODE, 40, p );
        break;
    case DO_ABSOLUTE:
        if( dd->ins.type == DI_MIPS_J || dd->ins.type == DI_MIPS_JAL ) {
            // Handle j/jal as having pointer operand to show target symbol
            val.mach.offset = ins->op[op].value;
            MCAddrToString( val, MIPST_N32_PTR, MLK_CODE, 40, p );
            break;
        }
        // Fall through
    case DO_IMMED:
    case DO_MEMORY_ABS:
        MCTypeInfoForHost( MTK_INTEGER, -(int)sizeof( ins->op[0].value ), &mti );
        max = 40;
        MCTypeToString( dd->radix, &mti, &ins->op[op].value, &max, p );
        break;
    }
    return( strlen( buff ) );
}

unsigned DIGENTRY MIDisasmDataSize( void )
{
    return( sizeof( mad_disasm_data ) );
}

unsigned DIGENTRY MIDisasmNameMax( void )
{
    return( DisInsNameMax( &DH ) );
}

mad_status DisasmOne( mad_disasm_data *dd, address *a, int adj )
{
    addr_off    new;

    dd->addr = *a;
    new = dd->addr.mach.offset + adj * (int)sizeof( unsigned_32 );
    if( (adj < 0 && new > dd->addr.mach.offset)
     || (adj > 0 && new < dd->addr.mach.offset) ) {
        return( MS_FAIL );
    }
    dd->addr.mach.offset = new;
    DisDecodeInit( &DH, &dd->ins );
    if( DisDecode( &DH, dd, &dd->ins ) != DR_OK ) {
        return( MS_ERR | MS_FAIL );
    }
    a->mach.offset = dd->addr.mach.offset + sizeof( unsigned_32 );
    return( MS_OK );
}

mad_status DIGENTRY MIDisasm( mad_disasm_data *dd, address *a, int adj )
{
    return( DisasmOne( dd, a, adj ) );
}

unsigned DIGENTRY MIDisasmFormat( mad_disasm_data *dd, mad_disasm_piece dp, unsigned radix, unsigned max, char *buff )
{
    char                nbuff[20];
    char                obuff[256];
    char                *np;
    char                *op;
    unsigned            nlen;
    unsigned            olen;
    unsigned            len;
    dis_format_flags    ff;

    nbuff[0] = '\0';
    obuff[0] = '\0';
    np = (dp & MDP_INSTRUCTION) ? nbuff : NULL;
    op = (dp & MDP_OPERANDS)    ? obuff : NULL;
    ff = DFF_NONE;
    if( MADState->disasm_state & DT_PSUEDO_OPS ) ff |= DFF_PSEUDO;
    if( MADState->disasm_state & DT_UPPER ) ff |= DFF_INS_UP | DFF_REG_UP;
    if( MADState->reg_state[CPU_REG_SET] & CT_SYMBOLIC_NAMES ) {
        ff |= DFF_SYMBOLIC_REG;
    }
    dd->radix = radix;
    if( DisFormat( &DH, dd, &dd->ins, ff, np, op ) != DR_OK ) {
        return( 0 );
    }
    olen = strlen( obuff );
    nlen = strlen( nbuff );
    if( dp == MDP_ALL ) nbuff[ nlen++ ] = ' ';
    len = nlen + olen;
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        if( nlen > max ) nlen = max;
        memcpy( buff, nbuff, nlen );
        buff += nlen;
        max -= nlen;
        if( olen > max ) olen = max;
        memcpy( buff, obuff, olen );
        buff[max] = '\0';
    }
    return( len );
}

unsigned DIGENTRY MIDisasmInsSize( mad_disasm_data *dd )
{
    return( sizeof( unsigned_32 ) );
}

mad_status DIGENTRY MIDisasmInsUndoable( mad_disasm_data *dd )
{
    switch( dd->ins.type ) {
    case DI_MIPS_SYSCALL:
    case DI_MIPS_ERET:
    case DI_MIPS_BREAK:
        return( MS_FAIL );
    }
    return( MS_OK );
}

#define SKIP_ASM_REGS
const unsigned_16   RegTrans[] = {
#undef regpick
#define regpick( e, n ) offsetof( mad_registers, mips.e ),
#include "regmips.h"
};

// Map left/right accesses to plain word/dword accesses
#define MIPST_WORDL     MIPST_WORD
#define MIPST_WORDR     MIPST_WORD
#define MIPST_DWORDR    MIPST_DWORD
#define MIPST_DWORDL    MIPST_DWORD
#define MIPST_SFLOAT    MIPST_FLOAT
#define MIPST_DFLOAT    MIPST_DOUBLE

static const mad_type_handle RefTrans[] = {
#undef refpick
#define refpick( e, n ) MIPST_##e,
#include "refmips.h"
};

static int Cond1( mad_disasm_data *dd, const mad_registers *mr, unsigned condition )
{
    const unsigned_64   *rs;
    int                 cmp;

    rs = &TRANS_REG( mr, dd->ins.op[0].base );
    if( rs->u._32[I64LO32] & 0x80000000 ) {     // check sign bit
        cmp = -1;
    } else if( rs->u._32[I64LO32] != 0 ) {
        cmp = +1;
    } else {
        cmp = 0;
    }

    switch( condition ) {
    case DI_MIPS_BGEZ:
    case DI_MIPS_BGEZL:
    case DI_MIPS_BGEZAL:
    case DI_MIPS_BGEZALL:
        if( cmp >= 0 ) break;
        return( 0 );
    case DI_MIPS_BGTZ:
    case DI_MIPS_BGTZL:
        if( cmp > 0 ) break;
        return( 0 );
    case DI_MIPS_BLEZ:
    case DI_MIPS_BLEZL:
        if( cmp <= 0 ) break;
        return( 0 );
    case DI_MIPS_BLTZ:
    case DI_MIPS_BLTZL:
    case DI_MIPS_BLTZAL:
    case DI_MIPS_BLTZALL:
        if( cmp < 0 ) break;
        return( 0 );
    default:
        break;
    }
    return( 1 );
}

static int Cond2( mad_disasm_data *dd, const mad_registers *mr, unsigned condition )
{
    const unsigned_64   *rs;
    const unsigned_64   *rt;

    rs = &TRANS_REG( mr, dd->ins.op[0].base );
    rt = &TRANS_REG( mr, dd->ins.op[1].base );
    switch( condition ) {
    case DI_MIPS_BEQ:
    case DI_MIPS_BEQL:
        if( rs->u._32[I64LO32] == rt->u._32[I64LO32] ) break;
        return( 0 );
    case DI_MIPS_BNE:
    case DI_MIPS_BNEL:
        if( rs->u._32[I64LO32] != rt->u._32[I64LO32] ) break;
        return( 0 );
    default:
        break;
    }
    return( 1 );
}

mad_disasm_control DisasmControl( mad_disasm_data *dd, mad_registers const *mr )
{
    mad_disasm_control  c;
    bool                is_call = FALSE;

    switch( dd->ins.type ) {
    case DI_MIPS_J:
        return( MDC_JUMP | MDC_TAKEN );
    case DI_MIPS_JAL:
    case DI_MIPS_JALR:
        return( MDC_CALL | MDC_TAKEN );
    case DI_MIPS_JR:
        if( dd->ins.op[0].base == DR_MIPS_r31 ) {
            c = MDC_RET;    //  'jr ra'
        } else {
            c = MDC_JUMP;
        }
        return( c | MDC_TAKEN );
    case DI_MIPS_ERET:
        return( MDC_SYSRET | MDC_TAKEN );
    case DI_MIPS_SYSCALL:
        return( MDC_SYSCALL | MDC_TAKEN );
    case DI_MIPS_BEQ:
    case DI_MIPS_BEQL:
    case DI_MIPS_BNE:
    case DI_MIPS_BNEL:
        if( !Cond2( dd, mr, dd->ins.type ) )
            return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[2].value < 0
            ? (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_BACK)
            : (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
    case DI_MIPS_BGEZAL:
    case DI_MIPS_BGEZALL:
    case DI_MIPS_BLTZAL:
    case DI_MIPS_BLTZALL:
        is_call = TRUE;
        /* fall through */
    case DI_MIPS_BGEZ:
    case DI_MIPS_BGEZL:
    case DI_MIPS_BGTZ:
    case DI_MIPS_BGTZL:
    case DI_MIPS_BLEZ:
    case DI_MIPS_BLEZL:
    case DI_MIPS_BLTZ:
    case DI_MIPS_BLTZL:
        if( is_call )
            c = MDC_CALL;
        else
            c = MDC_JUMP;
        if( !Cond1( dd, mr, dd->ins.type ) )
            return( c | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[1].value < 0
            ? (c | MDC_CONDITIONAL | MDC_TAKEN_BACK)
            : (c | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
//    case DI_MIPS_SLT:
//        if( !Cond( dd, mr, DI_MIPS_BEQ ) )
//            return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
//        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_MIPS_TEQ:
    case DI_MIPS_TEQI:
    case DI_MIPS_TGE:
    case DI_MIPS_TGEI:
    case DI_MIPS_TGEU:
    case DI_MIPS_TLT:
    case DI_MIPS_TLTI:
    case DI_MIPS_TLTU:
    case DI_MIPS_TNE:
    case DI_MIPS_TNEI:
        c = MDC_SYSRET | MDC_CONDITIONAL;
//        if( TrapTest( dd, mr ) & dd->ins.op[0].value ) {
//            c |= MDC_TAKEN;
//        }
        return( c );
    default:
        break;
    }
    return( MDC_OPER | MDC_TAKEN );
}

mad_disasm_control DIGENTRY MIDisasmControl( mad_disasm_data *dd, mad_registers const *mr )
{
    return( DisasmControl( dd, mr ) );
}

mad_status DIGENTRY MIDisasmInsNext( mad_disasm_data *dd, mad_registers const *mr, address *next )
{
    mad_disasm_control  dc;
    addr_off            new;
    unsigned            op;
    const unsigned_64   *reg;

    memset( next, 0, sizeof( *next ) );
    next->mach.offset = mr->mips.pc.u._32[I64LO32] + sizeof( unsigned_32 );
    dc = DisasmControl( dd, mr );
    if( (dc & MDC_TAKEN_MASK) == MDC_TAKEN_NOT ) {
        return( MS_OK );
    }
    switch( dc & MDC_TYPE_MASK ) {
    case MDC_JUMP:
    case MDC_CALL:
    case MDC_RET:
        if( dd->ins.type == DI_MIPS_J || dd->ins.type == DI_MIPS_JAL || dd->ins.type == DI_MIPS_JR )
            op = 0; // Target is the first operand, for other instructions it's the second operand
        else
            op = 1;
        new = dd->ins.op[op].value;
        if( dd->ins.op[op].type == DO_RELATIVE ) {
            new += mr->mips.pc.u._32[I64LO32];
        }
        if( dd->ins.op[op].base != DR_NONE ) {
            reg = &TRANS_REG( mr, dd->ins.op[op].base );
            new += reg->u._32[0];
        }
        next->mach.offset = new;
    }
    return( MS_OK );
}

walk_result DIGENTRY MIDisasmMemRefWalk( mad_disasm_data *dd, MI_MEMREF_WALKER *wk, mad_registers const *mr, void *d )
{
    address             a;
    unsigned            i;
    walk_result         wr;
    mad_memref_kind     mmk;

    if( dd->ins.type >= DI_MIPS_LB && dd->ins.type <= DI_MIPS_LWC1 ) {
        mmk = MMK_READ;
    } else if( dd->ins.type >= DI_MIPS_SB && dd->ins.type <= DI_MIPS_SWC1 ) {
        mmk = MMK_WRITE;
    } else {
        return( WR_CONTINUE );
    }
    a = dd->addr;
    for( i = 0; i < dd->ins.num_ops; ++i ) {
        if( dd->ins.op[i].type == DO_MEMORY_ABS ) {
            a.mach.offset = dd->ins.op[i].value;
            if( dd->ins.op[i].base != DR_MIPS_r0 ) {
                a.mach.offset += TRANS_REG( mr, dd->ins.op[i].base ).u._32[I64LO32];
            }
            mmk &= (MMK_READ | MMK_WRITE);
            if( dd->ins.op[i].base == DR_MIPS_sp ) {
                mmk |= MMK_VOLATILE;
            }
            wr = wk( a, RefTrans[dd->ins.op[i].ref_type - DRT_MIPS_FIRST], mmk, d );
            return( wr );
        } else if( dd->ins.op[i].extra & PE_XFORM ) {
            a.mach.offset = 0;
            if( dd->ins.op[i].base != DR_MIPS_r0 ) {
                a.mach.offset += TRANS_REG( mr, dd->ins.op[i].base ).u._32[I64LO32];
            }
            a.mach.offset += TRANS_REG( mr, dd->ins.op[i+1].base ).u._32[I64LO32];
            mmk &= (MMK_READ | MMK_WRITE);
            if( dd->ins.op[i].base == DR_MIPS_sp || dd->ins.op[i+1].base == DR_MIPS_sp ) {
                mmk |= MMK_VOLATILE;
            }
            wr = wk( a, RefTrans[dd->ins.op[i].ref_type - DRT_MIPS_FIRST], mmk, d );
            return( wr );
        }
    }
    return( WR_CONTINUE );
}

const mad_toggle_strings *DIGENTRY MIDisasmToggleList( void )
{
    static const mad_toggle_strings list[] = {
        { MSTR_MPSEUDOINS, MSTR_PSEUDOINS, MSTR_RAWINS },
        { MSTR_MUPPER, MSTR_UPPER, MSTR_LOWER },
        { MSTR_NIL, MSTR_NIL, MSTR_NIL }
    };
    return( list );
}

unsigned DIGENTRY MIDisasmToggle( unsigned on, unsigned off )
{
    unsigned    toggle;

    toggle = (on & off);
    MADState->disasm_state ^= toggle;
    MADState->disasm_state |= on & ~toggle;
    MADState->disasm_state &= ~off | toggle;
    return( MADState->disasm_state );
}

mad_status DIGENTRY MIDisasmInspectAddr( char *from, unsigned len, unsigned radix, mad_registers const *mr, address *a )
{
    char        *buff = __alloca( len * 2 );
    char        *to;

    mr = mr;
    to = buff;
    while( len != 0 ) {
        if( *from == '(' ) *to++ = '+';
        *to++ = *from++;
        --len;
    }
    return( MCMemExpr( buff, to - buff, radix, a ) );
}
