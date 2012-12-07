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


/* aligned */
/*        Structures to hold object information */

#include "offset.h"

typedef int                     objhandle;

typedef enum {
        BYTE_PATCH              = 0x00,
        ADD_PATCH               = 0x01,
        WORD_PATCH              = 0x02,
        LONG_PATCH              = 0x04
} patch_attr;

#define INVALID 0       /*  invalid value for an objhandle */

typedef struct array_control {
        pointer         array;  /*  pointer to allocated array */
        int             alloc;  /*  number of elements allocated */
        int             used;   /*  index of first unused element */
        int             entry;  /*  size in bytes of each array element */
        int             inc;    /*  number of elements to increment by */
} array_control;

typedef struct patch {
        int                     where;
        objhandle               ref;
        patch_attr              attr;
} patch;

typedef struct temp_patch {
        struct patch            pat;
        struct temp_patch       *link;
        label_handle            lbl;
} temp_patch;

typedef enum {
        AP_HAVE_VALUE           = 0x01,
        AP_HAVE_OFFSET          = 0x02
} abspatch_flags;

typedef struct abspatch {
        struct patch            pat;
        struct object           *obj;
        struct abspatch         *link;
        long_offset             value;
        abspatch_flags          flags;
} abspatch;

typedef struct object {
        struct array_control    data;
        struct array_control    fixes;
        struct temp_patch       *patches;
        struct array_control    *exports;
        struct array_control    *lines;
        long_offset             start;
        objhandle               segfix;
        unsigned_16             index;
        unsigned_16             sofar;
        unsigned                gen_static_exports      : 1;
        unsigned                lines_generated         : 1;
        unsigned                line_info               : 1;
        unsigned                pending_label_line      : 1;
        cg_linenum              pending_line_number;
} object;

typedef enum {
        F_LOBYTE        = 0,    /* not used */
        F_OFFSET        = 1,
        F_BASE          = 2,
        F_PTR           = 3,
        F_HIBYTE        = 4,    /* not used */
        F_PHAR_OFFSET   = 5,
        F_LDR_OFFSET    = 5,
        F_PHAR_PTR      = 6,
        F_MS_OFFSET_32  = 9,
        F_MS_PTR        = 11,
        F_MS_LDR_OFFSET_32  = 13,
        F_MASK          = 0x3f,
        F_TLS           = 0x40, /* special trigger value */
        F_FAR16         = 0x80 /* special trigger value */
} fix_class;
