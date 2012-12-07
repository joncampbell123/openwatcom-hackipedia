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
* Description:  Varoius environment checks for the DOS debugger.
*
****************************************************************************/


extern byte EnhancedWinCheck( void );
#pragma aux EnhancedWinCheck =     \
        "mov    ax, 1600H"         \
        "int    2fH"               \
        value [al];

extern unsigned DPMIVersion( void );
#pragma aux DPMIVersion =          \
        "       mov     ax,1687h"  \
        "       int     2fh"       \
        "       test    ax,ax"     \
        "       je      l1"        \
        "       xor     dx,dx"     \
        "l1:    "                  \
        value [dx] modify [ ax bx cx dx si es di ]

const char DOSEMUString[] = "$DOSEMU$";

extern int DOSEMUCheck( void );
#pragma aux DOSEMUCheck =          \
        "       push   ds"         \
        "       mov    ax, 0f000h" \
        "       mov    es, ax"     \
        "       mov    di, 0ffe0h" \
        "       mov    ax, seg DOSEMUString" \
        "       mov    ds, ax"     \
        "       mov    si, offset DOSEMUString" \
        "       mov    cx, 4"      \
        "       cld"               \
        "       repe   cmpsw"      \
        "       mov    ax, 0"      \
        "       jne    l1"         \
        "       inc    ax"         \
        "l1:    pop    ds" \
        value [ax] modify [ bx cx dx si es di ]
