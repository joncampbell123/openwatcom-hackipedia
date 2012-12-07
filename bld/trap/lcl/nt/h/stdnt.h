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
* Description:  NT trap dll internal interfaces
*
****************************************************************************/


//#define WOW
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <tlhelp32.h>
#include "trpimp.h"
#include "packet.h"
#include "exepe.h"
#include "exeos2.h"
#include "exedos.h"

#ifndef STATUS_SEGMENT_NOTIFICATION
#define STATUS_SEGMENT_NOTIFICATION 0x40000005
#endif
#ifdef WOW
#include "vdmdbg.h"
#endif

#define EXE_PE  PE_SIGNATURE
#define EXE_NE  OS2_SIGNATURE_WORD
#define EXE_MZ  DOS_SIGNATURE

typedef struct {
    WORD                sig;
    union {
        pe_header       peh;
        os2_exe_header  neh;
    };
    char                modname[16];
} header_info;

#ifdef WOW
typedef struct {
    WORD        offset;
    WORD        segment;
    DWORD       tid;
    WORD        htask;
    DWORD       hmodule;
    char        modname[MAX_MODULE_NAME+1];
} wow_info;
#endif

#define COND_VDM_START                  0x80000000

#define STATE_NONE                      0x00000000
#define STATE_WATCH                     0x00000001
#define STATE_WATCH_386                 0x00000002
#define STATE_EXPECTING_FAULT           0x00000004
#define STATE_IGNORE_DEAD_THREAD        0x00000008
#define STATE_WAIT_FOR_VDM_START        0x00000010
#define STATE_IGNORE_DEBUG_OUT          0x00000020

#if defined(__386__)

    #define TRACE_BIT   0x100
    #define BRK_POINT   0xcc
    typedef unsigned_8  brkpnt_type;

    extern void BreakPoint( void );
    #pragma aux BreakPoint = BRK_POINT;

    extern WORD DS( void );
    #pragma aux DS = \
            "mov        ax,ds" \
            value[ax];

    extern WORD CS( void );
    #pragma aux CS = \
            "mov        ax,cs" \
            value[ax];

#elif defined(__ALPHA__)

    #define BRK_POINT   0x00000080
    typedef unsigned_32 brkpnt_type;

    #define DS()        0
    #define CS()        0

#elif defined(__PPC__)

    #define TRACE_BIT   (1UL << MSR_L_se)
    #define BRK_POINT   0x00000000
    typedef unsigned_32 brkpnt_type;

    #define DS()        0
    #define CS()        0

#else

    #error stdnt.h not configured for machine

#endif

typedef struct thread_info {
    struct thread_info  *next;
    DWORD               tid;
    HANDLE              thread_handle;
    LPVOID              start_addr;
    addr_off            brk_addr;
    brkpnt_type         brk_opcode;
    char                alive:1;
    char                suspended:1;
    char                is_wow:1;
    char                is_dos:1;
    char                is_foreign:1;
} thread_info;

typedef struct process_info {
    thread_info         *thread_list;
    DWORD               pid;
    HANDLE              __unused_field;
    HANDLE              process_handle;
    LPVOID              base_addr;
} process_info;

typedef enum {
    SS_UNKNOWN          = 0x0000,
    SS_NATIVE           = 0x0001,
    SS_WINDOWS_GUI      = 0x0002,
    SS_WINDOWS_CHAR     = 0x0003,
    SS_OS2_CHAR         = 0x0005,
    SS_POSIX_CHAR       = 0x0007
} subsystems;

typedef struct msg_list msg_list;

struct msg_list {
    msg_list    *next;
    char        msg[1]; /* variable size */
};


/*
 * global variables
 */

#define TRPGLOBAL extern
#define TRPGLOBINIT( x )
#include "globals.h"

/*
 * function prototypes
 */

/* accmap.c */
BOOL FindExceptInfo( addr_off off, LPVOID *base, addr_off *size );
void FixUpDLLNames( void );
void RemoveModuleFromLibList( char *module, char *filename );
BOOL IsMagicalFileHandle( HANDLE h );
HANDLE GetMagicalFileHandle( char *name );
void AddProcess( header_info * );
void DelProcess( BOOL );
void VoidProcess( void );
#ifndef WOW
typedef void IMAGE_NOTE;
#endif
void AddLib( BOOL, IMAGE_NOTE *im );
void DelLib( void );
void FreeLibList( void );
int DoListLibs( char *buff, int is_first, int want_16, int want_32,
                                        int verbose, int sel );

/* accmem.c */
DWORD WriteMem( WORD seg, DWORD base, LPVOID buff, DWORD size );
DWORD ReadMem( WORD seg, DWORD base, LPVOID buff, DWORD size );

/* accmisc.c */
BOOL IsBigSel( WORD sel );
int FindFilePath( char *pgm, char *buffer, char *ext_list );
void AddMessagePrefix( char *buff, int len );

/* accrun.c */
int DebugExecute( DWORD state, int *tsc, bool );
void InterruptProgram( void );
void InterruptProgram( void );
bool Terminate( void );

/* misc.c */
BOOL MyGetThreadContext( thread_info *ti, PCONTEXT pc );
#define GetThreadContext Dont_call_GetThreadContext_directly__Call_MyGetThreadContext_instead
BOOL MySetThreadContext( thread_info *ti, PCONTEXT pc );
#define SetThreadContext Dont_call_SetThreadContext_directly__Call_MySetThreadContext_instead

/* peread.c */
BOOL SeekRead( HANDLE handle, DWORD newpos, void *buff, WORD size );
int GetEXEHeader( HANDLE handle, header_info *hi, WORD *stack );
int GetModuleName( HANDLE fhdl, char *name );

/* pgmexec.c */
BOOL CausePgmToLoadThisDLL( void );
BOOL TaskReadWord( WORD seg, DWORD off, LPWORD data );
BOOL TaskWriteWord( WORD seg, DWORD off, WORD data );
BOOL TaskReadFPU( LPVOID data );
BOOL TaskWriteFPU( LPVOID data );
void TaskDoExit( void );
HANDLE TaskGetModuleHandle( char *name );

/* thread.c */
void AddThread( DWORD tid, HANDLE th, LPVOID sa );
void DeadThread( DWORD tid );
thread_info *FindThread( DWORD tid );
void RemoveThread( DWORD tid );
void RemoveAllThreads( void );

/* accbrwat.c */
BOOL CheckWatchPoints( void );
BOOL SetDebugRegs( void );
void ClearDebugRegs( void );
DWORD GetDR6( void );
void SetDR7( DWORD tmp );
BOOL FindBreak( WORD segment, DWORD offset, BYTE *ch );

/* dbgthrd.c */
extern DWORD StartControlThread( char *name, DWORD *pid, DWORD cr_flags );
BOOL MyWaitForDebugEvent( void );
void MyContinueDebugEvent( int );
extern void StopControlThread( void );
extern void ProcessQueuedRepaints( void );
extern void ParseServiceStuff( char *name,
    char **pdll_name, char **pservice_name,
    char **pdll_destination, char **pservice_parm );

/* accregs.c */
extern DWORD AdjustIP( CONTEXT *, int );
extern void SetIP( CONTEXT *, DWORD );

extern void say( char *fmt, ... );
