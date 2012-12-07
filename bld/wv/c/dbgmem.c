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
* Description:  Access to program memory.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbglit.h"
#include "dbgitem.h"
#include "dbgerr.h"
#include "mad.h"

extern char             *GetCmdName( int );
extern char             *Format( char *buff, char *fmt, ... );
extern char             *CnvULong( unsigned long, char * );
extern unsigned         ProgPeek( address, void *, unsigned int );
extern unsigned         ChangeMem( address, void *, unsigned int );
extern unsigned         PortPeek( unsigned, void *, unsigned );
extern unsigned         PortPoke( unsigned, void *, unsigned );
extern char             *StrCopy( char *, char * );
extern char             *AddHexSpec( char * );
extern void             AddrFix( address * );
extern address          AddrAddWrap( address, long );
extern void             RecordEvent( char * );
extern bool             AdvMachState( int );
extern void             CollapseMachState( void );
extern void             DbgUpdate( update_list );

extern char             *TxtBuff;

//MAD: convert this stuff to work off of mad_type_handle's ?

/* this item doesn't really exist, it's just being used for sizeof */
extern item_mach    Mach;

/***********************************************************************
  !!!!!! must correspond with enum order (item_type) in dbgitem.h !!!!!
 ***********************************************************************/
static unsigned Sizes[] = {
        0,
        sizeof( Mach.ub ),
        sizeof( Mach.sb ),
        sizeof( Mach.uw ),
        sizeof( Mach.sw ),
        sizeof( Mach.ud ),
        sizeof( Mach.sd ),
        sizeof( Mach.uq ),
        sizeof( Mach.sq ),
        sizeof( Mach.sf ),
        sizeof( Mach.lf ),
        sizeof( Mach.xf ),
        sizeof( Mach.so ),
        sizeof( Mach.lo ),
        sizeof( Mach.qo ),
        sizeof( Mach.sa ),
        sizeof( Mach.la ),
        sizeof( Mach.xa ),
        sizeof( Mach.sc ),
        sizeof( Mach.lc ),
        sizeof( Mach.xc ),
        sizeof( Mach.nwscb ),
        sizeof( Mach.fwscb ),
        sizeof( Mach.ndscb ),
        sizeof( Mach.fdscb ),
};

void ChangeMemUndoable( address addr, void *item, int size )
{
    char        *p;
    char        *it;

    if( AdvMachState( ACTION_MODIFY_MEMORY ) ) {
        ChangeMem( addr, item, size );
        it = (char*)item;
        p = Format( TxtBuff, "%s %A", GetCmdName( CMD_MODIFY ), addr );
        while( --size >= 0 ) {
            p = StrCopy( ", ", p );
            p = CnvULong( *it++, p );
        }
        RecordEvent( TxtBuff );
        DbgUpdate( UP_MEM_CHANGE );
        CollapseMachState();
    }
}

unsigned        ProgPeekWrap(address addr,char * buff,unsigned length )
{
    unsigned    peek1,peek2;

    peek1 = ProgPeek( addr, buff, length );
    if( peek1 == 0 || peek1 == length ) return( peek1 );
    peek2 = ProgPeek( AddrAddWrap( addr, peek1 ), buff+peek1, length-peek1 );
    return( peek1+peek2 );
}

static item_type ItemType( unsigned size )
{
    item_type   i;

    for( i = 0; Sizes[ i ] != size; ++i ) {
        if( i >= sizeof( Sizes ) / sizeof( Sizes[0] ) ) return( IT_NIL );
    }
    return( i );
}


unsigned ItemSize( item_type typ )
{
    return( Sizes[ typ & IT_TYPE_MASK ] );
}


static bool ItemGet( address *addr, item_mach *item, item_type typ )
{
    unsigned    size = Sizes[ typ & IT_TYPE_MASK ];

    if( typ & IT_DEC ) addr->mach.offset -= size;
    if( typ & IT_IO ) {
        if( PortPeek( addr->mach.offset, item, size ) != size ) {
            if( typ & IT_ERR ) {
                Error( ERR_NONE, LIT( ERR_NO_READ_PORT ), *addr );
            }
            return( FALSE );
        }
    } else {
        if( ProgPeek( *addr , item, size ) != size ) {
            if( typ & IT_ERR ) {
                AddrFix( addr );
                Error( ERR_NONE, LIT( ERR_NO_READ_MEM ), *addr );
            }
            return( FALSE );
        }
    }
    if( typ & IT_INC ) addr->mach.offset += size;
    return( TRUE );

}


static bool ItemPut( address *addr, item_mach *item, item_type typ )
{
    unsigned    size = Sizes[ typ & IT_TYPE_MASK ];

    if( typ & IT_DEC ) addr->mach.offset -= size;
    if( typ & IT_IO ) {
        if( PortPoke( addr->mach.offset, item, size ) != size ) {
            if( typ & IT_ERR ) {
                Error( ERR_NONE, LIT( ERR_NO_WRITE_PORT ), *addr );
            }
            return( FALSE );
        }
    } else {
        if( ChangeMem( *addr, item, size ) != size ) {
            if( typ & IT_ERR ) {
                Error( ERR_NONE, LIT( ERR_NO_WRITE_MEM ), *addr );
            }
            return( FALSE );
        }
    }
    if( typ & IT_INC ) addr->mach.offset += size;
    return( TRUE );

}

static item_type ItemTypeFromMADType( mad_type_handle th )
{
    mad_type_info       mti;

    MADTypeInfo( th, &mti );
    switch( mti.b.kind ) {
    case MTK_INTEGER:
    case MTK_XMM:
    case MTK_CUSTOM:
        return( ItemType( mti.b.bits / BITS_PER_BYTE ) );
    case MTK_ADDRESS:
        switch( mti.b.bits ) {
        case 16:
            return( IT_SO );
        case 32:
            if( mti.a.seg.bits == 0 ) {
                return( IT_LO );
            } else {
                return( IT_SA );
            }
        case 48:
            return( IT_LA );
        }
        break;
    case MTK_FLOAT:
        switch( mti.b.bits ) {
        case 32:
            return( IT_SF );
        case 64:
            return( IT_LF );
        case 80:
            return( IT_XF );
        }
        break;
    }
    return( IT_NIL );
}

item_type ItemGetMAD( address *addr, item_mach *item, item_type ops, mad_type_handle th )
{
    item_type   it;

    it = ItemTypeFromMADType( th );
    if( !ItemGet( addr, item, it | ops ) ) return( IT_NIL );
    return( it );
}

item_type ItemPutMAD( address *addr, item_mach *item, item_type ops, mad_type_handle th )
{
    item_type   it;

    it = ItemTypeFromMADType( th );
    if( !ItemPut( addr, item, it | ops ) ) return( IT_NIL );
    return( it );
}
