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


typedef enum {
        NO,
        NYI,
        BIN1,
        BIN2,
        BIN4,
        BIN8,
        UBIN1,
        UBIN2,
        UBIN4,
        NBIN1,
        NBIN2,
        NBIN4,
        UN1,
        UN2,
        UN4,
        UN8,
        MOV1,
        MOV2,
        MOV4,
        MOV8,
        MOVXX,
        CONV,
        FCONV,
        CALL,
        CALLI,
        PUSH,
        POP,
        LA2,
        LA4,
        LA8,
        CMP4,
        CMP8,
        TEST4,
        TEST8,
        SET4,
        SET8,
        ZAP,
        EXT4,
        FBINS,
        FBIND,
        MOVFS,
        MOVFD,
        CMPFS,
        CMPFD,
        RTN,
        NEGF,
        PROM,
        VASTR,
        MOD4,
        DONOTHING,
        BAD,
        LAST
} table_def;
