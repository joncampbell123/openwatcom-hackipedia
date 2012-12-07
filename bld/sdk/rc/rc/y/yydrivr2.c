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
* Description:  YACC driver for OS/2 resource script parser.
*
****************************************************************************/


#include <string.h>
#include "watcom.h"

#include "rctypes.h"
#include "semantic.h"
#include "rcmem.h"
#include "global.h"
#include "errors.h"
#include "ytab2.gh"
#include "yydrivr2.h"
#include "scan2.h"
#include "errprt.h"

#include "os2res.h"

typedef uint_16         YYCHKTYPE;
typedef uint_16         YYACTTYPE;
typedef uint_16         YYPLHSTYPE;
typedef uint_8          YYPLENTYPE;
typedef uint_16         YYTOKENTYPE;

typedef union {
    ScanInt                     intinfo;
    unsigned long               integral;
    IntMask                     maskint;
    ScanString                  string;
    WResID                      *resid;
    FullAccelEntryOS2           accelfullentry;
    FullAccelFlagsOS2           accflags;
    FullAccelTableOS2           *acceltable;
    AccelEvent                  accevent;
    FullHelpTableOS2            *helptable;
    FullHelpEntryOS2            helpfullentry;
    FullHelpSubTableOS2         *helpsubtable;
    ResLocation                 resloc;
    FullOptFlagsOS2             fullresflags;
    MenuFlags                   menuflags;
    FullMenuPtrOS2              menuptr;
    FullMenuItemOS2             menufull;
    MenuItemOS2                 menuitem;
    DialogSizeInfo              sizeinfo;
    FullDialogOptions           diagopts;
    ResNameOrOrdinal            *nameorord;
    FullDialogBoxHeader         *diaghead;
    FullDialogBoxControlOS2     *diagctrl;
    FullDiagCtrlListOS2         *diagctrllist;
    FullDiagCtrlOptionsOS2      diagctrlopts;
    PresParamsOS2               presparams;
    PresParamListOS2            *presparamlist;
    StringItem                  stritem;
    FullStringTable             *strtable;
    RawDataItem                 rawitem;
    YYCHKTYPE                   token;
    DataElemList                *dataelem;
} YYSTYPE;

#ifdef _I86FAR
#define YYFAR           _I86FAR
#else
#ifdef __386__
#define YYFAR
#else
#define YYFAR           __far
#endif
#endif

#define STACK_MAX       100

/* define value and state stacks for both expressions and declarations */

typedef struct {
    YYSTYPE             *vsp;
    YYSTYPE             *vstack;
    YYACTTYPE           *ssp;
    YYACTTYPE           *sstack;
} parse_stack;

/* This is pretty ugly but... these variables are defined in yydriver.c and
 * shared between the Windows and OS/2 parsers. It seems safe to assume that
 * either one or the other will be used, never both at the same time.
 */
extern YYSTYPE yylval;
extern uint_8  yysyntaxerror;  /* boolean variable */
#define YYERRORTHRESHOLD    5   /* no. of tokens to accept before restarting */

typedef enum {
    P_SHIFT,
    P_ACCEPT,
    P_SYNTAX,
    P_ERROR
} p_action;

/* definitions and tables here */
            /*  */
/* */

#ifdef YYDEBUG
#include <stdio.h>
#include "param.h"

static void dump_rule( unsigned rule )
{
    unsigned                     i;
    const YYCHKTYPE YYFAR       *tok;
    const char YYFAR            *p;

    if (CmdLineParms.DebugParser) {
        for( p = yytoknames[ yyplhstab[ rule ] ]; *p; ++p ) {
            RcFprintf( stdout, NULL, "%c", *p );
        }
        RcFprintf( stdout, NULL, " <-" );
        tok = &yyrhstoks[ yyrulebase[ rule ] ];
        for( i = yyplentab[ rule ]; i != 0; --i ) {
            RcFprintf( stdout, NULL, " " );
            for( p = yytoknames[ *tok ]; *p; ++p ) {
                RcFprintf( stdout, NULL, "%c", *p );
            }
            ++tok;
        }
        RcFprintf( stdout, NULL, "\n" );
    }
}
static void puts_far( const char YYFAR * string )
{
    const char YYFAR            *p;

    if (CmdLineParms.DebugParser) {
        for( p = string; *p; ++p ) {
            RcFprintf( stdout, NULL, "%c", *p );
        }
        RcFprintf( stdout, NULL, "%c", '\n' );
    }
}

#endif

static int yylexOS2( void )
/*************************/
{
    int         curtoken;
    ScanValue   value;

    curtoken = ScanOS2( &value );

    while (RcIoIsCOrHFile()) {
        switch (curtoken) {
        case Y_NAME:
        case Y_STRING:
        case Y_DOS_FILENAME:
            RcMemFree( value.string.string );
            break;
        case Y_INTEGER:
            RcMemFree( value.intinfo.str );
            break;
        case YYEOFTOKEN:
    /**/    goto END_LOOP;
            break;
        case Y_SCAN_ERROR:
            break;          /* ignore unknown chars in a c or h file */
        default:
            break;
        }

        curtoken = ScanOS2( &value );
    }
    END_LOOP:

    switch( curtoken ) {
    case Y_INTEGER:
        yylval.intinfo = value.intinfo;
        break;
    case Y_NAME:
    case Y_STRING:
    case Y_DOS_FILENAME:
        yylval.string = value.string;
        break;
    case Y_SCAN_ERROR:
        RcError( ERR_UNKNOWN_CHAR, value.UnknownChar );
        break;
    default:
        break;
    }
    return( curtoken );
}

static p_action doAction( YYCHKTYPE t, parse_stack *state )
{
    YYSTYPE yyval;
    YYSTYPE *yyvp;
    YYACTTYPE yyk;
    YYACTTYPE yyi;
    YYACTTYPE yyaction;
    YYACTTYPE rule;
    YYCHKTYPE yylhs;

    for(;;) {
        yyk = *(state->ssp);
        yyi = yyk + t;
        while( yyi >= YYUSED || yychktab[ yyi ] != t ) {
            yyi = yyk + YYPTOKEN;
            if( yyi >= YYUSED || yychktab[ yyi ] != YYPTOKEN ) {
                goto use_d_token;
            }
            yyk = yyacttab[ yyi ];
            yyi = yyk + t;
        }
        yyaction = yyacttab[ yyi ];
        if( yyaction == YYNOACTION ) {
    use_d_token:
            yyk = *(state->ssp);
            yyi = yyk + YYDTOKEN;
            while( yyi >= YYUSED || yychktab[ yyi ] != YYDTOKEN ) {
                yyi = yyk + YYPTOKEN;
                if( yyi >= YYUSED || yychktab[ yyi ] != YYPTOKEN ) {
                    return( P_SYNTAX );
                }
                yyk = yyacttab[ yyi ];
                yyi = yyk + YYDTOKEN;
            }
            yyaction = yyacttab[ yyi ];
            if( yyaction == YYNOACTION ) {
                return( P_SYNTAX );
            }
        }
        if( yyaction < YYUSED ) {
            if( yyaction == YYSTOP ) {
                return( P_ACCEPT );
            }
            state->ssp++;
            *(state->ssp) = yyaction;
            state->vsp++;
            *(state->vsp) = yylval;
#ifdef YYDEBUG
            puts_far( yytoknames[ t ] );
#endif
            return( P_SHIFT );
        }
        rule = yyaction - YYUSED;
        yyi = yyplentab[ rule ];
        state->vsp -= yyi;
        state->ssp -= yyi;
        if( state->ssp < state->sstack ) {
            return( P_ERROR );
        }
        yylhs = yyplhstab[ rule ];
        yyk = *(state->ssp);
        yyi = yyk + yylhs;
        while( yyi >= YYUSED || yychktab[ yyi ] != yylhs ) {
            yyi = yyk + YYPTOKEN;
            if( yyi >= YYUSED || yychktab[ yyi ] != YYPTOKEN ) {
                return( P_ERROR );
            }
            yyk = yyacttab[ yyi ];
            yyi = yyk + yylhs;
        }
        state->ssp++;
        *(state->ssp) = yyacttab[yyi];
#if 0                                   /*** change with new yacc */
        yyvp = state->vsp;
#else
        yyvp = ++(state->vsp);
#endif
#ifdef YYDEBUG
        dump_rule( rule );
#endif
        if (!yysyntaxerror) {
            switch( rule ) {
            /*  */
/* */
            default:
#if 0                                   /*** change with new yacc */
                yyval = yyvp[1];
#else
                yyval = yyvp[0];
#endif
            }
        }
#if 0                               /*** change with new yacc **/
        state->vsp++;
#endif
        *(state->vsp) = yyval;
        /* reduce as far as possible */
    }
}

void ParseInitOS2( void )
/***********************/
{
    ScanInitOS2();
}

void ParseFiniOS2( void )
/***********************/
{
}

static void initParseStack( parse_stack *stack )
{
    stack->vsp = stack->vstack;
    stack->ssp = stack->sstack;
    /* initialize */
    *(stack->ssp) = YYSTART;
}

static void newParseStack( parse_stack *stack )
{
    /* get new stack */
    stack->vstack = RcMemMalloc( STACK_MAX * sizeof( YYSTYPE ) );
    stack->sstack = RcMemMalloc( STACK_MAX * sizeof( YYACTTYPE ) );
    initParseStack( stack );
}

static void deleteStack( parse_stack *stack )
{
    if( stack->vstack ) {
        RcMemFree( stack->vstack );
    }
    if( stack->sstack ) {
        RcMemFree( stack->sstack );
    }
}

static void handleError( YYCHKTYPE token, parse_stack * state, int error_state )
{
    if (!error_state) {
        switch (token) {
        case Y_INTEGER:
            RcError( ERR_SYNTAX_STR, yylval.intinfo.str );
            RcMemFree( yylval.intinfo.str );
            break;
        case Y_NAME:
        case Y_STRING:
        case Y_DOS_FILENAME:
            RcError( ERR_SYNTAX_STR, yylval.string.string );
            RcMemFree( yylval.string.string );
            break;
        case Y_SCAN_ERROR:
            break;
        default:
            RcError( ERR_SYNTAX_STR, SemOS2TokenToString( token ) );
            break;
        }
    }

    initParseStack( state );
}

static p_action doParse( parse_stack * resource_state )
{
    p_action    what;
    int         error_state;
    YYCHKTYPE   token;
    int         token_count = 0;

    error_state = FALSE;

    do {
        token = yylexOS2();
        if (error_state) {
            token_count++;
            if (token_count >= YYERRORTHRESHOLD) {
                error_state = FALSE;
            }
        }

        what = doAction( token, resource_state );

        if( what == P_SYNTAX ) {
            handleError( token, resource_state, error_state );
            error_state = TRUE;
            yysyntaxerror = TRUE;
            ErrorHasOccured = TRUE;
            token_count = 0;
        } else if( token == Y_INTEGER && yylval.intinfo.str != NULL ) {
            RcMemFree( yylval.intinfo.str );
            yylval.intinfo.str = NULL;
        }
    } while (what != P_ACCEPT && what != P_ERROR);

    return( what );
}

bool ParseOS2( void )
/*******************/
{
    parse_stack resource_state;
    p_action    what;

    newParseStack( &resource_state );
    what = doParse( &resource_state );
    if( what == P_ERROR ) {
        RcError( ERR_PARSER_INTERNAL );
        ErrorHasOccured = TRUE;
    }
    deleteStack( &resource_state );
    return( what != P_ACCEPT );
}

void ParseInitStaticsOS2( void )
/******************************/
{
    memset( &yylval, 0, sizeof( YYSTYPE ) );
    yysyntaxerror = FALSE;
}
