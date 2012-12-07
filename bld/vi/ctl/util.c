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


#include "vi.h"
#include "util.h"
#include "source.h"

vi_rc UtilUpdateBoolean( BOOL old, BOOL val, char *name )
{
    char    cmd[MAX_SRC_LINE] = "set ";
    if( old == val ) {
        return( ERR_NO_ERR );
    }
    if( !val ) {
        strcat( cmd, "no" );
    }
    strcat( cmd, name );
    return( RunCommandLine( cmd ) );
}

vi_rc UtilUpdateInt( int old, int val, char *name )
{
    char    cmd[MAX_SRC_LINE];
    if( old == val ) {
        return( ERR_NO_ERR );
    }
    sprintf( cmd, "set %s %d", name, val );
    return( RunCommandLine( cmd ) );
}

vi_rc UtilUpdateChar( char old, char val, char *name )
{
    char    cmd[MAX_SRC_LINE];
    if( old == val ) {
        return( ERR_NO_ERR );
    }
    sprintf( cmd, "set %s %c", name, val );
    return( RunCommandLine( cmd ) );
}

vi_rc UtilUpdateStr( char *old, char *val, char *name )
{
    char    cmd[MAX_SRC_LINE];
    if( !strcmp( old, val ) ) {
        return( ERR_NO_ERR );
    }
    sprintf( cmd, "set %s %s", name, val );
    return( RunCommandLine( cmd ) );
}
