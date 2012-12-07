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


//
// UNDEFCG      : output routines that don't do undefined checking
//

#include "ftnstd.h"
#include "rundat.h"
#include "target.h"

#include <stdlib.h>
#include <string.h>

extern  void            R_FmtLog(uint);
extern  void            SetMaxPrec(int);
extern  void            F_SendData(char *,uint);
extern  void            R_F2F(extended,char *,int,int,bool,int);
extern  void            R_F2E(extended,char *,int,int,bool,int,int,char);
extern  bool            ToFFormat(extended);
extern  char            *JmpBlanks(char *);
extern  bool            GetReal(extended *);
extern  void            SendStr(char PGM *,int);


static  void    FmtFloat( char *buff, extended value, int digits, char ch ) {
//===========================================================================

    if( ToFFormat( value ) ) {
        R_F2F( value, buff, EXTENDED_IO_WINDOW, digits, FALSE, 0 );
    } else {
        R_F2E( value, buff, EXTENDED_IO_WINDOW, digits, FALSE, 1, EXP_LEN, ch );
    }
    buff[EXTENDED_IO_WINDOW] = NULLCHAR;
    strcpy( buff, JmpBlanks( buff ) );
}


void OutLogCG( void ) {
//===============

    R_FmtLog( INTEGER_IO_WINDOW );
}


void OutIntCG( void ) {
//===============

    ltoa( IORslt.intstar4, IOCB->buffer, 10 );
    F_SendData( IOCB->buffer, INTEGER_IO_WINDOW );
}


void FmtRealCG( char *buffer, single *value ) {
//=============================================

    SetMaxPrec( MAX_SP );
    FmtFloat( buffer, *value, MAX_SP, 'E' );
}


void FmtDoubleCG( char *buffer, double *value ) {
//===============================================

    SetMaxPrec( MAX_DP );
    FmtFloat( buffer, *value, MAX_DP, 'D' );
}


void FmtExtendedCG( char *buffer, extended *value ) {
//===================================================

    SetMaxPrec( MAX_XP );
    FmtFloat( buffer, *value, MAX_XP, 'Q' );
}


bool UndefRealCG( single *value ) {
//=================================

    value = value;
    return( 0 );
}


bool UndefDoubleCG( double *value ) {
//===================================

    value = value;
    return( 0 );
}


bool UndefExtendedCG( extended *value ) {
//=======================================

    value = value;
    return( 0 );
}


bool UndefLogCG( void ) {
//=================

    return( 0 );
}


bool UndefIntCG( uint width ) {
//=============================

    width = width;
    return( 0 );
}


bool GetRealCG( extended *value, uint width ) {
//=============================================

    width = width;
    GetReal( value );
    return( 1 );
}


// statically initialize the vectors for no undefined checking
void    (*FmtRealRtn)(char *,single *)          = FmtRealCG;
void    (*FmtDoubleRtn)(char *,double *)        = FmtDoubleCG;
void    (*FmtExtendedRtn)(char *,extended *)    = FmtExtendedCG;
void    (*SendStrRtn)(char PGM *,int)           = SendStr;
bool    (*UndefRealRtn)(single *)               = UndefRealCG;
bool    (*UndefDoubleRtn)(double *)             = UndefDoubleCG;
bool    (*UndefExtendedRtn)(extended *)         = UndefExtendedCG;
bool    (*UndefLogRtn)(void)                    = UndefLogCG;
bool    (*UndefIntRtn)(uint)                    = UndefIntCG;
bool    (*GetRealRtn)(extended *,uint)          = GetRealCG;
