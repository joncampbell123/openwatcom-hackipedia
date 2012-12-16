/*
 *  os2def.h    OS/2 typedefs and constants for 32-bit development.
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

#ifndef __OS2DEF__
#define __OS2DEF__
#define OS2DEF_INCLUDED

#ifndef _ENABLE_AUTODEPEND
 #pragma read_only_file;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
 #ifdef __cplusplus
  #if !defined(_M_I86) || defined(__SMALL__) || defined(__MEDIUM__)
   #define NULL 0
  #else
   #define NULL 0L
  #endif 
 #else
  #define NULL ((void *)0)
 #endif
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define FAR
#define NEAR

#define MAKEP( sel,off )   ((void *)(void * _Seg16)( (sel) << 16 | (off) ))
#define MAKE16P( sel,off ) ((void * _Seg16)( (sel) << 16 | (off) ))

#define SELECTOROF(ptr)    ((((ULONG)(ptr))>>13)|7)
#define OFFSETOF(p)        (((PUSHORT)&(p))[0])

#define MAKETYPE(v, type)  (*((type *)&v))

#define FIELDOFFSET(type, field)   ((SHORT)&(((type *)0)->field))

#define MAKEULONG(l, h)  ((ULONG)(((USHORT)(l)) | ((ULONG)((USHORT)(h))) << 16))
#define MAKELONG(l, h)   ((LONG)MAKEULONG(l, h))
#define MAKEUSHORT(l, h) (((USHORT)(l)) | ((USHORT)(h)) << 8)
#define MAKESHORT(l, h)  ((SHORT)MAKEUSHORT(l, h))

#define LOBYTE(w)   LOUCHAR(w)
#define HIBYTE(w)   HIUCHAR(w)
#define LOUCHAR(w)  ((UCHAR)(w))
#define HIUCHAR(w)  ((UCHAR)(((USHORT)(w) >> 8) & 0xff))
#define LOUSHORT(l) ((USHORT)((ULONG)l))
#define HIUSHORT(l) ((USHORT)(((ULONG)(l) >> 16) & 0xffff))

#define NULLHANDLE  ((LHANDLE)0)
#define NULLSHANDLE ((SHANDLE)0)

#define MAKEERRORID(sev, error) (ERRORID)(MAKEULONG((error), (sev)))
#define ERRORIDERROR(errid)     (LOUSHORT(errid))
#define ERRORIDSEV(errid)       (HIUSHORT(errid))

#define SEVERITY_NOERROR       0x0000
#define SEVERITY_WARNING       0x0004
#define SEVERITY_ERROR         0x0008
#define SEVERITY_SEVERE        0x000C
#define SEVERITY_UNRECOVERABLE 0x0010

#define WINERR_BASE 0x1000
#define GPIERR_BASE 0x2000
#define DEVERR_BASE 0x3000
#define SPLERR_BASE 0x4000

#define WRECT    RECTL
#define PWRECT   PRECTL
#define NPWRECT  NPRECTL

#define WPOINT   POINTL
#define PWPOINT  PPOINTL
#define NPWPOINT NPPOINTL

#define ICON_FILE     1
#define ICON_RESOURCE 2
#define ICON_DATA     3
#define ICON_CLEAR    4

#define APIENTRY16 _Far16 _Pascal
#define PASCAL16   _Far16 _Pascal
#define CDECL16    _Far16 _Cdecl

#define EXPENTRY  _System
#define APIENTRY  _System

#define CHAR     char
#define SHORT    short
#define LONG     long
#define INT      int
#define VOID     void

#define DRIVER_NAME      1
#define DRIVER_DATA      2
#define DATA_TYPE        3
#define COMMENT          4
#define PROC_NAME        5
#define PROC_PARAMS      6
#define SPL_PARAMS       7
#define NETWORK_PARAMS   8

#define PD_JOB_PROPERTY   0x0001

#define FATTR_SEL_ITALIC            0x0001
#define FATTR_SEL_UNDERSCORE        0x0002
#define FATTR_SEL_OUTLINE           0x0008
#define FATTR_SEL_STRIKEOUT         0x0010
#define FATTR_SEL_BOLD              0x0020
#define FATTR_SEL_MUST_COLOR        0x0100
#define FATTR_SEL_MUST_MIXEDMODES   0x0200
#define FATTR_SEL_MUST_HOLLOW       0x0400

#define FATTR_TYPE_KERNING          0x0004
#define FATTR_TYPE_MBCS             0x0008
#define FATTR_TYPE_DBCS             0x0010
#define FATTR_TYPE_ANTIALIASED      0x0020


#define FATTR_FONTUSE_NOMIX         0x0002
#define FATTR_FONTUSE_OUTLINE       0x0004
#define FATTR_FONTUSE_TRANSFORMABLE 0x0008

#define FACESIZE 32

#define FM_TYPE_FIXED           0x0001
#define FM_TYPE_LICENSED        0x0002
#define FM_TYPE_KERNING         0x0004
#define FM_TYPE_DBCS            0x0010
#define FM_TYPE_MBCS            0x0018
#define FM_TYPE_UNICODE         0x0040
#define FM_TYPE_64K             0x8000
#define FM_TYPE_ATOMS           0x4000
#define FM_TYPE_FAMTRUNC        0x2000
#define FM_TYPE_FACETRUNC       0x1000

#define FM_DEFN_OUTLINE         0x0001
#define FM_DEFN_IFI             0x0002
#define FM_DEFN_WIN             0x0004
#define FM_DEFN_GENERIC         0x8000

#define FM_DEFN_LATIN1          0x0010
#define FM_DEFN_PC              0x0020
#define FM_DEFN_LATIN2          0x0040
#define FM_DEFN_CYRILLIC        0x0080
#define FM_DEFN_HEBREW          0x0100
#define FM_DEFN_GREEK           0x0200
#define FM_DEFN_ARABIC          0x0400
#define FM_DEFN_UGLEXT          0x0800
#define FM_DEFN_KANA            0x1000
#define FM_DEFN_THAI            0x2000

#define FM_DEFN_UGL383          0x0070
#define FM_DEFN_UGL504          0x00F0
#define FM_DEFN_UGL767          0x0FF0
#define FM_DEFN_UGL1105         0x3FF0

#define FM_SEL_ITALIC           0x0001
#define FM_SEL_UNDERSCORE       0x0002
#define FM_SEL_NEGATIVE         0x0004
#define FM_SEL_OUTLINE          0x0008
#define FM_SEL_STRIKEOUT        0x0010
#define FM_SEL_BOLD             0x0020
#define FM_SEL_ISO9241_TESTED   0x0040

#define FM_SEL_JAPAN            0x1000
#define FM_SEL_TAIWAN           0x2000
#define FM_SEL_CHINA            0x4000
#define FM_SEL_KOREA            0x8000
#define FM_SEL_DBCSMASK         0xF000

#define FM_ISO_9518_640         0x01
#define FM_ISO_9515_640         0x02
#define FM_ISO_9515_1024        0x04
#define FM_ISO_9517_640         0x08
#define FM_ISO_9517_1024        0x10

#define FM_CAP_NOMIX            0x0001
#define FM_CAP_NO_COLOR         0x0002
#define FM_CAP_NO_MIXEDMODES    0x0004
#define FM_CAP_NO_HOLLOW        0x0008

typedef unsigned long   APIRET;
typedef unsigned short  APIRET16;
typedef unsigned long   APIRET32;

typedef unsigned char   UCHAR, *PUCHAR;
typedef unsigned char   BYTE, *PBYTE, *NPBYTE;
typedef unsigned short  USHORT, *PUSHORT;
typedef unsigned int    UINT, *PUINT;
typedef unsigned long   ULONG, *PULONG;

typedef unsigned short  SHANDLE;
typedef unsigned long   LHANDLE;

typedef char *PSZ;
typedef char *NPSZ;
typedef char *PCH;
typedef char *NPCH;
typedef const char *PCSZ;

typedef unsigned long BOOL, *PBOOL;
typedef unsigned LONG BOOL32, *PBOOL32;

typedef unsigned short BOOL16;
typedef BOOL16   * _Seg16 PBOOL16;

typedef CHAR     *PCHAR;
typedef SHORT    *PSHORT;
typedef INT      *PINT;
typedef LONG     *PLONG;

typedef CHAR     * _Seg16 PCHAR16;
typedef UCHAR    * _Seg16 PUCHAR16;

typedef VOID     *PVOID;
typedef PVOID    *PPVOID;
typedef VOID     * _Seg16 PVOID16;

typedef int (APIENTRY _PFN)();
typedef _PFN  *PFN;
typedef int (APIENTRY _NPFN)();
typedef _NPFN *NPFN;

typedef USHORT  SEL, *PSEL;

typedef LHANDLE HFILE, *PHFILE;
typedef LHANDLE HMODULE, *PHMODULE;
typedef LHANDLE PID, *PPID;
typedef LHANDLE TID, *PTID;
typedef LHANDLE HAB, *PHAB;
typedef LHANDLE HPS, *PHPS;
typedef LHANDLE HDC, *PHDC;
typedef LHANDLE HRGN, *PHRGN;
typedef LHANDLE HBITMAP, *PHBITMAP;
typedef LHANDLE HMF, *PHMF;
typedef LHANDLE HPAL, *PHPAL;
typedef LHANDLE HWND, *PHWND;
typedef LHANDLE HMQ, *PHMQ;
typedef LHANDLE HPOINTER;
typedef LONG    COLOR, *PCOLOR;
typedef LONG    FIXED, *PFIXED;
typedef ULONG   HEV,  *PHEV;
typedef ULONG   HMTX, *PHMTX;
typedef ULONG   HMUX, *PHMUX;
typedef VOID    *HSEM;
typedef HSEM    *PHSEM;
typedef USHORT  SGID;

typedef VOID    *MPARAM, **PMPARAM;
typedef VOID    *MRESULT, **PMRESULT;

typedef ULONG   ERRORID, *PERRORID;

typedef CHAR    STR8[8];
typedef STR8    *PSTR8;

typedef PSZ *PDEVOPENDATA;

typedef struct _QWORD {
    ULONG   ulLo;
    ULONG   ulHi;
} QWORD, *PQWORD;

typedef struct _PRINTDEST {
   ULONG        cb;
   LONG         lType;
   PSZ          pszToken;
   LONG         lCount;
   PDEVOPENDATA pdopData;
   ULONG        fl;
   PSZ          pszPrinter;
} PRINTDEST, *PPRINTDEST;

typedef struct _ICONINFO {
    ULONG   cb;
    ULONG   fFormat;
    PSZ     pszFileName;
    HMODULE hmod;
    ULONG   resid;
    ULONG   cbIconData;
    PVOID   pIconData;
} ICONINFO, *PICONINFO;

typedef struct _ACCEL {
    USHORT fs;
    USHORT key;
    USHORT cmd;
} ACCEL, *PACCEL;

#ifndef LONGLONG_INCLUDED
#define LONGLONG_INCLUDED 1

#ifndef INCL_LONGLONG
typedef struct _LONGLONG {
    ULONG ulLo;
    LONG  ulHi;
} LONGLONG, *PLONGLONG;

typedef struct _ULONGLONG {
    ULONG ulLo;
    ULONG ulHi;
} ULONGLONG, *PULONGLONG;
#else
typedef long long LONGLONG, *PLONGLONG;
typedef unsigned long long ULONGLONG, *PULONGLONG;
#endif

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
