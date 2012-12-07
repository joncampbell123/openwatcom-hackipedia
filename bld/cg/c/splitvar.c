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
* Description:  Variable splitting - find cases where a var is used for
*               separate and independent purposes.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "conflict.h"
#include "opcodes.h"
#include "stackok.h"
#include "stack.h"

extern  block           *HeadBlock;
extern  conflict_node   *ConfList;

extern  void            NullConflicts( var_usage );
extern  void            FreeConflicts( void );
extern  void            FindReferences( void );
extern  void            MakeConflicts( void );
extern  name            *SAllocTemp( type_class_def, type_length );
extern  bool            InsDead( void );
extern  bool            MoreConflicts( void );
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,
                                    type_length,int,i_flags);

static  block_num       Instance;
static  global_bit_set  Id;
static  pointer         MarkInstance(pointer);

static  block   *FindUnMarkedInstance( void )
/*******************************************/
{
    block               *blk;
    data_flow_def       *flow;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( blk->class & BLOCK_VISITED ) continue;
        flow = blk->dataflow;
        if( _GBitOverlap( Id, flow->in )
         || _GBitOverlap( Id, flow->out )
         || _GBitOverlap( Id, flow->def )
         || _GBitOverlap( Id, flow->use ) ) break;
    }
    return( blk );
}


static  void    NotVisited( void )
/********************************/
{
    block       *blk;

    blk = HeadBlock;
    while( blk != NULL ) {
        blk->class &= ~BLOCK_VISITED;
        blk->id = 0;
        blk = blk->next_block;
    }
}


extern  bool    RepOp( name **pop, name *of, name *with )
/*******************************************************/
{
    name        *op;
    name        *base;
    name        *index;
    bool        change;

    op = *pop;
    change = FALSE;
    if( op == of ) {
        *pop = with;
        change = TRUE;
    } else if( op->n.class == N_INDEXED ) {
        base = op->i.base;
        if( HasTrueBase( op ) && base == of ) {
            base = with;
            change = TRUE;
        }
        index = op->i.index;
        if( index == of ) {
            index = with;
            change = TRUE;
        }
        if( change ) {
            *pop = ScaleIndex( index, base, op->i.constant,
                                op->n.name_class, op->n.size,
                                op->i.scale, op->i.index_flags );
        }
    }
    return( change );
}


static  void    ReplaceInstances( name *of, name *with )
/******************************************************/
{
    block       *blk;
    instruction *ins;
    int         i;
    block       *replaced;

    blk = HeadBlock;
    replaced = NULL;
    while( blk != NULL ) {
        if( blk->id == Instance ) {
            ins = blk->ins.hd.next;
            while( ins->head.opcode != OP_BLOCK ) {
                i = ins->num_operands;
                while( --i >= 0 ) {
                    RepOp( &ins->operands[ i ], of, with );
                }
                if( ins->result != NULL ) {
                    RepOp( &ins->result, of, with );
                }
                ins = ins->head.next;
            }
        }
        blk = blk->next_block;
    }
}


static  bool    Split1Var( conflict_node *conf )
/**********************************************/
{
    block       *unlabeled;
    bool        change;
    name        *op;

    NotVisited();
    Instance = 0;
    _GBitAssign( Id, conf->id.out_of_block );
    for( ;; ) {
        unlabeled = FindUnMarkedInstance();
        if( unlabeled == NULL ) break;
        ++Instance;
        MarkInstance( unlabeled );
    }
    op = conf->name;
    change = FALSE;
    while( Instance > 1 ) {
        change = TRUE;
        ReplaceInstances( op, SAllocTemp( op->n.name_class, op->n.size ) );
        --Instance;
    }
    return( change );
}


static  void    CleanUp( void )
/*****************************/
{
    block       *blk;
    block_num   id;

    blk = HeadBlock;
    id = 0;
    while( blk != NULL ) {
        blk->class &= ~BLOCK_VISITED;
        blk->id = ++id;
        blk = blk->next_block;
    }
}


static  pointer MarkInstance( pointer bl )
/****************************************/
{
    block_edge          *edge;
    int                 i;
    data_flow_def       *flow;
    global_bit_set      *bitp;
    block               *blk = bl;

    if( blk->class & BLOCK_VISITED ) return NULL;
    blk->class |= BLOCK_VISITED;
    blk->id = Instance;
    flow = blk->dataflow;
    if( _GBitOverlap( flow->in, Id ) ) {
        edge = blk->input_edges;
        while( edge != NULL ) {
            bitp = &edge->source->dataflow->out;
            if( _GBitOverlap( *bitp, Id ) ) {
                SafeRecurse( MarkInstance, edge->source );
            }
            edge = edge->next_source;
        }
    }
    if( _GBitOverlap( flow->out, Id ) ) {
        i = blk->targets;
        edge = &blk->edge[ 0 ];
        while( --i >= 0 ) {
            bitp = &edge->destination->dataflow->in;
            if( _GBitOverlap( *bitp, Id ) ) {
                SafeRecurse( MarkInstance, edge->destination );
            }
            ++edge;
        }
    }
    return NULL;
}


extern  void    SplitVars( void )
/*******************************/
/* For each variable, find out if it can be split into two separate variables.*/
/* This often happens when programmers re-use variables rather than defining*/
/* a new one.*/
{
    name                *op;
    conflict_node       *conf;

    for( ;; ) {
        for( conf = ConfList; conf != NULL; conf = conf->next_conflict ) {
            op = conf->name;
            if( !( op->v.usage & USE_IN_ANOTHER_BLOCK ) ) continue;
            if( op->n.class != N_TEMP ) continue;
            if( conf->state & CONF_VISITED ) continue;
            conf->state &= ~CONFLICT_ON_HOLD;
            if( _GBitEmpty( conf->id.out_of_block ) ) continue;
            if( op->t.alias == op ) Split1Var( conf );
            _GBitInit( conf->id.out_of_block, EMPTY );
            conf->state |= CONFLICT_ON_HOLD | CONF_VISITED;
        }
        if( !MoreConflicts() ) break;
    }
    CleanUp();
    FreeConflicts();
    InsDead();
    NullConflicts( EMPTY );
    FindReferences();
    MakeConflicts();
}
