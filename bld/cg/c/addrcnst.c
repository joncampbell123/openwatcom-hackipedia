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


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "addrcnst.h"

extern    block         *HeadBlock;

extern  name            *AllocAddrConst(name*,int,constant_class,type_class_def);
extern  name            *AllocTemp(type_class_def);
extern  void            PrefixIns(instruction*,instruction*);
extern  instruction     *MakeUnary(opcode_defs,name*,name*,type_class_def);


extern  void    MakeMovAddrConsts( void ) {
/*****************************************/

    block       *blk;
    instruction *ins;
    name        *op;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            if( ins->head.opcode == OP_LA ) {
                op = ins->operands[ 0 ];
                if( op->n.class == N_TEMP ) {
                    op->v.usage |= USE_IN_ANOTHER_BLOCK;
                    op = AllocAddrConst(op,0, CONS_TEMP_ADDR, ins->type_class);
                    ins->head.opcode = OP_MOV;
                    ins->operands[ 0 ] = op;
                }
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
}


extern  void    KillMovAddrConsts( void ) {
/*****************************************/

    block               *blk;
    instruction         *ins;
    instruction         *new_ins;
    name                *op;
    name                *new_op;
    type_class_def      class;
    int                 i;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            i = ins->num_operands;
            while( --i >= 0 ) {
                op = ins->operands[ i ];
                if( op->n.class == N_CONSTANT
                 && op->c.const_type == CONS_TEMP_ADDR ) {
                    class = _OpClass( ins );
                    new_op = AllocTemp( class );
                    new_ins = MakeUnary( OP_LA, op->c.value, new_op, class );
                    ins->operands[ i ] = new_op;
                    PrefixIns( ins, new_ins );
                }
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
}
