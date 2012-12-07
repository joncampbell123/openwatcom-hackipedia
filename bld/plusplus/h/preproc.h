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
* Description:  Definitions for preprocessing.
*
****************************************************************************/


#ifndef _PREPROC_H_
#define _PREPROC_H_

#include <stdio.h>

#include "ctokens.h"            // tokens
#include "srcfile.h"            // source files

#include "macro.h"              // macro structures

#include "i64.h"

#include "pragdefn.h"

typedef unsigned char TOKEN;


#define BUF_SIZE_SHIFT          (9)
#define BUF_SIZE                (1<<BUF_SIZE_SHIFT)

#ifdef pick
#undef pick
#endif
#define pick( a, b, c ) a,
enum
{
#include "auxinfo.h"
};

//typedef target_ulong target_int_const;

typedef enum ppstate_t {
    PPS_NO_EXPAND       = 0x01, // don't expand macros
    PPS_EOL             = 0x02, // return <end-of-line> as a token
    PPS_NO_LEX_ERRORS   = 0x04, // don't diagnose lexical problems
    PPS_NORMAL          = 0x00, // expand macros, treat <eol> as white space
    PPS_NULL            = 0x00
} ppstate_t;

// PREPROCESSOR DATA:

global  unsigned    PPState;            // pre-processor state (ppstate_t)
global  boolean     PPStateAsm;         // pre-processor state in _asm
global  enum TOKEN  CurToken;           // current token
global  unsigned    BadTokenInfo;       // error message that describes why T_BAD_TOKEN is bad
global  int         TokenLen;           // length of current token
global  int         TokenLine;          // line # of current token
global  int         TokenColumn;        // column # of current token
global  int         CurrChar;           // current character
global  int         ConstType;          // type of constant
global  signed_64   Constant64;         // value of constant: 33-64 bits
global  int         NestLevel;          // pre-processing level of #if
global  int         SkipLevel;          // pre-processing level of #if to skip to
global  char *      SavedId;            // saved id when doing look ahead
global  enum TOKEN  LAToken;            // look ahead token
global  macro_flags InitialMacroFlag;   // current value to init macro flags to
global  MACADDR_T   MacroOffset;        // first free byte in MacroSegment
global  char        __Time[10];         // "HH:MM:SS" for __TIME__ macro
global  char        __Date[12];         // "MMM DD YYYY" for __DATE__ macro
global  FILE *      CppFile;            /* output for preprocessor */
global  char *      ForceInclude;
global  char *      SrcFName;           /* source file name without suffix */
global  char *      WholeFName;         /* whole file name with suffix */
global  char        PreProcChar;        /* preprocessor directive indicator */
global  int         SwitchChar;         // DOS switch character

// token buffer
// extra 16 is for unrolled scanning loops
// extra uint_32 is for buffer overrun checking in debugging compiler
global  char        Buffer[BUF_SIZE+16+sizeof(uint_32)];

extern  int     (*NextChar)( void );    // next-character routine (initialized in SRCFILE)

// PROTOTYPES: exposed to C++ project

typedef struct {
    void                *curr_offset;
    unsigned long       undef_count;
} MACRO_STATE;

void MacroStateClear( MACRO_STATE * );
void MacroStateGet( MACRO_STATE * );
boolean MacroStateMatchesCurrent( MACRO_STATE * );

// provide a temporary source of tokens
void (*SetTokenSource( void (*)( void ) ))( void );

// restore source of tokens
void ResetTokenSource( void (*)( void ) );

boolean TokenUsesBuffer( unsigned t );

void GetNextToken(              // GET THE NEXT TOKEN FOR PROCESSOR
    void )
;
unsigned IfDepthInSrcFile(      // COMPUTE #IF DEPTH IN CURRENT SOURCE FILE
    void )
;
void CBanner(                   // PRINT C++ BANNER
    void )
;
void LookPastName(              // SCAN PAST ID (makes a name out of id)
    void )
;
void UndoNextToken(             // make NextToken return the current token
    void )
;
void MacroDefsSysind            // SYSTEM-INDEPENDENT MACRO DEFINITIONS
    ( void )
;
boolean MacroDependsDefined     // MACRO DEPENDENCY: DEFINED OR NOT
    ( void )
;
boolean MacroExists(            // TEST IF MACRO EXISTS
    const char *macname,        // - macro name
    unsigned len )              // - length of macro name
;
void MacroFini(                 // MACRO PROCESSING -- COMPLETION
    void )
;
void MacroInit(                 // MACRO PROCESSING -- INITIALIZATION
    void )
;
void PCHDumpMacroCheck(         // DUMP MACRO CHECK INFO INTO PCHDR
    void )
;
boolean PCHVerifyMacroCheck(    // READ AND VERIFY MACRO CHECK INFO FROM PCHDR
    void )
;
void MacroCanBeRedefined(       // SET MACRO SO THAT USE CAN REDEFINE IN SOURCE
    MEPTR mptr )                // - the macro entry
;
MEPTR MacroScan(                // SCAN AND DEFINE A MACRO (#define, -d)
    macro_scanning defn )       // - scanning definition
;
int OpenSrcFile(                // OPEN A SOURCE FILE
    const char * filename,      // - file name
    boolean is_lib )            // - TRUE ==> is <file>
;
void PpInit(                    // INITIALIZE PREPROCESSING
    void )
;
void PpOpen(                    // OPEN PREPROCESSOR OUTPUT
    void )
;
void PpParse(                   // PARSE WHEN PREPROCESSING
    void )
;
unsigned PpVerifyWidth(         // VERIFY WIDTH FOR PREPROCESSING
    unsigned width )            // - new width
;
void PpSetWidth(                // SET WIDTH FOR PREPROCESSING
    unsigned width )            // - new width
;
void PpStartFile(               // INDICATE START OF A FILE
    void )
;
void ScanInit(                  // SCAN INITIALIZATION
    void )
;
int InitPPScan(                 // INIT SCANNER FOR PPNUMBER TOKENS
    void )
;
void FiniPPScan(                 // INIT SCANNER FOR PPNUMBER TOKENS
    int ppscan_mode )           // - mode returned by InitPPScan()
;
int SpecialMacro(               // EXECUTE A SPECIAL MACRO
    MEPTR fmentry )             // - macro entry
;
void DefineAlternativeTokens(   // DEFINE ALTERNATIVE TOKENS
    void )
;
AUX_INFO * PragmaLookup(        // FIND A PRAGMA
    char * name,                // - name of the pragma
    unsigned index )            // - index (M_UNKNOWN if not known)
;
unsigned PragmaGetIndex( AUX_INFO * );

AUX_INFO *PragmaMapIndex( unsigned index );

// PROTOTYPES: internal to scanner

int ChkControl(                 // CHECK AND PROCESS DIRECTIVES
    int expanding )
;
int CppPrinting(                // TEST IF AT LEVEL FOR PREPROC PRINTING
    void )
;
void CPragma(                   // #PRAGMA PROCESSING
    void )
;
void DirectiveFini(             // COMPLETE DIRECTIVE PROCESSING
    void )
;
void DirectiveInit(             // INITIALIZE FOR DIRECTIVE PROCESSING
    void )
;
void DoMacroExpansion(          // EXPAND A MACRO
    MEPTR fmentry )             // - macro to expand
;
void EmitLine(                  // EMIT #LINE DIRECTIVE, IF REQ'D
    LINE_NO line_num,           // - line number
    char *filename )            // - file name
;
void EmitLineNL(                // EMIT #LINE DIRECTIVE ON ITS OWN LINE, IF REQ'D
    LINE_NO line_num,           // - line number
    char *filename )            // - file name
;
void Expecting(                 // ISSUE EXPECTING ERROR FOR A TOKEN
    char *a_token )             // - required token
;
char *TokenString(              // RETURN A PRINTABLE STRING FOR CURRENT TOK
    void )
;
void GetMacroToken(             // GET NEXT TOKEN
    boolean doing_macro_expansion ) // - TRUE ==> doing an expansion
;
int GetNextChar(                // GET NEXT CHARACTER FROM A SOURCE FILE
    void )
;
void GetNextCharUndo(           // UNDO PREVIOUS GET NEXT CHARACTER
    int c )                     // - character to undo
;
AUX_INFO *GetTargetHandlerPragma // GET PRAGMA FOR FS HANDLER
    ( void )
;
int KwLookup(                   // TRANSFORM TO T_ID OR KEYWORD TOKEN
    unsigned len )              // - length of id
;
char KwDisable(                 // DISABLE A KEYWORD TOKEN TO T_ID
    unsigned token )            // - token id
;
void KwEnable(                  // ENABLE A KEYWORD TOKEN FROM T_ID
    unsigned token,             // - token id
    char first_char )           // - first character of name
;
MEPTR MacroSpecialAdd(          // ADD A SPECIAL MACRO
    char *name,                 // - macro name
    unsigned value,             // - value for special macro
    macro_flags flags )         // - macro flags
;
MEPTR MacroDefine(              // DEFINE A NEW MACRO
    MEPTR mentry,               // - scanned macro
    unsigned len,               // - length of entry
    unsigned name_len )         // - name of macro name
;
MEPTR MacroLookup(              // LOOKUP NAME AS A MACRO
    char const *name,           // - name
    unsigned len )              // - length of name
;
void MacroOverflow(             // OVERFLOW SEGMENT IF REQUIRED
    unsigned amount_needed,     // - amount for macro
    unsigned amount_used )      // - amount used in segment
;
void MacroStorageInit(          // INITIALIZE FOR MACRO STORAGE
    void )
;
void MacroStorageFini(          // RELEASE MACRO STORAGE
    void )
;
void MacroUndefine(             // UNDEFINE CURRENT NAME AS MACRO
    unsigned len )              // - length of macro name
;
void MacroCmdLnUndef(           // -U<macro-name>
    char *name,                 // - macro name
    unsigned len )              // - length of macro name
;
void MustRecog(                 // REQUIRE A SPECIFIC TOKEN AND SCAN NEXT
    int this_token )            // - token to be recognized
;
int NextToken(                  // GET NEXT TOKEN
    void )
;
long int PpConstExpr(           // PREPROCESSOR CONSTANT EXPRESSION
    void )
;
void PrtChar(                   // PRINT PREPROC CHAR IF REQ'D
    int c )                     // - character to be printed
;
void PrtString(                 // PRINT CPP STRING
    const char *str )           // - string to be printed
;
void PrtToken(                  // PRINT PREPROC TOKEN IF REQ'D
    void )
;
void ReScanInit(                // RE-SCAN TOKEN INITIALIZATION
    char *buf )
;
int ReScanToken(                // RE-SCAN TOKEN FROM BUFFER
    void )
;
boolean ScanOptionalComment(    // SCAN AN OPTIONAL COMMENT
    void )
;
int ScanToken(                  // SCAN NEXT TOKEN
    int doing_macro_expansion ) // - TRUE ==> expanding
;
void SkipAhead(                 // SKIP AHEAD TO SIGNIFICANT LOCATION
    void )
;
void TimeInit(                  // INITIALIZE TIME
    void )
;
time_t TimeOfCompilation(       // GET TIME OF COMPILATION
    void )
;
#endif
