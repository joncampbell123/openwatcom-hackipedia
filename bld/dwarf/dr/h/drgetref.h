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


#ifndef __DRGETREF_H__
#define __DRGETREF_H__

#include "dr.h"
#include "drsrchdf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int         size;   /* available room */
    int         free;   /* next free entry */
    dr_handle * stack;  /* values */
} dr_scope_stack;

typedef struct {
    dr_scope_stack  scope;
    dr_handle       dependent;
    char *          file;
    unsigned_32     line;
    unsigned_8      column;
} dr_ref_info;

extern void DRRefersTo( dr_handle, void *,
    bool (*callback)(dr_handle,dr_ref_info *,char*,void *));

extern void DRReferredToBy( dr_handle, void *,
    bool (*callback)(dr_handle,dr_ref_info *,char*,void *));

extern void DRReferencedSymbols( dr_sym_type, void *,
    bool (*callback)(dr_handle,dr_ref_info *,char*,void *));

#ifdef __cplusplus
}
#endif

#endif // __DRGETREF_H__
