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


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "yacc.h"
#include "alloc.h"

#define INFINITY        32767

static a_look **stk, **top;

static void Reads( a_look *x )
{
    a_shift_action  *tx;
    a_look          *y;
    int             k;

    *top = x;
    x->depth = k = ++top - stk;
    for( tx = x->trans->state->trans; tx->sym; ++tx ) {
        if( !tx->sym->pro ) {
            SetBit( x->follow, tx->sym->idx );
        }
    }
    for( y = x->trans->state->look; y->trans; ++y ) {
        if( y->trans->sym->nullable ) {
            if( y->depth == 0 ) {
                Reads( y );
            }
            if( y->depth < x->depth ) {
                x->depth = y->depth;
            }
            Union( x->follow, y->follow );
        }
    }
    if( x->depth == k ) {
        do {
            (*--top)->depth = INFINITY;
            Assign( (*top)->follow, x->follow );
        } while( *top != x );
    }
}

static void CalcReads( void )
{
    a_state     *x;
    a_look      *p;

    for( x = statelist; x; x = x->next ) {
        for( p = x->look; p->trans; ++p ) {
            if( !p->depth ) {
                Reads( p );
            }
        }
    }
}

static void Nullable( void )
{
    a_sym       *sym;
    a_pro       *pro;
    an_item     *p;
    bool        nullable_added;

    for( sym = symlist; sym; sym = sym->next ) {
        sym->nullable = FALSE;
    }
    do {
        nullable_added = FALSE;
        for( sym = symlist; sym; sym = sym->next ) {
            if( !sym->nullable ) {
                for( pro = sym->pro; pro; pro = pro->next ) {
                    for( p = pro->item; p->p.sym; ++p ) {
                        if( !p->p.sym->nullable ) {
                            goto next_production;
                        }
                    }
                    /* all of the RHS symbols are nullable */
                    /* (the vacuous case means the LHS is nullable) */
                    sym->nullable = TRUE;
                    nullable_added = TRUE;
                    next_production:;
                }
            }
        }
    } while( nullable_added != FALSE );
}

static void Includes( a_look *x )
{
    a_look      *y;
    a_link      *link;
    int         k;

    *top = x;
    x->depth = k = ++top - stk;
    for( link = x->include; link; link = link->next ) {
        y = link->el;
        if( y->depth == 0 ) {
            Includes( y );
        }
        if( y->depth < x->depth ) {
            x->depth = y->depth;
        }
        Union( x->follow, y->follow );
    }
    if( x->depth == k ) {
        do {
            (*--top)->depth = INFINITY;
            Assign( (*top)->follow, x->follow );
        } while( *top != x );
    }
}

static void CalcIncludes( void )
{
    a_state         *x, *y;
    a_shift_action  *tx;
    a_look          *p, *q;
    a_sym           *sym;
    a_pro           *pro;
    an_item         *nullable, *item;
    a_link          *free;

    for( x = statelist; x; x = x->next ) {
        for( p = x->look; p->trans; ++p ) {
            p->depth = 0;
            for( pro = p->trans->sym->pro; pro; pro = pro->next ) {
                nullable = pro->item;
                for( item = pro->item; (sym = item->p.sym); ++item ) {
                    if( !sym->nullable ) {
                        nullable = item;
                    }
                }
                y = x;
                for( item = pro->item; (sym = item->p.sym); ++item ) {
                    if( !sym->pro ) {
                        for( tx = y->trans; tx->sym != sym; ++tx );
                        y = tx->state;
                    } else {
                        for( q = y->look; q->trans->sym != sym; ++q );
                        if( item >= nullable ) {
                            free = CALLOC( 1, a_link );
                            free->el = p;
                            free->next = q->include;
                            q->include = free;
                        }
                        y = q->trans->state;
                    }
                }
            }
        }
    }
    for( x = statelist; x; x = x->next ) {
        for( p = x->look; p->trans; ++p ) {
            if( !p->depth ) {
                Includes( p );
            }
        }
    }
}

static void Lookback( void )
{
    a_state         *x, *y;
    a_shift_action  *tx;
    a_look          *p;
    a_reduce_action *rx;
    a_sym           *sym;
    a_pro           *pro;
    an_item         *item;

    for( x = statelist; x; x = x->next ) {
        for( p = x->look; p->trans; ++p ) {
            for( pro = p->trans->sym->pro; pro; pro = pro->next ) {
                y = x;
                for( item = pro->item; (sym = item->p.sym); ++item ) {
                    for( tx = y->trans; tx->sym != sym; ++tx );
                    y = tx->state;
                }
                for( rx = y->redun; rx->pro !=NULL && rx->pro != pro; ++rx );
                Union( rx->follow, p->follow );
            }
        }
    }
}

static a_pro *extract_pro( an_item *p )
{
    an_item     *q;

    for( q = p; q->p.sym; ++q );
    return( q[ 1 ].p.pro );
}

static void check_for_user_hooks( a_state *state, a_shift_action *saction,
                                  index_t rule )
{
    int                 min_max_set;
    int                 all_match;
    unsigned            min;
    unsigned            max;
    unsigned            index;
    a_name              name;
    a_pro               *pro;
    a_SR_conflict       *conflict;
    a_SR_conflict_list  *cx;
    a_SR_conflict_list  *last;
    a_sym               *sym;

    if( state->kersize < 2 ) {
        return;
    }
    if( state->name.item[ 0 ] == NULL || !IsState( *state->name.item[ 0 ] ) ) {
        sym = saction->sym;
        min_max_set = 0;
        min = UINT_MAX;
        max = 0;
        for( name.item = state->name.item; *name.item; ++name.item ) {
            pro = extract_pro( *name.item );
            if( pro->SR_conflicts == NULL ) {
                /* production doesn't contain any conflicts */
                return;
            }
            for( cx = pro->SR_conflicts; cx != NULL; cx = cx->next ) {
                conflict = cx->conflict;
                if( conflict->sym != sym ) {
                    continue;
                }
                index = conflict->id;
                if( index < min ) {
                    min_max_set = 1;
                    min = index;
                }
                if( index > max ) {
                    min_max_set = 1;
                    max = index;
                }
            }
            if( ! min_max_set ) {
                /* production doesn't contain a matching conflict */
                return;
            }
        }
        for( index = min; index <= max; ++index ) {
            last = NULL;
            all_match = 1;
            for( name.item = state->name.item; *name.item; ++name.item ) {
                pro = extract_pro( *name.item );
                for( cx = pro->SR_conflicts; cx != NULL; cx = cx->next ) {
                    conflict = cx->conflict;
                    if( conflict->id != index ) {
                        continue;
                    }
                    if( conflict->sym != sym ) {
                        continue;
                    }
                    break;
                }
                if( cx == NULL ) {
                    all_match = 0;
                    break;
                }
                last = cx;
            }
            if( all_match ) {
                /* found the desired S/R conflict */
                Ambiguous( *state );
                conflict = last->conflict;
                conflict->state = state;
                conflict->shift = saction->state;
                conflict->reduce = rule;
                return;
            }
        }
    }
}

static void resolve( a_state *x, short *work, a_reduce_action **reduce )
{
    a_shift_action  *tx, *ux;
    a_reduce_action *rx;
    short int       *p, *w;
    int             i;
    a_prec          symprec, proprec, prevprec;


    w = work;
    for( rx = x->redun; rx->pro; ++rx ) {
        p = Members( rx->follow, setmembers );
        while( --p >= setmembers ) {
            if( reduce[ *p ] ) {
                prevprec = reduce[ *p ]->pro->prec;
                proprec = rx->pro->prec;
                if( !prevprec.prec || !proprec.prec
                ||  prevprec.prec == proprec.prec ) {
                    *w++ = *p;
                    /* resolve to the earliest production */
                    if( rx->pro->pidx >= reduce[ *p ]->pro->pidx ) {
                        continue;
                    }
                } else if( prevprec.prec > proprec.prec ) {
                    /* previous rule had higher precedence so leave it alone */
                    continue;
                }
            }
            reduce[ *p ] = rx;
        }
    }
    while( --w >= work ) {
        if( *w == errsym->token ) continue;
        printf( "r/r conflict in state %d on %s:\n", x->sidx, symtab[ *w ]->name);
        ++RR_conflicts;
        for( rx = x->redun; rx->pro; ++rx ) {
            if( IsBitSet( rx->follow, *w ) ) {
                showitem( rx->pro->item, "" );
            }
        }
        printf( "\n" );
        for( rx = x->redun; rx->pro; ++rx ) {
            if( IsBitSet( rx->follow, *w ) ) {
                ShowSentence( x, symtab[ *w ], rx->pro, NULL );
            }
        }
        printf( "---\n\n" );
    }
    for( tx = ux = x->trans; tx->sym; ++tx ) {
        i = tx->sym->idx;
        if( i >= nterm || !reduce[ i ] ) {
            *ux++ = *tx;
        } else {
            /* shift/reduce conflict detected */
            check_for_user_hooks( x, tx, reduce[ i ]->pro->pidx );
            symprec = tx->sym->prec;
            proprec = reduce[ i ]->pro->prec;
            if( !symprec.prec || !proprec.prec ) {
                if( tx->sym != errsym ) {
                    printf( "s/r conflict in state %d on %s:\n",
                    x->sidx, tx->sym->name );
                    ++SR_conflicts;
                    printf( "\tshift to %d\n", tx->state->sidx );
                    showitem( reduce[ i ]->pro->item, "" );
                    printf( "\n" );
                    ShowSentence( x, tx->sym, reduce[ i ]->pro, NULL );
                    ShowSentence( x, tx->sym, NULL, tx->state );
                    printf( "---\n\n" );
                }
                *ux++ = *tx;
                reduce[ i ] = NULL;
            } else {
                if( symprec.prec > proprec.prec ) {
                    *ux++ = *tx;
                    reduce[ i ] = NULL;
                } else if( symprec.prec == proprec.prec ) {
                    if( symprec.assoc == R_ASSOC ) {
                        *ux++ = *tx;
                        reduce[ i ] = NULL;
                    } else if( symprec.assoc == NON_ASSOC ) {
                        ux->sym = tx->sym;
                        ux->state = errstate;
                        ++ux;
                        reduce[ i ] = NULL;
                    }
                }
            }
        }
    }
    ux->sym = NULL;
    for( rx = x->redun; rx->pro; ++rx ) {
        Clear( rx->follow );
    }
    for( i = 0; i < nterm; ++i ) {
        if( reduce[ i ] ) {
            SetBit( reduce[ i ]->follow, i );
            reduce[ i ] = NULL;
        }
    }
}

static void Conflict( void )
{
    a_word          *set;
    a_state         *x;
    a_shift_action  *tx;
    a_reduce_action *rx;
    a_reduce_action **reduce;
    short           *work;
    int             i;

    set = CALLOC( wperset, a_word );
    reduce = CALLOC( nterm, a_reduce_action * );
    work = CALLOC( nterm, short );
    for( x = statelist; x; x = x->next ) {
        Clear( set );
        for( tx = x->trans; tx->sym; ++tx ) {
            if( !tx->sym->pro ) {
                SetBit( set, tx->sym->idx );
            }
        }
        for( rx = x->redun; rx->pro; ++rx ) {
            for( i = 0; i < wperset; ++i ) {
                if( rx->follow[ i ] & set[ i ] ) {
                    resolve( x, work, reduce );
                    goto continu;
                }
                set[ i ] |= rx->follow[ i ];
            }
        }
        continu:;
    }
    free( set );
    free( reduce );
}

void lalr1( void )
{
    a_state         *x;
    a_look          *look, *lk;
    a_shift_action  *tx;
    a_reduce_action *rx;
    a_word          *lp, *lset, *rp, *rset;

    InitSets( nterm );
    lk = look = CALLOC( nvtrans + nstate, a_look );
    lp = lset = CALLOC( nvtrans*wperset, a_word );
    rp = rset = CALLOC( nredun*wperset, a_word );
    for( x = statelist; x; x = x->next ) {
        x->look = lk;
        for( tx = x->trans; tx->sym; ++tx ) {
            if( tx->sym->pro ) {
                lk->trans = tx;
                lk->follow = lp;
                lp += wperset;
                ++lk;
            }
        }
        ++lk;
        for( rx = x->redun; rx->pro; ++rx ) {
            rx->follow = rp;
            rp += wperset;
        }
    }
    top = stk = CALLOC( nvtrans, a_look * );
    Nullable();
    CalcReads();
    CalcIncludes();
    Lookback();
    if( lk - look != nvtrans + nstate ) {
        puts( "internal error" );
    }
    if( lp - lset != nvtrans * wperset ) {
        puts( "internal error" );
    }
    if( rp - rset != nredun * wperset ) {
        puts( "internal error" );
    }
    free( look );
    free( lset );
    free( stk );
    Conflict();
    nbstate = nstate;
}

void showstates( void )
{
    int         i;

    for( i = 0; i < nstate; ++i ) {
        printf( "\n" );
        showstate( statetab[ i ] );
    }
}

void showstate( a_state *x )
{
    a_parent        *parent;
    a_shift_action  *tx;
    a_reduce_action *rx;
    unsigned        col, new_col;
    short int       *p;
    a_name          name;

    printf( "state %d:\n", x->sidx );
    col = printf( "  parent states:" );
    for( parent = x->parents; parent != NULL; parent = parent->next ) {
        col += printf( " %d", parent->state->sidx );
        if( col > 79 ) {
            printf( "\n" );
            col = 0;
        }
    }
    printf( "\n" );
    if( x->name.item[ 0 ] == NULL || !IsState( *x->name.item[ 0 ] ) ) {
        for( name.item = x->name.item; *name.item; ++name.item ) {
            showitem( *name.item, " ." );
        }
    } else {
        for( name.state = x->name.state; *name.state; ++name.state ) {
            printf( " %d", (*name.state)->sidx );
        }
        printf( "\n" );
    }
    printf( "actions:" );
    col = 8;
    for( tx = x->trans; tx->sym; ++tx ) {
        new_col = col + 1 + strlen( tx->sym->name ) + 1 + 1 + 3;
        if( new_col > 79 ) {
            putchar('\n');
            new_col -= col;
        }
        col = new_col;
        printf( " %s:s%03d", tx->sym->name, tx->state->sidx );
    }
    putchar( '\n' );
    col = 0;
    for( rx = x->redun; rx->pro; ++rx ) {
        for( p = Members( rx->follow, setmembers ); --p >= setmembers; ) {
            new_col = col + 1 + strlen( symtab[ *p ]->name );
            if( new_col > 79 ) {
                putchar('\n');
                new_col -= col;
            }
            col = new_col;
            printf( " %s", symtab[ *p ]->name );
        }
        new_col = col + 1 + 5;
        if( new_col > 79 ) {
            putchar('\n');
            new_col -= col;
        }
        col = new_col;
        printf( ":r%03d", rx->pro->pidx );
    }
    putchar( '\n' );
}
