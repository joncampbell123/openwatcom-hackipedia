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
* Description:  Keep track of dialog messages.
*
****************************************************************************/


#include <windows.h>
#include "wrglbl.h"
#include "wrlist.h"
#include "wrdmsg.h"
#include "wrdmsgi.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/*****************************************************************************/

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static  LIST    *WRDialogList   = NULL;

void WRDialogMsgInit( void )
{
    // do nothing
}

void WRDialogMsgFini( void )
{
    ListFree( WRDialogList );
    WRDialogList = NULL;
}

void WRRegisterDialog( HWND hdlg )
{
    WRInsertObject( &WRDialogList, (void *)hdlg );
}

void WRUnregisterDialog( HWND hdlg )
{
    ListRemoveElt( &WRDialogList, (void *)hdlg );
}

int WR_EXPORT WRIsWRDialogMsg( MSG *msg )
{
    HWND        hdlg;
    LIST        *item;

    for( item = WRDialogList; item != NULL ; item = ListNext( item ) ) {
        hdlg = (HWND)(DWORD)ListElement( item );
        if( ( hdlg != (HWND)NULL ) && IsDialogMessage( hdlg, msg ) ) {
            return( TRUE );
        }
    }

    return( FALSE );
}

