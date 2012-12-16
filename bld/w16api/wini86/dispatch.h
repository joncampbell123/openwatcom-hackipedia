/*
 *  dispatch.h  OLE automation interfaces and functions
 *
 * =========================================================================
 *
 *                          Open Watcom Project
 *
 * Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
 *
 *    This file is automatically generated. Do not edit directly.
 *
 * =========================================================================
 */

#ifndef _DISPATCH_H_
#define _DISPATCH_H_

#ifndef _ENABLE_AUTODEPEND
 #pragma read_only_file;
#endif

#include <variant.h>

/* Forward declarations */
typedef interface ITypeInfo         ITypeInfo;
typedef interface ITypeComp         ITypeComp;
typedef interface ICreateTypeInfo   ICreateTypeInfo;

/* Locale identifier data type */
#ifndef _LCID_DEFINED
#define _LCID_DEFINED
typedef unsigned long   LCID;
#endif

/* Primitive data types for automation */
typedef long            DISPID;
typedef DISPID          MEMBERID;
typedef unsigned long   HREFTYPE;

/* Version numbers for STDOLE.TLB */
#define STDOLE_MAJORVERNUM  1
#define STDOLE_MINORVERNUM  0
#define STDOLE_LCID         0x0000

/* Macros to make OLE automation error codes */
#define DISP_ERROR( x ) MAKE_SCODE( SEVERITY_ERROR, FACILITY_DISPATCH, x )
#define TYPE_ERROR( x ) MAKE_SCODE( SEVERITY_ERROR, FACILITY_DISPATCH, x )

/* OLE automation error codes */
#define DISP_E_UNKNOWNINTERFACE DISP_ERROR( 1 )
#define DISP_E_MEMBERNOTFOUND   DISP_ERROR( 3 )
#define DISP_E_PARAMNOTFOUND    DISP_ERROR( 4 )
#define DISP_E_TYPEMISMATCH     DISP_ERROR( 5 )
#define DISP_E_UNKNOWNNAME      DISP_ERROR( 6 )
#define DISP_E_NONAMEDARGS      DISP_ERROR( 7 )
#define DISP_E_BADVARTYPE       DISP_ERROR( 8 )
#define DISP_E_EXCEPTION        DISP_ERROR( 9 )
#define DISP_E_OVERFLOW         DISP_ERROR( 10 )
#define DISP_E_BADINDEX         DISP_ERROR( 11 )
#define DISP_E_UNKNOWNLCID      DISP_ERROR( 12 )
#define DISP_E_ARRAYISLOCKED    DISP_ERROR( 13 )
#define DISP_E_BADPARAMCOUNT    DISP_ERROR( 14 )
#define DISP_E_PARAMNOTOPTIONAL DISP_ERROR( 15 )
#define DISP_E_BADCALLEE        DISP_ERROR( 16 )
#define DISP_E_NOTACOLLECTION   DISP_ERROR( 17 )

/* Type error codes */
#define TYPE_E_BUFFERTOOSMALL           TYPE_ERROR( 32790 )
#define TYPE_E_INVDATAREAD              TYPE_ERROR( 32792 )
#define TYPE_E_UNSUPFORMAT              TYPE_ERROR( 32793 )
#define TYPE_E_REGISTRYACCESS           TYPE_ERROR( 32796 )
#define TYPE_E_LIBNOTREGISTERED         TYPE_ERROR( 32797 )
#define TYPE_E_UNDEFINEDTYPE            TYPE_ERROR( 32807 )
#define TYPE_E_QUALIFIEDNAMEDISALLOWED  TYPE_ERROR( 32808 )
#define TYPE_E_INVALIDSTATE             TYPE_ERROR( 32809 )
#define TYPE_E_WRONGTYPEKIND            TYPE_ERROR( 32810 )
#define TYPE_E_ELEMENTNOTFOUND          TYPE_ERROR( 32811 )
#define TYPE_E_AMBIGUOUSNAME            TYPE_ERROR( 32812 )
#define TYPE_E_NAMECONFLICT             TYPE_ERROR( 32813 )
#define TYPE_E_UNKNOWNLCID              TYPE_ERROR( 32814 )
#define TYPE_E_DLLFUNCTIONNOTFOUND      TYPE_ERROR( 32815 )
#define TYPE_E_BADMODULEKIND            TYPE_ERROR( 35005 )
#define TYPE_E_SIZETOOBIG               TYPE_ERROR( 35013 )
#define TYPE_E_DUPLICATEID              TYPE_ERROR( 35014 )
#define TYPE_E_INVALIDID                TYPE_ERROR( 35023 )
#define TYPE_E_TYPEMISMATCH             TYPE_ERROR( 36000 )
#define TYPE_E_OUTOFBOUNDS              TYPE_ERROR( 36001 )
#define TYPE_E_IOERROR                  TYPE_ERROR( 36002 )
#define TYPE_E_CANTCREATETMPFILE        TYPE_ERROR( 36003 )
#define TYPE_E_CANTLOADLIBRARY          TYPE_ERROR( 40010 )
#define TYPE_E_INCONSISTENTPROPFUNCS    TYPE_ERROR( 40067 )
#define TYPE_E_CIRCULARTYPE             TYPE_ERROR( 40068 )

/* VariantChangeType() flags */
#define VARIANT_NOVALUEPROP 0x0001

/* Variant date conversion flags */
#define VAR_TIMEVALUEONLY   0x0001
#define VAR_DATEVALUEONLY   0x0002

/* Member identifier special value */
#define MEMBERID_NIL    DISPID_UNKNOWN

/* Default instance identifier */
#define ID_DEFAULTINST  (-2)

/* IDL flags */
#define IDLFLAG_NONE    0x0000
#define IDLFLAG_FIN     0x0001
#define IDLFLAG_FOUT    0x0002
#define IDLFLAG_FLCID   0x0004
#define IDLFLAG_FRETVAL 0x0008

/* IDispatch::Invoke() flags */
#define DISPATCH_METHOD         0x00000001L
#define DISPATCH_PROPERTYGET    0x00000002L
#define DISPATCH_PROPERTYPUT    0x00000004L
#define DISPATCH_PROPERTYPUTREF 0x00000008L

/* Implementation type flags */
#define IMPLTYPEFLAG_FDEFAULT       0x0001
#define IMPLTYPEFLAG_FSOURCE        0x0002
#define IMPLTYPEFLAG_FRESTRICTED    0x0004

/* Dispatch identifier special values */
#define DISPID_VALUE        0
#define DISPID_UNKNOWN      (-1)
#define DISPID_PROPERTYPUT  (-3)
#define DISPID_NEWENUM      (-4)
#define DISPID_EVALUATE     (-5)
#define DISPID_CONSTRUCTOR  (-6)
#define DISPID_DESTRUCTOR   (-7)
#define DISPID_COLLECT      (-8)

/* RegisterActiveObject() flags */
#define ACTIVEOBJECT_STRONG 0x00000000L
#define ACTIVEOBJECT_WEAK   0x00000001L

/* System kinds */
typedef enum tagSYSKIND {
    SYS_WIN16   = 0,
    SYS_WIN32   = 1,
    SYS_MAC     = 2
} SYSKIND;

/* Library flags */
typedef enum tagLIBFLAGS {
    LIBFLAG_FRESTRICTED = 0x0001,
    LIBFLAG_FCONTROL    = 0x0002,
    LIBFLAG_FHIDDEN     = 0x0004
} LIBFLAGS;

/* Type library attribute */
typedef struct FARSTRUCT tagTLIBATTR {
    GUID            guid;
    LCID            lcid;
    SYSKIND         syskind;
    unsigned short  wMajorVerNum;
    unsigned short  wMinorVerNum;
    unsigned short  wLibFlags;
} TLIBATTR;
typedef TLIBATTR FAR    *LPTLIBATTR;

/* Type kinds */
typedef enum tagTYPEKIND {
    TKIND_ENUM      = 0,
    TKIND_RECORD    = 1,
    TKIND_MODULE    = 2,
    TKIND_INTERFACE = 3,
    TKIND_DISPATCH  = 4,
    TKIND_COCLASS   = 5,
    TKIND_ALIAS     = 6,
    TKIND_UNION     = 7,
    TKIND_MAX       = 8
} TYPEKIND;

/* Type descriptor */
typedef struct FARSTRUCT tagTYPEDESC {
    union {
        struct FARSTRUCT tagTYPEDESC FAR    *lptdesc;
        struct FARSTRUCT tagARRAYDESC FAR   *lpadesc;
        HREFTYPE                            hreftype;
#ifdef NONAMELESSUNION
    } u;
#else
    };
#endif
    VARTYPE vt;
} TYPEDESC;

/* Array descriptor */
typedef struct FARSTRUCT tagARRAYDESC {
    TYPEDESC        tdescElem;
    unsigned short  cDims;
    SAFEARRAYBOUND  rgbounds[1];
} ARRAYDESC;

/* IDL descriptor */
typedef struct FARSTRUCT tagIDLDESC {
    BSTR            bstrIDLInfo;
    unsigned short  wIDLFlags;
} IDLDESC;
typedef IDLDESC FAR *LPIDLDESC;

/* Element descriptor */
typedef struct FARSTRUCT tagELEMDESC {
    TYPEDESC    tdesc;
    IDLDESC     idldesc;
} ELEMDESC;
typedef ELEMDESC FAR    *LPELEMDESC;

/* Type attributes */
typedef struct FARSTRUCT tagTYPEATTR {
    GUID            guid;
    LCID            lcid;
    unsigned long   dwReserved;
    MEMBERID        memidConstructor;
    MEMBERID        memidDestructor;
    OLECHAR FAR     *lpstrSchema;
    unsigned long   cbSizeInstance;
    TYPEKIND        typekind;
    unsigned short  cFuncs;
    unsigned short  cVars;
    unsigned short  cImplTypes;
    unsigned short  cbSizeVft;
    unsigned short  cbAlignment;
    unsigned short  wTypeFlags;
    unsigned short  wMajorVerNum;
    unsigned short  wMinorVerNum;
    TYPEDESC        tdescAlias;
    IDLDESC         idldescType;
} TYPEATTR;
typedef TYPEATTR FAR    *LPTYPEATTR;

/* Dispatch parameters */
typedef struct FARSTRUCT tagDISPPARAMS {
    VARIANTARG FAR  *rgvarg;
    DISPID FAR      *rgdispidNamedArgs;
    unsigned int    cArgs;
    unsigned int    cNamedArgs;
} DISPPARAMS;

/* Exception information */
typedef struct FARSTRUCT tagEXCEPINFO {
    unsigned short  wCode;
    unsigned short  wReserved;
    BSTR            bstrSource;
    BSTR            bstrDescription;
    BSTR            bstrHelpFile;
    unsigned long   dwHelpContext;
    void FAR        *pvReserved;
    HRESULT         (STDAPICALLTYPE *pfnDeferredFillIn)( struct tagEXCEPINFO FAR * );
    SCODE           scode;
} EXCEPINFO;
typedef EXCEPINFO FAR   *LPEXCEPINFO;

/* Calling conventions */
typedef enum tagCALLCONV {
    CC_CDECL        = 1,
    CC_MSCPASCAL    = 2,
    CC_PASCAL       = CC_MSCPASCAL,
    CC_MACPASCAL    = 3,
    CC_STDCALL      = 4,
    CC_SYSCALL      = 6,
    CC_MPWCDECL     = 7,
    CC_MPWPASCAL    = 8,
    CC_MAX          = 9
} CALLCONV;

/* Function kinds */
typedef enum tagFUNCKIND {
    FUNC_VIRTUAL        = 0,
    FUNC_PUREVIRTUAL    = 1,
    FUNC_NONVIRTUAL     = 2,
    FUNC_STATIC         = 3,
    FUNC_DISPATCH       = 4
} FUNCKIND;

/* Invoke kinds */
typedef enum tagINVOKEKIND {
    INVOKE_FUNC             = DISPATCH_METHOD,
    INVOKE_PROPERTYGET      = DISPATCH_PROPERTYGET,
    INVOKE_PROPERTYPUT      = DISPATCH_PROPERTYPUT,
    INVOKE_PROPERTYPUTREF   = DISPATCH_PROPERTYPUTREF
} INVOKEKIND;

/* Function descriptor */
typedef struct FARSTRUCT tagFUNCDESC {
    MEMBERID        memid;
    SCODE FAR       *lprgscode;
    ELEMDESC FAR    *lprgelemdescParam;
    FUNCKIND        funckind;
    INVOKEKIND      invkind;
    CALLCONV        callconv;
    short           cParams;
    short           cParamsOpt;
    short           oVft;
    short           cScodes;
    ELEMDESC        elemdescFunc;
    unsigned short  wFuncFlags;
} FUNCDESC;
typedef FUNCDESC FAR    *LPFUNCDESC;

/* Variable kinds */
typedef enum tagVARKIND {
    VAR_PERINSTANCE = 0,
    VAR_STATIC      = 1,
    VAR_CONST       = 2,
    VAR_DISPATCH    = 3
} VARKIND;

/* Variable descriptor */
typedef struct FARSTRUCT tagVARDESC {
    MEMBERID        memid;
    OLECHAR FAR     *lpstrSchema;
    union {
        unsigned long   oInst;
        VARIANT FAR     *lpvarValue;
#ifdef NONAMELESSUNION
    } u;
#else
    };
#endif
    ELEMDESC        elemdescVar;
    unsigned short  wVarFlags;
    VARKIND         varkind;
} VARDESC;
typedef VARDESC FAR *LPVARDESC;

/* Type flags */
typedef enum tagTYPEFLAGS {
    TYPEFLAG_FAPPOBJECT     = 0x0001,
    TYPEFLAG_FCANCREATE     = 0x0002,
    TYPEFLAG_FLICENSED      = 0x0004,
    TYPEFLAG_FPREDECLID     = 0x0008,
    TYPEFLAG_FHIDDEN        = 0x0010,
    TYPEFLAG_FCONTROL       = 0x0020,
    TYPEFLAG_FDUAL          = 0x0040,
    TYPEFLAG_FNONEXTENSIBLE = 0x0080,
    TYPEFLAG_FOLEAUTOMATION = 0x0100,
    TYPEFLAG_FRESTRICTED    = 0x0200
} TYPEFLAGS;

/* Function flags */
typedef enum tagFUNCFLAGS {
    FUNCFLAG_FRESTRICTED        = 0x0001,
    FUNCFLAG_FSOURCE            = 0x0002,
    FUNCFLAG_FBINDABLE          = 0x0004,
    FUNCFLAG_FREQUESTEDIT       = 0x0008,
    FUNCFLAG_FDISPLAYBIND       = 0x0010,
    FUNCFLAG_FDEFAULTBIND       = 0x0020,
    FUNCFLAG_FHIDDEN            = 0x0040,
    FUNCFLAG_FUSESGETLASTERROR  = 0x0080
} FUNCFLAGS;

/* Variable flags */
typedef enum tagVARFLAGS {
    VARFLAG_FREADONLY       = 0x0001,
    VARFLAG_FSOURCE         = 0x0002,
    VARFLAG_FBINDABLE       = 0x0004,
    VARFLAG_FREQUESTEDIT    = 0x0008,
    VARFLAG_FDISPLAYBIND    = 0x0010,
    VARFLAG_FDEFAULTBIND    = 0x0020,
    VARFLAG_FHIDDEN         = 0x0040
} VARFLAGS;

/* Descriptor kinds */
typedef enum tagDESCKIND {
    DESCKIND_NONE           = 0,
    DESCKIND_FUNCDESC       = 1,
    DESCKIND_VARDESC        = 2,
    DESCKIND_TYPECOMP       = 3,
    DESCKIND_IMPLICITAPPOBJ = 4,
    DESCKIND_MAX            = 5
} DESCKIND;

/* Bind pointer */
typedef union tagBINDPTR {
    FUNCDESC FAR    *lpfuncdesc;
    VARDESC FAR     *lpvardesc;
    ITypeComp FAR   *lptcomp;
} BINDPTR;
typedef BINDPTR FAR *LPBINDPTR;

/* Parameter data */
typedef struct FARSTRUCT tagPARAMDATA {
    OLECHAR FAR *szName;
    VARTYPE     vt;
} PARAMDATA;
typedef PARAMDATA FAR   *LPPARAMDATA;

/* Method data */
typedef struct FARSTRUCT tagMETHODDATA {
    OLECHAR FAR     *szName;
    PARAMDATA FAR   *ppdata;
    DISPID          dispid;
    unsigned int    iMeth;
    CALLCONV        cc;
    unsigned int    cArgs;
    unsigned short  wFlags;
    VARTYPE         vtReturn;
} METHODDATA;
typedef METHODDATA FAR  *LPMETHODDATA;

/* Interface data */
typedef struct FARSTRUCT tagINTERFACEDATA {
    METHODDATA FAR  *pmethdata;
    unsigned int    cMembers;
} INTERFACEDATA;
typedef INTERFACEDATA FAR   *LPINTERFACEDATA;

/* GUIDs */
EXTERN_C const GUID CDECL FAR   IID_IDispatch;
EXTERN_C const GUID CDECL FAR   IID_IEnumVARIANT;
EXTERN_C const GUID CDECL FAR   IID_ITypeInfo;
EXTERN_C const GUID CDECL FAR   IID_ITypeLib;
EXTERN_C const GUID CDECL FAR   IID_ITypeComp;
EXTERN_C const GUID CDECL FAR   IID_ICreateTypeInfo;
EXTERN_C const GUID CDECL FAR   IID_ICreateTypeLib;
EXTERN_C const GUID CDECL FAR   IID_StdOle;
EXTERN_C const GUID CDECL FAR   IID_IErrorInfo;
EXTERN_C const GUID CDECL FAR   IID_ICreateErrorInfo;
EXTERN_C const GUID CDECL FAR   IID_ISupportErrorInfo;

/* ITypeLib interface */
#undef INTERFACE
#define INTERFACE   ITypeLib
DECLARE_INTERFACE_( ITypeLib, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* ITypeLib methods */
    STDMETHOD_( unsigned int, GetTypeInfoCount )( THIS ) PURE;
    STDMETHOD( GetTypeInfo )( THIS_ unsigned int, ITypeInfo FAR * FAR * ) PURE;
    STDMETHOD( GetTypeInfoType )( THIS_ unsigned int, TYPEKIND FAR * ) PURE;
    STDMETHOD( GetTypeInfoOfGuid )( THIS_ REFGUID, ITypeInfo FAR * FAR * ) PURE;
    STDMETHOD( GetLibAttr )( THIS_ TLIBATTR FAR * FAR * ) PURE;
    STDMETHOD( GetTypeComp )( THIS_ ITypeComp FAR * FAR * ) PURE;
    STDMETHOD( GetDocumentation )( THIS_ int, BSTR FAR *, BSTR FAR *, unsigned long FAR *, BSTR FAR * ) PURE;
    STDMETHOD( IsName )( THIS_ OLECHAR FAR *, unsigned long, int FAR * ) PURE;
    STDMETHOD( FindName )( THIS_ OLECHAR FAR *, unsigned long, ITypeInfo FAR * FAR *, MEMBERID FAR *, unsigned short FAR * ) PURE;
    STDMETHOD_( void, ReleaseTLibAttr )( THIS_ TLIBATTR FAR * ) PURE;
};
typedef ITypeLib FAR    *LPTYPELIB;

/* ITypeInfo interface */
#undef INTERFACE
#define INTERFACE   ITypeInfo
DECLARE_INTERFACE_( ITypeInfo, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* ITypeInfo methods */
    STDMETHOD( GetTypeAttr )( THIS_ TYPEATTR FAR * FAR * ) PURE;
    STDMETHOD( GetTypeComp )( THIS_ ITypeComp FAR * FAR * ) PURE;
    STDMETHOD( GetFuncDesc )( THIS_ unsigned int, FUNCDESC FAR * FAR * ) PURE;
    STDMETHOD( GetVarDesc )( THIS_ unsigned int, VARDESC FAR * FAR * ) PURE;
    STDMETHOD( GetNames )( THIS_ MEMBERID, BSTR FAR *, unsigned int, unsigned int FAR * ) PURE;
    STDMETHOD( GetRefTypeOfImplType )( THIS_ unsigned int, HREFTYPE FAR * ) PURE;
    STDMETHOD( GetImplTypeFlags )( THIS_ unsigned int, int FAR * ) PURE;
    STDMETHOD( GetIDsOfNames )( THIS_ OLECHAR FAR * FAR *, unsigned int, MEMBERID FAR * ) PURE;
    STDMETHOD( Invoke )( THIS_ void FAR *, MEMBERID, unsigned short, DISPPARAMS FAR *, VARIANT FAR *, EXCEPINFO FAR *, unsigned int FAR * ) PURE;
    STDMETHOD( GetDocumentation )( THIS_ MEMBERID, BSTR FAR *, BSTR FAR *, unsigned long FAR *, BSTR FAR * ) PURE;
    STDMETHOD( GetDllEntry )( THIS_ MEMBERID, INVOKEKIND, BSTR FAR *, BSTR FAR *, unsigned short FAR * ) PURE;
    STDMETHOD( GetRefTypeInfo )( THIS_ HREFTYPE, ITypeInfo FAR * FAR * ) PURE;
    STDMETHOD( AddressOfMember )( THIS_ MEMBERID, INVOKEKIND, void FAR * FAR * ) PURE;
    STDMETHOD( CreateInstance )( THIS_ IUnknown FAR *, REFIID, void FAR * FAR * ) PURE;
    STDMETHOD( GetMops )( THIS_ MEMBERID, BSTR FAR * ) PURE;
    STDMETHOD( GetContainingTypeLib )( THIS_ ITypeLib FAR * FAR *, unsigned int FAR * ) PURE;
    STDMETHOD_( void, ReleaseTypeAttr )( THIS_ TYPEATTR FAR * ) PURE;
    STDMETHOD_( void, ReleaseFuncDesc )( THIS_ FUNCDESC FAR * ) PURE;
    STDMETHOD_( void, ReleaseVarDesc )( THIS_ VARDESC FAR * ) PURE;
};
typedef ITypeInfo FAR   *LPTYPEINFO;

/* ITypeComp interface */
#undef INTERFACE
#define INTERFACE   ITypeComp
DECLARE_INTERFACE_( ITypeComp, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* ITypeComp methods */
    STDMETHOD( Bind )( THIS_ OLECHAR FAR *, unsigned long, unsigned short, ITypeInfo FAR * FAR *, DESCKIND FAR *, BINDPTR FAR * ) PURE;
    STDMETHOD( BindType )( THIS_ OLECHAR FAR *, unsigned long, ITypeInfo FAR * FAR *, ITypeComp FAR * FAR * ) PURE;
};
typedef ITypeComp FAR   *LPTYPECOMP;

/* ICreateTypeLib interface */
#undef INTERFACE
#define INTERFACE   ICreateTypeLib
DECLARE_INTERFACE_( ICreateTypeLib, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* ICreateTypeLib methods */
    STDMETHOD( CreateTypeInfo )( THIS_ OLECHAR FAR *, TYPEKIND, ICreateTypeInfo FAR * FAR * ) PURE;
    STDMETHOD( SetName )( THIS_ OLECHAR FAR * ) PURE;
    STDMETHOD( SetVersion )( THIS_ unsigned short, unsigned short ) PURE;
    STDMETHOD( SetGuid )( THIS_ REFGUID ) PURE;
    STDMETHOD( SetDocString )( THIS_ OLECHAR FAR * ) PURE;
    STDMETHOD( SetHelpFileName )( THIS_ OLECHAR FAR * ) PURE;
    STDMETHOD( SetHelpContext )( THIS_ unsigned long ) PURE;
    STDMETHOD( SetLcid )( THIS_ LCID ) PURE;
    STDMETHOD( SetLibFlags )( THIS_ unsigned int ) PURE;
    STDMETHOD( SaveAllChanges )( THIS ) PURE;
};
typedef ICreateTypeLib FAR  *LPCREATETYPELIB;

/* ICreateTypeInfo interface */
#undef INTERFACE
#define INTERFACE   ICreateTypeInfo
DECLARE_INTERFACE_( ICreateTypeInfo, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* ICreateTypeInfo methods */
    STDMETHOD( SetGuid )( THIS_ REFGUID ) PURE;
    STDMETHOD( SetTypeFlags )( THIS_ unsigned int ) PURE;
    STDMETHOD( SetDocString )( THIS_ OLECHAR FAR * ) PURE;
    STDMETHOD( SetHelpContext )( THIS_ unsigned long ) PURE;
    STDMETHOD( SetVersion )( THIS_ unsigned short, unsigned short ) PURE;
    STDMETHOD( AddRefTypeInfo )( THIS_ ITypeInfo FAR *, HREFTYPE FAR * ) PURE;
    STDMETHOD( AddFuncDesc )( THIS_ unsigned int, FUNCDESC FAR * ) PURE;
    STDMETHOD( AddImplType )( THIS_ unsigned int, HREFTYPE ) PURE;
    STDMETHOD( SetImplTypeFlags )( THIS_ unsigned int, int ) PURE;
    STDMETHOD( SetAlignment )( THIS_ unsigned short ) PURE;
    STDMETHOD( SetSchema )( THIS_ OLECHAR FAR * ) PURE;
    STDMETHOD( AddVarDesc )( THIS_ unsigned int, VARDESC FAR * ) PURE;
    STDMETHOD( SetFuncAndParamNames )( THIS_ unsigned int, OLECHAR FAR * FAR *, unsigned int ) PURE;
    STDMETHOD( SetVarName )( THIS_ unsigned int, OLECHAR FAR * ) PURE;
    STDMETHOD( SetTypeDescAlias )( THIS_ TYPEDESC FAR * ) PURE;
    STDMETHOD( DefineFuncAsDllEntry )( THIS_ unsigned int, OLECHAR FAR *, OLECHAR FAR * ) PURE;
    STDMETHOD( SetFuncDocString )( THIS_ unsigned int, OLECHAR FAR * ) PURE;
    STDMETHOD( SetVarDocString )( THIS_ unsigned int, OLECHAR FAR * ) PURE;
    STDMETHOD( SetFuncHelpContext )( THIS_ unsigned int, unsigned long ) PURE;
    STDMETHOD( SetVarHelpContext )( THIS_ unsigned int, unsigned long ) PURE;
    STDMETHOD( SetMops )( THIS_ unsigned int, BSTR ) PURE;
    STDMETHOD( SetTypeIdldesc )( THIS_ IDLDESC FAR * ) PURE;
    STDMETHOD( LayOut )( THIS ) PURE;
};
typedef ICreateTypeInfo FAR *LPCREATETYPEINFO;

/* IEnumVARIANT interface */
#undef INTERFACE
#define INTERFACE   IEnumVARIANT
DECLARE_INTERFACE_( IEnumVARIANT, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* IEnumVARIANT methods */
    STDMETHOD( Next )( THIS_ unsigned long, VARIANT FAR *, unsigned long FAR * ) PURE;
    STDMETHOD( Skip )( THIS_ unsigned long ) PURE;
    STDMETHOD( Reset )( THIS ) PURE;
    STDMETHOD( Clone )( THIS_ IEnumVARIANT FAR * FAR * );
};
typedef IEnumVARIANT FAR    *LPENUMVARIANT;

/* IDispatch interface */
#undef INTERFACE
#define INTERFACE   IDispatch
DECLARE_INTERFACE_( IDispatch, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* IDispatch methods */
    STDMETHOD( GetTypeInfoCount )( THIS_ unsigned int FAR * ) PURE;
    STDMETHOD( GetTypeInfo )( THIS_ unsigned int, LCID, ITypeInfo FAR * FAR * ) PURE;
    STDMETHOD( GetIDsOfNames )( THIS_ REFIID, OLECHAR FAR * FAR *, unsigned int, LCID, DISPID FAR * ) PURE;
    STDMETHOD( Invoke )( THIS_ DISPID, REFIID, LCID, unsigned short, DISPPARAMS FAR *, VARIANT FAR *, EXCEPINFO FAR *, unsigned int FAR * ) PURE;
};
typedef IDispatch FAR   *LPDISPATCH;

/* IErrorInfo interface */
#undef INTERFACE
#define INTERFACE   IErrorInfo
DECLARE_INTERFACE_( IErrorInfo, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* IErrorInfo methods */
    STDMETHOD( GetGUID )( THIS_ GUID FAR * ) PURE;
    STDMETHOD( GetSource )( THIS_ BSTR FAR * ) PURE;
    STDMETHOD( GetDescription )( THIS_ BSTR FAR * ) PURE;
    STDMETHOD( GetHelpFile )( THIS_ BSTR FAR * ) PURE;
    STDMETHOD( GetHelpContext )( THIS_ unsigned long FAR * ) PURE;
};

/* ICreateErrorInfo interface */
#undef INTERFACE
#define INTERFACE   ICreateErrorInfo
DECLARE_INTERFACE_( ICreateErrorInfo, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* ICreateErrorInfo methods */
    STDMETHOD( SetGUID )( THIS_ REFGUID ) PURE;
    STDMETHOD( SetSource )( THIS_ LPOLESTR ) PURE;
    STDMETHOD( SetDescription )( THIS_ LPOLESTR ) PURE;
    STDMETHOD( SetHelpFile )( THIS_ LPOLESTR ) PURE;
    STDMETHOD( SetHelpContext )( THIS_ unsigned long ) PURE;
};

/* ISupportErrorInfo interface */
#undef INTERFACE
#define INTERFACE   ISupportErrorInfo
DECLARE_INTERFACE_( ISupportErrorInfo, IUnknown ) {
    /* IUnknown methods */
    STDMETHOD( QueryInterface )( THIS_ REFIID, LPVOID FAR * ) PURE;
    STDMETHOD_( ULONG, AddRef )( THIS ) PURE;
    STDMETHOD_( ULONG, Release )( THIS ) PURE;

    /* ISupportErrorInfo methods */
    STDMETHOD( InterfaceSupportsErrorInfo )( THIS_ REFIID ) PURE;
};

/* Functions in OLE2DISP.DLL */
STDAPI                  CreateDispTypeInfo( INTERFACEDATA FAR *, LCID, ITypeInfo FAR * FAR * );
STDAPI                  CreateErrorInfo( ICreateErrorInfo FAR * FAR * );
STDAPI                  CreateStdDispatch( IUnknown FAR *, void FAR *, ITypeInfo FAR *, IUnknown FAR * FAR * );
STDAPI                  DispGetIDsOfNames( ITypeInfo FAR *, OLECHAR FAR * FAR *, unsigned int, DISPID FAR * );
STDAPI                  DispGetParam( DISPPARAMS FAR *, unsigned int, VARTYPE, VARIANT FAR *, unsigned int FAR * );
STDAPI                  DispInvoke( void FAR *, ITypeInfo FAR *, DISPID, unsigned short, DISPPARAMS FAR *, VARIANT FAR *, EXCEPINFO FAR *, unsigned int FAR * );
STDAPI_( int )          DosDateTimeToVariantTime( unsigned short, unsigned short, double FAR * );
STDAPI                  GetActiveObject( REFCLSID, void FAR *, IUnknown FAR * FAR * );
STDAPI                  GetErrorInfo( unsigned long, IErrorInfo FAR * FAR * );
STDAPI                  RegisterActiveObject( IUnknown FAR *, REFCLSID, unsigned long, unsigned long FAR * );
STDAPI                  RevokeActiveObject( unsigned long, void FAR * );
STDAPI                  SafeArrayAccessData( SAFEARRAY FAR *, void HUGEP * FAR * );
STDAPI                  SafeArrayAllocData( SAFEARRAY FAR * );
STDAPI                  SafeArrayAllocDescriptor( unsigned int, SAFEARRAY FAR * FAR * );
STDAPI                  SafeArrayCopy( SAFEARRAY FAR *, SAFEARRAY FAR * FAR * );
STDAPI_( SAFEARRAY FAR * )  SafeArrayCreate( VARTYPE, unsigned int, SAFEARRAYBOUND FAR * );
STDAPI                  SafeArrayDestroy( SAFEARRAY FAR * );
STDAPI                  SafeArrayDestroyData( SAFEARRAY FAR * );
STDAPI                  SafeArrayDestroyDescriptor( SAFEARRAY FAR * );
STDAPI_( unsigned int ) SafeArrayGetDim( SAFEARRAY FAR * );
STDAPI                  SafeArrayGetElement( SAFEARRAY FAR *, long FAR *, void FAR * );
STDAPI_( unsigned int ) SafeArrayGetElemsize( SAFEARRAY FAR * );
STDAPI                  SafeArrayGetLBound( SAFEARRAY FAR *, unsigned int, long FAR * );
STDAPI                  SafeArrayGetUBound( SAFEARRAY FAR *, unsigned int, long FAR * );
STDAPI                  SafeArrayLock( SAFEARRAY FAR * );
STDAPI                  SafeArrayPtrOfIndex( SAFEARRAY FAR *, long FAR *, void HUGEP * FAR * );
STDAPI                  SafeArrayPutElement( SAFEARRAY FAR *, long FAR *, void FAR * );
STDAPI                  SafeArrayRedim( SAFEARRAY FAR *, SAFEARRAYBOUND FAR * );
STDAPI                  SafeArrayUnaccessData( SAFEARRAY FAR * );
STDAPI                  SafeArrayUnlock( SAFEARRAY FAR * );
STDAPI                  SetErrorInfo( unsigned long, IErrorInfo FAR * );
STDAPI_( BSTR )         SysAllocString( const OLECHAR FAR * );
STDAPI_( BSTR )         SysAllocStringLen( const OLECHAR FAR *, unsigned int );
STDAPI_( void )         SysFreeString( BSTR );
STDAPI_( int )          SysReAllocString( BSTR FAR *, const OLECHAR FAR * );
STDAPI_( int )          SysReAllocStringLen( BSTR FAR *, const OLECHAR FAR *, unsigned int );
STDAPI_( unsigned int ) SysStringLen( BSTR );
STDAPI                  VarBoolFromCy( CY, VARIANT_BOOL FAR * );
STDAPI                  VarBoolFromDate( DATE, VARIANT_BOOL FAR * );
STDAPI                  VarBoolFromDisp( IDispatch FAR *, LCID, VARIANT_BOOL FAR * );
STDAPI                  VarBoolFromI2( short, VARIANT_BOOL FAR * );
STDAPI                  VarBoolFromI4( long, VARIANT_BOOL FAR * );
STDAPI                  VarBoolFromR4( float, VARIANT_BOOL FAR * );
STDAPI                  VarBoolFromR8( double, VARIANT_BOOL FAR * );
STDAPI                  VarBoolFromStr( OLECHAR FAR *, LCID, unsigned long, VARIANT_BOOL FAR * );
STDAPI                  VarBoolFromUI1( unsigned char, VARIANT_BOOL FAR * );
STDAPI                  VarBstrFromBool( VARIANT_BOOL, LCID, unsigned long, BSTR FAR * );
STDAPI                  VarBstrFromCy( CY, LCID, unsigned long, BSTR FAR * );
STDAPI                  VarBstrFromDate( DATE, LCID, unsigned long, BSTR FAR * );
STDAPI                  VarBstrFromDisp( IDispatch FAR *, LCID, unsigned long, BSTR FAR * );
STDAPI                  VarBstrFromI2( short, LCID, unsigned long, BSTR FAR * );
STDAPI                  VarBstrFromI4( long, LCID, unsigned long, BSTR FAR * );
STDAPI                  VarBstrFromR4( float, LCID, unsigned long, BSTR FAR * );
STDAPI                  VarBstrFromR8( double, LCID, unsigned long, BSTR FAR * );
STDAPI                  VarBstrFromUI1( unsigned char, LCID, unsigned long, BSTR FAR * );
STDAPI                  VarCyFromBool( VARIANT_BOOL, CY FAR * );
STDAPI                  VarCyFromDate( DATE, CY FAR * );
STDAPI                  VarCyFromDisp( IDispatch FAR *, LCID, CY FAR * );
STDAPI                  VarCyFromI2( short, CY FAR * );
STDAPI                  VarCyFromI4( long, CY FAR * );
STDAPI                  VarCyFromR4( float, CY FAR * );
STDAPI                  VarCyFromR8( double, CY FAR * );
STDAPI                  VarCyFromStr( OLECHAR FAR *, LCID, unsigned long, CY FAR * );
STDAPI                  VarCyFromUI1( unsigned char, CY FAR * );
STDAPI                  VarDateFromBool( VARIANT_BOOL, DATE FAR * );
STDAPI                  VarDateFromCy( CY, DATE FAR * );
STDAPI                  VarDateFromDisp( IDispatch FAR *, LCID, DATE FAR * );
STDAPI                  VarDateFromI2( short, DATE FAR * );
STDAPI                  VarDateFromI4( long, DATE FAR * );
STDAPI                  VarDateFromR4( float, DATE FAR * );
STDAPI                  VarDateFromR8( double, DATE FAR * );
STDAPI                  VarDateFromStr( OLECHAR FAR *, LCID, unsigned long, DATE FAR * );
STDAPI                  VarDateFromUI1( unsigned char, DATE FAR * );
STDAPI                  VarI2FromBool( VARIANT_BOOL, short FAR * );
STDAPI                  VarI2FromCy( CY, short FAR * );
STDAPI                  VarI2FromDate( DATE, short FAR * );
STDAPI                  VarI2FromDisp( IDispatch FAR *, LCID, short FAR * );
STDAPI                  VarI2FromI4( long, short FAR * );
STDAPI                  VarI2FromR4( float, short FAR * );
STDAPI                  VarI2FromR8( double, short FAR * );
STDAPI                  VarI2FromStr( OLECHAR FAR *, LCID, unsigned long, short FAR * );
STDAPI                  VarI2FromUI1( unsigned char, short FAR * );
STDAPI                  VarI4FromBool( VARIANT_BOOL, long FAR * );
STDAPI                  VarI4FromCy( CY, long FAR * );
STDAPI                  VarI4FromDate( DATE, long FAR * );
STDAPI                  VarI4FromDisp( IDispatch FAR *, LCID, long FAR * );
STDAPI                  VarI4FromI2( short, long FAR * );
STDAPI                  VarI4FromR4( float, long FAR * );
STDAPI                  VarI4FromR8( double, long FAR * );
STDAPI                  VarI4FromStr( OLECHAR FAR *, LCID, unsigned long, long FAR * );
STDAPI                  VarI4FromUI1( unsigned char, long FAR * );
STDAPI                  VarR4FromBool( VARIANT_BOOL, float FAR * );
STDAPI                  VarR4FromCy( CY, float FAR * );
STDAPI                  VarR4FromDate( DATE, float FAR * );
STDAPI                  VarR4FromDisp( IDispatch FAR *, LCID, float FAR * );
STDAPI                  VarR4FromI2( short, float FAR * );
STDAPI                  VarR4FromI4( long, float FAR * );
STDAPI                  VarR4FromR8( double, float FAR * );
STDAPI                  VarR4FromStr( OLECHAR FAR *, LCID, unsigned long, float FAR * );
STDAPI                  VarR4FromUI1( unsigned char, float FAR * );
STDAPI                  VarR8FromBool( VARIANT_BOOL, double FAR * );
STDAPI                  VarR8FromCy( CY, double FAR * );
STDAPI                  VarR8FromDate( DATE, double FAR * );
STDAPI                  VarR8FromDisp( IDispatch FAR *, LCID, double FAR * );
STDAPI                  VarR8FromI2( short, double FAR * );
STDAPI                  VarR8FromI4( long, double FAR * );
STDAPI                  VarR8FromR4( float, double FAR * );
STDAPI                  VarR8FromStr( OLECHAR FAR *, LCID, unsigned long, double FAR * );
STDAPI                  VarR8FromUI1( unsigned char, double FAR * );
STDAPI                  VarUI1FromBool( VARIANT_BOOL, unsigned char FAR * );
STDAPI                  VarUI1FromCy( CY, unsigned char FAR * );
STDAPI                  VarUI1FromDate( DATE, unsigned char FAR * );
STDAPI                  VarUI1FromDisp( IDispatch FAR *, LCID, unsigned char FAR * );
STDAPI                  VarUI1FromI2( short, unsigned char FAR * );
STDAPI                  VarUI1FromI4( long, unsigned char FAR * );
STDAPI                  VarUI1FromR4( float, unsigned char FAR * );
STDAPI                  VarUI1FromR8( double, unsigned char FAR * );
STDAPI                  VarUI1FromStr( OLECHAR FAR *, LCID, unsigned long, unsigned char FAR * );
STDAPI                  VariantChangeType( VARIANTARG FAR *, VARIANTARG FAR *, unsigned short, VARTYPE );
STDAPI                  VariantChangeTypeEx( VARIANTARG FAR *, VARIANTARG FAR *, LCID, unsigned short, VARTYPE );
STDAPI                  VariantClear( VARIANTARG FAR * );
STDAPI                  VariantCopy( VARIANTARG FAR *, VARIANTARG FAR * );
STDAPI                  VariantCopyInd( VARIANT FAR *, VARIANTARG FAR * );
STDAPI_( void )         VariantInit( VARIANTARG FAR * );
STDAPI_( int )          VariantTimeToDosDateTime( double, unsigned short FAR *, unsigned short FAR * );

/* Functions in TYPELIB.DLL */
STDAPI                      CreateTypeLib( SYSKIND, const OLECHAR FAR *, ICreateTypeLib FAR * FAR * );
STDAPI_( unsigned long )    LHashValOfNameSys( SYSKIND, LCID, const OLECHAR FAR * );
STDAPI                      LoadRegTypeLib( REFGUID, unsigned short, unsigned short, LCID, ITypeLib FAR * FAR * );
STDAPI                      LoadTypeLib( const OLECHAR FAR *, ITypeLib FAR * FAR * );
STDAPI_( unsigned long )    OaBuildVersion( void );
STDAPI                      QueryPathOfRegTypeLib( REFGUID, unsigned short, unsigned short, LCID, LPBSTR );
STDAPI                      RegisterTypeLib( ITypeLib FAR *, OLECHAR FAR *, OLECHAR FAR * );

/* Functions implemented as macros */
#define LHashValOfName( p1, p2 )    LHashValOfNameSys( SYS_WIN32, p1, p2 )
#define WHashValOfLHashVal( x )     ((unsigned short)((x) & 0x0000FFFFL))
#define IsHashValCompatible( p1, p2 ) \
    ((BOOL)(((p1) & 0x00FF0000L) == ((p2) & 0x00FF0000L)))

#endif /* _DISPATCH_H_ */
