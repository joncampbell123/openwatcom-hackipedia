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


#define MSG_RC_BASE     1
#define MSG_USE_BASE    500

#define MSG_LANG_SPACING        1000

#define MSG_USE_E_BASE  (MSG_USE_BASE + RLE_ENGLISH*MSG_LANG_SPACING)
#define MSG_USE_J_BASE  (MSG_USE_BASE + RLE_JAPANESE*MSG_LANG_SPACING)

#include "msg.gh"

#define MAX_RESOURCE_SIZE       128

extern int MsgInit();
extern int MsgGet( int, char * );
extern void MsgPutUsage();
extern void MsgFini();
extern void MsgSubStr( char *, char *, char );
extern void MsgChgeSpec( char *strptr, char specifier );
