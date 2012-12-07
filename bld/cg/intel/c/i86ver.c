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
* Description:  Instruction conditions verification for Intel processors.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "regset.h"
#include "model.h"
#include "vergen.h"
#include "pattern.h"
#include "cfloat.h"


extern  int             NumOperands(instruction*);
extern  bool            OtherVerify(vertype,instruction*,name*,name*,name*);

extern  byte            OptForSize;
extern  hw_reg_set      Low16Reg( hw_reg_set );
extern  type_class_def  RegClass(hw_reg_set);


extern  bool    DoVerify( vertype kind, instruction *ins ) {
/**********************************************************/

    name        *op1 = NULL;
    name        *op2 = NULL;
    name        *result;

    result = ins->result;
    if( ins->num_operands != 0 ) {
        op1 = ins->operands[ 0 ];
        if( ins->num_operands != 1 ) {
            op2 = ins->operands[ 1 ];
        }
    }
    switch( kind ) {
    case V_80186:
         if( _CPULevel( CPU_186 ) ) return( TRUE );
         break;
    case V_80386:
         if( _CPULevel( CPU_386 ) ) return( TRUE );
         break;
    case V_DIFF_TYPES:
        if( op1->n.name_class != result->n.name_class ) return( TRUE );
        break;
    case V_HIGHEQLOW:
        if( op1->c.const_type != CONS_ABSOLUTE ) return( FALSE );
        if(!CFIsI32( op1->c.value ) && !CFIsU32( op1->c.value )) return(FALSE);
        if( ( op1->c.int_value & 0xffff0000 )
         == ( op1->c.int_value << 16 ) ) return( TRUE );
        break;
    case V_OP2HIGH_B_ZERO:
        if( op2->c.const_type == CONS_ABSOLUTE
         && ( op2->c.int_value & 0x0000ff00 ) == 0 ) return( TRUE );
        break;
    case V_OP2LOW_B_FF:
        if( op2->c.const_type == CONS_ABSOLUTE
         && ( ~op2->c.int_value & 0x000000ff ) == 0 ) return( TRUE );
        break;
    case V_OP2LOW_B_ZERO:
        if( op2->c.const_type == CONS_ABSOLUTE
         && ( op2->c.int_value & 0x000000ff ) == 0 ) return( TRUE );
        break;
    case V_OP2LOW_W_FFFF:
        if( op2->c.const_type == CONS_ABSOLUTE
         && ( ~op2->c.int_value & 0x0000ffff ) == 0 ) return( TRUE );
        break;
    case V_LSHONE:
        if( !_CPULevel( CPU_286 ) ) return( FALSE );
        if( ins->head.opcode != OP_LSHIFT ) return( FALSE );
        return( OtherVerify( V_OP2ONE, ins, op1, op2, result ) );
    case V_OP2TWO_SIZE:
        if( OptForSize <= 50 ) return( FALSE );
        return( OtherVerify( V_OP2TWO, ins, op1, op2, result ) );
    case V_BYTESHIFT:
        if( ins->head.opcode != OP_LSHIFT
         && ins->type_class != U4 ) return( FALSE );
        if( op2->c.const_type != CONS_ABSOLUTE ) return( FALSE );
        if( op2->c.int_value != 8 ) return( FALSE );
        if( op1->n.class == N_REGISTER
         && !HW_CEqual( op1->r.reg, HW_ABCD ) ) return( FALSE );
        if( result->n.class == N_REGISTER
         && !HW_CEqual( result->r.reg, HW_ABCD ) ) return( FALSE );
        return( TRUE );
    case V_CYP2SHIFT:
        if( op2->c.const_type != CONS_ABSOLUTE ) return( FALSE );
        if( op2->c.int_value != 8 ) return( FALSE );
        if( _CPULevel( CPU_186 ) ) return( FALSE );
        if( !HW_COvlap( ins->head.live.regs, HW_AX ) ) return( TRUE );
        return( FALSE );
    case V_CYP4SHIFT:
        if( op2->c.const_type != CONS_ABSOLUTE ) return( FALSE );
        if( op2->c.int_value < 16 ) return( FALSE );
        return( TRUE );
    case V_OP2HIGH_B_FF:
        if( op2->c.const_type == CONS_ABSOLUTE
         && ( op2->c.int_value & 0x0000ff00 ) == 0x0000ff00 ) return( TRUE );
        break;
    case V_OP2HIGH_W_FFFF_REG:
        if( op1->n.class == N_REGISTER && HW_Ovlap( op1->r.reg, HW_BP ) ) break;
    case V_OP2HIGH_W_FFFF:
        if( op2->c.const_type == CONS_ABSOLUTE
         && ( op2->c.int_value & 0xffff0000 ) == 0xffff0000 ) return( TRUE );
        break;
    case V_OP2HIGH_W_ZERO_REG:
        if( op1->n.class == N_REGISTER && HW_Ovlap( op1->r.reg, HW_BP ) ) break;
    case V_OP2HIGH_W_ZERO:
        if( op2->c.const_type == CONS_ABSOLUTE
         && ( op2->c.int_value & 0xffff0000 ) == 0 ) return( TRUE );
        break;
    case V_OP2LOW_W_ZERO:
        if( op2->c.const_type == CONS_ABSOLUTE
         && ( op2->c.int_value & 0x0000ffff ) == 0 ) return( TRUE );
        break;
    case V_OP2PTR:
        if( op2->n.name_class == PT || op2->n.name_class == CP ) return( TRUE );
        break;
    case V_LEA_GOOD:
        if( OptForSize > 50 ) return( FALSE );
        /* fall through */
    case V_LEA:
        if( op2->c.const_type != CONS_ABSOLUTE ) return( FALSE );
        switch( ins->head.opcode ) {
#if _TARGET & _TARG_80386
        case OP_MUL:
            switch( op2->c.int_value ) {
            case 3:
            case 5:
            case 9:
                return( TRUE );
            }
            break;
        case OP_LSHIFT:
            if( op1 == result && (_CPULevel( CPU_586 ) && !_CPULevel( CPU_686 )) )
                return( FALSE );
            if( OptForSize > 50 ) return( FALSE );
            switch( op2->c.int_value ) {
            case 1:
            case 2:
            case 3:
                /* If the shift is *not* followed by an add, we're going to
                 * end up with a huge lea instruction which is unlikely to
                 * be any faster. See also GetNextAddConstant() in i86enc32.c.
                 */
                switch( ins->head.next->head.opcode ) {
                case OP_ADD:
                case OP_SUB:
                    if( ins->head.next->operands[0] != ins->result ) break;
                    if( ins->head.next->result != ins->result ) break;
                    if( ins->head.next->operands[1]->n.class != N_CONSTANT ) break;
                    if( ins->head.next->operands[1]->c.const_type != CONS_ABSOLUTE ) break;
                    if( ins->head.next->ins_flags & INS_CC_USED ) break;
                    return( TRUE );
                default:
                    break;
                }
            }
            break;
#endif
        case OP_ADD:
        case OP_SUB:
            if( OptForSize < 50 && !_CPULevel( CPU_286 ) ) return( FALSE );
            return( TRUE );
        default:
            break;
        }
        break;
    case V_SIZE:
        if( OptForSize > 50 ) return( TRUE );
        break;
    case V_DIV_BUG: /* cant do idiv from mem on 80186 (or 8086 for compatibility)*/
        if( !_CPULevel( CPU_286 )
             && ins->type_class != U1 && ins->type_class != U2 ) return( TRUE );
        break;
    case V_OP1SP:
        if( HW_CEqual( op1->r.reg, HW_SP ) ) return( TRUE );
        break;
    case V_WORDREG_AVAIL:
        if( !HW_COvlap( ins->head.live.regs, HW_AX ) ) return( TRUE );
        if( !HW_COvlap( ins->head.live.regs, HW_BX ) ) return( TRUE );
        if( !HW_COvlap( ins->head.live.regs, HW_CX ) ) return( TRUE );
        if( !HW_COvlap( ins->head.live.regs, HW_DX ) ) return( TRUE );
        if( !HW_COvlap( ins->head.live.regs, HW_SI ) ) return( TRUE );
        if( !HW_COvlap( ins->head.live.regs, HW_DI ) ) return( TRUE );
        break;
    case V_AC_BETTER:
        if( op2->c.const_type != CONS_ABSOLUTE ) return( TRUE );
        if( op2->c.int_value > 127 ) return( TRUE );
        if( op2->c.int_value < -128 ) return( TRUE );
        break;
    case V_OP2POW2_286:
        if( !_CPULevel( CPU_286 ) ) return( FALSE );
        return( OtherVerify( V_OP2POW2, ins, op1, op2, result ) );
    case V_OP1ADDR:
    case V_OP1LOC:
        if( _IsTargetModel( INDEXED_GLOBALS ) ) return( FALSE );
        if( op1->n.class == N_MEMORY ) return( TRUE );
        break;
    case V_RESLOC:
        if( _IsTargetModel( INDEXED_GLOBALS ) ) return( FALSE );
        if( result->n.class == N_MEMORY ) return( TRUE );
        break;
    case V_GOOD_CLR:
        if( op1 == result ) return( TRUE );
        /* Always allow cases like ax<-al which is just a simple mov or xor */
        if( op1->n.class == N_REGISTER
            && RegClass( result->r.reg ) == U2
            && HW_Equal( op1->r.reg, Low16Reg( result->r.reg ) ) ) {
            return( TRUE );
        }
        /* On P6 architecture, 'and' will cause a partial register stall with
         * horrible performance implications. Always use movzx.
         */
        if( !_CPULevel( CPU_486 ) || _CPULevel( CPU_686 ) ) break;
        if( OptForSize > 50 ) break;
        if( result->n.class == N_REGISTER ) {
            if( HW_Ovlap( result->r.reg, HW_BP ) ) break;
        }
        return( TRUE );
    case V_CDQ:
        /* On a Pentium, a MOV/SAR sequence is faster because it pairs while
         * CDQ does not. However, on Pentium Pro CDQ is no slower yet smaller.
         * Therefore, generate CDQ except when optimizing for time and the
         * CPU is 586 (on 386/486, CDQ is always better).
         */
        if( _CPULevel( CPU_686 ) || !_CPULevel( CPU_586 ) ) return( TRUE );
        if( OptForSize >= 50 ) return( TRUE );
        break;
    case V_P5_FXCH:
        if( !_CPULevel( CPU_586 ) ) return( FALSE );
        if( OptForSize <= 50 ) return( TRUE );
        break;
    case V_CYP_SEX:
        if( !_CPULevel( CPU_586 ) ) return( FALSE );
        if( OptForSize > 50 ) return( FALSE );
        op1 = ins->operands[0];
        {
            type_length backup;
            type_length new_off;

            backup = WORD_SIZE - op1->n.size;
            switch( op1->n.class ) {
            case N_TEMP:
                return( TRUE );
            case N_MEMORY:
                new_off = op1->v.offset - backup;
                if( new_off >= 0 ) return( TRUE );
                if( ins->num_operands > NumOperands( ins ) ) return( FALSE );
                return( TRUE );
            case N_INDEXED:
                new_off = op1->i.constant - backup;
                if( new_off >= 0 ) return( TRUE );
                if( ins->num_operands > NumOperands( ins ) ) return( FALSE );
                if( op1->i.base == NULL ) return( FALSE );
                return( TRUE );
            default:
                break;
            }
        }
        break;
    case V_NEGATE:
         if( ins->head.opcode == OP_NEGATE ) return( TRUE );
         break;
    case V_CONSTTEMP:
        {
            int                 i;

            for( i = ins->num_operands-1; i >= 0; --i ) {
                op1 = ins->operands[i];
                if( op1->n.class == N_TEMP
                    && (op1->t.temp_flags & CONST_TEMP ) ) return( TRUE );
            }
            return( FALSE );
        }
    case V_SWAP_GOOD_386:
        if( !_CPULevel( CPU_386 ) ) return( FALSE );
        return( OtherVerify( V_SWAP_GOOD, ins, op1, op2, result ) );
    case V_INTCOMP:
        if( ins->result != NULL && ins->type_class != FS ) return( FALSE );
        if( ins->head.opcode == OP_CMP_EQUAL ) return( TRUE );
        if( ins->head.opcode == OP_CMP_NOT_EQUAL ) return( TRUE );
#if _TARGET & _TARG_80386
        // rINTCOMP reductions for 16-bit need work to handle < and >
        // comparisons - not worth it for now - BBB Apr 24, 1995
        if( ins->type_class != FS ) return( FALSE );
        if( ins->operands[ 1 ]->n.class == N_CONSTANT &&
            ins->operands[ 1 ]->c.const_type == CONS_ABSOLUTE &&
            CFTest( ins->operands[ 1 ]->c.value ) > 0 ) return( TRUE );
#endif
        return( FALSE );
    default:
        return( OtherVerify( kind, ins, op1, op2, result ) );
    }
    return( FALSE );
}
