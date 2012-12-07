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


#include <windows.h>
#include <ddeml.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "wreglbl.h"
#include "wresall.h"
#include "wre_wres.h"
#include "wremem.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wreres.h"
#include "wrestrdp.h"
#include "wregcres.h"
#include "wrenames.h"
#include "wrerenam.h"
#include "wrenew.h"
#include "wredel.h"
#include "wrestat.h"
#include "wrelist.h"
#include "wredde.h"
#include "wre_rc.h"
#include "wredlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define MAX_RETRIES 99

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WREDialogInfo {
    char         *file_name;
    WResID       *res_name;
    WResLangType  lang;
    uint_16       MemFlags;
    int           is32bit;
    uint_32       data_size;
    void         *data;
} WREDialogInfo;

typedef struct WREDialogSession {
    HCONV               server;
    HCONV               client;
    WREDialogInfo       info;
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WREResInfo          *rinfo;
} WREDialogSession;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static  WResID                  *WRECreateDialogTitle     ( void );
static  Bool                    WREAddDialogToDir         ( WRECurrentResInfo *curr );
static  WREDialogSession        *WREStartDialogSession    ( WRECurrentResInfo *curr );
static  WREDialogSession        *WREAllocDialogSession    ( void );
static  WREDialogSession        *WREFindDialogSession     ( HCONV conv );
static  WREDialogSession        *WREFindResDialogSession  ( WREResInfo *rinfo );
static  WREDialogSession        *WREFindLangDialogSession ( WResLangNode *lnode );
static  void                    WRERemoveDialogEditSession( WREDialogSession *session );
static  void                    WREFreeEditSession        ( WREDialogSession *session );
static  void                    WREDisconnectSession      ( WREDialogSession *session );
static  void                    WREBringSessionToFront    ( WREDialogSession *session );
static  void                    WREShowSession            ( WREDialogSession *session, Bool show );
static  void                    WREPokeDialogCmd          ( WREDialogSession *session, char *cmd, Bool );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static  WREDialogSession        *PendingSession = NULL;
static  LIST                    *WREDlgSessions = NULL;
static  uint_32                 WRENumDialogTitles = 0;

static void DumpEmptyResource( WREDialogSession *session )
{
    WRECurrentResInfo   curr;

    if( !session->lnode->Info.Length ) {
        curr.info = session->rinfo;
        curr.type = session->tnode;
        curr.res  = session->rnode;
        curr.lang = session->lnode;
        WRERemoveEmptyResource( &curr );
        WRESetStatusByID( -1, WRE_EMPTYREMOVED );
    }
}

WResID *WRECreateDialogTitle( void )
{
    char        *text;
    char        *title;
    WResID      *name;

    WRENumDialogTitles++;

    name = NULL;
    text = WREAllocRCString( WRE_DEFDIALOGNAME );
    if( text ) {
        title = (char *)WREMemAlloc( strlen(text) + 10 + 1 );
        if( title ) {
            title[0] = '\0';
            sprintf( title, text, WRENumDialogTitles );
            name = WResIDFromStr( title );
            WREMemFree( title );
        }
        WREFreeRCString( text );
    }

    return( name );
}

Bool WREAddDialogToDir( WRECurrentResInfo *curr )
{
    WResLangType       lang;
    int                dup, num_retries;
    WResID            *rname, *tname;
    Bool               ok, tname_alloc;

    ok          = TRUE;
    tname_alloc = FALSE;

    WREGetCurrentResource( curr );

    if( !curr->info ) {
        curr->info = WRECreateNewResource( NULL );
        ok = ( curr->info != NULL );
    }

    if( ok ) {
        if( curr->info->current_type == (uint_16)RT_DIALOG ) {
            tname = &curr->type->Info.TypeName;
        } else {
            tname = WResIDFromNum( (uint_16)RT_DIALOG );
            tname_alloc = TRUE;
        }
        lang.lang    = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
    }

    if( ok ) {
        dup         = TRUE;
        num_retries = 0;
        rname       = NULL;
        while( ok && dup && ( num_retries <= MAX_RETRIES ) ) {
            rname = WRECreateDialogTitle();
            ok = ( rname != NULL );
            if( ok ) {
                ok = WRENewResource( curr, tname, rname, DEF_MEMFLAGS, 0, 0,
                                     &lang, &dup, (uint_16)RT_DIALOG,
                                     tname_alloc );
                if( !ok && dup ) {
                    ok = TRUE;
                }
                num_retries++;
            }
            if( rname ) {
                WREMemFree( rname );
            }
        }
        if( dup ) {
            WREDisplayErrorMsg( WRE_CANTFINDUNUSEDNAME );
        }
    }

    if( ok ) {
        curr->info->modified = TRUE;
    }

    if( tname_alloc ) {
        WREMemFree( tname );
    }

    return( ok );
}

Bool WRENewDialogResource( void )
{
    WRECurrentResInfo  curr;
    Bool               ok;

    ok = WREAddDialogToDir( &curr );

    if( ok ) {
        ok = ( WREStartDialogSession( &curr ) != NULL );
    }

    return( ok );
}

Bool WREDumpPendingDialogSession( void )
{
    Bool                ret;

    ret = TRUE;

    if( PendingSession != NULL ) {
        DumpEmptyResource( PendingSession );
        WREFreeEditSession( PendingSession );
        PendingSession = NULL;
        WRESetPendingService( NoServicePending );
        WREDisplayErrorMsg( WRE_DLGSESSIONKILLED );
    }

    return( ret );
}

Bool WREEndEditDialogResource( HCONV conv )
{
    WREDialogSession    *session;
    Bool                ret;

    ret = FALSE;

    session = WREFindDialogSession( conv );

    if( session ) {
        ret = TRUE;
        DumpEmptyResource( session );
        WRERemoveDialogEditSession( session );
    }

    return( ret );
}

Bool WRECommitDialogSession( HCONV server, HCONV client )
{
    Bool        ok;

    ok = ( ( client != (HCONV)NULL ) && ( server != (HCONV)NULL ) &&
           ( PendingSession != NULL ) );

    if( ok ) {
        WREInsertObject( &WREDlgSessions, PendingSession );
        PendingSession->server = server;
        PendingSession->client = client;
    } else {
        WREFreeEditSession( PendingSession );
    }

    WRESetPendingService( NoServicePending );
    PendingSession = NULL;

    return( ok );
}

Bool WREGetDlgSessionFileName( HCONV server, void **data, uint_32 *size )
{
    WREDialogSession *session;

    if( !data || !size ) {
        return( FALSE );
    }

    session = WREFindDialogSession( server );
    if( !session ) {
        return( FALSE );
    }

    *data = WREStrDup( session->info.file_name );
    if( *data ) {
        *size = strlen( *data ) + 1;
    }

    return( TRUE );
}

Bool WREGetDlgSessionResName( HCONV server, void **data, uint_32 *size )
{
    WREDialogSession *session;

    if( !data || !size ) {
        return( FALSE );
    }

    session = WREFindDialogSession( server );
    if( !session ) {
        return( FALSE );
    }

    if( !WRWResID2Mem( session->info.res_name, data, size,
                       session->info.is32bit ) ) {
        return( FALSE );
    }

    return( TRUE );
}

Bool WREGetDlgSessionIs32Bit( HCONV server, void **data, uint_32 *size )
{
    WREDialogSession *session;

    if( !data || !size ) {
        return( FALSE );
    }

    session = WREFindDialogSession( server );
    if( !session ) {
        return( FALSE );
    }

    if( !session->info.is32bit ) {
        return( FALSE );
    }

    *size = sizeof(Bool);
    *data = WREMemAlloc( *size );
    if( *data == NULL ) {
        return( FALSE );
    }
    memcpy( *data, &session->info.is32bit, *size );

    return( TRUE );
}

Bool WREGetDlgSessionData( HCONV server, void **data, uint_32 *size )
{
    WREDialogSession *session;

    if( !data || !size ) {
        return( FALSE );
    }

    session = WREFindDialogSession( server );
    if( !session ) {
        return( FALSE );
    }

    if( session->info.data == NULL  ) {
        *data = NULL;
        *size = 0;
        return( TRUE );
    }

    *size = session->info.data_size;
    *data = WREMemAlloc( *size );
    if( *data == NULL ) {
        return( FALSE );
    }
    memcpy( *data, session->info.data, *size );

    return( TRUE );
}

Bool WRESetDlgSessionResName( HCONV server, HDDEDATA hdata )
{
    WREDialogSession    *session;
    WResID              *name;
    void                *data;
    uint_32             size;
    Bool                ok;

    ok = ( ( server != (HCONV)NULL ) && ( hdata != (HDDEDATA)NULL ) );

    if( ok ) {
        session = WREFindDialogSession( server );
        ok = ( session != NULL );
    }

    if( ok ) {
        ok = WREHData2Mem( hdata, &data, &size );
    }

    if( ok ) {
        name = WRMem2WResID( data, session->info.is32bit );
        ok = ( name != NULL );
    }

    if( ok ) {
        ok = WRERenameWResResNode( session->tnode, &session->rnode, name );
    }

    if( ok ) {
        session->rinfo->modified    = TRUE;
        WRESetResNamesFromType( session->rinfo, (uint_16)RT_DIALOG,
                                TRUE, name, 0 );
    }

    if( data ) {
        WREMemFree( data );
    }

    if( name ) {
        WREMemFree( name );
    }

    return( ok );
}

Bool WRESetDlgSessionResData( HCONV server, HDDEDATA hdata )
{
    WREDialogSession    *session;
    void                *data;
    uint_32             size;
    Bool                ok;

    ok = ( ( server != (HCONV)NULL ) && ( hdata != (HDDEDATA)NULL ) );

    if( ok ) {
        session = WREFindDialogSession( server );
        ok = ( session != NULL );
    }

    if( ok ) {
        ok = WREHData2Mem( hdata, &data, &size );
    }

    if( ok ) {
        if( session->lnode->data ) {
            WREMemFree( session->lnode->data );
        }
        session->lnode->data        = data;
        session->lnode->Info.Length = size;
        session->rinfo->modified    = TRUE;
    }

    return( ok );
}

WREDialogSession *WREStartDialogSession( WRECurrentResInfo *curr )
{
    WREDialogSession    *session;
    int                 is32bit;

    if( !curr ) {
        return( NULL );
    }

    is32bit = curr->info->is32bit;

    session = WREAllocDialogSession();
    if( !session ) {
        return( NULL );
    }

    session->info.file_name = WREStrDup( WREGetQueryName( curr->info ) );
    session->info.res_name  = WRECopyWResID( &curr->res->Info.ResName );
    session->info.lang      = curr->lang->Info.lang;
    session->info.MemFlags  = curr->lang->Info.MemoryFlags;
    session->info.data_size = curr->lang->Info.Length;
    session->info.data      = curr->lang->data;
    session->info.is32bit   = is32bit;

    session->tnode = curr->type;
    session->rnode = curr->res;
    session->lnode = curr->lang;
    session->rinfo = curr->info;

    WRESetPendingService( DialogService );

    PendingSession = session;

    if( WinExec( "wde.exe -dde", SW_SHOW ) < 32 ) {
        WREDisplayErrorMsg( WRE_DLGEDITNOTSPAWNED );
        WREFreeEditSession( session );
        PendingSession = NULL;
        WRESetPendingService( NoServicePending );
    }

    return( session );
}

Bool WREEditDialogResource( WRECurrentResInfo *curr )
{
    Bool                ok;
    WREDialogSession    *session;

    ok = ( curr && curr->lang );

    if( ok ) {
        session = WREFindLangDialogSession( curr->lang );
        if( session ) {
            WREBringSessionToFront( session );
            return( TRUE );
        }
    }

    if( ok ) {
        if( !curr->lang->data && curr->lang->Info.Length ) {
            curr->lang->data = WREGetCurrentResData( curr );
            ok = ( curr->lang->data != NULL );
        }
    }

    if( ok ) {
        ok = ( WREStartDialogSession( curr ) != NULL );
    }

    return( ok );
}

Bool WREEndAllDialogSessions( Bool fatal_exit )
{
    WREDialogSession    *session;
    LIST                *slist;
    Bool                ok;

    _wre_touch(fatal_exit);

    ok = TRUE;

    if( WREDlgSessions ) {
        for( slist = WREDlgSessions; ok && slist; slist = ListNext(slist) ) {
            session = (WREDialogSession *) ListElement(slist);
            if( session ) {
                WREDisconnectSession( session );
                WREFreeEditSession( session );
            }
        }
        if( ok ) {
            ListFree( WREDlgSessions );
            WREDlgSessions = NULL;
        }
    }

    return( ok );
}

void WREEndLangDialogSession( WResLangNode *lnode )
{
    WREDialogSession    *session;

    session = WREFindLangDialogSession( lnode );
    while( session ) {
        WREDisconnectSession( session );
        WRERemoveDialogEditSession( session );
        session = WREFindLangDialogSession( lnode );
    }
}

void WREEndResDialogSessions( WREResInfo *rinfo )
{
    WREDialogSession    *session;

    session = WREFindResDialogSession( rinfo );
    while( session ) {
        WREDisconnectSession( session );
        WRERemoveDialogEditSession( session );
        session = WREFindResDialogSession( rinfo );
    }
}

WREDialogSession *WREAllocDialogSession( void )
{
    WREDialogSession *session;

    session = (WREDialogSession *) WREMemAlloc( sizeof(WREDialogSession) );

    if( session ) {
        memset( session, 0, sizeof(WREDialogSession) );
    }

    return( session );
}

WREDialogSession *WREFindDialogSession( HCONV conv )
{
    WREDialogSession *session;
    LIST             *slist;

    for( slist = WREDlgSessions; slist; slist = ListNext(slist) ) {
        session = (WREDialogSession *) ListElement(slist);
        if( ( session->server == conv ) || ( session->client == conv ) ) {
            return( session );
        }
    }

    return( NULL );
}

WREDialogSession *WREFindResDialogSession( WREResInfo *rinfo )
{
    WREDialogSession *session;
    LIST             *slist;

    for( slist = WREDlgSessions; slist; slist = ListNext(slist) ) {
        session = (WREDialogSession *) ListElement(slist);
        if( session->rinfo == rinfo ) {
            return( session );
        }
    }

    return( NULL );
}

WREDialogSession *WREFindLangDialogSession( WResLangNode *lnode )
{
    WREDialogSession *session;
    LIST             *slist;

    for( slist = WREDlgSessions; slist; slist = ListNext(slist) ) {
        session = (WREDialogSession *) ListElement(slist);
        if( session->lnode == lnode ) {
            return( session );
        }
    }

    return( NULL );
}

void WRERemoveDialogEditSession( WREDialogSession *session )
{
    if( session ) {
        ListRemoveElt( &WREDlgSessions, session );
        WREFreeEditSession( session );
    }
}

void WREFreeEditSession( WREDialogSession *session )
{
    if( session ) {
        if( session->info.file_name ) {
            WREMemFree( session->info.file_name );
        }
        if( session->info.res_name ) {
            WREMemFree( session->info.res_name );
        }
        WREMemFree( session );
    }
}

void WREDisconnectSession( WREDialogSession *session )
{
    if( session ) {
        WREPokeDialogCmd( session, "endsession", TRUE );
        DumpEmptyResource( session );
        if( session->server != (HCONV)NULL ) {
            DdeDisconnect( session->server );
            session->server = (HCONV)NULL;
        }
        if( session->client != (HCONV)NULL ) {
            DdeDisconnect( session->client );
            session->client = (HCONV)NULL;
        }
    }
}

void WREBringSessionToFront( WREDialogSession *session )
{
    WREPokeDialogCmd( session, "bringtofront", FALSE );
}

void WREShowAllDialogSessions( Bool show )
{
    WREDialogSession    *session;
    LIST                *slist;

    if( WREDlgSessions ) {
        for( slist = WREDlgSessions; slist; slist = ListNext(slist) ) {
            session = (WREDialogSession *) ListElement(slist);
            if( session ) {
                WREShowSession( session, show );
            }
        }
    }
}

void WREShowSession( WREDialogSession *session, Bool show )
{
    if( show ) {
        WREPokeDialogCmd( session, "show", FALSE );
    } else {
        WREPokeDialogCmd( session, "hide", FALSE );
    }
}

void WREPokeDialogCmd( WREDialogSession *session, char *cmd, Bool retry )
{
    if( session && cmd ) {
        WREPokeData( session->client, cmd, strlen(cmd) + 1, retry );
    }
}

