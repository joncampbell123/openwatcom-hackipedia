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
* Description:  Interface for RDOS debug class
*
****************************************************************************/

#ifndef _DEBUG_H
#define _DEBUG_H

struct TDebug *GetCurrentDebug();
void SetCurrentDebug( struct TDebug *obj );

// TDebugBreak data

struct TDebugBreak
{
	int Sel;
	long Offset;
    char Instr;
    struct TDebugBreak *Next;
};

// TDebugThread data

struct TDebugThread
{
    char *FaultText;
    char *ThreadName;
    char *ThreadList;
    int ListOffset;
    short int ListSel;

    int ThreadID;
    unsigned int FsLinear;
    long Eip;
    short int Cs;

    long Esp0;
    long Ess0;
    long Esp1;
    long Ess1;
    long Esp2;
    long Ess2;
    long Cr3;
    long Eflags;
    long Eax;
    long Ecx;
    long Edx;
    long Ebx;
    long Esp;
    long Ebp;
    long Esi;
    long Edi;
    long Es;
    long Ss;
    long Ds;
    long Fs;
    long Gs;
    long Ldt;

    long Dr[4];
    long Dr7;
    long MathControl;
    long MathStatus;
    long MathTag;
    long MathEip;
    short int MathCs;
    char MathOp[2];
    long MathDataOffs;
    long MathDataSel;
    long double St[8];

    struct TDebugThread *Next;

    int FDebug;
	int FHasBreak;
	int FHasTrace;
	int FHasException;
    int FWasTrace;        
};

// TDebugThread methods

int IsDebug( struct TDebugThread *obj );

int ReadMem( struct TDebugThread *obj, int Sel, long Offset, char *Buf, int Size );
int WriteMem( struct TDebugThread *obj, int Sel, long Offset, char *Buf, int Size );
void WriteRegs( struct TDebugThread *obj );

int WasTrace( struct TDebugThread *obj );

int HasBreakOccurred( struct TDebugThread *obj );
int HasTraceOccurred( struct TDebugThread *obj );
int HasFaultOccurred( struct TDebugThread *obj );

// TDebugModule data

struct TDebugModule
{
    char *ModuleName;
    int FileHandle;
    int Handle;
    unsigned int ImageBase;
	unsigned int ImageSize;
	unsigned int ObjectRva;

    int FNew;

    struct TDebugModule *Next;
};

// TDebug data

struct TDebug
{
    char *FProgram;
    char *FParam;
	char *FStartDir;
	int FHandle;

	int FSection;

    struct TDebugThread *CurrentThread;
	struct TDebugThread *ThreadList;
	struct TDebugModule *ModuleList;

    struct TDebugBreak *BreakList;

    int UserSignal;
    int UserWait;

    int FInstalled;
    int FThreadChanged;
    int FModuleChanged;

};

// TDebug methods

void InitDebug( struct TDebug *obj, const char *Program, const char *Param, const char *StartDir );
void FreeDebug( struct TDebug *obj );

struct TDebugThread *GetMainThread( struct TDebug *obj );
struct TDebugModule *GetMainModule( struct TDebug *obj );

int GetNextThread( struct TDebug *obj, int ThreadID );
int GetNextModule( struct TDebug *obj, int Module );
    
struct TDebugThread *GetCurrentThread( struct TDebug *obj );
void SetCurrentThread( struct TDebug *obj, int ThreadID );

struct TDebugThread *LockThread( struct TDebug *obj, int ThreadID );
void UnlockThread( struct TDebug *obj );

struct TDebugModule *LockModule( struct TDebug *obj, int Handle );
void UnlockModule( struct TDebug *obj );

void AddBreak( struct TDebug *obj, int Sel, long Offset );
void ClearBreak( struct TDebug *obj, int Sel, long Offset );

void WaitForLoad( struct TDebug *obj );
void Go( struct TDebug *obj );
void Trace( struct TDebug *obj );

int HasThreadChange( struct TDebug *obj );
void ClearThreadChange( struct TDebug *obj );

int HasModuleChange( struct TDebug *obj );
void ClearModuleChange( struct TDebug *obj );

int IsTerminated( struct TDebug *obj );

#endif
