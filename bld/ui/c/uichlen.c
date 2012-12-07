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
* Description:  Multi-byte character support for ui library.
*
****************************************************************************/


#include "uidef.h"

#if !defined( __NETWARE_386__ )

#if defined( __UNIX__ )

int global uicharlen( int ch )
/****************************/
{
    return( 1 );
}

int global uiisdbcs( void )
/*************************/
{
    return( FALSE );
}

#else

#include <mbctype.h>

int global uicharlen( int ch )
/****************************/
{
    return( _mbislead( ch ) ? 2 : 1 );
}

int global uiisdbcs( void )
/*************************/
{
    return( __IsDBCS );
}

#endif

#if defined( __AXP__ ) || defined( __UNIX__ )
    int global uionnec( void )
    {
        return( 0 );
    }
#else
    extern int __NonIBM;

    int global uionnec( void )
    {
        return( __NonIBM );
    }
#endif

#else

int global uicharlen( int ch )
/****************************/
{
    ch=ch;
    return( 1 ); // this is the stub version.  Real verion in system directories
}

int global uiisdbcs( void )
/*************************/
{
    return( 0 );
}

int global uionnec( void )
{
    return( 0 );
}

#endif
