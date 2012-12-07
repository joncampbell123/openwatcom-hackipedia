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


#include "sample.h"
#include "smpstuff.h"
#include "sampwin.h"

volatile WORD           near WaitForFirst=0;
volatile WORD           near IsSecondOK=0;
HWND                    near MainWindowHandle;
HWND                    near OutputWindow;
HANDLE                  InstanceHandle,PrevInstance;
samp_save               far * near SampSave;
shared_data             far * near SharedMemory;
WORD                    SleepTime;
DWORD                   TotalTime;
HANDLE                  SampledProg;
BOOL                    WDebug386;
interrupt_struct        near IntData;
seg_offset              CommonAddr = { 0, 0 };
HANDLE                  WaitForInt3;
HANDLE                  WaitForInt1;
