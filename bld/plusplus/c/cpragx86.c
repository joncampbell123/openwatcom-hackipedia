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
* Description:  Pragma processing for x86 targets.
*
****************************************************************************/


#include "plusplus.h"
#include "preproc.h"
#include "name.h"
#include "memmgr.h"
#include "errdefns.h"
#include "cgdata.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "asminlin.h"
#include "fnovload.h"
#include "cgswitch.h"
#include "initdefs.h"
#include "cginlibs.h"
#include "cginimps.h"
#include "asmstmt.h"
#include "pcheader.h"


static int      GetAliasInfo();
static byte_seq *AuxCodeDup( byte_seq *code );
static int      GetByteSeq( void );

static  hw_reg_set      asmRegsSaved = { HW_D( HW_FULL ) };

#define WCPP_ASM     // enable assembler

#define ASM_BLOCK       (64)

#define ROUND_ASM_BLOCK(x)   ((x+ASM_BLOCK-1) & ~(ASM_BLOCK-1))

static void pragmaInit(         // INITIALIZATION FOR PRAGMAS
    INITFINI* defn )            // - definition
{
    defn = defn;
    PragInit();

    PragmaAuxInfoInit( CompFlags.use_stdcall_at_number );

#if _CPU == 386
    HW_CTurnOff( asmRegsSaved, HW_EAX );
    HW_CTurnOff( asmRegsSaved, HW_EBX );
    HW_CTurnOff( asmRegsSaved, HW_ECX );
    HW_CTurnOff( asmRegsSaved, HW_EDX );
    HW_CTurnOff( asmRegsSaved, HW_ESI );
    HW_CTurnOff( asmRegsSaved, HW_EDI );
#else
    HW_CTurnOff( asmRegsSaved, HW_ABCD );
    HW_CTurnOff( asmRegsSaved, HW_SI );
    HW_CTurnOff( asmRegsSaved, HW_DI );
    HW_CTurnOff( asmRegsSaved, HW_ES );
#endif

    SetAuxDefaultInfo();
}


static void freeInfo( AUX_INFO *info )
{
    if( info->code != NULL ) {
        CMemFree( info->code );
        info->code = NULL;
    }
    if( !IsAuxParmsBuiltIn( info->parms ) ) {
        CMemFree( info->parms );
        info->parms = NULL;
    }
    if( info->objname != NULL ) {
        CMemFree( info->objname );
        info->objname = NULL;
    }
}


static void pragmaFini(         // FINISH PRAGMAS
    INITFINI* defn )            // - definition
{
    register struct aux_entry *next;
    register void             *junk;

    defn = defn;
    next = AuxList;
    while( next != NULL ) {
        junk = next;
        if( next->info != NULL ) {
            if( next->info->use != 1 ) {
                next->info->use--;
            } else {
                freeInfo( next->info );
#ifndef NDEBUG
                if( IsAuxInfoBuiltIn( next->info ) ) {
                    CFatal( "freeing a static calling convention info" );
                }
#endif
                if( !IsAuxInfoBuiltIn( next->info ) ) {
                    CMemFree( next->info );
                }
            }
        }
        next = next->next;
        CMemFree( junk );
    }
    freeInfo( &DefaultInfo );
    freeInfo( &CdeclInfo );
    freeInfo( &PascalInfo );
    freeInfo( &SyscallInfo );
    freeInfo( &OptlinkInfo );
    freeInfo( &StdcallInfo );
    freeInfo( &FastcallInfo );
#if _CPU == 386
    freeInfo( &Far16CdeclInfo );
    freeInfo( &Far16PascalInfo );
#endif
    freeInfo( &WatcallInfo );
    freeInfo( &FortranInfo );
    //CMemFreePtr( &FortranInfo.objname );
    AuxList = NULL;
    CgInfoFreeLibs();
    CgInfoFreeImports();
    CgInfoFreeImportsS();
}


INITDEFN( pragmas, pragmaInit, pragmaFini )


static void assemblerInit(      // INITIALIZATION OF ASSEMBLER
    INITFINI* defn )            // - definition
{
    int         cpu;
    int         fpu;
    int         use32;

    defn = defn;
#if _CPU == 386
    use32 = 1;
    cpu = 3;
#else
    use32 = 0;
    cpu = 0;
#endif
    switch( GET_CPU( CpuSwitches ) ) {
      case CPU_86:        cpu = 0; break;
      case CPU_186:       cpu = 1; break;
      case CPU_286:       cpu = 2; break;
      case CPU_386:       cpu = 3; break;
      case CPU_486:       cpu = 4; break;
      case CPU_586:       cpu = 5; break;
      case CPU_686:       cpu = 6; break;
    }
    switch( GET_FPU( CpuSwitches ) ) {
    case FPU_NONE:      fpu = 0; break;
    default:            fpu = 1; break;
    }
    AsmInit( cpu, fpu, use32, 1 );
}


INITDEFN( assembler, assemblerInit, InitFiniStub )


static void AuxCopy(           // COPY AUX STRUCTURE
    AUX_INFO *to,               // - destination
    AUX_INFO *from )            // - source
{
    freeAuxInfo( to );
    *to = *from;
    to->parms = AuxParmDup( from->parms );
    to->objname = AuxObjnameDup( from->objname );
    to->code = AuxCodeDup( from->code );
}

static boolean GetAliasInfo(
    void )
{
    char buff[256];
    boolean isfar16;
    boolean retn;

    CurrAlias = &DefaultInfo;
    if( CurToken != T_LEFT_PAREN ) {
        retn = TRUE;
    } else {
        NextToken();
        if( ! PragIdCurToken() ) {
            retn = FALSE;
        } else {
            PragCurrAlias();
            strcpy( buff, Buffer );
            NextToken();
            if( CurToken == T_RIGHT_PAREN ) {
                NextToken();
                retn = TRUE;
            } else if( CurToken == T_COMMA ) {
                NextToken();
                if( ! PragIdCurToken() ) {
                    retn = FALSE;
                } else {
                    isfar16 = PragRecog( "far16" );
                    CreateAux( buff );
                    PragCurrAlias();
                    NextToken();
                    if( CurToken == T_RIGHT_PAREN ) {
                        AuxCopy( CurrInfo, CurrAlias );
                        NextToken();
                    }
                    if( isfar16 ) {
                        CurrInfo->flags |= AUX_FLAG_FAR16;
                    }
                    PragEnding( TRUE );
                    retn = FALSE;
                }
            }
        }
    }
    return retn;
}

static void GetParmInfo(
    void )
    {
    struct {
        unsigned f_pop           : 1;
        unsigned f_reverse       : 1;
        unsigned f_loadds        : 1;
        unsigned f_nomemory      : 1;
        unsigned f_list          : 1;
    } have;

    have.f_pop           = 0;
    have.f_reverse       = 0;
    have.f_loadds        = 0;
    have.f_nomemory      = 0;
    have.f_list          = 0;
    for( ;; ) {
        if( !have.f_pop && PragRecog( "caller" ) ) {
            CurrInfo->cclass |= CALLER_POPS;
            have.f_pop = 1;
        } else if( !have.f_pop && PragRecog( "routine" ) ) {
            CurrInfo->cclass &= ~ CALLER_POPS;
            have.f_pop = 1;
        } else if( !have.f_reverse && PragRecog( "reverse" ) ) {
            CurrInfo->cclass |= REVERSE_PARMS;
            have.f_reverse = 1;
        } else if( !have.f_nomemory && PragRecog( "nomemory" ) ) {
            CurrInfo->cclass |= NO_MEMORY_READ;
            have.f_nomemory = 1;
        } else if( !have.f_loadds && PragRecog( "loadds" ) ) {
            CurrInfo->cclass |= LOAD_DS_ON_CALL;
            have.f_loadds = 1;
        } else if( !have.f_list && PragSet() != T_NULL ) {
            PragManyRegSets();
            have.f_list = 1;
        } else {
            break;
        }
    }
}

static void GetSTRetInfo(
    void )
{
    struct {
        unsigned f_float        : 1;
        unsigned f_struct       : 1;
        unsigned f_allocs       : 1;
        unsigned f_list         : 1;
    } have;

    have.f_float  = 0;
    have.f_struct = 0;
    have.f_allocs = 0;
    have.f_list   = 0;
    for( ;; ) {
        if( !have.f_float && PragRecog( "float" ) ) {
            have.f_float = 1;
            CurrInfo->cclass |= NO_FLOAT_REG_RETURNS;
        } else if( !have.f_struct && PragRecog( "struct" ) ) {
            have.f_struct = 1;
            CurrInfo->cclass |= NO_STRUCT_REG_RETURNS;
        } else if( !have.f_allocs && PragRecog( "routine" ) ) {
            have.f_allocs = 1;
            CurrInfo->cclass |= ROUTINE_RETURN;
        } else if( !have.f_allocs && PragRecog( "caller" ) ) {
            have.f_allocs = 1;
            CurrInfo->cclass &= ~ROUTINE_RETURN;
        } else if( !have.f_list && PragSet() != T_NULL ) {
            have.f_list = 1;
            CurrInfo->cclass |= SPECIAL_STRUCT_RETURN;
            CurrInfo->streturn = PragRegList();
        } else {
            break;
        }
    }
}

static void GetRetInfo(
    void )
{
    struct {
        unsigned f_no8087        : 1;
        unsigned f_list          : 1;
        unsigned f_struct        : 1;
    } have;

    have.f_no8087  = 0;
    have.f_list    = 0;
    have.f_struct  = 0;
    CurrInfo->cclass &= ~ NO_8087_RETURNS;
    for( ;; ) {
        if( !have.f_no8087 && PragRecog( "no8087" ) ) {
            have.f_no8087 = 1;
            HW_CTurnOff( CurrInfo->returns, HW_FLTS );
            CurrInfo->cclass |= NO_8087_RETURNS;
        } else if( !have.f_list && PragSet() != T_NULL ) {
            have.f_list = 1;
            CurrInfo->cclass |= SPECIAL_RETURN;
            CurrInfo->returns = PragRegList();
        } else if( !have.f_struct && PragRecog( "struct" ) ) {
            have.f_struct = 1;
            GetSTRetInfo();
        } else {
            break;
        }
    }
}

static void GetSaveInfo(
    void )
{
    hw_reg_set      modlist;
    hw_reg_set      default_flt_n_seg;
    hw_reg_set      flt_n_seg;

    struct {
        unsigned f_exact        : 1;
        unsigned f_nomemory     : 1;
        unsigned f_list         : 1;
    } have;

    have.f_exact    = 0;
    have.f_nomemory = 0;
    have.f_list     = 0;
    for( ;; ) {
        if( !have.f_exact && PragRecog( "exact" ) ) {
            CurrInfo->cclass |= MODIFY_EXACT;
            have.f_exact = 1;
        } else if( !have.f_nomemory && PragRecog( "nomemory" ) ) {
            CurrInfo->cclass |= NO_MEMORY_CHANGED;
            have.f_nomemory = 1;
        } else if( !have.f_list && PragSet() != T_NULL ) {
            modlist = PragRegList();
            have.f_list = 1;
        } else {
            break;
        }
    }
    if( have.f_list ) {
        HW_Asgn( default_flt_n_seg, DefaultInfo.save );
        HW_CTurnOn( CurrInfo->save, HW_FULL );
        if( !have.f_exact && !CompFlags.save_restore_segregs ) {
            HW_CAsgn( flt_n_seg, HW_FLTS );
            HW_CTurnOn( flt_n_seg, HW_SEGS );
            HW_TurnOff( CurrInfo->save, flt_n_seg );
            HW_OnlyOn( default_flt_n_seg, flt_n_seg );
            HW_TurnOn( CurrInfo->save, default_flt_n_seg );
        }
        HW_TurnOff( CurrInfo->save, modlist );
    }
}

static void doPragAux(                   // #PRAGMA AUX ...
    void )
{
    struct {
        unsigned f_call   : 1;
        unsigned f_loadds : 1;
        unsigned f_export : 1;
        unsigned f_parm   : 1;
        unsigned f_value  : 1;
        unsigned f_modify : 1;
        unsigned f_frame  : 1;
        unsigned uses_auto: 1;
    } have;

    if( !GetAliasInfo() ) return;
    CurrEntry = NULL;
    if( !PragIdCurToken() ) return;
    SetCurrInfo();
    NextToken();
    AuxCopy( CurrInfo, CurrAlias );
    PragObjNameInfo();
    have.f_call   = 0;
    have.f_loadds = 0;
    have.f_export = 0;
    have.f_parm   = 0;
    have.f_value  = 0;
    have.f_modify = 0;
    have.f_frame = 0;
    have.uses_auto = 0;
    for( ;; ) {
        if( !have.f_call && CurToken == T_EQUAL ) {
            have.uses_auto = GetByteSeq();
            have.f_call = 1;
        } else if( !have.f_call && PragRecog( "far" ) ) {
            CurrInfo->cclass |= FAR;
            have.f_call = 1;
        } else if( !have.f_call && PragRecog( "near" ) ) {
            CurrInfo->cclass &= ~FAR;
            have.f_call = 1;
        } else if( !have.f_loadds && PragRecog( "loadds" ) ) {
            CurrInfo->cclass |= LOAD_DS_ON_ENTRY;
            have.f_loadds = 1;
        } else if( !have.f_export && PragRecog( "export" ) ) {
            CurrInfo->cclass |= DLL_EXPORT;
            have.f_export = 1;
        } else if( !have.f_parm && PragRecog( "parm" ) ) {
            GetParmInfo();
            have.f_parm = 1;
        } else if( !have.f_value && PragRecog( "value" ) ) {
            GetRetInfo();
            have.f_value = 1;
        } else if( !have.f_value && PragRecog( "aborts" ) ) {
            CurrInfo->cclass |= SUICIDAL;
            have.f_value = 1;
        } else if( !have.f_modify && PragRecog( "modify" ) ) {
            GetSaveInfo();
            have.f_modify = 1;
        } else if( !have.f_frame && PragRecog( "frame" ) ) {
            CurrInfo->cclass |= GENERATE_STACK_FRAME;
            have.f_frame = 1;
        } else {
            break;
        }
    }
    if( have.uses_auto ) {
        AsmSysUsesAuto();
    }
    PragEnding( TRUE );
}

void PragAux(                   // #PRAGMA AUX ...
    void )
{
    PPState = PPS_EOL;
    doPragAux();
    PPState = PPS_EOL | PPS_NO_EXPAND;
}


typedef enum
{       FIXWORD_NONE
,       FIXWORD_FLOAT
,       FIXWORD_SEGMENT
,       FIXWORD_OFFSET
,       FIXWORD_RELOFF
} fix_words;


static fix_words FixupKeyword(
    void )
{
    fix_words retn;             // - return

    if( CurToken == T_FLOAT ) {
        NextToken();
        retn = FIXWORD_FLOAT;
    } else if( PragRecog( "seg" ) ) {
        retn = FIXWORD_SEGMENT;
    } else if( PragRecog( "offset" ) ) {
        retn = FIXWORD_OFFSET;
    } else if( PragRecog( "reloff" ) ) {
        retn = FIXWORD_RELOFF;
    } else {
        retn = FIXWORD_NONE;
    }
    return( retn );
}


enum sym_state AsmQueryExternal(
    char *name )
{
#if 1
    SYMBOL sym;
    enum sym_state state;

    state = SYM_UNDEFINED;
    sym = ScopeASMLookup( name );
    if( sym != NULL ) {
        if( SymIsAutomatic( sym ) ) {
            state = SYM_STACK;
        } else {
            state = SYM_EXTERNAL;
        }
    }
    return( state );
#else
    name = name;
    return( SYM_UNDEFINED );
#endif
}


#if _CPU == 8086
    #define SYM_INT     SYM_INT2
    #define SYM_FFAR    SYM_FAR2
    #define SYM_FNEAR   SYM_NEAR2
    #define SYM_DFAR    SYM_INT4
    #define SYM_DNEAR   SYM_INT2
#else
    #define SYM_INT     SYM_INT4
    #define SYM_FFAR    SYM_FAR4
    #define SYM_FNEAR   SYM_NEAR2
    #define SYM_DFAR    SYM_INT6
    #define SYM_DNEAR   SYM_INT4
#endif

#ifdef WCPP_ASM
static enum sym_type CodePtrType( type_flag flags )
{
    enum sym_type retn;

    if( flags & TF1_FAR ) {
        retn = SYM_FFAR;
    } else if( flags & TF1_NEAR ) {
        retn = SYM_FNEAR;
    } else if( TargetSwitches & BIG_CODE ) {
        retn = SYM_FFAR;
    } else {
        retn = SYM_FNEAR;
    }
    return retn;
}
#endif


static enum sym_type PtrType( type_flag flags )
{
    enum sym_type retn;

    if( flags & ( TF1_FAR | TF1_HUGE ) ) {
        retn = SYM_DFAR;
    } else if( flags & TF1_NEAR ) {
        retn = SYM_DNEAR;
    } else if( TargetSwitches & BIG_DATA ) {
        retn = SYM_DFAR;
    } else {
        retn = SYM_DNEAR;
    }
    return retn;
}


#ifdef WCPP_ASM

#define ENTRY_ERROR             0,
#define ENTRY_BOOL              SYM_INT1,
#define ENTRY_CHAR              SYM_INT1,
#define ENTRY_SCHAR             SYM_INT1,
#define ENTRY_UCHAR             SYM_INT1,
#define ENTRY_WCHAR             SYM_INT2,
#define ENTRY_SSHORT            SYM_INT2,
#define ENTRY_USHORT            SYM_INT2,
#define ENTRY_SINT              SYM_INT,
#define ENTRY_UINT              SYM_INT,
#define ENTRY_SLONG             SYM_INT4,
#define ENTRY_ULONG             SYM_INT4,
#define ENTRY_SLONG64           SYM_INT8,
#define ENTRY_ULONG64           SYM_INT8,
#define ENTRY_FLOAT             SYM_FLOAT4,
#define ENTRY_DOUBLE            SYM_FLOAT8,
#define ENTRY_LONG_DOUBLE       SYM_FLOAT8,
#define ENTRY_ENUM              0,
#define ENTRY_POINTER           0,
#define ENTRY_TYPEDEF           0,
#define ENTRY_CLASS             0,
#define ENTRY_BITFIELD          0,
#define ENTRY_FUNCTION          0,
#define ENTRY_ARRAY             0,
#define ENTRY_DOT_DOT_DOT       0,
#define ENTRY_VOID              SYM_INT1,
#define ENTRY_MODIFIER          0,
#define ENTRY_MEMBER_POINTER    0,
#define ENTRY_GENERIC           0,

static enum sym_type AsmDataType[] = {
    #include "type_arr.h"
};
#endif

#ifdef WCPP_ASM

static enum sym_type AsmType(
    TYPE type )
{
    type_flag mod_flags;

    type = TypeModFlags( type, &mod_flags );
    switch( type->id ) {
      case TYP_POINTER :
        type = TypeModFlags( type->of, &mod_flags );
        if( type->id == TYP_FUNCTION ) {
            return( CodePtrType( mod_flags ) );
        } else {
            return( PtrType( mod_flags ) );
        }
        break;
      case TYP_ARRAY :
      case TYP_BITFIELD :
      case TYP_ENUM :
        return( AsmType( type->of ) );
        break;
      case TYP_CLASS :
        return( SYM_INT1 );
        break;
      case TYP_FUNCTION :
        return( CodePtrType( mod_flags ) );
        break;
      default:
        return( AsmDataType[ type->id ] );
        break;
    }
}
#endif


enum sym_type AsmQueryType( char *name )
{
#if 1
    SYMBOL sym;
    enum sym_type type;

    sym = ScopeASMLookup( name );
    if( sym != NULL ) {
        type = AsmType( sym->sym_type );
    } else {
        type = SYM_INT1;
    }
    return( type );
#else
    name = name;
    return( SYM_INT1 );
#endif
}


static int insertFixups( VBUF *src_code )
{
    struct asmfixup     *fix;
    struct asmfixup     *head;
    struct asmfixup     *chk;
    struct asmfixup     *next;
    struct asmfixup     **owner;
    unsigned char       *src;
    unsigned char       *src_start;
    unsigned char       *src_end;
    unsigned char       cg_fix;
    unsigned long       perform_fixups;
    byte_seq            *seq;
    SYMBOL              sym;
    char                *name;
    unsigned char       *dst;
    unsigned            len;
    unsigned            skip;
    int                 mutate_to_segment;
    boolean             uses_auto;
#if _CPU == 8086
    int                 fixup_padding;
#endif
    VBUF                out_code;

    uses_auto = FALSE;
    perform_fixups = 0;
    head = FixupHead;
    if( head == NULL ) {
        out_code = *src_code;
    } else {
        VbufInit( &out_code );
        FixupHead = NULL;
        /* sort the fixup list in increasing fixup_loc's */
        for( fix = head; fix != NULL; fix = next ) {
            owner = &FixupHead;
            for( ;; ) {
                chk = *owner;
                if( chk == NULL ) break;
                if( chk->fixup_loc > fix->fixup_loc ) break;
                owner = &chk->next;
            }
            next = fix->next;
            fix->next = *owner;
            *owner = fix;
        }
        len = 0;
        src_start = VbufBuffer( src_code );
        src_end = src_start + VbufLen( src_code );
        fix = FixupHead;
        owner = &FixupHead;
        /* insert fixup escape sequences */
        for( src = src_start; src < src_end; ) {
            /* reserve at least ASM_BLOCK bytes in the buffer */
            VbufReqd( &out_code, ROUND_ASM_BLOCK( len + ASM_BLOCK ) );
            dst = VbufBuffer( &out_code );
            if( fix != NULL && fix->fixup_loc == (src - src_start) ) {
                name = fix->name;
                if( name != NULL ) {
                    sym = ScopeASMUseSymbol( name, &uses_auto );
                    if( sym == NULL ) {
                        return( 0 );
                    }
                }
                /* insert fixup information */
                skip = 0;
                dst[ len++ ] = FLOATING_FIXUP_BYTE;
                mutate_to_segment = 0;
#if _CPU == 8086
                fixup_padding = 0;
#endif
                switch( fix->fixup_type ) {
                case FIX_SEG:
                    if( name == NULL ) {
                        /* special case for floating point fixup */
                        if( src[0] == 0x9b ) {
                           // FWAIT instruction as first byte
                        } else if( ( src[0] == 0x90 ) && ( src[1] == 0x9B ) ) {
                           // inline assembler FWAIT instruction 0x90, 0x9b
                        } else if( ( src[0] & 0xd8 ) == 0xd8 ) {
                           // FPU instruction, add FWAIT before it
                            dst[ len++ ] = 0x9b;
                        } else {
                            // FIXME - probably wrong use of float !!!!
                        }
                    } else {
                        skip = 2;
                        cg_fix = FIX_SYM_SEGMENT;
                    }
                    break;
                case FIX_RELOFF16:
                    skip = 2;
                    cg_fix = FIX_SYM_RELOFF;
                    break;
                case FIX_RELOFF32:
                    skip = 4;
                    cg_fix = FIX_SYM_RELOFF;
#if _CPU == 8086
                    fixup_padding = 1;
#endif
                    break;
                case FIX_PTR16:
                    mutate_to_segment = 1;
                    /* fall through */
                case FIX_OFF16:
                    skip = 2;
                    cg_fix = FIX_SYM_OFFSET;
                    break;
                case FIX_PTR32:
                    mutate_to_segment = 1;
                    /* fall through */
                case FIX_OFF32:
                    skip = 4;
                    cg_fix = FIX_SYM_OFFSET;
#if _CPU == 8086
                    fixup_padding = 1;
#endif
                    break;
                default:
                    CErr2p( ERR_ASSEMBLER_ERROR, "cannot reach label" );
                    break;
                }
                if( skip != 0 ) {
                    dst[ len++ ] = cg_fix;
                    *((unsigned long *)&dst[ len ]) = (unsigned long)sym;
                    len += sizeof( long );
                    *((unsigned long *)&dst[ len ]) = fix->offset;
                    len += sizeof( long );
                    src += skip;
                }
#if _CPU == 8086
                if( fixup_padding ) {
                    // add offset fixup padding to 32-bit
                    // cg create only 16-bit offset fixup
                    dst[ len++ ] = 0;
                    dst[ len++ ] = 0;
                    //
                }
#endif
                if( mutate_to_segment ) {
                    /*
                        Since the CG escape sequences don't allow for
                        FAR pointer fixups, we have to split them into two.
                        This is done by doing the offset fixup first, then
                        mutating the fixup structure to look like a segment
                        fixup one near pointer size later.
                    */
                    fix->fixup_type = FIX_SEG;
                    fix->fixup_loc += skip;
                    fix->offset = 0;
                } else {
                    head = fix;
                    fix = fix->next;
                    if( head->external ) {
                        *owner = fix;
                        CMemFree( head->name );
                        CMemFree( head );
                    } else {
                        owner = &head->next;
                    }
                }
            } else {
                if( *src == FLOATING_FIXUP_BYTE ) {
                    dst[ len++ ] = FLOATING_FIXUP_BYTE;
                }
                dst[ len++ ] = *src++;
            }
            VbufSetLen( &out_code, len );
        }
        perform_fixups = DO_FLOATING_FIXUPS;
    }
    len = VbufLen( &out_code );
    seq = CMemAlloc( offsetof( byte_seq, data ) + len );
    seq->length = len | perform_fixups;
    memcpy( seq->data, VbufBuffer( &out_code ), len );
    CurrInfo->code = seq;
    if( VbufBuffer( &out_code ) != VbufBuffer( src_code ) )
        VbufFree( &out_code );
    return( uses_auto );
}


static void AddAFix(
    unsigned i,
    char *name,
    unsigned type,
    unsigned long off )
{
    struct asmfixup        *fix;

    fix = (struct asmfixup *)CMemAlloc( sizeof( *fix ) );
    fix->external = 1;
    fix->fixup_loc = i;
    fix->name = name;
    fix->offset = off;
    fix->fixup_type = type;
    fix->next = FixupHead;
    FixupHead = fix;
}

static byte_seq *AuxCodeDup(        // DUPLICATE AUX CODE
    byte_seq *code )
{
    byte_seq *new_code;
    byte_seq_len code_length;
    byte_seq_len size;

    if( code == NULL ) {
        return( code );
    }
    code_length = code->length & MAX_BYTE_SEQ_LEN;
    size = offsetof( byte_seq, data ) + code_length;
    new_code = (byte_seq *)vctsave( (char *)code, size );
    return( new_code );
}

void AsmSysCopyCode( void )
/*************************/
{
    byte_seq *code;

    code = CurrInfo->code;
    if( code != NULL && code == CurrAlias->code ) {
        CurrInfo->code = AuxCodeDup( code );
    }
}

boolean AsmSysInsertFixups( VBUF *code )
/**************************************/
{
    boolean uses_auto;

    uses_auto = insertFixups( code );
    AsmSymFini();
    return( uses_auto );
}

AUX_INFO *AsmSysCreateAux( char *name )
/*************************************/
{
    CreateAux( name );
    AuxCopy( CurrInfo, &WatcallInfo );
    CurrInfo->use = 1;
    CurrInfo->save = asmRegsSaved;
    CurrEntry->info = CurrInfo;
    return( CurrInfo );
}

void AsmSysUsesAuto( void )
/*************************/
{
    /*
       We want to force the calling routine to set up a [E]BP frame
       for the use of this pragma. This is done by saying the pragma
       modifies the [E]SP register. A kludge, but it works.
    */
    HW_CTurnOff( CurrInfo->save, HW_SP );
    ScopeASMUsesAuto();
}

char const *AsmSysDefineByte( void )
/**********************************/
{
    return( "db" );
}

void AsmSysDone( void )
/*********************/
{
    PragEnding( FALSE );
}

void AsmSysInit( void )
/*********************/
{
    AsmCodeAddress = 0;
    AsmSaveCPUInfo();
}

void AsmSysFini( void )
/*********************/
{
    AsmRestoreCPUInfo();
}

static byte *copyCodeLen( byte *d, void *v, unsigned len )
{
    byte *s = v;

    while( len ) {
        *d++ = *s++;
        --len;
    }
    return( d );
}

void AsmSysPCHWriteCode( AUX_INFO *info )
/***************************************/
{
    SYMBOL sym;
    byte_seq_len code_length;
    byte_seq_len seq_size;
    unsigned fixup;
    byte_seq *code;
    byte *d;
    byte *s;
    byte *c;
    byte *p;
    byte *tmp_buff;
#ifndef NDEBUG
    byte buff[8];
#else
    byte buff[1024];
#endif

    seq_size = 0;
    code = info->code;
    if( code == NULL ) {
        /* no code */
        PCHWriteUInt( seq_size );
        return;
    }
    code_length = code->length & MAX_BYTE_SEQ_LEN;
    seq_size = offsetof( byte_seq, data ) + code_length;
    PCHWriteUInt( seq_size );
    if(( code->length & DO_FLOATING_FIXUPS ) == 0 ) {
        /* code has no relocs */
        PCHWrite( code, seq_size );
        return;
    }
    /* code has relocs */
    tmp_buff = buff;
    if( seq_size > sizeof( buff ) ) {
        tmp_buff = CMemAlloc( seq_size );
    }
    c = code->data;
    s = code->data + code_length;
    d = copyCodeLen( tmp_buff, code, offsetof( byte_seq, data ) );
    for( p = c; p != s; ) {
        if( p[0] == FLOATING_FIXUP_BYTE ) {
            fixup = p[1];
            *d++ = *p++;
            *d++ = *p++;
            if( fixup != FLOATING_FIXUP_BYTE ) {
                switch( fixup ) {
                case FIX_SYM_OFFSET:
                case FIX_SYM_SEGMENT:
                case FIX_SYM_RELOFF:
                    sym = SymbolGetIndex( *((SYMBOL*)p) );
                    d = copyCodeLen( d, &sym, sizeof( sym ) );
                    p += sizeof( SYMBOL );
                    d = copyCodeLen( d, p, sizeof( unsigned long ) );
                    p += sizeof( unsigned long );
                    break;
                case 0x9B: // FWAIT
                    break;
                default:
                    DbgNever();
                }
            }
        } else {
            *d++ = *p++;
        }
    }
    PCHWrite( tmp_buff, seq_size );
    if( tmp_buff != buff ) {
        CMemFree( tmp_buff );
    }
}

void AsmSysPCHReadCode( AUX_INFO *info )
/**************************************/
{
    byte *p;
    byte *s;
    byte_seq *code;
    unsigned fixup;
    SYMBOL sym;
    byte_seq_len seq_size;
    byte_seq_len code_length;

    seq_size = PCHReadUInt();
    if( seq_size == 0 ) {
        info->code = NULL;
        return;
    }
    code = CMemAlloc( seq_size );
    info->code = code;
    PCHRead( code, seq_size );
    if(( code->length & DO_FLOATING_FIXUPS ) == 0 ) {
        /* code has no relocs */
        return;
    }
    code_length = code->length & MAX_BYTE_SEQ_LEN;
    p = code->data;
    s = code->data + code_length;
    while( p != s ) {
        if( p[0] == FLOATING_FIXUP_BYTE ) {
            fixup = p[1];
            p += 2;
            if( fixup != FLOATING_FIXUP_BYTE ) {
                switch( fixup ) {
                case FIX_SYM_OFFSET:
                case FIX_SYM_SEGMENT:
                case FIX_SYM_RELOFF:
                    sym = SymbolMapIndex( *((SYMBOL*)p) );
                    copyCodeLen( p, &sym, sizeof( sym ) );
                    p += sizeof( SYMBOL ) + sizeof( unsigned long );
                    break;
                case 0x9B: // FWAIT
                    break;
                default:
                    DbgNever();
                }
            }
        } else {
            ++p;
        }
    }
}

static int GetByteSeq( void )
{
    int             len;
    char            *name;
    unsigned long   offset;
    unsigned        fixword;
    int             uses_auto;
    VBUF            code_buffer;

    VbufInit( &code_buffer );
    AsmSysInit();
    NextToken();
    len = 0;
    for( ;; ) {
        /* reserve at least ASM_BLOCK bytes in the buffer */
        VbufReqd( &code_buffer, ROUND_ASM_BLOCK( len + ASM_BLOCK ) );
        if( CurToken == T_STRING ) {
            AsmCodeAddress = len;
            AsmCodeBuffer = VbufBuffer( &code_buffer );
            AsmLine( Buffer );
            len = AsmCodeAddress;
            NextToken();
            if( CurToken == T_COMMA )  NextToken();
        } else if( CurToken == T_CONSTANT ) {
            VbufBuffer( &code_buffer )[ len++ ] = U32Fetch( Constant64 );
            NextToken();
        } else {
            fixword = FixupKeyword();
            if( fixword == FIXWORD_NONE ) break;
            if( fixword == FIXWORD_FLOAT ) {
#if _CPU == 8086
                if( GET_FPU_EMU( CpuSwitches ) ) {
                    AddAFix( len, NULL, FIX_SEG, 0 );
                }
#endif
            } else { /* seg or offset */
                if( !PragIdCurToken() ) {
                    CErr1( ERR_EXPECTING_ID );
                } else {
                    name = strsave( Buffer );
                    offset = 0;
                    NextToken();
                    if( CurToken == T_PLUS ) {
                        NextToken();
                        if( CurToken == T_CONSTANT ) {
                            offset = U32Fetch( Constant64 );
                            NextToken();
                        }
                    } else if( CurToken == T_MINUS ) {
                        NextToken();
                        if( CurToken == T_CONSTANT ) {
                            offset = - U32Fetch( Constant64 );
                            NextToken();
                        }
                    }
                }
                switch( fixword ) {
                case FIXWORD_RELOFF:
#if _CPU == 8086
                    AddAFix( len, name, FIX_RELOFF16, offset );
                    len += 2;
#else
                    AddAFix( len, name, FIX_RELOFF32, offset );
                    len += 4;
#endif
                    break;
                case FIXWORD_OFFSET:
#if _CPU == 8086
                    AddAFix( len, name, FIX_OFF16, offset );
                    len += 2;
#else
                    AddAFix( len, name, FIX_OFF32, offset );
                    len += 4;
#endif
                    break;
                case FIXWORD_SEGMENT:
                    AddAFix( len, name, FIX_SEG, 0 );
                    len += 2;
                    break;
                }
            }
        }
        VbufSetLen( &code_buffer, len );
    }
    uses_auto = AsmSysInsertFixups( &code_buffer );
    AsmSysFini();
    VbufFree( &code_buffer );
    return( uses_auto );
}


hw_reg_set PragRegName(         // GET REGISTER NAME
    char *str )                 // - register
{
    register int index;
    register char *p;
    hw_reg_set      name;

    if( *str == '\0' ) {
        HW_CAsgn( name, HW_EMPTY );
        return( name );
    }
    if( *str == '_' ) {
        ++str;
        if( *str == '_' ) {
            ++str;
        }
    }
    index = 0;
    p = Registers;
    while( *p != '\0' ) {
        if( stricmp( p, str ) == 0 )
            return( RegBits[ index ] );
        index++;
        while( *p++ != '\0' ) {
            ;
        }
    }
    if( strcmp( str, "8087" ) == 0 ) {
        HW_CAsgn( name, HW_FLTS );
    } else {
        CErr2p( ERR_BAD_REGISTER_NAME, str );
        HW_CAsgn( name, HW_EMPTY );
    }
    return( name );
}

static boolean parmSetsIdentical( hw_reg_set *parms1, hw_reg_set *parms2 )
{
    if( parms1 == parms2 ) {
        return( TRUE );
    }
    if( parms1 != NULL && parms2 != NULL ) {
        for(;;) {
            if( HW_Equal( *parms1, *parms2 ) ) {
                if( HW_CEqual( *parms1, HW_EMPTY ) ) {
                    return( TRUE );
                }
                ++parms1;
                ++parms2;
            } else {
                break;
            }
        }
    }
    return( FALSE );
}

// The following defines which flags are to be ignored when checking
// a pragma call classes for equivalence.
//
#define CALL_CLASS_IGNORE ( 0                       \
                          | NO_MEMORY_CHANGED       \
                          | NO_MEMORY_READ          \
                          | MODIFY_EXACT            \
                          | GENERATE_STACK_FRAME    \
                          | EMIT_FUNCTION_NAME      \
                          | GROW_STACK              \
                          | PROLOG_HOOKS            \
                          | EPILOG_HOOKS            \
                          | TOUCH_STACK             \
                          | LOAD_DS_ON_ENTRY        \
                          | DLL_EXPORT              \
                          )

boolean PragmasTypeEquivalent(  // TEST IF TWO PRAGMAS ARE TYPE-EQUIVALENT
    AUX_INFO *inf1,             // - pragma [1]
    AUX_INFO *inf2 )            // - pragma [2]
{
    if( inf1 == NULL ) {
        inf1 = &DefaultInfo;
    }
    if( inf2 == NULL ) {
        inf2 = &DefaultInfo;
    }
    if( inf1 == inf2 ) {
        return TRUE;
    }
    return
           ( ( inf1->cclass & ~CALL_CLASS_IGNORE ) ==
             ( inf2->cclass & ~CALL_CLASS_IGNORE ) )
        && parmSetsIdentical( inf1->parms, inf2->parms )
        && HW_Equal( inf1->returns, inf2->returns )
        && HW_Equal( inf1->streturn, inf2->streturn )
        && HW_Equal( inf1->save, inf2->save )
        && ( inf1->flags == inf2->flags );
}

boolean PragmaOKForInlines(     // TEST IF PRAGMA IS SUITABLE FOR INLINED FN
    AUX_INFO *fnp )             // - pragma
{
    if( fnp->code != NULL ) {
        return FALSE;
    }
    if( ReverseParms( fnp ) ) {
        return FALSE;
    }
    return TRUE;
}

boolean PragmaOKForVariables(   // TEST IF PRAGMA IS SUITABLE FOR A VARIABLE
    AUX_INFO *datap )           // - pragma
{
    AUX_INFO *def_info;

    // only the obj_name can be set for variables everything else is
    // specific to functions except for "__cdecl" and "__syscall"
    if( datap == &CdeclInfo ) {
        return( TRUE );
    }
    if( datap == &SyscallInfo ) {
        return( TRUE );
    }
    def_info = &DefaultInfo;
    if( datap->cclass != def_info->cclass ) {
        return( FALSE );
    }
    if( datap->code != def_info->code ) {
        return( FALSE );
    }
    if( ! parmSetsIdentical( datap->parms, def_info->parms ) ) {
        return( FALSE );
    }
    if( ! HW_Equal( datap->returns, def_info->returns ) ) {
        return( FALSE );
    }
    if( ! HW_Equal( datap->streturn, def_info->streturn ) ) {
        return( FALSE );
    }
    if( ! HW_Equal( datap->save, def_info->save ) ) {
        return( FALSE );
    }
    if( datap->flags != def_info->flags ) {
        return( FALSE );
    }
    return( TRUE );
}


static boolean okClassChange(   // TEST IF OK TO CHANGE A CLASS IN PRAGMA
    call_class oldp,                 // - old
    call_class newp,                 // - new
    call_class defp )                // - default
{
    return ( ( oldp & newp) == oldp ) || ( oldp == defp );
}

static boolean okPtrChange(     // TEST IF OK TO CHANGE A PTR IN PRAGMA
    void *oldp,                 // - old ptr
    void *newp,                 // - new ptr
    void *defp )                // - default pointer
{
    return ( oldp == newp ) || ( oldp == defp );
}


static boolean okStrChange(     // TEST IF OK TO CHANGE A STRING IN PRAGMA
    void *oldp,                 // - old ptr
    void *newp,                 // - new ptr
    void *defp )                // - default pointer
{
    if( oldp == NULL || newp == NULL || defp == NULL ) {
        return ( oldp == newp ) || ( oldp == defp );
    }
    return ( 0 == strcmp( oldp, newp ) )
        || ( 0 == strcmp( oldp, defp ) );
}


static boolean okHwChange(      // TEST IF OK TO CHANGE HARDWARE DEFINITION
    hw_reg_set oldr,            // - reg. set, old
    hw_reg_set newr,            // - reg. set, new
    hw_reg_set defr )           // - reg. set, default
{
    return HW_Equal( oldr, newr ) || HW_Equal( oldr, defr );
}


static boolean okParmChange(    // TEST IF OK TO CHANGE PARMS
    hw_reg_set *oldr,           // - addr[ reg. set, old ]
    hw_reg_set *newr,           // - addr[ reg. set, new ]
    hw_reg_set *defr )          // - addr[ reg. set, default ]
{
    return parmSetsIdentical( oldr, newr ) || parmSetsIdentical( oldr, defr );
}


boolean PragmaChangeConsistent( // TEST IF PRAGMA CHANGE IS CONSISTENT
    AUX_INFO *oldp,             // - pragma (old)
    AUX_INFO *newp )            // - pragma (new)
{
    if( oldp == NULL ) {
        oldp = &DefaultInfo;
    }
    if( newp == NULL ) {
        newp = &DefaultInfo;
    }
    if( oldp == newp ) {
        return TRUE;
    }
    return( ( okClassChange( oldp->cclass
                         , newp->cclass
                         , DefaultInfo.cclass ) )
        && ( ( oldp->flags & newp->flags ) == oldp->flags )
        && ( okParmChange( oldp->parms
                         , newp->parms
                         , DefaultInfo.parms ) )
        && ( okHwChange( oldp->returns
                       , newp->returns
                       , DefaultInfo.returns ) )
        && ( okHwChange( oldp->streturn
                       , newp->streturn
                       , DefaultInfo.streturn ) )
        && ( okHwChange( oldp->save
                       , newp->save
                       , DefaultInfo.save ) )
        && ( okStrChange( oldp->objname
                        , newp->objname
                        , DefaultInfo.objname ) )
        && ( okPtrChange( oldp->code
                        , newp->code
                        , DefaultInfo.code ) ) );
}
