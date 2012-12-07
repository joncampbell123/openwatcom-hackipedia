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
* Description:  prototypes for various non-public directory functions
*
****************************************************************************/


#include "widechar.h"

#define _DIR_ISFIRST            0
#define _DIR_NOTFIRST           1
#define _DIR_INVALID            2
#define _DIR_CLOSED             3

#ifdef __WIDECHAR__
typedef wchar_t     UINT_WC_TYPE;
#else
typedef unsigned    UINT_WC_TYPE;
#endif

extern char *__tmpdir( char *__buff );
#ifdef __WIDECHAR__
_WCRTLINK extern DIR_TYPE *_w_opendir( const wchar_t *__name, unsigned __attr, DIR_TYPE *__dirp );
_WCRTLINK extern DIR_TYPE *_w__opendir( const wchar_t *__name, unsigned __attr, DIR_TYPE *__dirp );
#else
_WCRTLINK extern DIR_TYPE *_opendir( const char *__name, unsigned __attr, DIR_TYPE *__dirp );
_WCRTLINK extern DIR_TYPE *__opendir( const char *__name, unsigned __attr, DIR_TYPE *__dirp );
#endif
