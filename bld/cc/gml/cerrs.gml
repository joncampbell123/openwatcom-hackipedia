:cmt *****************************************************************************
:cmt *
:cmt *                            Open Watcom Project
:cmt *
:cmt *    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
:cmt *
:cmt *  ========================================================================
:cmt *
:cmt *    This file contains Original Code and/or Modifications of Original
:cmt *    Code as defined in and that are subject to the Sybase Open Watcom
:cmt *    Public License version 1.0 (the 'License'). You may not use this file
:cmt *    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
:cmt *    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
:cmt *    provided with the Original Code and Modifications, and is also
:cmt *    available at www.sybase.com/developer/opensource.
:cmt *
:cmt *    The Original Code and all software distributed under the License are
:cmt *    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
:cmt *    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
:cmt *    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
:cmt *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
:cmt *    NON-INFRINGEMENT. Please see the License for the specific language
:cmt *    governing rights and limitations under the License.
:cmt *
:cmt *  ========================================================================
:cmt *
:cmt * Description:  C compiler diagnostic messages.
:cmt *
:cmt *****************************************************************************
:cmt

:cmt    Word usage:
:cmt
:cmt    'parameter' is used for macro parms
:cmt    'argument' is used for function arguments

:cmt    GML Macros used (see MSGMACS.GML):

:cmt        :ansi <n>   warning if extensions enabled, error otherwise
:cmt        :ansierr    ignored if extensions enabled, error otherwise
:cmt        :ansiwarn <n>   ignored if extensions enabled, warn otherwise
:cmt        :warning <n>    message is a warning with a specific level
:cmt        :info       informational message
:cmt
:cmt        :msgsym <sym>   internal symbolic name for message
:cmt        :msgtxt <text>  text for message
:cmt
:cmt        :msglvl     start of header title for a group of messages
:cmt        :emsglvl    end of header title for a group of messages
:cmt        :errbad     start of an example that generates an error msg
:cmt        :eerrbad    end of an example that generates an error msg
:cmt        :errgood    start of an example that compiles clean
:cmt        :eerrgood   end of an example that compiles clean
:cmt
:cmt        .kw     highlight a keyword
:cmt        .id     highlight an identifier
:cmt        .ev     highlight an environment variable
:cmt        .us     italicize a phrase
:cmt        .np     start a new paragraph

:cmt    The following substitutions are made:
:cmt        &incvarup   environment variable for include path
:cmt        &wclname    Compile and Link utility name

:cmt    Note for translators:

:cmt    Japanese error messages are supported via the :MSGJTXT tag.
:cmt    If there is no :MSGJTXT. for a particular :MSGSYM. then the
:cmt    message will come out in English.  Translation may proceed
:cmt    by translating error messages that do not have the :MSGJTXT.
:cmt    tag present and adding the :MSGJTXT. tag with the Japanese
:cmt    text after the :MSGTXT. tag.  If the :MSGJTXT. has no text
:cmt    then the error message must also be translated.  This has
:cmt    been found to be easier when searching for messages that
:cmt    still need to be translated.

:cmt -------------------------------------------------------------------
:MSGGRP. Warn1
:MSGGRPSTR. W
:MSGGRPNUM. 100
:MSGGRPTXT. Warning Level 1 Messages
:cmt -------------------------------------------------------------------
:MSGSYM. ERR_PARM_INCONSISTENT_INDIRECTION_LEVEL
:MSGTXT. Parameter %d contains inconsistent levels of indirection
:MSGJTXT. �p�����[�^%d�͖����������x���̊ԐڎQ�Ƃ��܂�ł��܂�
:WARNING. 1
.np
The function is expecting something like
.id char **
and it is being passed a
.id char *
for instance.
:MSGSYM. ERR_NONPORTABLE_PTR_CONV
:MSGTXT. Non-portable pointer conversion
:MSGJTXT. �ڐA���̂Ȃ��|�C���^�ϊ��ł�
:WARNING. 1
.np
This message is issued whenever you convert a non-zero constant to a pointer.
:MSGSYM. ERR_PCTYPE_MISMATCH
:MSGTXT. Type mismatch (warning)
:MSGJTXT. �^����v���܂���
:WARNING. 1
.np
This message is issued for a function return value or an assignment where
both types are pointers, but they are pointers to different kinds of objects.
:MSGSYM. ERR_PARM_COUNT_WARNING
:MSGTXT. Parameter count does not agree with previous definition (warning)
:MSGJTXT. �p�����[�^�[�����C�O�̒�`�ƈ�v���܂���
:WARNING. 1
.np
You have either not enough parameters or too many parameters in a call
to a function.
If the function is supposed to have a variable number of parameters, then
you can ignore this warning, or you can change the function declaration
and prototypes to use the ",..." to indicate that the function indeed
takes a variable number of parameters.
:MSGSYM. ERR_INCONSISTENT_INDIRECTION_LEVEL
:MSGTXT. Inconsistent levels of indirection
:MSGJTXT. �ԐڎQ�Ƃ̃��x�����������Ă��܂�
:WARNING. 1
.np
This occurs in an assignment or return statement when one of the operands
has more levels of indirection than the other operand.
For example, a
.id char **
is being assigned to a
.id char *.
.np
Solution: Correct the levels of indirection or use a
.id void *.
:MSGSYM. ERR_ASSIGNMENT_IN_BOOL_EXPR
:MSGTXT. Assignment found in boolean expression
:MSGJTXT. �_�����̒��ɑ��������܂�
:WARNING. 1
.np
An assignment of a constant has been detected in a boolean expression.
For example: "if( var = 0 )".
It is most likely that you want to use "==" for testing for equality.
:MSGSYM. ERR_CONSTANT_TOO_BIG
:MSGTXT. Constant out of range - truncated
:MSGJTXT. �͈͊O�̒萔�ł� - �؂�l�߂��܂���
:WARNING. 1
.np
This message is issued if a constant cannot be represented in 32 bits
or if a constant is outside the range of valid values
that can be assigned to a variable.
:MSGSYM. ERR_MISSING_RETURN_VALUE
:MSGTXT. Missing return value for function '%s'
:MSGJTXT. �֐�'%s'�̂��߂̖߂�l������܂���
:WARNING. 1
.np
A function has been declared with a function return type, but no
.kw return
statement was found in the function.  Either add a
.kw return
statement or change the function return type to
.kw void.
:MSGSYM. ERR_DUPLICATE_TYPEDEF_ALREADY_DEFINED
:MSGTXT. Duplicate typedef already defined
:MSGJTXT. ���łɒ�`���ꂽtypedef���Ē�`���Ă��܂�
:WARNING. 1
.np
A duplicate typedef is not allowed in ISO C.
This warning is issued when compiling with extensions enabled.
You should delete the duplicate typedef definition.
:MSGSYM. ERR_UNUSED_1
:MSGTXT. not used
:MSGJTXT. ���̃��b�Z�[�W�͎g�p����܂���
:WARNING. 1
unused message
:MSGSYM. ERR_FORTRAN_PRAGMA_NOT_DEFINED
:MSGTXT. 'fortran' pragma not defined
:MSGJTXT. 'fortran'�͒�`����Ă��܂���
:WARNING. 1
.np
You have used the
.kw fortran
keyword in your program, but have not defined a #pragma for
.kw fortran.
:MSGSYM. ERR_MEANINGLESS
:MSGTXT. Meaningless use of an expression
:MSGJTXT. ���̎��̎g�p�͖��Ӗ��ł�
:WARNING. 1
.np
The line contains an expression that does nothing useful.
In the example "i = (1,5);", the expression "1," is meaningless.
:MSGSYM. ERR_POINTER_TRUNCATION
:MSGTXT. Pointer truncated
:MSGJTXT. �|�C���^�͐؂�l�߂��܂���
:WARNING. 1
.np
A far pointer is being passed to a function that is expecting a near
pointer, or a far pointer is being assigned to a near pointer.
:MSGSYM. ERR_POINTER_TYPE_MISMATCH
:MSGTXT. Pointer type mismatch
:MSGJTXT. �|�C���^�[�^���s��v�ł�
:WARNING. 1
.np
You have two pointers that either point to different objects, or the
pointers are of different size, or they have different modifiers.
:MSGSYM. ERR_MISSING_LAST_SEMICOLON
:MSGTXT. Missing semicolon
:MSGJTXT. �Z�~�R����������܂���
:WARNING. 1
.np
You are missing the semicolon ";" on the field definition just before
the right curly brace "}".
:MSGSYM. ERR_ADDR_OF_ARRAY
:MSGTXT. &array may not produce intended result
:MSGJTXT. �z�񖼂ւ�&���Z�q�̎g�p�C�Ӑ}���ꂽ���ʂ𐶂��Ȃ���������܂���
:WARNING. 1
.np
The type of the expression "&array" is different from the type of the
expression "array".
Suppose we have the declaration
.id char buffer[80];
Then the expression
.id (&buffer + 3)
will be evaluated as
.id (buffer + 3 * sizeof(buffer))
which is
.id (buffer + 3 * 80)
and not
.id (buffer + 3 * 1)
which is what most people expect to happen.
The address of operator "&" is not required for getting the address of an
array.
:MSGSYM. ERR_RET_ADDR_OF_AUTO
:MSGTXT. Attempt to return address of auto variable
:MSGJTXT. �����ϐ��̃A�h���X��Ԃ����Ƃ��܂�
:WARNING. 1
.np
This warning usually indicates a serious programming error.
When a function exits, the storage allocated on the stack for
auto variables is released.
This storage will be overwritten by further function calls
and/or hardware interrupt service routines.
Therefore, the data pointed to by the return value may be destroyed
before your program has a chance to reference it or make a copy of it.
:MSGSYM. ERR_PASTE_TOKEN_DISCARDED
:MSGTXT. '##' tokens did not generate a single token (rest discarded)
:MSGJTXT. '##'�g�[�N�����C�P��g�[�N���𐶐����܂���ł����i�c��͎̂Ă��܂����j
:WARNING. 1
.np
When two tokens are pasted together using ##, they must form a string
that can be parsed as a single token.
:MSGSYM. ERR_UNREFERENCED_LABEL
:MSGTXT. Label '%s' has been defined but not referenced
:MSGJTXT. ���x��'%s'����`����܂������C�Q�Ƃ���܂���ł���
:WARNING. 1
.np
You have defined a label that is not referenced in a
.kw goto
statement.
It is possible that you are missing the
.kw case
keyword when using an enumerated type name as a case in a
.kw switch
statement.
If not, then the label can be deleted.
:MSGSYM. ERR_ADDR_OF_STATIC_FUNC_TAKEN
:MSGTXT. Address of static function '%s' has been taken
:MSGJTXT. �X�^�e�B�b�N�֐�'%s'�̃A�h���X���Ƃ��܂���
:WARNING. 1
.np
This warning may indicate a potential problem when the program is overlayed.
:MSGSYM. ERR_LVALUE_CAST
:MSGTXT. lvalue cast is not standard C
:MSGJTXT. ���Ӓl�̃L���X�g�́C�W����C�ł͂���܂���
:WARNING. 1
.np
A cast operation does not yield an lvalue in ISO C.
However, to provide compatibility with code written prior to the availability
of ISO compliant C compilers, if an expression was an lvalue prior to the
cast operation, and the cast operation does not cause any conversions, the
compiler treats the result as an lvalue and issues this warning.
:MSGSYM. ERR_JUNK_FOLLOWS_DIRECTIVE
:MSGTXT. Text following pre-processor directives is not standard C
:MSGJTXT. �v���v���Z�b�T�[�����߂ɑ����Ă���e�L�X�g���C�W����C�ł͂���܂���
:WARNING. 1
.np
Arbitrary text is not allowed following a pre-processor directive.
Only comments are allowed following a pre-processor directive.
:MSGSYM. ERR_LIT_TOO_LONG
:MSGTXT. Literal string too long for array - truncated
:MSGJTXT. ���e���������񂪔z��ɑ΂��Ē������܂� - �؂�l�߂��܂���
:WARNING. 1
.np
The supplied literal string contains more characters than the specified
dimension of the array.
Either shorten the literal string, or increase the dimension of the array
to hold all of the characters from the literal string.
:MSGSYM. ERR_SPLICE_IN_CPP_COMMENT
:MSGTXT. '//' style comment continues on next line
:MSGJTXT. '//'�`���̃R�����g�����s�ɑ����Ă��܂�
:WARNING. 1
.np
The compiler has detected a line continuation during the processing
of a C++ style comment ("//").  The warning can be removed by switching to
a C style comment ("/**/").  If you require the comment to be terminated
at the end of the line, make sure that the backslash character is not
the last character in the line.
.errbad
#define XX 23 // comment start \
comment \
end

int x = XX; // comment start ...\
comment end
.eerrbad
:MSGSYM. ERR_COMPARE_ALWAYS
:MSGTXT. Comparison result always %d
:MSGJTXT. ��r�̌��ʂ͏��%d�ł�
:WARNING. 1
.np
The line contains a comparison that is always true (1) or false (0).
For example comparing an unsigned expression to see if it is >= 0 or < 0
is redundant.
Check to see if the expression should be signed instead of unsigned.
:MSGSYM. ERR_INCDEPTH
:MSGTXT. Nested include depth of %d exceeded
:MSGJTXT. �l�X�g�����C���N���[�h�̐[����%d�𒴂��܂���
:WARNING. 1
.np
The number of nested include files has reached a  preset limit,
check for recursive include statements.
:MSGSYM. NON_ZERO_CONST
:MSGTXT. Constant must be zero for pointer compare
:MSGJTXT. �|�C���^�Ɣ�r����萔�̓[���łȂ���΂Ȃ�܂���
:WARNING. 1
.np
A pointer is being compared using == or != to a non-zero constant.
:MSGSYM. ERR_EXPANDED_TRIGRAPH
:MSGTXT. trigraph found in string
:MSGJTXT. ���ƂȂ�O�d�����i�g���C�O���t�j������܂�
:WARNING. 1
.np
Trigraph expansion occurs inside a  string literal.
This warning can be disabled
via the command line or
.kw #pragma warning
directive.
.exam begin
// string expands to "(?]?~????"!
char *e = "(???)???-????";
// possible work-arounds
char *f = "(" "???" ")" "???" "-" "????";
char *g = "(\?\?\?)\?\?\?-\?\?\?\?";
.exam end
:MSGSYM. ERR_SLACK_ADDED
:MSGTXT. %d padding byte(s) added
:MSGJTXT. %d�o�C�g�̃p�f�B���O���������܂���
:WARNING. 1
.np
The compiler has added slack bytes to align a member to
the correct offset.
:MSGSYM. ERR_WEIRD_ENDIF_ENCOUNTER
:MSGTXT. #endif matches #if in different source file '%s'
:MSGJTXT. #endif�����̃\�[�X�t�@�C��'%s'��#if�ƑΉ����Ă��܂�
:WARNING. 1
This warning may indicate a
.kw #endif
nesting problem since the traditional usage of
.kw #if
directives is confined to the same source file.
This warning may often come before an error
and it is hoped will provide information to
solve a preprocessing directive problem.
:MSGSYM. ERR_LOSE_PRECISION
:MSGTXT. Possible loss of precision
:MSGJTXT. ���x�������Ă���\��������܂��B
:WARNING. 1
.np
This warning indicates that you may be converting a argument
of one size to another, different size. For instance, you may be
losing precision by passing a long argument to a function that
takes a short.
This warning is initially disabled. It must be explicitly enabled with
.us #pragma enable_message(130)
or option "-wce=130". It can be disabled later by using
.us #pragma disable_message(130).
:MSGSYM. ERR_ASSUMED_IMPORT
:MSGTXT. No prototype found for function '%s'
:MSGJTXT. '%s'�̃v���g�^�C�v������܂���
:WARNING. 1
.np
A reference for a function appears in your program, but you do not
have a prototype for that function defined. Implicit prototype will
be used, but this will cause problems if the assumed prototype does
not match actual function definition.
:MSGSYM. ERR_NO_STG_OR_TYPE
:MSGTXT. No storage class or type specified
:MSGJTXT. �L���N���X�܂��͌^���w�肳��Ă��܂���
:WARNING. 1
.np
When declaring a data object, either storage class or data type must be
given. If no type is specified,
.kw int
is assumed. If no storage class is specified, the default depends on
scope (see the
.us C Language Reference
for details). For instance
.errgood
auto i;
.eerrgood
is a valid declaration, as is
.errgood
short i;
.eerrgood
However,
.errbad
i;
.eerrbad
is not a correctly formed declaration.
:MSGSYM. ERR_SYMBOL_NAME_TOO_LONG
:MSGTXT. Symbol name truncated for '%s'
:MSGJTXT. �V���{������'%s'�ɐ؎̂Ă��܂���
:WARNING. 1
.np
Symbol is longer than the object file format allows and has been truncated
to fit. Maximum length is 255 characters for OMF and 1024 characters for
COFF or ELF object files.
:MSGSYM. ERR_SHIFT_AMOUNT_NEGATIVE
:MSGTXT. Shift amount negative
:MSGJTXT. �V�t�g�ʂ����ł�
:WARNING. 1
.np
The right operand of a left or right shift operator is a negative value.
The result of the shift operation is undefined.
:errbad.
int a = 1 << -2;
:eerrbad.
The value of 'a' in the above example is undefined.
:MSGSYM. ERR_SHIFT_AMOUNT_TOO_BIG
:MSGTXT. Shift amount too large
:MSGJTXT. �V�t�g�ʂ��傫�߂��܂�
:WARNING. 1
.np
The right operand of a left or right shift operator is a value greater than
or equal to the width in bits of the type of the promoted left operand.
The result of the shift operation is undefined.
:errbad.
int a = 1 >> 123;
:eerrbad.
The value of 'a' in the above example is undefined.
:MSGSYM. ERR_COMPARE_UNSIGNED_VS_ZERO
:MSGTXT. Comparison equivalent to 'unsigned == 0'
:MSGJTXT. ��r�̌��ʂ͏��%d�ł�
:WARNING. 1
.np
Comparing an unsigned expression to see whether it is <= 0 is equivalent to
testing for == 0.
Check to see if the expression should be signed instead of unsigned.
:MSGSYM. ERR_FUNCTION_STG_CLASS_REDECLARED
:MSGTXT. Extern function '%s' redeclared as static
:MSGJTXT. �O���֐�'%s'��static�Ƃ��čĐ錾����܂���
:WARNING. 1
.np
The specified function was either explicitly or implicitly declared as
.kw extern
and later redeclared as
.kw static.
This is not allowed in ISO C and may produce unexpected results with ISO
compliant compilers.
:errbad.
int bar( void );

void foo( void )
{
    bar();
}

static int bar( void )
{
    return( 0 );
}
:eerrbad.
:MSGSYM. ERR_NO_EOL_BEFORE_EOF
:MSGTXT. No newline at end of file
:MSGJTXT. �t�@�C���̍Ō�ɉ��s����������܂���
:WARNING. 1
ISO C requires that a non-empty source file must include a newline character
at the end of the last line. If no newline was found, it will be automatically
inserted.
:MSGSYM. ERR_DIV_BY_ZERO
:MSGTXT. Divisor for modulo or division operation is zero
:MSGJTXT. Divisor for modulo or division operation is zero
:WARNING. 2
.np
The right operand of a division or modulo operation is zero. The result
of this operation is undefined and you should rewrite the offending code
to avoid divisions by zero.
:errbad.
int foo( void )
{
    return( 7 / 0 );
}
:eerrbad.
:eMSGGRP. Warn1
:cmt -------------------------------------------------------------------
:MSGGRP. Warn2
:MSGGRPSTR. W
:MSGGRPNUM. 200
:MSGGRPTXT. Warning Level 2 Messages
:cmt -------------------------------------------------------------------
:MSGSYM. ERR_SYM_NOT_ASSIGNED
:MSGTXT. '%s' has been referenced but never assigned a value
:MSGJTXT. '%s'�͎Q�Ƃ���܂������C�l���������Ă��܂���
:WARNING. 2
.np
You have used the variable in an expression without previously assigning
a value to that variable.
:MSGSYM. ERR_DEAD_CODE
:MSGTXT. Unreachable code
:MSGJTXT. ���s����Ȃ��R�[�h�ł�
:WARNING. 2
.np
The statement will never be executed, because there is no path through
the program that causes control to reach this statement.
:MSGSYM. ERR_SYM_NOT_REFERENCED
:MSGTXT. Symbol '%s' has been defined, but not referenced
:MSGJTXT. �V���{��'%s'����`����܂������C�Q�Ƃ���܂���ł���
:WARNING. 2
.np
There are no references to the declared variable.
The declaration for the variable can be deleted.
.np
In some cases, there may be a valid reason for retaining the variable.
You can prevent the message from being issued through use of
.us #pragma off(unreferenced).
:MSGSYM. ERR_UNDECLARED_PP_SYM
:MSGTXT. Preprocessing symbol '%s' has not been declared
:MSGJTXT. �O�����V���{��'%s'���C�錾����܂���ł���
:WARNING. 2
.np
The symbol has been used in a preprocessor expression.
The compiler assumes the symbol has a value of 0 and continues.
A
.id #define
may be required for the symbol, or you may have forgotten
to include the file which contains a
.id #define
for the symbol.
:eMSGGRP. Warn2
:cmt -------------------------------------------------------------------
:MSGGRP. Warn3
:MSGGRPSTR. W
:MSGGRPNUM. 300
:MSGGRPTXT. Warning Level 3 Messages
:cmt -------------------------------------------------------------------
:MSGSYM. ERR_NESTED_COMMENT
:MSGTXT. Nested comment found in comment started on line %u
:MSGJTXT. %u�s����n�܂�R�����g�̒��ɁC�l�X�g�ɂ��ꂽ�R�����g������܂�
:WARNING. 3
.np
While scanning a comment for its end, the compiler detected
.id /*
for the start of another comment.
Nested comments are not allowed in ISO C.
You may be missing the
.id */
for the previous comment.
:MSGSYM. ERR_UNUSED_2
:MSGTXT. not used
:MSGJTXT. ���̃��b�Z�[�W�͎g�p����܂���
:WARNING. 2
unused message
:MSGSYM. ERR_USEFUL_SIDE_EFFECT
:MSGTXT. Expression is only useful for its side effects
:MSGJTXT. ���̎��͕���p�݂̂��N�����܂�
:WARNING. 3
.np
You have an expression that would have generated the warning
"Meaningless use of an expression", except that it also contains
a side-effect, such as ++, &minus.&minus., or a function call.
:MSGSYM. ERR_PARM_NOT_REFERENCED
:MSGTXT. Parameter '%s' has been defined, but not referenced
:MSGJTXT. �p�����[�^'%s'�͒�`����܂������C�Q�Ƃ���܂���ł���
:WARNING. 3
.np
There are no references to the declared parameter.
The declaration for the parameter can be deleted.
Since it is a parameter to a function, all calls to the function
must also have the value for that parameter deleted.
.np
In some cases, there may be a valid reason for retaining the parameter.
You can prevent the message from being issued through use of
.us #pragma off(unreferenced).
.np
This warning is initially disabled. It must be specifically enabled with
.us #pragma enable_message(303)
or option "-wce=303". It can be disabled later by using
.us #pragma disable_message(303).
:MSGSYM. ERR_NO_RET_TYPE_GIVEN
:MSGTXT. Return type 'int' assumed for function '%s'
:MSGJTXT. �֐�'%s'�̖߂�^��int�Ƃ݂Ȃ��܂�
:WARNING. 3
.np
If a function is declared without specifying return type, such as
.errbad
foo( void );
.eerrbad
then its return type will be assumed to be
.kw int
.
:MSGSYM. ERR_NO_DATA_TYPE_GIVEN
:MSGTXT. Type 'int' assumed in declaration of '%s'
:MSGJTXT. '%s'�̐錾�ɂ�����^��int�Ƃ݂Ȃ��܂�
:WARNING. 3
.np
If an object is declared without specifying its type, such as
.errbad
register count;
.eerrbad
then its type will be assumed to be
.kw int
.
:MSGSYM. ERR_ASSEMBLER_WARNING
:MSGTXT. Assembler warning: '%s'
:MSGJTXT. �A�Z���u���x��: '%s'
:WARNING. 3
.np
A problem has been detected by the in-line assembler.
The message indicates the problem detected.
:MSGSYM. ERR_OBSOLETE_FUNC_DECL
:MSGTXT. Obsolete non-prototype declarator
:MSGJTXT. �p�~���ꂽ��v���g�^�C�v�錾�q�ł�
:WARNING. 3
.np
Function parameter declarations containing only empty parentheses,
that is, non-prototype declarations, are an obsolescent language feature.
Their use is dangerous and discouraged.
.errbad
int func();
.eerrbad
:MSGSYM. ERR_NONPROTO_FUNC_CALLED
:MSGTXT. Unprototyped function '%s' called
:MSGJTXT. ��v���g�^�C�v�̊֐�'%s'���Ă΂�܂���
:WARNING. 3
.np
A call to an unprototyped function was made, preventing the compiler
from checking the number of function arguments and their types. Use
of unprototyped functions is obsolescent, dangerous and discouraged.
.errbad
int func();

void bar( void )
{
    func( 4, "s" );     /* possible argument mismatch */
}
.eerrbad
:MSGSYM. ERR_NONPROTO_FUNC_CALLED_INDIRECT
:MSGTXT. Unprototyped function indirectly called
:MSGJTXT. ��v���g�^�C�v�̊֐����ԐړI�ɌĂ΂�܂���
:WARNING. 3
.np
An indirect call to an unprototyped function was made, preventing the
compiler from checking the number of function arguments and their types.
Use of unprototyped functions is obsolescent, dangerous and discouraged.
.errbad
int (*func)();

void bar( void )
{
    func( 4, "s" );     /* possible argument mismatch */
}
.eerrbad
:MSGSYM. ERR_CAST_POINTER_TRUNCATION
:MSGTXT. Pointer truncated during cast
:MSGJTXT. �|�C���^�͐؂�l�߂��܂���
:WARNING. 1
.np
A far pointer is being cast to a near pointer, losing segment information
in the process.
.errbad
char __near *foo( char __far *fs )
{
    return( (char __near *)fs );
}
.eerrbad
:eMSGGRP. Warn3
:cmt -------------------------------------------------------------------
:MSGGRP. Warn4
:MSGGRPSTR. W
:MSGGRPNUM. 400
:MSGGRPTXT. Warning Level 4 Messages
:cmt -------------------------------------------------------------------
:MSGSYM. ERR_PLAIN_CHAR_SUBSCRIPT
:MSGTXT. Array subscript is of type plain char
:MSGJTXT. jNeedTranslation
:WARNING. 3
.np
Array subscript expression is of plain char type. Such expression may be
interpreted as either signed or unsigned, depending on compiler settings.
A different type should be chosen instead of char. A cast may be used in
cases when the value of the expression is known to never fall outside the
0-127 range.
.errbad
int foo( int arr[], char c )
{
    return( arr[c] );
}
.eerrbad
:eMSGGRP. Warn4
:cmt -------------------------------------------------------------------
:MSGGRP. Errs
:MSGGRPSTR. E
:MSGGRPNUM. 1000
:MSGGRPTXT. Error Messages
:cmt -------------------------------------------------------------------
:MSGSYM. ERR_MISPLACED_BREAK
:MSGTXT. BREAK must appear in while, do, for or switch statement
:MSGJTXT. BREAK��while, do, for, switch���̒��ɂȂ���΂Ȃ�܂���
.np
A
.kw break
statement has been found in an illegal place in the program.
You may be missing an opening brace
.id {
for a
.kw while,
.kw do,
.kw for
or
.kw switch
statement.
:MSGSYM. ERR_MISPLACED_CASE
:MSGTXT. CASE must appear in switch statement
:MSGJTXT. CASE��switch���̒��ɂȂ���΂Ȃ�܂���
.np
A
.kw case
label has been found that is not inside a
.kw switch
statement.
:MSGSYM. ERR_MISPLACED_CONTINUE
:MSGTXT. CONTINUE must appear in while, do or for statement
:MSGJTXT. CONTINUE��while, do, for���̒��ɂȂ���΂Ȃ�܂���
.np
The
.kw continue
statement must be inside a
.kw while,
.kw do
or
.kw for
statement.
You may have too many
.id }
between the
.kw while,
.kw do
or
.kw for
statement and the
.kw continue
statement.
:MSGSYM. ERR_MISPLACED_DEFAULT
:MSGTXT. DEFAULT must appear in switch statement
:MSGJTXT. DEFAULT��switch���̒��ɂȂ���΂Ȃ�܂���
.np
A
.kw default
label has been found that is not inside a
.kw switch
statement.
You may have too many
.id }
between the start of the
.kw switch
and the
.kw default
label.
:MSGSYM. ERR_MISPLACED_RIGHT_BRACE
:MSGTXT. Misplaced '}' or missing earlier '{'
:MSGJTXT. �Ԉ�����ʒu��'}'�����邩�C�����ƑO�ɂ���ׂ�'{'������܂���
.np
An extra
.id }
has been found which cannot be matched up with an earlier
.id {.
:MSGSYM. ERR_MISPLACED_ELIF
:MSGTXT. Misplaced #elif directive
:MSGJTXT. #elif�[�����߂̈ʒu���Ԉ���Ă��܂�
.np
The
.id #elif
directive must be inside an
.id #if
preprocessing group and before the
.id #else
directive if present.
:MSGSYM. ERR_MISPLACED_ELSE
:MSGTXT. Misplaced #else directive
:MSGJTXT. #else�[�����߂̈ʒu���Ԉ���Ă��܂�
.np
The
.id #else
directive must be inside an
.id #if
preprocessing group and follow all
.id #elif
directives if present.
:MSGSYM. ERR_MISPLACED_ENDIF
:MSGTXT. Misplaced #endif directive
:MSGJTXT. #endif�[�����߂̈ʒu���Ԉ���Ă��܂�
.np
A
preprocessing directive has been found without a matching
.id #if
directive.
You either have an extra
or you are missing an
.id #if
directive earlier in the file.
:MSGSYM. ERR_ONLY_1_DEFAULT
:MSGTXT. Only 1 DEFAULT per switch allowed
:MSGJTXT. 1��switch���ɂ�1��DEFAULT�������F�߂��܂�
.np
You cannot have more than one
.kw default
label in a
.kw switch
statement.
:MSGSYM. ERR_EXPECTING_BUT_FOUND
:MSGTXT. Expecting '%s' but found '%s'
:MSGJTXT. '%s'������͂��ł����C'%s'������܂���
.np
A syntax error has been detected.
The tokens displayed in the message should help you to determine the problem.
:MSGSYM. ERR_TYPE_MISMATCH
:MSGTXT. Type mismatch
:MSGJTXT. �^����v���܂���
.np
For pointer subtraction, both pointers must point to the same type.
For other operators, both expressions must be assignment compatible.
:MSGSYM. ERR_UNDECLARED_SYM
:MSGTXT. Symbol '%s' has not been declared
:MSGJTXT. �V���{��'%s'���錾����܂���ł���
.np
The compiler has found a symbol which has not been previously declared.
The symbol may be spelled differently than the declaration, or you may
need to
.id #include
a header file that contains the declaration.
:MSGSYM. ERR_NOT_A_FUNCTION
:MSGTXT. Expression is not a function
:MSGJTXT. �����֐��ł͂���܂���
.np
The compiler has found an expression that looks like a function call, but
it is not defined as a function.
:MSGSYM. ERR_CANNOT_MODIFY_CONST
:MSGTXT. Constant variable cannot be modified
:MSGJTXT. �萔�ϐ��͕ύX�ł��܂���
.np
An expression or statement has been found which modifies a variable which
has been declared with the
.kw const
keyword.
:MSGSYM. ERR_MUST_BE_LVALUE
:MSGTXT. Left operand must be an 'lvalue'
:MSGJTXT. ���ӂ�'���Ӓl'�łȂ���΂Ȃ�܂���
.np
The operand on the left side of an "=" sign must be a variable or memory
location which can have a value assigned to it.
:MSGSYM. ERR_SYM_ALREADY_DEFINED_AS_VAR
:MSGTXT. '%s' is already defined as a variable
:MSGJTXT. '%s'�͊��ɕϐ��Ƃ��Ē�`����Ă��܂�
.np
You are trying to declare a function with the same name as a previously
declared variable.
:MSGSYM. ERR_EXPECTING_ID
:MSGTXT. Expecting identifier
:MSGJTXT. ���ʎq������ׂ��ł�
.np
The token following "->" and "." operators must be the name of an
identifier which appears in the struct or union identified by the
operand preceding the "->" and "." operators.
:MSGSYM. ERR_LABEL_ALREADY_DEFINED
:MSGTXT. Label '%s' already defined
:MSGJTXT. ���x��'%s'�͊��ɒ�`����Ă��܂�
.np
All labels within a function must be unique.
:MSGSYM. ERR_UNDEFINED_LABEL
:MSGTXT. Label '%s' not defined in function
:MSGJTXT. ���x��'%s'�͊֐��̒��Œ�`����Ă��܂���
.np
A
.kw goto
statement has referenced a label that is not defined in the function.
Add the necessary label or check the spelling of the label(s) in the function.
:MSGSYM. ERR_DUPLICATE_TAG
:MSGTXT. Tag '%s' already defined
:MSGJTXT. �^�O'%s'�͊��ɒ�`����Ă��܂�
.np
All
.kw struct,
.kw union
and
.kw enum
tag names must be unique.
:MSGSYM. ERR_INVALID_DIMENSION
:MSGTXT. Dimension cannot be 0 or negative
:MSGJTXT. ������0�܂��͕��ł��邱�Ƃ͂ł��܂���
.np
The dimension of an array must be positive and non-zero.
:MSGSYM. ERR_DIMENSION_REQUIRED
:MSGTXT. Dimensions of multi-dimension array must be specified
:MSGJTXT. �������z��̎����͎w�肳��Ȃ���΂Ȃ�܂���
.np
All dimensions of a multiple dimension array must be specified.
The only exception is the first dimension which can declared as "[]".
:MSGSYM. ERR_MISSING_DATA_TYPE
:MSGTXT. Missing or misspelled data type near '%s'
:MSGJTXT. '%s'�̋߂��̃f�[�^�^���Ȃ����C�Â肪�Ԉ���ĂĂ��܂�
.np
The compiler has found an identifier that is not a predefined type or the
name of a "typedef".
Check the identifier for a spelling mistake.
:MSGSYM. ERR_INVALID_STG_CLASS_FOR_PARM
:MSGTXT. Storage class of parameter must be register or unspecified
:MSGJTXT. �p�����[�^�[�̋L���N���X�́C���W�X�^�����w��łȂ���΂Ȃ�܂���
.np
The only storage class allowed for a parameter declaration is
.kw register.
:MSGSYM. ERR_SYM_NOT_IN_PARM_LIST
:MSGTXT. Declared symbol '%s' is not in parameter list
:MSGJTXT. �錾���ꂽ�V���{��'%s'�̓p�����[�^�[���X�g�ɂ���܂���
.np
Make sure that all the identifiers in the parameter list match those
provided in the declarations between the start of the function and the
opening brace "{".
:MSGSYM. ERR_PARM_ALREADY_DECLARED
:MSGTXT. Parameter '%s' already declared
:MSGJTXT. �p�����[�^�['%s'�͊��ɐ錾����Ă��܂�
.np
A declaration for the specified parameter has already been processed.
:MSGSYM. ERR_INVALID_DECLARATOR
:MSGTXT. Invalid declarator
:MSGJTXT. �s�K�؂Ȑ錾�q�ł�
.np
A syntax error has occurred while parsing a declaration.
:MSGSYM. ERR_INVALID_STG_CLASS_FOR_FUNC
:MSGTXT. Invalid storage class for function
:MSGJTXT. �֐��ɑ΂��ĕs�K�؂ȋL���N���X�ł�
.np
If a storage class is given for a function, it must be
.kw static
or
.kw extern.
:MSGSYM. ERR_VAR_CANT_BE_VOID
:MSGTXT. Variable '%s' cannot be void
:MSGJTXT. �ϐ�'%s'��void�^�ł��邱�Ƃ͂ł��܂���
.np
You cannot declare a
.kw void
variable.
:MSGSYM. ERR_EXPR_MUST_BE_POINTER_TO
:MSGTXT. Expression must be 'pointer to ...'
:MSGJTXT. ����'...�ւ̃|�C���^'�łȂ���΂Ȃ�܂���
.np
An attempt has been made to de-reference (*) a variable or expression which
is not declared to be a pointer.
:MSGSYM. ERR_CANT_TAKE_ADDR_OF_RVALUE
:MSGTXT. Cannot take the address of an rvalue
:MSGJTXT. �E�Ӓl(���l�萔��)�̃A�h���X���Ƃ邱�Ƃ͂ł��܂���
.np
You can only take the address of a variable or memory location.
:MSGSYM. ERR_NAME_NOT_FOUND_IN_STRUCT
:MSGTXT. Name '%s' not found in struct/union %s
:MSGJTXT. ���O%s��struct/union %s �̒��ɂ���܂���
.np
The specified identifier is not one of the fields declared in the
.kw struct
or
.kw union.
Check that the field name is spelled correctly, or that you are pointing
to the correct
.kw struct
or
.kw union.
:MSGSYM. ERR_MUST_BE_STRUCT_OR_UNION
:MSGTXT. Expression for '.' must be a 'structure' or 'union'
:MSGJTXT. .�ɑ΂��鎮�́C'�\����'��'���p��'�łȂ���΂Ȃ�܂���
.np
The compiler has encountered the pattern "expression" "." "field_name"
where the expression is not a
.kw struct
or
.kw union
type.
:MSGSYM. ERR_MUST_BE_PTR_TO_STRUCT_OR_UNION
:MSGTXT. Expression for '->' must be 'pointer to struct or union'
:MSGJTXT. '->'�ɑ΂��鎮�́C'struct��union�ւ̃|�C���^'�łȂ���΂Ȃ�܂���
.np
The compiler has encountered the pattern "expression" "->" "field_name"
where the expression is not a pointer to
.kw struct
or
.kw union
type.
:MSGSYM. ERR_SYM_ALREADY_DEFINED
:MSGTXT. Symbol '%s' already defined
:MSGJTXT. �V���{��'%s'�͊��ɒ�`����Ă��܂�
.np
The specified symbol has already been defined.
:MSGSYM. ERR_FUNCTION_NOT_DEFINED
:MSGTXT. static function '%s' has not been defined
:MSGJTXT. �X�^�e�B�b�N�֐�'%s'����`����Ă��܂���
.np
A prototype has been found for a
.kw static
function, but a definition for the
.kw static
function has not been found in the file.
:MSGSYM. ERR_RIGHT_OPERAND_IS_A_POINTER
:MSGTXT. Right operand of '%s' is a pointer
:MSGJTXT. '%s'�̉E�I�y�����h���|�C���^�ł�
.np
The right operand of "+=" and "&minus.=" cannot be a pointer.
The right operand of "&minus." cannot be a pointer unless the left
operand is also a pointer.
:MSGSYM. ERR_MUST_BE_SCALAR_TYPE
:MSGTXT. Type cast must be a scalar type
:MSGJTXT. �^�L���X�g�́C�X�J���[�^�łȂ���΂Ȃ�܂���
.np
You cannot type cast an expression to be a
.kw struct,
.kw union,
array or function.
:MSGSYM. ERR_EXPECTING_LABEL
:MSGTXT. Expecting label for goto statement
:MSGJTXT. goto���ɑ΂��郉�x��������ׂ��ł�
.np
The
.kw goto
statement requires the name of a label.
:MSGSYM. ERR_DUPLICATE_CASE_VALUE
:MSGTXT. Duplicate case value '%s' found
:MSGJTXT. case�̒l'%s'��2����܂�
.np
Every case value in a
.kw switch
statement must be unique.
:MSGSYM. ERR_FIELD_TOO_WIDE
:MSGTXT. Field width too large
:MSGJTXT. �t�B�[���h�̕����傫�����܂�
.np
The maximum field width allowed is 16 bits.
:MSGSYM. ERR_WIDTH_0
:MSGTXT. Field width of 0 with symbol not allowed
:MSGJTXT. �V���{��������0�̃t�B�[���h�͋�����܂���
.np
A bit field must be at least one bit in size.
:MSGSYM. ERR_WIDTH_NEGATIVE
:MSGTXT. Field width must be positive
:MSGJTXT. �t�B�[���h�̕��͐��̐��łȂ���΂Ȃ�܂���
.np
You cannot have a negative field width.
:MSGSYM. ERR_INVALID_TYPE_FOR_FIELD
:MSGTXT. Invalid type specified for bit field
:MSGJTXT. �r�b�g�t�B�[���h�ɕs�K�؂Ȍ^���w�肳��܂���
.np
The types allowed for bit fields are
.kw signed
or
.kw unsigned
varieties of
.kw char,
.kw short
and
.kw int.
:MSGSYM. ERR_INCOMPLETE_TYPE
:MSGTXT. Variable '%s' has incomplete type
:MSGJTXT. �ϐ�'%s'�̌^�͕s���S�ł�
.np
A full definition of a
.kw struct
or
.kw union
has not been given.
:MSGSYM. ERR_EXPR_MUST_BE_ARRAY
:MSGTXT. Subscript on non-array
:MSGJTXT. ��z��ւ̓Y�����ł�
.np
One of the operands of "[]" must be an array.
:MSGSYM. ERR_INCOMPLETE_COMMENT
:MSGTXT. Incomplete comment started on line %u
:MSGJTXT. Incomplete comment started on line %u
.np
The compiler did not find
.id */
to mark the end of a comment.
:MSGSYM. ERR_MUST_BE_MACRO_PARM
:MSGTXT. Argument for # must be a macro parm
:MSGJTXT. #�ւ̈����́C�}�N���E�p�����[�^�łȂ���΂Ȃ�܂���
.np
The argument for the stringize operator "#" must be a macro parameter.
:MSGSYM. ERR_UNKNOWN_DIRECTIVE
:MSGTXT. Unknown preprocessing directive '#%s'
:MSGJTXT. �O�����[������'#%s'�̈Ӗ����s���ł�
.np
An unrecognized preprocessing directive has been encountered.
Check for correct spelling.
:MSGSYM. ERR_INVALID_INCLUDE
:MSGTXT. Invalid #include directive
:MSGJTXT. �s�K�؂�#include�[�����߂ł�
.np
A syntax error has been encountered in a
.id #include
directive.
:MSGSYM. ERR_TOO_FEW_MACRO_PARMS
:MSGTXT. Not enough parameters given for macro '%s'
:MSGJTXT. �}�N��'%s'�ɗ^����ꂽ�p�����[�^���s�\���ł�
.np
You have not supplied enough parameters to the specified macro.
:MSGSYM. ERR_NOT_EXPECTING_RETURN_VALUE
:MSGTXT. Not expecting a return value for function '%s'
:MSGJTXT. �֐�'%s'�ɑ΂��Ė߂�l�͂Ȃ��͂��ł�
.np
The specified function is declared as a
.kw void
function.
Delete the
.kw return
statement, or change the type of the function.
:MSGSYM. ERR_EXPR_HAS_VOID_TYPE
:MSGTXT. Expression has void type
:MSGJTXT. ����void�^�ł�
.np
You tried to use the value of a
.kw void
expression inside another expression.
:MSGSYM. ERR_CANT_TAKE_ADDR_OF_BIT_FIELD
:MSGTXT. Cannot take the address of a bit field
:MSGJTXT. �r�b�g�t�B�[���h�̃A�h���X�͎��܂���
.np
The smallest addressable unit is a byte.
You cannot take the address of a bit field.
:MSGSYM. ERR_NOT_A_CONSTANT_EXPR
:MSGTXT. Expression must be constant
:MSGJTXT. ���͒萔�łȂ���΂Ȃ�܂���
.np
The compiler expects a constant expression.
This message can occur during static initialization if you are
trying to initialize a non-pointer type with an address expression.
:MSGSYM. ERR_CANT_OPEN_FILE
:MSGTXT. Unable to open '%s'
:MSGJTXT. '%s'���I�[�v�����邱�Ƃ��ł��܂���
.np
The file specified in an
.id #include
directive could not be located.
Make sure that the file name is spelled correctly, or that the
appropriate path for the file is included in the list of paths
specified in the
.id &incvarup
environment variable or the "-I" option on the command line.
:MSGSYM. ERR_TOO_MANY_MACRO_PARMS
:MSGTXT. Too many parameters given for macro '%s'
:MSGJTXT. �}�N��'%s'�ɗ^����ꂽ�p�����[�^���������܂�
.np
You have supplied too many parameters for the specified macro.
:MSGSYM. ERR_MODIFIERS_DISAGREE
:MSGTXT. Modifiers disagree with previous definition of '%s'
:MSGJTXT. �C���q��'%s'�̑O�̒�`�ƈ�v���܂���
.np
You have more than one definition or prototype for the variable or
function which have different type modifiers.
:MSGSYM. ERR_CANT_USE_TYPEDEF_AS_VAR
:MSGTXT. Cannot use typedef '%s' as a variable
:MSGJTXT. typedef'%s'�͕ϐ��Ƃ��Ďg���܂���
.np
The name of a typedef has been found when an operand or operator is expected.
If you are trying to use a type cast, make sure there are parentheses
around the type, otherwise check for a spelling mistake.
:MSGSYM. ERR_INV_STG_CLASS_FOR_GLOBAL
:MSGTXT. Invalid storage class for non-local variable
:MSGJTXT. ��Ǐ��ϐ��ɑ΂���s�K�؂ȋL���N���X�ł�
.np
A variable with module scope cannot be defined with the storage class of
.kw auto
or
.kw register.
:MSGSYM. ERR_INV_TYPE
:MSGTXT. Invalid type
:MSGJTXT. �s�K�؂Ȍ^�ł�
.np
An invalid combination of the following keywords has been specified in a
type declaration:
.kw const,
.kw volatile,
.kw signed,
.kw unsigned,
.kw char,
.kw int,
.kw short,
.kw long,
.kw float
and
.kw double.
:MSGSYM. ERR_EXPECTING_DECL_BUT_FOUND
:MSGTXT. Expecting data or function declaration, but found '%s'
:MSGJTXT. �f�[�^�܂��͊֐��̐錾������͂��ł����C'%s'������܂�
.np
The compiler is expecting the start of a data or function declaration.
If you are only part way through a function, then you have too many
closing braces "}".
:MSGSYM. ERR_INCONSISTENT_TYPE
:MSGTXT. Inconsistent return type for function '%s'
:MSGJTXT. �֐�'%s'�ɑ΂��閵�������߂�^�ł�
.np
Two prototypes for the same function disagree.
:MSGSYM. ERR_MISSING_OPERAND
:MSGTXT. Missing operand
:MSGJTXT. �I�y�����h������܂���
.np
An operand is required in the expression being parsed.
:MSGSYM. ERR_OUT_OF_MEMORY
:MSGTXT. Out of memory
:MSGJTXT. �������s���ł�
.np
The compiler has run out of memory to store information about the file
being compiled.
Try reducing the number of data declarations and or the size of the file
being compiled.
Do not
.id #include
header files that are not required.
.np
For the 16-bit C compiler,
the "-d2" option causes the compiler to use more memory.
Try compiling with the "-d1" option instead.
:MSGSYM. ERR_INV_CHAR_CONSTANT
:MSGTXT. Invalid character constant
:MSGJTXT. �s�K�؂ȕ����萔�ł�
.np
This message is issued for an improperly formed character constant.
:MSGSYM. ERR_CANT_USE_VOID
:MSGTXT. Cannot perform operation with pointer to void
:MSGJTXT. void�ւ̃|�C���^�ŉ��Z�����s���邱�Ƃ͂ł��܂���
.np
You cannot use a "pointer to void" with the operators +, &minus., ++,
&minus.&minus., += and &minus.=.
:MSGSYM. ERR_CANT_TAKE_ADDR_OF_REGISTER
:MSGTXT. Cannot take address of variable with storage class 'register'
:MSGJTXT. �L���N���X'register'�̕ϐ��̃A�h���X����邱�Ƃ͂ł��܂���
.np
If you want to take the address of a local variable, change the storage
class from
.kw register
to
.kw auto.
:MSGSYM. ERR_VAR_ALREADY_INITIALIZED
:MSGTXT. Variable '%s' already initialized
:MSGJTXT. �ϐ�'%s'�͊��ɏ���������܂���
.np
The specified variable has already been statically initialized.
:MSGSYM. ERR_MISSING_QUOTE
:MSGTXT. String literal not terminated before end of line
:MSGJTXT. ������萔�̍Ōオ������܂���
.np
A string literal is enclosed by double quote " characters.
.np
The compiler did not find a closing double quote " or line
continuation character \ before the end of a line or before
the end of the source file.
:MSGSYM. ERR_NEED_BRACES
:MSGTXT. Data for aggregate type must be enclosed in curly braces
:MSGJTXT. �W���̌^�̃f�[�^�́C�g���ʂɓ�����Ȃ���΂Ȃ�܂���
.np
When an array, struct or union is statically initialized, the data
must be enclosed in curly braces {}.
:MSGSYM. ERR_PARM_TYPE_MISMATCH
:MSGTXT. Type of parameter %d does not agree with previous definition
:MSGJTXT. �p�����[�^%d�̌^���O�̒�`�ƈ�v���܂���
.np
The type of the specified parameter is incompatible with the prototype
for that function.
The following example illustrates a problem that can arise when the
sequence of declarations is in the wrong order.
.errbad
/* Uncommenting the following line will
   eliminate the error */
/* struct foo; */

void fn1( struct foo * );

struct foo {
    int     a,b;
};

void fn1( struct foo *bar )
{
    fn2( bar );
}
.eerrbad
.pc
The problem can be corrected by reordering the sequence in which items
are declared (by moving the description of the structure
.id foo
ahead of its first reference or by adding the indicated statement).
This will assure that the first instance of structure
.id foo
is defined at the proper outer scope.
:MSGSYM. ERR_STG_CLASS_DISAGREES
:MSGTXT. Storage class disagrees with previous definition of '%s'
:MSGJTXT. �L���N���X��'%s'�̑O�̒�`�ƈ�v���܂���
.np
The previous definition of the specified variable has a storage class of
.kw static.
The current definition must have a storage class of
.kw static
or
.kw extern.
.np
Alternatively, a variable was previously declared as
.kw extern
and later defined as
.kw static.
:MSGSYM. ERR_INVALID_OPTION
:MSGTXT. Invalid option '%s'
:MSGJTXT. �I�v�V����'%s'�͕s�K�؂ł�
.np
The specified option is not recognized by the compiler.
:MSGSYM. ERR_INVALID_OPTIMIZATION
:MSGTXT. Invalid optimization option '%s'
:MSGJTXT. �s�K�؂ȍœK���I�v�V����'%s'�ł�
.np
The specified option is an unrecognized optimization option.
:MSGSYM. ERR_INVALID_MEMORY_MODEL
:MSGTXT. Invalid memory model '%s'
:MSGJTXT. �s�K�؂ȃ������[���f��'%s'�ł�
.np
Memory model option must be one of "ms", "mm", "mc", "ml", "mh" or
"mf" which selects the Small, Medium, Compact, Large, Huge or Flat
memory model.
:MSGSYM. ERR_MISSING_SEMICOLON
:MSGTXT. Missing semicolon at end of declaration
:MSGJTXT. �X���̏I���ɃZ�~�R����������܂���
.np
You are missing a semicolon ";" on the declaration just before the left
curly brace "{".
:MSGSYM. ERR_MISSING_RIGHT_BRACE
:MSGTXT. Missing '}'
:MSGJTXT. '}'������܂���
.np
The compiler detected end of file before finding a right curly brace "}" to
end the current function.
:MSGSYM. ERR_INVALID_TYPE_FOR_SWITCH
:MSGTXT. Invalid type for switch expression
:MSGJTXT. switch���ɑ΂���s�K�؂Ȍ^�ł�
.np
The type of a switch expression must be integral.
:MSGSYM. ERR_EXPR_MUST_BE_INTEGRAL
:MSGTXT. Expression must be integral
:MSGJTXT. ���͐����^�łȂ���΂Ȃ�܂���
.np
An integral expression is required.
:MSGSYM. ERR_EXPR_MUST_BE_ARITHMETIC
:MSGTXT. Expression must be arithmetic
:MSGJTXT. ���͎Z�p���łȂ���΂Ȃ�܂���
.np
Both operands of the "*", "/" and "%" operators must be arithmetic.
The operand of the unary minus must also be arithmetic.
:MSGSYM. ERR_EXPR_MUST_BE_SCALAR
:MSGTXT. Expression must be scalar type
:MSGJTXT. ���̓X�J���[�^�łȂ���΂Ȃ�܂���
.np
A scalar expression is required.
:MSGSYM. ERR_STMT_REQUIRED_AFTER_LABEL
:MSGTXT. Statement required after label
:MSGJTXT. ���x���̌�ɂ͕����K�v�ł�
.np
The C language definition requires a statement following a label.
You can use a null statement which consists of just a semicolon (";").
:MSGSYM. ERR_STMT_REQUIRED_AFTER_DO
:MSGTXT. Statement required after 'do'
:MSGJTXT. 'do'�̌�ɂ͕����K�v�ł�
.np
A statement is required between the
.kw do
and
.kw while
keywords.
:MSGSYM. ERR_STMT_REQUIRED_AFTER_CASE
:MSGTXT. Statement required after 'case'
:MSGJTXT. 'case'�̌�ɕ����K�v�ł�
.np
The C language definition requires a statement following a
.kw case
label.
You can use a null statement which consists of just a semicolon (";").
:MSGSYM. ERR_STMT_REQUIRED_AFTER_DEFAULT
:MSGTXT. Statement required after 'default'
:MSGJTXT. 'default'�̌�ɕ����K�v�ł�
.np
The C language definition requires a statement following a
.kw default
label.
You can use a null statement which consists of just a semicolon (";").
:MSGSYM. ERR_EXPR_TOO_COMPLICATED
:MSGTXT. Expression too complicated, split it up and try again
:MSGJTXT. �������܂�ɕ��G�ł��̂ŁC�������čēx�����Ă�������
.np
The expression contains too many levels of nested parentheses.
Divide the expression up into two or more sub-expressions.
:MSGSYM. ERR_MISSING_CENDIF
:MSGTXT. Missing matching #endif directive
:MSGJTXT. �Ή�����#endif�[�����߂�����܂���
.np
You are missing a
to terminate a
.id #if, #ifdef
or
.id #ifndef
preprocessing directive.
:MSGSYM. ERR_INVALID_MACRO_DEFN
:MSGTXT. Invalid macro definition, missing )
:MSGJTXT. �s�K�؂ȃ}�N����`�ł��C�j������܂���
.np
The right parenthesis ")" is required for a function-like macro definition.
:MSGSYM. ERR_INCOMPLETE_MACRO
:MSGTXT. Missing ) for expansion of '%s' macro
:MSGJTXT. '%s'�}�N���̓W�J�ɑ΂���j������܂���
.np
The compiler encountered end-of-file while collecting up the argument for a
function-like macro.
A right parenthesis ")" is required to mark the end of the argument(s) for
a function-like macro.
:MSGSYM. ERR_INVALID_CONVERSION
:MSGTXT. Invalid conversion
:MSGJTXT. �s�K�؂ȕϊ��ł�
.np
A
.kw struct
or
.kw union
cannot be converted to anything.
A
.kw float
or
.kw double
cannot be converted to a pointer and a pointer cannot be converted to a
.kw float
or
.kw double.
:MSGSYM. ERR_USER_ERROR_MSG
:MSGTXT. %s
:MSGJTXT. %s
.np
This is a user message generated with the
.id #error
preprocessing directive.
:MSGSYM. ERR_CANT_HAVE_AN_ARRAY_OF_FUNCTIONS
:MSGTXT. Cannot define an array of functions
:MSGJTXT. �֐��̔z��͒�`�ł��܂���
.np
You can have an array of pointers to functions, but not an array
of functions.
:MSGSYM. ERR_FUNCTION_CANT_RETURN_AN_ARRAY
:MSGTXT. Function cannot return an array
:MSGJTXT. �֐��͔z���Ԃ��܂���
.np
A function cannot return an array.
You can return a pointer to an array.
:MSGSYM. ERR_FUNCTION_CANT_RETURN_A_FUNCTION
:MSGTXT. Function cannot return a function
:MSGJTXT. �֐��͊֐���Ԃ��܂���
.np
You cannot return a function.
You can return a pointer to a function.
:MSGSYM. ERR_CANT_TAKE_ADDR_OF_LOCAL_VAR
:MSGTXT. Cannot take address of local variable in static initialization
:MSGJTXT. �X�^�e�B�b�N�ȏ������̒��ŋǏ��ϐ��̃A�h���X����邱�Ƃ͂ł��܂���
.np
You cannot take the address of an
.kw auto
variable at compile time.
:MSGSYM. ERR_INCONSISTENT_USE_OF_RETURN
:MSGTXT. Inconsistent use of return statements
:MSGJTXT. return���̎g�p���������Ă��܂�
.np
The compiler has found a
.kw return
statement which returns a value and a
.kw return
statement that does not return a value both in the same function.
The
.kw return
statement which does not return a value needs to have a
value specified to be consistent with the other
.kw return
statement in the function.
:MSGSYM. ERR_MISSING_QUESTION_OR_MISPLACED_COLON
:MSGTXT. Missing ? or misplaced :
:MSGJTXT. ?���Ȃ����C:�̈ʒu���Ԉ���Ă��܂�
.np
The compiler has detected a syntax error related to the "?" and ":"
operators.
You may need parenthesis around the expressions involved so that it can
be parsed correctly.
:MSGSYM. ERR_MAX_STRUCT_SIZE_IS_64K
:MSGTXT. Maximum struct or union size is 64K
:MSGJTXT. struct�����union�̑傫���͍ő�64 K�ł�
.np
The size of a
.kw struct
or
.kw union
is limited to 64K so that the compiler
can represent the offset of a member in a 16-bit register.
:MSGSYM. ERR_STMT_MUST_BE_INSIDE_FUNCTION
:MSGTXT. Statement must be inside function. Probable cause: missing {
:MSGJTXT. ���͊֐��̓����łȂ���΂Ȃ�܂���B�l�����錴���F{������܂���
.np
The compiler has detected a statement such as
.kw for,
.kw while,
.kw switch,
etc., which must be inside a function.
You either have too many closing braces "}" or you are missing an
opening brace "{" earlier in the function.
:MSGSYM. ERR_MACRO_DEFN_NOT_IDENTICAL
:MSGTXT. Definition of macro '%s' not identical to previous definition
:MSGJTXT. �}�N��'%s'�̒�`���O�̒�`�ƈ�v���܂���
.np
If a macro is defined more than once, the definitions must be identical.
If you want to redefine a macro to have a different definition, you must
.id #undef
it before you can define it with a new definition.
:MSGSYM. ERR_CANT_UNDEF_THESE_NAMES
:MSGTXT. Cannot #undef '%s'
:MSGJTXT. '%s'��#undef�ł��܂���
.np
The special macros
.id __LINE__, __FILE__, __DATE__, __TIME__, __STDC__, __FUNCTION__
and
.id __func__,
and the identifier "defined",
cannot be deleted by the
.id #undef
directive.
:MSGSYM. ERR_CANT_DEFINE_DEFINED
:MSGTXT. Cannot #define the name 'defined'
:MSGJTXT. ���O'defined'��#define�ł��܂���
.np
You cannot define a macro called
.id defined.
:MSGSYM. ERR_MISPLACED_SHARP_SHARP
:MSGTXT. ## must not be at start or end of replacement tokens
:MSGJTXT. ##�͒u�������g�[�N���̐擪�܂��͍Ō�ɂ����Ă͂Ȃ�܂���
.np
There must be a token on each side of the "##" (token pasting) operator.
:MSGSYM. ERR_NO_CAST_DURING_PP
:MSGTXT. Type cast not allowed in #if or #elif expression
:MSGJTXT. �^�L���X�g��#if���邢��#elif���̒��ł͋�����܂���
.np
A type cast is not allowed in a preprocessor expression.
:MSGSYM. ERR_NO_SIZEOF_DURING_PP
:MSGTXT. 'sizeof' not allowed in #if or #elif expression
:MSGJTXT. 'sizeof'��#if��#elif���̒��ŋ�����܂���
.np
The
.kw sizeof
operator is not allowed in a preprocessor expression.
:MSGSYM. ERR_INVALID_RELOP_FOR_STRUCT_OR_UNION
:MSGTXT. Cannot compare a struct or union
:MSGJTXT. struct���邢��union�͔�r�ł��܂���
.np
A
.kw struct
or
.kw union
cannot be compared with "==" or "!=".
You must compare each member of a
.kw struct
or
.kw union
to determine equality or inequality.
If the
.kw struct
or
.kw union
is packed (has no holes in it for alignment purposes)
then you can compare two structs using
.id memcmp.
:MSGSYM. ERR_EMPTY_ENUM_LIST
:MSGTXT. Enumerator list cannot be empty
:MSGJTXT. �񋓎q���X�g�͋�ł��邱�Ƃ��ł��܂���
.np
You must have at least one identifier in an
.kw enum
list.
:MSGSYM. ERR_INVALID_FLOATING_POINT_CONSTANT
:MSGTXT. Invalid floating-point constant
:MSGJTXT. �s�K�؂ȕ��������_�萔�ł�
.np
The exponent part of the floating-point constant is not formed correctly.
:MSGSYM. ERR_CANT_TAKE_SIZEOF_FIELD
:MSGTXT. Cannot take sizeof a bit field
:MSGJTXT. �r�b�g�t�B�[���h��sizeof���v�Z���邱�Ƃ͂ł��܂���
.np
The smallest object that you can ask for the size of is a char.
:MSGSYM. ERR_CANT_INITIALIZE_EXTERN_VAR
:MSGTXT. Cannot initialize variable with storage class of extern
:MSGJTXT. extern�̋L���N���X�̕ϐ��͏������ł��܂���
.np
A storage class of
.kw extern
is used to associate the variable with its actual definition somewhere
else in the program.
:MSGSYM. ERR_INVALID_STG_CLASS_FOR_PARM_PROTO
:MSGTXT. Invalid storage class for parameter
:MSGJTXT. �p�����[�^�ɑ΂��ĕs�K�؂ȋL���N���X�ł�
.np
The only storage class allowed for a parameter is
.kw register.
:MSGSYM. ERR_EMPTY_INITIALIZER_LIST
:MSGTXT. Initializer list cannot be empty
:MSGJTXT. ���������X�g�͋�ł��邱�Ƃ��ł��܂���
.np
An initializer list must have at least one item specified.
:MSGSYM. ERR_INCOMPLETE_EXPR_TYPE
:MSGTXT. Expression has incomplete type
:MSGJTXT. ���̌^���s���S�ł�
.np
An attempt has been made to access a struct or union whose definition is
not known, or an array whose dimensions are not known.
:MSGSYM. ERR_STRUCT_OR_UNION_INSIDE_ITSELF
:MSGTXT. Struct or union cannot contain itself
:MSGJTXT. Struct���邢��union�����ꎩ�g���܂ނ��Ƃ͂ł��܂���
.np
You cannot have a
.kw struct
or
.kw union
contain itself.
You can have a pointer in the
.kw struct
which points to an instance of itself.
Check for a missing "*" in the declaration.
:MSGSYM. ERR_INCOMPLETE_ENUM_DECL
:MSGTXT. Incomplete enum declaration
:MSGJTXT. �s���S��enum�錾�ł�
.np
The enumeration tag has not been previously defined.
:MSGSYM. ERR_ID_LIST_SHOULD_BE_EMPTY
:MSGTXT. An id list not allowed except for function definition
:MSGJTXT. �֐���`�������āCID���X�g�͋�����܂���
.np
A function prototype must contain type information.
:MSGSYM. ERR_MUST_BE_VAR_PARM_FUNC
:MSGTXT. Must use 'va_start' macro inside function with variable parameters
:MSGJTXT. 'va_start'�}�N���͉σp�����[�^�����֐��̓����Ŏg�p���Ȃ���΂Ȃ�܂���
.np
The
.id va_start
macro is used to setup access to the parameters in a function
that takes a variable number of parameters.
A function is defined with a variable number of parameters by declaring
the last parameter in the function as "...".
:MSGSYM. ERR_FATAL_ERROR
:MSGTXT. ***FATAL*** %s
:MSGJTXT. ***�v���I*** %s
.np
A fatal error has been detected during code generation time.
The type of error is displayed in the message.
:MSGSYM. ERR_BACK_END_ERROR
:MSGTXT. Internal compiler error %d
:MSGJTXT. �R���p�C�������G���[%d
.np
A bug has been encountered in the C compiler.
Please report the specified internal compiler error number and any other
helpful details about the program being compiled to compiler developers
so that we can fix the problem.
:MSGSYM. ERR_BAD_PARM_REGISTER
:MSGTXT. Parameter number %d - invalid register in #pragma
:MSGJTXT. �p�����[�^�ԍ� %d - #pragma�̒��̕s�K�؂ȃ��W�X�^
.np
The designated registers cannot hold the value for the parameter.
:MSGSYM. ERR_BAD_RETURN_REGISTER
:MSGTXT. Procedure '%s' has invalid return register in #pragma
:MSGJTXT. �v���V�[�W��'%s'��#pragma�̒��ŕs�K�؂ȃ��^�[���E���W�X�^�������Ă��܂�
.np
The size of the return register does not match the size of the result
returned by the function.
:MSGSYM. ERR_BAD_SAVE
:MSGTXT. Illegal register modified by '%s' #pragma
:MSGJTXT. ��@�ȃ��W�X�^��'%s'#pragma�ɂ���ďC������܂���
.np
.us For the 16-bit C compiler:
The BP, CS, DS, and SS registers cannot be modified in small data models.
The BP, CS, and SS registers cannot be modified in large data models.
.np
.us For the 32-bit C compiler:
The EBP, CS, DS, ES, and SS registers cannot be modified in flat
memory models.
The EBP, CS, DS, and SS registers cannot be modified in small data
models.
The EBP, CS, and SS registers cannot be modified in large data models.
:MSGSYM. ERR_NO_EXTERNAL_DEFNS_FOUND
:MSGTXT. File must contain at least one external definition
:MSGJTXT. �t�@�C���͏����Ƃ�1�̊O����`���܂܂Ȃ���΂Ȃ�܂���
.np
Every file must contain at least one global object, (either a data variable
or a function).
This message is only issued in strict ANSI mode (-za).
:MSGSYM. ERR_OUT_OF_MACRO_MEMORY
:MSGTXT. Out of macro space
:MSGJTXT. �}�N����Ԃ��s���ł�
.np
The compiler ran out of memory for storing macro definitions.
:MSGSYM. ERR_BREAK_KEY_HIT
:MSGTXT. Keyboard interrupt detected
:MSGJTXT. �L�[�{�[�h���荞�݂����o����܂���
.np
The compile has been aborted with Ctrl/C or Ctrl/Break.
:MSGSYM. ERR_INV_DATA_TYPE_FOR_REGISTER
:MSGTXT. Array, struct or union cannot be placed in a register
:MSGJTXT. �z��Cstruct�C���邢��union�����W�X�^�ɒu�����Ƃ͂ł��܂���
.np
Only scalar objects can be specified with the
.kw register
class.
:MSGSYM. ERR_TYPE_REQUIRED_IN_PARM_LIST
:MSGTXT. Type required in parameter list
:MSGJTXT. �^���p�����[�^�E���X�g�̒��ɕK�v�ł�
.np
If the first parameter in a function definition or prototype is defined
with a type, then all of the parameters must have a type specified.
:MSGSYM. ERR_ENUM_CONSTANT_OUT_OF_RANGE
:MSGTXT. Enum constant is out of range %s
:MSGJTXT. enum�萔��%s�͈̔͊O�ł�
.np
All of the constants must fit into appropriate value range.
:MSGSYM. ERR_TYPE_DOES_NOT_AGREE
:MSGTXT. Type does not agree with previous definition of '%s'
:MSGJTXT. �^��'%s'�̑O�̒�`�ƈ�v���܂���
.np
You have more than one definition of a variable or function that do not
agree.
:MSGSYM. ERR_DUPLICATE_FIELD_NAME
:MSGTXT. Duplicate name '%s' not allowed in struct or union
:MSGJTXT. struct���邢��union�̒��ŁC�������O'%s'���x�g�����Ƃ͂ł��܂���
.np
All the field names in a
.kw struct
or
.kw union
must be unique.
:MSGSYM. ERR_DUPLICATE_MACRO_PARM
:MSGTXT. Duplicate macro parameter '%s'
:MSGJTXT. �}�N���E�p�����[�^'%s'�̒�`����x�s���Ă��܂�
.np
The parameters specified in a macro definition must be unique.
:MSGSYM. ERR_UNABLE_TO_OPEN_WORK_FILE
:MSGTXT. Unable to open work file: error code = %d
:MSGJTXT. ���[�N�E�t�@�C�����I�[�v�����邱�Ƃ��ł��܂���F�G���[�R�[�h = %d
.np
The compiler tries to open a new work file by the name "__wrkN__.tmp" where
N is the digit 0 to 9.
This message will be issued if all of those files already exist.
:MSGSYM. ERR_WORK_FILE_WRITE_ERROR
:MSGTXT. Write error on work file: error code = %d
:MSGJTXT. ���[�N�E�t�@�C���̏����݃G���[�F�G���[�R�[�h = %d
.np
An error was encountered trying to write information to the work file.
The disk could be full.
:MSGSYM. ERR_WORK_FILE_READ_ERROR
:MSGTXT. Read error on work file: error code = %d
:MSGJTXT. ���[�N�E�t�@�C���̓ǎ��G���[�F�G���[�R�[�h = %d
.np
An error was encountered trying to read information from the work file.
:MSGSYM. ERR_WORK_FILE_SEEK_ERROR
:MSGTXT. Seek error on work file: error code = %d
:MSGJTXT. ���[�N�E�t�@�C���̃V�[�N�E�G���[�F�G���[�R�[�h = %d
.np
An error was encountered trying to seek to a position in the work file.
:MSGSYM. ERR_UNUSED_3
:MSGTXT. not used
:MSGJTXT. ���̃��b�Z�[�W�͎g�p����܂���
unused message
:MSGSYM. ERR_OUT_OF_ENUM_MEMORY
:MSGTXT. Out of enum space
:MSGJTXT. enum��Ԃ��s���ł�
.np
The compiler has run out of space allocated to store information on all
of the
.kw enum
constants defined in your program.
:MSGSYM. ERR_FILENAME_REQUIRED
:MSGTXT. Filename required on command line
:MSGJTXT. �R�}���h�E���C����Ƀt�@�C�������K�v�ł�
.np
The name of a file to be compiled must be specified on the command line.
:MSGSYM. ERR_CAN_ONLY_COMPILE_ONE_FILE
:MSGTXT. Command line contains more than one file to compile
:MSGJTXT. �R�}���h�E���C���ɃR���p�C������t�@�C����2�ȏ�w�肳��Ă��܂�
.np
You have more than one file name specified on the command line to be
compiled.
The compiler can only compile one file at a time.
You can use the &wclname. utility to compile multiple files with a
single command.
:MSGSYM. ERR_MISPLACED_LEAVE
:MSGTXT. _leave must appear in a _try statement
:MSGJTXT. _leave��_try���̒��ɂȂ���΂Ȃ�܂���
.np
The
.kw _leave
keyword must be inside a
.kw _try
statement.
The
.kw _leave
keyword causes the program to jump to the start of the
.kw _finally
block.
:MSGSYM. ERR_EXPECTING_END_OF_LINE_BUT_FOUND
:MSGTXT. Expecting end of line but found '%s'
:MSGJTXT. �s���ł���͂��ł����C'%s'������܂�
.np
A syntax error has been detected.
The token displayed in the message should help you determine the problem.
:MSGSYM. ERR_TOO_MANY_BYTES_IN_PRAGMA
:MSGTXT. Too many bytes specified in #pragma
:MSGJTXT. #pragma�̒��Ŏw�肳�ꂽ�o�C�g�E�R�[�h���������܂�
.np
There is an internal limit on the number of bytes for in-line code
that can be specified with a pragma.
Try splitting the function into two or more smaller functions.
:MSGSYM. ERR_BAD_LINKAGE
:MSGTXT. Cannot resolve linkage conventions for routine '%s' #pragma
:MSGJTXT. ���[�`��'%s'#pragma�ɑ΂��ă����P�[�W�K��������ł��܂���
.np
The compiler cannot generate correct code for the specified routine
because of register conflicts.
Change the registers used by the parameters of the pragma.
:MSGSYM. ERR_SYM_MUST_BE_GLOBAL
:MSGTXT. Symbol '%s' in pragma must be global
:MSGJTXT. �v���O�}�̒��̃V���{��'%s'�̓O���[�o���łȂ���΂Ȃ�܂���
.np
The in-line code for a pragma can only reference a global variable or
function.
You can only reference a parameter or local variable by passing it as
a parameter to the in-line code pragma.
:MSGSYM. ERR_INTERNAL_LIMIT_EXCEEDED
:MSGTXT. Internal compiler limit exceeded, break module into smaller pieces
:MSGJTXT. �R���p�C���̓����������z���܂����B���W���[������菬���������ɕ������Ă�������
.np
The compiler can handle 65535 quadruples, 65535 leaves, and 65535 symbol
table entries and literal strings.
If you exceed one of these limits, the program must be broken into smaller
pieces until it is capable of being processed by the compiler.
:MSGSYM. ERR_INVALID_INITIALIZER
:MSGTXT. Invalid initializer for integer data type
:MSGJTXT. �����f�[�^�^�ɑ΂��ĕs�K�؂ȏ������ł�
.np
Integer data types (int and long) can be initialized with
numeric expressions or address expressions that are the same size
as the integer data type being initialized.
:MSGSYM. ERR_TOO_MANY_ERRORS
:MSGTXT. Too many errors: compilation aborted
:MSGJTXT. �G���[���������܂��F�R���p�C���𒆎~���܂�
.np
The compiler stops compiling when the number of errors generated
exceeds the error limit.
The error limit can be set with the "-e" option.
The default error limit is 20.
:MSGSYM. ERR_EXPECTING_IDENTIFIER_BUT_FOUND
:MSGTXT. Expecting identifier but found '%s'
:MSGJTXT. ���ʎq������͂��ł����C'%s'������܂���
.np
A syntax error has been detected.
The token displayed in the message should help you determine the problem.
:MSGSYM. ERR_EXPECTING_CONSTANT_BUT_FOUND
:MSGTXT. Expecting constant but found '%s'
:MSGJTXT. �萔������͂��ł����C'%s'������܂���
.np
The #line directive must be followed by a constant indicating the
desired line number.
:MSGSYM. ERR_EXPECTING_STRING_BUT_FOUND
:MSGTXT. Expecting \"filename\" but found '%s'
:MSGJTXT. \"�t�@�C����\"������͂��ł����C'%s'������܂���
.np
The second argument of the #line directive must be a filename
enclosed in quotes.
:MSGSYM. ERR_PARM_COUNT_MISMATCH
:MSGTXT. Parameter count does not agree with previous definition
:MSGJTXT. �p�����[�^�����O�̒�`�ƈ�v���܂���
.np
You have either not enough parameters or too many parameters in a call
to a function.
If the function is supposed to have a variable number of parameters, then
you are missing the ", ..." in the function prototype.
:MSGSYM. ERR_SEGMENT_NAME_REQUIRED
:MSGTXT. Segment name required
:MSGJTXT. �Z�O�����g�����K�v�ł�
.np
A segment name must be supplied in the form of a literal string
to the __segname() directive.
:MSGSYM. ERR_INVALID_BASED_DECLARATOR
:MSGTXT. Invalid __based declaration
:MSGJTXT. �s�K�؂�__based�錾�ł�
.np
The compiler could not recognize one of the allowable forms of __based
declarations.
See the
.us C Language Reference
document for description of all the allowable forms of __based declarations.
:MSGSYM. ERR_SYM_MUST_BE_TYPE_SEGMENT
:MSGTXT. Variable for __based declaration must be of type __segment or pointer
:MSGJTXT. __based�錾�ɑ΂���ϐ��͌^__segment�łȂ���΂Ȃ�܂���
.np
A based pointer declaration must be based on a simple variable of type
__segment or pointer.
:MSGSYM. ERR_DUPLICATE_ID
:MSGTXT. Duplicate external symbol %s
:MSGJTXT. �O���V���{��%s��2����܂�
.np
Duplicate external symbols will exist when the specified symbol name is
truncated to 8 characters.
:MSGSYM. ERR_ASSEMBLER_ERROR
:MSGTXT. Assembler error: '%s'
:MSGJTXT. �A�Z���u���E�G���[�F'%s'
.np
An error has been detected by the in-line assembler.
The message indicates the error detected.
:MSGSYM. ERR_VAR_TOO_LARGE
:MSGTXT. Variable must be 'huge'
:MSGJTXT. �ϐ���'huge'�łȂ���΂Ȃ�܂���
.np
A variable or an array that requires more than 64K of storage in the 16-bit
compiler must be declared as
.kw huge.
:MSGSYM. ERR_TOO_MANY_PARM_SETS
:MSGTXT. Too many parm sets
:MSGJTXT. parm�̎w�肪�������܂�
.np
Too many parameter register sets have been specified in the pragma.
:MSGSYM. ERR_IO_ERR
:MSGTXT. I/O error reading '%s': %s
:MSGJTXT. '%s'�ǂݍ��ݒ��̂h�^�n�G���[�F%s
.np
An I/O error has been detected by the compiler while reading the source file.
The system dependent reason is also displayed in the message.
:MSGSYM. ERR_NO_SEG_REGS
:MSGTXT. Attempt to access far memory with all segment registers disabled in '%s'
:MSGJTXT. '%s'�̒��Ŏg�p�֎~�ɂ��ꂽ���ׂẴZ�O�����g�E���W�X�^��far�������ɃA�N�Z�X���悤�Ƃ��Ă��܂�
.np
The compiler does not have any segment registers available to access the
desired far memory location.
:MSGSYM. ERR_NO_MACRO_ID_COMMAND_LINE
:MSGTXT. No identifier provided for '-D' option
:MSGJTXT. /D�I�v�V�����ɑ΂��鎯�ʎq������܂���
.np
The command line option "-D" must be followed by the name of the macro
to be defined.
:MSGSYM. ERR_BAD_PEG_REG
:MSGTXT. Invalid register pegged to a segment in '%s'
:MSGJTXT. �s�K�؂ȃ��W�X�^��'%s'�̒��ŃZ�O�����g�ɌŒ肳��Ă��܂�
.np
The register specified in a #pragma data_seg, or a
.kw __segname
expression must be a valid segment register.
:MSGSYM. ERR_INVALID_OCTAL_CONSTANT
:MSGTXT. Invalid octal constant
:MSGJTXT. �s�K�؂ȂW�i�萔�ł�
.np
An octal constant cannot contain the digits 8 or 9.
:MSGSYM. ERR_INVALID_HEX_CONSTANT
:MSGTXT. Invalid hexadecimal constant
:MSGJTXT. �s�K�؂ȂP�U�i�萔�ł�
.np
The token sequence "0x" must be followed by a hexadecimal character
(0-9, a-f, or A-F).
:MSGSYM. ERR_UNEXPECTED_RIGHT_PAREN
:MSGTXT. Unexpected ')'. Probable cause: missing '('
:MSGJTXT. �Ԉ�����ꏊ��')'������܂��B�l�����錴���F'('������܂���
.np
A closing parenthesis was found in an expression without a corresponding
opening parenthesis.
:MSGSYM. ERR_UNREACHABLE_SYM
:MSGTXT. Symbol '%s' is unreachable from #pragma
:MSGJTXT. �V���{��'%s'��#pragma����͂��܂���
.np
The in-line assembler found a jump instruction to a label that is
too far away.
:MSGSYM. ERR_CONST_DIV_ZERO
:MSGTXT. Division or remainder by zero in a constant expression
:MSGJTXT. �萔���̒��Ƀ[���ɂ�鏜�Z����]������܂�
.np
The compiler found a constant expression containing a division or
remainder by zero.
:MSGSYM. ERR_INVALID_STRING_LITERAL
:MSGTXT. Cannot end string literal with backslash
:MSGJTXT. ������萔�̓o�b�N�X���b�V���ŏI�����邱�Ƃ͂ł��܂���
.np
The argument to a macro that uses the stringize operator '#'
on that argument must not end in a backslash character.
.errbad
#define str(x) #x
str(@#\)
.eerrbad
:MSGSYM. ERR_INVALID_DECLSPEC
:MSGTXT. Invalid __declspec declaration
:MSGJTXT. �s�K�؂�__declspec�錾�ł�
.np
The only valid __declspec declarations are "__declspec(thread)",
"__declspec(dllexport)",
and "__declspec(dllimport)".
:MSGSYM. ERR_TOO_MANY_STORAGE_CLASS_SPECIFIERS
:MSGTXT. Too many storage class specifiers
:MSGJTXT. �L���N���X�w��q���������܂�
.np
You can only specify one storage class specifier in a declaration.
:MSGSYM. ERR_EXPECTING_BUT_FOUND_END_OF_FILE
:MSGTXT. Expecting '%s' but found end of file
:MSGJTXT. '%s'������͂��ł����C�t�@�C�����I�����Ă��܂��܂���
.np
A syntax error has been detected.
The compiler is still expecting more input when it reached the
end of the source program.
:MSGSYM. ERR_EXPECTING_STRUCT_UNION_TAG_BUT_FOUND
:MSGTXT. Expecting struct/union tag but found '%s'
:MSGJTXT. �\����/���p�̃^�O������͂��ł����C'%s'������܂���
.np
The compiler expected to find an identifier following the
.kw struct
or
.kw union
keyword.
:MSGSYM. ERR_OPND_OF_BUILTIN_ISFLOAT_MUST_BE_TYPE
:MSGTXT. Operand of __builtin_isfloat() must be a type
:MSGJTXT. __builtin_isfloat()�̃I�y�����h�͌^�łȂ���΂Ȃ�܂���
.np
The __builtin_isfloat() function is used by the
.kw va_arg
macro to determine if a type is a floating-point type.
:MSGSYM. ERR_INVALID_CONSTANT
:MSGTXT. Invalid constant
:MSGJTXT. �s�K�؂Ȓ萔�ł�
.np
The token sequence does not represent a valid numeric constant.
:MSGSYM. ERR_TOO_MANY_INITS
:MSGTXT. Too many initializers
:MSGJTXT. �������w��q���������܂�
.np
There are more initializers than objects to initialize.
For example  int X[2] = { 0, 1, 2 };
The variable "X" requires two initializers not three.
:MSGSYM. ERR_PARM_POINTER_TYPE_MISMATCH
:MSGTXT. Parameter %d, pointer type mismatch
:MSGJTXT. �p�����[�^%d, �|�C���^�̌^���Ⴂ�܂�
.np
You have two pointers that either point to different objects, or the
pointers are of different size, or they have different modifiers.
:MSGSYM. ERR_REPEATED_MODIFIER
:MSGTXT. Modifier repeated in declaration
:MSGJTXT. �C���q���錾�̒��ŌJ��Ԃ��g�p����Ă��܂�
.np
You have repeated the use of a modifier like "const" (an error)
or "far" (a warning) in a declaration.
:MSGSYM. ERR_QUALIFIER_MISMATCH
:MSGTXT. Type qualifier mismatch
:MSGJTXT. �^�C���q����v���܂���
.np
You have two pointers that have different "const" or "volatile" qualifiers.
:MSGSYM. ERR_PARM_QUALIFIER_MISMATCH
:MSGTXT. Parameter %d, type qualifier mismatch
:MSGJTXT. �p�����[�^%d, �^�C���q����v���܂���
.np
You have two pointers that have different const or "volatile" qualifiers.
:MSGSYM. ERR_SIGN_MISMATCH
:MSGTXT. Sign specifier mismatch
:MSGJTXT. �����w��q����v���܂���
.np
You have two pointers that point to types that have different sign specifiers.
:MSGSYM. ERR_PARM_SIGN_MISMATCH
:MSGTXT. Parameter %d, sign specifier mismatch
:MSGJTXT. �p�����[�^%d, �����w��q����v���܂���
.np
You have two pointers that point to types that have different sign specifiers.
:MSGSYM. ERR_MISSING_LINE_CONTINUE
:cmt This message not currently used.  25-June-2006
:MSGTXT. Missing \\ for string literal
:MSGJTXT. ������萔�ɑ΂��� \\ ������܂���
.np
You need a '\' to continue a string literal across a line.
:MSGSYM. ERR_EXPECTING_AFTER_BUT_FOUND
:MSGTXT. Expecting '%s' after '%s' but found '%s'
:MSGJTXT. '%s'��'%s'�̌�ɂ���͂��ł����C'%s'������܂�
.np
A syntax error has been detected.
The tokens displayed in the message should help you to determine the problem.
:MSGSYM. ERR_EXPECTING_AFTER_BUT_FOUND_END_OF_FILE
:MSGTXT. Expecting '%s' after '%s' but found end of file
:MSGJTXT. '%s'��'%s'�̌�ɂ���͂��ł����C�t�@�C�����I�����Ă��܂��܂���
.np
A syntax error has been detected.
The compiler is still expecting more input when it reached the
end of the source program.
:MSGSYM. ERR_BAD_REGISTER_NAME
:MSGTXT. Invalid register name '%s' in #pragma
:MSGJTXT. #pragma�̒��ɖ����ȃ��W�X�^��'%s'������܂�
.np
The register name is invalid/unknown.
:MSGSYM. ERR_INVALID_STG_CLASS_FOR_LOOP_DECL
:MSGTXT. Storage class of 'for' statement declaration not register or auto
:MSGJTXT. 'for'�X�e�[�g�����g�L���N���X�����W�X�^�܂���auto�ł���܂���
.np
The only storage class allowed for the optional declaration part of a
.kw for
statement is
.kw auto
or
.kw register.
:MSGSYM. ERR_NO_TYPE_IN_DECL
:MSGTXT. No type specified in declaration
:MSGJTXT. �錾���Ō^���w�肳��Ă��܂���
.np
A declaration specifier must include a type specifier.
.errbad
auto i;
.eerrbad
:MSGSYM. ERR_DECL_IN_LOOP_NOT_OBJECT
:MSGTXT. Symbol '%s' declared in 'for' statement must be object
:MSGJTXT. 'for'�X�e�[�g�����g���Ő錾���ꂽ�V���{��'%s'�̓I�u�W�F�N�g�łȂ���΂Ȃ�܂���
.np
Any identifier declared in the optional declaration part of a
.kw for
statement must denote an object. Functions, structures, or enumerations may
not be declared in this context.
.errbad
for( int i = 0, j( void ); i < 5; ++i ) {
    ...
}
.eerrbad
:MSGSYM. ERR_UNEXPECTED_DECLARATION
:MSGTXT. Unexpected declaration
:MSGJTXT. �\�����Ȃ��錾�ł�
.np
Within a function body, in C99 mode a declaration is only allowed in
a compound statement and in the opening clause of a
.kw for
loop.
Declarations are not allowed after
.kw if,
.kw while,
or
.kw switch
statement, etc.
.errbad
void foo( int a )
{
    if( a > 0 )
        int j = 3;
}
.eerrbad
.np
In C89 mode, declarations within a function body are only allowed at
the beginning of a compound statement.
.errbad
void foo( int a )
{
    ++a;
    int j = 3;
}
.eerrbad
:eMSGGRP. Errs
:cmt -------------------------------------------------------------------
:MSGGRP. Info
:MSGGRPSTR. I
:MSGGRPNUM. 2000
:MSGGRPTXT. Informational Messages
:cmt -------------------------------------------------------------------
:MSGSYM. INFO_NOT_ENOUGH_MEMORY_TO_FULLY_OPTIMIZE
:MSGTXT. Not enough memory to fully optimize procedure '%s'
:MSGJTXT. �v���V�[�W��'%s'���œK������̂ɁC���������s�\���ł�
:INFO.
.np
The compiler did not have enough memory to fully optimize the
specified procedure.
The code generated will still be correct and execute properly.
This message is purely informational.
:MSGSYM. INFO_NOT_ENOUGH_MEMORY_TO_MAINTAIN_PEEPHOLE
:MSGTXT. Not enough memory to maintain full peephole
:MSGJTXT. ���S�ȃs�[�v�z�[�����ێ�����̂ɁC���������s�\���ł�
:INFO.
.np
Certain optimizations benefit from being able to store the entire module
in memory during optimization.
All functions will be individually optimized but the optimizer will
not be able to share code between functions if this message appears.
The code generated will still be correct and execute properly.
This message is purely informational.
It is only printed if the warning level is greater than or equal to 4.
.np
The main reason for this message is for those people who are concerned
about reproducing the exact same object code when the same source file
is compiled on a different machine.
You may not be able to reproduce the exact same object code from one
compile to the next unless the available memory is exactly the same.
:MSGSYM. INFO_SYMBOL_DECLARATION
:MSGTXT. '%s' defined in: %s(%u)
:MSGJTXT. '%s' ��%s(%u)�Œ�`����Ă��܂�
:INFO.
This informational message indicates where the symbol in question was defined.
The message is displayed following an error or warning diagnostic for the
symbol in question.
:errbad.
static int a = 9;
int b = 89;
:eerrbad.
The variable 'a' is not referenced in the preceding example and so will
cause a warning to be generated.
Following the warning, the informational message indicates the line at
which 'a' was declared.
:MSGSYM. INFO_SRC_CNV_TYPE
:MSGTXT. source conversion type is '%s'
:MSGJTXT. �ϊ��\�[�X�̌^��'%s'�ł�
:INFO.
This informational message indicates the type of the source operand, for the
preceding conversion diagnostic.
:MSGSYM. INFO_TGT_CNV_TYPE
:MSGTXT. target conversion type is '%s'
:MSGJTXT. �ϊ��^�[�Q�b�g�̌^��'%s'�ł�
:INFO.
This informational message indicates the target type of the conversion,
for the preceding conversion diagnostic.
:MSGSYM. INFO_INCLUDING_FILE
:MSGTXT. Including file '%s'
:MSGJTXT. �t�@�C��'%s'���C���N���[�h���܂�
:INFO.
This informational message indicates that the specified file was opened
as a result of
.id #include
directive processing.
:eMSGGRP. Info
:cmt -------------------------------------------------------------------
:MSGGRP. PCHDR
:MSGGRPSTR. H
:MSGGRPNUM. 3000
:MSGGRPTXT. Pre-compiled Header Messages
:cmt -------------------------------------------------------------------
:MSGSYM. PCHDR_READ_ERROR
:MSGTXT. Error reading PCH file
:MSGJTXT. PCH�t�@�C���̓ǂݍ��݃G���[
:INFO.
.np
The pre-compiled header file does not follow the correct format.
:MSGSYM. PCHDR_INVALID_HEADER
:MSGTXT. PCH file header is out of date
:MSGJTXT. PCH�t�@�C���̃o�[�W�������Ⴂ�܂�
:INFO.
.np
The pre-compiled header file is out of date with the compiler.
The current version of the compiler is expecting a different format.
:MSGSYM. PCHDR_DIFFERENT_OPTIONS
:MSGTXT. Compile options differ with PCH file
:MSGJTXT. �R���p�C���I�v�V������PCH�t�@�C���̓��e�ƈقȂ�܂�
:INFO.
.np
The command line options are not the same as used when
making the pre-compiled header file.
This can effect the values of the pre-compiled information.
:MSGSYM. PCHDR_DIFFERENT_CWD
:MSGTXT. Current working directory differs with PCH file
:MSGJTXT. ���݂̍�ƃf�B���N�g����PCH�̓��e�ƈقȂ�܂�
:INFO.
.np
The pre-compiled header file was compiled in a different directory.
:MSGSYM. PCHDR_INCFILE_CHANGED
:MSGTXT. Include file '%s' has been modified since PCH file was made
:MSGJTXT. PCH�t�@�C��������Ă���C�C���N���[�h�t�@�C��'%s'���C������܂���
:INFO.
.np
The include files have been modified since the pre-compiled header
file was made.
:MSGSYM. PCHDR_INCFILE_DIFFERENT
:MSGTXT. PCH file was made from a different include file
:MSGJTXT. PCH�t�@�C�����قȂ�C���N���[�h�t�@�C���������܂���
:INFO.
.np
The pre-compiled header file was made using a different include file.
:MSGSYM. PCHDR_INCPATH_CHANGED
:MSGTXT. Include path differs with PCH file
:MSGJTXT. �C���N���[�h�p�X��PCH�̓��e�ƈႢ�܂�
:INFO.
.np
The include paths have changed.
:MSGSYM. PCHDR_MACRO_CHANGED
:MSGTXT. Preprocessor macro definition differs with PCH file
:MSGJTXT. �v���v���Z�b�T�}�N���̒�`��PCH�t�@�C���ƈقȂ�܂�
:INFO.
.np
The definition of a preprocessor macro has changed.
:MSGSYM. PCHDR_NO_OBJECT
:MSGTXT. PCH cannot have data or code definitions.
:MSGJTXT. PCH�̓f�[�^�܂��̓R�[�h�̒�`���܂߂܂���
:INFO.
.np
The include files used to build the pre-compiled header
contain function or data  definitions.
This is not currently supported.
:eMSGGRP. PCHDR
:cmt -------------------------------------------------------------------
:MSGGRP. Misc
:MSGGRPSTR. M
:MSGGRPNUM. 4000
:MSGGRPTXT. Miscellaneous Messages and Phrases
:cmt -------------------------------------------------------------------
:MSGSYM. PHRASE_CODE_SIZE
:MSGTXT. Code size
:MSGJTXT. �R�[�h�T�C�Y
:INFO.
.np
String used in message construction.
:MSGSYM. PHRASE_ERROR
:MSGTXT. Error!
:MSGJTXT. �G���[�I
:INFO.
.np
String used in message construction.
:MSGSYM. PHRASE_WARNING
:MSGTXT. Warning!
:MSGJTXT. �x���I
:INFO.
.np
String used in message construction.
:MSGSYM. PHRASE_NOTE
:MSGTXT. Note!
:MSGJTXT. ���
:INFO.
.np
String used in message construction.
:MSGSYM. PHRASE_PRESS_RETURN
:MSGTXT. (Press return to continue)
:MSGJTXT. (���^�[���������ƁC���s���܂�)
:INFO.
.np
String used in message construction.
:eMSGGRP. Misc
