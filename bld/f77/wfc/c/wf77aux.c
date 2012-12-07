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
* Description:  Auxiliary information processing.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fcgbls.h"
#include "wf77aux.h"
#include "errcod.h"
#include "cpopt.h"
#include "progsw.h"
#include "fio.h"
#include "sdfile.h"
#include "fmemmgr.h"
#include "ferror.h"
#include "inout.h"
#include "cspawn.h"

#if _INTEL_CPU
  #include "asminlin.h"
#elif ( _CPU == _AXP || _CPU == _PPC )
  #include "asinline.h"
#else
  #error Unknow Target
#endif

#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

extern  int             KwLookUp(char **,int,char *,int,bool);
extern  int             MkHexConst(char *,char *,int);
extern  char            *SkipBlanks(char *);
extern  aux_info        *AuxLookupName(char *,int);
extern  sym_id          SymFind(char *,uint);
extern  void            FreeChain(void *);

static  aux_info        *CurrAux;
static  char            *TokStart;
static  char            *TokEnd;
static  aux_info        *AliasInfo;
static  char            SymName[MAX_SYMLEN];
static  int             SymLen;

#if ( _CPU == 8086 || _CPU == 386 )
static  arr_info        *ArrayInfo;
#endif

extern  char            *RegNames[];
extern  hw_reg_set      RegValue[];
extern  byte            MaxReg;
#if _CPU == 8086
extern  hw_reg_set      WinParms[];
#elif _CPU == 386
extern  hw_reg_set      StackParms[];
#endif
extern  aux_info        IFInfo;
extern  aux_info        IFCharInfo;
extern  aux_info        IFChar2Info;
extern  aux_info        IFVarInfo;
extern  aux_info        IFXInfo;
extern  aux_info        CoRtnInfo;
extern  aux_info        RtRtnInfo;
extern  aux_info        RtVarInfo;
extern  aux_info        RtStopInfo;

extern  char            MsHexConst[];
extern  char            MsPragCallBytes[];
extern  char            MsArray[];

#if _CPU == 8086

#define _FLIBM          5
#define _FLIB7M         6
#define _FLIBL          5
#define _FLIB7L         6
#define _CLIBM          5
#define _CLIBL          5
#define _MATHM          5
#define _MATH7M         7
#define _MATHL          5
#define _MATH7L         7
#define _EMU87          5
#define _NOEMU87        7
#define _WRESL          5
#define _WRESM          5

static  char            _flibm[] = { "flibm" };
static  char            _flib7m[] = { "flib7m" };
static  char            _flibl[] = { "flibl" };
static  char            _flib7l[] = { "flib7l" };
static  char            _clibm[] = { "clibm" };
static  char            _clibl[] = { "clibl" };
static  char            _mathm[] = { "mathm" };
static  char            _math7m[] = { "math87m" };
static  char            _mathl[] = { "mathl" };
static  char            _math7l[] = { "math87l" };
static  char            _emu87[] = { "emu87" };
static  char            _noemu87[] = { "noemu87" };
static  char            _wresl[] = { "wresl" };
static  char            _wresm[] = { "wresm" };

#elif _CPU == 386

#define _FLIB           4
#define _FLIB7          5
#define _FLIBS          5
#define _FLIB7S         6
#define _CLIB           6
#define _CLIBS          6
#define _MATH           6
#define _MATHS          6
#define _MATH7          8
#define _MATH7S         8
#define _EMU387         6
#define _NOEMU387       8
#define _WRESF          5
#define _WRESFS         6

static  char            _flib[] = { "flib" };
static  char            _flibs[] = { "flibs" };
static  char            _flib7[] = { "flib7" };
static  char            _flib7s[] = { "flib7s" };
static  char            _clib[] = { "clib3r" };
static  char            _clibs[] = { "clib3s" };
static  char            _math[] = { "math3r" };
static  char            _maths[] = { "math3s" };
static  char            _math7[] = { "math387r" };
static  char            _math7s[] = { "math387s" };
static  char            _emu387[] = { "emu387" };
static  char            _noemu387[] = { "noemu387" };
static  char            _wresf[] = { "wresf" };
static  char            _wresfs[] = { "wresfs" };

#elif _CPU == _AXP

#define _FLIB           4
#define _CLIB           4
#define _MATH           4
#define _WRESAXP        7

static  char            _flib[] = { "flib" };
static  char            _clib[] = { "clib" };
static  char            _math[] = { "math" };
static  char            _wresaxp[] = { "wresaxp" };

#elif _CPU == _PPC

#define _FLIB           4
#define _CLIB           4
#define _MATH           4
#define _WRESPPC        7

static  char            _flib[] = { "flib" };
static  char            _clib[] = { "clib" };
static  char            _math[] = { "math" };
static  char            _wresppc[] = { "wresppc" };

#else
  #error Unknown Platform
#endif

#define MAX_REG_SETS    16
#define MAXIMUM_BYTESEQ 127

#if ( _CPU == 8086 || _CPU == 386 )
#elif _CPU == _AXP || _CPU == _PPC
  #define AsmSymFini    AsmFini
#else
  #error Unknown Target
#endif


#if _CPU == 386
    static      char    __Syscall[] = { "aux __syscall \"*\""
                                    "parm caller []"
                                    "value struct struct caller []"
                                    "modify [eax ecx edx]" };
    static      char    __Cdecl[] =   { "aux __cdecl \"_*\""
                                    "parm caller loadds []"
                                    "value struct float struct routine [eax]"
                                    "modify [eax ebx ecx edx]" };
    static      char    __Pascal[] =  { "aux __pascal \"^\""
                                    "parm reverse routine []"
                                    "value struct float struct caller []"
                                    "modify [eax ebx ecx edx]" };
    static      char    __Stdcall[] = { "aux __stdcall \"_*#\""
                                    "parm routine []"
                                    "value struct []"
                                    "modify [eax ecx edx]" };
#elif _CPU == 8086
    static      char    __Pascal[] =  { "aux __pascal \"^\""
                                    "parm routine reverse []"
                                    "value struct float struct caller []"
                                    "modify [ax bx cx dx]" };
    static      char    __Cdecl[] =   { "aux __cdecl \"_*\""
                                    "parm caller []"
                                    "value struct float struct routine [ax]"
                                    "modify [ax bx cx dx]" };
#endif

extern  aux_info        DefaultInfo;

default_lib             *DefaultLibs;
aux_info                *AuxInfo;
aux_info                FortranInfo;
aux_info                ProgramInfo;
dep_info                *DependencyInfo;

/* Forward declarations */
static  void    FreeAuxEntry( aux_info *aux );
static  void    FreeAuxElements( aux_info *aux );
static  void    FreeArgList( aux_info *aux );
static  void    ScanToken( void );
static  void    ScanFnToken( void );
static  void    SymbolId( void );
static  void    TokUpper( void );
static  void    ReqToken( char *tok );
static  void    AliasName( void );
static  void    SymbolName( void );
static  void    ProcessAlias( void );
static  void    DupCallBytes( aux_info *dst, aux_info *src );
static  void    ObjectName( void );
static  void    GetParmInfo( void );
static  void    GetByteSeq( void );
static  void    GetRetInfo( void );
static  void    GetSaveInfo( void );
static  void    GetArgList( void );
static  void    GetSTRetInfo( void );
static  void    DupParmInfo( aux_info *dst, aux_info *src );
static  void    DupObjectName( aux_info *dst, aux_info *src );
static  void    DupArgInfo( aux_info *dst, aux_info *src );
void    CopyAuxInfo( aux_info *dst, aux_info *src );
void    DoPragma( char *ptr );


void            InitAuxInfo( void ) {
//=============================

#if ( _CPU == 8086 || _CPU == 386 )
    int         cpu;
    int         fpu;
    int         use32;

#if _CPU == 8086
    use32 = 0;
#elif _CPU == 386
    use32 = 1;
#endif
    cpu = 0;
    fpu = 0;
#if _CPU == 8086
    if( CPUOpts & CPUOPT_80186 ) cpu = 1;
    if( CPUOpts & CPUOPT_80286 ) cpu = 2;
#endif
    if( CPUOpts & CPUOPT_80386 ) cpu = 3;
    if( CPUOpts & CPUOPT_80486 ) cpu = 4;
    if( CPUOpts & CPUOPT_80586 ) cpu = 5;
    if( CPUOpts & CPUOPT_80686 ) cpu = 6;
    if( CPUOpts & ( CPUOPT_FPI87 | CPUOPT_FPI ) ) fpu = 1;
    AsmInit( cpu, fpu, use32, 1 );
#elif _CPU == _AXP || _CPU == _PPC
    AsmInit();
#else
    #error Unknown Target
#endif

    DefaultLibs = NULL;
    AuxInfo = NULL;
    DependencyInfo = NULL;
#if _CPU == 8086 || _CPU == 386

#if _CPU == 8086
    // Change auxiliary information for calls to run-time routines to match
    // the options used to compile the run-time routines
    if( CGOpts & CGOPT_M_LARGE ) {
        if( !(CGOpts & CGOPT_WINDOWS) ) {
            HW_CTurnOff( IFXInfo.save, HW_DS );
            HW_CTurnOff( RtRtnInfo.save, HW_DS );
            HW_CTurnOff( RtStopInfo.save, HW_DS );
            HW_CTurnOff( RtVarInfo.save, HW_DS );
            HW_CTurnOff( CoRtnInfo.save, HW_DS );
            HW_CTurnOff( IFInfo.save, HW_DS );
            HW_CTurnOff( IFCharInfo.save, HW_DS );
            HW_CTurnOff( IFChar2Info.save, HW_DS );
            HW_CTurnOff( IFVarInfo.save, HW_DS );
        }
    }
        HW_CTurnOff( IFXInfo.save, HW_ES );
        HW_CTurnOff( RtRtnInfo.save, HW_ES );
        HW_CTurnOff( RtStopInfo.save, HW_ES );
        HW_CTurnOff( RtVarInfo.save, HW_ES );
        HW_CTurnOff( CoRtnInfo.save, HW_ES );
        HW_CTurnOff( IFInfo.save, HW_ES );
        HW_CTurnOff( IFCharInfo.save, HW_ES );
        HW_CTurnOff( IFChar2Info.save, HW_ES );
        HW_CTurnOff( IFVarInfo.save, HW_ES );
#endif

    if( !(CGOpts & CGOPT_SEG_REGS) ) {
        if( _FloatingDS( CGOpts ) ) {
            HW_CTurnOff( DefaultInfo.save, HW_DS );
        }
        if( _FloatingES( CGOpts ) ) {
            HW_CTurnOff( DefaultInfo.save, HW_ES );
        }
#if _CPU == 8086
        if( CPUOpts & (CPUOPT_80386 | CPUOPT_80486 | CPUOPT_80586 | CPUOPT_80686) )
#endif
        {
            if( _FloatingFS( CGOpts ) ) {
                HW_CTurnOff( DefaultInfo.save, HW_FS );
            }
            if( _FloatingGS( CGOpts ) ) {
                HW_CTurnOff( DefaultInfo.save, HW_GS );
            }
        }
    }
    if( OZOpts & OZOPT_O_FRAME ) {
        DefaultInfo.cclass |= GENERATE_STACK_FRAME;
    }
#endif
#if _CPU == 386
    if( CGOpts & CGOPT_STK_ARGS ) {
        DefaultInfo.cclass |= CALLER_POPS | NO_8087_RETURNS;
        DefaultInfo.parms = StackParms;
        HW_CTurnOff( DefaultInfo.save, HW_EAX );
        HW_CTurnOff( DefaultInfo.save, HW_EDX );
        HW_CTurnOff( DefaultInfo.save, HW_ECX );
        HW_CTurnOff( DefaultInfo.save, HW_FLTS );

        IFXInfo.cclass |= CALLER_POPS | NO_8087_RETURNS;
        IFXInfo.parms = StackParms;

        HW_CTurnOff( IFXInfo.save, HW_FLTS );
        HW_CTurnOff( RtRtnInfo.save, HW_FLTS );
        HW_CTurnOff( RtStopInfo.save, HW_FLTS );
        HW_CTurnOff( RtVarInfo.save, HW_FLTS );
        HW_CTurnOff( CoRtnInfo.save, HW_FLTS );
        HW_CTurnOff( IFInfo.save, HW_FLTS );
        HW_CTurnOff( IFCharInfo.save, HW_FLTS );
        HW_CTurnOff( IFChar2Info.save, HW_FLTS );
        HW_CTurnOff( IFVarInfo.save, HW_FLTS );
    }
#endif
#if _CPU == 8086
    if( CGOpts & CGOPT_WINDOWS ) {
        DefaultInfo.parms = WinParms;
        IFXInfo.parms = WinParms;
    }
#endif

    FortranInfo = DefaultInfo;
    ProgramInfo = DefaultInfo;
#if _CPU == 386
    DoPragma( __Syscall );
    DoPragma( __Stdcall );
#endif
#if _CPU == 8086 || _CPU == 386
    DoPragma( __Pascal );
    DoPragma( __Cdecl );
#endif
}


void            FiniAuxInfo( void ) {
//=============================

    void        *next;

    while( AuxInfo != NULL ) {
        next = AuxInfo->link;
        FreeAuxEntry( AuxInfo );
        AuxInfo = next;
    }
    FreeAuxElements( &FortranInfo );
    FreeChain( &DefaultLibs );
    // in case of fatal error, FiniAuxInfo() is called
    // from TDPurge()
#if ( _CPU == 8086 || _CPU == 386 )
    FreeChain( &ArrayInfo );
#endif
    FreeChain( &DependencyInfo );
    AsmSymFini();
}


void    SubAuxInit( void ) {
//====================

#if ( _CPU == 8086 || _CPU == 386 )
// Initialize aux information for a subprogram.

    ArrayInfo = NULL;
#endif
}


void    SubAuxFini( void ) {
//====================

#if ( _CPU == 8086 || _CPU == 386 )
// Finalize aux information for a subprogram.

    arr_info    *next;
    sym_id      arr;

    while( ArrayInfo != NULL ) {
        next = ArrayInfo->link;
        arr = SymFind( ArrayInfo->arr, strlen( ArrayInfo->arr ) );
        if( ( arr != NULL ) && ( arr->ns.flags & SY_SUBSCRIPTED ) &&
            ( arr->ns.typ != TY_CHAR ) &&
            ( ( arr->ns.flags & SY_SUB_PARM ) || _Allocatable( arr ) ) ) {
            arr->ns.si.va.dim_ext->dim_flags |= DIM_EXTENDED;
        }
        FMemFree( ArrayInfo );
        ArrayInfo = next;
    }
#endif
}

#if ( _CPU == 8086 || _CPU == 386 )
static  void    AddArrayInfo( char *arr_name, uint arr_len ) {
//============================================================

// Process aux information for an array.

    arr_info    **arr;
    arr_info    *new_arr;

    for( arr = &ArrayInfo; *arr != NULL; arr = &(*arr)->link ) {
        if( strlen( &(*arr)->arr ) != arr_len )
            continue;
        if( memcmp( &(*arr)->arr, arr_name, arr_len ) == 0 ) {
            return;
        }
    }
    new_arr = FMemAlloc( sizeof( arr_info ) + arr_len );
    new_arr->link = NULL;
    memcpy( &new_arr->arr, arr_name, arr_len );
    new_arr->arr[arr_len] = NULLCHAR;
    *arr = new_arr;
}
#endif

void    AddDependencyInfo( source_t *fi ) {
//=======================================

// Add dependency information for an included file.

    char        *p;
    dep_info    **dep;
    dep_info    *new_dep;
    struct stat stat_info;
    char        buff[MAX_FILE];

    p = _fullpath( buff, fi->name, MAX_FILE );
    if( p != NULL ) {
        for( dep = &DependencyInfo; *dep != NULL; dep = &(*dep)->link ) {
            if( strcmp( &(*dep)->fn, p ) == 0 ) {
                return;
            }
        }
        if( fstat( ((a_file *)(fi->fileptr))->handle, &stat_info ) != -1 ) {
            new_dep = FMemAlloc( sizeof( dep_info ) + strlen( p ) );
            new_dep->link = NULL;
            strcpy( &new_dep->fn, p );
            new_dep->time_stamp = stat_info.st_mtime;
            *dep = new_dep;
        }
    }
}


static  void    AddDefaultLib( char *lib_ptr, int lib_len, char priority ) {
//==========================================================================

    default_lib         **lib;
    default_lib         *new_lib;

    if( !( Options & OPT_DFLT_LIB ) )
        return;
    if( ( *lib_ptr == '"' ) && ( lib_ptr[lib_len - 1] == '"' ) ) {
        lib_len -= 2;
        ++lib_ptr;
    }
    for( lib = &DefaultLibs; *lib != NULL; lib = &(*lib)->link ) {
        if( strlen( &(*lib)->lib[1] ) != lib_len )
            continue;
        if( memcmp( &(*lib)->lib[1], lib_ptr, lib_len ) == 0 ) {
            return;
        }
    }
    new_lib = FMemAlloc( sizeof( default_lib ) + lib_len );
    new_lib->link = NULL;
    new_lib->lib[0] = priority;
    memcpy( &new_lib->lib[1], lib_ptr, lib_len );
    new_lib->lib[1+lib_len] = NULLCHAR;
    *lib = new_lib;
}


void    DefaultLibInfo( void ) {
//========================

#if _CPU == 386
    if( CGOpts & CGOPT_STK_ARGS ) {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLib( _flibs, _FLIBS, '1' );
            AddDefaultLib( _maths, _MATHS, '1' );
        } else {
            AddDefaultLib( _flib7s, _FLIB7S, '1' );
            AddDefaultLib( _math7s, _MATH7S, '1' );
        }
        AddDefaultLib( _clibs, _CLIBS, '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLib( _wresfs, _WRESFS, '1' );
        }
    } else {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLib( _flib, _FLIB, '1' );
            AddDefaultLib( _math, _MATH, '1' );
        } else {
            AddDefaultLib( _flib7, _FLIB7, '1' );
            AddDefaultLib( _math7, _MATH7, '1' );
        }
        AddDefaultLib( _clib, _CLIB, '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLib( _wresf, _WRESF, '1' );
        }
    }
    if( CPUOpts & CPUOPT_FPI ) {
        AddDefaultLib( _emu387, _EMU387, '1' );
    } else if( CPUOpts & CPUOPT_FPI87 ) {
        AddDefaultLib( _noemu387, _NOEMU387, '1' );
    }
#elif _CPU == 8086
    if( CGOpts & CGOPT_M_MEDIUM ) {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLib( _flibm, _FLIBM, '1' );
            AddDefaultLib( _mathm, _MATHM, '1' );
        } else {
            AddDefaultLib( _flib7m, _FLIB7M, '1' );
            AddDefaultLib( _math7m, _MATH7M, '1' );
        }
        AddDefaultLib( _clibm, _CLIBM, '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLib( _wresm, _WRESM, '1' );
        }
    } else {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLib( _flibl, _FLIBL, '1' );
            AddDefaultLib( _mathl, _MATHL, '1' );
        } else {
            AddDefaultLib( _flib7l, _FLIB7L, '1' );
            AddDefaultLib( _math7l, _MATH7L, '1' );
        }
        AddDefaultLib( _clibl, _CLIBL, '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLib( _wresl, _WRESL, '1' );
        }
    }
    if( CPUOpts & CPUOPT_FPI ) {
        AddDefaultLib( _emu87, _EMU87, '1' );
    } else if( CPUOpts & CPUOPT_FPI87 ) {
        AddDefaultLib( _noemu87, _NOEMU87, '1' );
    }
#elif _CPU == _AXP
    AddDefaultLib( _flib, _FLIB, '1' );
    AddDefaultLib( _math, _MATH, '1' );
    AddDefaultLib( _clib, _CLIB, '1' );
    if( Options & OPT_RESOURCES ) {
        AddDefaultLib( _wresaxp, _WRESAXP, '1' );
    }
#elif _CPU == _PPC
    AddDefaultLib( _flib, _FLIB, '1' );
    AddDefaultLib( _math, _MATH, '1' );
    AddDefaultLib( _clib, _CLIB, '1' );
    if( Options & OPT_RESOURCES ) {
        AddDefaultLib( _wresppc, _WRESPPC, '1' );
    }
#else
  #error Unknown Platform
#endif
}


static  void    FreeAuxEntry( aux_info *aux ) {
//=============================================

    FreeAuxElements( aux );
    FMemFree( aux );
}


static  void    FreeAuxElements( aux_info *aux ) {
//================================================

    FreeArgList( aux );
    if( aux->parms != DefaultInfo.parms ) {
        FMemFree( aux->parms );
        aux->parms = DefaultInfo.parms;
    }
    if( aux->code != DefaultInfo.code ) {
        FMemFree( aux->code );
        aux->code = DefaultInfo.code;
    }
    if( aux->objname != DefaultInfo.objname ) {
        FMemFree( aux->objname );
        aux->objname = DefaultInfo.objname;
    }
}


static  void    FreeArgList( aux_info *aux ) {
//============================================

    FreeChain( &aux->arg_info );
}


aux_info        *NewAuxEntry( char *name, int name_len ) {
//========================================================

    aux_info    *aux;

    aux = FMemAlloc( sizeof( aux_info ) + name_len );
    aux->sym_len = name_len;
    memcpy( aux->sym_name, name, name_len );
    aux->sym_name[ name_len ] = NULLCHAR;
    aux->link = AuxInfo;
    aux->parms = DefaultInfo.parms;
    aux->code = DefaultInfo.code;
    aux->objname = DefaultInfo.objname;
    aux->arg_info = NULL;
    AuxInfo = aux;
    return( aux );
}


static  bool    CurrToken( char *tok ) {
//======================================

    char    *ptr;

    ptr = TokStart;
    for(;;) {
        if( ptr == TokEnd )
            break;
        if( toupper( *ptr ) != *tok )
            break;
        ptr++;
        tok++;
    }
    if( ( ptr == TokEnd ) && ( *tok == '\0' ) )
        return( TRUE );
    return( FALSE );
}


static  bool    RecToken( char *tok ) {
//=====================================

    if( CurrToken( tok ) ) {
        ScanToken();
        return( TRUE );
    }
    return( FALSE );
}

static  bool    RecFnToken( char *tok ) {
//=======================================

    if( CurrToken( tok ) ) {
        ScanFnToken();
        return( TRUE );
    }
    return( FALSE );
}



void            Pragma( void ) {
//========================

// Process a pragma.

#if ( _CPU == 8086 || _CPU == 386 )
    char        *arr;
    uint        arr_len;
#endif

    struct {
        unsigned f_far    : 1;
        unsigned f_far16  : 1;
        unsigned f_loadds : 1;
        unsigned f_export : 1;
        unsigned f_parm   : 1;
        unsigned f_value  : 1;
        unsigned f_modify : 1;
    } have;

    if( RecFnToken( "LIBRARY" ) ) {
        if( RecFnToken( "\0" ) ) {
            DefaultLibInfo();
        } else {
            while( !RecFnToken( "\0" ) ) {
                AddDefaultLib( TokStart, TokEnd - TokStart, '9' );
                ScanFnToken();
            }
        }
#if ( _CPU == 8086 || _CPU == 386 )
    } else if( RecToken( "ARRAY" ) ) {
        SymbolId();
        TokUpper();
        arr = TokStart;
        arr_len = TokEnd - TokStart;
        ScanToken();
#if _CPU == 386
        if( RecToken( "FAR" ) ) {
            if( _SmallDataModel( CGOpts ) ) {
                AddArrayInfo( arr, arr_len );
            }
        }
#elif _CPU == 8086
        if( RecToken( "FAR" ) ) {
            if( _SmallDataModel( CGOpts ) ) {
                AddArrayInfo( arr, arr_len );
            }
        } else if( RecToken( "HUGE" ) ) {
            if( CGOpts & CGOPT_M_LARGE ) {
                AddArrayInfo( arr, arr_len );
            }
        }
#endif
#endif
    } else {
        AliasInfo = &FortranInfo;
        if( RecToken( "LINKAGE" ) ) {
            ReqToken( "(" );
            SymbolName();
            ReqToken( "," );
            AliasName();
            ProcessAlias();
            ReqToken( ")" );
        } else {
            ReqToken( "AUX" );
            if( RecToken( "(" ) ) {
                AliasName();
                ReqToken( ")" );
            }
            SymbolName();
            ProcessAlias();
            ObjectName();

            have.f_far    = 0;
            have.f_loadds = 0;
            have.f_export = 0;
            have.f_value  = 0;
            have.f_modify = 0;
            have.f_parm   = 0;
            for( ;; ) {
                if( !have.f_parm && RecToken( "PARM" ) ) {
                    GetParmInfo();
                    have.f_parm = 1;
                } else if( !have.f_far && RecToken( "=" ) ) {
                    GetByteSeq();
#if ( _CPU == 8086 || _CPU == 386 )
                    have.f_far = 1;
                } else if( !have.f_far && RecToken( "FAR" ) ) {
                    CurrAux->cclass |= FAR;
                    have.f_far = 1;
#if _CPU == 386
                } else if( !have.f_far16 && RecToken( "FAR16" ) ) {
                    CurrAux->cclass |= FAR16_CALL;
                    have.f_far16 = 1;
#endif
                } else if( !have.f_far && RecToken( "NEAR" ) ) {
                    CurrAux->cclass &= ~FAR;
                    have.f_far = 1;
                } else if( !have.f_loadds && RecToken( "LOADDS" ) ) {
                    CurrAux->cclass |= LOAD_DS_ON_ENTRY;
                    have.f_loadds = 1;
#endif
                } else if( !have.f_export && RecToken( "EXPORT" ) ) {
                    CurrAux->cclass |= DLL_EXPORT;
                    have.f_export = 1;
#if ( _CPU == 8086 || _CPU == 386 )
                } else if( !have.f_value && RecToken( "VALUE" ) ) {
                    GetRetInfo();
                    have.f_value = 1;
#endif
                } else if( !have.f_value && RecToken( "ABORTS" ) ) {
                    CurrAux->cclass |= SUICIDAL;
                    have.f_value = 1;
#if ( _CPU == 8086 || _CPU == 386 )
                } else if( !have.f_modify && RecToken( "MODIFY" ) ) {
                    GetSaveInfo();
                    have.f_modify = 1;
#endif
                } else {
                    break;
                }
            }
        }
    }
}


void    DoPragma( char *ptr ) {
//=============================

    int         status;

    TokStart = ptr;
    TokEnd = ptr;
    ScanToken();
    for(;;) {
        status = CSpawn( &Pragma );
        if( status != 0 ) {
            if( ProgSw & PS_FATAL_ERROR ) {
                CSuicide();
            }
            AsmSymFini();
            break;
        }
        if( RecToken( "\0" ) ) {
            break;
        }
    }
}


void    ProcPragma( char *ptr ) {
//===============================

    // don't process auxiliary pragma's until pass 2
    if( ProgSw & PS_DONT_GENERATE )
        return;
    DoPragma( ptr );
}


static  void    ScanToken( void ) {
//===========================

    char    *ptr;
    int     found_token;
    int     first;

    ptr = TokEnd;
    ptr = SkipBlanks( ptr );
    TokStart = ptr;
    first = TRUE;
    found_token = FALSE;
    for(;;) {
        switch( *ptr ) {
        case ' ' :
        case '\t' :
        case '\0' :
            found_token = TRUE;
            break;
        case '"' :
            if( first ) {
                for(;;) {
                    ++ptr;
                    if( *ptr == '\0' )
                        break;
                    if( *ptr == '"' ) {
                        ++ptr;
                        break;
                    }
                }
            }
            found_token = TRUE;
            break;
        case '[' :
        case ']' :
        case '(' :
        case ')' :
        case ',' :
        case '=' :
        case '*' :
            if( first ) {
                ++ptr;
            }
            found_token = TRUE;
            break;
        case '\\' :
            if( first ) {
                ReadSrc();
                ptr = SrcBuff;
                if( ( *ptr == '*' ) || ( *ptr == 'C' ) || ( *ptr == 'c' ) ) {
                    ++ptr;
                } else {
                    Error( PR_BAD_CONTINUATION );
                    CSuicide();
                }
                ptr = SkipBlanks( ptr );
                TokStart = ptr;
            } else {
                found_token = TRUE;
            }
            break;
        default :
            first = FALSE;
            ptr++;
        }
        if( found_token ) {
            break;
        }
    }
    TokEnd = ptr;
}

static  void    ScanFnToken( void ) {
//===========================

    char    *ptr;
    int     found_token;
    int     first;

    ptr = TokEnd;
    ptr = SkipBlanks( ptr );
    TokStart = ptr;
    first = TRUE;
    for( found_token = FALSE; found_token == FALSE; ) {
        switch( *ptr ) {
        case ' ' :
        case '\t' :
        case '\0' :
            found_token = TRUE;
            break;
        case '"' :
            if( first ) {
                for(;;) {
                    ++ptr;
                    if( *ptr == '\0' )
                        break;
                    if( *ptr == '"' ) {
                        ++ptr;
                        break;
                    }
                }
            }
            found_token = TRUE;
            break;
        default :
            first = FALSE;
            ptr++;
        }
    }
    TokEnd = ptr;
}

#if (( _CPU == 8086 || _CPU == 386))
static  void    TokUpper( void ) {
//==========================

    char        *ptr;

    ptr = TokStart;
    while( ptr != TokEnd ) {
        *ptr = toupper( *ptr );
        ++ptr;
    }
}

#endif


static  void    ReqToken( char *tok ) {
//=====================================

    if( !RecToken( tok ) ) {
        *TokEnd = NULLCHAR;
        Error( PR_BAD_SYNTAX, tok, TokStart );
        CSuicide();
    }
}


static  void            AliasName( void ) {
//===================================

    aux_info    *alias;

    SymbolId();
    alias = AuxLookupName( TokStart, TokEnd - TokStart );
    if( alias != NULL ) {
        AliasInfo = alias;
    }
    ScanToken();
}


static  void            SymbolName( void ) {
//====================================

    if( RecToken( "DEFAULT" ) ) {
        SymLen = 0;
    } else {
        SymbolId();
        SymLen = TokEnd - TokStart;
        if( SymLen > MAX_SYMLEN ) {
            SymLen = MAX_SYMLEN;
        }
        memcpy( SymName, TokStart, SymLen );
        ScanToken();
    }
}


static  void            ProcessAlias( void ) {
//======================================

    if( SymLen == 0 ) { // "DEFAULT"
        CurrAux = AliasInfo;
    } else {
        CurrAux = AuxLookupName( SymName, SymLen );
        if( CurrAux != AliasInfo ) { // Consider: c$pragma aux (sp) sp
            if( CurrAux == NULL ) {
                CurrAux = NewAuxEntry( SymName, SymLen );
            } else {
                FreeAuxElements( CurrAux );
            }
        }
        CopyAuxInfo( CurrAux, AliasInfo );
    }
}


void            CopyAuxInfo( aux_info *dst, aux_info *src ) {
//===========================================================

    if( dst != src ) {
        dst->cclass = src->cclass;
        dst->save = src->save;
        dst->returns = src->returns;
        dst->streturn = src->streturn;
        if( src->parms != DefaultInfo.parms ) {
            DupParmInfo( dst, src );
        }
        if( src->code != DefaultInfo.code ) {
            DupCallBytes( dst, src );
        }
        if( src->objname != DefaultInfo.objname ) {
            DupObjectName( dst, src );
        }
        DupArgInfo( dst, src );
    }
}


static  void    DupCallBytes( aux_info *dst, aux_info *src ) {
//============================================================
#if _CPU == 8086 || _CPU == 386

    byte_seq    *new_seq;
    uint        seq_len;
    seq_len = src->code->length & ~DO_FLOATING_FIXUPS;
    new_seq = FMemAlloc( sizeof( byte_seq ) + seq_len );
    memcpy( new_seq->data, src->code->data, seq_len );
    dst->code = new_seq;
    dst->code->length = src->code->length;

#elif _CPU == _AXP || _CPU == _PPC

    risc_byte_seq       *new_seq;
    uint                seq_len;
    byte_seq_reloc      **lnk;
    byte_seq_reloc      *new;
    byte_seq_reloc      *head;
    byte_seq_reloc      *reloc;

    seq_len = src->code->length;
    new_seq = FMemAlloc( sizeof( byte_seq ) + seq_len );
    memcpy( new_seq->data, src->code->data, seq_len );
    dst->code = new_seq;
    dst->code->length = src->code->length;

    head = NULL;
    lnk = &head;
    for( reloc = src->code->relocs; reloc; reloc = reloc->next ) {
        new = FMemAlloc( sizeof( byte_seq_reloc ) );
        new->off = reloc->off;
        new->type = reloc->type;
        new->sym = reloc->sym;
        new->next = NULL;
        *lnk = new;
        lnk = &new->next;
    }
    dst->code->relocs = head;

#else
  #error Unknown Target
#endif
}


static  void    DupParmInfo( aux_info *dst, aux_info *src ) {
//===========================================================

    hw_reg_set  *new_reg_set;
    hw_reg_set  *reg_set;
    int         size;

    reg_set = src->parms;
    size = 0;
    while( !HW_CEqual( reg_set[ size ], HW_EMPTY ) ) {
        ++size;
    }
    ++size;
    new_reg_set = FMemAlloc( size * sizeof( hw_reg_set ) );
    memcpy( new_reg_set, reg_set, size * sizeof( hw_reg_set ) );
    dst->parms = new_reg_set;
}


static  void    DupObjectName( aux_info *dst, aux_info *src ) {
//=============================================================

    char        *new_name;

    new_name = FMemAlloc( strlen( src->objname ) + sizeof( char ) );
    strcpy( new_name, src->objname );
    dst->objname = new_name;
}


static  void    DupArgInfo( aux_info *dst, aux_info *src ) {
//==========================================================

    pass_by     *new_arg;
    pass_by     *args;
    pass_by     **curr_arg;

    args = src->arg_info;
    curr_arg = &dst->arg_info;
    while( args != NULL ) {
        new_arg = FMemAlloc( sizeof( pass_by ) );
        new_arg->info = args->info;
        new_arg->link = NULL;
        *curr_arg = new_arg;
        curr_arg = &new_arg->link;
        args = args->link;
    }
}


static  void            SymbolId( void ) {
//==================================

    char        *ptr;

    ptr = TokStart;
    if( ( isalpha( *ptr ) == 0 ) && ( *ptr != '$' ) && ( *ptr != '_' ) ) {
        Error( PR_SYMBOL_NAME );
        CSuicide();
    }
    for(;;) {
        ptr++;
        if( ptr == TokEnd )
            break;
        if( ( isalnum( *ptr ) == 0 ) && ( *ptr != '$' ) && ( *ptr != '_' ) ) {
            Error( PR_SYMBOL_NAME );
            CSuicide();
        }
    }
}


static  void            ObjectName( void ) {
//====================================

    int         obj_len;
    char        *name;

    if( *TokStart != '"' )
        return;
    if( TokStart == TokEnd - sizeof( char ) )
        CSuicide();
    if( *(TokEnd - sizeof( char )) != '"' )
        CSuicide();
    obj_len = TokEnd - TokStart - 2*sizeof( char );
    name = FMemAlloc( obj_len + sizeof( char ) );
    if( CurrAux->objname != DefaultInfo.objname ) {
        FMemFree( CurrAux->objname );
    }
    memcpy( name, TokStart + sizeof( char ), obj_len );
    name[ obj_len ] = NULLCHAR;
    CurrAux->objname = name;
    ScanToken();
}


enum    sym_state       AsmQueryExternal( char *name ) {
//======================================================

    name = name;
    return( SYM_UNDEFINED );
}


#if ( _CPU == 8086 || _CPU == 386 )
enum    sym_type        AsmQueryType( char *name ) {
//==================================================

    name = name;
    return( SYM_INT1 );
}


static  void    InsertFixups( unsigned char *buff, unsigned i ) {
//===============================================================
                        // additional slop in buffer to simplify the code
    unsigned char       temp[MAXIMUM_BYTESEQ+2*sizeof(byte)];
    struct asmfixup     *fix;
    struct asmfixup     *head;
    struct asmfixup     *chk;
    struct asmfixup     *next;
    struct asmfixup     **owner;
    byte                *dst;
    byte                *src;
    byte                *end;
    byte_seq            *seq;
    byte_seq_len        perform_fixups;
    char                *name;

    perform_fixups = 0;
    head = FixupHead;
    if( head != NULL ) {
        FixupHead = NULL;
        // sort the fixup list in increasing fixup_loc's
        for( fix = head; fix != NULL; fix = next ) {
            owner = &FixupHead;
            for( ;; ) {
                chk = *owner;
                if( chk == NULL )
                    break;
                if( chk->fixup_loc > fix->fixup_loc )
                    break;
                owner = &chk->next;
            }
            next = fix->next;
            fix->next = *owner;
            *owner = fix;
        }
        dst = temp;
        src = buff;
        end = src + i;
        fix = FixupHead;
        owner = &FixupHead;
        // insert fixup escape sequences
        while( src < end ) {
            if( (fix != NULL) && (fix->fixup_loc == (src - buff)) ) {
                name = fix->name;
                // insert fixup information
                *dst++ = FLOATING_FIXUP_BYTE;
                // only expect a 'fixup_type' of FIX_SEG
                switch( fix->fixup_type ) {
                case FIX_SEG:
                    if( name == NULL ) {
                        // special case for floating point fixup
                        if( *src != 0x9b ) { /* FWAIT */
                            *dst++ = 0x9b;
                        }
                        break;
                    }
                default:
                    Error( PR_SYMBOL_UNRESOLVED, name );
                    return;
                }
                head = fix;
                fix = fix->next;
                if( head->external ) {
                    *owner = fix;
                    if( head->name != NULL ) {
                        FMemFree( head->name );
                    }
                    FMemFree( head );
                } else {
                    owner = &head->next;
                }
            } else {
                if( *src == FLOATING_FIXUP_BYTE ) {
                    *dst++ = FLOATING_FIXUP_BYTE;
                }
                *dst++ = *src++;
            }
            if( dst > &temp[ MAXIMUM_BYTESEQ ] ) {
                Error( PR_BYTE_SEQ_LIMIT );
                CSuicide();
            }
        }
        buff = temp;
        i = dst - temp;
        perform_fixups = DO_FLOATING_FIXUPS;
    }
    seq = FMemAlloc( sizeof( byte_seq ) + i );
    seq->length = i | perform_fixups;
    memcpy( &seq->data, buff, i );
    if( CurrAux->code != DefaultInfo.code ) {
        FMemFree( CurrAux->code );
    }
    CurrAux->code = seq;
}

#elif _CPU == _AXP || _CPU == _PPC

uint_32 AsmQuerySPOffsetOf( char *name ) {
//========================================

    return( 0 );
}


static  void    InsertFixups( unsigned char *buff, unsigned i ) {
//===============================================================

    risc_byte_seq       *seq;
    asmreloc            *reloc;
    byte_seq_reloc      *head;
    byte_seq_reloc      *new;
    byte_seq_reloc      **lnk;

    head = NULL;
    lnk = &head;
    for( reloc = AsmRelocs; reloc; reloc = reloc->next ) {
        new = FMemAlloc( sizeof( byte_seq_reloc ) );
        new->off = reloc->offset;
        new->type = reloc->type;
        new->sym = (void *)SymFind( reloc->name, strlen( reloc->name ) );
        new->next = NULL;
        *lnk = new;
        lnk = &new->next;
    }

    seq = FMemAlloc( sizeof( byte_seq ) + i );
    seq->relocs = head;
    seq->length = i;
    memcpy( &seq->data, buff, i );
    if( CurrAux->code != DefaultInfo.code ) {
        FMemFree( CurrAux->code );
    }
    CurrAux->code = seq;
}
#endif

#if _CPU == 8086

static  void    AddAFix( unsigned i, char *name, unsigned type,
                         unsigned long off ) {
//============================================

    struct asmfixup     *fix;

    fix = FMemAlloc( sizeof( *fix ) );
    fix->external = 1;
    fix->fixup_loc = i;
    fix->name = name;
    fix->offset = off;
    fix->fixup_type = type;
    fix->next = FixupHead;
    FixupHead = fix;
}

#endif


static  void    GetByteSeq( void ) {
//==================================

    int             seq_len;
    int             len;
    char            *ptr;
    byte            buff[MAXIMUM_BYTESEQ+32]; // extra for assembler
#if _CPU == 8086
    bool            float_specified;

    float_specified = FALSE;
#endif
    seq_len = 0;
#if ( _CPU == 8086 || _CPU == 386 )
    AsmSaveCPUInfo();
#endif
    for(;;) {
        if( *TokStart == '"' ) {
            if( TokStart == TokEnd - sizeof( char ) )
                CSuicide();
            if( *(TokEnd - sizeof( char )) != '"' )
                CSuicide();
            *(char *)(TokEnd - sizeof( char )) = NULLCHAR;
            AsmCodeAddress = seq_len;
            AsmCodeBuffer = buff;
            AsmLine( &TokStart[1] );
            if( AsmCodeAddress <= MAXIMUM_BYTESEQ ) {
                seq_len = AsmCodeAddress;
            } else {
                Error( PR_BYTE_SEQ_LIMIT );
                CSuicide();
            }
            ScanToken();
        } else if( RecToken( "FLOAT" ) ) {
#if _CPU == 8086
            AddAFix( seq_len, NULL, FIX_SEG, 0 );
#endif
        } else {
            ptr = TokStart;
            if( ( *ptr != 'Z' ) && ( *ptr != 'z' ) )
                break;
            ++ptr;
            len = MkHexConst( ptr, ptr, TokEnd - TokStart - 1 );
            if( len == 0 ) {
                Error( PR_BAD_BYTE_SEQ );
                CSuicide();
            }
            if( seq_len + len <= MAXIMUM_BYTESEQ ) {
                memcpy( buff + seq_len, ptr, len );
                seq_len += len;
            } else {
                Error( PR_BYTE_SEQ_LIMIT );
                CSuicide();
            }
            ScanToken();
        }
    }
    InsertFixups( buff, seq_len );
    AsmSymFini();
#if ( _CPU == 8086 || _CPU == 386 )
    AsmRestoreCPUInfo();
#endif
}


#if ( _CPU == 8086 || _CPU == 386 )
static  hw_reg_set      RegSet( void ) {
//================================

    hw_reg_set  reg_set;
    int         reg;

    HW_CAsgn( reg_set, HW_EMPTY );
    for(;;) {
        TokUpper();
        reg = KwLookUp( RegNames, MaxReg, TokStart, TokEnd-TokStart, TRUE );
        if( reg == 0 )
            break;
        HW_TurnOn( reg_set, RegValue[ reg ] );
        ScanToken();
    }
    ReqToken( "]" );
    return( reg_set );
}


static  hw_reg_set      *RegSets( void ) {
//==================================

    hw_reg_set  reg_sets[MAX_REG_SETS];
    hw_reg_set  *regs;
    int         num_sets;

    num_sets = 0;
    while( RecToken( "[" ) ) {
        reg_sets[ num_sets ] = RegSet();
        if( ( num_sets < MAX_REG_SETS ) &&
            !HW_CEqual( reg_sets[num_sets], HW_EMPTY ) ) {
            ++num_sets;
        }
    }
    HW_CAsgn( reg_sets[ num_sets ], HW_EMPTY );
    regs = FMemAlloc( ( num_sets + 1 ) * sizeof( hw_reg_set ) );
    memcpy( regs, reg_sets, ( num_sets + 1 ) * sizeof( hw_reg_set ) );
    return( regs );
}
#endif


static  void            GetParmInfo( void ) {
//=====================================

// Collect argument information.

    struct {
#if ( _CPU == 8086 || _CPU == 386 )
        unsigned f_pop           : 1;
        unsigned f_reverse       : 1;
        unsigned f_loadds        : 1;
        unsigned f_nomemory      : 1;
        unsigned f_list          : 1;
#endif
        unsigned f_args          : 1;
    } have;

#if ( _CPU == 8086 || _CPU == 386 )
    have.f_pop           = 0;
    have.f_reverse       = 0;
    have.f_loadds        = 0;
    have.f_nomemory      = 0;
    have.f_list          = 0;
#endif
    have.f_args          = 0;
    for(;;) {
        if( !have.f_args && RecToken( "(" ) ) {
            GetArgList();
            have.f_args = 1;
#if ( _CPU == 8086 || _CPU == 386 )
        } else if( !have.f_pop && RecToken( "CALLER" ) ) {
            CurrAux->cclass |= CALLER_POPS;
            have.f_pop = 1;
        } else if( !have.f_pop && RecToken( "ROUTINE" ) ) {
            CurrAux->cclass &= ~CALLER_POPS;
            have.f_pop = 1;
        } else if( !have.f_reverse && RecToken( "REVERSE" ) ) {
            // arguments are processed in reverse order by default
            CurrAux->cclass |= REVERSE_PARMS;
            have.f_reverse = 1;
        } else if( !have.f_nomemory && RecToken( "NOMEMORY" ) ) {
            CurrAux->cclass |= NO_MEMORY_READ;
            have.f_nomemory = 1;
        } else if( !have.f_loadds && RecToken( "LOADDS" ) ) {
            CurrAux->cclass |= LOAD_DS_ON_CALL;
            have.f_loadds = 1;
        } else if( !have.f_list && CurrToken( "[" ) ) {
            if( CurrAux->parms != DefaultInfo.parms ) {
                FMemFree( CurrAux->parms );
            }
            CurrAux->parms = RegSets();
            have.f_list = 1;
#endif
        } else {
            break;
        }
    }
}


static  void    GetArgList( void ) {
//============================

    pass_by     *arg;
    pass_by     **curr_arg;
    unsigned_16 pass_info;

    FreeArgList( CurrAux );
    if( RecToken( ")" ) )
        return;
    curr_arg = &CurrAux->arg_info;
    for(;;) {
        pass_info = 0;
        if( RecToken( "VALUE" ) ) {
            pass_info |= PASS_BY_VALUE;
            if( RecToken( "*" ) ) {
                if( RecToken( "1" ) ) {
                    pass_info |= ARG_SIZE_1;
                } else if( RecToken( "2" ) ) {
                    pass_info |= ARG_SIZE_2;
                } else if( RecToken( "4" ) ) {
                    pass_info |= ARG_SIZE_4;
                } else if( RecToken( "8" ) ) {
                    pass_info |= ARG_SIZE_8;
                } else {
                    Error( PR_BAD_PARM_SIZE );
                    CSuicide();
                }
#if ( _CPU == 8086 || _CPU == 386 )
            } else if( RecToken( "FAR" ) ) {
                pass_info |= ARG_FAR;
    #if ( _CPU == 8086 )
                pass_info |= ARG_SIZE_2;
    #elif ( _CPU == 386 )
                pass_info |= ARG_SIZE_4;
    #endif
            } else if( RecToken( "NEAR" ) ) {
                pass_info |= ARG_NEAR;
    #if ( _CPU == 8086 )
                pass_info |= ARG_SIZE_2;
    #elif ( _CPU == 386 )
                pass_info |= ARG_SIZE_4;
    #endif
            } else {
    #if ( _CPU == 8086 )
                pass_info |= ARG_SIZE_2;
    #elif ( _CPU == 386 )
                pass_info |= ARG_SIZE_4;
    #endif
#endif
            }
        } else if( RecToken( "REFERENCE" ) ) {
            pass_info |= PASS_BY_REFERENCE;
#if ( _CPU == 8086 || _CPU == 386 )
            if( RecToken( "FAR" ) ) {
                pass_info |= ARG_FAR;
            } else if( RecToken( "NEAR" ) ) {
                pass_info |= ARG_NEAR;
            }
#endif
            if( RecToken( "DESCRIPTOR" ) ) {
                pass_info |= PASS_BY_DESCRIPTOR;
            } else if( RecToken( "NODESCRIPTOR" ) ) {
                pass_info |= PASS_BY_NODESCRIPTOR;
            }
        } else if( RecToken( "DATA_REFERENCE" ) ) {
            pass_info |= PASS_BY_DATA | PASS_BY_REFERENCE;
#if ( _CPU == 8086 || _CPU == 386 )
            if( RecToken( "FAR" ) ) {
                pass_info |= ARG_FAR;
            } else if( RecToken( "NEAR" ) ) {
                pass_info |= ARG_NEAR;
            }
#endif
        } else {
            Error( PR_BAD_PARM_ATTR );
            CSuicide();
        }
        arg = FMemAlloc( sizeof( pass_by ) );
        arg->link = NULL;
        arg->info = pass_info;
        *curr_arg = arg;
        curr_arg = &arg->link;
        if( !RecToken( "," ) ) {
            break;
        }
    }
    ReqToken( ")" );
}


#if ( _CPU == 8086 || _CPU == 386 )
static  void            GetRetInfo( void ) {
//====================================

    struct {
        unsigned f_no8087        : 1;
        unsigned f_list          : 1;
        unsigned f_struct        : 1;
    } have;

    have.f_no8087  = 0;
    have.f_list    = 0;
    have.f_struct  = 0;
    // "3s" default is NO_8087_RETURNS - turn off NO_8087_RETURNS
    // flag so that "3s" model programs can use 387 pragmas
    CurrAux->cclass &= ~NO_8087_RETURNS;
    for(;;) {
        if( !have.f_no8087 && RecToken( "NO8087" ) ) {
            CurrAux->cclass |= NO_8087_RETURNS;
            HW_CTurnOff( CurrAux->returns, HW_FLTS );
            have.f_no8087 = 1;
        } else if( !have.f_list && RecToken( "[" ) ) {
            CurrAux->cclass |= SPECIAL_RETURN;
            CurrAux->returns = RegSet();
            have.f_list = 1;
        } else if( !have.f_struct && RecToken( "STRUCT" ) ) {
            GetSTRetInfo();
            have.f_struct = 1;
        } else {
            break;
        }
    }
}


static  void    GetSTRetInfo( void ) {
//==============================

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
    for(;;) {
        if( !have.f_float && RecToken( "FLOAT" ) ) {
            CurrAux->cclass |= NO_FLOAT_REG_RETURNS;
            have.f_float = 1;
        } else if( !have.f_struct && RecToken( "STRUCT" ) ) {
            CurrAux->cclass |= NO_STRUCT_REG_RETURNS;
            have.f_struct = 1;
        } else if( !have.f_allocs && RecToken( "ROUTINE" ) ) {
            CurrAux->cclass |= ROUTINE_RETURN;
            have.f_allocs = 1;
        } else if( !have.f_allocs && RecToken( "CALLER" ) ) {
            CurrAux->cclass &= ~ROUTINE_RETURN;
            have.f_allocs = 1;
        } else if( !have.f_list && RecToken( "[" ) ) {
            CurrAux->cclass |= SPECIAL_STRUCT_RETURN;
            CurrAux->streturn = RegSet();
            have.f_list = 1;
        } else {
            break;
        }
    }
}


static  void            GetSaveInfo( void ) {
//=====================================

    hw_reg_set  modlist;
    hw_reg_set  default_flt_n_seg;
    hw_reg_set  flt_n_seg;

    struct {
        unsigned f_exact        : 1;
        unsigned f_nomemory     : 1;
        unsigned f_list         : 1;
    } have;

    have.f_exact    = 0;
    have.f_nomemory = 0;
    have.f_list     = 0;
    for(;;) {
        if( !have.f_exact && RecToken( "EXACT" ) ) {
            CurrAux->cclass |= MODIFY_EXACT;
            have.f_exact = 1;
        } else if( !have.f_nomemory && RecToken( "NOMEMORY" ) ) {
            CurrAux->cclass |= NO_MEMORY_CHANGED;
            have.f_nomemory = 1;
        } else if( !have.f_list && RecToken( "[" ) ) {
            modlist = RegSet();
            have.f_list = 1;
        } else {
            break;
        }
    }
    if( have.f_list ) {
        HW_Asgn( default_flt_n_seg, DefaultInfo.save );
        HW_CTurnOn( CurrAux->save, HW_FULL );
        if( !have.f_exact && !(CGOpts & CGOPT_SEG_REGS) ) {
            HW_CAsgn( flt_n_seg, HW_FLTS );
            HW_CTurnOn( flt_n_seg, HW_SEGS );
            HW_TurnOff( CurrAux->save, flt_n_seg );
            HW_OnlyOn( default_flt_n_seg, flt_n_seg );
            HW_TurnOn( CurrAux->save, default_flt_n_seg );
        }
        HW_TurnOff( CurrAux->save, modlist );
    }
}
#endif
