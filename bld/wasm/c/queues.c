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
* Description:  Output OBJ queues routines
*
****************************************************************************/

#include "asmglob.h"

#include "asmalloc.h"
#include "mangle.h"
#include "directiv.h"
#include "queues.h"
#include "objprs.h"
#include "namemgr.h"
#include "womputil.h"

#include "myassert.h"

typedef struct queuenode {
    void *next;
    void *data;
} queuenode;

static qdesc   *LnameQueue  = NULL;   // queue of LNAME structs
static qdesc   *PubQueue    = NULL;   // queue of pubdefs
static qdesc   *AliasQueue  = NULL;   // queue of aliases
static qdesc   *LinnumQueue = NULL;   // queue of linnum_data structs

static void QAddItem( qdesc **queue, void *data )
/***********************************************/
{
    struct queuenode    *node;

    node = AsmAlloc( sizeof( queuenode ) );
    node->data = data;
    if( *queue == NULL ) {
        *queue = AsmAlloc( sizeof( qdesc ) );
        QInit( *queue );
    }
    QEnqueue( *queue, node );
}

static long QCount( qdesc *q )
/****************************/
/* count the # of entries in the queue, if the retval is -ve we have an error */
{
    long        count = 0;
    queuenode   *node;

    if( q == NULL )
        return( 0 );
    for( node = q->head; node != NULL; node = node->next ) {
        if( ++count < 0 ) {
            return( -1 );
        }
    }
    return( count );
}

static char **NameArray;

const char *NameGet( uint_16 hdl )
/********************************/
// WOMP callback routine
{
    return( NameArray[hdl] );
}

void AddPublicData( dir_node *dir )
/*********************************/
{
    dir->sym.public = TRUE;
    QAddItem( &PubQueue, dir );
}

void AddPublicProc( dir_node *dir )
/*********************************/
{
    QAddItem( &PubQueue, dir );
}

bool GetPublicData( void )
/************************/
{
    obj_rec             *objr;
    struct queuenode    *start;
    struct queuenode    *curr;
    struct queuenode    *last;
    dir_node            *curr_seg;
    dir_node            *dir;
    struct pubdef_data  *d;
    unsigned char       cmd;
    uint                i;

    if( PubQueue == NULL )
        return( FALSE );

    for( start = PubQueue->head; start != NULL; start = last ) {
        dir = (dir_node *)start->data;
        cmd = ( dir->sym.public ) ? CMD_PUBDEF : CMD_STATIC_PUBDEF;
        objr = ObjNewRec( cmd );
        objr->is_32 = FALSE;
        objr->d.pubdef.free_pubs = TRUE;
        objr->d.pubdef.num_pubs = 0;
        objr->d.pubdef.base.frame = 0;
        objr->d.pubdef.base.grp_idx = 0;
        objr->d.pubdef.base.seg_idx = 0;
        curr_seg = (dir_node *)dir->sym.segment;
        if( curr_seg != NULL ) {
            objr->d.pubdef.base.seg_idx = curr_seg->e.seginfo->idx;
            if( curr_seg->e.seginfo->group != NULL ) {
                objr->d.pubdef.base.grp_idx = ((dir_node *)curr_seg->e.seginfo->group)->e.grpinfo->idx;
            }
        }
        for( curr = start; curr != NULL; curr = curr->next ) {
            if( objr->d.pubdef.num_pubs > MAX_PUB_SIZE )
                break;
            dir = (dir_node *)curr->data;
            if( (dir_node *)dir->sym.segment != curr_seg )
                break;
            if( dir->sym.state == SYM_PROC ) {
                if( dir->sym.public ) {
                    if( cmd == CMD_STATIC_PUBDEF ) {
                        break;
                    }
                } else {
                    if( cmd == CMD_PUBDEF ) {
                        break;
                    }
                }
            } else {
                if( cmd == CMD_STATIC_PUBDEF ) {
                    break;
                }
            }
            if( dir->sym.offset > 0xffffUL ) {
                objr->is_32 = TRUE;
            }
            objr->d.pubdef.num_pubs++;
        }
        last = curr;

        objr->d.pubdef.pubs = d = AsmAlloc( objr->d.pubdef.num_pubs * sizeof( struct pubdef_data ) );
        NameArray = AsmAlloc( objr->d.pubdef.num_pubs * sizeof( char * ) );
        for( i = 0, curr = start; curr != last; curr = curr->next, ++i ) {
            dir = (dir_node *)curr->data;
            NameArray[ i ] = Mangle( &dir->sym, NULL );
            d->name = i;
            if( dir->sym.state != SYM_CONST ) {
                d->offset = dir->sym.offset;
            } else {
                if( dir->e.constinfo->data[0].token != T_NUM  ) {
                    AsmWarn( 2, PUBLIC_CONSTANT_NOT_NUMERIC );
                    d->offset = 0;
                } else {
                    d->offset = dir->e.constinfo->data[0].u.value;
                }
            }
            d->type.idx = 0;
            ++d;
        }
        d = objr->d.pubdef.pubs;
        write_record( objr, TRUE );
        for( i = 0; i < objr->d.pubdef.num_pubs; i++ ) {
            AsmFree( NameArray[ i ] );
        }
        AsmFree( NameArray );
    }
    return( TRUE );
}

static void FreePubQueue( void )
/******************************/
{
    if( PubQueue != NULL ) {
        while( PubQueue->head != NULL ) {
            AsmFree( QDequeue( PubQueue ) );
        }
        AsmFree( PubQueue );
    }
}

void AddAliasData( char *data )
/*****************************/
{
    QAddItem( &AliasQueue, data );
}

char *GetAliasData( bool first )
/******************************/
{
    static queuenode    *node;
    char                *p;

    if( AliasQueue == NULL )
        return( NULL );
    if( first )
        node = AliasQueue->head;
    if( node == NULL )
        return( NULL );
    p = node->data ;
    node = node->next;
    return( p );
}

static void FreeAliasQueue( void )
/********************************/
{
    if( AliasQueue != NULL ) {
        while( AliasQueue->head != NULL ) {
            queuenode   *node;

            node = QDequeue( AliasQueue );
            AsmFree( node->data );
            AsmFree( node );
        }
        AsmFree( AliasQueue );
    }
}

void AddLnameData( dir_node *dir )
/********************************/
{
    QAddItem( &LnameQueue, dir );
}

bool GetLnameData( obj_rec *objr )
/********************************/
{
    queuenode       *curr;
    dir_node        *dir;
    unsigned        len;

    if( LnameQueue == NULL )
        return( FALSE );

    len = 0;
    for( curr = LnameQueue->head; curr != NULL ; curr = curr->next ) {
        dir = (dir_node *)(curr->data);
        len += strlen( dir->sym.name ) + 1;
    }
    ObjAllocData( objr, len );
    for( curr = LnameQueue->head; curr != NULL ; curr = curr->next ) {
        dir = (dir_node *)curr->data;
        len = strlen( dir->sym.name );
        ObjPutName( objr, dir->sym.name, len );
        objr->d.lnames.num_names++;
        switch( dir->sym.state ) {
        case SYM_GRP:
            dir->e.grpinfo->idx = objr->d.lnames.num_names;
            break;
        case SYM_SEG:
            dir->e.seginfo->idx = objr->d.lnames.num_names;
            break;
        case SYM_CLASS_LNAME:
            dir->e.lnameinfo->idx = objr->d.lnames.num_names;
            break;
        }
    }
    return( TRUE );
}

static void FreeLnameQueue( void )
/********************************/
{
    dir_node *dir;
    queuenode *node;

    if( LnameQueue != NULL ) {
        while( LnameQueue->head != NULL ) {
            node = QDequeue( LnameQueue );
            dir = (dir_node *)node->data;
            if( dir->sym.state == SYM_CLASS_LNAME ) {
                AsmFree( dir->e.lnameinfo );
                AsmFree( dir->sym.name );
                AsmFree( dir );
            }
            AsmFree( node );
        }
        AsmFree( LnameQueue );
    }
}

void AddLinnumData( struct line_num_info *data )
/**********************************************/
{
    QAddItem( &LinnumQueue, data );
}

int GetLinnumData( struct linnum_data **ldata, bool *need32 )
/***********************************************************/
{
    queuenode               *node;
    struct line_num_info    *next;
    int                     count, i;

    count = QCount( LinnumQueue );
    if( count == 0 )
        return( count );
    *need32 = FALSE;
    *ldata = AsmAlloc( count * sizeof( struct linnum_data ) );
    for( i = 0; i < count; i++ ) {
        node = QDequeue( LinnumQueue );
        next = (struct line_num_info *)(node->data);
        if( *ldata != NULL ) {
            (*ldata)[i].number = next->number;
            (*ldata)[i].offset = next->offset;
            if( next->offset > 0xffffUL ) {
                *need32 = TRUE;
            }
        }
        AsmFree( next );
        AsmFree( node );
    }
    AsmFree( LinnumQueue );
    LinnumQueue = NULL;
    return( count );
}

void FreeAllQueues( void )
/************************/
{
    FreePubQueue();
    FreeAliasQueue();
    FreeLnameQueue();
}
