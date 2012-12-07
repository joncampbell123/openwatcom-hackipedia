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
* Description:  Run and compile-time constants indicating parameter type
*               and attributes
*
****************************************************************************/

/**************** IMPORTANT NOTE *******************************

  data in files:  ptypes.inc, ptypes.h and ptypdefn.h 
  
  must corespond each to other

****************************************************************/

//
// Run and compile-time consts indicating parameter type and attributes
// NOTE: These constants should stay in the following order.
//       ( routines rely on them being sequential )

//    id              type          dbgtype       cgtype
pick( PT_NOTYPE,      TY_NO_TYPE,   DBG_NIL_TYPE, T_USER_DEFINED  , ""  ) // type of statement #'s and subroutine return values
pick( PT_LOG_1,       TY_LOGICAL_1, DBG_NIL_TYPE, T_UINT_1        , "LOGICAL*1" ) // LOGICAL*1
pick( PT_LOG_4,       TY_LOGICAL,   DBG_NIL_TYPE, T_UINT_4        , "LOGICAL*4" ) // LOGICAL*4
pick( PT_INT_1,       TY_INTEGER_1, DBG_NIL_TYPE, T_INT_1         , "INTEGER*1" ) // INTEGER*1
pick( PT_INT_2,       TY_INTEGER_2, DBG_NIL_TYPE, T_INT_2         , "INTEGER*2" ) // INTEGER*2
pick( PT_INT_4,       TY_INTEGER,   DBG_NIL_TYPE, T_INT_4         , "INTEGER*4" ) // INTEGER*4
pick( PT_REAL_4,      TY_REAL,      DBG_NIL_TYPE, T_SINGLE        , "REAL*4"    ) // REAL*4
pick( PT_REAL_8,      TY_DOUBLE,    DBG_NIL_TYPE, T_DOUBLE        , "REAL*8"    ) // REAL*8
pick( PT_REAL_16,     TY_EXTENDED,  DBG_NIL_TYPE, T_LONGDOUBLE    , "REAL*16"   ) // REAL*16
pick( PT_CPLX_8,      TY_COMPLEX,   DBG_NIL_TYPE, T_COMPLEX       , "COMPLEX*8" ) // COMPLEX*8
pick( PT_CPLX_16,     TY_DCOMPLEX,  DBG_NIL_TYPE, T_DCOMPLEX      , "COMPLEX*16") // COMPLEX*16
pick( PT_CPLX_32,     TY_XCOMPLEX,  DBG_NIL_TYPE, T_XCOMPLEX      , "COMPLEX*32") // COMPLEX*32
pick( PT_CHAR,        TY_CHAR,      DBG_NIL_TYPE, T_LOCAL_POINTER , "CHARACTER" ) // CHARACTER
pick( PT_STRUCT,      TY_STRUCTURE, DBG_NIL_TYPE, T_USER_DEFINED  , "STRUCTURE" ) // STRUCTURE
pick( PT_ARRAY,       TY_NO_TYPE,   DBG_NIL_TYPE, T_USER_DEFINED  , "ARRAY"     ) // ARRAY
pick( PT_END_DATASET, TY_NO_TYPE,   DBG_NIL_TYPE, T_USER_DEFINED  , ""          ) // indicates the end of a data set in DATA statement
pick( PT_NOT_STARTED, TY_NO_TYPE,   DBG_NIL_TYPE, T_USER_DEFINED  , ""          ) // IOCB->typ is initialized to this for i/o statements
                                                                                  // so that the i/o list may be flushed in halt when an
                                                                                  // error occurs
