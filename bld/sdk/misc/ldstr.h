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
* Description:  Prototypes for resource strings helper routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>


/************************************************************************
 * SetInstance - must be called before any other routine in this file
 *
 * If SPECIAL_STRING_LOADING is defined this first group of functions
 * must be provided by the application.  This allows an app to have
 * its own buffering scheme for resource strings.
 ***********************************************************************/
void SetInstance( HANDLE inst );
char *AllocRCString( DWORD id );
void FreeRCString( char *str );
char *GetRCString( DWORD msgid );
DWORD CopyRCString( DWORD id, char *buf, DWORD bufsize );

/*
 * All functions below this point obtain strings using AllocRCString and
 * FreeRCString
 */

int  RCMessageBox( HWND hwnd , DWORD msgid, char *title, UINT type );
void RCfprintf( FILE *fp, DWORD strid, ...  );
int  RCsprintf( char *buf, DWORD fmtid, ... );
void RCvfprintf( FILE *fp, DWORD strid, va_list al  );
