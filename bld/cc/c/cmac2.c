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
* Description:  Macro processing, part 2.
*
****************************************************************************/


#include "cvars.h"
#include "scan.h"
#include <stddef.h>
#include "cgmisc.h"

static void    CSkip( void );
static void    CSkipIf( void );
static void    CDefine( void );
static void    CInclude( void );
static void    CIfDef( void );
static void    CIfNDef( void );
static void    CIf( void );
static void    CElif( void );
static void    CElse( void );
static void    CEndif( void );
static void    CUndef( void );
static void    CLine( void );
static void    CError( void );
static void    CIdent( void );
static void    CUnknown( void );

local void Flush2EOL( void );
local void GrabTokens( int parm_cnt, struct macro_parm *formal_parms, const char *mac_name );
local void IncLevel( int value );
local int  FormalParm( struct macro_parm *formal_parms );
local void ChkEOL( void );

struct preproc {
    char  *directive;
    void  (*samelevel)( void ); /* func to call when NestLevel == SkipLevel */
    void  (*skipfunc)( void );  /* func to call when NestLevel != SkipLevel */
};

static unsigned char PreProcWeights[] = {
//a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y,z
  2, 0, 0,11, 1, 4, 0, 0, 5, 0, 0,12, 0, 0, 0,13, 0,14, 0, 9,15, 0, 0, 0, 0,0
};

static struct preproc PreProcTable[] = {
    { "",       NULL,           NULL },  // 0
    { "line",   CLine,          CSkip }, // 4 +12 + 1 = 17 mod 16 = 1
    { "define", CDefine,        CSkip }, // 6 +11 + 1 = 18 mod 16 = 2
    { "ident",  CIdent,         CSkip }, // 5 + 5 + 9 = 19 mod 16 = 3
    { "error",  CError,         CSkip }, // 5 + 1 +14 = 20 mod 16 = 4
    { "pragma", CPragma,        CSkip }, // 6 +13 + 2 = 21 mod 16 = 5
    { "else",   CElse,          CElse }, // 4 + 1 + 1 = 6
    { "",       NULL,           NULL },  // 7
    { "undef",  CUndef,         CSkip }, // 5 +15 + 4 = 24 mod 16 = 8
    { "elif",   CElif,          CElif }, // 4 + 1 + 4 = 9
    { "endif",  CEndif,         CEndif },// 5 + 1 + 4 = 10
    { "if",     CIf,            CSkipIf },//2 + 5 + 4 = 11
    { "",       NULL,           NULL },  // 12
    { "include",CInclude,       CSkip }, // 7 + 5 + 1 = 13
    { "ifdef",  CIfDef,         CSkipIf },//5 + 5 + 4 = 14
    { "ifndef", CIfNDef,        CSkipIf },//6 + 5 + 4 = 15
};

enum    pp_types {
        PRE_IF,
        PRE_ELIF,
        PRE_ELSE
};

struct  cpp_info {              /* C Pre-processor information */
        struct cpp_info *prev_cpp;
        source_loc      src_loc;
        FNAMEPTR        flist;
        enum pp_types   cpp_type;
        int             processing;
};


static void PreProcStmt( void )
{
    struct preproc      *pp;
    int                 hash;

    NextChar();                 /* skip over '#' */
    PPNextToken();
    if( CurToken == T_ID ) {
        hash = (TokenLen + PreProcWeights[ Buffer[ 0 ] - 'a' ]
                 + PreProcWeights[ Buffer[ TokenLen - 1 ] - 'a' ]) & 15;
        pp = &PreProcTable[ hash ];
        if( strcmp( pp->directive, Buffer ) == 0 ) {
            if( NestLevel == SkipLevel ) {
                pp->samelevel();
            } else {
                pp->skipfunc();
            }
        } else {
            CUnknown();
        }
    } else if( CurToken != T_NULL ) {
        CUnknown();
    }
    Flush2EOL();
}

extern bool PrintWhiteSpace;  //ppc printing   (from ccmain.c)

TOKEN ChkControl( void )
{
    int lines_skipped;


    if( !CompFlags.doing_macro_expansion ) {
        if( CompFlags.cpp_output ) {
            PrintWhiteSpace = FALSE;
        }
    }
    while( CurrChar == '\n' ) {
        if( TBreak() ) {
            CErr1( ERR_BREAK_KEY_HIT );
            CSuicide();
        }
        lines_skipped = 0;
        for( ; CurrChar != EOF_CHAR; ) {
            if( CompFlags.cpp_output ) {
                CppPrtChar( '\n' );
            }
            NextChar();
            if( CurrChar != PreProcChar ) {
                SkipAhead();
            }
            if( CurrChar == EOF_CHAR )
                break;
            CompFlags.pre_processing = 2;
            if( CurrChar == PreProcChar ) { /* start of comp control line */
                PreProcStmt();
            } else if( NestLevel != SkipLevel ) {
                PPNextToken();              /* get into token mode */
                Flush2EOL();
            }
            if( NestLevel == SkipLevel )
                break;
            if( CurrChar == '\n' ) {
                lines_skipped = 1;
            }
        }
        CompFlags.pre_processing = 0;
        if( CompFlags.cpp_output ) {
            if( lines_skipped ) {
                if( SrcFile != NULL ) {                 /* 14-may-92 */
                    EmitLine( SrcFile->src_loc.line, SrcFile->src_name );
                }
            }
        }
    }
    // we have already skipped past all white space at the start of the line
    CurToken = T_WHITE_SPACE;
//  CurToken = ScanToken();
    return( T_WHITE_SPACE );
}


static void Flush2EOL( void )
{
    while( CurToken != T_NULL ) {
        if( CurToken == T_EOF )
            break;
        PPNextToken();
    }
}


local void CSkip( void )
{
}


local void CSkipIf( void )
{
    IncLevel( 0 );
}


local void CUnknown( void )
{
    if( NestLevel == SkipLevel ) {
        CErr2p( ERR_UNKNOWN_DIRECTIVE, Buffer );
    }
}


local void CIdent( void )
{
    if( !CompFlags.extensions_enabled )
        CUnknown();
    Flush2EOL();
}


void CInclude( void )
{
    char        in_macro;
    auto char   buf[ 82 ];

    if( PCH_FileName != NULL && CompFlags.make_precompiled_header == 0 ) {
        if( CompFlags.ok_to_use_precompiled_hdr ) {     /* 27-jun-94 */
            CompFlags.use_precompiled_header = 1;
        }
    }
    if( CompFlags.use_precompiled_header ) {
        InitBuildPreCompiledHeader();
    }
    InitialMacroFlag = MFLAG_NONE;
    in_macro = 0;
    if( MacroPtr != NULL )
        in_macro = 1;
    CompFlags.pre_processing = 1;
    PPNextToken();
    if( CurToken == T_STRING ) {
        OpenSrcFile( Buffer, FALSE );
#if _CPU == 370
        if( !CompFlags.use_precompiled_header ) {
            SrcFile->colum = Column;    /* do trunc and col on  */
            SrcFile->trunc = Trunc;     /* on user source files */
        }
#endif
    } else if( CurToken == T_LT ) {
        if( !in_macro )                 /* 28-may-89 */
            CompFlags.pre_processing = 2;
        buf[ 0 ] = '\0';
        for( ;; ) {
            PPNextToken();
            if( CurToken == T_GT ) {
                OpenSrcFile( buf, TRUE );
                break;
            }
            strncat( buf, Buffer, 80 );
            if( (in_macro != 0  &&  MacroPtr == NULL)
                ||      CurToken == T_NULL
                ||      CurToken == T_EOF ) {
                CErr1( ERR_INVALID_INCLUDE );
                break;
            }
        }
    } else {
        CErr1( ERR_INVALID_INCLUDE );
    }
    if( CurToken != T_EOF ) {
        PPNextToken();
    }
    CompFlags.use_precompiled_header = 0;
}


local void CDefine( void )
{
    struct macro_parm *mp, *prev_mp, *formal_parms;
    int         parm_cnt, parm_end = 0;
    int         ppscan_mode;
    char        *token_buf;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        return;
    }
    if( strcmp( Buffer, "defined" ) == 0 ) {
        CErr1( ERR_CANT_DEFINE_DEFINED );
        return;
    }
    token_buf = CStrSave( Buffer );
    formal_parms = NULL;
    parm_cnt = -1;              /* -1 ==> no () following */
    if( CurrChar == '(' ) {     /* parms present */
        PPNextToken();          /* grab the '(' */
        PPNextToken();
        parm_cnt = 0;           /* 0 ==> () following */
        parm_end = 0;
        prev_mp = NULL;
        for( ;; ) {
            if( CurToken == T_RIGHT_PAREN )
                break;
            if( parm_end ) {
                ExpectingAfter( ")", "..." );
                return;
            }
            if( CurToken != T_ID &&
                CurToken != T_DOT_DOT_DOT ) {
                ExpectIdentifier();
                return;
            }
            ++parm_cnt;
            if( CurToken == T_DOT_DOT_DOT ) {
                parm_end = 1; /* can have no more parms after this. */
            }
            mp = (struct macro_parm *)CMemAlloc( sizeof( struct macro_parm ) );
            if( formal_parms == NULL ) {
                formal_parms = mp;
            } else {
                if( FormalParm( formal_parms ) ) {
                    CErr2p( ERR_DUPLICATE_MACRO_PARM, Buffer );
                }
                prev_mp->next_macro_parm = mp;
            }
            if( CurToken == T_DOT_DOT_DOT )
                mp->parm = CStrSave( "__VA_ARGS__" );
            else
                mp->parm = CStrSave( Buffer );
            prev_mp = mp;
            PPNextToken();
            if( CurToken == T_RIGHT_PAREN )
                break;
            if( CurToken == T_NULL ) {
                CErr1( ERR_INVALID_MACRO_DEFN );
                break;
            }
            if( parm_end ) {
                ExpectingAfter( ")", "..." );
                return;
            }
            MustRecog( T_COMMA );
            if( CurToken != T_ID && CurToken != T_DOT_DOT_DOT ) {   /* 16-nov-94 */
                ExpectIdentifier();
                return;
            }
        }
    }
    /* grab replacement tokens */
    ppscan_mode = InitPPScan();         // enable T_PPNUMBER tokens
    GrabTokens( parm_end ? -(parm_cnt + 1) : (parm_cnt + 1), formal_parms, token_buf );
    FiniPPScan( ppscan_mode );          // disable T_PPNUMBER tokens
    for( ; (mp = formal_parms) != NULL; ) {
        formal_parms = mp->next_macro_parm;
        CMemFree( mp->parm );
        CMemFree( mp );
    }
    CMemFree( token_buf );
}


local void GrabTokens( int parm_cnt, struct macro_parm *formal_parms, const char *mac_name )
{
    MEPTR       mentry;
    int         i;
    int         j;
    TOKEN       prev_token;
    int         prev_non_ws_token;
    unsigned    mlen;
    macro_flags mflags;
    TOKEN       *p_token;

    j = strlen( mac_name ) + 1;
    mentry = (MEPTR)CMemAlloc( sizeof( MEDEFN ) + j );
    mflags = MFLAG_USER_DEFINED;
    if( parm_cnt < 0 ) {
        mflags |= MFLAG_VAR_ARGS;
        parm_cnt = -parm_cnt;
    }
    mentry->parm_count = parm_cnt;
    strcpy( mentry->macro_name, mac_name );
    mlen = offsetof( MEDEFN, macro_name ) + j;
    mentry->macro_defn = mlen;
    MacroOverflow( mlen, 0 );
    MacroCopy( mentry, MacroOffset, mlen );
    prev_token = T_NULL;
    prev_non_ws_token = T_NULL;
    if( CurToken != T_NULL ) {
        do {
            CurToken = ScanToken();
        } while( CurToken == T_WHITE_SPACE );
        if( CurToken == T_SHARP_SHARP ) {
            CErr1( ERR_MISPLACED_SHARP_SHARP );
            PPNextToken();
        }
    }
    for( ;; ) {
        i = 0;
        if( ( CurToken == T_STRING ) && CompFlags.wide_char_string ) {
            CurToken = T_LSTRING;                   /* 15-may-92 */
        }
        p_token = (TOKEN *)&TokenBuf[ i ];
        *p_token = CurToken;
        i += sizeof( TOKEN );
        if( CurToken == T_NULL )
            break;
        if( CurToken == T_EOF )
            break;
        switch( CurToken ) {
        case T_SHARP:
            /* if it is a function-like macro definition */
            if( parm_cnt != 0 ) {
                *p_token = T_MACRO_SHARP;
                CurToken = T_MACRO_SHARP;           /* 26-mar-91 */
            }
            break;
        case T_SHARP_SHARP:
            *p_token = T_MACRO_SHARP_SHARP;
            break;
        case T_WHITE_SPACE:
            if( prev_token == T_WHITE_SPACE )
                i -= sizeof( TOKEN );
            break;
        case T_ID:
            j = FormalParm( formal_parms );
            if( j != 0 ) {
                if( (mflags & MFLAG_VAR_ARGS) && (j == parm_cnt - 1) )
                    CurToken = T_MACRO_VAR_PARM;
                else
                    CurToken = T_MACRO_PARM;
                *p_token = CurToken;
                TokenBuf[ i ] = j - 1;
                ++i;
            } else {
                j = 0;
                while( (TokenBuf[ i++ ] = Buffer[ j++ ]) != '\0' )
                    ;   /*empty*/
            }
            break;
        case T_BAD_CHAR:
            TokenBuf[ i++ ] = Buffer[ 0 ];
            if( Buffer[ 1 ] != '\0' ) {
                *(TOKEN *)&TokenBuf[ i ] = T_WHITE_SPACE;
                i += sizeof( TOKEN );
            }
            break;
        case T_CONSTANT:
        case T_STRING:
        case T_LSTRING:
        case T_BAD_TOKEN:
        case T_PPNUMBER:
            j = 0;
            while( (TokenBuf[ i++ ] = Buffer[ j++ ]) != '\0' )
                ;   /* empty */
            break;
        default:
            break;
        }
        if( CurToken != T_WHITE_SPACE ) {
            if( prev_non_ws_token == T_MACRO_SHARP &&       /* 26-mar-91 */
                CurToken != T_MACRO_PARM &&
                CurToken != T_MACRO_VAR_PARM ) {
                CErr1( ERR_MUST_BE_MACRO_PARM );
                prev_token = *(TOKEN *)TokenBuf;
                *(TOKEN *)TokenBuf = T_SHARP;               /* 17-jul-92 */
                MacroCopy( TokenBuf, MacroOffset + mlen - sizeof( TOKEN ), sizeof( TOKEN ) );
                *(TOKEN *)TokenBuf = prev_token;
            }
            prev_non_ws_token = CurToken;
        }
        prev_token = CurToken;
        CurToken = ScanToken();
        MacroOverflow( mlen + i, mlen );
        MacroCopy( TokenBuf, MacroOffset + mlen, i );
        mlen += i;
    }
    if( prev_non_ws_token == T_MACRO_SHARP ) {      /* 16-nov-94 */
        CErr1( ERR_MUST_BE_MACRO_PARM );
    }
    if( prev_token == T_WHITE_SPACE ) {
        mlen -= sizeof( TOKEN );
    }
    MacroOverflow( mlen + sizeof( TOKEN ), mlen );
    *(TOKEN *)(MacroOffset + mlen) = T_NULL;
    mlen += sizeof( TOKEN );
    if( prev_non_ws_token == T_SHARP_SHARP ) {
        CErr1( ERR_MISPLACED_SHARP_SHARP );
    }
    mentry->macro_len = mlen;
    MacLkAdd( mentry, mlen, mflags );
    CMemFree( mentry );                     /* 22-aug-88, FWC */
    MacroSize += mlen;
}


local int FormalParm( struct macro_parm *formal_parms )
{
    int i;

    i = 1;
    while( formal_parms != NULL ) {
        if( strcmp( formal_parms->parm, Buffer ) == 0 )
            return( i );
        ++i;
        formal_parms = formal_parms->next_macro_parm;
    }
    return( 0 );
}


local void CIfDef( void )
{
    MEPTR       mentry;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        IncLevel( 0 );
        return;
    }
    mentry = MacroLookup( Buffer );
    if( mentry != NULL ) {
        mentry->macro_flags |= MFLAG_REFERENCED;
        IncLevel( 1 );
    } else {
        IncLevel( 0 );
    }
    PPNextToken();
    ChkEOL();
}


local void CIfNDef( void )
{
    MEPTR       mentry;

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        IncLevel( 0 );
        return;
    }
    mentry = MacroLookup( Buffer );
    if( mentry != NULL ) {
        mentry->macro_flags |= MFLAG_REFERENCED;
        IncLevel( 0 );
    } else {
        IncLevel( 1 );
    }
    PPNextToken();
    ChkEOL();
}


local int GetConstExpr( void )                              /* 13-nov-91 */
{
    int         value;
    int         useful_side_effect;
    int         meaningless_stmt;

/* This solves the following weird condition.   */
/*      while( f() == 1 )                       */
/* The expression for the #if destroys the flags saved for the while expr */
/*   #if 1                                      */
/*              ;                               */
/*   #endif                                     */

    useful_side_effect = CompFlags.useful_side_effect;
    meaningless_stmt   = CompFlags.meaningless_stmt;
    value = BoolConstExpr();
    CompFlags.useful_side_effect = useful_side_effect;
    CompFlags.meaningless_stmt   = meaningless_stmt;
    return( value );
}

local void CIf( void )
{
    int value;

    CompFlags.pre_processing = 1;
    PPNextToken();
    value = GetConstExpr();
    IncLevel( value );
    ChkEOL();
}


local void CElif( void )
{
    int value;

    CompFlags.pre_processing = 1;
    PPNextToken();
    if( ( NestLevel == 0 ) || ( CppStack->cpp_type == PRE_ELSE ) ) {
        CErr1( ERR_MISPLACED_ELIF );
    } else {
        if( NestLevel == SkipLevel ) {
            --SkipLevel;                /* start skipping else part */
            CppStack->processing = 0;
            CppStack->cpp_type = PRE_ELIF;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* only evaluate the expression when required */
            if( CppStack->cpp_type == PRE_IF ) {        /* 30-jun-88 */
                value = GetConstExpr();         /* 21-jun-88 */
                ChkEOL();
                if( value ) {
                    SkipLevel = NestLevel; /* start including else part */
                    CppStack->processing = 1;
                    CppStack->cpp_type = PRE_ELIF;
                }
            }
        }
    }
}


local void IncLevel( int value )
{
    struct cpp_info *cpp;

    cpp = (struct cpp_info *)CMemAlloc( sizeof( struct cpp_info ) );
    cpp->prev_cpp = CppStack;
    cpp->src_loc = TokenLoc;
    cpp->flist = SrcFile->src_flist;
    cpp->cpp_type = PRE_IF;
    cpp->processing = 0;
    CppStack = cpp;
    if( NestLevel == SkipLevel ) {
        if( value ) {
            ++SkipLevel;
            cpp->processing = 1;
        }
    }
    ++NestLevel;
}


local void WantEOL( void )
{
    if( CompFlags.extensions_enabled ) {
        if( CurToken != T_NULL  &&  CurToken != T_EOF ) {
            if( NestLevel == SkipLevel ) {
                CWarn1( WARN_JUNK_FOLLOWS_DIRECTIVE, ERR_JUNK_FOLLOWS_DIRECTIVE );
            }
            Flush2EOL();
        }
    } else {
        ChkEOL();
    }
}


local void CElse( void )
{
    if( ( NestLevel == 0 ) || ( CppStack->cpp_type == PRE_ELSE ) ) {
        CErr1( ERR_MISPLACED_ELSE );
    } else {
        if( NestLevel == SkipLevel ) {
            --SkipLevel;                /* start skipping else part */
            CppStack->processing = 0;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* cpp_type will be PRE_ELIF if an elif was true */
            if( CppStack->cpp_type == PRE_IF ) {        /* 19-sep-88 */
                SkipLevel = NestLevel;  /* start including else part */
                CppStack->processing = 1;
            }
        }
        CppStack->cpp_type = PRE_ELSE;
    }
    PPNextToken();
    WantEOL();
}


local void CEndif( void )
{
    if( NestLevel == 0 ) {
        CErr1( ERR_MISPLACED_ENDIF );
    } else {
        struct cpp_info *cpp;

        --NestLevel;
        cpp = CppStack;
        if( cpp->flist != SrcFile->src_flist ) {
             CWarn( WARN_LEVEL_1, ERR_WEIRD_ENDIF_ENCOUNTER,
                                 FileIndexToCorrectName( cpp->src_loc.fno ) );
        }
        CppStack = cpp->prev_cpp;
        CMemFree( cpp );
    }
    if( NestLevel < SkipLevel ) {
        SkipLevel = NestLevel;
    }
    PPNextToken();
    WantEOL();
}

extern bool MacroDel( char *name )
/********************************/
{
    MEPTR       mentry;
    MEPTR       prev_entry;
    int         len;
    bool        ret;

    ret = FALSE;
    if( strcmp( name, "defined" ) == 0 ) {
        CErr2p( ERR_CANT_UNDEF_THESE_NAMES, name  );
        return( ret );
    }
    prev_entry = NULL;
    len = strlen( name ) + 1;
    mentry = MacHash[ MacHashValue ];
    while( mentry != NULL ) {
        if( memcmp( mentry->macro_name, name, len ) == 0 )
            break;
        prev_entry = mentry;
        mentry = mentry->next_macro;
    }
    if( mentry != NULL ) {
        if( mentry->macro_defn == 0 ) {
            CErr2p( ERR_CANT_UNDEF_THESE_NAMES, name );
        } else {
            if( prev_entry != NULL ) {
                prev_entry->next_macro = mentry->next_macro;
            } else {
                MacHash[ MacHashValue ] = mentry->next_macro;
            }
            if( (InitialMacroFlag & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE) == 0 ) {
                /* remember macros that were defined before first include */
                if( mentry->macro_flags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE ) {
                    mentry->next_macro = UndefMacroList;
                    UndefMacroList = mentry;
                }
            }
            ret = TRUE;
        }
    }
    return( ret );
}


local void CUndef( void )
{

    PPNextToken();
    if( CurToken != T_ID ) {
        ExpectIdentifier();
        return;
    }
    MacroDel( Buffer );
    PPNextToken();
    ChkEOL();
}


local void ChkEOL( void )
{
    if( ( CurToken != T_NULL ) && ( CurToken != T_EOF ) ) {
        ExpectEndOfLine();
    }
}


local void CLine( void )
{
    FNAMEPTR        flist;
    unsigned long   src_line;

    src_line = 0;
    CompFlags.pre_processing = 1;
    PPNextToken();
    if( CurToken != T_CONSTANT ) {
        ExpectConstant();
        return;
    }
    if( CompFlags.cpp_ignore_line == 0 ) {
        src_line = Constant; // stash in case of side effects
        SrcFile->src_loc.line = src_line - 1; /* don't count this line */
    }
    PPNextToken();
    if( CurToken != T_NULL ) {
        if( ( CurToken != T_STRING ) || CompFlags.wide_char_string ) {
            /* wide char string not allowed, 26-mar-91 */
            ExpectString();
            return;
        }
        if( CompFlags.cpp_ignore_line == 0 ) {
            //          RemoveEscapes( Buffer );                /* 04-apr-91 */
            flist = AddFlist( Buffer );
            flist->rwflag = FALSE;  // not a real file so no autodep
            SrcFile->src_name = flist->name;
            SrcFile->src_loc.fno  = flist->index;
            if( CompFlags.cpp_output ) {            /* 30-may-95 */
                EmitLine( src_line, SrcFile->src_name );
            }
        }
        PPNextToken();
        ChkEOL();
    } else if( CompFlags.cpp_ignore_line == 0 ) {
        if( CompFlags.cpp_output ) {            /* 30-may-95 */
            EmitLine( src_line, SrcFile->src_name );
        }
    }
}


local void CError( void )
{
    int i;
    int save;

    i = 0;
    while( CurrChar != '\n' && CurrChar != '\r' && CurrChar != EOF_CHAR ) {
        if( i != 0 || CurrChar != ' ' ) {
            Buffer[ i ] = CurrChar;
            ++i;
        }
        NextChar();
    }
    Buffer[ i ] = '\0';
    /* Force #error output to be reported, even with preprocessor */
    save = CompFlags.cpp_output;
    CompFlags.cpp_output = 0;
    CErr2p( ERR_USER_ERROR_MSG, Buffer );
    CompFlags.cpp_output = save;
}


void CppStackInit( void )
{
    NestLevel = 0;
    SkipLevel = 0;
    CppStack = NULL;
}


void CppStackFini( void )
{
    struct cpp_info *cpp;

    while( (cpp = CppStack) != NULL ) {
        SetErrLoc( &cpp->src_loc );
        CErr1( ERR_MISSING_CENDIF );
        CppStack = cpp->prev_cpp;
        CMemFree( cpp );
    }
    CppStack = NULL;
}
