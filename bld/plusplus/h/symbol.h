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


// SYMBOL.H -- operations on symbols
//
// 91/11/06 -- J.W.Welch        -- defined

// Macros

#include "intsupp.h"

#define SymAddrTaken(s)     ( (s)->flag & SF_ADDR_TAKEN )
#define SymDbgAddrTaken(s)  ( (s)->flag & SF_DBG_ADDR_TAKEN )
#define SymIsAnError(s)     ( (s)->flag & SF_ERROR )
#define SymIsConstantInt(s) ( (s)->flag & SF_CONSTANT_INT )
#define SymIsConstantNoCode(s) ( (s)->flag & SF_CONST_NO_CODE )
#define SymIsInitialized(s) ( (s)->flag & SF_INITIALIZED )
#define SymIsReferenced(s)  ( (s)->flag & SF_REFERENCED )
#define SymInVft(s)         ( (s)->flag & SF_IN_VFT )
#define SymIsAnonymousMember(s) ( (s)->flag & SF_ANONYMOUS )
#define SymIsUninlineable(s)    ( (s)->flag & SF_DONT_INLINE )
#define SymIsMustGen(s)     ( (s)->flag & SF_MUST_GEN )
#define SymIsFnTemplateMatchable(s) ( (s)->flag & SF_TEMPLATE_FN )

#define SymIsAlias(s)       ( (s)->flag & SF_ALIAS )
#define SymIsCatchAlias(s)  ( (s)->id == SC_AUTO && ( (s)->flag & SF_CATCH_ALIAS ))

#define SymIsTypedef(s)     ( (s)->id == SC_TYPEDEF )
#define SymIsDefArg(s)      ( (s)->id == SC_DEFAULT )
#define SymIsEnumeration(s) ( (s)->id == SC_ENUM )
#define SymIsFunctionTemplateModel(s) ( ( (s)->id == SC_FUNCTION_TEMPLATE ) || ( (s)->id == SC_EXTERN_FUNCTION_TEMPLATE ) || ( (s)->id == SC_STATIC_FUNCTION_TEMPLATE ) )
#define SymIsClassTemplateModel(s) ( (s)->id == SC_CLASS_TEMPLATE )
#define SymIsNameSpace(s)   ( (s)->id == SC_NAMESPACE )

#define SymFuncReturnType(s) ( SymUnmodifiedType(s)->of )
#define SymUnmodifiedType(s) ( TypedefModifierRemoveOnly( (s)->sym_type ) )
#define SymIsEllipsisFunc(s) ( TypeHasEllipsisArg( (s)->sym_type ) )
#define SymIsEnumAnonymousTypedef(s) ( TypeIsAnonymousEnum( (s)->sym_type ) )

#define SymAddressOf(s) (((s)->id == SC_ADDRESS_ALIAS ) ? (s)->u.alias : NULL )

#define SymIsComdatFun(s)       ( SymComdatFunInfo((s)) != SCF_NULL )
#define SymIsGennedComdatFun(s) \
        (( SymComdatFunInfo((s)) & (SCF_STATIC|SCF_COMDAT) ) == SCF_COMDAT )

typedef enum {
    SCF_NULL            = 0x00,
    SCF_COMDAT          = 0x01, // fn is a comdat function
    SCF_STATIC          = 0x02, // user wants comdat fn in the text segment
} scf_mask;

// Protoypes:
#ifndef NDEBUG
SYMBOL SymDeAlias(              // REDUCE TO NON-ALIASED SYMBOL
    SYMBOL sym )                // - the symbol
;
#else
#define SymDeAlias(s)       ( SymIsAlias(s) ? s->u.alias : s )
#endif

TOKEN_LOCN* LocnForClass        // GET DEFINITION LOCATION, IF POSSIBLE
    ( TYPE cltype )             // - a type
;
SYMBOL SymAllocReturn(          // ALLOCATE A RETURN SYMBOL
    SCOPE scope,                // - scope for symbol
    TYPE type )                 // - symbol's type
;
SYMBOL SymBindConstant              // BIND A CONSTANT TO A SYMBOL
    ( SYMBOL sym                    // - the symbol
    , signed_64 con )               // - the constant
;
boolean SymCDtorExtraParm(      // does the CTOR/DTOR need the extra int arg?
    SYMBOL sym )                // - the symbol
;
TYPE SymClass(                  // GET TYPE FOR CLASS CONTAINING A SYMBOL
    SYMBOL sym )                // - the symbol
;
boolean SymClassCorrupted(      // TEST IF SYMBOL'S CLASS CORRUPTED
    SYMBOL sym )                // - the symbol
;
CLASSINFO *SymClassInfo(        // GET CLASSINFO FOR SYMBOL
    SYMBOL sym )                // - the symbol
;
SYMBOL SymConstantValue         // GET CONSTANT VALUE FOR SYMBOL
    ( SYMBOL sym                // - the symbol
    , INT_CONSTANT* pval )      // - addr[ value ]
;
SYMBOL SymCreate(               // CREATE NEW SYMBOL
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name,                 // - symbol name
    SCOPE scope )               // - scope for insertion
;
SYMBOL SymCreateAtLocn(         // CREATE NEW SYMBOL AT LOCATION
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name,                 // - symbol name
    SCOPE scope,                // - scope for insertion
    TOKEN_LOCN* locn )          // - location
;
SYMBOL SymCreateCurrScope(      // CREATE NEW CURR-SCOPE SYMBOL
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name )                // - symbol name
;
SYMBOL SymCreateFileScope(      // CREATE NEW FILE-SCOPE SYMBOL
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name )                // - symbol name
;
SYMBOL SymCreateTempScope(      // CREATE NEW TEMP-SCOPE SYMBOL
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name )                // - symbol name
;
SYMBOL SymDefaultBase(          // REMOVE DEFAULT ARGUMENTS TO GET BASE SYMBOL
    SYMBOL sym )                // - the symbol
;
SYMBOL SymDeriveThrowBits(      // DERIVE SF_.._LONGJUMP BITS FROM SOURCE
    SYMBOL tgt,                 // - target symbol
    SYMBOL src )                // - source symbol
;
SYMBOL SymForClass              // GET SYMBOL FOR CLASS IF IT EXISTS
    ( TYPE cltype )             // - a type
;
arg_list *SymFuncArgList(       // GET FUNCTION ARG. LIST
    SYMBOL func )               // - a function SYMBOL
;
SYMBOL SymFunctionReturn(       // GET SYMBOL FOR RETURN
    void )
;
SYMBOL SymIsAnonymous(          // TEST IF SYMBOL IS AN ANONYMOUS UNION MEMBER
    SYMBOL sym )                // - the symbol
;
#ifdef OPT_BR
boolean SymIsBrowsable          // TEST IF SYMBOL CAN BE BROWSED
    ( SYMBOL sym )              // - the symbol
;
#endif
boolean SymIsArgument(          // TEST IF SYMBOL IS AN ARGUMENT
    SYMBOL sym )                // - symbol
;
boolean SymIsAutomatic(         // TEST IF SYMBOL IS AUTOMATIC VARIABLE
    SYMBOL sym )                // - the symbol
;
boolean SymIsClass(             // DETERMINE IF SYMBOL IS A CLASS OBJECT
    SYMBOL sym )                // - the symbol
;
boolean SymIsClassArray(        // DETERMINE IF SYMBOL IS ARRAY OF CLASS ITEMS
    SYMBOL sym )                // - the symbol
;
boolean SymIsClassDefinition(   // TEST IF SYMBOL IS TYPEDEF FOR A CLASS
    SYMBOL sym )                // - the symbol
;
boolean SymIsClassMember(       // TEST IF SYMBOL IS MEMBER OF CLASS
    SYMBOL sym )                // - the symbol
;
boolean SymIsNameSpaceMember(    // TEST IF SYMBOL IS MEMBER OF NAMESPACE
    SYMBOL sym )                // - the symbol
;
boolean SymIsClassTemplateMember(// TEST IF SYMBOL IS MEMBER OF CLASS TEMPLATE
    SYMBOL sym )                // - the symbol    \\ i.e., a class instantiation
;
boolean SymIsComdatData(        // TEST IF DATA SYMBOL IS IN COMDAT SEGMENT
    SYMBOL sym )                // - data symbol
;
scf_mask SymComdatFunInfo(      // TEST IF FUNCTION IS IN COMDAT SEGMENT
    SYMBOL sym )                // - function symbol
;
boolean SymIsConstant(          // TEST IF SYMBOL IS A CONSTANT
    SYMBOL sym )                // - symbol
;
boolean SymIsThreaded(          // TEST IF SYMBOL IS A __declspec(thread)
    SYMBOL sym )                // - symbol
;
boolean SymIsDllImport(         // TEST IF SYMBOL IS A __declspec(dllimport)
    SYMBOL sym )                // - symbol
;
boolean SymIsConversionToType(  // SYMBOL IS operator T()?
    SYMBOL sym,                 // - symbol to check
    TYPE type )                 // - type T
;
boolean SymIsCtor(              // TEST IF SYMBOL IS A CTOR FUNCTION
    SYMBOL ctor )               // - potential CTOR function
;
boolean SymIsCtorOrDtor(        // TEST IF SYMBOL IS CTOR OR DTOR
    SYMBOL sym )                // - hopefully, one of them
;
boolean SymIsAssign(            // TEST IF SYMBOL IS "operator ="
    SYMBOL op_eq )              // - potential op= function
;
boolean SymIsData(              // TEST IF SYMBOL IS DATA
    SYMBOL sym )                // - the symbol
;
boolean SymIsDtor(              // TEST IF SYMBOL IS A DTOR FUNCTION
    SYMBOL dtor )               // - potential DTOR function
;
boolean SymIsEnumDefinition(    // TEST IF SYMBOL IS AN ENUMERATION
    SYMBOL sym )                // - the symbol
;
boolean SymIsExtern(            // SYMBOL IS DEFINED OUTSIDE THIS MODULE
    SYMBOL sym )                // - the symbol
;
boolean SymIsFsFunction(        // TEST IF SPECIFIC FILE-SCOPE FUNCTION
    SYMBOL func,                // - potential function
    const char* name,           // - specific name
    unsigned length )           // - length of name
;
boolean SymIsFuncMember(        // TEST IF SYMBOL IS A MEMBER FUNCTION
    SYMBOL sym )                // - the symbol
;
boolean SymIsFunction(          // TEST IF SYMBOL IS A FUNCTION
    SYMBOL sym )                // - the function
;
SYMBOL SymIsFunctionTemplateInst(// TEST IF SYMBOL WAS GENERATED FROM A FUNCTION
    SYMBOL sym )                // - symbol                     \\     TEMPLATE
;
boolean SymIsGenedFunc(         // DETERMINE IF SYMBOL IS A GENERATED FUNC.
    SYMBOL func )               // - the symbol
;
boolean SymIsHugeData(          // TEST IF SYMBOL IS HUGE DATA
    SYMBOL sym )                // - the symbol
;
boolean SymIsInjectedTypedef(   // TEST IF SYMBOL IS INJECTED TYPEDEF
    SYMBOL sym )                // - the symbol
;
boolean SymIsInline(            // TEST IF FUNCTION IS INLINE
    SYMBOL fun )                // - function symbol
;
boolean SymIsInMem(             // TEST IF SYMBOL SHOULD STAY IN MEMORY
    SYMBOL sym )                // - symbol
;
boolean SymIsModuleDtorable(    // TEST IF SYMBOL IS MODULE-DTORABLE
    SYMBOL sym )                // - a DTORable symbol
;
boolean SymIsNextInitializableMember(// SYMBOL CAN BE BRACE INITIALIZED
    SYMBOL *prev,               // - previous initializable member
    SYMBOL sym )                // - the member
;
boolean SymIsOpDel(             // TEST IF SYMBOL IS "operator delete"
    SYMBOL op_del )             // - potential operator delete
;
boolean SymIsOpDelar(           // TEST IF SYMBOL IS "operator delete[]"
    SYMBOL op_del )             // - potential operator delete[]
;
boolean SymIsPure(              // TEST IF A PURE VIRTUAL FUNCTION
    SYMBOL sym )                // - a function
;
boolean SymIsRegularStaticFunc( // TEST IF SYMBOL IF NON-MEMBER STATIC FUNC.
    SYMBOL sym )                // - the symbol
;
boolean SymIsStatic(            // DETERMINE IF SYMBOL IS STATIC
    SYMBOL sym )                // - the symbol
;
boolean SymIsStaticData(        // TEST IF SYMBOL IS STATIC DATA ELEMENT
    SYMBOL sym )                // - the symbol
;
boolean SymIsStaticDataMember(  // TEST IF SYMBOL IS STATIC DATA MEMBER
    SYMBOL sym )                // - the symbol
;
boolean SymIsStaticFuncMember(  // TEST IF SYMBOL IS A STATIC MEMBER FUNC.
    SYMBOL sym )                // - the symbol
;
boolean SymIsStaticMember(      // TEST IF SYMBOL IS STATIC MEMBER
    SYMBOL sym )                // - the symbol
;
boolean SymIsTemporary(         // DETERMINE IF INTERNAL SYMBOL
    SYMBOL sym )                // - the symbol
;
boolean SymIsThisDataMember(    // TEST IF SYMBOL IS THIS DATA MEMBER
    SYMBOL sym )                // - the symbol
;
boolean SymIsThisFuncMember(    // TEST IF SYMBOL IS A THIS MEMBER FUNC.
    SYMBOL sym )                // - the symbol
;
boolean SymIsThisMember(        // TEST IF SYMBOL IS DATA/FUNCTION MEMBER
    SYMBOL sym )                // - the symbol
;
boolean SymIsThunk(             // DETERMINE IF FUNCTION IS THUNK
    SYMBOL func )               // - function which is possible thunk
;
boolean SymIsThunkCtorCopy(     // TEST IF COPY CTOR ADDRESSABILITY THUNK
    SYMBOL sym )                // - function symbol
;
boolean SymIsThunkCtorDflt(     // TEST IF DEFAULT CTOR ADDRESSABILITY THUNK
    SYMBOL sym )                // - function symbol
;
boolean SymIsThunkDtor(         // TEST IF DTOR ADDRESSABILITY THUNK
    SYMBOL sym )                // - function symbol
;
boolean SymIsUDC(               // TEST IF SYMBOL IS USER-DEFINED CONVERSION
    SYMBOL udc )                // - symbol in question
;
boolean SymIsVft(               // TEST IF SYMBOL IS VFT SYMBOL
    SYMBOL sym )                // - symbol
;
boolean SymIsVirtual(           // TEST IF FUNCTION IS VIRTUAL
    SYMBOL fun )                // - function symbol
;
void SymLocnCopy(               // COPY LOCATION TO SYMBOL FROM ANOTHER SYMBOL
    SYMBOL dest,                // - 'to' symbol
    SYMBOL src )                // - 'from' symbol
;
SYMBOL SymMakeDummy(            // MAKE A DUMMY SYMBOL
    TYPE type,                  // - type of the symbol
    char **name )               // - gets filled in with the name
;
SYMBOL SymMarkRefed(            // MARK SYMBOL AS REFERENCED
    SYMBOL sym )                // - the symbol
;
SYMBOL SymReloc (               // RELOCATE SYMBOL
    SYMBOL src,                 // - source symbol
    RELOC_LIST *reloc_list )    // - reloc list
;
boolean SymRequiresCtoring(     // TEST IF SYMBOL REQUIRES CTOR-ING
    SYMBOL sym )                // - the symbol
;
boolean SymRequiresDtoring(     // TEST IF SYMBOL REQUIRES DTOR-ING
    SYMBOL sym )                // - the symbol
;
SCOPE SymScope(                 // GET SCOPE FOR SYMBOL
    SYMBOL sym )                // - the symbol
;
void SymSetNvReferenced(        // SET REFERENCED IF NOT VIRTUAL FUNCTION
    SYMBOL sym )                // - the symbol
;
symbol_flag SymThrowFlags(      // GET SYMBOL'S THROW BITS
    SYMBOL sym )                // - the symbol
;
SYMBOL SymMakeAlias(            // DECLARE AN ALIAS IN CURRSCOPE
    SYMBOL aliasee,             // - sym to be aliased
    TOKEN_LOCN *locn )          // - location for symbol
;
SYMBOL SymDefArgBase(           // GET DEFARG BASE SYMBOL
    SYMBOL sym )                // - the symbol
;
