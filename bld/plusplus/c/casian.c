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
* Description:  MBCS/DBCS character support.
*
****************************************************************************/


#include "plusplus.h"

#ifndef __UNIX__
#include <mbstring.h>
#include <mbctype.h>
#endif

#include "scan.h"

#define LEAD_BYTE_INIT  ( C_DB | C_EX )

static void setRange( unsigned low, unsigned high )
{
    unsigned i;

    for( i = low; i <= high; ++i ) {
        CharSet[i] = LEAD_BYTE_INIT;
    }
}


void SetDBChar( int character_set )
/*********************************/
{
    switch( character_set ) {
    case 0:
        setRange( 0x81, 0x9f );
        setRange( 0xe0, 0xfc );
        break;
    case 1:
        setRange( 0x81, 0xfc );
        break;
    case 2:
        setRange( 0x81, 0xfd );
        break;
    case -1:
#ifndef __UNIX__
        {
            unsigned i;

            _setmbcp( _MB_CP_ANSI );
            for( i = 0x80; i <= 0x0ff; ++i ) {
                if( _mbislead( i ) ) {
                    CharSet[i] = LEAD_BYTE_INIT;
                }
            }
        }
#endif
        break;
    }
}
