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
* Description:  Select registers used for passing an arguments.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "procdef.h"
#include "cgdefs.h"
#include "typedef.h"
#include "regset.h"
#include "zoiks.h"
#include "feprotos.h"
#include "cgaux.h"


extern  hw_reg_set      InLineParm(hw_reg_set,hw_reg_set);
extern  reg_list        *ParmChoices(type_class_def);
extern  type_def        *TypeAddress(cg_type);
extern  hw_reg_set      ParmRegConflicts(hw_reg_set);

extern  type_length     ParmAlignment( type_def *tipe ) {
/*******************************************************/

    tipe = tipe;
    return( 1 );
}

extern  hw_reg_set      ParmReg( type_class_def class, type_length len, type_length alignment, call_state *state ) {
/******************************************************************************************************************/

    hw_reg_set  *possible;
    hw_reg_set  *reg_set;
    hw_reg_set  regs;

    len = len;
    alignment = alignment;
    possible = ParmChoices( class );
    if( possible == NULL || HW_CEqual( *possible, HW_EMPTY ) ) {
        if( !HW_CEqual( *state->parm.curr_entry, HW_EMPTY ) ) {
            state->parm.curr_entry++;
        }
        return( HW_EMPTY );
    }
    while( !HW_CEqual( *state->parm.curr_entry, HW_EMPTY ) ) {
        reg_set = possible;
        for(;;) {
            regs = *reg_set;
            if( HW_CEqual( regs, HW_EMPTY ) ) break;
            if( !HW_Ovlap( regs, state->parm.used ) ) {
                if( HW_Subset( *state->parm.curr_entry, regs ) ) {
                    HW_TurnOn( state->parm.used, regs );
                    HW_TurnOn( state->parm.used, ParmRegConflicts( regs ) );
                    return( regs );
                }
            }
            ++ reg_set;
        }
        state->parm.curr_entry ++;
    }
#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
    /* Optionally consider registers again for the next argument, even if
     * this one could not be passed in regsiters. Required for __fastcall.
     */
    if( state->attr & ROUTINE_PREFER_REGS )
        state->parm.curr_entry = state->parm.table;
#endif
    return( HW_EMPTY );
}
