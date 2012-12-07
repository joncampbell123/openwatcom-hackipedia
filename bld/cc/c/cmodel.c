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


#include "cvars.h"
#include "scan.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include <ctype.h>

extern  char    CompilerID[];


/* COMMAND LINE PARSING OF MACRO DEFINITIONS */


static char *copy_eq( char *dest, char *src )
{
    char        c;

    for(;;) {
        c = *src;
        if( c == '\0' ) break;
        if( EqualChar( c ) ) break;
        if( c == ' '  ) break;
        if( c == '-'  ) break;
        if( c == SwitchChar  ) break;
        *dest++ = c;
        ++src;
    }
    *dest = '\0';
    return( src );
}


char *BadCmdLine( int error_code, char *str )
{
    char        *p;
    auto char   buffer[128];

    p = buffer;
    for(;;) {
        if( *str == '\0' ) break;
        *p++ = *str++;
        if( *str == ' ' ) break;
        if( *str == '-' ) break;
        if( *str == SwitchChar ) break;
    }
    *p = '\0';
    CBanner();
    CErr2p( error_code, buffer );
    return( str );
}


static char *Def_Macro_Tokens( char *str, int multiple_tokens, macro_flags mflags )
{
    int         i;
    MEPTR       mentry;
    TOKEN       *p_token;

    str = copy_eq( Buffer, str);
    i = strlen( Buffer );
    if( i == 0 ) {
        CErr1( ERR_NO_MACRO_ID_COMMAND_LINE );
        return( str );
    }
    mentry = (MEPTR) CMemAlloc( sizeof( MEDEFN ) + i );
    strcpy( mentry->macro_name, Buffer );
    mentry->parm_count = 0;
    i = 0;
    if( !EqualChar( *str ) ) {
        p_token = (TOKEN *)&TokenBuf[i];
        *p_token = T_PPNUMBER;
        i += sizeof( TOKEN );
        TokenBuf[ i++ ] = '1';
        TokenBuf[ i++ ] = '\0';
    } else {
        int ppscan_mode;

        ppscan_mode = InitPPScan();
        ReScanInit( ++str );
        for(;;) {
            if( *str == '\0' ) break;
            ReScanToken();
            if( ReScanPos() == str ) break;
            if( CurToken == T_WHITE_SPACE ) break;      /* 28-apr-94 */
            if( CurToken == T_BAD_CHAR && ! multiple_tokens ) break;
            p_token = (TOKEN *)&TokenBuf[i];
            *p_token = CurToken;
            i += sizeof( TOKEN );

            switch( CurToken ) {
            case T_BAD_CHAR:
                TokenBuf[i++] = Buffer[0];
                break;
            case T_CONSTANT:
            case T_PPNUMBER:
                /* fall through */
            case T_ID:
                ++TokenLen; /* to pick up NULLCHAR */
                /* fall through */
            case T_LSTRING:
            case T_STRING:
                memcpy( &TokenBuf[i], &Buffer[0], TokenLen );
                i += TokenLen;
                break;
            default:
                break;
            }
            str = ReScanPos();
            if( !multiple_tokens ) break;
        }
        FiniPPScan( ppscan_mode );
    }
    *(TOKEN *)&TokenBuf[i] = T_NULL;
    if( strcmp( mentry->macro_name, "defined" ) != 0 ){
        MacroAdd( mentry, TokenBuf, i + sizeof( TOKEN ), mflags );
    }else{
        CErr1( ERR_CANT_DEFINE_DEFINED );
        CMemFree( mentry );
    }
    return( str );
}

char *Define_Macro( char *str )
{
    return( Def_Macro_Tokens( str, CompFlags.extended_defines, MFLAG_NONE ) );
}

char *Define_UserMacro( char *str )
{
    return( Def_Macro_Tokens( str, CompFlags.extended_defines, MFLAG_USER_DEFINED ) );
}

void PreDefine_Macro( char *str )
{
    struct undef_names  *uname;
    int                 len;
    char                *p;

    if( ! CompFlags.undefine_all_macros ) {
        if( UndefNames != NULL ) {
            p = str;
            len = 0;
            for(;;) {
                if( *p == '\0' ) break;
                if( EqualChar( *p ) ) break;
                ++len;
                ++p;
            }
            for( uname = UndefNames; uname; uname = uname->next ) {
                if( memcmp( uname->name, str, len ) == 0 ) {
                    if( uname->name[len] == '\0' ) return;
                }
            }
        }
        Def_Macro_Tokens( str, 1, MFLAG_CAN_BE_REDEFINED );
    }
}


char *AddUndefName( char *str )
{
    int                 len;
    struct undef_names  *uname;

    len = strlen( str );
    if( len == 0 ) {
        CompFlags.undefine_all_macros = 1;
    } else {
        CalcHash( str, len );
        if( !MacroDel( str ) ){
            uname = (struct undef_names *)CMemAlloc(sizeof(struct undef_names));
            uname->next = UndefNames;
            uname->name = CMemAlloc( len + 1 );
            memcpy( uname->name, str, len );
            uname->name[len] = '\0';
            UndefNames = uname;
            str += len;
        }
    }
    return( str );
}


static void FreeUndefNames( void )
{
    struct undef_names *uname;

    for(; (uname = UndefNames); ) {
        UndefNames = uname->next;
        CMemFree( uname->name );
        CMemFree( uname );
    }
}


static void Define_Extensions( void )
{
    PreDefine_Macro( "_far16=__far16" );
    PreDefine_Macro( "near=__near" );
    PreDefine_Macro( "far=__far" );
    PreDefine_Macro( "huge=__huge" );
    PreDefine_Macro( "cdecl=__cdecl" );
    PreDefine_Macro( "pascal=__pascal" );
    PreDefine_Macro( "fortran=__fortran" );
    PreDefine_Macro( "interrupt=__interrupt" );
    PreDefine_Macro( "_near=__near" );
    PreDefine_Macro( "_far=__far" );
    PreDefine_Macro( "_huge=__huge" );
    PreDefine_Macro( "_cdecl=__cdecl" );
    PreDefine_Macro( "_pascal=__pascal" );
    PreDefine_Macro( "_fortran=__fortran" );
    PreDefine_Macro( "_inline=__inline" );
    PreDefine_Macro( "_fastcall=__fastcall" );
    PreDefine_Macro( "_interrupt=__interrupt" );
    PreDefine_Macro( "_export=__export" );
    PreDefine_Macro( "_loadds=__loadds" );
    PreDefine_Macro( "_saveregs=__saveregs" );
    PreDefine_Macro( "_stdcall=__stdcall" );
    PreDefine_Macro( "_syscall=__syscall" );
    PreDefine_Macro( "_based=__based" );
    PreDefine_Macro( "_self=__self" );
    PreDefine_Macro( "_segname=__segname" );
    PreDefine_Macro( "_segment=__segment" );
    PreDefine_Macro( "_try=__try");
    PreDefine_Macro( "_except=__except");
    PreDefine_Macro( "_finally=__finally");
    PreDefine_Macro( "_leave=__leave");
    PreDefine_Macro( "_asm=__asm");
#if _CPU == 8086
    /* SOM for Windows macros */
    PreDefine_Macro( "SOMLINK=__cdecl" );
    PreDefine_Macro( "SOMDLINK=__far" );
#else
    PreDefine_Macro( "SOMLINK=_Syscall" );
    PreDefine_Macro( "SOMDLINK=_Syscall" );
#endif
}


void MiscMacroDefs( void )
{
    if( CompFlags.inline_functions ) {
        Define_Macro( "__INLINE_FUNCTIONS__" );
    }
    if( ! CompFlags.extensions_enabled ) {
        Define_Macro( "NO_EXT_KEYS" );
    } else {
        Define_Extensions();
    }
    if( CompFlags.signed_char ) {
        Define_Macro( "__CHAR_SIGNED__" );
    }
    if( CompFlags.rent ) {
        Define_Macro( "__RENT__" );
    }
    PreDefine_Macro( "_PUSHPOP_SUPPORTED" );
    PreDefine_Macro( CompilerID );
    FreeUndefNames();
}

void InitModInfo( void )
{
    GenSwitches = 0;
    TargetSwitches = 0;
    Toggles = TOGGLE_CHECK_STACK
            | TOGGLE_UNREFERENCED
            | TOGGLE_REUSE_DUPLICATE_STRINGS;
    DataThreshold = 32767;
    OptSize = 50;
    UndefNames = NULL;
    WholeFName = NULL;
    ObjectFileName = NULL;
    ErrorFileName = CStrSave( "*" );
    DependFileName = NULL;
    DependForceSlash = 0;
    ModuleName = NULL;
    ErrLimit = 20;
    WngLevel = 1;
    PackAmount = 8;
#if _CPU == _AXP || _CPU == _PPC || _CPU == _MIPS
    CompFlags.make_enums_an_int = 1;     // make enums ints
    CompFlags.original_enum_setting = 1;
    PackAmount = 8;
#else
    #if _CPU == 386
        PackAmount = 8;                 // BBB - Aug 15,1996
    #else
        PackAmount = TARGET_INT;     /* pack structs on word boundaries */
    #endif
#endif
    PreProcChar = '#';
    CompFlags.check_syntax        = 0;
    CompFlags.signed_char         = 0;
    CompFlags.use_full_codegen_od = 0;
    CompFlags.inline_functions    = 0;
    CompFlags.dump_prototypes     = 0;
    CompFlags.generate_prototypes = 0;
    CompFlags.bss_segment_used    = 0;
    CompFlags.undefine_all_macros = 0;
    CompFlags.extensions_enabled  = 1;
    CompFlags.unix_ext            = 0;
    CompFlags.slack_byte_warning  = 0;
    CompFlags.errfile_written     = 0;
    CompFlags.zu_switch_used      = 0;
    CompFlags.register_conventions = 0;
    CompFlags.emit_library_any = 0;
    CompFlags.emit_library_with_main = 1;
    CompFlags.emit_dependencies   = 1;                  /* 04-dec-92 */
    CompFlags.emit_targimp_symbols  = 1;                  /* 27-oct-02 */
    CompFlags.use_unicode         = 1;                  /* 05-jun-91 */
    CompFlags.no_debug_type_names = 0;                  /* 16-aug-91 */
    CompFlags.auto_agg_inits      = 0;
    CompFlags.no_check_inits      = 0;                  /* 16-aug-91 */
    CompFlags.no_check_qualifiers = 0;                  /* 16-aug-91 */
    CompFlags.curdir_inc          = 1;
    CompFlags.use_stdcall_at_number = 1;
    CompFlags.rent = 0;

    SetAuxWatcallInfo();
}
