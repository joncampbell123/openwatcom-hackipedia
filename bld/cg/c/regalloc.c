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
* Description:  Register allocator.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "conflict.h"
#include "regset.h"
#include "pattern.h"
#include "procdef.h"
#include "hostsys.h"
#include "zoiks.h"
#include "model.h"

enum allocation_state {
    ALLOC_DONE,
    ALLOC_BITS,
    ALLOC_CONST_TEMP
};

extern  bool            SideEffect(instruction*);
extern  void            NowDead(name*,conflict_node*,name_set*,block*);
extern  void            PrefixIns(instruction*,instruction*);
extern  void            BurnRegTree(reg_tree*);
extern  void            IMBlip(void);
extern  conflict_node   *NameConflict(instruction*,name*);
extern  void            BuildNameTree(conflict_node*);
extern  void            AxeDeadCode(void);
extern  void            BurnNameTree(reg_tree*);
extern  bool            WorthProlog(conflict_node*,hw_reg_set);
extern  instruction     *MakeMove(name*,name*,type_class_def);
extern  void            DoNothing(instruction*);
extern  int             ExpandOps(bool);
extern  void            FindReferences(void);
extern  void            NowAlive(name*,conflict_node*,name_set*,block*);
extern  name            *DeAlias(name*);
extern  void            BuildRegTree(conflict_node*);
extern  void            FreeAConflict(conflict_node*);
extern  bool            IsIndexReg(hw_reg_set,type_class_def,bool);
extern  void            LiveInfoUpdate(void);
extern  void            MakeLiveInfo(void);
extern  void            FreeConflicts(void);
extern  reg_set_index   SegIndex(void);
extern  void            DelSegOp(instruction*,int);
extern  void            FixChoices(void);
extern  void            DelSegRes(instruction*);
extern  void            MakeConflicts(void);
extern  void            AddSegment(instruction*);
extern  void            SuffixIns(instruction*,instruction*);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  void            FixGenEntry(instruction*);
extern  int             NumOperands(instruction*);
extern  void            CalcSavings(conflict_node*);
extern  hw_reg_set      LowOffsetReg(hw_reg_set);
extern  name            *AllocRegName(hw_reg_set);
extern  bool            PropagateMoves(void);
extern  bool            PropRegsOne(void);
extern  conflict_node   *FindConflictNode(name*,block*,instruction*);
extern  hw_reg_set      HighOffsetReg(hw_reg_set);
extern  void            DeadInstructions(void);
extern  void            GRBlip(void);
extern  bool            IsSegReg(hw_reg_set);
extern  void            *SortList(void *,unsigned,bool (*)(void*,void*) );
extern  bool            MoreConflicts(void);
extern  void            DBAllocReg(name*,name*);
extern  void            MemConstTemp(conflict_node*);
extern  void            ConstSavings(void);
extern  void            RegInsDead(void);
extern  instruction     *FoldIns( instruction * );
extern  bool            IsUncacheableMemory( name * );
extern  hw_reg_set      MustSaveRegs(void);
extern  void            FreePossibleForAlias( conflict_node * );

extern  proc_def         *CurrProc;
extern  conflict_node    *ConfList;
extern  op_regs          RegList[];
extern  hw_reg_set       *RegSets[];
extern  hw_reg_set       GivenRegisters;
extern  name             *Names[];
extern  bool             BlockByBlock;
extern  bool             HaveLiveInfo;
extern  block           *HeadBlock;




static  bool    ContainedIn( name *name1, name *name2 ) {
/********************************************************
    Returns true if name1 is completely contained within the location
    occupied by name2
*/

    if( name1->n.class != name2->n.class ) return( FALSE );
    if( name1->n.class == N_TEMP ) {
        if( DeAlias( name1 ) != DeAlias( name2 ) ) return( FALSE );
    } else if( name1->n.class == N_MEMORY ) {
        if( name1 != name2 ) return( FALSE );
    } else {
        return( FALSE );
    }
    if( name1->v.offset < name2->v.offset ) return( FALSE );
    if( name1->v.offset + name1->n.size > name2->v.offset + name2->n.size )
        return( FALSE );
    return( TRUE );
}


static  hw_reg_set      SearchTree( reg_tree *tree,
                                    name *opnd, hw_reg_set reg ) {
/*****************************************************************
    Given a register "reg", and a reg_tree "tree", return the
    appropriate piece of "reg" to be associated with name "opnd".
*/

    if( tree->offset == opnd->v.offset && tree->size == opnd->n.size ) {
        return( reg );
    }
    if( opnd->v.offset < tree->offset + ( tree->size / 2 ) ) {
        return( SearchTree( tree->lo, opnd, LowOffsetReg( reg ) ) );
    } else {
        return( SearchTree( tree->hi, opnd, HighOffsetReg( reg ) ) );
    }
}


static  name    *FindReg( reg_tree *tree, name *opnd, name *reg ) {
/******************************************************************
    see SearchTree
*/

    return( AllocRegName( SearchTree( tree, opnd, reg->r.reg ) ) );
}


static  name    *ReplIndex( instruction *ins,
                            reg_tree *tree, name *x, name *reg ) {
/*****************************************************************
    Replace the index field of "x" with register "reg" in instruction "ins".
*/

    name        *new_x;

    ins->t.index_needs = RL_;
    reg = FindReg( tree, x->i.index, reg );
    new_x = ScaleIndex(reg, x->i.base, x->i.constant,
                        x->n.name_class, x->n.size, x->i.scale,
                        x->i.index_flags );
    return( new_x );
}


static  void    AssignMoreBits( void )
/*************************************
    Run through the list of conflicts and turn off the CONFLICT_ON_HOLD
    bit.  This is on for conflicts that needed an id bit but didn't get
    one.  MoreConflicts will assign a bit to any of these that weren't
    allocated the first time around.
*/
{
    conflict_node       *conf;

    conf = ConfList;
    while( conf != NULL ) {
        conf->state &= ~CONFLICT_ON_HOLD;
        conf = conf->next_conflict;
    }
    if( MoreConflicts() ) {
        MakeLiveInfo();
        AxeDeadCode();
    } else {
        LiveInfoUpdate();
    }
}


#if 0 /* 2007-07-10 RomanT -- This method is not used anymore */
static  void    InitAChoice( name *temp ) {
/*****************************************/

    name        *alias;

    if( temp->n.class != N_TEMP ) return;
    if( temp->t.temp_flags & ALIAS ) return;
    alias = temp;
    do {
        alias->t.possible = RL_NUMBER_OF_SETS;
        alias = alias->t.alias;
    } while( alias != temp );
}
#endif


static  void    InitChoices( void )
/**********************************
    Set the possible register choices of all conflicts/temps to be
    RL_NUMBER_OF_SETS meaning there are no restrictions as yet.  This
    choice gets more restricted as each instruction involving the
    conflict is expanded.

    For aliased temp vars, just free list of choices (without entry,
    other code will return RL_NUMBER_OF_SETS meaning there are no
    restrictions as yet).
*/
{
    conflict_node       *conf;
#if 0 /* 2007-07-10 RomanT -- This method is not used anymore */
    name                *opnd;
    block               *blk;
    instruction         *ins;
    int                 i;
#endif

    conf = ConfList;
    while( conf != NULL ) {
        conf->possible = RL_NUMBER_OF_SETS;
        FreePossibleForAlias( conf );  /* 2007-07-10 RomanT */
        conf = conf->next_conflict;
    }
#if 0 /* 2007-07-10 RomanT -- This method is not used anymore */
    if( BlockByBlock ) {
        /* this is WAY faster for BlockByBlock */
        blk = HeadBlock;
        while( blk != NULL ) {
            ins = blk->ins.hd.next;
            while( ins->head.opcode != OP_BLOCK ) {
                i = ins->num_operands;
                while( --i >= 0 ) {
                    InitAChoice( ins->operands[i] );
                }
                if( ins->result != NULL ) {
                    InitAChoice( ins->result );
                }
                ins = ins->head.next;
            }
            blk = blk->next_block;
        }
    } else {
        opnd = Names[ N_TEMP ];
        while( opnd != NULL ) {
            opnd->t.possible = RL_NUMBER_OF_SETS;
            opnd = opnd->n.next_name;
        }
    }
#endif
}


static  void    ReAlias( reg_tree *tree ) {
/******************************************
    Given a name tree, turn all temporaries into MASTER (not ALIAS)
    temporaries if their ancestor in the tree has no restrictions on
    which register it can have.  This effectively detaches the hi and lo
    part of a temporary so they may be treated separately during
    register allocation.
*/

    name        *curr;
    name        *new_ring;
    name        **owner;
    type_length endpoint;
    type_length begpoint;

    if( tree != NULL ) {
        if( tree->idx == RL_NUMBER_OF_SETS ) {
            ReAlias( tree->lo );
            ReAlias( tree->hi );
        } else {
            begpoint = tree->offset;
            endpoint = tree->size + begpoint;
            owner = &tree->temp->t.alias;
            new_ring = NULL;
            for(;;) {
                curr = *owner;
                if( curr->v.offset < begpoint
                 || curr->v.offset + curr->n.size > endpoint ) {
                    owner = &curr->t.alias;
                } else {
                    *owner = curr->t.alias;
                    if( new_ring == NULL ) {
                        new_ring = curr;
                        new_ring->t.alias = new_ring;
                    } else {
                        curr->t.alias = new_ring->t.alias;
                        new_ring->t.alias = curr;
                    }
                }
                if( curr == tree->temp ) break;
            }
            curr->t.temp_flags &= ~ALIAS;
        }
    }
}


static  bool    SplitConflicts( void )
/*************************************
    Build a name tree for each conflict, and then if the top of the tree
    has no restrictions on which registers it can have, its name mustn't
    be referenced by any instructions, so we call ReAlias to split make
    the high and low part their own "MASTER" rather than aliases of the
    temporary for the top of the tree.  See REGTREE.C for info re: name
    trees.

*/
{
    conflict_node       *conf;
    bool                change;

    change = FALSE;
    conf = ConfList;
    while( conf != NULL ) {
        BuildNameTree( conf );
        if( conf->tree != NULL
         && conf->tree->idx == RL_NUMBER_OF_SETS ) {
            change = TRUE;
            ReAlias( conf->tree );
            if( conf->tree->temp != NULL ) {
                conf->tree->temp->v.usage |= USE_MEMORY;
            }
            if( conf->tree->alt != NULL ) {
                conf->tree->alt->v.usage |= USE_MEMORY;
            }
        }
        BurnNameTree( conf->tree );
        conf->tree = NULL;
        conf = conf->next_conflict;
    }
    return( change );
}


extern  void    NullConflicts( var_usage off ) {
/***********************************************
    Turn off the usage attributes for each temporary, and null the
    conflict field.  This is called in preparation for calling
    MakeConflicts.  "off" is an extra bit will get turned off.  Generate
    will call NullConflicts( USE_IN_ANOTHER_BLOCK ) since it will call
    SearchDefUse to calculate this bit accurately.  USE_IN_ANOTHER_BLOCK
    bit is initially turned on for all front end symbols since we don't
    know how it is going to be used.
*/

    name        *temp;

    temp = Names[ N_TEMP ];
    while( temp != NULL ) {
        temp->v.conflict = NULL;
        temp->v.usage &=(USE_IN_ANOTHER_BLOCK+USE_MEMORY+USE_ADDRESS
                        +VAR_VOLATILE+NEEDS_MEMORY+HAS_MEMORY);
        if( ( temp->v.usage & (USE_MEMORY+USE_ADDRESS+VAR_VOLATILE+NEEDS_MEMORY) )
            == EMPTY ) {
            temp->v.usage &= ~off;
        }
        temp->v.block_usage = EMPTY;
        temp = temp->n.next_name;
    }
}


static  bool  FixInstructions( conflict_node *conf, reg_tree *tree,
                               hw_reg_set reg, bool need_live_update ) {
/***********************************************************************
    Run the the instruction list assigning register "reg" to conflict
    "conf" whose tree is "tree".  "need_live_info_update" will be set to
    false if the caller of this function is going to update the live
    information itself.
*/

    name                *reg_name;
#include "savcache.h"

    reg_name = AllocRegName( reg );
    if( ( conf->name->v.usage & USE_IN_ANOTHER_BLOCK )
     && ( conf->name->v.usage & ( NEEDS_MEMORY | USE_ADDRESS ) ) ) {
        conf->name->v.usage |= USE_MEMORY;
    }
    opnd = tree->temp;
    if( _IsModel( DBG_LOCALS ) ) {
        DBAllocReg( reg_name, opnd );
    }

#define _InRegAssgn
#include "savcode.h"

    if( _LBitEmpty( conf->id.within_block )
     && _GBitEmpty( conf->id.out_of_block ) ) {
        /* update live info since the conflict had no id.*/
        if( need_live_update ) {
            LiveInfoUpdate();
            return( FALSE );
        } else {
            return( TRUE );
        }
    }
    return( FALSE );
}


static  bool    Idx( name *op ) {
/********************************
    Return TRUE if "op" is a name that has been used as the index field
    of an N_INDEXED (eg: 5[t1])
*/

    if( op == NULL ) return( FALSE );
    if( op->n.class != N_TEMP ) return( FALSE );
    if( ( op->t.temp_flags & INDEXED ) == EMPTY ) return( FALSE );
    return( TRUE );
}


static  void    BitOff( conflict_node *conf ) {
/**********************************************
    Run through the instruction turning the bits for conflict "conf" off
    in the live information.  (Done after it's been given a register or
    stuffed in memory).
*/


    block       *blk;
    instruction *ins;

    blk = conf->start_block;
    ins = conf->ins_range.first;
    if( blk != NULL ) {
        for(;;) {
            if( ins == conf->ins_range.last ) break;
            if( ins->head.opcode == OP_BLOCK ) {
                blk = blk->next_block;
                ins = (instruction *)&blk->ins;
            }
            ins = ins->head.next;
            _GBitTurnOff( ins->head.live.out_of_block, conf->id.out_of_block );
            _LBitTurnOff( ins->head.live.within_block, conf->id.within_block );
        }
    }
}


static  signed_32     CountRegMoves( conflict_node *conf,
                               hw_reg_set reg, reg_tree *tree,
                               int levels ) {
/********************************************
    For a conflict "conf", whose tree is "tree", count the number of
    instructions of the form (1) "MOV Rn => Rn" or (2) "OP Rn,x => Rn"
    that assigning register "reg" to the conflict would create.  These
    are good things to create since the go away (1) or are easy to
    generate in one machine instruction (2).
*/

    block               *blk;
    instruction         *ins;
    signed_32           count;
    int                 half;
    name                *reg_name;
    name                *op1;
#if _TARGET & (_TARG_80386|_TARG_IAPX86|_TARG_370)
    name                *op2;
#endif
    name                *res;
    bool                idx;
    conflict_node       *other_conf;
    name                *other_opnd;

    levels = levels;
    if( tree == NULL ) return( 0 );
    reg_name = AllocRegName( reg );
    count = 0;
    blk = conf->start_block;
    ins = conf->ins_range.first;
    if( tree->temp != NULL ) {
        idx = IsIndexReg( reg, tree->temp->n.name_class, FALSE );
    } else {
        idx = FALSE;
    }
    half = tree->size / 2;
    for(;;) {
        if( ins->head.opcode == OP_BLOCK ) {
            blk = blk->next_block;
            ins = blk->ins.hd.next;
        } else {
            if( ins->head.opcode != OP_MOV ) {
/* 88-Dec-23*/
            #if _TARGET & (_TARG_80386|_TARG_IAPX86|_TARG_370)
                op1 = NULL;
                op2 = NULL;
                if( ins->num_operands != 0 ) {
                    op1 = ins->operands[ 0 ];
                    switch( ins->head.opcode ) {
                    case OP_ADD:
                    case OP_EXT_ADD:
                    case OP_MUL:
                    case OP_AND:
                    case OP_OR:
                    case OP_XOR:
                        op2 = ins->operands[ 1 ];
                        break;
                    }
                }
                res = ins->result;
                if( res == reg_name ) {
                    if( op1 == tree->temp || op1 == tree->alt
                      || op2 == tree->temp || op2 == tree->alt ) {
                        count += half;
                    }
                } else if( res == tree->temp || res == tree->alt ) {
                   if( op1 == reg_name || op2 == reg_name ) {
                        count += half;
                   }
                }
/* 88-Dec-23*/
            #endif
            } else {
                op1 = ins->operands[ 0 ];
                res = ins->result;
                if( ( ( op1 == tree->temp || op1 == tree->alt )
                     && ( res == reg_name || ( idx && Idx( res ) ) ) )
                 || ( ( res == tree->temp || res == tree->alt )
                     && ( op1 == reg_name || ( idx && Idx( op1 ) ) ) ) ) {
                    count += tree->size;
                } else if( ( res->n.class == N_REGISTER )
                        && HW_Ovlap( reg, res->r.reg ) ) {
                    /*
                       if we're moving into an overlapping register, give
                       it half a move waiting. E.g.:
                         MOV U2 [DI] ==> t1
                         MOV U1 t1   ==> CL
                    */
                    count += half;
                }
            }
            if( ins == conf->ins_range.last ) break;
            ins = ins->head.next;
        }
    }
#if ( _TARGET & _TARG_370 )
    if( 1 ) {
#else
    if( _IsModel( SUPER_OPTIMAL ) ) {
#endif
        count += CountRegMoves( conf, HighOffsetReg( reg ), tree->hi, levels );
        count += CountRegMoves( conf, LowOffsetReg( reg ), tree->lo, levels );
    }
    if( _IsModel( SUPER_OPTIMAL ) ) {
        /*
         * This is really expensive, compile time-wise, but what it does is
         * checks to see if we have an intervening temp or two that may turn
         * into register register moves, as in
         * MOV Rn => T1
         * MOV T1 => T2
         * MOV T2 => Rn
         * which will save us something in the case that T1 or T2 get assigned
         * to Rn
         */

        hw_reg_set      saved_regs;

        saved_regs = MustSaveRegs();
        if( !HW_Ovlap( saved_regs, reg ) ) count += 2;
        count <<= levels;
        if( count != 0 || levels == 0 ) return( count );
        blk = HeadBlock;
        while( blk != NULL ) {
            ins = blk->ins.hd.next;
            for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
                if( ins->head.opcode == OP_MOV ) {
                    other_opnd = NULL;
                    if( ins->result == conf->name ) {
                        other_opnd = ins->operands[ 0 ];
                    } else if( ins->operands[ 0 ] == conf->name ) {
                        other_opnd = ins->result;
                    }
                    if( other_opnd == NULL ) continue;
                    switch( other_opnd->n.class ) {
                    case N_MEMORY:
                    case N_INDEXED:
                    case N_CONSTANT:
                        break;
                    default:
                        other_conf = FindConflictNode( other_opnd, blk, ins );
                        if( other_conf != NULL ) {
                            reg_name = tree->temp;
                            tree->temp = other_conf->name;
                            count = CountRegMoves( other_conf, reg, tree, levels-1 );
                            tree->temp = reg_name;
                            return( count );
                        }
                    }
                }
            }
            blk = blk->next_block;
        }
        return( 0 );
    }
    return( count );
}


static  bool    UnaryOpGetsReg( instruction *ins, hw_reg_set reg,
                                name *op ) {
/******************************************************************************
    return true if this is a unary operator and we can generate code
    given that the result or operand gets a register (a little machine specific
    but I don't know of any machines for which this isn't true).
*/
     return( NumOperands( ins ) == 1 && ins->result != NULL &&
            !IsSegReg( reg ) && ins->head.opcode != OP_CONVERT &&
            ( ins->operands[0] == op || ins->result == op ) );
}


static  bool    StealsSeg( instruction *ins,
                           hw_reg_set reg, hw_reg_set except,
                           name *actual_op ) {
/***************************************************************
    Does giving "reg" to "conf" steal the last segment away from "ins"?
*/

    hw_reg_set          *index_needs;
    name                *op;
    conflict_node       *new_conf;
    int                 i;

    i = ins->num_operands;
    --i;
    if( i < NumOperands( ins ) ) return( FALSE );
    op = ins->operands[ i ];
    new_conf = NameConflict( ins, op );
    if( new_conf == NULL ) return( FALSE );
    if( ( op == actual_op ) && IsSegReg( reg ) ) return( FALSE );
    index_needs = RegSets[  SegIndex()  ];
    if( HW_CEqual( *index_needs, HW_EMPTY ) ) return( FALSE );
    for(;;) {
        if( !HW_Ovlap( *index_needs, except ) ) return( FALSE );
        ++index_needs;
        if( HW_CEqual( *index_needs, HW_EMPTY ) ) break;
    }
    return( TRUE );
}


static  bool    StealsIdx( instruction *ins,
                           hw_reg_set except, name *actual_op ) {
/****************************************************************
    Does precluding the set of regiseters "except" from operands of
    "ins" make it impossible to generate an indexed addressing mode?
*/

    hw_reg_set          *index_needs;
    name                *op;
    conflict_node       *new_conf;
    int                 i;
    bool                is_result;

    i = ins->num_operands;
    is_result = FALSE;
    while( --i >= 0 ) {
        op = ins->operands[ i ];
        if( op->n.class == N_INDEXED ) {
            new_conf = NameConflict( ins, op->i.index ); // oops
            if( new_conf == NULL || actual_op == op->i.index ) return( FALSE );
        }
    }
    if( ins->result != NULL ) {
        op = ins->result;
        if( op->n.class == N_INDEXED ) {
            new_conf = NameConflict( ins, op->i.index ); // oops
            if( new_conf == NULL || actual_op == op->i.index ) return( FALSE );
            is_result = TRUE;
        }
    }
    index_needs = RegSets[  ins->t.index_needs  ];
    if( HW_CEqual( *index_needs, HW_EMPTY ) ) return( FALSE );
    for(;;) {
        if( !HW_Ovlap( *index_needs, except ) ) {
            if( !is_result ) return( FALSE );
            if( !HW_Ovlap( *index_needs, ins->zap->reg ) ) return( FALSE );
        }
        ++index_needs;
        if( HW_CEqual( *index_needs, HW_EMPTY ) ) break;
    }
    return( TRUE );
}


extern  bool            CheckIndecies( instruction *ins,
                                       hw_reg_set reg, hw_reg_set except,
                                       name *op ) {
/***************************************************************************
    Used by TooGreedy
*/

    HW_TurnOn( except, ins->head.live.regs );
    HW_TurnOn( except, reg );
    if( StealsIdx( ins, except, op ) ) return( MAYBE );
    if( StealsSeg( ins, reg, except, op ) ) return( MAYBE );
    return( FALSE );
}


static  bool            TooGreedy( conflict_node *conf,
                                   hw_reg_set reg, name *op ) {
/*************************************************************************
    This routine decides whether giving register "reg" to conflict
    "conf" would be too greedy.  Too greedy means that it would use the
    last index registers across an instruction that still needed one, or
    take the last segment register across an instruction still needing
    one, or take away the last register of a certain type that an
    instruction is reserving.  For example, a generate entry like
        _Un(   ANY, ANY,  NONE ), V_NO,  G_UNKNOWN,    RG_BYTE_NEED
    means that this instruction is content to sit around for a while but
    will need a byte register left over in order to be able to guarantee
    that we can generate code, since all operands may go into memory and
    a register will be needed if the machine has no addressing mode for
    memory/memory operations. Notice that all three of these requirements
    must be satisfied. An instruction could be reserving 3 registers.
*/

    block               *blk;
    instruction         *ins;
    instruction         *last;
    hw_reg_set          *ins_needs;
    bool                rc;
    reg_set_index       needs;

    blk = conf->start_block;
    ins = conf->ins_range.first;
    if( conf->name->n.class == N_TEMP
     && _Is( conf, ( INDEX_SPLIT | SEGMENT_SPLIT ) ) ) {
        ins = conf->ins_range.last;
    }
    last = conf->ins_range.last;
    rc = FALSE;
    for(;;) {
        if( ins->u.gen_table == NULL ) { /* just created instruction*/
            needs = RL_;
        } else {
            needs = ins->u.gen_table->reg_set;
        }
        ins_needs = RegSets[  RegList[  needs  ].need  ];
        if( HW_CEqual( *ins_needs, HW_EMPTY ) || _Is( conf, NEVER_TOO_GREEDY )
         || UnaryOpGetsReg( ins, reg, op ) ) {
            rc = CheckIndecies( ins, reg, HW_EMPTY, op );
        } else { /* can the instruction and indecies still get needed regs?*/
            rc = TRUE;
            for(;;) {
                if( !HW_Ovlap( *ins_needs, ins->head.live.regs )
                 && !HW_Ovlap( reg, *ins_needs ) ) {
                    rc = CheckIndecies( ins, reg, *ins_needs, op );
                    if( rc == FALSE ) break;
                }
                ++ins_needs;
                if( HW_CEqual( *ins_needs, HW_EMPTY ) ) break;
            }
        }
        if( ins == last ) break;
        ins = ins->head.next;
        while( ins->head.opcode == OP_BLOCK ) {
            blk = blk->next_block;
            ins = blk->ins.hd.next;
        }
        if( rc != FALSE ) break;
    }
    return( rc );
}


static  void    CheckIndexZap( conflict_node *conf, block *blk, instruction *ins ) {
/***********************************************************************************
    If the given instruction uses the name for conf as an index in the result,
    then mark the conflict as conflicting with anything in the instructions
    zap set, as the zap will take place before the result is written.
*/

    name        *dst;

    dst = ins->result;
    if( dst != NULL && dst->n.class == N_INDEXED ) {
        if( FindConflictNode( dst->i.index, blk, ins ) == conf ) {
            HW_TurnOn( conf->with.regs, ins->zap->reg );
        }
    }
}


static  void    NeighboursUse( conflict_node *conf ) {
/*****************************************************
    Calculate which conflicts "conf" could not share the same register
    with by running through the live range of the conflict and checking
    what other conflicts are live and what registers are live or
    modified at a point where "conf" is also live.
*/

    block               *blk;
    instruction         *ins;
    name                *dst;
    name                *definition;
    name_set            no_conflict;
    hw_reg_set          tmp;
    instruction         *last;
    global_bit_set      gbit;
    local_bit_set       lbit;

    blk = conf->start_block;
    ins = conf->ins_range.first;
    last = conf->ins_range.last;
    if( ins->head.opcode == OP_MOV && ins->result == conf->name ) {
        definition = ins->operands[0];
    } else {
        definition = NULL;
    }
    HW_CAsgn( conf->with.regs, HW_EMPTY );
    _GBitInit( conf->with.out_of_block, EMPTY );
    _LBitInit( conf->with.within_block, EMPTY );
    _INS_NOT_BLOCK ( last );
    if( ins != last ) {
        _NameSetInit( no_conflict );
        for(;;) {
            ins = ins->head.next;
            if( ins->head.opcode != OP_BLOCK ) {

                /*   The no_conflict set indicates names which do not conflict*/
                /*   with conf->name due to OP_MOV instructions*/

                dst = ins->result;
                if( dst != NULL ) {
                    NowDead( dst, FindConflictNode( dst, blk, ins ),
                                &no_conflict, blk );
                }
                if( ins->head.opcode != OP_MOV ) {
                    if( dst == conf->name ) {
                        _NameSetInit( no_conflict );
                    }
                } else if( ins->operands[ 0 ] == conf->name ) {
                    NowAlive( dst, FindConflictNode( dst, blk, ins ),
                                    &no_conflict, blk );
                } else if( dst == conf->name ) {
                    _NameSetInit( no_conflict );
                    definition = ins->operands[ 0 ];
                    NowAlive( definition,
                              FindConflictNode( definition, blk, ins ),
                              &no_conflict, blk );
                }
                /* it only conflicts if temp is live across result/zap*/
                if( ins != last ) {
                    if( ( conf->name->v.usage & ( NEEDS_MEMORY | USE_ADDRESS ) )
                     || ( _LBitEmpty( conf->id.within_block )
                       && _GBitEmpty( conf->id.out_of_block ) )
                     || ( _LBitOverlap( conf->id.within_block,
                          ins->head.next->head.live.within_block ) )
                     || ( _GBitOverlap( conf->id.out_of_block,
                          ins->head.next->head.live.out_of_block ) ) ) {
                        HW_TurnOn( conf->with.regs, ins->zap->reg );
                        if( dst != NULL && dst->n.class == N_REGISTER ) {
                            HW_TurnOn( conf->with.regs, dst->r.reg );
                        }
                    } else {
                        // know that conf is not live after this instruction
                        // if it was live before, must mark it as conflicting
                        // with anything in the zap set         BBB - Nov, 1994
                        if( ( _LBitOverlap( conf->id.within_block,
                                ins->head.live.within_block ) )
                         || ( _GBitOverlap( conf->id.out_of_block,
                                ins->head.live.out_of_block ) ) ) {
                            CheckIndexZap( conf, blk, ins );
                        }
                    }
                } else {
                    CheckIndexZap( conf, blk, ins );
                }
            }
            if( _GBitOverlap( ins->head.live.out_of_block,
                              conf->id.out_of_block )
             || _LBitOverlap( ins->head.live.within_block,
                              conf->id.within_block )
             || ( _LBitEmpty( conf->id.within_block )
               && _GBitEmpty( conf->id.out_of_block ) ) ) {
                tmp = ins->head.live.regs;
                HW_TurnOff( tmp, no_conflict.regs );
                HW_TurnOn( conf->with.regs, tmp );
                _GBitAssign( gbit, ins->head.live.out_of_block );
                _GBitTurnOff( gbit, no_conflict.out_of_block );
                _GBitTurnOn( conf->with.out_of_block, gbit );
                _LBitAssign( lbit, ins->head.live.within_block );
                _LBitTurnOff( lbit, no_conflict.within_block );
                _LBitTurnOn( conf->with.within_block, lbit );
            }
            if( ins->head.opcode == OP_CALL
                        || ins->head.opcode == OP_CALL_INDIRECT ) {
                _NameSetInit( no_conflict );
            } else if( ins->head.opcode == OP_BLOCK ) {
                _LBitInit( conf->with.within_block, EMPTY );
                if( blk->next_block == NULL ) {
                    Zoiks( ZOIKS_141 );
                    break;
                }
                blk = blk->next_block;
                ins = (instruction *)&blk->ins;
                _NameSetInit( no_conflict );
            }
            if( ins->id == last->id ) break;
        }
    }
    /*
        Here's the deal with the following: we are assuming that a register
        being used to initialize a const temp must be the allocated register
        of another const temp - ie we cannot allow reductions which will turn
        something like "mov K -> t1" into "mov K -> R1, mov R1 -> t1" - any
        constant loading reduction which can generate a move from a register
        into a const temp must wait until the const temp has been allocated a
        register or dissolved back into a constant before happening.

        The reason we need to do this is because const temps percolating down
        from parent-loops will be marked as being live all throughout the loop,
        which means they would conflict with this const temp and so could not
        normally share a register.

        BBB - July 22, 1995
     */
    if( _ConstTemp( conf->name ) ) {
        if( definition != NULL && definition->n.class == N_REGISTER ) {
            HW_TurnOff( conf->with.regs, definition->r.reg );
            conf->savings = MAX_SAVE;
        }
    }
}


static  hw_reg_set      GiveBestReg( conflict_node *conf, reg_tree *tree,
                                     hw_reg_set except, bool needs_one ) {
/*************************************************************************
    Give the best possible register to conflict "conf", (whose tree is
    "tree"), excluding registers in the set "except".  The best register
    is one that would create the most register register moves of the
    form "MOV Rn => Rn", or operations of the form  "OP Rn,x => Rn"
    but is not too greedy (See TooGreedy).  needs_one is TRUE when the
    conflict really really really needs to be assigned a register.  If
    this routine fails, but needs_one is TRUE, something truly bad has
    happened.
*/

    hw_reg_set  reg;
    hw_reg_set  best;
    int         best_saves;
    int         saves;
    hw_reg_set  *possible;
    hw_reg_set  given;
    hw_reg_set  gave_hi;
    hw_reg_set  gave_lo;
    bool        greed;
    bool        all_TRUE;
    bool        failed;

    NeighboursUse( conf );
    if( tree == NULL ) {
        HW_CAsgn( given, HW_EMPTY );
    } else if( tree->regs == NULL ) {
        /*
         * there are no restraints on the whole temporary (not referenced)
         * so give registers to its high and low parts seperately
         */
        failed = FALSE;
        HW_CAsgn( gave_hi, HW_EMPTY );
        HW_CAsgn( gave_lo, HW_EMPTY );
        if( tree->hi != NULL ) {
            gave_hi = GiveBestReg( conf, tree->hi, except, needs_one );
            if( HW_CEqual( gave_hi, HW_EMPTY ) ) {
                failed = TRUE;
            }
        }
        if( tree->lo != NULL ) {
            given = except;
            HW_TurnOn( given, gave_hi );
            gave_lo = GiveBestReg( conf, tree->lo, given, needs_one );
            if( HW_CEqual( gave_lo, HW_EMPTY ) ) {
                failed = TRUE;
            }
        }
        if( failed ) {
            HW_CAsgn( given, HW_EMPTY );
        } else {
            given = gave_hi;
            HW_TurnOn( given, gave_lo );
        }
        HW_TurnOn( GivenRegisters, gave_hi );
        HW_TurnOn( GivenRegisters, gave_lo );
    } else {
        best = HW_EMPTY;
        best_saves = -1;
        all_TRUE = TRUE;
        possible = tree->regs;
        for(;;) {
            reg = *possible;
            if( HW_CEqual( reg, HW_EMPTY ) ) break;
            if( !HW_Ovlap( reg, conf->with.regs )
             && !HW_Ovlap( reg, except ) ) {
                greed = TooGreedy( conf, reg, tree->temp );
                if( greed == FALSE ) {
                    saves = CountRegMoves( conf, reg, conf->tree, 3 );
                    if( ( saves > best_saves )
                     || ( saves == best_saves
                       && HW_Subset( GivenRegisters, reg )
                       && !HW_Subset( GivenRegisters, best ) ) ) {
                        best = reg;
                        best_saves = saves;
                    }
                }
                if( greed != TRUE ) {
                    all_TRUE = FALSE;
                }
            }
            ++ possible;
        }
        if( all_TRUE ) {
            HW_CAsgn( given, HW_EMPTY );
        } else if( HW_CEqual( best, HW_EMPTY ) ) {
            if( _Is( conf, NEEDS_INDEX ) ) {
                _SetTrue( conf, NEEDS_INDEX_SPLIT );
                _SetFalse( conf, NEEDS_INDEX );
            }
            if( _Is( conf, NEEDS_SEGMENT ) ) {
                _SetTrue( conf, NEEDS_SEGMENT_SPLIT );
                _SetFalse( conf, NEEDS_SEGMENT );
            }
            HW_CAsgn( given, HW_EMPTY );
        } else if( needs_one || WorthProlog( conf, best ) ) {
            FixInstructions( conf, tree, best, TRUE );
            given = best;
            HW_TurnOn( GivenRegisters, given );
        } else {
            HW_CAsgn( given, HW_EMPTY );
        }
    }
    if( HW_CEqual( given, HW_EMPTY ) && needs_one ) {
        _Zoiks( ZOIKS_040 );
    }
    return( given );
}


extern  bool    AssignARegister( conflict_node *conf, hw_reg_set reg ) {
/***********************************************************************
    Used to assign register "reg" to conflict "conf", before we've
    started the true register allocator.  (used by I87REG.C)
*/

    bool        need_live_update;

    BuildRegTree( conf );
    need_live_update = FixInstructions( conf, conf->tree, reg, FALSE );
    BitOff( conf );
    BurnRegTree( conf->tree );
    FreeAConflict( conf );
    return( need_live_update );
}


static  void    PutInMemory( conflict_node *conf ) {
/***************************************************
    Put "conf" into memory.  Also if the conflict is only used by one
    instruction, its useless, so we zap that instruction to not get
    generated.  We also turn the bit for "conf" off in the live
    information
*/

    block       *blk;
    instruction *ins;
    name        *opnd;

    IMBlip();
    opnd = conf->name;
    if( opnd->n.class == N_TEMP ) {
        if( opnd->t.temp_flags & CONST_TEMP ) {
            MemConstTemp( conf );
        } else {
            for(;;) {
                opnd->v.usage |= NEEDS_MEMORY | USE_MEMORY;
                opnd = opnd->t.alias;
                if( opnd == conf->name ) break;
            }
        }
    } else {
        opnd->v.usage |= NEEDS_MEMORY | USE_MEMORY;
    }
    blk = conf->start_block;
    ins = conf->ins_range.first;
    if( blk != NULL ) {
        for(;;) {
            if( ins->head.opcode != OP_BLOCK &&
                ins->head.state == OPERANDS_NEED_WORK ) {
                ins->head.state = INS_NEEDS_WORK;
            }
            if( ins->id == conf->ins_range.last->id ) break;
            if( ins->head.opcode == OP_BLOCK ) {
                if( blk->next_block == NULL ) {
                    Zoiks( ZOIKS_141 );
                    break;
                }
                blk = blk->next_block;
                ins = (instruction *)&blk->ins;
            }
            ins = ins->head.next;
            _GBitTurnOff( ins->head.live.out_of_block, conf->id.out_of_block );
            _LBitTurnOff( ins->head.live.within_block, conf->id.within_block );
        }
        if( conf->ins_range.first == conf->ins_range.last
         && conf->name->n.class == N_TEMP
         && ( conf->name->v.usage & USE_ADDRESS ) == 0
         && SideEffect( conf->ins_range.first ) == FALSE ) {
            DoNothing( conf->ins_range.first );
        }
    }
}


extern  conflict_node   *InMemory( conflict_node *conf ) {
/*********************************************************
    Put conflict "conf" and all other conflicts associated with the same
    name as "conf" into memory.  Sorry charlie, no register.
    Save conf as last to go so it's next_conf will be valid in case
    a bunch got freed in MemConstTemp
*/

    conflict_node       *next;
    conflict_node       *conf_list;

    conf_list = conf->name->v.conflict;
    while( conf_list != NULL ) {
        if( conf != conf_list ){
            PutInMemory( conf_list );
            next = conf_list->next_for_name;
            FreeAConflict( conf_list );
            conf_list = next;
        }else{
            conf_list = conf_list->next_for_name;
        }
    }
    PutInMemory( conf );
    next = conf->next_conflict;
    FreeAConflict( conf );
    return( next );
}


extern  conflict_node   *GiveRegister( conflict_node *conf, bool needs_one ) {
/*****************************************************************************
    Give a register to conflict "conf", if at all possible.  The
    NEEDS_INDEX and NEEDS_SEGMENT stuff is just saying that if we tried
    to give a register to a conflict that needs to be an index or
    segment register, and failed, we will set NEEDS_INDEX_SPLIT, or
    NEEDS_SEGMENT_SPLIT.  This in turn will cause ExpandOps the next
    time around to turn any reference to this variable into a temporary
    reference, for example:

        ADD     [x],y => Z
            becomes
        MOV     x => temp
        ADD     [temp],y => Z

    thus shifting the NEEDS_INDEX/NEEDS_SEGMENT requiremement onto a
    very short lived temporary, which will be guaranteed to get the
    right type of register (ensured by TooGreedy).
*/

    reg_tree            *tree;
    conflict_node       *next_valid;
    hw_reg_set          given;

    GRBlip();
    BuildRegTree( conf );
    tree = conf->tree;
    if( _Is( conf, ( INDEX_SPLIT | SEGMENT_SPLIT ) ) ) {
        needs_one = TRUE;
    }
    given = GiveBestReg( conf, tree, CurrProc->state.unalterable, needs_one );
    if( tree != NULL && ( tree->hi != NULL || tree->lo != NULL ) ) {
        LiveInfoUpdate(); /* the live info is FAR to conservative. */
    }
    if( !HW_CEqual( given, HW_EMPTY ) ) {
        next_valid = conf->next_conflict;
        BitOff( conf );
        FreeAConflict( conf );
    } else {
        if( _Is( conf, NEEDS_INDEX ) ) {
            _SetTrue( conf, NEEDS_INDEX_SPLIT );
            _SetFalse( conf, NEEDS_INDEX );
        } else if( _Is( conf, NEEDS_SEGMENT ) ) {
            _SetTrue( conf, NEEDS_SEGMENT_SPLIT );
            _SetFalse( conf, NEEDS_SEGMENT );
        }
        if( _Isnt( conf, ( NEEDS_SEGMENT_SPLIT | NEEDS_INDEX_SPLIT ) ) ) {
            next_valid = InMemory( conf );
        } else {
            next_valid = conf->next_conflict;
        }
    }
    BurnRegTree( tree );
    return( next_valid );
}


static  bool            ConfBefore( void *c1, void *c2 ) {
/*********************************************************
    used by SortConflicts
*/

    return( ((conflict_node *)c1)->savings > ((conflict_node *)c2)->savings );
}


static  void    SortConflicts( void )
/************************************
    Sort the conflicts in order of descending savings.
*/
{
    ConfList = SortList( ConfList, offsetof( conflict_node, next_conflict ),
                         ConfBefore );
}


static  enum allocation_state    AssignConflicts( void )
/*******************************************************
    Run through the conflict list and calculate the savings associated
    with giving a register to each one.  Sort the list in order of
    descending savings and then give a register (or memory location) to
    each conflict in the list that is not ON_HOLD.
*/
{
    conflict_node               *conf;
    conflict_node               *next;
    enum allocation_state       state;
    name                        *opnd;
    bool                        only_const_temps;

    conf = ConfList;
    while( conf != NULL ) {
        next = conf->next_conflict;
        if( conf->start_block == NULL ) {
            FreeAConflict( conf );
        } else {
            if( _Isnt( conf, SAVINGS_CALCULATED ) ) {
                conf->available = 1; /* FOR NOW for CalcSavings' benifit */
                CalcSavings( conf );
                if( _Isnt( conf, CONFLICT_ON_HOLD ) ) {
                    _SetTrue( conf, SAVINGS_CALCULATED );
                    _SetTrue( conf, SAVINGS_JUST_CALCULATED );
                }
            }
            _SetFalse( conf, NEEDS_INDEX_SPLIT | NEEDS_SEGMENT_SPLIT );
        }
        conf = next;
    }
    ConstSavings();

    SortConflicts();
    state = ALLOC_BITS;
    conf = ConfList;
    if( conf == NULL ) return( state );
    opnd = conf->name;
    if( opnd->n.class == N_TEMP && (opnd->t.temp_flags & CONST_TEMP) ) {
        only_const_temps = TRUE;
    } else {
        only_const_temps = FALSE;
    }
    for( ;;  ) {
        if( conf == NULL ) break;
        opnd = conf->name;
        next = conf->next_conflict;
        if( _Isnt( conf, CONFLICT_ON_HOLD ) ) {
            /*
                We stop register allocating on the first CONST_TEMP we see
                so that any CONST_TEMP's that aren't needed can get cleaned
                up and their live information regenerated.
            */
            if( !only_const_temps && opnd->n.class == N_TEMP
                && (opnd->t.temp_flags & CONST_TEMP) ) return( ALLOC_CONST_TEMP );
            if( conf->savings == 0 || IsUncacheableMemory( conf->name ) ) {
                next = InMemory( conf );
            } else {
                next = GiveRegister( conf, FALSE );
            }
            if( opnd->v.conflict == conf ) { /* if it didn't get processed */
                state = ALLOC_DONE;
            }
        }
        conf = next;
    }
    return( state );
}


extern  void    ReConstFold( void )
/**********************************
    Call FoldIns on each instruction in case we propagated a constant
    into an instruction leaving something which looks like C op C -> T,
    which none of the regalloc tables can handle. Can't just call
    ConstFold because it works on a stupid partition.
*/
{
    instruction                 *ins;
    instruction                 *next;
    block                       *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
            next = ins->head.next;
            FoldIns( ins );
        }
    }
}


extern  bool    RegAlloc( bool keep_on_truckin ) {
/*************************************************
    Allocate registers to variables while expanding instructions.
    Instructions are expanded until they correspond 1 to 1 with machine
    instructions.  The first part of this routine is for the 8086.  We
    expand all instructions and then see if any variables get split up
    (Far pointers for example rarely get referenced as a whole.) If any
    variables get split so that only their parts are referenced, we
    throw away the conflict graph and start again.  This causes a
    segment and an offset to be treated as separate variables on the
    8086.  The second part of this routine just expands instructions,
    and gives out registers in turn until no more instructions or
    operands need work.
*/

    int unknowns;
    enum allocation_state       last;

    HW_CAsgn( GivenRegisters, HW_EMPTY );
    if( BlockByBlock == FALSE ) {
        InitChoices();
        unknowns = ExpandOps( keep_on_truckin );
        if( unknowns <= 0 ) return( unknowns == 0 );
        if( SplitConflicts() ) {
            FreeConflicts();
            NullConflicts( EMPTY );
            HaveLiveInfo = FALSE;
            if( _IsntModel( NO_OPTIMIZATION ) ) {
                DeadInstructions();
                FindReferences();
                PropagateMoves();
                PropRegsOne();
                ReConstFold();
            }
            FindReferences();
            MakeConflicts();
            MakeLiveInfo();
            HaveLiveInfo = TRUE;
            AxeDeadCode();
        }
    }
    last = ALLOC_DONE;
    for(;;) {
        InitChoices();
        unknowns = ExpandOps( keep_on_truckin );
        if( unknowns <= 0 ) break;
        FixChoices();
        if( last == ALLOC_CONST_TEMP ) {
            /* Ran into the first CONST_TEMP conflict.
               need to do a RegInsDead again in case some of the
               temps aren't used anymore */
            RegInsDead();
        }
        last = AssignConflicts();
        if( last == ALLOC_BITS ) {
            /* ran out of bits */
            AssignMoreBits();
        }
    }
    return( unknowns == 0 );
}
