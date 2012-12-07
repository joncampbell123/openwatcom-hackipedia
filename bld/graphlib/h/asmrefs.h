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
* Description:  Assembler routines called by C.
*
****************************************************************************/


/* To make the 386 graphics library, the two libraries, graph3r and
   graph3s, are combined. Only one copy of the assembler modules is
   placed in the combined library. For this to work properly, all
   references to symbol names must be done the same way. */

#pragma aux _CoRep "*_";
#pragma aux _CoXor "*_";
#pragma aux _CoAnd "*_";
#pragma aux _CoOr "*_";
#pragma aux _MoveUp "*_";
#pragma aux _MoveDown "*_";
#pragma aux _Move1Left "*_";
#pragma aux _Move2Left "*_";
#pragma aux _Move1Right "*_";
#pragma aux _Move2Right "*_";
#pragma aux _Get1Dot "*_";
#pragma aux _Get2Dot "*_";
#pragma aux _Pix1Zap "*_";
#pragma aux _Pix2Zap "*_";
#pragma aux _Pix1Fill "*_";
#pragma aux _Pix2Fill "*_";
#pragma aux _Pix1Copy "*_";
#pragma aux _Pix2Copy "*_";
#pragma aux _Pix1Read "*_";
#pragma aux _Pix2Read "*_";
#pragma aux _CGAScanLeft "*_";
#pragma aux _CGAScan1Right "*_";
#pragma aux _CGAScan2Right "*_";
#pragma aux _EGAMoveUpHi "*_";
#pragma aux _EGAMoveUpLo "*_";
#pragma aux _EGAMoveDownHi "*_";
#pragma aux _EGAMoveDownLo "*_";
#pragma aux _EGAMoveLeft "*_";
#pragma aux _EGAMoveRight "*_";
#pragma aux _EGARep "*_";
#pragma aux _EGAGetDot "*_";
#pragma aux _EGAGetDotEO "*_";
#pragma aux _EGAGetDotMono "*_";
#pragma aux _EGAZap "*_";
#pragma aux _EGAZapEO "*_";
#pragma aux _EGAZapMono "*_";
#pragma aux _EGAFill "*_";
#pragma aux _EGAFillEO "*_";
#pragma aux _EGAFillMono "*_";
#pragma aux _EGAPixCopy "*_";
#pragma aux _EGAPixCopyEO "*_";
#pragma aux _EGAPixCopyMono "*_";
#pragma aux _EGAReadRow "*_";
#pragma aux _EGAReadRowEO "*_";
#pragma aux _EGAReadRowMono "*_";
#pragma aux _EGAScanLeft "*_";
#pragma aux _EGAScanLeftEO "*_";
#pragma aux _EGAScanLeftMono "*_";
#pragma aux _EGAScanRight "*_";
#pragma aux _EGAScanRightEO "*_";
#pragma aux _EGAScanRightMono "*_";
#pragma aux _HercMoveUp "*_";
#pragma aux _HercMoveDown "*_";
#pragma aux _MoveUp19 "*_";
#pragma aux _MoveDown19 "*_";
#pragma aux _MoveLeft19 "*_";
#pragma aux _MoveRight19 "*_";
#pragma aux _Rep19 "*_";
#pragma aux _And19 "*_";
#pragma aux _Zap19 "*_";
#pragma aux _GetDot19 "*_";
#pragma aux _Fill19 "*_";
#pragma aux _PixCopy19 "*_";
#pragma aux _PixRead19 "*_";
#pragma aux _ScanLeft19 "*_";
#pragma aux _ScanRight19 "*_";
#pragma aux _MoveUp100 "*_";
#pragma aux _MoveDown100 "*_";
#pragma aux _MoveLeft256 "*_";
#pragma aux _MoveRight256 "*_";
#pragma aux _MoveUp640 "*_";
#pragma aux _MoveDown640 "*_";
#pragma aux _MoveUp800 "*_";
#pragma aux _MoveDown800 "*_";
#pragma aux _MoveDown1024 "*_";
#pragma aux _MoveDown128 "*_";
#pragma aux _MoveUp1024 "*_";
#pragma aux _MoveUp128 "*_";
#pragma aux _Zap256 "*_";
#pragma aux _Fill256 "*_";
#pragma aux _PixCopy256 "*_";
#pragma aux _PixRead256 "*_";
#pragma aux _ScanLeft256 "*_";
#pragma aux _ScanRight256 "*_";
#if 0
// This mode is untested
#pragma aux _MoveUp1280 "*_";
#pragma aux _MoveDown1280 "*_";
#pragma aux _MoveRightWord "*_";
#pragma aux _MoveLeftWord "*_";
#pragma aux _RepWord "*_";
#pragma aux _AndWord "*_";
#pragma aux _XorWord "*_";
#pragma aux _OrWord "*_";
#pragma aux _GetDotWord "*_";
#pragma aux _ZapWord "*_";
#pragma aux _FillWord "*_";
#pragma aux _PixCopyWord "*_";
#pragma aux _PixReadWord "*_";
#pragma aux _ScanLeftWord "*_";
#pragma aux _ScanRightWord "*_";
#endif
#pragma aux _PageVESA "*_";
#pragma aux _PageVideo7 "*_";
#pragma aux _PageParadise "*_";
#pragma aux _PageATI "*_";
#pragma aux _PageTseng3 "*_";
#pragma aux _PageTseng4 "*_";
#pragma aux _PageOak "*_";
#pragma aux _PageTrident "*_";
#pragma aux _PageChips "*_";
#pragma aux _PageGenoa "*_";
#pragma aux _PageS3 "*_";
#pragma aux _PageCirrus "*_";
#pragma aux _PageViper "*_";

/* Variables */

#pragma aux _StartUp "_*";
#pragma aux _GrMode "_*";
#pragma aux _ErrorStatus "_*";
#pragma aux _CurrPos "_*";
#pragma aux _LogOrg "_*";
#pragma aux _Clipping "_*";
#pragma aux _Tx_Row_Min "_*";
#pragma aux _Tx_Col_Min "_*";
#pragma aux _Tx_Row_Max "_*";
#pragma aux _Tx_Col_Max "_*";
#pragma aux _TextPos "_*";
#pragma aux _CharAttr "_*";
#pragma aux _Wrap "_*";
#pragma aux _CursState "_*";
#pragma aux _CursorShape "_*";
#pragma aux _GrCursor "_*";
#pragma aux _CurrActivePage "_*";
#pragma aux _CurrVisualPage "_*";
#pragma aux _DefMode "_*";
#pragma aux _DefTextRows "_*";
#pragma aux _CurrBkColor "_*";
#pragma aux _CurrColor "_*";
#pragma aux _Palette "_*";
#pragma aux _FillMask "_*";
#pragma aux _DefMask "_*";
#pragma aux _HaveMask "_*";
#pragma aux _Transparent "_*";
#pragma aux _PaRf_x "_*";
#pragma aux _PaRf_y "_*";
#pragma aux _LineStyle "_*";
#pragma aux _StyleWrap "_*";
#pragma aux _PlotAct "_*";
#pragma aux _TextSettings "_*";
#pragma aux _StdFont "_*";
#pragma aux _8x8Font "_*";
#pragma aux _8x8BitMap "_*";
#pragma aux _HercFont "_*";
#pragma aux _CurrFont "_*";
#pragma aux _ArcInfo "_*";
#pragma aux _CurrPos_w "_*";
#pragma aux _Window "_*";
#pragma aux _Screen "_*";
#pragma aux _IsDBCS "_*";
#pragma aux _DBCSPairs "_*";
#pragma aux _CurrState "_*";
#pragma aux _ConfigBuffer "_*";
#pragma aux _StackSeg "_*";
#pragma aux _BiosSeg "_*";
#pragma aux _BiosOff "_*";

#pragma aux _MonoSeg "_*";
#pragma aux _CgaSeg "_*";
#pragma aux _EgaSeg "_*";
#pragma aux _RomSeg "_*";
#pragma aux _MonoOff "_*";
#pragma aux _CgaOff "_*";
#pragma aux _EgaOff "_*";
#pragma aux _RomOff "_*";

#if defined( __QNX__ )
#pragma aux _CompileSeg "_*";
#pragma aux _ShadowSeg "_*";
#endif

#if defined( _SUPERVGA )
#pragma aux _VGAPage "_*";
#pragma aux _VGAGran "_*";
#pragma aux _SVGAType "_*";
#pragma aux _SetVGAPage "_*";
#pragma aux _VGAStride "_*";
#endif

// generic, CGA or Hecules
extern void pascal      near _MoveUp( void );
extern void pascal      near _Move1Left( void );
extern void pascal      near _Move2Left( void );
extern void pascal      near _MoveDown( void );
extern void pascal      near _Move1Right( void );
extern void pascal      near _Move2Right( void );
extern void pascal      near _HercMoveUp( void );
extern void pascal      near _HercMoveDown( void );

extern void             near _CoRep( char far *, int, int );
extern void             near _CoXor( char far *, int, int );
extern void             near _CoAnd( char far *, int, int );
extern void             near _CoOr( char far *, int, int );
extern short            near _Get1Dot( char far *, int, int );
extern short            near _Get2Dot( char far *, int, int );
extern void             near _Pix1Zap( char far *, int, int, int, int );
extern void             near _Pix2Zap( char far *, int, int, int, int );
extern void             near _Pix1Fill( char far *, int, int, int, int );
extern void             near _Pix2Fill( char far *, int, int, int, int );
extern void             near _Pix1Copy( char far *, char far *, int, int, int );
extern void             near _Pix2Copy( char far *, char far *, int, int, int );
extern void             near _Pix1Read( char far *, char far *, int, int, int );
extern void             near _Pix2Read( char far *, char far *, int, int, int );
extern short            near _CGAScanLeft( char far *, int, int, int, int, int );
extern short            near _CGAScan1Right( char far *, int, int, int, int, int );
extern short            near _CGAScan2Right( char far *, int, int, int, int, int );

// EGA
extern void pascal      near _EGAMoveLeft( void );
extern void pascal      near _EGAMoveRight( void );
extern void pascal      near _EGAMoveUpLo( void );
extern void pascal      near _EGAMoveUpHi( void );
extern void pascal      near _EGAMoveDownLo( void );
extern void pascal      near _EGAMoveDownHi( void );
extern void             near _EGARep( char far *, int, int );
extern short            near _EGAGetDot( char far *, int, int );
extern short            near _EGAGetDotEO( char far *, int, int );
extern short            near _EGAGetDotMono( char far *, int, int );
extern void             near _EGAZap( char far *, int, int, int, int );
extern void             near _EGAZapEO( char far *, int, int, int, int );
extern void             near _EGAZapMono( char far *, int, int, int, int );
extern void             near _EGAFill( char far *, int, int, int, int );
extern void             near _EGAFillEO( char far *, int, int, int, int );
extern void             near _EGAFillMono( char far *, int, int, int, int );
extern void             near _EGAPixCopy( char far *, char far *, int, int, int );
extern void             near _EGAPixCopyEO( char far *, char far *, int, int, int );
extern void             near _EGAPixCopyMono( char far *, char far *, int, int, int );
extern void             near _EGAReadRow( char far *, char far *, int, int, int );
extern void             near _EGAReadRowEO( char far *, char far *, int, int, int );
extern void             near _EGAReadRowMono( char far *, char far *, int, int, int );
extern short            near _EGAScanLeft( char far *, int, int, int, int, int );
extern short            near _EGAScanLeftEO( char far *, int, int, int, int, int );
extern short            near _EGAScanLeftMono( char far *, int, int, int, int, int );
extern short            near _EGAScanRight( char far *, int, int, int, int, int );
extern short            near _EGAScanRightEO( char far *, int, int, int, int, int );
extern short            near _EGAScanRightMono( char far *, int, int, int, int, int );

// SVGA
extern void pascal      near _MoveUp19( void );
extern void pascal      near _MoveUp100( void );
extern void pascal      near _MoveUp128( void );
extern void pascal      near _MoveUp640( void );
extern void pascal      near _MoveUp800( void );
extern void pascal      near _MoveUp1024( void );
extern void pascal      near _MoveLeft19( void );
extern void pascal      near _MoveLeft256( void );
extern void pascal      near _MoveDown19( void );
extern void pascal      near _MoveDown100( void );
extern void pascal      near _MoveDown128( void );
extern void pascal      near _MoveDown640( void );
extern void pascal      near _MoveDown800( void );
extern void pascal      near _MoveDown1024( void );
extern void pascal      near _MoveRight19( void );
extern void pascal      near _MoveRight256( void );
extern void             near _Rep19( char far *, int, int );
extern void             near _And19( char far *, int, int );
extern short            near _GetDot19( char far *, int, int );
extern void             near _Zap19( char far *, int, int, int, int );
extern void             near _Zap256( char far *, int, int, int, int );
extern void             near _Fill19( char far *, int, int, int, int );
extern void             near _Fill256( char far *, int, int, int, int );
extern void             near _PixCopy19( char far *, char far *, int, int, int );
extern void             near _PixCopy256( char far *, char far *, int, int, int );
extern void             near _PixRead19( char far *, char far *, int, int, int );
extern void             near _PixRead256( char far *, char far *, int, int, int );
extern short            near _ScanLeft256( char far *, int, int, int, int, int );
extern short            near _ScanLeft19( char far *, int, int, int, int, int );
extern short            near _ScanRight19( char far *, int, int, int, int, int );
extern short            near _ScanRight256( char far *, int, int, int, int, int );
#if 0 // This mode is untested
extern void             near _MoveUp1280( void );
extern void             near _MoveDown1280( void );
extern void             near _MoveLeftWord( void );
extern void             near _MoveRightWord( void );
extern void             near _RepWord( char far *, int, int );
extern void             near _XorWord( char far *, int, int );
extern void             near _AndWord( char far *, int, int );
extern void             near _OrWord( char far *, int, int );
extern void             near _GetDotWord( char far *, int, int );
extern void             near _ZapWord( char far *, int, int, int, int );
extern void             near _FillWord( char far *, int, int, int, int );
extern void             near _PixCopyWord( char far *, char far *, int, int, int );
extern void             near _PixReadWord( char far *, char far *, int, int, int );
extern void             near _ScanLeftWord( char far *, int, int, int, int, int );
extern void             near _ScanRightWord( char far *, int, int, int, int, int );
#endif

extern void             _FARC _PageVESA( short );
extern void             _FARC _PageVideo7( short );
extern void             _FARC _PageParadise( short );
extern void             _FARC _PageATI( short );
extern void             _FARC _PageTseng3( short );
extern void             _FARC _PageTseng4( short );
extern void             _FARC _PageOak( short );
extern void             _FARC _PageTrident( short );
extern void             _FARC _PageChips( short );
extern void             _FARC _PageGenoa( short );
extern void             _FARC _PageS3( short );
extern void             _FARC _PageCirrus( short );
extern void             _FARC _PageViper( short );
