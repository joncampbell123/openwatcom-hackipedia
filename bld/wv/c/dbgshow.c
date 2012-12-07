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
* Description:  Debugger 'show' command.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgtoken.h"
#include "dbginfo.h"
#include "dbgerr.h"
#include "dbglit.h"
#include "dbgtoggl.h"
#include "dbgtback.h"
#include "dbgmem.h"
#include "dui.h"
#include <string.h>
#include <ctype.h>


extern unsigned int     ScanCmd( char * );
extern void             Scan( void );
extern unsigned int     OptExpr( unsigned int );
extern char             *ScanPos( void );
extern unsigned int     ScanLen( void );
extern bool             ScanEOC( void );
extern void             ReqEOC( void );
extern char             *ReScan( char * );
extern int              AddrComp( address , address );
extern char             *Format( char *, char *, ... );
extern char             *StrCopy( char *, char * );
extern void             ConfigSet( void );
extern void             ConfigDisp( void );
extern void             ConfigFont( void );
extern void             ConfigPaint( void );
extern void             ConfigFlip( void );
extern void             ConfigHook( void );
extern void             ConfigEvent( void );
extern void             ConfigCalls( void );
extern void             InvokeAFile( char * );
extern void             CreateInvokeFile( char *name, void (*rtn)( void ) );
extern char             *GetCmdName( int );

extern tokens           CurrToken;
extern char             *TxtBuff;
extern char             *NameBuff;


/*
 * GetCmdEntry -- get an entry from a command table
 */

char *GetCmdEntry( char *tab, int index, char *buff )
{
    for( --index; index != 0; --index ) {
        for( ; *tab != NULLCHAR; ++tab )
            ;
        ++tab;
    }
    for( ;; ) {
        *buff = tolower( *tab );
        if( *buff == NULLCHAR ) break;
        ++buff;
        ++tab;
    }
    return( buff );
}


#define INDENT_AMOUNT   4

void ConfigCmdList( char *cmds, int indent )
{
    char        *p;
    char        ch;
    int         i;

    indent += INDENT_AMOUNT;
again:
    while( *cmds == ' ' ) ++cmds;
    p = TxtBuff;
    for( i = 0; i < indent; ++i ) {
        *p++ = ' ';
    }
    for( ;; ) {
        *p++ = ch = *cmds++;
        if( ch == '{' ) {
            *p = '\0';
            DUIDlgTxt( TxtBuff );
            indent += INDENT_AMOUNT;
            goto again;
        } else if( ch == '}' ) {
            *--p = '\0';
            for( ;; ) {
                if( p == TxtBuff ) break;
                --p;
                if( *p != ' ' ) {
                    ++p;
                    break;
                }
                *p = '\0';
            }
            if( p != TxtBuff ) DUIDlgTxt( TxtBuff );
            p = TxtBuff;
            for( i = 0; i < indent-INDENT_AMOUNT; ++i ) {
                *p++ = ' ';
            }
            *p++ = '}';
            *p = '\0';
            DUIDlgTxt( TxtBuff );
            indent -= INDENT_AMOUNT;
            goto again;
        } else if( ch == '\r' ) {
            --p;
        } else if( ch == '\0' ) {
            if( p != TxtBuff ) {
                DUIDlgTxt( TxtBuff );
            }
            break;
        }
    }
}

void ConfigLine( char *conf )
{
    char        *buff;

    _AllocA( buff, strlen( conf ) + strlen( NameBuff ) + 2 );
    Format( buff, "%s %s", NameBuff, conf );
    DUIDlgTxt( buff );
}


void DoConfig( char *cmd, char *name_tab, void(**jmp_tab)( void ), void(**not_all)( void ) )
{
    int num;
    char *start;
    char *ptr;
    unsigned i;

    ptr = StrCopy( cmd, NameBuff );
    *ptr++ = ' ';
    if( ScanEOC() ) {
        /* show configuration on everything */
        for( num = 0; jmp_tab[ num ] != NULL; ++num ) {
            GetCmdEntry( name_tab, num + 1, ptr );
            for( i = 0; not_all[i] != NULL; ++i ) {
                if( jmp_tab[ num ] == not_all[ i ] ) break;
            }
            if( not_all[ i ] == NULL ) {
                (*jmp_tab[ num ])();
            }
        }
    } else {
        start = ScanPos();
        do {
            i = ScanCmd( name_tab );
            if( i == 0 ) {
                Format( TxtBuff, "%s %s", GetCmdName( CMD_SHOW ), cmd );
                Error( ERR_LOC, LIT( ERR_BAD_SUBCOMMAND ), TxtBuff );
            }
        } while( !ScanEOC() );
        ReScan( start );
        do {
            i = ScanCmd( name_tab );
            GetCmdEntry( name_tab, i, ptr );
            (*jmp_tab[ i - 1 ])();
        } while( !ScanEOC() );
    }
}


OVL_EXTERN void BadShow( void )
{
    Error( ERR_LOC, LIT( ERR_BAD_SUBCOMMAND ), GetCmdName( CMD_SHOW ) );
}


static char ShowNameTab[] = {
    "Paint\0"
    "Display\0"
    "Font\0"
    "Set\0"
    "Flip\0"
    "Hook\0"
    "Event\0"
    "Calls\0"
    "Types\0"
};

typedef struct {
    void        (*rtn)( void );
    bool        config; /* should info be dumped for save config call */
} show_rtn;

static  show_rtn ShowJmpTab[] = {
        &BadShow,       FALSE,
        &ConfigPaint,   TRUE,
        &ConfigDisp,    TRUE,
        &ConfigFont,    TRUE,
        &ConfigSet,     TRUE,
        &ConfigFlip,    TRUE,
        &ConfigHook,    TRUE,
        &ConfigEvent,   FALSE,
        &ConfigCalls,   FALSE,
};

void ProcShow( void )
{
    ShowJmpTab[ ScanCmd( &ShowNameTab ) ].rtn();
}


OVL_EXTERN void ShowAll( void )
{
    unsigned    i;

    DUIDlgTxt( GetCmdName( CMD_CONFIGFILE ) );
    for( i = 1; i < sizeof( ShowJmpTab ) / sizeof( ShowJmpTab[0] ); ++i ) {
        if( ShowJmpTab[i].config ) {
            ShowJmpTab[i].rtn();
        }
    }
}

void SaveConfigToFile( char *name )
{
    CreateInvokeFile( name, ShowAll );
}

void RestoreConfigFromFile( char *name )
{
    InvokeAFile( name );
}
