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

//#define DEBUG_TRAP

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include "trpimp.h"
#include "trperr.h"
#include "wdebug.h"

#include "dos4g.h"
#include "pmode32.h"
#include "dbglib.h"
#include "dos16.h"
#include "dpmi.h"
#include "dbg386.h"
#include "drset.h"
#include "ioports.h"
#include "madregs.h"

#include "exedos.h"
#include "exeos2.h"
#include "exeflat.h"

#include "x86cpu.h"
#include "misc7086.h"
#include "dosredir.h"

TSF32   Proc;
char    Break;

extern  unsigned        ExceptionText( unsigned, char * );

bool                    FakeBreak;
bool                    AtEnd;
unsigned                NumObjects;
struct {
    unsigned_32         size;
    unsigned_32         start;
}                       *ObjInfo;

static unsigned_8       RealNPXType;
#define BUFF_SIZE       256
char                    UtilBuff[BUFF_SIZE];
#define IsDPMI          (_d16info.swmode == 0)

typedef struct watch {
    addr48_ptr          addr;
    dword               value;
    dword               linear;
    short               dregs;
    unsigned short      len;
    dpmi_watch_handle   handle;
    dpmi_watch_handle   handle2;
} watch;

#define MAX_WP 32
watch   WatchPoints[ MAX_WP ];
int     WatchCount;


#ifdef DEBUG_TRAP
#define _DBG2( x ) printf x ; fflush( stdout )
#define _DBG1( x ) printf x ; fflush( stdout )
#define _DBG( x ) printf x ; fflush( stdout )
#else
#define _DBG2( x )
#define _DBG1( x )
#define _DBG( x )
#endif

int SetUsrTask( void )
{
    return( 1 );
}

void SetDbgTask( void )
{
}

static unsigned short ReadWrite( int (*r)(OFFSET32,SELECTOR,int,char far*,unsigned int), addr48_ptr *addr, byte far *data, unsigned short req ) {

    unsigned short  len;

    _DBG(("checking %4.4x:%8.8lx for 0x%x bytes -- ",
            addr->segment, addr->offset, req ));
    if( D32AddressCheck( addr->segment, addr->offset, req, NULL ) &&
            r( addr->offset, addr->segment, 0, data, req ) == 0 ) {
        _DBG(( "OK\n" ));
        addr->offset += req;
        return( req );
    }
    _DBG(( "Bad\n" ));
    len = 0;
    while( req > 0 ) {
        if( !D32AddressCheck( addr->segment, addr->offset, 1, NULL ) ) break;
        if( r( addr->offset, addr->segment, 0, data, 1 ) != 0 ) break;
        ++addr->offset;
        ++data;
        ++len;
        --req;
    }
    return( len );
}

static unsigned short ReadMemory( addr48_ptr *addr, byte far *data, unsigned short len )
{
    return( ReadWrite( D32DebugRead, addr, data, len ) );
}

static unsigned short WriteMemory( addr48_ptr *addr, byte far *data, unsigned short len )
{
    return( ReadWrite( D32DebugWrite, addr, data, len ) );
}


unsigned ReqGet_sys_config()
{
    get_sys_config_ret  *ret;

    _DBG1(( "AccGetConfig\n" ));

    ret = GetOutPtr(0);
    ret->sys.os = OS_RATIONAL;
    ret->sys.osmajor = _osmajor;
    ret->sys.osminor = _osminor;
    ret->sys.cpu = X86CPUType();
    ret->sys.huge_shift = 12;
    if( !AtEnd && HAVE_EMU ) {
        ret->sys.fpu = X86_EMU;
    } else {
        ret->sys.fpu = RealNPXType;
    }
    ret->sys.mad = MAD_X86;
    return( sizeof( *ret ) );
}


unsigned ReqMap_addr()
{
    Fptr32              fp;
    map_addr_req        *acc;
    map_addr_ret        *ret;
    unsigned            i;

    _DBG1(( "AccMapAddr\n" ));

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    fp.off = acc->in_addr.offset;
    fp.sel = acc->in_addr.segment;
    switch( fp.sel ) {
    case MAP_FLAT_CODE_SELECTOR:
    case MAP_FLAT_DATA_SELECTOR:
        fp.sel = 1;
        fp.off += ObjInfo[0].start;
        for( i = 0; i < NumObjects; ++i ) {
            if( ObjInfo[i].start <= fp.off
             && (ObjInfo[i].start + ObjInfo[i].size) > fp.off ) {
                fp.sel = i + 1;
                fp.off -= ObjInfo[i].start;
                ret->lo_bound = ObjInfo[i].start - ObjInfo[0].start;
                ret->hi_bound = ret->lo_bound + ObjInfo[i].size - 1;
                break;
            }
        }
        break;
    }
    D32Relocate(&fp);
    ret->out_addr.segment = fp.sel;
    ret->out_addr.offset = fp.off;
    return( sizeof( *ret ) );
}

#pragma aux GetLAR =  \
    0x66 0x33 0xD2              /*  xor     edx,edx */  \
    0x8B 0xD0                   /*  mov     dx,ax   */  \
    0x66 0x0F 0x02 0xC2         /*  lar     eax,edx */  \
    0x66 0x8B 0xD0              /*  mov     edx,eax */  \
    0x66 0xC1 0xEA 0x10         /*  shr     edx,16  */  \
    parm caller [ax] value [dx ax];

extern unsigned long GetLAR( unsigned );

//OBSOLETE - use ReqMachine_data
unsigned ReqAddr_info()
{
    addr_info_req       *acc;
    addr_info_ret       *ret;

    _DBG1(( "AccAddrInfo\n" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->is_32 = 0;
    if( D32AddressCheck( acc->in_addr.segment, 0, 1, NULL ) ) {
        if( GetLAR( acc->in_addr.segment ) & 0x400000 ) {
            ret->is_32 = 1;
        }
    }
    return( sizeof( *ret ) );
}

unsigned ReqMachine_data()
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    unsigned_8          *data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    *data = 0;
    if( D32AddressCheck( acc->addr.segment, 0, 1, NULL ) ) {
        if( GetLAR( acc->addr.segment ) & 0x400000 ) {
            *data = X86AC_BIG;
        }
    }
    return( sizeof( *ret ) + sizeof( *data ) );
}

unsigned ReqChecksum_mem()
{
    unsigned short      len;
    int                 i;
    unsigned short      read;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    _DBG1(( "AccChkSum\n" ));

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    len = acc->len;
    ret->result = 0;
    while( len >= BUFF_SIZE ) {
        read = ReadMemory( (addr48_ptr *)&acc->in_addr, &UtilBuff, BUFF_SIZE );
        for( i = 0; i < read; ++i ) {
            ret->result += UtilBuff[ i ];
        }
        if( read != BUFF_SIZE ) return( sizeof( *ret ) );
        len -= BUFF_SIZE;
    }
    if( len != 0 ) {
        read = ReadMemory( (addr48_ptr *)&acc->in_addr, &UtilBuff, len );
        if( read == len ) {
            for( i = 0; i < len; ++i ) {
                ret->result += UtilBuff[ i ];
            }
        }
    }
    return( sizeof( ret ) );
}


unsigned ReqRead_mem()
{
    read_mem_req        *acc;
    void                far *buff;
    unsigned short      len;

    _DBG1(( "ReadMem\n" ));
    acc = GetInPtr( 0 );
    buff = GetOutPtr( 0 );
    len = ReadMemory( (addr48_ptr *)&acc->mem_addr, buff, acc->len );
    return( len );
}

unsigned ReqWrite_mem()
{
    write_mem_req       *acc;
    write_mem_ret       *ret;

    _DBG1(( "WriteMem\n" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = WriteMemory( (addr48_ptr *)&acc->mem_addr,
                            GetInPtr( sizeof(*acc) ),
                            GetTotalSize() - sizeof(*acc) );
    return( sizeof( *ret ) );
}

unsigned ReqRead_io()
{
    read_io_req         *acc;
    void                *data;

    acc = GetInPtr(0);
    data = GetOutPtr(0);
    if( acc->len == 1 ) {
        *( (byte *)data ) = In_b( acc->IO_offset );
    } else if( acc->len == 2 ) {
        *( (word *)data ) = In_w( acc->IO_offset );
    } else {
        *( (dword *)data ) = In_d( acc->IO_offset );
    }
    return( acc->len );
}

unsigned ReqWrite_io()
{
    unsigned            len;
    write_io_req        *acc;
    write_io_ret        *ret;
    void                *data;

    acc = GetInPtr(0);
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );
    ret = GetOutPtr(0);
    if( len == 1 ) {
        Out_b( acc->IO_offset, *( (byte *)data ) );
    } else if( len == 2 ) {
        Out_w( acc->IO_offset, *( (word *)data ) );
    } else {
        Out_d( acc->IO_offset, *( (dword *)data ) );
    }
    ret->len = len;
    return( sizeof( *ret ) );
}

static void ReadCPU( struct x86_cpu *r )
{
    r->efl = Proc.eflags;
    r->eax = Proc.eax;
    r->ebx = Proc.ebx;
    r->ecx = Proc.ecx;
    r->edx = Proc.edx;
    r->esi = Proc.esi;
    r->edi = Proc.edi;
    r->esp = Proc.esp;
    r->ebp = Proc.ebp;
    r->eip = Proc.eip;
    r->ds = Proc.ds;
    r->cs = Proc.cs;
    r->es = Proc.es;
    r->ss = Proc.ss;
    r->fs = Proc.fs;
    r->gs = Proc.gs;
}

static void WriteCPU( struct x86_cpu *r )
{
    Proc.eflags = r->efl;
    Proc.eax = r->eax;
    Proc.ebx = r->ebx;
    Proc.ecx = r->ecx;
    Proc.edx = r->edx;
    Proc.esi = r->esi;
    Proc.edi = r->edi;
    Proc.esp = r->esp;
    Proc.ebp = r->ebp;
    Proc.eip = r->eip;
    Proc.ds = r->ds;
    Proc.cs = r->cs;
    Proc.es = r->es;
    Proc.ss = r->ss;
    Proc.fs = r->fs;
    Proc.gs = r->gs;
}

static void ReadFPU( struct x86_fpu *r )
{
    if( HAVE_EMU ) {
        if( CheckWin386Debug() == WGOD_VERSION ) {
            EMUSaveRestore( Proc.cs, r, 1 );
        } else {
            Read387( r );
        }
    } else if( RealNPXType != X86_NO ) {
        if( _d16info.cpumod >= 3 ) {
            Read387( r );
        } else {
            Read8087( r );
        }
    }
}

static void WriteFPU( struct x86_fpu *r )
{
    if( HAVE_EMU ) {
        if( CheckWin386Debug() == WGOD_VERSION ) {
            EMUSaveRestore( Proc.cs, r, 0 );
        } else {
            Write387( r );
        }
    } else if( RealNPXType != X86_NO ) {
        if( _d16info.cpumod >= 3 ) {
            Write387( r );
        } else {
            Write8087( r );
        }
    }
}


//OBSOLETE - use ReqRead_regs
unsigned ReqRead_cpu()
{
    trap_cpu_regs       *regs;

    regs = GetOutPtr( 0 );
    ReadCPU( (struct x86_cpu *)regs );
    return( sizeof( *regs ) );
}

//OBSOLETE - use ReqRead_regs
unsigned ReqRead_fpu()
{
    trap_fpu_regs       *regs;

    regs = GetOutPtr( 0 );
    ReadFPU( (struct x86_fpu *)regs );
    return( sizeof( *regs ) );
}

//OBSOLETE - use ReqWrite_regs
unsigned ReqWrite_cpu()
{
    trap_cpu_regs       *regs;

    regs = GetInPtr( sizeof( write_cpu_req ) );
    WriteCPU( (struct x86_cpu *)regs );
    return( 0 );
}

//OBSOLETE - use ReqWrite_regs
unsigned ReqWrite_fpu()
{
    trap_fpu_regs       *regs;

    regs = GetInPtr( sizeof( write_fpu_req ) );
    WriteFPU( (struct x86_fpu *)regs );
    return( 0 );
}

unsigned ReqRead_regs( void )
{
    mad_registers       *mr;

    mr = GetOutPtr(0);
    ReadCPU( &mr->x86.cpu );
    ReadFPU( &mr->x86.fpu );
    return( sizeof( mr->x86 ) );
}

unsigned ReqWrite_regs( void )
{
    mad_registers       *mr;

    mr = GetInPtr(sizeof(write_regs_req));
    WriteCPU( &mr->x86.cpu );
    WriteFPU( &mr->x86.fpu );
    return( 0 );
}

static void GetObjectInfo( char *name )
{
    int                 handle;
    unsigned_32         off;
    unsigned            i;
    object_record       obj;
    union {
        dos_exe_header  dos;
        os2_flat_header os2;
    }   head;

    handle = open( name, O_BINARY | O_RDONLY, 0 );
    if( handle == -1 ) return;
    read( handle, &head.dos, sizeof( head.dos ) );
    if( head.dos.signature != DOS_SIGNATURE ) {
        close( handle );
        return;
    }
    lseek( handle, OS2_NE_OFFSET, SEEK_SET );
    read( handle, &off, sizeof( off ) );
    lseek( handle, off, SEEK_SET );
    read( handle, &head.os2, sizeof( head.os2 ) );
    switch( head.os2.signature ) {
    case OSF_FLAT_SIGNATURE:
    case OSF_FLAT_LX_SIGNATURE:
        lseek( handle, head.os2.objtab_off + off, SEEK_SET );
        NumObjects = head.os2.num_objects;
        ObjInfo = realloc( ObjInfo, NumObjects * sizeof( *ObjInfo ) );
        for( i = 0; i < head.os2.num_objects; ++i ) {
            read( handle, &obj, sizeof( obj ) );
            ObjInfo[i].size = obj.size;
            ObjInfo[i].start = obj.addr;
        }
        break;
    default:
        NumObjects = 0;
        break;
    }
    close( handle );
}

unsigned ReqProg_load()
{
    char            *src;
    char            *dst;
    char            *name;
    char            ch;
    prog_load_ret   *ret;
    unsigned        len;

    _DBG1(( "AccLoadProg\r\n" ));
    AtEnd = FALSE;
    dst = UtilBuff;
    src = name = GetInPtr( sizeof( prog_load_req ) );
    ret = GetOutPtr( 0 );
    while( *src++ != '\0' ) {};
    len = GetTotalSize() - (src - name) - sizeof( prog_load_req );
    if( len > 126 )
        len = 126;
    for( ; len > 0; --len ) {
        ch = *src++;
        if( ch == '\0' ) {
            if( len == 1 )
                break;
            ch = ' ';
        }
        *dst++ = ch;
    }
    *dst = '\0';
    _DBG1(( "about to debugload\r\n" ));
    _DBG1(( "Name :" ));
    _DBG1(( name ));
    _DBG1(( "\r\n" ));
    _DBG1(( "UtilBuff :" ));
    _DBG1(( UtilBuff ));
    _DBG1(( "\r\n" ));
    GetObjectInfo( name );
    ret->err = D32DebugLoad( name, UtilBuff, &Proc );
    _DBG1(( "back from debugload - %d\r\n", ret->err ));
    ret->flags = LD_FLAG_IS_32 | LD_FLAG_IS_PROT | LD_FLAG_DISPLAY_DAMAGED;
    if( ret->err == 0 ) {
        ret->task_id = Proc.es;
    } else {
        ret->task_id = 0;
    }
    ret->mod_handle = 0;
    Proc.int_id = -1;
    _DBG1(( "done AccLoadProg\r\n" ));
    return( sizeof( *ret ) );
}

unsigned ReqProg_kill()
{
    prog_kill_ret       *ret;

    _DBG1(( "AccKillProg\n" ));
    ret = GetOutPtr( 0 );
    RedirectFini();
    AtEnd = TRUE;
    ret->err = 0;
    return( sizeof( *ret ) );
}


unsigned ReqSet_watch()
{
    watch           *curr;
    set_watch_req   *acc;
    set_watch_ret   *ret;
    int             i;
    int             needed;

    _DBG1(( "AccSetWatch\n" ));

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 5000;
    ret->err = 0;
    curr = WatchPoints + WatchCount;
    curr->addr.segment = acc->watch_addr.segment;
    curr->addr.offset = acc->watch_addr.offset;
    curr->linear = DPMIGetSegmentBaseAddress( curr->addr.segment ) + curr->addr.offset;
    curr->len = acc->size;
    curr->dregs = ( curr->linear & (curr->len-1) ) ? 2 : 1;
    curr->handle = -1;
    curr->handle2 = -1;
    curr->value = 0;
    ReadMemory( (addr48_ptr *)&acc->watch_addr, (byte far *)&curr->value, curr->len );
    ++WatchCount;
    needed = 0;
    for( i = 0; i < WatchCount; ++i ) {
        needed += WatchPoints[ i ].dregs;
    }
    if( needed <= 4 ) ret->multiplier |= USING_DEBUG_REG;
    return( sizeof( *ret ) );
}


unsigned ReqClear_watch()
{
    _DBG1(( "AccRestoreWatch\n" ));
    /* assume all watches removed at same time */
    WatchCount = 0;
    return( 0 );
}

unsigned ReqSet_break()
{
    set_break_req       *acc;
    set_break_ret       *ret;

    _DBG1(( "AccSetBreak\n" ));

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    D32DebugSetBreak( acc->break_addr.offset, acc->break_addr.segment,
                          FALSE, &Break, (byte *)&ret->old );
    return( sizeof( *ret ) );
}


unsigned ReqClear_break()
{
    clear_break_req     *acc;
    char        dummy;

    acc = GetInPtr( 0 );
    _DBG1(( "AccRestoreBreak\n" ));
    /* assume all breaks removed at same time */
    D32DebugSetBreak( acc->break_addr.offset, acc->break_addr.segment,
                          FALSE, (byte *)&acc->old, &dummy );
    return( 0 );
}

static unsigned long SetDRn( int i, unsigned long linear, long type )
{
    switch( i ) {
    case 0:
        SetDR0( linear );
        break;
    case 1:
        SetDR1( linear );
        break;
    case 2:
        SetDR2( linear );
        break;
    case 3:
        SetDR3( linear );
        break;
    }
    return( ( type << DR7_RWLSHIFT(i) )
//          | ( DR7_GEMASK << DR7_GLSHIFT(i) ) | DR7_GE
          | ( DR7_LEMASK << DR7_GLSHIFT(i) ) | DR7_LE );
}


void ClearDebugRegs()
{
    int         i;
    watch       *wp;

    if( IsDPMI ) {
        for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
            if( wp->handle >= 0 ) {
                DPMIClearWatch( wp->handle );
                wp->handle = -1;
            }
            if( wp->handle2 >= 0 ) {
                DPMIClearWatch( wp->handle2 );
                wp->handle2 = -1;
            }
        }
    } else {
        SetDR6( 0 );
        SetDR7( 0 );
    }
}


static bool SetDebugRegs()
{
    int                 needed;
    int                 i;
    watch               *wp;
    bool                success;
    long                rc;

    needed = 0;
    for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
        needed += wp->dregs;
    }
    if( needed > 4 ) return( FALSE );
    if( IsDPMI ) {
        success = TRUE;
        for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
            wp->handle = -1;
            wp->handle2 = -1;
        }
        for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
            _DBG2(( "Setting Watch On %8.8lx\r\n", wp->linear ));
            success = FALSE;
            rc = DPMISetWatch( wp->linear, wp->len, DPMI_WATCH_WRITE );
            _DBG2(( "OK 1 = %d\r\n", rc >= 0 ));
            if( rc < 0 ) break;
            wp->handle = rc;
            if( wp->dregs == 2 ) {
                rc = DPMISetWatch( wp->linear+4, wp->len, DPMI_WATCH_WRITE );
                _DBG2(( "OK 2 = %d\r\n", rc >= 0 ));
                if( rc <= 0 ) break;
                wp->handle2 = rc;
            }
            success = TRUE;
        }
        if( !success ) {
            ClearDebugRegs();
        }
        return( success );
    } else {
        int             dr;
        unsigned long   dr7;

        dr = 0;
        dr7 = 0;
        for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
            dr7 |= SetDRn( dr, wp->linear, DRLen( wp->len ) | DR7_BWR );
            ++dr;
            if( wp->dregs == 2 ) {
                dr7 |= SetDRn( dr, wp->linear+4, DRLen( wp->len ) | DR7_BWR );
                ++dr;
            }
        }
        SetDR7( dr7 );
        return( TRUE );
    }
}

static unsigned DoRun()
{
    D32DebugRun( &Proc );
    switch( Proc.int_id ) {
    case 1:
        return( COND_TRACE );
    case 3:
        return( COND_BREAK );
    case 5:
        return( COND_USER );
    case 6:
    case 7:
    case 0xd:
        return( COND_EXCEPTION );
    case 0x21:
        return( COND_TERMINATE );
    case 0x23:
        return( COND_EXCEPTION ); // should be TRAP_USER
    default:
        return( COND_EXCEPTION );
    }
}

static bool CheckWatchPoints()
{
    addr48_ptr  addr;
    dword       val;
    watch       *wp;

    for( wp = WatchPoints; wp < WatchPoints + WatchCount; ++wp ) {
        addr.segment = wp->addr.segment;
        addr.offset = wp->addr.offset;
        val = 0;
        if( ReadMemory( &addr, (byte far *)&val, wp->len ) != wp->len ) {
            return( TRUE );
        }
        if( val != wp->value ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static unsigned ProgRun( bool step )
{
    prog_go_ret *ret;
    byte        int_buff[3];
    addr48_ptr  addr;

    _DBG1(( "AccRunProg\n" ));

    ret = GetOutPtr( 0 );

    if( step ) {
        Proc.eflags |= 0x100;
        ret->conditions = DoRun();
        Proc.eflags &= ~0x100;
    } else if( WatchCount != 0 ) {
        if( SetDebugRegs() ) {
            ret->conditions = DoRun();
            ClearDebugRegs();
            if( ret->conditions & COND_TRACE ) {
                ret->conditions |= COND_WATCH;
                ret->conditions &= ~COND_TRACE;
            }
        } else {
            for( ;; ) {
                addr.segment = Proc.cs;
                addr.offset = Proc.eip;

                if( ReadMemory( &addr, int_buff, 3 ) == 3
                    && int_buff[0] == 0xcd ) {
                    /* have to breakpoint across software interrupts because Intel
                        doesn't know how to design chips */
                    addr.offset = Proc.eip + 2;
                    int_buff[0] = 0xcc;
                    WriteMemory( &addr, int_buff, 1 );
                } else {
                    Proc.eflags |= 0x100;
                    int_buff[0] = 0;
                }

                ret->conditions = DoRun();
                if( int_buff[0] != 0 ) {
                    addr.offset = Proc.eip;
                    WriteMemory( &addr, &int_buff[2], 1 );
                } else {
                    Proc.eflags &= ~0x100;
                }
                if( !(ret->conditions & (COND_TRACE|COND_BREAK)) ) break;
                if( CheckWatchPoints() ) {
                    ret->conditions |= COND_WATCH;
                    ret->conditions &= ~(COND_TRACE|COND_BREAK);
                    break;
                }
            }
        }
    } else {
        ret->conditions = DoRun();
    }

    ret->conditions |= COND_CONFIG;
    ret->program_counter.offset = Proc.eip;
    ret->program_counter.segment = Proc.cs;
    ret->stack_pointer.offset = Proc.esp;
    ret->stack_pointer.segment = Proc.ss;
    return( sizeof( *ret ) );
}

unsigned ReqProg_go()
{
    return( ProgRun( FALSE ) );
}

unsigned ReqProg_step()
{
    return( ProgRun( TRUE ) );
}

unsigned ReqGet_next_alias()
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}


unsigned ReqGet_err_text()
{
    static char *DosErrMsgs[] = {
#include "dosmsgs.h"
    };
    get_err_text_req    *acc;
    char                *err_txt;

    #define MAX_CODE (sizeof( DosErrMsgs ) / sizeof( char * ) - 1)

    _DBG(("AccErrText\r\n"));
    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );
    if( acc->err > MAX_CODE ) {
        _DBG(( "After acc->error_code > MAX_CODE" ));
        strcpy( (char *)err_txt, TRP_ERR_unknown_system_error );
        ultoa( acc->err, (char *)err_txt + strlen( err_txt ), 16 );
        _DBG(("After utoa()\r\n"));
    } else {
        strcpy( (char *)err_txt, DosErrMsgs[ acc->err ] );
        _DBG(("After strcpy\r\n"));
    }
    return( strlen( err_txt ) + 1 );
}

unsigned ReqGet_lib_name()
{
    char                *ch;
    get_lib_name_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->handle = 0;
    ch = GetOutPtr( sizeof( *ret ) );
    *ch = '\0';
    return( sizeof( *ret ) + 1 );
}

unsigned ReqGet_message_text()
{
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof(*ret) );
    if( Proc.int_id == -1 ) {
        err_txt[0] = '\0';
    } else {
        ExceptionText( Proc.int_id, err_txt );
        Proc.int_id = -1;
    }
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_version TRAPENTRY TrapInit( char *parm, char *err, bool remote )
{
    trap_version        ver;
    int                 error_num;
    extern int          hotkey_int;


    _DBG1(( "TrapInit\n" ));
    remote = remote; parm = parm;
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    RedirectInit();
    RealNPXType = NPXType();
    WatchCount = 0;
    FakeBreak = FALSE;
    hotkey_int = 5;
    error_num = D32DebugInit( &Proc );
    if( error_num ) {
        _DBG(("D32DebugInit() failed:\n"));
        exit(1);
    }
    Proc.int_id = -1;
    D32DebugBreakOp(&Break);    /* Get the 1 byte break op */
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    _DBG1(( "TrapFini\n" ));
    D32DebugTerm();
}
