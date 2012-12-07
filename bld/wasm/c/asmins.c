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


/* set this macro to 0 if you do NOT want to allow structures to be defined
 * ie:
 *    struct foo
 *      ...
 *    foo ends
 *    ...
 *    bar foo < 4, 2, 3 >
 */
#define ALLOW_STRUCT_INIT 1

#include "asmglob.h"
#include "hash.h"
#include "asmexpnd.h"
#include "asmfixup.h"
#include "asmeval.h"
#include "asmlabel.h"

#if defined( _STANDALONE_ )
  #include "directiv.h"
  #include "myassert.h"
  #include "asminput.h"
#endif

#include "asminsd.h"
#include "asmopsd.gh"

extern int              match_phase_1( void );
extern int              ptr_operator( memtype, uint_8 );
extern int              jmp( expr_list * );

static struct asm_code  Code_Info;
struct asm_code         *Code = &Code_Info;

unsigned char           Opnd_Count;

static void             SizeString( unsigned op_size );
static int              check_size( void );
static int              segm_override_jumps( expr_list *opndx );


#if defined( _STANDALONE_ )
extern int              directive( int , long );
extern int              SymIs32( struct asm_sym *sym );

extern int_8            DefineProc;     // TRUE if the definition of procedure
                                        // has not ended

static void             check_assume( struct asm_sym *, enum prefix_reg );

uint_8                  CheckSeg;       // if checking of opened segment is needed
struct asm_sym          *Frame;         // Frame of current fixup
struct asm_sym          *SegOverride;

static int              in_epilogue = 0;

#else

#define     directive( i, value )   cpu_directive( value )

#endif

extern void             make_inst_hash_table( void );

static int              curr_ptr_type;
static char             ConstantOnly;

static int              mem2code( char, int, int, asm_sym * );

/* moved here from asmline */
static asm_tok          tokens[MAX_TOKEN];

asm_tok                 *AsmBuffer[MAX_TOKEN];  // buffer to store token

#if defined( _STANDALONE_ )
void find_frame( struct asm_sym *sym )
/*******************************************/
{
    if( SegOverride != NULL ) {
        sym = SegOverride;
        if( sym->state == SYM_GRP ) {
            Frame = sym;
        } else if( sym->segment != NULL ) {
            Frame = sym->segment;
        }
    } else {
        switch( sym->state ) {
        case SYM_INTERNAL:
        case SYM_PROC:
            if( sym->segment != NULL ) {
                if( GetGrp( sym ) != NULL ) {
                    Frame = GetGrp( sym );
                } else {
                    Frame = sym->segment;
                }
            }
            break;
        case SYM_GRP:
            Frame = sym;
            break;
        case SYM_SEG:
            Frame = sym->segment;
            break;
        default:
            break;
        }
    }
}

void GetInsString( asm_token token, char *string, int len )
/**************************************************************/
{
    int index;

    if( len > AsmOpcode[ token ].len ) {
        len = AsmOpcode[ token ].len;
        index = AsmOpcode[ token ].index;
        if( AsmChars[index]== '.' ) {
            index++;
            len--;
        }
        strncpy( string, &(AsmChars[ index ]), len );
        string[ len ] = '\0';
    } else {
        *string='\0';
    }
    return;
}
#endif

int get_instruction_position( char *string )
/******************************************/
{
    unsigned short  i;

    for( i = inst_table[ hashpjw( string ) ]; i--; i = AsmOpcode[ i ].next ) {
        if( strnicmp( string, &AsmChars[ AsmOpcode[ i ].index ], AsmOpcode[ i ].len ) == 0
            && string[ AsmOpcode[ i ].len ] == '\0' ) {
            return( AsmOpcode[ i ].position );
        }
    }
    return( EMPTY );
}

static int comp_mem( int reg1, int reg2 )
/***************************************/
/*
- compare and return the r/m field encoding of 16-bit address mode;
- call by mem2code() only;
*/
{
    switch( reg1 ) {
    case T_BX:
        switch( reg2 ) {
        case T_SI: return( MEM_BX_SI );
        case T_DI: return( MEM_BX_DI );
        }
        break;
    case T_BP:
        switch( reg2 ) {
        case T_SI: return( MEM_BP_SI );
        case T_DI: return( MEM_BP_DI );
        }
        break;
    }
    AsmError( INVALID_MEMORY_POINTER );
    return( ERROR );
}

static void seg_override( int seg_reg, asm_sym *sym )
/***************************************************/
/*
- determine if segment override is necessary with the current address mode;
*/
{
    enum prefix_reg     default_seg;
#if defined( _STANDALONE_ )
    enum assume_reg     assume_seg;

    switch( seg_reg ) {
    case T_SS:
    case T_BP:
    case T_EBP:
    case T_ESP:
        default_seg = PREFIX_SS;
        break;
    default:
        default_seg = PREFIX_DS;
    }
    if( sym != NULL ) {
        if( Code->prefix.seg == EMPTY ) {
            if( Code->info.token == T_LEA ) {
                check_assume( sym, EMPTY );
            } else {
                check_assume( sym, default_seg );
            }
        } else {
            switch( Code->prefix.seg ) {
            case PREFIX_ES:
                assume_seg = ASSUME_ES;
                break;
            case PREFIX_CS:
                assume_seg = ASSUME_CS;
                break;
            case PREFIX_SS:
                assume_seg = ASSUME_SS;
                break;
            case PREFIX_DS:
                assume_seg = ASSUME_DS;
                break;
            case PREFIX_FS:
                assume_seg = ASSUME_FS;
                break;
            case PREFIX_GS:
                assume_seg = ASSUME_GS;
                break;
            default:
                break;
            }
            if( GetPrefixAssume( sym, assume_seg ) == ASSUME_NOTHING ) {
//                AsmWarn( 3, CANNOT_ADDRESS_WITH_ASSUMED_REGISTER );
            }
        }
    }

    if( Code->prefix.seg == default_seg ) {
        Code->prefix.seg = EMPTY;
    }
#else
    if( Code->prefix.seg != EMPTY ) {
        switch( seg_reg ) {
        case T_SS:
        case T_BP:
        case T_EBP:
        case T_ESP:
            default_seg = PREFIX_SS;
            break;
        default:
            default_seg = PREFIX_DS;
        }
        if( Code->prefix.seg == default_seg ) {
            Code->prefix.seg = EMPTY;
        }
    }
#endif
}

#if defined( _STANDALONE_ )

static void check_assume( struct asm_sym *sym, enum prefix_reg default_reg )
/**************************************************************************/
/* Check if an assumed register is found, and prefix a register if necessary */
{
    enum assume_reg     reg;
    enum assume_reg     def_reg;

    /**/myassert( sym != NULL );
    if( sym->state == SYM_UNDEFINED )
        return;

    switch( default_reg ) {
    case PREFIX_SS:
        def_reg = ASSUME_SS;
        break;
    case PREFIX_DS:
        def_reg = ASSUME_DS;
        break;
    default:
        def_reg = ASSUME_NOTHING;
        break;
    }

    reg = GetAssume( sym, def_reg );

    if( reg == ASSUME_NOTHING ) {
        if( ( sym->state != SYM_EXTERNAL ) && ( sym->state != SYM_PROC ) ) {
            if( Parse_Pass == PASS_2 ) {
                AsmWarn( 3, CANNOT_ADDRESS_WITH_ASSUMED_REGISTER );
            }
        } else {
            Code->prefix.seg = default_reg;
        }
    } else if( default_reg != EMPTY ) {
        switch( reg ) {
        case ASSUME_ES:
            Code->prefix.seg = PREFIX_ES;
            break;
        case ASSUME_CS:
            Code->prefix.seg = PREFIX_CS;
            break;
        case ASSUME_DS:
            Code->prefix.seg = PREFIX_DS;
            break;
        case ASSUME_GS:
            Code->prefix.seg = PREFIX_GS;
            break;
        case ASSUME_FS:
            Code->prefix.seg = PREFIX_FS;
            break;
        case ASSUME_SS:
            Code->prefix.seg = PREFIX_SS;
            break;
        default:
            break;
        }
    }
}

int check_override( int *i )
/**************************/
/* Check if there is a register, segment or group override */
{
    int         index;

    index = *i;

    if( ( index + 2 ) < Token_Count ) {
        if( AsmBuffer[index+1]->token == T_COLON ) {
            switch( AsmBuffer[index]->token ) {
            case T_REG:
                Code->prefix.seg =
                    AsmOpTable[AsmOpcode[AsmBuffer[index]->u.value].position].opcode;
                (*i) += 2;
                if( *i >= Token_Count ) {
                    AsmError( LABEL_EXPECTED_AFTER_COLON );
                    return( ERROR );
                }
                break;
            case T_ID:      // Segment or Group override
                if( FixOverride(*i) != NOT_ERROR ) {
                    return( ERROR );
                }
                (*i) += 2;
                if( *i >= Token_Count ) {
                    AsmError( LABEL_EXPECTED_AFTER_COLON );
                    return( ERROR );
                }
                break;
            default:
                break;
            }
        }
    }
    return( NOT_ERROR );
}
#endif

static int Reg386( int reg_token )
/********************************/
{
    switch( reg_token ) {
    case T_EAX:         return( 0 );
    case T_ECX:         return( 1 );
    case T_EDX:         return( 2 );
    case T_EBX:         return( 3 );
    case T_ESP:         return( 4 );
    case T_EBP:         return( 5 );
    case T_ESI:         return( 6 );
    case T_EDI:         return( 7 );
    }
    /* I don't think this should happen */
    return( 0 );
}

int OperandSize( OPNDTYPE opnd )
/******************************/
{
    if( ( opnd == OP_NONE ) || ( opnd & OP_SPECIAL ) ) {
        return( 0 );
    } else if( opnd == OP_M ) {
        /* fixme */
        switch( Code->mem_type ) {
        case MT_EMPTY:  return( 0 );
#if defined( _STANDALONE_ )
        case MT_SBYTE:
#endif
        case MT_BYTE:    return( 1 );
#if defined( _STANDALONE_ )
        case MT_SWORD:
#endif
        case MT_WORD:    return( 2 );
#if defined( _STANDALONE_ )
        case MT_SDWORD:
#endif
        case MT_DWORD:   return( 4 );
        case MT_FWORD:   return( 6 );
        case MT_QWORD:   return( 8 );
        case MT_TBYTE:   return( 10 );
        case MT_OWORD:   return( 16 );
        default:         break;
        }
    } else if( opnd & ( OP_M8_R8 | OP_M_B | OP_I8 | OP_I_1 | OP_I_3 | OP_I8_U ) ) {
        return( 1 );
    } else if( opnd & ( OP_M16_R16 | OP_M_W | OP_I16 | OP_SR ) ) {
        return( 2 );
    } else if( opnd & ( OP_R32 | OP_M_DW | OP_I32 | OP_J32 | OP_SPEC_REG ) ) {
        return( 4 );
    } else if( opnd & ( OP_I | OP_J48 ) ) {
        return( 6 );
    } else if( opnd & ( OP_M_QW | OP_MMX ) ) {
        return( 8 );
    } else if( opnd & ( OP_STI | OP_M_TB ) ) {
        return( 10 );
    } else if( opnd & ( OP_M_OW | OP_XMM ) ) {
        return( 16 );
    }
    return( 0 );
}

int InRange( unsigned long val, unsigned bytes )
/**********************************************/
/*
     Can 'val' be represented in 'bytes' bytes?
*/
{
    unsigned long max;
    unsigned long mask;

    max = ( 1UL << ( bytes * 8 ) ) - 1;
    if( val <= max ) /* absolute value fits */
        return( 1 );
    mask = ~(max >> 1);
    if( ( val & mask ) == mask ) /* just a sign extension */
        return( 1 );
    return( 0 );

}

static int mem2code( char ss, int index, int base, asm_sym *sym )
/***************************************************************/
/*
  encode the memory operand to machine code
*/
{
    struct asm_code     *rCode = Code;
    int                 temp;
    unsigned char       mod_field;
    unsigned char       rm_field;

    // clear mod
    rm_field = 0;
    if( InsFixups[Opnd_Count] != NULL ) {
        mod_field = MOD_10;
    } else if( rCode->data[Opnd_Count] == 0 ) {
        mod_field = MOD_00;
    } else if( ( rCode->data[Opnd_Count] > SCHAR_MAX )
       || ( rCode->data[Opnd_Count] < SCHAR_MIN ) ) {
        mod_field = MOD_10;
    } else {
        mod_field = MOD_01;
    }
    if( ( index == EMPTY ) && ( base == EMPTY ) ) {
        // direct memory
        // clear the rightmost 3 bits
        mod_field = MOD_00;
        if( !addr_32( rCode ) ) {
            if( !InRange( rCode->data[Opnd_Count], 2 ) ) {
                // expect 16-bit but got 32-bit address
                AsmError( DISPLACEMENT_OUT_OF_RANGE );
                return( ERROR );
            }
            rm_field = D16;
        } else {
            rm_field = D32;
        }
        // default is DS:[], DS: segment override is not needed
        seg_override( T_DS, sym );
    } else if( ( index == EMPTY ) && ( base != EMPTY ) ) {
        switch( base ) {
        case T_SI:
            rm_field = 0x04; // SI
            // default is DS:[], DS: segment override is not needed
            break;
        case T_DI:
            rm_field = 0x05; // DI
            // default is DS:[], DS: segment override is not needed
            break;
        case T_BP:
            rm_field = BP;
            if( mod_field == MOD_00 ) {
                mod_field = MOD_01;
            }
            // default is SS:[], SS: segment override is not needed
            break;
        case T_BX:
            rm_field = 0x07; // BX
            // default is DS:[], DS: segment override is not needed
            break;
        case T_EBP:
            rm_field = EBP;
            if( mod_field == MOD_00 ) {
                mod_field = MOD_01;
            }
            // default is SS:[], SS: segment override is not needed
            break;
        case T_ESP:
            rm_field = ESP;
            // ss = 00, index = 100 ( no index ), base = 100 ( ESP )
            rCode->sib = 0x24;
            // default is SS:[], SS: segment override is not needed
            break;
        default: // for 386 and up
            rm_field = Reg386( base );
            // default is DS:[], DS: segment override is not needed
        }
        seg_override( base, sym );
    } else if( ( index != EMPTY ) && ( base == EMPTY ) ) {
        // mod field is 00
        mod_field = MOD_00;
        // s-i-b is present ( r/m = 100 )
        rm_field = S_I_B;
        // scale factor, index, base ( 0x05 => no base reg )
        rCode->sib = ( ss | ( Reg386(index) << 3 ) | 0x05 );
        // default is DS:[], DS: segment override is not needed
        seg_override( T_DS, sym );
    } else {
        // base != EMPTY && index != EMPTY
        switch( index ) {
        case T_BX:
        case T_BP:
            if( ( temp = comp_mem( index, base ) ) == ERROR )
                return( ERROR );
            rm_field = temp;
            seg_override( index, sym );
            break;
        case T_SI:
        case T_DI:
            if( ( temp = comp_mem( base, index ) ) == ERROR )
                return( ERROR );
            rm_field = temp;
            seg_override( base, sym );
            break;
        case T_ESP:
            AsmError( ESP_CANNOT_BE_USED_AS_INDEX );
            return( ERROR );
        default:
            if( base < T_EAX ) {
                AsmError( CANNOT_MIX_16_AND_32_BIT_REGISTERS );
                return( ERROR );
            } else if( base == T_EBP ) {
                if( mod_field == MOD_00 ) {
                    mod_field = MOD_01;
                }
            }
            // s-i-b is present ( r/m = 100 )
            rm_field |= S_I_B;
            rCode->sib = ( ss | (Reg386(index) << 3) | Reg386(base) );
            seg_override( base, sym );
        }
    }
    if( Opnd_Count == OPND2 ) {
        // shift the register field to left by 3 bit
        rCode->info.rm_byte = mod_field | ( rm_field << 3 ) | ( rCode->info.rm_byte & BIT_012 );
    } else if( Opnd_Count == OPND1 ) {
        rCode->info.rm_byte = mod_field | rm_field;
    }
    return( NOT_ERROR );
}

static int comp_opt( uint direct )
/********************************/
/*
  Compare function for CPU directive
*/
{
    switch( direct ) {
    case T_DOT_NO87:
        return( P_NO87 );
    case T_DOT_8086:
#if defined( _STANDALONE_ )
    case T_P8086:
#endif
        return( P_86 );
    case T_DOT_8087:
#if defined( _STANDALONE_ )
    case T_P8087:
#endif
        return( P_87 );
    case T_DOT_186:
#if defined( _STANDALONE_ )
    case T_P186:
#endif
        return( P_186 );
    case T_DOT_286:
#if defined( _STANDALONE_ )
    case T_P286:
#endif
        return( P_286 );
    case T_DOT_287:
#if defined( _STANDALONE_ )
    case T_P287:
#endif
        return( P_287 );
    case T_DOT_286P:
#if defined( _STANDALONE_ )
    case T_P286P:
#endif
        return( P_286p );
    case T_DOT_386:
#if defined( _STANDALONE_ )
    case T_P386:
#endif
        return( P_386 );
    case T_DOT_387:
#if defined( _STANDALONE_ )
    case T_P387:
#endif
        return( P_387 );
    case T_DOT_386P:
#if defined( _STANDALONE_ )
    case T_P386P:
#endif
        return( P_386p );
    case T_DOT_486:
#if defined( _STANDALONE_ )
    case T_P486:
#endif
        return( P_486 );
    case T_DOT_486P:
#if defined( _STANDALONE_ )
    case T_P486P:
#endif
        return( P_486p );
    case T_DOT_586:
#if defined( _STANDALONE_ )
    case T_P586:
#endif
        return( P_586 );
    case T_DOT_586P:
#if defined( _STANDALONE_ )
    case T_P586P:
#endif
        return( P_586p );
    case T_DOT_686:
#if defined( _STANDALONE_ )
    case T_P686:
#endif
        return( P_686 );
    case T_DOT_686P:
#if defined( _STANDALONE_ )
    case T_P686P:
#endif
        return( P_686p );
    case T_DOT_MMX:
#if defined( _STANDALONE_ )
    case T_PMMX:
#endif
        return( P_MMX );
    case T_DOT_K3D:
#if defined( _STANDALONE_ )
    case T_PK3D:
#endif
        return( P_K3D | P_MMX );
    case T_DOT_XMM:
#if defined( _STANDALONE_ )
    case T_PXMM:
#endif
        return( P_SSE | P_MMX );
    case T_DOT_XMM2:
#if defined( _STANDALONE_ )
    case T_PXMM2:
#endif
        return( P_SSE2 | P_SSE | P_MMX );
    case T_DOT_XMM3:
#if defined( _STANDALONE_ )
    case T_PXMM3:
#endif
        return( P_SSE3 | P_SSE2 | P_SSE | P_MMX );
    default:
        // not found
        return( EMPTY );
    }
}

static int def_fpu( uint direct )
/********************************/
/*
  get FPU from CPU directive
*/
{
    switch( direct ) {
    case T_DOT_8086:
    case T_DOT_186:
#if defined( _STANDALONE_ )
    case T_P8086:
    case T_P186:
#endif
        return( P_87 );
    case T_DOT_286:
    case T_DOT_286P:
#if defined( _STANDALONE_ )
    case T_P286:
    case T_P286P:
#endif
        return( P_287 );
    case T_DOT_386:
    case T_DOT_386P:
    case T_DOT_486:
    case T_DOT_486P:
    case T_DOT_586:
    case T_DOT_586P:
    case T_DOT_686:
    case T_DOT_686P:
#if defined( _STANDALONE_ )
    case T_P386:
    case T_P386P:
    case T_P486:
    case T_P486P:
    case T_P586:
    case T_P586P:
    case T_P686:
    case T_P686P:
#endif
        return( P_387 );
    default:
        return( 0 );
    }
}

#if defined( _STANDALONE_ )
static void MakeCPUConstant( asm_token tok )
/******************************************/
{
    char    buffer[MAX_LINE_LEN];

    GetInsString( tok, buffer, MAX_LINE_LEN );
    MakeConstantUnderscored( buffer );

    switch( tok ) {
    // fall right through
    case T_DOT_686P:
    case T_P686P:
    case T_DOT_686:
    case T_P686:
        MakeConstantUnderscored( "686" );
    case T_DOT_586P:
    case T_P586P:
    case T_DOT_586:
    case T_P586:
        MakeConstantUnderscored( "586" );
    case T_DOT_486P:
    case T_P486P:
    case T_DOT_486:
    case T_P486:
        MakeConstantUnderscored( "486" );
    case T_DOT_386P:
    case T_P386P:
    case T_DOT_386:
    case T_P386:
        MakeConstantUnderscored( "386" );
        break;
    case T_DOT_286P:
    case T_P286P:
    case T_DOT_286:
    case T_P286:
        MakeConstantUnderscored( "286" );
        break;
    }
    return;
}
#endif

int cpu_directive( int i )
/************************/
{
    int                 temp;

    if( (temp = comp_opt( i )) != EMPTY ) {
        if( i == T_DOT_NO87 ) {
            Code->info.cpu &= ~P_FPU_MASK;                 // turn off FPU bits
        } else if( temp & P_EXT_MASK ) {
            Code->info.cpu |= temp & P_EXT_MASK;           // turn on desired bit(s)
        } else if( temp & P_FPU_MASK ) {
            Code->info.cpu &= ~P_FPU_MASK;
            Code->info.cpu |= temp & P_FPU_MASK;           // setup FPU bits
        } else {
            Code->info.cpu &= ~( P_CPU_MASK | P_PM );
            Code->info.cpu |= temp & ( P_CPU_MASK | P_PM );// setup CPU bits
            Code->info.cpu &= ~P_FPU_MASK;
            Code->info.cpu |= def_fpu( i ) & P_FPU_MASK;   // setup FPU bits
        }
    } else {
        AsmError( UNKNOWN_DIRECTIVE );
        return( ERROR );
    }

#if defined( _STANDALONE_ )
    MakeCPUConstant( i );
    switch( i ) {
    case T_DOT_686P:
    case T_P686P:
    case T_DOT_686:
    case T_P686:
    case T_DOT_586P:
    case T_P586P:
    case T_DOT_586:
    case T_P586:
    case T_DOT_486P:
    case T_P486P:
    case T_DOT_486:
    case T_P486:
    case T_DOT_386P:
    case T_P386P:
    case T_DOT_386:
    case T_P386:
        SetUse32Def( TRUE );
        break;
    case T_DOT_286P:
    case T_P286P:
    case T_DOT_286C:
    case T_P286N:
    case T_DOT_286:
    case T_P286:
    case T_DOT_186:
    case T_P186:
    case T_DOT_8086:
    case T_P8086:
        SetUse32Def( FALSE );
        break;
    default:
        // set FPU
        break;
    }
#endif
    return( NOT_ERROR );
}

static int idata_float( long value )
/**********************************/
/*
  check the correct operand/data size for float immediate operand;
*/
{
    switch( Code->mem_type ) {
    case MT_EMPTY:
        if( Code->info.token == T_PUSHW ) { // sigh. another special case
            // expect 32-bit code but get 16-bit
            AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
            return( ERROR );
        }
        break;
    case MT_FAR:
    case MT_NEAR:
    case MT_SHORT:
#if defined( _STANDALONE_ )
    case MT_PROC:
#endif
        AsmError( SYNTAX_ERROR );
        return( ERROR );
#if defined( _STANDALONE_ )
    case MT_SBYTE:
    case MT_SWORD:
#endif
    case MT_BYTE:
    case MT_WORD:
        AsmError( OPERANDS_MUST_BE_THE_SAME_SIZE );
        return( ERROR );
#if defined( _STANDALONE_ )
    case MT_SDWORD:
#endif
    case MT_DWORD:
        // set w-bit
        Code->info.opcode |= W_BIT;
        break;
    default:
        break;
    }
    SET_OPSIZ_32( Code );
    Code->info.opnd_type[Opnd_Count] = OP_I32;
    Code->data[Opnd_Count] = value;
    return( NOT_ERROR );
}

static unsigned char get_sr_rm_byte( enum prefix_reg seg_prefix )
/***************************************************************/
{
    switch( seg_prefix ) {
    case PREFIX_ES:
        return( 0 );
    case PREFIX_CS:
        return( 1 );
    case PREFIX_SS:
        return( 2 );
    case PREFIX_DS:
        return( 3 );
    case PREFIX_FS:
        return( 4 );
    case PREFIX_GS:
        return( 5 );
    default:
        break;
    }
#if defined( _STANDALONE_ )
    /**/myassert( 0 );
#endif
    return( 0 );
}

#if defined( _STANDALONE_ )

static int proc_check( void )
/***************************/
/* Check if we are inside a procedure and write prologue statements if the
   current line is the first instruction line following the procedure
   declaration */
{
    if( ( CurrProc == NULL ) || ( Token_Count == 0 ) || !DefineProc )
        return( FALSE );

    if( AsmBuffer[0]->token == T_DIRECTIVE )
        return( FALSE );

    if( Token_Count > 1 ) {
        if( ( AsmBuffer[1]->token == T_DIRECTIVE )
            || ( AsmBuffer[1]->token == T_DIRECT_EXPR ) ) {
            return( FALSE );
        }
    }

    if( WritePrologue() == ERROR )
        return( ERROR );
    DefineProc = FALSE;
    return( TRUE );
}

char *regs[4] = { "ax",  "dx",  "bx",  "cx" };

int get_register_argument( int index, char *buffer, int *register_count )
{
    int     size, i, j;
    char    ch;

    i = index;
    j = *register_count;
    if( j > 3 )
        return( TRUE );
    if( Use32 )
        size = 4;
    else
        size = 2;
    if( AsmBuffer[i]->token == T_OP_SQ_BRACKET ) {
        i++;
        if( AsmBuffer[i]->token == T_RES_ID ) {
            switch( AsmBuffer[i]->u.value ) {
            case T_BYTE:
                size = 1;
                break;
            case T_WORD:
                size = 2;
                break;
            case T_DWORD:
                size = 4;
                break;
            case T_FWORD:
                if( Use32 ) {
                    size = 6;
                    break;
                }
            case T_QWORD:
                if( Use32 ) {
                    size = 8;
                    break;
                }
            default:
                AsmError( STRANGE_PARM_TYPE );
                return( ERROR );
            }
            i++;
        }
        if( ( AsmBuffer[i]->token != T_ID ) &&
            ( AsmBuffer[i+1]->token != T_CL_SQ_BRACKET ) ) {
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        }
        if( Use32 ) {
            switch( size ) {
            case 1:
                sprintf( buffer, "movzx e%s,[byte %s]", regs[j], AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( FALSE );
            case 2:
                sprintf( buffer, "movzx e%s,[word %s]", regs[j], AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( FALSE );
            case 4:
                sprintf( buffer, "mov e%s,[dword %s]", regs[j], AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( FALSE );
            case 6:
                if( j > 2 ) {
                    return( TRUE );
                } else {
                    sprintf( buffer, "mov e%s,[dword %s]", regs[j], AsmBuffer[i]->string_ptr );
                    InputQueueLine( buffer );
                    j++;
                    sprintf( buffer, "movzx e%s,[word %s+4]", regs[j], AsmBuffer[i]->string_ptr );
                    InputQueueLine( buffer );
                    *register_count = j;
                    return( FALSE );
                }
            case 8:
                if( j > 2 ) {
                    return( TRUE );
                } else {
                    sprintf( buffer, "mov e%s,[dword %s]", regs[j], AsmBuffer[i]->string_ptr );
                    InputQueueLine( buffer );
                    j++;
                    sprintf( buffer, "movzx e%s,[dword %s+4]", regs[j], AsmBuffer[i]->string_ptr );
                    InputQueueLine( buffer );
                    *register_count = j;
                    return( FALSE );
                }
            }
        } else {
            switch( size ) {
            case 1:
                ch = regs[j][0];
                sprintf( buffer, "xor %ch,%ch", ch, ch );
                InputQueueLine( buffer );
                sprintf( buffer, "mov %cl,[byte %s]", ch, AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( FALSE );
            case 2:
                sprintf( buffer, "mov %s,[word %s]", regs[j], AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( FALSE );
            case 4:
                if( j > 2 ) {
                    return( TRUE );
                } else {
                    sprintf( buffer, "mov %s,[word %s]", regs[j], AsmBuffer[i]->string_ptr );
                    InputQueueLine( buffer );
                    j++;
                    sprintf( buffer, "movzx %s,[word %s+2]", regs[j], AsmBuffer[i]->string_ptr );
                    InputQueueLine( buffer );
                    *register_count = j;
                    return( FALSE );
                }
            }
        }
    } else if( ( AsmBuffer[i]->token == T_REG ) &&
               ( ( AsmBuffer[i+1]->token == T_COMMA ) ||
                 ( AsmBuffer[i+1]->token == T_FINAL ) ) ) {
        switch( AsmBuffer[i]->u.value ) {
        case T_EAX:
        case T_EBX:
        case T_ECX:
        case T_EDX:
        case T_EDI:
        case T_ESI:
        case T_EBP:
        case T_ESP:
            if( Use32 )
                size = 4;
            break;
        case T_AX:
        case T_BX:
        case T_CX:
        case T_DX:
        case T_DI:
        case T_SI:
        case T_BP:
        case T_SP:
            size = 2;
            break;
        case T_DS:
        case T_CS:
        case T_ES:
        case T_SS:
        case T_FS:
        case T_GS:
            if( Use32 )
                size = 4;
            else
                size = 2;
            break;
        case T_AL:
        case T_AH:
        case T_BL:
        case T_BH:
        case T_CL:
        case T_CH:
        case T_DL:
        case T_DH:
            size = 1;
            break;
        }
        switch( size ) {
        case 1:
            if( Use32 ) {
                sprintf( buffer, "movzx e%s,%s", regs[j], AsmBuffer[i]->string_ptr );
            } else {
                ch = regs[j][0];
                sprintf( buffer, "mov %cl,%s", ch, AsmBuffer[i]->string_ptr );
            }
            InputQueueLine( buffer );
            return( FALSE );
        case 2:
            if( Use32 ) {
                sprintf( buffer, "movzx e%s,%s", regs[j], AsmBuffer[i]->string_ptr );
            } else {
                sprintf( buffer, "mov %s,%s", regs[j], AsmBuffer[i]->string_ptr );
            }
            InputQueueLine( buffer );
            return( FALSE );
        case 4:
            if( Use32 ) {
                sprintf( buffer, "mov e%s,%s", regs[j], AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( FALSE );
            }
            break;
        }
    } else if( AsmBuffer[i]->token == T_QUESTION_MARK ) {
        return FALSE;
    } else {
        if( Use32 )
            sprintf( buffer, "mov e%s,", regs[j] );
        else
            sprintf( buffer, "mov %s,", regs[j] );
        while( ( AsmBuffer[i]->token != T_FINAL ) &&
               ( AsmBuffer[i]->token != T_COMMA ) ) {
            strcat( buffer, AsmBuffer[i++]->string_ptr );
        }
        InputQueueLine( buffer );
        return( FALSE );
    }
    AsmError( SYNTAX_ERROR );
    return( ERROR );
}

int get_stack_argument( int index, char *buffer )
{
    int     size, i = index;

    if( Use32 )
        size = 4;
    else
        size = 2;
    if( AsmBuffer[i]->token == T_OP_SQ_BRACKET ) {
        i++;
        if( AsmBuffer[i]->token == T_RES_ID ) {
            switch( AsmBuffer[i]->u.value ) {
            case T_BYTE:
                size = 1;
                break;
            case T_WORD:
                size = 2;
                break;
            case T_DWORD:
                size = 4;
                break;
            case T_FWORD:
                if( Use32 ) {
                    size = 6;
                    break;
                }
            case T_QWORD:
                if( Use32 ) {
                    size = 8;
                    break;
                }
            default:
                AsmError( STRANGE_PARM_TYPE );
                return( ERROR );
            }
            i++;
        }
        if( ( AsmBuffer[i]->token != T_ID ) ||
            ( AsmBuffer[i + 1]->token != T_CL_SQ_BRACKET ) ) {
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        }
        if( Use32 ) {
            switch( size ) {
            case 1:
                sprintf( buffer, "movzx eax,[byte %s]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push eax" );
                InputQueueLine( buffer );
                return( 0 );
            case 2:
                sprintf( buffer, "movzx eax,[word %s]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push eax" );
                InputQueueLine( buffer );
                return( 0 );
            case 4:
                sprintf( buffer, "push [dword %s]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( 0 );
            case 6:
                sprintf( buffer, "movzx eax,[word %s+4]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push eax" );
                InputQueueLine( buffer );
                sprintf( buffer, "push [dword %s]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( 0 );
            case 8:
                sprintf( buffer, "push [dword %s+4]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push [dword %s]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( 0 );
            }
        } else {
            switch( size ) {
            case 1:
                sprintf( buffer, "xor ah,ah" );
                InputQueueLine( buffer );
                sprintf( buffer, "mov al,[%s]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push ax" );
                InputQueueLine( buffer );
                return( 0 );
            case 2:
                sprintf( buffer, "push [%s]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( 0 );
            case 4:
                sprintf( buffer, "push [word %s+2]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                sprintf( buffer, "push [word %s]", AsmBuffer[i]->string_ptr );
                InputQueueLine( buffer );
                return( 0 );
            }
        }
    }
    sprintf( buffer, "push " );
    while( ( AsmBuffer[i]->token != T_FINAL ) &&
           ( AsmBuffer[i]->token != T_COMMA ) ) {
        strcat( buffer, AsmBuffer[i++]->string_ptr );
    }
    InputQueueLine( buffer );
    return( 0 );
}

int expand_call( int index, int lang_type )
{
    int         i, j;
    int         arglist[16];
    int         argcount, cleanup, reversed, register_count, register_arguments, parameter_on_stack;
    char        buffer[ MAX_LINE_LEN ];

    argcount = cleanup = reversed = register_count = register_arguments = 0;
    parameter_on_stack = TRUE;
    switch( lang_type ) {
    case LANG_C:
    case LANG_SYSCALL:
        cleanup++;
        reversed++;
        break;
    case LANG_WATCOM_C:
        if( Options.watcom_parms_passed_by_regs || !Use32 ) {
            parameter_on_stack = FALSE;
        } else {
            cleanup++;
        }
        /* fall into T_STDCALL */
    case LANG_STDCALL:
        reversed++;
        break;
    case LANG_PASCAL:
    case LANG_FORTRAN:
    case LANG_BASIC:
        break;
    case LANG_NONE:
        if( AsmBuffer[index]->token == T_FINAL )
            break;
        AsmError( SYNTAX_ERROR );
        return ( ERROR );
    }
    for( i = index; AsmBuffer[i]->token != T_FINAL; ) {
        if( ( AsmBuffer[i]->token != T_COMMA ) ||
            ( AsmBuffer[i+1]->token == T_FINAL ) ) {
            AsmError( SYNTAX_ERROR );
            return ( ERROR );
        }
        if( argcount == 16 ) {
            AsmError( TOO_MANY_ARGS );
            return( ERROR );
        }
        for( j = ++i; ; j++ ) {
            if( ( AsmBuffer[j]->token == T_FINAL ) ||
                ( AsmBuffer[j]->token == T_COMMA ) ) {
                break;
            }
        }
        arglist[argcount++] = i;
        i = j;
    }
    if( parameter_on_stack == FALSE ) {
        for( i = 0; i < argcount; i++ ) {
            parameter_on_stack = get_register_argument( arglist[i], buffer,
                                                        &register_count );
            if( parameter_on_stack == ERROR )
                return( ERROR );
            if( parameter_on_stack )
                break;
            register_count++;
            register_arguments++;
        }
    }
    /* put parameters on top of stack */
    if( reversed ) {    /* Reversed order (right to left)*/

        i = argcount;
        while( i > register_arguments ) {
            j = arglist[--i];
            if( get_stack_argument( j, buffer ) )
                return( ERROR );
        }
    } else {
        for( i = 0; i < argcount; i++ ) {
            j = arglist[i];
            if( get_stack_argument( j, buffer ) )
                return( ERROR );
        }
    }
    *buffer = 0;
    /* add original line up to before language */
    for( i = 0; i < index - 1; i++ ) {
        sprintf( buffer + strlen(buffer), "%s ", AsmBuffer[i]->string_ptr );
    }
    InputQueueLine( buffer );
    /* add cleanup after call */
    if( cleanup && argcount ) {
        if( Code->use32 )
            sprintf( buffer, "add esp,%d", argcount << 2 );
        else
            sprintf( buffer, "add sp,%d", argcount << 1 );
        InputQueueLine( buffer );
    }
    return( NOT_ERROR );
}

#endif

static int process_jumps( expr_list *opndx )
/********************************************/
/*
  parse the jumps instructions operands
*/
{
    int         temp;
    bool        flag;

    segm_override_jumps( opndx );

    flag = ( opndx->explicit ) ? TRUE : FALSE ;
    if( ptr_operator( opndx->mem_type, flag ) == ERROR )
        return( ERROR );
    if( ptr_operator( MT_PTR, flag ) == ERROR ) {
        return( ERROR );
    }
    if( opndx->mbr != NULL ) {
        flag = FALSE;
        if( ptr_operator( opndx->mbr->mem_type, flag ) == ERROR )
            return( ERROR );
        if( ptr_operator( MT_PTR, flag ) == ERROR ) {
            return( ERROR );
        }
    }
    temp = jmp( opndx );
    switch( temp ) {
    case ERROR:
        return( ERROR );
    case SCRAP_INSTRUCTION:
        return( SCRAP_INSTRUCTION );
    case INDIRECT_JUMP:
        return( ERROR );
    default:
        return( NOT_ERROR );
    }
}

static int segm_override_jumps( expr_list *opndx )
/******************************************/
{
    if( opndx->override != EMPTY ) {
        if( AsmBuffer[opndx->override]->token == T_REG ) {
            Code->prefix.seg = AsmOpTable[AsmOpcode[AsmBuffer[opndx->override]->u.value].position].opcode;
        } else {
#if defined( _STANDALONE_ )
            if( FixOverride( opndx->override ) != NOT_ERROR ) {
                return( ERROR );
            }
#endif
        }
    } else {
    }
    return( NOT_ERROR );
}

static int segm_override_idata( expr_list *opndx )
/******************************************/
{
    if( opndx->override != EMPTY ) {
        if( AsmBuffer[opndx->override]->token == T_REG ) {
            Code->prefix.seg = AsmOpTable[AsmOpcode[AsmBuffer[opndx->override]->u.value].position].opcode;
        } else {
#if defined( _STANDALONE_ )
            if( FixOverride( opndx->override ) != NOT_ERROR ) {
                return( ERROR );
            }
#endif
        }
    } else {
    }
    return( NOT_ERROR );
}

static int segm_override_memory( expr_list *opndx )
/******************************************/
{
    if( opndx->override != EMPTY ) {
        if( AsmBuffer[opndx->override]->token == T_REG ) {
            Code->prefix.seg = AsmOpTable[AsmOpcode[AsmBuffer[opndx->override]->u.value].position].opcode;
        } else {
#if defined( _STANDALONE_ )
            if( FixOverride( opndx->override ) != NOT_ERROR ) {
                return( ERROR );
            }
#endif
        }
    } else {
    }
    return( NOT_ERROR );
}

static int idata_nofixup( expr_list *opndx )
/******************************************/
{
    OPNDTYPE    op_type = OP_NONE;
    long        value;

    if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
        return( process_jumps( opndx ) );
    }
    value = opndx->value;
    Code->data[Opnd_Count] = value;

    switch( Code->mem_type ) {
    case MT_EMPTY:
        if( Code->info.token == T_PUSH ) {
            if( opndx->explicit ) {
                if( opndx->mem_type == MT_BYTE ) {
                    op_type = OP_I8;
                } else if( opndx->mem_type == MT_WORD ) {
                    op_type = OP_I16;
                    SET_OPSIZ_16( Code );
                } else if( opndx->mem_type == MT_DWORD ) {
                    op_type = OP_I32;
                    SET_OPSIZ_32( Code );
                } else {
                    // FIXME !!
                    op_type = OP_I32;
                    SET_OPSIZ_32( Code );
                }
                break;
            }
            if( Code->use32 ) {
                if( (int_8)value == (int_32)value ) {
                    op_type = OP_I8;
                } else {
                    op_type = OP_I32;
                }
            } else if( (unsigned long)value > USHRT_MAX ) {
                SET_OPSIZ_32( Code );
                if( (int_8)value == (int_32)value ) {
                    op_type = OP_I8;
                } else {
                    op_type = OP_I32;
                }
            } else {
                if( (int_8)value == (int_16)value ) {
                    op_type = OP_I8;
                } else {
                    op_type = OP_I16;
                }
            }
            break;
        } else if( Code->info.token == T_PUSHW ) {
            op_type = OP_I16;
            if( (int_8)value == (int_16)value ) {
                op_type = OP_I8;
            }
            break;
        } else if( Code->info.token == T_PUSHD ) {
            op_type = OP_I32;
            if( (int_8)value == (int_32)value ) {
                op_type = OP_I8;
            }
            break;
        }
        // fall through
        if( ( value > SHRT_MAX ) || ( value < SHRT_MIN ) ) {
            op_type = OP_I32;
        } else if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            op_type = OP_I16;
        } else {
            op_type = OP_I8;
        }
        break;
    case MT_FAR:
        if( ( value > SHRT_MAX ) || ( value < SHRT_MIN ) ) {
            op_type = OP_I32;
        } else if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            op_type = OP_I16;
        } else {
            op_type = OP_I8;
        }
        break;
    case MT_NEAR:
        if( !Code->use32 ) {
            op_type = OP_I16;
        } else {
            op_type = OP_I32;
        }
        break;
    case MT_SHORT:
        if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            // expect 8-bit but got 16 bit
            AsmError( JUMP_OUT_OF_RANGE );
            return( ERROR );
        } else {
            op_type = OP_I8;
        }
        break;
    case MT_BYTE:
        if( !InRange( value, 1 ) ) {
            // expect 8-bit but got 16 bit
            AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
            return( ERROR );
        } else {
            op_type = OP_I8;
        }
        break;
#if defined( _STANDALONE_ )
    case MT_SBYTE:
        if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
            return( ERROR );
        } else {
            op_type = OP_I8;
        }
        break;
    case MT_SWORD:
        if( ( value > SHRT_MAX ) || ( value < SHRT_MIN ) ) {
            AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
            return( ERROR );
        } else if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            op_type = OP_I16;
        } else {
            op_type = OP_I8;
        }
        // set w-bit
        Code->info.opcode |= W_BIT;
        break;
#endif
    case MT_WORD:
#if defined( _STANDALONE_ )
        if( Options.sign_value ) {
            if( !InRange( value, 2 ) ) {
                AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
                return( ERROR );
            } else if( value > UCHAR_MAX ) {
                op_type = OP_I16;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
        } else {
#endif
            if( !InRange( value, 2 ) ) {
                AsmError( IMMEDIATE_DATA_OUT_OF_RANGE );
                return( ERROR );
            } else if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
                op_type = OP_I16;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
        }
#endif
        break;
#if defined( _STANDALONE_ )
   case MT_SDWORD:
        if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
            op_type = OP_I32;
        } else {
            op_type = OP_I8;
        }
        // set w-bit
        Code->info.opcode |= W_BIT;
        break;
#endif
    case MT_DWORD:
#if defined( _STANDALONE_ )
        if( Options.sign_value ) {
            if( value > UCHAR_MAX ) {
                op_type = OP_I32;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
        } else {
#endif
            if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
                op_type = OP_I32;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
        }
#endif
        break;
    case MT_QWORD:
#if defined( _STANDALONE_ )
        if( Options.sign_value ) {
            if( value > UCHAR_MAX ) {
                op_type = OP_I32;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
        } else {
#endif
            if( ( value > SCHAR_MAX ) || ( value < SCHAR_MIN ) ) {
                op_type = OP_I32;
            } else {
                op_type = OP_I8;
            }
            // set w-bit
            Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
        }
#endif
        break;
    default:
        break;
    }
    Code->info.opnd_type[Opnd_Count] = op_type;
    return( NOT_ERROR );
}

static int idata_fixup( expr_list *opndx )
/****************************************/
{
    struct asmfixup     *fixup;
    enum fixup_types    fixup_type;
    int                 type;
    int                 sym32;

    if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
        return( process_jumps( opndx ) );
    }
    Code->data[Opnd_Count] = opndx->value;
    segm_override_idata( opndx );

#if defined( _STANDALONE_ )
    if( ( opndx->sym->state == SYM_SEG )
        || ( opndx->sym->state == SYM_GRP )
        || ( opndx->instr == T_SEG ) ) {
        sym32 = 0;
    } else if( opndx->abs ) {
        sym32 = 0;
    } else {
        sym32 = SymIs32( opndx->sym );
    }
#else
    sym32 = Code->use32;
#endif
    if( opndx->instr != EMPTY ) {
        if( ( opndx->base_reg != EMPTY )
            || ( opndx->idx_reg != EMPTY ) ) {
            AsmError( INVALID_MEMORY_POINTER );
            return( ERROR );
        }
    }
    if( opndx->instr == T_SEG ) {
        if( opndx->sym->state == SYM_STACK ) {
            AsmError( CANNOT_SEG_AUTO );
            return( ERROR );
        }
    } else {
        if( MEM_TYPE( Code->mem_type, BYTE ) ) {
            AsmError( OFFSET_TOO_SMALL );
            return( ERROR );
        }
        if( opndx->sym->state == SYM_STACK ) {
            AsmError( CANNOT_OFFSET_AUTO );
            return( ERROR );
#if defined( _STANDALONE_ )
        } else if( opndx->sym->state == SYM_GRP ) {
            AsmError( CANNOT_OFFSET_GRP );
            return( ERROR );
#endif
        }
    }
    switch( Code->mem_type ) {
    case MT_EMPTY:
        if( Opnd_Count > OPND1 ) {
            type = OperandSize( Code->info.opnd_type[OPND1] );
            if( type == 4 ) {
                Code->mem_type = MT_DWORD;
                Code->info.opnd_type[Opnd_Count] = OP_I32;
                SET_OPSIZ_32( Code );
                break;
            } else if( type == 2 ) {
                Code->mem_type = MT_WORD;
                Code->info.opnd_type[Opnd_Count] = OP_I16;
                SET_OPSIZ_16( Code );
                break;
            }
        }
        if( opndx->abs ) {
            if( opndx->mem_type == MT_BYTE ) {
                Code->mem_type = MT_BYTE;
                Code->info.opnd_type[Opnd_Count] = OP_I8;
                break;
            } else if( opndx->mem_type == MT_EMPTY ) {
                SET_OPSIZ_NO( Code );
                if( oper_32( Code ) ) {
                    Code->mem_type = MT_DWORD;
                    Code->info.opnd_type[Opnd_Count] = OP_I32;
                    sym32 = 1;
                    break;
                }
            } else if( opndx->mem_type == MT_DWORD ) {
                sym32 = 1;
            }
        }
        if( ( Code->info.token == T_PUSHD ) || sym32 ) {
            Code->mem_type = MT_DWORD;
            Code->info.opnd_type[Opnd_Count] = OP_I32;
            SET_OPSIZ_32( Code );
            break;
        }
        Code->mem_type = MT_WORD;
        // no break
#if defined( _STANDALONE_ )
    case MT_SWORD:
#endif
    case MT_WORD:
        Code->info.opnd_type[Opnd_Count] = OP_I16;
        SET_OPSIZ_16( Code );
        break;
#if defined( _STANDALONE_ )
    case MT_SDWORD:
#endif
    case MT_DWORD:
        Code->info.opnd_type[Opnd_Count] = OP_I32;
        SET_OPSIZ_32( Code );
        break;
    default:
        break;
    }
    if( opndx->instr == T_SEG ) {
        fixup_type = FIX_SEG;
    } else {
        if( Code->mem_type == MT_BYTE ) {
            fixup_type = FIX_LOBYTE;
        } else if( oper_32( Code ) ) {
            fixup_type = ( sym32 ) ? FIX_OFF32 : FIX_OFF16;
        } else {
            if( sym32 ) {
                // fixme !!!! warning
                // operand size is 16bit
                // but fixup is 32-bit
            }
            fixup_type = FIX_OFF16;
        }
    }
    ConstantOnly = TRUE;
    Code->info.opcode |= W_BIT;

#if defined( _STANDALONE_ )
    find_frame( opndx->sym );
#endif

    fixup = AddFixup( opndx->sym, fixup_type, OPTJ_NONE );
//    add_frame();   // ???
    if( fixup == NULL ) {
        return( ERROR );
    } else {
        return( NOT_ERROR );
    }
}

static int memory_operand( expr_list *opndx, bool with_fixup )
/************************************************************/
{
    uint_8              ss = SCALE_FACTOR_1;
    int                 index = EMPTY;
    int                 base = EMPTY;
    struct asm_sym      *sym;
    char                base_lock = FALSE;
    enum fixup_types    fixup_type;
    int                 flag;
#if defined( _STANDALONE_ )
    int                 sym32;
#endif

    Code->data[Opnd_Count] = opndx->value;
    Code->info.opnd_type[Opnd_Count] = OP_M;
    sym = opndx->sym;

    segm_override_memory( opndx );

    flag = ( opndx->explicit ) ? TRUE : FALSE ;
    if( ptr_operator( opndx->mem_type, flag ) == ERROR )
        return( ERROR );
    if( ptr_operator( MT_PTR, flag ) == ERROR ) {
        return( ERROR );
    }
    if( opndx->mbr != NULL ) {
        flag = FALSE;
        if( ptr_operator( opndx->mbr->mem_type, flag ) == ERROR )
            return( ERROR );
        if( ptr_operator( MT_PTR, flag ) == ERROR ) {
            return( ERROR );
        }
    }
    if( opndx->base_reg != EMPTY ) {
        base = AsmBuffer[opndx->base_reg]->u.value;
        switch( base ) {     // check for base registers
        case T_EAX:
        case T_EBX:
        case T_ECX:
        case T_EDX:
        case T_ESP:
        case T_EBP:
        case T_ESI:
        case T_EDI:
            if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
                // 286 and down cannot use 386 registers
                AsmError( CANNOT_USE_386_ADDRESSING_MODE_WITH_CURRENT_CPU_SETTING );
                return( ERROR );
            }
            SET_ADRSIZ_32( Code );
            break;
        case T_BX:
        case T_BP:
        case T_SI:
        case T_DI:
            SET_ADRSIZ_16( Code );
            break;
        default:
            AsmError( INVALID_MEMORY_POINTER );
            return( ERROR );
        }
    }
    if( opndx->idx_reg != EMPTY ) {
        index = AsmBuffer[opndx->idx_reg]->u.value;
        switch( index ) {     // check for index registers
        case T_EAX:
        case T_EBX:
        case T_ECX:
        case T_EDX:
        case T_ESP:
        case T_EBP:
        case T_ESI:
        case T_EDI:
            if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
                // 286 and down cannot use 386 registers
                AsmError( CANNOT_USE_386_ADDRESSING_MODE_WITH_CURRENT_CPU_SETTING );
                return( ERROR );
            }
            SET_ADRSIZ_32( Code );
            break;
        case T_BX:
        case T_BP:
        case T_SI:
        case T_DI:
            SET_ADRSIZ_16( Code );
            break;
        default:
            AsmError( INVALID_MEMORY_POINTER );
            return( ERROR );
        }
        if( AsmBuffer[opndx->idx_reg]->u.value == T_ESP ) {
            if( opndx->scale == 1 ) {
                index = base;
                base = AsmBuffer[opndx->idx_reg]->u.value;
            } else {
                AsmError( ESP_CANNOT_BE_USED_AS_INDEX );
                return( ERROR );
            }
        }
        if( addr_32( Code ) ) {
            if( ( Code->info.cpu & P_CPU_MASK ) >= P_386 ) {
                if( !Code->use32 )
                    Code->prefix.adrsiz = TRUE;
                switch( index ) {
                case T_ESP:
                case T_BX:
                case T_BP:
                case T_SI:
                case T_DI:
                    // cannot use ESP or 16-bit reg as index
                    AsmError( INVALID_INDEX_REGISTER );
                    return( ERROR );
                default:
                    if( !Code->use32 )
                        Code->prefix.adrsiz = TRUE;
                    switch( opndx->scale ) {
                    case 1:
                        // ss = 00
                        break;
                    case 2:
                        // ss = 01
                        ss = SCALE_FACTOR_2;
                        break;
                    case 4:
                        // ss = 10
                        ss = SCALE_FACTOR_4;
                        break;
                    case 8:
                        // ss = 11
                        ss = SCALE_FACTOR_8;
                        break;
                    default: // must be * 1, 2, 4 or 8
                        AsmError( SCALE_FACTOR_MUST_BE_1_2_4_OR_8 );
                        return( ERROR );
                    }
                }
            } else {
                // 286 and down cannot use this memory mode
                AsmError( INVALID_ADDRESSING_MODE_WITH_CURRENT_CPU_SETTING );
                return( ERROR );
            }
        }
    }
    if( with_fixup ) {
        switch( sym->state ) {
        case SYM_UNDEFINED:
            // forward reference
            break;
#if defined( _STANDALONE_ )
        case SYM_SEG:
        case SYM_GRP:
            // error !!!!!
            break;
#endif
        case SYM_STACK:
            if( base != EMPTY ) {
                if( base_lock == TRUE ) {
                    // [reg + data][reg + data] is not allowed
                    AsmError( TOO_MANY_BASE_REGISTERS );
                    return( ERROR );
                } else {
                    index = base;
                }
            }
            if( Code->use32 ) {
                base = T_EBP;
            } else {
                base = T_BP;
            }
            base_lock = TRUE;   // add lock
            /* fall through */
        default:
            if( Code->mem_type == MT_EMPTY ) {
                if( ptr_operator( sym->mem_type, FALSE ) == ERROR )
                    return( ERROR );
                if( ptr_operator( MT_PTR, FALSE ) == ERROR ) {
                    return( ERROR );
                }
            }
            break;
        }

#if defined( _STANDALONE_ )
        if( opndx->abs ) {
            sym32 = addr_32( Code );
        } else {
            sym32 = SymIs32( sym );
        }
        if( ( opndx->base_reg == EMPTY ) && ( opndx->idx_reg == EMPTY ) ) {
            SET_ADRSIZ( Code, sym32 );
            fixup_type = ( sym32 ) ? FIX_OFF32 : FIX_OFF16;
        } else {
            if( addr_32( Code ) ) {
                fixup_type = ( sym32 ) ? FIX_OFF32 : FIX_OFF16;
            } else {
                if( sym32 ) {
                    // fixme !!!! warning
                    // address size is 16bit
                    // but fixup is 32-bit
                }
                fixup_type = FIX_OFF16;
            }
        }

        AddFixup( sym, fixup_type, OPTJ_NONE );

        if( Modend ) {
            GetAssume( sym, ASSUME_NOTHING );
        } else {
            if( mem2code( ss, index, base, sym ) == ERROR ) {
                return( ERROR );
            }
        }
        add_frame();
#else
        fixup_type = ( Code->use32 ) ? FIX_OFF32 : FIX_OFF16;

        AddFixup( sym, fixup_type, OPTJ_NONE );

        if( mem2code( ss, index, base, sym ) == ERROR ) {
            return( ERROR );
        }
#endif
    } else {
        if( mem2code( ss, index, base, sym ) == ERROR ) {
            return( ERROR );
        }
    }
    if( IS_JMPCALLN( Code->info.token ) ) {
        if( Code->mem_type == MT_DWORD ) {
            if( !Code->use32 ) {
                Code->info.token++;
            }
        }
    }
    return( NOT_ERROR );
}

static int process_address( expr_list *opndx )
/********************************************/
/*
  parse the memory reference operand
*/
{
    memtype     mem_type;

    if( opndx->indirect ) {           // indirect operand
        if( opndx->sym == NULL ) {
            return( memory_operand( opndx, FALSE ) );
        } else {
            return( memory_operand( opndx, TRUE ) );
        }
    } else {                          // direct operand
        if( opndx->instr != EMPTY ) { // OFFSET ..., SEG ...
            if( opndx->sym == NULL ) {
                return( idata_nofixup( opndx ) );
            } else {
                return( idata_fixup( opndx ) );
            }
        } else {                      // direct operand only
            if( opndx->sym == NULL ) {       // without symbol
                if( opndx->override != EMPTY ) {
                    // direct absolute memory without fixup ... DS:[0]
                    return( memory_operand( opndx, FALSE ) );
                } else {
                    return( idata_nofixup( opndx ) );  // error ????
                }
            } else {                  // with symbol
#if defined( _STANDALONE_ )
                if( ( opndx->sym->state == SYM_UNDEFINED ) && !opndx->explicit ) {
                    if( Parse_Pass != PASS_1 ) {
                        AsmErr( SYMBOL_NOT_DEFINED, opndx->sym->name );
                        return( ERROR );
                    }
                    // undefined symbol, it is not possible to determine
                    // operand type and size
                    switch( Code->info.token ) {
                    case T_PUSH:
                    case T_PUSHW:
                    case T_PUSHD:
                        return( idata_nofixup( opndx ) );
                        break;
                    default:
                        if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
                            return( idata_nofixup( opndx ) );
                        } else {
                            return( memory_operand( opndx, FALSE ) );
                        }
                        break;
                    }
#else
                if( ( opndx->sym->state == SYM_UNDEFINED ) && !opndx->explicit ) {
                    // undefined symbol, it is not possible to determine
                    // operand type and size
                    switch( Code->info.token ) {
                    case T_PUSH:
                    case T_PUSHW:
                    case T_PUSHD:
                        return( idata_nofixup( opndx ) );
                        break;
                    default:
                        if( IS_ANY_BRANCH( Code->info.token ) ) {  // jumps/call processing
                            return( idata_nofixup( opndx ) );
                        } else {
                            return( memory_operand( opndx, TRUE ) );
                        }
                        break;
                    }
#endif
#if defined( _STANDALONE_ )
                } else if( ( opndx->sym->state == SYM_SEG )
                    || ( opndx->sym->state == SYM_GRP ) ) {
                    // SEGMENT and GROUP symbol is converted to SEG symbol
                    // for next prrocessing
                    opndx->instr = T_SEG;
                    return( idata_fixup( opndx ) );
#endif
                } else {
                    // CODE location is converted to OFFSET symbol
                    mem_type = ( opndx->explicit ) ? opndx->mem_type : opndx->sym->mem_type;
#if defined( _STANDALONE_ )
                    if( opndx->abs ) {
                        return( idata_fixup( opndx ) );
                    }
#endif
                    switch( mem_type ) {
                    case MT_FAR:
                    case MT_NEAR:
                    case MT_SHORT:
#if defined( _STANDALONE_ )
                    case MT_PROC:
#endif
                        if( Code->info.token == T_LEA ) {
                            return( memory_operand( opndx, TRUE ) );
#if defined( _STANDALONE_ )
                        } else if( IS_SYM_COUNTER( opndx->sym->name ) ) {
                            return( idata_fixup( opndx ) );
#endif
                        } else if( opndx->mbr != NULL ) { // structure or structure member
                            return( memory_operand( opndx, TRUE ) );
                        } else {
                            return( idata_fixup( opndx ) );
                        }
                        break;
                    default:
                        // direct memory with fixup
                        return( memory_operand( opndx, TRUE ) );
                        break;
                    }
                }
            }
        }
    }
    return( NOT_ERROR );
}

static int process_const( expr_list *opndx )
/******************************************/
{
    if( ( Code->info.token == T_IMUL )
        && ( Code->info.opnd_type[OPND1] & OP_R ) ) {
        if( Opnd_Count == OPND2 ) {
            Code->info.rm_byte = ( Code->info.rm_byte & ~BIT_345 )
                          | ( ( Code->info.rm_byte & BIT_012 ) << 3 );
        } else if( Opnd_Count == OPND3 ) {
            Code->info.opnd_type[OPND1] = Code->info.opnd_type[OPND2];
            Code->info.opnd_type[OPND2] = OP_NONE;
            Code->data[OPND1] = Code->data[OPND2];
            Code->data[OPND2] = 0;
            InsFixups[OPND1] = InsFixups[OPND2];
            InsFixups[OPND2] = NULL;
            Opnd_Count = OPND2;
        }
    }
    return( idata_nofixup( opndx ) );
}

static int process_reg( expr_list *opndx )
/****************************************/
/*
- parse and encode the register operand;
*/
{
    int                 temp;
    int                 reg;

    if( opndx->indirect )  // simple register indirect operand ... [EBX]
        return( process_address( opndx ) );
    temp = AsmOpcode[AsmBuffer[opndx->base_reg]->u.value].position;
    reg = AsmOpTable[temp].opcode;
    Code->info.opnd_type[Opnd_Count] = AsmOpTable[temp].opnd_type[OPND2];
    switch( AsmOpTable[temp].opnd_type[OPND2] ) {
    case OP_AL:
    case OP_R8:
        Code->info.opcode &= NOT_W_BIT;         // clear w-bit
        break;
    case OP_CL: /* only appears in "shift opnd,CL" instructions */
        break;
    case OP_AX:
    case OP_DX: /* only appears in "in" and "out" instructions  */
    case OP_R16:
        Code->info.opcode |= W_BIT;             // set w-bit
        if( Code->use32 )
            Code->prefix.opsiz = TRUE;
        break;
    case OP_MMX:
        break;
    case OP_XMM:
        break;
    case OP_ST:
        temp = opndx->idx_reg & BIT_012;
        Code->info.rm_byte |= temp;
        if( temp != 0 )
            Code->info.opnd_type[Opnd_Count] = OP_ST_REG;
        break;
    case OP_SR3:                        // 386 segment register
        if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
            // 8086 ins cannot use 80386 segment register
            AsmError( CANNOT_USE_386_SEGMENT_REGISTER_WITH_CURRENT_CPU_SETTING );
            return( ERROR );
        }
    case OP_SR:                                 // any seg reg
    case OP_SR2:                                // 8086 segment register
        if( AsmBuffer[opndx->base_reg]->u.value == T_CS ) {
            // POP CS is not allowed
            if( Code->info.token == T_POP ) {
                AsmError( POP_CS_IS_NOT_ALLOWED );
                return( ERROR );
            }
        }
        reg = get_sr_rm_byte( AsmOpTable[temp].opcode );
        break;
    case OP_EAX:
    case OP_R32:
        if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
            // 8086 ins cannot use 386 register
            AsmError( CANNOT_USE_386_REGISTER_WITH_CURRENT_CPU_SETTING );
            return( ERROR );
        }
        Code->info.opcode |= W_BIT;             // set w-bit
        if( !Code->use32 )
            Code->prefix.opsiz = TRUE;
        break;
    case OP_TR:                 // Test registers
        switch( AsmBuffer[opndx->base_reg]->u.value ) {
        case T_TR3:
        case T_TR4:
        case T_TR5:
            if( ( ( ( Code->info.cpu & P_CPU_MASK ) < P_486 )
               || ( ( Code->info.cpu & P_CPU_MASK ) >= P_686 ) )
                && ( ( AsmOpTable[temp].cpu & P_CPU_MASK ) >= P_486 ) ) {
                // TR3, TR4, TR5 are available on 486 only
                AsmError( CANNOT_USE_TR3_TR4_TR5_IN_CURRENT_CPU_SETTING );
                return( ERROR );
            }
            break;
        case T_TR6:
        case T_TR7:
            if( ( ( ( Code->info.cpu & P_CPU_MASK ) < P_386 )
               || ( ( Code->info.cpu & P_CPU_MASK ) >= P_686 ) )
                && ( ( AsmOpTable[temp].cpu & P_CPU_MASK ) >= P_386 ) ) {
                // TR6, TR7 are available on 386...586 only
                AsmError( CANNOT_USE_TR3_TR4_TR5_IN_CURRENT_CPU_SETTING );
                return( ERROR );
            }
            break;
        }
    case OP_CR:                 // Control registers
    case OP_DR:                 // Debug registers
        if( Code->info.token != T_MOV ) {
            AsmError( ONLY_MOV_CAN_USE_SPECIAL_REGISTER );
            return( ERROR );
        }
        break;
    }
    if( Opnd_Count == OPND1 ) {
        // the first operand
        // r/m is treated as a 'reg' field
        Code->info.rm_byte |= MOD_11;
        // fill the r/m field
        Code->info.rm_byte |= reg;
    } else {
        // the second operand
        if( ( Code->info.token == T_XCHG )
            && ( ( Code->info.opnd_type[OPND1] == OP_AX )
            || ( Code->info.opnd_type[OPND1] == OP_EAX ) ) ) {
            // XCHG can use short form if op1 is AX or EAX
            Code->info.rm_byte = ( Code->info.rm_byte & BIT_67 ) | reg;
        } else {
            // fill reg field with reg
            Code->info.rm_byte = ( Code->info.rm_byte & ~BIT_345 ) | ( reg << 3 );
        }
    }
    return( NOT_ERROR );
}

int AsmParse( void )
/******************/
/*
- co-ordinate the parsing process;
- it is a basically a big loop to loop through all the tokens and identify them
  with the switch statement;
*/
{
    int                 i;
    OPNDTYPE            cur_opnd = OP_NONE;
    OPNDTYPE            last_opnd = OP_NONE;
    struct asm_code     *rCode = Code;
    expr_list           opndx;
    int                 temp;

#if defined( _STANDALONE_ )
    Code->use32 = Use32;
    i = proc_check();
    if( i == ERROR )
        return( ERROR );
    if( i == TRUE )
        return( NOT_ERROR );
#endif

    //init
    rCode->info.token     = T_NULL;
    rCode->info.opcode    = 0;
    rCode->info.rm_byte   = 0;
    rCode->prefix.ins     = EMPTY;
    rCode->prefix.seg     = EMPTY;
    rCode->prefix.adrsiz  = FALSE;
    rCode->prefix.opsiz   = FALSE;
    rCode->mem_type       = MT_EMPTY;
    rCode->mem_type_fixed = FALSE;
    rCode->extended_ins   = EMPTY;
    rCode->sib            = 0;            // assume ss is *1
    rCode->indirect       = FALSE;
    for( i = 0; i < 3; i++ ) {
        rCode->info.opnd_type[i] = OP_NONE;
        rCode->data[i] = 0;
        InsFixups[i] = NULL;
    }
    Opnd_Count = 0;
    curr_ptr_type = EMPTY;

    // check if continue initializing array
    temp = NextArrayElement();
    if( temp != EMPTY )
        return( temp );

#if defined( _STANDALONE_ )
    CheckSeg = TRUE;
    Frame = NULL;
    SegOverride = NULL;
#endif

    for( i = 0; i < Token_Count; i++ ) {
        switch( AsmBuffer[i]->token ) {
        case T_INSTR:
//            ExpandTheWorld( i, FALSE, TRUE );
#if defined( _STANDALONE_ )
            if( ExpandAllConsts( i, FALSE ) == ERROR )
                return( ERROR );
#endif
            if( last_opnd != OP_NONE ) {
                // illegal operand is put before instruction
                AsmError( SYNTAX_ERROR );
                return( ERROR );
            }
            cur_opnd = OP_NONE;
#if defined( _STANDALONE_ )
            if( ( AsmBuffer[i+1]->token == T_DIRECTIVE )
                || ( AsmBuffer[i+1]->token == T_COLON ) ) {
                // instruction name is label
                AsmBuffer[i]->token = T_ID;
                i--;
                continue;
            }
#endif
            switch( AsmBuffer[i]->u.value ) {
            // prefix
            case T_LOCK:
            case T_REP:
            case T_REPE:
            case T_REPNE:
            case T_REPNZ:
            case T_REPZ:
                rCode->prefix.ins = AsmBuffer[i]->u.value;
                // prefix has to be followed by an instruction
                if( AsmBuffer[i+1]->token != T_INSTR ) {
                    AsmError( PREFIX_MUST_BE_FOLLOWED_BY_AN_INSTRUCTION );
                    return( ERROR );
                }
                continue;
#if defined( _STANDALONE_ )
            case T_RET:
                if( ( CurrProc != NULL ) && ( in_epilogue == 0 ) ) {
                    in_epilogue = 1;
                    return( Ret( i, Token_Count, FALSE ) );
                }
            case T_RETN:
            case T_RETF:
                in_epilogue = 0;
                rCode->info.token = AsmBuffer[i]->u.value;
                break;
            case T_IRET:
            case T_IRETD:
                if( ( CurrProc != NULL ) && ( in_epilogue == 0 ) ) {
                    in_epilogue = 1;
                    return( Ret( i, Token_Count, TRUE ) );
                }
            case T_IRETF:
            case T_IRETDF:
                in_epilogue = 0;
                rCode->info.token = AsmBuffer[i]->u.value;
                break;
            case T_CALL:
                if( Options.mode & MODE_IDEAL ) {
                    int n;
                    int lang_type;

                    for( n = i + 2; n < Token_Count; n++ ) {
                        lang_type = CheckForLang( n );
                        if( lang_type != ERROR ) {
                            return( expand_call( n + 1, lang_type ) );
                        }
                    }
                }
                /* fall into default */
#endif
            default:
                rCode->info.token = AsmBuffer[i]->u.value;
                break;
            }
            i++;
            if( EvalOperand( &i, Token_Count, &opndx, TRUE ) == ERROR ) {
                return( ERROR );
            }
            if( opndx.empty )
                break;
            switch( opndx.type ) {
            case EXPR_ADDR:
                temp = process_address( &opndx );
                if( temp == SCRAP_INSTRUCTION )
                    return( SCRAP_INSTRUCTION );
                if( temp == ERROR )
                    return( ERROR );
                break;
            case EXPR_CONST:
                process_const( &opndx );
                break;
            case EXPR_REG:
                process_reg( &opndx );
                break;
            case EXPR_UNDEF:
                return( ERROR );
            default:
                break;
            }
            i--;
            break;
        case T_RES_ID:
            if( rCode->info.token == T_NULL ) {
                temp = ( i == 0 ) ? -1 : 0;
                return( data_init( temp, i ) );
            }
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        case T_DIRECTIVE:
            return( directive( i, AsmBuffer[i]->u.value ) );
            break;
#if defined( _STANDALONE_ )
        case T_DIRECT_EXPR:
            if( Parse_Pass != PASS_1 ) {
                Modend = TRUE;
                temp = i;
                temp++;
                if( EvalOperand( &temp, Token_Count, &opndx, TRUE ) == ERROR ) {
                    return( ERROR );
                }
                if( !opndx.empty && ( opndx.type == EXPR_ADDR ) ) {
                    process_address( &opndx );
                }
            }
            return( directive( i, AsmBuffer[i]->u.value ) );
            break;
#endif
        case T_ID:
#if defined( _STANDALONE_ )
            if( !( ( AsmBuffer[i+1]->token == T_DIRECTIVE )
                && ( ( AsmBuffer[i+1]->u.value == T_EQU )
                || ( AsmBuffer[i+1]->u.value == T_EQU2 )
                || ( AsmBuffer[i+1]->u.value == T_TEXTEQU ) ) ) ) {
                switch( ExpandSymbol( i, FALSE ) ) {
                case ERROR:
                    return( ERROR );
                case STRING_EXPANDED:
                    // restart token processing
                    i--;
                    continue;
                }
            }
#endif
#if 0
            if( ( last_opnd != OP_NONE )
                && ( last_opnd != OP_M )
                && ( last_opnd != OP_I ) ) {
                AsmError( SYNTAX_ERROR );
                return( ERROR );
            }
#endif
            if( i == 0 ) {   // a new label
#if ALLOW_STRUCT_INIT
#if defined( _STANDALONE_ )
                if( IsLabelStruct( AsmBuffer[i]->string_ptr )
                    && ( AsmBuffer[i+1]->token != T_DIRECTIVE ) ) {
                    AsmBuffer[i]->token = T_DIRECTIVE;
                    AsmBuffer[i]->u.value = T_STRUCT;
                    return( data_init( -1, 0 ) );
                }
#endif
#endif

                switch( AsmBuffer[i+1]->token ) {
                case T_COLON:
                    cur_opnd = OP_LABEL;
                    break;
#if ALLOW_STRUCT_INIT
#if defined( _STANDALONE_ )
                case T_ID:
                    /* structure declaration */
                    if( IsLabelStruct( AsmBuffer[i+1]->string_ptr ) ) {
                        AsmBuffer[i+1]->token = T_DIRECTIVE;
                        AsmBuffer[i+1]->u.value = T_STRUCT;
                    } else {
                        AsmError( SYNTAX_ERROR );
                        return( ERROR );
                    }
                    /* fall through */
#endif
#endif
                case T_RES_ID:
                    return( data_init( i, i+1 ) );
                    break;
#if defined( _STANDALONE_ )
                case T_DIRECTIVE:
                    return( directive( i+1, AsmBuffer[i+1]->u.value ) );
                    break;
#endif
                default:
                    AsmError( SYNTAX_ERROR );
                    return( ERROR );
                }
            }
            break;
        case T_COMMA:
            if( Opnd_Count == OPND1 ) {
                Opnd_Count++;
            } else if( Opnd_Count == OPND2 ) {
                switch( rCode->info.token ) {
                case T_SHLD:
                case T_SHRD:
                    switch( AsmBuffer[i+1]->token ) {
                    case T_NUM:
                        break;
                    case T_REG:
                        i++;
                        if( AsmBuffer[i]->u.value == T_CL ) {
                            break;
                        }
                    default:
                        AsmError( INVALID_SHLD_SHRD_FORMAT );
                        return( ERROR );
                    }
                    break;
                case T_NULL:
                    break;
                default:
                    Opnd_Count++;
                    break;
                }
            } else {
                AsmError( TOO_MANY_COMMAS );
                return( ERROR );
            }
            i++;
            cur_opnd = OP_NONE;
            curr_ptr_type = EMPTY;
#if defined( _STANDALONE_ )
            Frame = NULL;
            SegOverride = NULL;
#endif
            if( EvalOperand( &i, Token_Count, &opndx, TRUE ) == ERROR ) {
                return( ERROR );
            }
            if( opndx.empty )
                break;
            switch( opndx.type ) {
            case EXPR_ADDR:
                temp = process_address( &opndx );
                if( temp == SCRAP_INSTRUCTION )
                    return( SCRAP_INSTRUCTION );
                if( temp == ERROR )
                    return( ERROR );
                break;
            case EXPR_CONST:
                process_const( &opndx );
                break;
            case EXPR_REG:
                process_reg( &opndx );
                break;
            case EXPR_UNDEF:
                return( ERROR );
            default:
                break;
            }
            i--;
            break;
        case T_COLON:
            if ( last_opnd == OP_LABEL ) {
                if( AsmBuffer[i+1]->token != T_RES_ID ) {
                    if( MakeLabel( AsmBuffer[i-1]->string_ptr, MT_NEAR )==ERROR ) {
                         return( ERROR );
                    }
                }
                cur_opnd = OP_NONE;
            } else {
                AsmError( SYNTAX_ERROR_UNEXPECTED_COLON );
                return( ERROR );
            }
            break;
        case T_FLOAT:
            if( idata_float( AsmBuffer[i]->u.value ) == ERROR ) {
                return( ERROR );
            }
            if( AsmBuffer[i-1]->token == T_MINUS ) {
                rCode->data[Opnd_Count] ^= 0x80000000;
            }
#if defined( _STANDALONE_ )
            AsmWarn( 4, FLOAT_OPERAND );
#endif
            break;
        default:
#if defined( _STANDALONE_ )
/* */myassert( 0 );
#endif
            break;
        }
        last_opnd = cur_opnd;
    }
    switch( rCode->info.token ) {
    case T_LODS:
    case T_SCAS:
    case T_STOS:
        SizeString( OperandSize( Code->info.opnd_type[OPND1] ) );
        break;
    }
    if( Opnd_Count > OPND1 ) {
        if( check_size() == ERROR ) {
            return( ERROR );
        }
    }
    return( match_phase_1() );
}

static void SizeString( unsigned op_size )
/****************************************/
{
    /* size an string instruction based on it's operand's size */
    switch( op_size ) {
    case 1:
        Code->mem_type = MT_BYTE;
        Code->info.opcode &= NOT_W_BIT;
        if( Code->use32 )
            Code->prefix.opsiz = FALSE;
        break;
    case 2:
        Code->mem_type = MT_WORD;
        Code->info.opcode |= W_BIT;
        Code->prefix.opsiz = Code->use32 ? TRUE : FALSE;
        break;
    case 4:
        Code->mem_type = MT_DWORD;
        Code->info.opcode |= W_BIT;
        Code->prefix.opsiz = Code->use32 ? FALSE : TRUE;
        break;
    }
}

static int check_size( void )
/***************************/
/*
- use to make sure the size of first operand match the size of second operand;
- optimize MOV instruction;
*/
{
    OPNDTYPE    op1 = Code->info.opnd_type[OPND1];
    OPNDTYPE    op2 = Code->info.opnd_type[OPND2];
    int         state = NOT_ERROR;
    int         temp;
    int         op1_size;
    int         op2_size;
    int         op_size = 0;

    switch( Code->info.token ) {
#if 0
    case T_PSLLW:
    case T_PSLLD:
    case T_PSLLQ:
    case T_PSRLW:
    case T_PSRLD:
    case T_PSRLQ:
    case T_PSRAW:
    case T_PSRAD:
        // check was wrong - instructions take a m64 OR an 8 bit immediate
        if( op2 & OP_I ) {
            op_size = OperandSize( op2 );
            if( op_size >= 2 ) {
                AsmError( OP_2_TOO_BIG );
                state = ERROR;
            }
        }
        break;
#endif
    case T_IN:
        if( op2 == OP_DX ) {
            switch( op1 ) {
            case OP_AX:
                break;
            case OP_AL:
                Code->info.opcode &= NOT_W_BIT;         // clear w-bit
            case OP_EAX:
                if( Code->use32 ) {
                    Code->prefix.opsiz = FALSE;
                }
                break;
            }
        }
        break;
    case T_OUT:
        if( op1 == OP_DX ) {
            switch( op2 ) {
            case OP_AX:
                break;
            case OP_AL:
                Code->info.opcode &= NOT_W_BIT;         // clear w-bit
            case OP_EAX:
                if( Code->use32 ) {
                    Code->prefix.opsiz = FALSE;
                }
            }
        }
        break;
    case T_INS:
    case T_CMPS:
        op_size = OperandSize( op1 );
        /* fall through */
    case T_MOVS:
    case T_OUTS:
        if( op_size == 0 )
            op_size = OperandSize( op2 );

        SizeString( op_size );
        break;
    case T_LEA:
        switch( OperandSize( op1 ) ) {
        case 2:
        case 4:
            break;
        default:
            AsmError( OPERANDS_MUST_BE_THE_SAME_SIZE );
            state = ERROR;
        }
        break;
    case T_RCL:
    case T_RCR:
    case T_ROL:
    case T_ROR:
    case T_SAL:
    case T_SAR:
    case T_SHL:
    case T_SHR:
    case T_LDS:
    case T_LES:
    case T_LFS:
    case T_LGS:
    case T_LSS:
        // checking will be done later
        break;
    case T_ENTER:
        // ENTER has to be OP_I16, OP_I8
        if( op1 == OP_I32 ) {
            //parse_phase_1 will treat 16-bit data as OP_I32 if CPU is 386
            if( Code->data[OPND1] > (signed long)USHRT_MAX ) {
                // if op1 is really 32-bit data, then error
                AsmError( IMMEDIATE_DATA_TOO_BIG );
                state = ERROR;
            }
        }
        // type cast op1 to OP_I16
        Code->info.opnd_type[OPND1] = OP_I16;
        // op2 have to be 8-bit data
        if( op2 >= OP_I16 ) {
            if( Code->data[OPND2] > UCHAR_MAX ) {
                AsmError( IMMEDIATE_DATA_TOO_BIG );
                state = ERROR;
            }
            Code->info.opnd_type[OPND2] = OP_I8;
        }
        break;
    case T_CALLF:
    case T_JMPF:
        // segment can only be 16-bit
        if( op1 > OP_I16 ) {
            AsmError( SEGMENT_TOO_BIG );
            state = ERROR;
        }
        if( ( Code->info.cpu & ( P_CPU_MASK | P_PM ) ) <= P_286p ) {
            // offset can only be 16-bit if CPU is 286 and down
            if( op2 > OP_I16 ) {
                AsmError( OFFSET_TOO_BIG );
                state = ERROR;
            }
        }
        // swap the 2 opnds to make output easier
        if( InsFixups[OPND2] != NULL ) {
            // absolute segment + offset nnnn,offset
            Code->info.opnd_type[OPND1] = Code->info.opnd_type[OPND2];
            InsFixups[OPND1] = InsFixups[OPND2];
            InsFixups[OPND2] = NULL;
        } else {
            // absolute ptr nnnn,nnnn
            Code->info.opnd_type[OPND1] = Code->use32 ? OP_I32 : OP_I16;
        }
        temp = Code->data[OPND1];
        Code->data[OPND1] = Code->data[OPND2];
        Code->data[OPND2] = temp;
        Code->info.opnd_type[OPND2] = OP_I16;
        Code->info.opcode = 0;
        break;
    case T_MOVSX:
    case T_MOVZX:
        Code->info.opcode &= NOT_W_BIT;
        op2_size = OperandSize( op2 );
        switch( OperandSize( op1 ) ) {
        case 4:
            switch( op2_size ) {
            case 2:
                Code->info.opcode |= W_BIT;
                break;
            case 4:
                AsmError( OP_2_TOO_BIG );
                state = ERROR;
            }
            if( Code->use32 ) {
                Code->prefix.opsiz = FALSE;     // - don't need opnd size prefix
            }
            break;
        case 2:
            if( op2_size >= 2 ) {
                AsmError( OP_2_TOO_BIG );
                state = ERROR;
            }
            break;
        default:
            // op1 have to be r16/r32
            AsmError( OP_1_TOO_SMALL );
            state = ERROR;
        }
        break;
    case T_LSL:                                 /* 19-sep-93 */
        op1_size = OperandSize( op1 );
        switch( op1_size ) {
        case 2:
            if( Code->use32 )
                Code->prefix.opsiz = TRUE;
            break;
        case 4:
            if( Code->use32 )
                Code->prefix.opsiz = FALSE;
            break;
        default:
            AsmError( INVALID_SIZE );
            return( ERROR );
        }
        op2_size = OperandSize( op2 );
        switch( op2_size ) {
        case 2:
        case 4:
            break;
        default:
            AsmError( INVALID_SIZE );
            state = ERROR;
            break;
        }
        break;
    case T_CVTSD2SI:
    case T_CVTTSD2SI:
    case T_CVTSS2SI:
    case T_CVTTSS2SI:
    case T_MOVNTI:
        break;
    case T_MOVD:
#if 0
        op1_size = OperandSize( op1 );
        op2_size = OperandSize( op2 );
        if( ( op1_size != 0 ) && ( op1_size != 4 )
            || ( op2_size != 0 ) && ( op2_size != 4 ) ) {
            AsmError( OPERANDS_MUST_BE_THE_SAME_SIZE );
            state = ERROR;
        }
#endif
        break;
    case T_MOV:
        if( op1 & OP_SR ) {
            op2_size = OperandSize( op2 );
            if( ( op2_size == 2 ) || ( op2_size == 4 ) ) {
//                Code->prefix.opsiz = FALSE;
                return( state );
            }
        } else if( op2 & OP_SR ) {
            op1_size = OperandSize( op1 );
            if( ( op1_size == 2 ) || ( op1_size == 4 ) ) {
//                if( op1 == OP_M )
//                    Code->prefix.opsiz = FALSE;
                return( state );
            }
        } else if( ( op1 == OP_M ) || ( op2 == OP_M ) ) {
            // to optimize MOV
            temp = Code->info.rm_byte;
            if( Code->info.opnd_type[OPND1] & OP_A ) {
                temp = ( temp & BIT_67 ) | ( ( temp & BIT_012 ) << 3 ) | ( ( temp & BIT_345 ) >> 3 );
                if( addr_32( Code ) && ( temp == D32 )
                    || !addr_32( Code ) && ( temp == D16 ) ) {
                    // DS:[d32] or DS:[d16] can use MOV Mem with Acc (short form)
                } else {
                    // we have to change OP_A to OP_R
                    Code->info.opnd_type[OPND1] &= ~OP_A;
                }
            } else if( Code->info.opnd_type[OPND2] & OP_A ) {
                if( addr_32( Code ) && ( temp == D32 )
                    || !addr_32( Code ) && ( temp == D16 ) ) {
                    // DS:[d32] or DS:[d16] can use MOV Mem with Acc (short form)
                    temp = EMPTY;
                } else {
                    // we have to change OP_A to OP_R
                    Code->info.opnd_type[OPND2] &= ~OP_A;
                }
            }
        } else if( ( op1 & OP_SPEC_REG ) || ( op2 & OP_SPEC_REG ) ) {
            Code->prefix.opsiz = FALSE;
            return( state );
        }
        // no break;
    default:
        // make sure the 2 opnds are of the same type
        op1_size = OperandSize( op1 );
        op2_size = OperandSize( op2 );
        if( op1_size > op2_size ) {
            if( ( op2 >= OP_I8 ) && ( op2 <= OP_I32 ) ) {     /* immediate */
                op2_size = op1_size;    /* promote to larger size */
            }
        }
        if( ( op1_size == 1 ) && ( op2 == OP_I16 )
            && ( Code->data[OPND2] <= UCHAR_MAX ) ) {
            return( state ); // OK cause no sign extension
        }
        if( ( op1_size == 2 ) && ( op2 == OP_I32 )
            && ( Code->data[OPND2] <= USHRT_MAX ) ) {
            return( state ); // OK cause no sign extension
        }
        if( op1_size != op2_size ) {
            /* if one or more are !defined, set them appropriately */
            if( ( op1 | op2 ) & ( OP_MMX | OP_XMM ) ) {
            } else if( ( op1_size != 0 ) && ( op2_size != 0 ) ) {
                AsmError( OPERANDS_MUST_BE_THE_SAME_SIZE );
//                state = ERROR;
            }
            if( op1_size == 0 ) {
                if( ( op1 & OP_M_ANY ) && ( op2 & OP_I ) ) {
                    if( (unsigned long)Code->data[OPND2] > USHRT_MAX ) {
                         Code->mem_type = MT_DWORD;
                         Code->info.opcode |= W_BIT;
                         Code->info.opnd_type[OPND2] = OP_I32;
#if defined( _STANDALONE_ )
                         if( Parse_Pass == PASS_1 ) {
                             if( (Options.mode & MODE_IDEAL) == 0 ) {
                                 AsmWarn( 1, ASSUMING_DWORD );
                             }
                         }
#endif
                    } else if( (unsigned long)Code->data[OPND2] > UCHAR_MAX ) {
                         Code->mem_type = MT_WORD;
                         Code->info.opcode |= W_BIT;
                         Code->info.opnd_type[OPND2] = OP_I16;
#if defined( _STANDALONE_ )
                         if( Parse_Pass == PASS_1 ) {
                             if( (Options.mode & MODE_IDEAL) == 0 ) {
                                 AsmWarn( 1, ASSUMING_WORD );
                             }
                         }
#endif
                    } else {
                         Code->mem_type = MT_BYTE;
                         Code->info.opnd_type[OPND2] = OP_I8;
#if defined( _STANDALONE_ )
                         if( Parse_Pass == PASS_1 ) {
                             if( (Options.mode & MODE_IDEAL) == 0 ) {
                                 AsmWarn( 1, ASSUMING_BYTE );
                             }
                         }
#endif
                    }
                } else if( ( op1 & OP_M_ANY ) && ( op2 & ( OP_R | OP_SR ) ) ) {
                } else if( ( op1 & ( OP_MMX | OP_XMM ) ) && ( op2 & OP_I ) ) {
                    if( (unsigned long)Code->data[OPND2] > USHRT_MAX ) {
                         Code->info.opnd_type[OPND2] = OP_I32;
                    } else if( (unsigned long)Code->data[OPND2] > UCHAR_MAX ) {
                         Code->info.opnd_type[OPND2] = OP_I16;
                    } else {
                         Code->info.opnd_type[OPND2] = OP_I8;
                    }
                } else if( ( op1 | op2 ) & ( OP_MMX | OP_XMM ) ) {
                } else {
#if defined( _STANDALONE_ )
                    AsmIntErr( 1 );
#endif
                    switch( op2_size ) {
                    case 1:
                        Code->mem_type = MT_BYTE;
#if defined( _STANDALONE_ )
                        if( ( Parse_Pass == PASS_1 ) && ( op2 & OP_I ) ) {
                            if( (Options.mode & MODE_IDEAL) == 0 ) {
                                AsmWarn( 1, ASSUMING_BYTE );
                            }
                        }
#endif
                        break;
                    case 2:
                        Code->mem_type = MT_WORD;
                        Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
                        if( ( Parse_Pass == PASS_1 ) && ( op2 & OP_I ) ) {
                            if( (Options.mode & MODE_IDEAL) == 0 ) {
                                AsmWarn( 1, ASSUMING_WORD );
                            }
                        }
#endif
                        if( Code->use32 )
                            Code->prefix.opsiz = TRUE;
                        break;
                    case 4:
                        Code->mem_type = MT_DWORD;
                        Code->info.opcode |= W_BIT;
#if defined( _STANDALONE_ )
                        if( ( Parse_Pass == PASS_1 ) && ( op2 & OP_I ) ) {
                             if( (Options.mode & MODE_IDEAL) == 0 ) {
                                AsmWarn( 1, ASSUMING_DWORD );
                            }
                        }
#endif
                        break;
                    }
                }
            }
        }
    }
    return( state );
}

void AsmInit( int cpu, int fpu, int use32, int extn )
/***************************************************/
{
    int         count;

    for( count = 0; count < MAX_TOKEN; count ++ ) {
        AsmBuffer[count] = &tokens[count];
    }

    if( use32 < 0 )
        use32 = 0;   // default is 16-bit segment
    if( cpu < 0 )
        cpu = 0;     // default is 8086 CPU
    if( fpu < 0 )
        fpu = 1;     // default is FPU use
    if( extn < 0 )
        extn = 0;    // default is no CPU extension instructions
    switch( use32 ) {
    case 0:
        Code->use32 = 0;
        break;
    case 1:
        Code->use32 = 1;
        break;
    }
    switch( cpu ) {
    case 0:
        Code->info.cpu = P_86;
        if( fpu )
            Code->info.cpu |= P_87;
        break;
    case 1:
        Code->info.cpu = P_186;
        if( fpu )
            Code->info.cpu |= P_87;
        break;
    case 2:
        Code->info.cpu = P_286p;
        if( fpu )
            Code->info.cpu |= P_287;
        break;
    case 3:
        Code->info.cpu = P_386p;
        if( fpu )
            Code->info.cpu |= P_387;
        break;
    case 4:
        Code->info.cpu = P_486p;
        if( fpu )
            Code->info.cpu |= P_387;
        break;
    case 5:
        Code->info.cpu = P_586p;
        if( fpu )
            Code->info.cpu |= P_387;
        if( extn )
            Code->info.cpu |= P_K3D | P_MMX;
        break;
    case 6:
        Code->info.cpu = P_686p;
        if( fpu )
            Code->info.cpu |= P_387;
        if( extn )
            Code->info.cpu |= P_K3D | P_MMX | P_SSE | P_SSE2 | P_SSE3;
        break;
    }
}

#if !defined( _STANDALONE_ )

static enum asm_cpu CPUinfo;

void AsmSaveCPUInfo( void )
{
    CPUinfo = Code->info.cpu;
}

void AsmRestoreCPUInfo( void )
{
    Code->info.cpu = CPUinfo;
}

#endif
