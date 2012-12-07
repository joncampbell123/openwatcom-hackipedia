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
* Description:  Miscellaneous odds and ends that don't have a home.
*
****************************************************************************/


#include <string.h>
#include "dbgdefn.h"
#include "dbgtoken.h"
#include "dbgerr.h"
#include "dbgtoggl.h"
#include "dbginfo.h"
#include "dbgbreak.h"
#include "dbgmem.h"
#include "dbgreg.h"
#include "trpcore.h"
#include "dui.h"
#include "dbglit.h"
#include "i64.h"


extern char             *StrCopy( char *, char * );
extern unsigned int     ReqExpr( void );
extern unsigned_64      ReqU64Expr( void );
extern void             ReqMemAddr( memory_expr ,address *);
extern unsigned         OptExpr( unsigned );
extern unsigned int     ScanCmd( char * );
extern void             DebugExit( void );
extern void             ReqEOC( void );
extern bool             ScanQuote( char **, unsigned int * );
extern bool             ScanItem( bool, char **, unsigned int * );
extern void             Scan( void );
extern bool             ScanEOC( void );
extern unsigned         Go( bool );
extern void             PopEntry( void );
extern void             NormalExpr( void );
extern cmd_list         *AllocCmdList( char *, unsigned int );
extern void             FreeCmdList( cmd_list * );
extern char             *GetCmdEntry( char *, int , char * );
extern char             *GetCmdName( int );
extern void             FlipScreen( void );
extern void             DbgUpdate( update_list );
extern void             PushCmdList( cmd_list * );
extern address          GetRegIP( void );
extern void             RecordSetRegIP( address );
extern char             *ScanPos( void );
extern char             *ReScan( char * );
extern void             DUIWndUser( void );
extern unsigned         RemoteReadUserKey( unsigned );
extern void             ReadDbgRegs( void );
extern void             WriteDbgRegs( void );
extern dtid_t           RemoteGetNextThread( dtid_t, unsigned * );
extern dtid_t           RemoteSetThreadWithErr( dtid_t, unsigned * );
extern void             RemoteThdName( dtid_t, char * );
extern void             TraceKill( void );
extern unsigned         SetCurrRadix( unsigned );
extern address          ReturnAddress( void );
extern void             SetCodeDot( address );
extern void             ChkExpr( void );
extern bool             AdvMachState( int );
extern void             CopyInpFlags( void );
extern void             FlushEOC( void );
extern void             RecordCommand( char *startpos, int cmd );
extern void             RawScanInit( void );
extern char             RawScanChar( void );
extern void             RawScanAdvance( void );
extern void             RawScanFini( void );
extern char             *CnvULongHex( unsigned long value, char *p );
extern int              AddrComp( address a, address b );

extern char             *CmdStart;
extern brkp             UserTmpBrk;
extern brkp             DbgTmpBrk;
extern char             *TxtBuff;
extern tokens           CurrToken;
extern char             OnOffNameTab[];
extern debug_level      DbgLevel;
extern screen_state     ScrnState;
extern thread_state     *HeadThd;
extern thread_state     *ExecThd;
extern machine_state    *DbgRegs;
extern system_config    SysConfig;
extern address          NilAddr;


static char ElseifTab[]         = { "ELSEIF\0" };
static char ElseTab[]           = { "ELSE\0" };
static char GoOptionTab[]       = { "Keep\0Noflip\0Until\0" };
static char ThreadOps[]         = { "Show\0Freeze\0Thaw\0Change\0" };

enum {
    KEEP = 1,
    NOFLIP,
    UNTIL
};


void Flip( unsigned wait )
{
    char        ch;

    if( !(ScrnState & USR_SCRN_VISIBLE) ) {
        DUIWndUser();
    }
    if( !(ScrnState & DBG_SCRN_VISIBLE) ) {
        ch = RemoteReadUserKey( wait );
    }
}

void ProcFlip( void )
{
    char        *old;
    unsigned    wait;
    unsigned    cmd;

    wait = 0;
    if( !ScanEOC() ) {
        old = ScanPos();
        cmd = ScanCmd( OnOffNameTab );
        switch( cmd ) {
        case 1:
            ReqEOC();
            _SwitchOn( SW_FLIP );
            return;
        case 2:
            ReqEOC();
            _SwitchOff( SW_FLIP );
            return;
        default:
            ReScan( old );
            wait = ReqExpr();
            ReqEOC();
            break;
        }
    }
    Flip( wait );
}


void ConfigFlip( void )
{
    char *p;

    ReqEOC();
    p = StrCopy( GetCmdName( CMD_FLIP ), TxtBuff );
    *p++ = ' ';
    GetCmdEntry( &OnOffNameTab, _IsOn( SW_FLIP ) ? 1 : 2, p );
    DUIDlgTxt( TxtBuff );
}

/*
 * ProcRemark - process a remark command
 */

void ProcRemark( void )
{
    char        c;

    RawScanInit();
    for( ;; ) {
        c = RawScanChar();
        if( c == '\0' ) break;
        RawScanAdvance();
        /* The CR character indicates end of line in a multi-line command
           list. */
        if( c == '\r' ) break;
    }
    RawScanFini();
}



/*
 * ProcDo - process do command
 */

void ProcDo( void )
{
    NormalExpr();
    PopEntry();
    ReqEOC();
}


void ProcAssign( void )
{
    char        *startpos;

    if( !AdvMachState( ACTION_ASSIGNMENT ) ) {
        FlushEOC();
        return;
    }
    startpos = ScanPos();
    NormalExpr();
    PopEntry();
    ReqEOC();
    RecordCommand( startpos, CMD_ASSIGN );
}


static void SetTempBreak( address addr )
{
    NullStatus( &UserTmpBrk );
    UserTmpBrk.status.b.has_address = TRUE;
    UserTmpBrk.status.b.active = TRUE;
    UserTmpBrk.loc.addr = addr;
}


void GoToAddr( address addr )
{
    if( IS_NIL_ADDR( addr ) ) return;
    SetTempBreak( addr );
    Go( TRUE );
    NullStatus( &UserTmpBrk );
}


void StepIntoFunction( char *func )
{
    char        *old;
    address     stop;

    old = ReScan( func );
    ReqMemAddr( EXPR_CODE, &stop );
    ReScan( old );
    GoToAddr( stop );
}


void SkipToAddr( address addr )
{
    if( IS_NIL_ADDR( addr ) ) return;
    if( !AdvMachState( ACTION_MODIFY_IP ) ) return;
    RecordSetRegIP( addr );
    DbgUpdate( UP_CSIP_CHANGE | UP_REG_CHANGE );
}


void GoToReturn( void )
{
    address     ra;

    ra = ReturnAddress();
    if( !IS_NIL_ADDR( ra ) ) {
        GoToAddr( ra );
    } else {
        Error( ERR_NONE, LIT( ERR_NO_RETURN_ADDRESS ) );
    }
}


/*
 * ProcGo -- process go command
 */

void ProcGo( void )
{
    address     start;
    address     stop;
    bool        have_start;
    bool        have_stop;
    bool        have_keep;
    bool        doflip;
    bool        flip_on;
    bool        until;
    unsigned    conditions;
    int         count;

    have_keep = FALSE;
    doflip = TRUE;
    until = FALSE;
    while( CurrToken == T_DIV ) {
        Scan();
        switch( ScanCmd( GoOptionTab ) ) {
        case KEEP:
            have_keep = TRUE;
            break;
        case NOFLIP:
            doflip = FALSE;
            break;
        case UNTIL:
            until = TRUE;
            break;
        default:
            Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_GO ) );
        }
    }
    if( !have_keep ) TraceKill();
    NullStatus( &DbgTmpBrk );
    start = GetRegIP();
    have_stop = FALSE;
    have_start = FALSE;
    if( !ScanEOC() ) {
        stop = start;
        ReqMemAddr( EXPR_GIVEN, &stop );
        if( CurrToken == T_COMMA ) {
            start = stop;
            have_start = TRUE;
            Scan();
            if( !ScanEOC() ) {
                ReqMemAddr( EXPR_GIVEN, &stop );
                have_stop = TRUE;
            }
        } else {
            have_stop = TRUE;
        }
    }
    ReqEOC();
    count = 0;
    do {
        if( have_stop ) {
            if( have_keep ) {
                if( UserTmpBrk.status.b.has_address ) {
                    NullStatus( &UserTmpBrk );
                    UserTmpBrk.status.b.has_address = TRUE;
                } else {
                    NullStatus( &UserTmpBrk );
                }
                Error( ERR_NONE, LIT( ERR_INCONSISTENT_GO ) );
            }
            SetTempBreak( stop );
        } else if( have_keep ) {
            if( UserTmpBrk.status.b.has_address ) {
                UserTmpBrk.status.b.active = TRUE;
            }
        } else {
            NullStatus( &UserTmpBrk );
        }
        if( have_start ) {
            RecordSetRegIP( start );
        }
        flip_on = _IsOn( SW_FLIP );
        if( !doflip ) {
            _SwitchOff( SW_FLIP );
        }
        conditions = Go( doflip );
        if( flip_on ) _SwitchOn( SW_FLIP );
        if( UserTmpBrk.status.b.has_address ) {
            NullStatus( &UserTmpBrk );
            UserTmpBrk.status.b.has_address = TRUE;
        } else {
            NullStatus( &UserTmpBrk );
        }
    } while( until && (conditions & COND_BREAK) && AddrComp( stop, GetRegIP() ) != 0 && ++count < 100 );
}

/*
 * ProcSkip -- process skip command
 */

void ProcSkip( void )
{
    address    start;

    ReqMemAddr( EXPR_GIVEN, &start );
    ReqEOC();
    SkipToAddr( start );
}

/*
 * ProcIf -- process if command
 */

void ProcIf( void )
{
    char                *start;
    unsigned            len;
    char                *true_start;
    unsigned            true_len;
    unsigned_64         res;
    cmd_list            *cmd;
    bool                have_true;

    true_len = 0;
    have_true = FALSE;
    for( ;; ) {
        if( have_true ) {
            ChkExpr();
            U64Clear( res );
        } else {
            res = ReqU64Expr();
        }
        if( !ScanQuote( &start, &len ) ) {
            Error( ERR_LOC, LIT( ERR_WANT_COMMAND_LIST ) );
        }
        if( U64Test( &res ) != 0 ) {
            true_start = start;
            true_len   = len;
            have_true  = TRUE;
        }
        if( ScanCmd( ElseifTab ) == 0 ) break;
    }
    ScanCmd( ElseTab ); /* optional else */
    if( ScanQuote( &start, &len ) && !have_true ) {
        true_start = start;
        true_len   = len;
    }
    ReqEOC();
    if( true_len != 0 ) {
        cmd = AllocCmdList( true_start, true_len );
        PushCmdList( cmd );
        CopyInpFlags();
        FreeCmdList( cmd );
    }
}



/*
 * ProcWhile -- process While command
 */

void ProcWhile( void )
{
    char                *start;
    unsigned            len;
    unsigned_64         res;
    cmd_list            *cmd;

    res = ReqU64Expr();
    if( !ScanQuote( &start, &len ) || len == 0 ) {
        Error( ERR_LOC, LIT( ERR_WANT_COMMAND_LIST ) );
    }
    ReqEOC();
    if( U64Test( &res ) != 0 ) {
        cmd = AllocCmdList( start, len );
        ReScan( CmdStart );
        PushCmdList( cmd );
        CopyInpFlags();
        FreeCmdList( cmd );
    }
}


/*
 * ProcError -- proccess error command
 */

void ProcError( void )
{
    char        *start;
    unsigned    len;

    ScanItem( FALSE, &start, &len );
    ReqEOC();
    memcpy( TxtBuff, start, len );
    TxtBuff[ len ] = NULLCHAR;
    Error( ERR_NONE, LIT( ERR_GENERIC ), TxtBuff );
}


/*
 * ProcQuit -- process quit command
 */

void ProcQuit( void )
{
    ReqEOC();
    DebugExit();
}

/*
 * ProcThread -- process thread command
 */
enum thread_cmds { T_BAD, T_SHOW, T_FREEZE, T_THAW, T_CHANGE };

static void FormThdState( thread_state *thd, char *buff )
{
    char *p;

    buff = CnvULongHex( thd->tid, buff );
    *buff++ = ' ';
    *buff++ = ' ';
    switch( thd->state ) {
    case THD_THAW:  p = LIT( Runnable ); break;
    case THD_FREEZE:p = LIT( Frozen );   break;
    case THD_DEAD:  p = LIT( Dead );     break;
    }
    p = StrCopy( p, buff );
    while( (p - buff) < 10 ) *p++ = ' ';
    StrCopy( thd->name, p );
}


void MakeThdCurr( thread_state *thd )
{
    unsigned    err;

    if( !AdvMachState( ACTION_THREAD_CHANGE ) ) return;
    // NYI - PUI - record the thread change?
    WriteDbgRegs();
    if( RemoteSetThreadWithErr( thd->tid, &err ) == 0 ) {
        Error( ERR_NONE, LIT( ERR_NO_MAKE_CURR_THREAD ), thd->tid, err );
    }
    DbgRegs->tid = thd->tid;
    ReadDbgRegs();
    SetCodeDot( GetRegIP() );
    DbgUpdate( UP_REG_CHANGE | UP_CSIP_CHANGE | UP_THREAD_STATE );
}


static void ThdCmd( thread_state *thd, enum thread_cmds cmd )
{
    unsigned    up;

    up = UP_THREAD_STATE;
    switch( cmd ) {
    case T_SHOW:
        FormThdState( thd, TxtBuff );
        DUIDlgTxt( TxtBuff );
        up = UP_NO_CHANGE;
        break;
    case T_FREEZE:
        if( thd->state == THD_THAW ) thd->state = THD_FREEZE;
        break;
    case T_THAW:
        if( thd->state == THD_FREEZE ) thd->state = THD_THAW;
        break;
    case T_CHANGE:
        MakeThdCurr( thd );
        break;
    }
    DbgUpdate( up );
}


bool IsThdCurr( thread_state *thd )
{
    return( DbgRegs->tid == thd->tid );
}


static thread_state     *AddThread( dtid_t tid, unsigned state )
{
    thread_state    **owner;
    thread_state    *thd;
    char            name[UTIL_LEN];

    owner =  &HeadThd;
    for( ;; ) {
        thd = *owner;
        if( thd == NULL ) break;
        if( tid < thd->tid ) break;
        if( tid == thd->tid ) {
            if( _IsOn( SW_THREAD_EXTRA_CHANGED ) ) {
                *owner = thd->link;
                state = thd->state & ~THD_DEAD;
                _Free( thd );
                break;
            }
            thd->state &= ~THD_DEAD;
            return( thd );
        }
        owner = &thd->link;
    }
    RemoteThdName( tid, name );
    _Alloc( thd, sizeof( thread_state ) + strlen( name ) );
    if( thd == NULL ) return( NULL );
    thd->link = *owner;
    *owner = thd;
    strcpy( thd->name, name );
    thd->tid = tid;
    thd->state = state;
    return( thd );
}


dtid_t GetNextTID( void )
/***********************/
{
    thread_state    *thd;

    for( thd = HeadThd; thd != NULL; thd = thd->link ) {
    if( IsThdCurr( thd ) ) {
        thd = thd -> link;
        break;
    }
    }
    if( thd == NULL ) thd = HeadThd;
    if( thd == NULL ) return( 0 );
    return( thd->tid );
}


void NameThread( dtid_t tid, char *name )
/***************************************/
{
    thread_state        **owner, *curr, *new;

    for( owner = &HeadThd; *owner != NULL; owner = &curr->link ) {
        curr = *owner;
        if( curr->tid == tid ) {
            _Alloc( new, sizeof( thread_state ) + strlen( name ) );
            *new = *curr;
            *owner = new;
            strcpy( new->name, name );
            DbgUpdate( UP_THREAD_STATE );
            _Free( curr );
            break;
        }
    }
}

static void MarkThreadsDead( void )
{
    thread_state    *thd;

    for( thd = HeadThd; thd != NULL; thd = thd->link ) {
        thd->state |= THD_DEAD;
    }
}

static void KillDeadThreads( void )
{
    thread_state    **owner;
    thread_state    *thd;

    owner = &HeadThd;
    for( ;; ) {
        thd = *owner;
        if( thd == NULL ) break;
        if( thd->state & THD_DEAD ) {
            *owner = thd->link;
            _Free( thd );
        } else {
            owner = &thd->link;
        }
    }
}

void CheckForNewThreads( bool set_exec )
{
    dtid_t              tid;
    thread_state        *thd;
    unsigned            state;

    if( set_exec ) ExecThd = NULL;
    tid = 0;
    MarkThreadsDead();
    for( ;; ) {
        tid = RemoteGetNextThread( tid, &state );
        if( tid == 0 ) break;
        thd = AddThread( tid, state );
        if( set_exec && thd != NULL && thd->tid == DbgRegs->tid ) ExecThd = thd;
    }
    KillDeadThreads();
    _SwitchOff( SW_THREAD_EXTRA_CHANGED );
}


thread_state   *FindThread( dtid_t tid )
{
    thread_state    *thd;

    for( thd = HeadThd; thd != NULL && thd->tid != tid; thd = thd->link )
        ;
    return( thd );
}


void ProcThread( void )
{
    enum thread_cmds cmd;
    dtid_t          tid;
    bool            all;
    thread_state    *thd;
    unsigned            old;

    cmd = T_BAD;
    if( CurrToken == T_DIV ) {
        Scan();
        cmd = ScanCmd( ThreadOps );
        if( cmd == T_BAD ) Error( ERR_NONE, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_THREAD ) );
    }
    CheckForNewThreads( FALSE );
    if( CurrToken == T_MUL ) {
        if( cmd == T_BAD ) cmd = T_SHOW;
        Scan();
        all = TRUE;
    } else if( ScanEOC() ) {
        if( cmd == T_BAD ) cmd = T_SHOW;
        if( cmd == T_SHOW ) {
            all = TRUE;
        } else {
            tid = DbgRegs->tid;
            all = FALSE;
        }
    } else {
        if( cmd == T_BAD ) cmd = T_CHANGE;
        old = SetCurrRadix( 16 );
        tid = ReqExpr();
        all = FALSE;
        SetCurrRadix( old );
    }
    ReqEOC();
    if( all ) {
        if( cmd == T_CHANGE ) Error( ERR_NONE, LIT( ERR_CANT_SET_ALL_THREADS ) );
        for( thd = HeadThd; thd != NULL; thd = thd->link ) {
            ThdCmd( thd, cmd );
        }
    } else {
        thd = FindThread( tid );
        if( thd == NULL ) Error( ERR_NONE, LIT( ERR_NO_SUCH_THREAD ), tid );
        ThdCmd( thd, cmd );
    }
}

void FreeThreads( void )
{
    thread_state    *thd;

    while( HeadThd != NULL ) {
        thd = HeadThd->link;
        _Free( HeadThd );
        HeadThd = thd;
    }
}
