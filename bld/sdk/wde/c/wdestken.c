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


#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "wdeglbl.h"
#include "wdemem.h"
#include "wdestken.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WdeStackEnvType {
    struct WdeStackEnvType *next;
    jmp_buf                 e;
} WdeStackEnvType;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WdeStackEnvType *WdeTop = NULL;

Bool WdePushEnv ( jmp_buf *e )
{
    WdeStackEnvType *s;

    s = (WdeStackEnvType *) WdeMemAlloc ( sizeof (WdeStackEnvType) );
    if ( !s ) {
        return ( FALSE );
    }

    memcpy ( &s->e, e, sizeof(jmp_buf) );
    s->next = WdeTop;
    WdeTop  = s;

    return ( TRUE );
}

Bool WdePopEnv ( jmp_buf *e )
{
    WdeStackEnvType *s;

    if ( WdeTop ) {
        s = WdeTop;
        WdeTop = s->next;
        memcpy ( e, &s->e, sizeof(jmp_buf) );
        WdeMemFree ( s );
    } else {
        return ( FALSE );
    }

    return ( TRUE );
}

Bool WdeTopEnv ( jmp_buf *e )
{
    if ( WdeTop ) {
        memcpy ( e, &WdeTop->e, sizeof(jmp_buf) );
    } else {
        return ( FALSE );
    }

    return ( TRUE );
}

