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
* Description:  Icon/cursor group structures. Documented on MSDN but not
*               declared in any public header.
*
****************************************************************************/


#ifndef WRICON_INCLUDED
#define WRICON_INCLUDED

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

#pragma pack (1);

typedef struct ICONDIRENTRY {
    BYTE        bWidth;
    BYTE        bHeight;
    BYTE        bColorCount;
    BYTE        bReserved;
    WORD        wPlanes;
    WORD        wBitCount;
    DWORD       dwBytesInRes;
    DWORD       dwImageOffset;
} ICONDIRENTRY;

typedef struct ICONHEADER {
    WORD                idReserved;
    WORD                idType;
    WORD                idCount;
    ICONDIRENTRY        idEntries[1];
} ICONHEADER;

typedef struct CURSORDIRENTRY {
    BYTE        bWidth;
    BYTE        bHeight;
    BYTE        bColorCount;
    BYTE        bReserved;
    WORD        wXHotspot;
    WORD        wYHotspot;
    DWORD       dwBytesInRes;
    DWORD       dwImageOffset;
} CURSORDIRENTRY;

typedef struct CURSORHEADER {
    WORD                cdReserved;
    WORD                cdType;
    WORD                cdCount;
    CURSORDIRENTRY      cdEntries[1];
} CURSORHEADER;

typedef struct CURSORHOTSPOT {
    WORD        xHotspot;
    WORD        yHotspot;
} CURSORHOTSPOT;

typedef struct RESICONDIRENTRY {
    BYTE        bWidth;
    BYTE        bHeight;
    BYTE        bColorCount;
    BYTE        bReserved;
    WORD        wPlanes;
    WORD        wBitCount;
    DWORD       lBytesInRes;
    WORD        wNameOrdinal;
//    WORD      pad;
} RESICONDIRENTRY;

typedef struct RESICONHEADER {
    WORD                wReserved;
    WORD                wType;
    WORD                cwCount;
//    WORD              pad;
    RESICONDIRENTRY     idEntries[1];
} RESICONHEADER;

typedef struct RESCURSORDIRENTRY {
    WORD        bWidth;
    WORD        bHeight;
    WORD        wPlanes;
    WORD        wBitCount;
    DWORD       lBytesInRes;
    WORD        wNameOrdinal;
//    WORD      pad;
} RESCURSORDIRENTRY;

typedef struct RESCURSORHEADER {
    WORD                wReserved;
    WORD                wType;
    WORD                cwCount;
//    WORD              pad;
    RESCURSORDIRENTRY   cdEntries[1];
} RESCURSORHEADER;

#pragma pack();

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern uint_32  WR_EXPORT WRSizeOfImage         ( BITMAPINFOHEADER *bih );
extern WORD     WR_EXPORT WRCountIconImages     ( BYTE *data, uint_32 size );
extern int      WR_EXPORT WRCreateIconHeader    ( BYTE *data, uint_32 size,
                                                  WORD type, ICONHEADER **ih,
                                                  uint_32 *ihsize );
extern int      WR_EXPORT WRCreateCursorResHeader( RESCURSORHEADER **rch,
                                                  uint_32 *rchsize,
                                                  BYTE *data,
                                                  uint_32 data_size );
extern int      WR_EXPORT WRCreateIconResHeader ( RESICONHEADER **rih,
                                                  uint_32 *rihsize,
                                                  BYTE *data,
                                                  uint_32 data_size );
extern int      WR_EXPORT WRAddCursorHotspot    ( BYTE **cursor,
                                                  uint_32 *size,
                                                  CURSORHOTSPOT *hs );
extern int      WR_EXPORT WRGetAndAddCursorImage( BYTE *data, WResDir dir,
                                                  CURSORDIRENTRY *cd,
                                                  int ord );
extern int      WR_EXPORT WRGetAndAddIconImage  ( BYTE *data, WResDir dir,
                                                  ICONDIRENTRY *id, int ord );
extern int      WR_EXPORT WRFindImageId         ( WRInfo *info,
                                                  WResTypeNode **otnode,
                                                  WResResNode **ornode,
                                                  WResLangNode **lnode,
                                                  uint_16 type, uint_16 id,
                                                  WResLangType *ltype );

extern int      WR_EXPORT WRAppendDataToData    ( BYTE **d1, uint_32 *d1size, BYTE *d2, uint_32 d2size );
extern int      WR_EXPORT WRAddCursorImageToData( WRInfo *info, WResLangNode*lnode, BYTE **data, uint_32 *size, CURSORHOTSPOT *hotspot );
extern int      WR_EXPORT WRAddIconImageToData  ( WRInfo *info, WResLangNode *lnode, BYTE **data, uint_32 *size );
extern int      WR_EXPORT WRCreateCursorData    ( WRInfo *info, WResLangNode *lnode, BYTE **data, uint_32 *size );
extern int      WR_EXPORT WRCreateIconData      ( WRInfo *info, WResLangNode *lnode, BYTE **data, uint_32 *size );
extern uint_16  WR_EXPORT WRFindUnusedImageId   ( WRInfo *info, uint_16 start );
extern int      WR_EXPORT WRCreateCursorEntries ( WRInfo *info,
                                                  WResLangNode *lnode,
                                                  void *data, uint_32 size );
extern int      WR_EXPORT WRCreateIconEntries   ( WRInfo *info,
                                                  WResLangNode *lnode,
                                                  void *data, uint_32 size );
extern int      WR_EXPORT WRDeleteGroupImages   ( WRInfo *info, WResLangNode *lnode, uint_16 type );

#endif
