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
* Description:  SIB helper routines.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"

typedef enum {
    MODIFIES_NOTHING    = 0,
    MODIFIES_BASE       = 1,
    MODIFIES_REG        = 2,
} mod_info;

extern  hw_reg_set      Low64Reg( hw_reg_set );
extern  hw_reg_set      High64Reg( hw_reg_set );
extern  bool            IsIndexReg( hw_reg_set, type_class_def, bool );
extern  bool            FoldIntoIndex( instruction * );
extern  void            UpdateLive( instruction *, instruction * );

extern  block           *HeadBlock;

static  bool    InsIsCandidate( instruction *ins ) 
/************************************************/
{
    name    *reg;

    reg = ins->result;
    if( reg == NULL ) return( FALSE );
    if( reg->n.class != N_REGISTER ) return( FALSE );
    if( reg->n.size != WORD_SIZE ) return( FALSE );
    if( ins->operands[ 0 ]->n.class != N_REGISTER ) return( FALSE );
    if( !IsIndexReg( reg->r.reg, WD, 0 ) ) return( FALSE );
    return( TRUE );
}


extern  void    BuildIndex( void ) 
/********************************/
{
    block       *blk;
    instruction *ins;
    instruction *next;
    bool        change;

    blk = HeadBlock;
    while( blk != NULL ) {
        do {
            ins = blk->ins.hd.next;
            change = FALSE;
            while( ins->head.opcode != OP_BLOCK ) {
                next = ins->head.next;
                if( InsIsCandidate( ins ) && FoldIntoIndex( ins ) ) {
                    UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
                    change = TRUE;
                }
                ins = next;
            }
        } while( change );
        blk = blk->next_block;
    }
}


static  bool    LivesAfterIns( instruction *ins, name *reg )
/**********************************************************/
{
    name        *res;
    hw_reg_set  live_later;

    HW_Asgn( live_later, ins->head.next->head.live.regs );
    HW_OnlyOn( live_later, reg->r.reg );
    if( HW_CEqual( live_later, HW_EMPTY ) ) return( FALSE );
    res = ins->result;
    if( res == NULL ) return( TRUE );
    if( res->n.class != N_REGISTER ) return( TRUE );
    if( HW_Subset( res->r.reg, live_later ) ) return( FALSE );
    return( TRUE );
}


static bool BadUse( name *reg, name *test, name **pindex, bool * is_base )
/************************************************************************/
{
    hw_reg_set  idx_name;
    hw_reg_set  base_name;

    if( test->n.class == N_REGISTER ) {
        if( HW_Ovlap( test->r.reg, reg->r.reg ) ) return( TRUE );
    }
    if( test->n.class == N_INDEXED ) {
        HW_Asgn( idx_name, test->i.index->r.reg );
        HW_CTurnOff( idx_name, HW_SEGS );
        base_name = idx_name;
        if( test->i.index_flags & X_HIGH_BASE ) {
            idx_name = Low64Reg( idx_name );
        } else if( test->i.index_flags & X_LOW_BASE ) {
            idx_name = High64Reg( idx_name );
        }
        HW_TurnOff( base_name, idx_name );
        if( HW_Equal( idx_name, reg->r.reg ) ) {
            *pindex = test;
        } else if( HW_Equal( base_name, reg->r.reg ) ) {
            *is_base = TRUE;
            *pindex = test;
        } else if( HW_Ovlap( test->i.index->r.reg, reg->r.reg ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


static  mod_info        Modifies( instruction *next, name *reg,
                                  hw_reg_set base_reg, hw_reg_set other_reg )
/***************************************************************************/
{
    mod_info    modifies;

    modifies = MODIFIES_NOTHING;
    if( HW_Ovlap( next->zap->reg, base_reg ) ) {
        modifies |= MODIFIES_BASE;
    }
    if( HW_Ovlap( next->zap->reg, other_reg ) ) {
        modifies |= MODIFIES_BASE;
    }
    if( HW_Ovlap( next->zap->reg, reg->r.reg ) ) {
        modifies |= MODIFIES_REG;
    }
    if( next->result != NULL && next->result->n.class == N_REGISTER ) {
        if( HW_Ovlap( next->result->r.reg, other_reg ) ) {
            modifies |= MODIFIES_BASE;
        }
        if( HW_Ovlap( next->result->r.reg, base_reg ) ) {
            modifies |= MODIFIES_BASE;
        }
        if( HW_Ovlap( next->result->r.reg, reg->r.reg ) ) {
            modifies |= MODIFIES_REG;
        }
    }
    return( modifies );
}


extern  instruction     *SIBPossibleIndex( instruction *ins, name *reg,
                                           name **pindex,
                                           bool *pbase,
                                           hw_reg_set base_reg,
                                           hw_reg_set other_reg,
                                           bool *dies, bool *pmodifies )
/**********************************************************************/
{
    instruction     *next;
    int             i;
    mod_info        modifies;

    next = ins->head.next;
    /* moving forward, ... */
    for( ;; ) {
        if( next->head.opcode == OP_BLOCK ) return( NULL );
        if( !HW_Ovlap( next->head.live.regs, reg->r.reg ) ) return( NULL );
        *pindex = NULL;
        *pbase = FALSE;
/*
        if we find an operand which uses the register, but not as an index
        we're outa here! NB: assumes only one unique index per instruction
*/
        i = next->num_operands;
        while( --i >= 0 ) {
            if( BadUse( reg, next->operands[ i ], pindex, pbase ) ) {
                return( NULL );
            }
        }
        if( *pindex != NULL ) break;
        if( next->result != NULL ) {
            if( BadUse( reg, next->result, pindex, pbase ) ) {
                return( NULL );
            }
        }
        if( *pindex != NULL ) break;
/*
        if instruction doesn't modify reg, (or possibly base reg) move forward
*/
        if( Modifies( next, reg, base_reg, other_reg ) != MODIFIES_NOTHING ) {
            return( NULL );
        }
        next = next->head.next;
    }
/*
    Hey, we found a good one as long as reg dies immediately after it
*/
    *dies = !LivesAfterIns( next, reg );
    modifies = Modifies( next, reg, base_reg, other_reg );
/*
    Check to see if the register "dies" in the middle of this instruction.
    IE: MOV 8[R1] => R1
*/
    if( !*dies ) {
        if( next->result != NULL && next->result->n.class == N_REGISTER ) {
            if( modifies == MODIFIES_REG ) {
                if( HW_Equal( next->result->r.reg, reg->r.reg ) ) {
                    *dies = TRUE;
                    modifies = MODIFIES_NOTHING;
                }
            }
        }
    }
    *pmodifies = ( modifies != MODIFIES_NOTHING );
    return( next );
}
