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
* Description:  Label directive, anonymous labels
*
****************************************************************************/


#include "asmglob.h"
#include "asmalloc.h"
#include "asmfixup.h"
#include "asmlabel.h"

#if defined( _STANDALONE_ )

#include "directiv.h"
#include "asmstruc.h"
#include "queues.h"

static unsigned   AnonymousCounter = 0;

void PrepAnonLabels( void )
/*************************/
{
    struct asm_sym *sym;
    char buffer[20];

    sprintf( buffer, "L&_%d", AnonymousCounter );
    AsmChangeName( "@B", buffer  );
    AnonymousCounter = 0;

    sym = AsmGetSymbol( "L&_0" );
    if( sym != NULL ) {
        AsmChangeName( sym->name, "@F" );
    }

}

bool IsLabelStruct( char *name )
/******************************/
{
    asm_sym *sym;

    sym = AsmGetSymbol( name );
    return( ( sym != NULL && sym->state == SYM_STRUCT ) );
}
#endif

int MakeLabel( char *symbol_name, memtype mem_type )
/**********************************************/
{
    struct asm_sym      *sym;
#if defined( _STANDALONE_ )
    int                 addr;
    char                buffer[20];
    struct asm_sym      *newsym;
    proc_info           *info;

    if( CurrSeg == NULL )
        AsmError( LABEL_OUTSIDE_SEGMENT );
    if( symbol_name[0] == '@' && symbol_name[1] == '@' && symbol_name[2] == '\0' ) {
        /* anonymous label */
        /* find any references to @F and mark them to here as @B */
        /* find the old @B */
        sym = AsmGetSymbol( "@B" );
        if( sym != NULL ) {
            /* change it to some magical name */
            sprintf( buffer, "L&_%d", AnonymousCounter++ );
            AsmChangeName( sym->name, buffer );
        }
        sym = AsmLookup( "@B" );
        /* change all forward anon. references to this location */
        newsym = AsmGetSymbol( "@F" );
        if( newsym != NULL ) {
            sym->fixup = newsym->fixup;
            newsym->fixup = NULL;
        }
        AsmTakeOut( "@F" );
        sym->state = SYM_INTERNAL;
        sym->mem_type = mem_type;  // fixme ??
        GetSymInfo( sym );
        BackPatch( sym );
        /* now point the @F marker at the next anon. label if we have one */
        sprintf( buffer, "L&_%d", AnonymousCounter+1 );
        sym = AsmGetSymbol( buffer );
        if( sym != NULL ) {
            AsmChangeName( sym->name, "@F" );
        }
        return( NOT_ERROR );
    }
    if( (Options.mode & MODE_IDEAL) && Options.locals_len ) {
        if( memcmp( symbol_name, Options.locals_prefix, Options.locals_len ) == 0
            && symbol_name[Options.locals_len] != '\0' ) {
            if( CurrProc == NULL ) {
                AsmError( SYNTAX_ERROR );
                return( ERROR );
            }
            info = CurrProc->e.procinfo;
            sym = AsmLookup( symbol_name );
            if( sym == NULL )
                return( ERROR );
            GetSymInfo( sym );
            BackPatch( sym );
            return( NOT_ERROR );
        }
    }
     sym = AsmLookup( symbol_name );
    if( sym == NULL )
        return( ERROR );
    if( Parse_Pass == PASS_1 ) {
        if( sym->state == SYM_EXTERNAL && ((dir_node *)sym)->e.extinfo->global ) {
            dir_to_sym( (dir_node *)sym );
            AddPublicData( (dir_node *)sym );
            if( sym->mem_type != mem_type ) {
                AsmErr( SYMBOL_TYPE_DIFF, sym->name );
                return( ERROR );
            }
        } else if( sym->state != SYM_UNDEFINED ) {
            AsmErr( SYMBOL_PREVIOUSLY_DEFINED, symbol_name );
            return( ERROR );
        }
    } else {
        /* save old offset */
        addr = sym->offset;
    }
    if( Definition.struct_depth != 0 ) {
        if( Parse_Pass == PASS_1 ) {
            sym->offset = AddFieldToStruct( sym,  -1 );
            sym->state = SYM_STRUCT_FIELD;
        }
    } else {
        sym->state = SYM_INTERNAL;
        GetSymInfo( sym );
    }
    sym->mem_type = mem_type;  // fixme ??
    if( Parse_Pass != PASS_1 && sym->offset != addr ) {
        PhaseError = TRUE;
    }
#else
    sym = AsmLookup( symbol_name );
    if( sym == NULL )
        return( ERROR );
    if( sym->state != SYM_UNDEFINED ) {
        AsmError( SYMBOL_ALREADY_DEFINED );
        return( ERROR );
    }
    sym->state = SYM_INTERNAL;
    sym->addr = AsmCodeAddress;
    //  it should define label type ?????
    sym->mem_type = mem_type;  // fixme ??
#endif
    BackPatch( sym );
    return( NOT_ERROR );
}

#if defined( _STANDALONE_ )
int LabelDirective( int i )
/***************************/
{
    int n;

    if( Options.mode & MODE_IDEAL ) {
        n = ++i;
    } else {
        n = i - 1;
    }
    if( ( n < 0 ) || ( AsmBuffer[n]->token != T_ID ) ) {
        AsmError( INVALID_LABEL_DEFINITION );
        return( ERROR );
    }
    if( AsmBuffer[++i]->token == T_ID ) {
        if( IsLabelStruct( AsmBuffer[i]->string_ptr ) )
            return( MakeLabel( AsmBuffer[n]->string_ptr, MT_STRUCT ) );
    }
    if( ( AsmBuffer[i]->token != T_RES_ID ) &&
        ( AsmBuffer[i]->token != T_DIRECTIVE ) ) {
        AsmError( INVALID_LABEL_DEFINITION );
        return( ERROR );
    }
    switch( AsmBuffer[i]->u.value ) {
    case T_NEAR:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_NEAR ));
    case T_FAR:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_FAR ));
    case T_BYTE:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_BYTE ));
    case T_WORD:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_WORD ));
    case T_DWORD:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_DWORD ));
    case T_FWORD:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_FWORD ));
    case T_PWORD:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_FWORD ));
    case T_QWORD:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_QWORD ));
    case T_TBYTE:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_TBYTE ));
    case T_OWORD:
        return( MakeLabel( AsmBuffer[n]->string_ptr, MT_OWORD ));
    case T_PROC:
        return( MakeLabel( AsmBuffer[n]->string_ptr, CurrProc->e.procinfo->mem_type ));
    default:
        AsmError( INVALID_LABEL_DEFINITION );
        return( ERROR );
    }
}
#endif
