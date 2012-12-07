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
* Description:  Manage basic block edges.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "cgmem.h"

extern  block           *MakeBlock(label_handle,block_num);
extern  void            SuffixIns(instruction *,instruction *);
extern  void            RemoveIns(instruction *);
extern  label_handle    AskForNewLabel(void);
extern  byte            *Copy( void *, void *, uint );
extern  void            FixBlockIds(void);

extern  void    PointEdge( block_edge *edge, block *new_dest )
/*************************************************************
    Make edge point to block new_dest
*/
{
    /* hook edge into new destination's input list*/
    edge->next_source = new_dest->input_edges;
    new_dest->input_edges = edge;
    new_dest->inputs++;
    /* point edge at new block*/
    edge->destination = new_dest;
    edge->flags = DEST_IS_BLOCK;
    edge->source->targets++;
}

extern  void    RemoveEdge( block_edge *edge )
/*********************************************
    Remove the given edge from it's block.
*/
{
    block_edge  *curr;
    block_edge  **owner;

    /* unhook edge from its old destination's input list*/
    if( edge->flags & DEST_IS_BLOCK ) {
        owner = &edge->destination->input_edges;
        for(;;) {
            curr = *owner;
            if( curr == edge ) break;
            owner = &(*owner)->next_source;
        }
        *owner = curr->next_source;
        edge->destination->inputs--;
    }
    edge->source->targets--;
}

extern  void    MoveEdge( block_edge *edge, block *new_dest )
/************************************************************
    Move edge to point to block new_dest
*/
{
    RemoveEdge( edge );
    PointEdge( edge, new_dest );
}

extern  block   *SplitBlock( block *blk, instruction *ins )
/**********************************************************
    Split a block in two before the given instruction. The first block
    will simply jump to the second block, which will receive all the
    edges from the previous block.
*/
{
    block       *new_blk;
    block_edge  *edge;
    instruction *old;
    block_num   i;

    new_blk = MakeBlock( AskForNewLabel(), blk->targets );
    Copy( blk, new_blk, sizeof(block)+(sizeof(block_edge)*(blk->targets-1)) );
    new_blk->next_block = blk->next_block;
    new_blk->prev_block = blk;
    blk->next_block = new_blk;
    new_blk->next_block->prev_block = new_blk;
    blk->class = blk->class & ~( CONDITIONAL | RETURN | SELECT |
                                 LOOP_EXIT | UNKNOWN_DESTINATION );
    blk->class |= JUMP;
    blk->targets = 1;
    new_blk->class = new_blk->class & ~( LOOP_HEADER );
    new_blk->inputs = 0;
    new_blk->input_edges = NULL;
    new_blk->id = NO_BLOCK_ID;
    PointEdge( &blk->edge[ 0 ], new_blk );
    edge = &new_blk->edge[ 0 ];
    i = 0;
    while( i < new_blk->targets ) {
        edge->source = new_blk;
        edge++;
        i++;
    }
    while( ins->head.opcode != OP_BLOCK ) {
        old = ins;
        ins = ins->head.next;
        RemoveIns( old );
        SuffixIns( new_blk->ins.hd.prev, old );
    };
    FixBlockIds();
    return( new_blk );
}

typedef struct edge_entry {
    struct edge_entry   *next;
    block_edge          *edge;
} edge_entry;

typedef struct edge_stack {
    edge_entry          *top;
} edge_stack;

extern  edge_stack      *EdgeStackInit( void )
/********************************************/
{
    edge_stack          *stk;

    stk = CGAlloc( sizeof( edge_stack ) );
    stk->top = NULL;
    return( stk );
}

extern  bool            EdgeStackEmpty( edge_stack *stk )
/*******************************************************/
{
    return( stk->top == NULL );
}

extern  void            EdgeStackPush( edge_stack *stk, block_edge *edge )
/************************************************************************/
{
    edge_entry          *new_entry;

    new_entry = CGAlloc( sizeof( edge_entry ) );
    new_entry->edge = edge;
    new_entry->next = stk->top;
    stk->top = new_entry;
}

extern  block_edge      *EdgeStackPop( edge_stack *stk )
/******************************************************/
{
    edge_entry          *top;
    block_edge          *edge;

    if( stk->top != NULL ) {
        top = stk->top;
        edge = top->edge;
        stk->top = top->next;
        CGFree( top );
        return( edge );
    }
    return( NULL );
}

extern  void            EdgeStackFini( edge_stack *stk )
/******************************************************/
{
    while( !EdgeStackEmpty( stk ) ) {
        EdgeStackPop( stk );
    }
    CGFree( stk );
}
