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
* Description:  GetClusterSize() - get cluster size
*
****************************************************************************/

/*
   GETCLSZ.C - get cluster size

   Date         By              Reason
   ====         ==              ======
   28-jan-92    Craig Eisler    split from ls.c
   25-mar-92    Craig Eisler    NT port
 */
#if !( defined( __OS_qnx__ ) || defined( __OS_qnx16__ ) )
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#if defined( __OS_dosos2__ ) || defined( __OS_os2v2__ )
#define INCL_DOS
#include <os2.h>
#endif
#if defined( __OS_nt__ )
#include <windows.h>
#endif

long GetClusterSize( unsigned drive )
{
    if( drive == 0 )
        _dos_getdrive( &drive );
#if defined( __OS_nt__ )
    {
        char    root[4];
        char    *proot;
        DWORD   spc, bps, nofc, tnoc;

        if( drive == 0 ) {
            proot = NULL;
        } else {
            proot = root;
            root[0] = 'a' + drive - 1;
            root[1] = ':';
            root[2] = '\\';
            root[3] = 0;
        }
        GetDiskFreeSpace( root, &spc, &bps, &nofc, &tnoc );
        return( spc * bps );
    }
#elif defined( __OS_dosos2__ ) || defined( __OS_os2v2__ )
    {
        FSALLOCATE      fs;
        DosQFSInfo( drive, 1, ( void *) & fs, sizeof( FSALLOCATE ) );
        return( ( long ) fs.cbSector * ( long ) fs.cSectorUnit );
    }
#else
    {
        struct diskfree_t       df;
        _dos_getdiskfree( drive, &df );
        return( ( long ) df.bytes_per_sector * ( long ) df.sectors_per_cluster );
    }
#endif
}
#endif
