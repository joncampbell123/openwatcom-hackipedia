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


#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include "dbgdefn.h"
#include "dbglit.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "dbgtoggl.h"
#include "dbgreg.h"
#include "dbgerr.h"
#include "madcli.h"
#include "dui.h"
#include "mad.h"


extern void             AddrSection( address *, unsigned );
extern void             DbgUpdate( update_list );
extern unsigned         ProgPeek( address, void *, unsigned );
extern unsigned         ProgPoke( address, void *, unsigned );
extern bool             RemoteOvlTransAddr(address *);
extern address          GetRegIP(void);
extern void             ResizeRegData(void);
extern void             ResizeTraceData(void);
extern void             PendingToggles( void );
extern bool             FixOvlRetAddr( address * );
extern void             FindRadixSpec( unsigned char value, char **start, unsigned *len );
extern char             *ReScan( char * );
extern void             EvalExpr( unsigned addr_depth );
extern void             MakeMemoryAddr( bool pops, memory_expr def_seg, address *val );
extern unsigned         SetCurrRadix( unsigned );
extern char             *AddrTypeToString( address *a, mad_type_handle th, char *p, unsigned max );
extern char             *CnvAddr( address addr, cnvaddr_option cao, bool uniq, char *p, unsigned max );
extern void             StartupErr( char * );
extern void             SetMADMenuItems( void );
extern void             ClearMachineDataCache( void );

extern address          NilAddr;
extern char             *TxtBuff;
extern system_config    SysConfig;
extern machine_state    *DbgRegs;
extern unsigned char    CurrRadix;

static mad_status               MADStatus;

/*
 * Client support routines
 */

void DIGCLIENT MADCliAddrSection( address *addr )
{
    AddrSection( addr, OVL_MAP_CURR );
}

mad_status      DIGCLIENT MADCliAddrOvlReturn( address *addr )
{
    return( FixOvlRetAddr( addr ) ? MS_OK : MS_FAIL );
}

mad_status      DIGCLIENT MADCliAddrToString( address a, mad_type_handle th,
                            mad_label_kind lk, unsigned max, char *buff )
{
    char        *p;

    RemoteOvlTransAddr( &a );
    switch( lk ) {
    case MLK_CODE:
        p = CnvAddr( a, CAO_NO_PLUS, FALSE, buff, max );
        break;
    default:
        p = CnvAddr( a, CAO_NORMAL_PLUS, FALSE, buff, max );
        break;
    }
    if( p != NULL ) return( MS_OK );
    AddrTypeToString( &a, th, buff, max );
    return( MS_FAIL );
}

mad_status      DIGCLIENT MADCliMemExpr( char const *start, unsigned len, unsigned radix, address *a )
{
    char        save;
    char       *startrw;
    char        *old_scan;
    unsigned    old_radix;

    startrw = (char *)start; //MJC const defeat type checking
    save = startrw[len];
    startrw[len] = '\0';
    old_radix = SetCurrRadix( radix );
    old_scan = ReScan( startrw );
    EvalExpr( 0 );   /* memory expression */
    MakeMemoryAddr( TRUE, EXPR_DATA, a );
    startrw[len] = save;
    SetCurrRadix( old_radix );
    ReScan( old_scan );
    return( MS_OK );
}


unsigned        DIGCLIENT MADCliReadMem( address a, unsigned size, void *buff )
{
    return( ProgPeek( a, buff, size ) );
}

unsigned        DIGCLIENT MADCliWriteMem( address a, unsigned size, const void *buff )
{
    return( ProgPoke( a, (void *)buff, size ) );
}

unsigned        DIGCLIENT MADCliString( mad_string mstr, unsigned max, char *buff )
{
    #undef wdpick
    #define wdpick( e, es, js ) LITREF( e ),
    static  char ** strings[] = {
        #include "mad.str"
    };
    #undef wdpick

    unsigned    len;

    len = strlen( *strings[mstr] );
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        memcpy( buff, *strings[mstr], max );
        buff[max] = '\0';
    }
    return( len );
}

mad_status      DIGCLIENT MADCliAddString( mad_string mstr, const char *str )
{
    //MAD: NYI
    return( MS_FAIL );
}

unsigned        DIGCLIENT MADCliRadixPrefix( unsigned radix, unsigned max, char *buff )
{
    char                *start;
    unsigned            len;

    if( radix == CurrRadix ) return( 0 );
    FindRadixSpec( radix, &start, &len );
    if( max > 0 && len > 0 ) {
        --max;
        if( max > len ) max = len;
        memcpy( buff, start, max );
        buff[max] = '\0';
    }
    return( len );
}

void            DIGCLIENT MADCliNotify( mad_notify_type nt, const void *d )
{
    switch( nt ) {
    case MNT_ERROR:
        MADStatus = *(mad_status *)d;
        break;
    case MNT_MODIFY_REG:
        DbgUpdate( UP_REG_CHANGE );
        break;
    case MNT_MODIFY_IP:
        DbgUpdate( UP_CSIP_CHANGE );
        break;
    case MNT_MODIFY_SP:
    case MNT_MODIFY_FP:
        break;
    case MNT_EXECUTE_TOUCH_SCREEN_BUFF:
        _SwitchOn( SW_TOUCH_SCREEN_BUFF );
        break;
    case MNT_EXECUTE_LONG:
        _SwitchOn( SW_EXECUTE_LONG );
        break;
    case MNT_REDRAW_DISASM:
        DbgUpdate( UP_ASM_RESIZE );
        break;
    case MNT_REDRAW_REG:
        DbgUpdate( UP_REG_RESIZE );
        break;
    }
}

system_config   *DIGCLIENT MADCliSystemConfig( void )
{
    return( &SysConfig );
}

/*
 *      Debugger routines
 */

void InitMADInfo()
{
    if( MADInit() != MS_OK ) {
        StartupErr( LIT( STARTUP_MAD_Not_Init ) );
    }
}

void FiniMADInfo()
{
    MADFini();
}

mad_type_handle GetMADTypeHandleDefaultAt( address a, mad_type_kind mtk )
{
    return( MADTypeDefault( mtk, MAF_FULL, &DbgRegs->mr, &a ) );
}

void    GetMADTypeDefaultAt( address a, mad_type_kind mtk, mad_type_info *mti )
{
    MADTypeInfo( GetMADTypeHandleDefaultAt( a, mtk ), mti );
}

void    GetMADTypeDefault( mad_type_kind mtk, mad_type_info *mti )
{
    GetMADTypeDefaultAt( GetRegIP(), mtk, mti );
}

void ReportMADFailure( mad_status ms )
{
    mad_handle  old;
    char        buff[TXT_LEN];

    if( SysConfig.mad == MAD_NIL ) {
        /* we're in deep do do */
        StartupErr( LIT( LMS_RECURSIVE_MAD_FAILURE ) );
    }
    old = SysConfig.mad;
    MADNameFile( old, TXT_LEN, buff );
    SysConfig.mad = MAD_NIL;
    /* this deregisters the MAD, and sets the active one to the dummy */
    MADRegister( old, NULL, NULL );
    _SwitchOn( SW_ERROR_RETURNS );
    switch( ms & ~MS_ERR ) {
    case MS_UNREGISTERED_MAD:
        Error( ERR_NONE, LIT( LMS_UNREGISTERED_MAD ), old );
        break;
    case MS_INVALID_MAD_VERSION:
        Error( ERR_NONE, LIT( LMS_INVALID_MAD_VERSION ), buff );
        break;
    case MS_INVALID_STATE:
        Error( ERR_INTERNAL, "MAD state" );
        break;
    case MS_FOPEN_FAILED:
        Error( ERR_NONE, LIT( ERR_FILE_NOT_OPEN ), buff );
        break;
    case MS_FREAD_FAILED:
    case MS_FSEEK_FAILED:
        Error( ERR_NONE, LIT( LMS_FREAD_FAILED ), buff );
        break;
    case MS_NO_MEM:
        Error( ERR_NONE, LIT( ERR_NO_MEMORY ) );
        break;
    case MS_INVALID_MAD:
    default:
        Error( ERR_NONE, LIT( LMS_INVALID_MAD ), buff );
        break;
    }
    _SwitchOff( SW_ERROR_RETURNS );
}

void CheckMADChange()
{
    mad_status          ms;
    mad_type_info       mti;

    if( MADActiveSet( SysConfig.mad ) != SysConfig.mad ) {
        if( MADLoaded( SysConfig.mad ) != MS_OK ) {
            ms = MADLoad( SysConfig.mad );
            if( ms != MS_OK ) {
                ReportMADFailure( ms );
            }
        }
        DbgUpdate( UP_MAD_CHANGE );
        ResizeTraceData();
        ResizeRegData();
        PendingToggles();
        if( MADCallStackGrowsUp() == MS_OK ) {
            _SwitchOn( SW_STACK_GROWS_UP );
        } else {
            _SwitchOff( SW_STACK_GROWS_UP );
        }
        GetMADTypeDefault( MTK_ADDRESS, &mti );
        if( mti.a.seg.bits != 0 ) {
            _SwitchOn( SW_HAVE_SEGMENTS );
        } else {
            _SwitchOff( SW_HAVE_SEGMENTS );
        }
        SetMADMenuItems();
        ClearMachineDataCache();
    }
}

static unsigned NormalizeString( char *p )
{
    char        *start;
    char        *d;

    start = p;
    d = p;
    while( *p != '\0' ) {
        switch( *p ) {
        case ' ':
        case '\t':
            *d++ = '_';
            break;
        case '&':
            break;
        default:
            *d++ = *p;
            break;
        }
        ++p;
    }
    *d = '\0';
    return( d - p );
}

unsigned GetMADNormalizedString( mad_string ms, unsigned max, char *p )
{
    MADCliString( ms, max, p );
    return( NormalizeString( p ) );
}

unsigned GetMADTypeNameForCmd( mad_type_handle th, unsigned max, char *p )
{
    return( GetMADNormalizedString( MADTypeName( th ), max, p ) );
}

struct find_handle {
    unsigned            bits;
    mad_type_handle     th;
};

static walk_result CheckOneHandle( mad_type_handle th, void *d )
{
    struct find_handle  *fd = d;
    mad_type_info       mti;

    MADTypeInfo( th, &mti );
    if( mti.b.bits != fd->bits ) return( WR_CONTINUE );
    fd->th = th;
    return( WR_STOP );
}

mad_type_handle FindMADTypeHandle( mad_type_kind tk, unsigned size )
{
    struct find_handle  data;

    data.bits = size * BITS_PER_BYTE;
    data.th = MAD_NIL_TYPE_HANDLE;
    MADTypeWalk( tk, CheckOneHandle, &data );
    return( data.th );
}

struct find_mad {
    char        *name;
    unsigned    len;
    mad_handle  mad;
};

static walk_result FindTheMad( mad_handle mh, void *d )
{
    struct find_mad     *fd = d;
    char                buff[80];
    char                *p;

    MADNameFile( mh, sizeof( buff ), buff );
    p = SkipPathInfo( buff, 0 );
    if( memicmp( buff, fd->name, fd->len ) == 0 ) {
        fd->mad = mh;
        return( WR_STOP );
    }
    MADNameDescription( mh, sizeof( buff ), buff );
    NormalizeString( buff );
    if( memicmp( buff, fd->name, fd->len ) == 0 ) {
        fd->mad = mh;
        return( WR_STOP );
    }
    return( WR_CONTINUE );
}

mad_handle FindMAD( char *name, unsigned len )
{
    struct find_mad     data;

    data.name = name;
    data.len = len;
    data.mad = MAD_NIL;
    MADWalk( FindTheMad, &data );
    return( data.mad );
}
