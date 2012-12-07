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



#include <sys/types.h>
#include "stdui.h"
#include "uidef.h"
#include "qnxuiext.h"
#include "uivirt.h"

        /* console number */
global unsigned         UIConsole = 0;
        /* filedescriptor */
global unsigned         UIConHandle = 0;
        /* proxy for all events */
global pid_t            UIProxy;
        /* remote proxy if nec.. */
global pid_t            UIRemProxy;
        /* proxy's incoming value (usually same as UIProxy */
global pid_t            UILocalProxy;
        /* Node of console mgr */
global nid_t            UIConNid;
        /* process group */
global pid_t            UIPGroup;
        /* tell keyboard app wants to see shift, alt, ... keys... */
global unsigned         UIWantShiftChanges = TRUE;
        /* Disable checking on non console devices */
global unsigned         UIDisableShiftChanges = FALSE;
        /* anytime is mouse time ?!? */
global unsigned long    MouseTime = 0L;
        /* Active virtual console functions */
global VirtDisplay      UIVirt;
