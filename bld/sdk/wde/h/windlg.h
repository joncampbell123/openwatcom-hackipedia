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


#if !defined( __OS2__ )

#ifdef __NT__
#define ADJUST_ITEMLEN( a ) a = (((a)+7) & ~7)
#define ADJUST_BLOCKLEN( a ) a = (((a)+3) & ~3)
#define ROUND_CLASSLEN( a ) (((a)+1) & ~1)
#define _ISFAR
#define _FARmemcpy      memcpy
#define MK_FP32( a )    a
#define SLEN( a ) ((a)?strlen((a))*2+2:2)
typedef WORD INFOTYPE;
#else
#define SLEN( a ) ((a)?strlen((a))+1:1)
#define ADJUST_ITEMLEN( a )
#define ADJUST_BLOCKLEN( a )
#define ROUND_CLASSLEN( a ) a
#define _ISFAR  __far
#define _FARmemcpy      _fmemcpy
//#define MK_FP32( a ) a
typedef BYTE INFOTYPE;
#endif

#if defined(__NT__)
//#if defined(__ALPHA__)
//    #pragma pack(1);
//#else
    #pragma pack(2);
//#endif
#else
#pragma pack(1);
#endif

typedef struct {
long    dtStyle;
#ifdef __NT__
DWORD   dtExtendedStyle;
WORD    dtItemCount;
#else
BYTE    dtItemCount;
#endif
short   dtX;
short   dtY;
short   dtCX;
short   dtCY;
//char  dtMenuName[];
//char  dtClassName[];
//char  dtCaptionText[];
} _DLGTEMPLATE;

typedef struct {
short   PointSize;
//char  szTypeFace[];
} FONTINFO;

typedef struct {
#ifdef __NT__
long    dtilStyle;
DWORD   dtExtendedStyle;
#endif
short   dtilX;
short   dtilY;
short   dtilCX;
short   dtilCY;
short   dtilID;
#ifndef __NT__
long    dtilStyle;
#endif

//char  dtilClass[];
//char  dtilText[];
//BYTE  dtilInfo;
//BYTE  dtilData;
} _DLGITEMTEMPLATE;
#if defined(__NT__)
#pragma pack(1);
#else
#pragma pack();
#endif

extern GLOBALHANDLE DialogTemplate( LONG dtStyle, int dtx, int dty,
                       int dtcx, int dtcy, char *menuname, char *classname,
                       char *captiontext, int pointsize, char *typeface );
extern void DoneAddingControls( GLOBALHANDLE data );
extern GLOBALHANDLE AddControl ( GLOBALHANDLE data, int dtilx, int dtily,
                   int dtilcx, int dtilcy, int id, long style, char *class,
                   char *text, BYTE infolen, char *infodata );
int DynamicDialogBox( LPVOID fn, HANDLE inst, HWND hwnd, GLOBALHANDLE data );

#endif
