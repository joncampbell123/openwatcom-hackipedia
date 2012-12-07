:cmt ***************************************************************************
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
:cmt * Description:  C++ compiler diagnostic messages.
:cmt *
:cmt ***************************************************************************
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

:MSGSYM. ERR_CALL_WATCOM
:MSGTXT. internal compiler error
:MSGJTXT. �����̃R���p�C���E�G���[
If this message appears, please report the problem directly to the
Open Watcom development team. See http://www.openwatcom.org/.

:MSGSYM. WARN_ASSIGN_CONST_IN_BOOL_EXPR
:MSGTXT. assignment of constant found in boolean expression
:MSGJTXT. �萔�̑�����_�����̒��ɂ���܂�
:WARNING. 3
An assignment of a constant has been detected in a boolean expression.
For example: "if( var = 0 )".
It is most likely that you want to use "==" for testing for equality.

:MSGSYM. WARN_CONSTANT_TOO_BIG
:MSGTXT. constant out of range; truncated
:MSGJTXT. �萔���͈͊O�ł��G�؂�l�߂��܂�
:WARNING. 1
This message is issued if a constant cannot be represented in 32 bits
or if a constant is outside the range of valid values
that can be assigned to a variable.
:errbad.
int a = 12345678901234567890;
:eerrbad.

:MSGSYM. ERR_MISSING_RETURN_VALUE
:MSGTXT. missing return value
:MSGJTXT. �߂�l������܂���
A function has been declared with a non-void return type, but no
.kw return
statement was found in the function.  Either add a
.kw return
statement or change the function return type to
.kw void.
:errbad.
int foo( int a )
{
    int b = a + a;
}
:eerrbad.
The message will be issued at the end of the function.

:MSGSYM. WARN_BASE_CLASS_HAS_NO_VDTOR
:MSGTXT. base class '%T' does not have a virtual destructor
:MSGJTXT. ���N���X'%T'�͉��z�f�X�g���N�^�������܂���
:WARNING. 1
A virtual destructor has been declared in a class with base classes.
However, one of those base classes does not have a virtual
destructor. A
.kw delete
of a pointer cast to such a base class will not function properly
in all circumstances.
:errbad.
struct Base {
    ~Base();
};
struct Derived : Base {
    virtual ~Derived();
};
:eerrbad.
It is considered good programming practice to declare virtual
destructors in all classes used as base classes of classes having
virtual destructors.

:MSGSYM. WARN_POINTER_TRUNCATION
:MSGTXT. pointer or reference truncated
:MSGJTXT. �|�C���^���邢�͎Q�Ƃ��؂�l�߂��܂�
:WARNING. 1
The expression contains a transfer of a pointer value to another
pointer value of smaller size.  This can be caused by
.kw __near
or
.kw __far
qualifiers (i.e., assigning a
.kw far
pointer to a
.kw near
pointer).
Function pointers can also have a different size than data pointers
in certain memory models.
This message indicates that some information is being lost so check the
code carefully.
:errbad.
extern int __far *foo();
int __far *p_far = foo();
int __near *p_near = p_far; // truncated
:eerrbad.

:MSGSYM. ERR_SYNTAX_MISSING_SEMICOLON
:MSGTXT. syntax error; probable cause: missing ';'
:MSGJTXT. �\���G���[�G ';'���Ȃ��\��������܂�
The compiler has found a complete expression (or declaration) during
parsing but could not continue.  The compiler has detected that it could
have continued if a semicolon was present so there may be a semicolon
missing.
:errbad.
enum S {
}   // missing ';'

class X {
};
:eerrbad.

:MSGSYM. WARN_ADDR_OF_ARRAY
:MSGTXT. '&array' may not produce intended result
:MSGJTXT. '&array'�͈Ӑ}���ꂽ���ʂ𐶂��Ȃ���������܂���
:WARNING. 3
The type of the expression '&array' is different from the type of the
expression 'array'.
Suppose we have the declaration
.id char buffer[80].
Then the expression
.id (&buffer + 3)
will be evaluated as
.id (buffer + 3 * sizeof(buffer))
which is
.id (buffer + 3 * 80)
and not
.id (buffer + 3 * 1)
which is what one may have expected.
The address-of operator '&' is not required for getting the address of an
array.

:MSGSYM. WARN_RET_ADDR_OF_AUTO
:MSGTXT. returning address of function argument or of auto or register variable
:MSGJTXT. �֐������܂��͎����^���W�X�^�ϐ��̃A�h���X��Ԃ��Ă��܂�
:WARNING. 1
This warning usually indicates a serious programming error.
When a function exits, the storage allocated on the stack for
auto variables is released.
This storage will be overwritten by further function calls
and/or hardware interrupt service routines.
Therefore, the data pointed to by the return value may be destroyed
before your program has a chance to reference it or make a copy of it.
:errbad.
int *foo()
{
    int k = 123;
    return &k;      // k is automatic variable
}
:eerrbad.

:MSGSYM. ERR_INVALID_OPTION_FILE
:MSGTXT. option requires a file name
:MSGJTXT. �I�v�V�����̓t�@�C������K�v�Ƃ��܂�
The specified option is not recognized by the compiler
since there was no file name after it (i.e., "-fo=my.obj" ).

:MSGSYM. WARN_ASM_IGNORED
:MSGTXT. asm directive ignored
:MSGJTXT. asm�[�����߂���������܂���
:WARNING. 1
The asm directive (e.g., asm( "mov r0,1" ); ) is a non-portable construct.
The Open Watcom C++ compiler treats all asm directives like comments.

:MSGSYM. WARN_ALL_PRIVATE_IN_CLASS
:MSGTXT. all members are private
:MSGJTXT. ���ׂẴ����o�̓v���C�x�[�g�ł�
:WARNING. 3
This message warns the programmer that there will be no way to use the
contents of the class because all accesses will be flagged as erroneous
(i.e., accessing a private member).
:errbad.
class Private {
    int a;
    Private();
    ~Private();
    Private( const Private& );
};
:eerrbad.

:MSGSYM. ERR_INVALID_TEMPLATE_ARG_TYPE
:MSGTXT. template argument cannot be type '%T'
:MSGJTXT. �e���v���[�g������'%T'�^�ł��邱�Ƃ��ł��܂���
A template argument can be either a generic type (e.g.,
.id template < class
T
.id >
), a pointer, or an integral type.
These types are required for expressions that can be checked at compile time.

:MSGSYM. WARN_DEAD_CODE
:MSGTXT. unreachable code
:MSGJTXT. �R�[�h�͎��s����܂���
:WARNING. 2
The indicated statement will never be executed because there is no path
through the program that causes control to reach that statement.
:errbad.
void foo( int *p )
{
    *p = 4;
    return;
    *p = 6;
}
:eerrbad.
The statement following the
.kw return
statement cannot be reached.

:MSGSYM. WARN_SYM_NOT_REFERENCED
:MSGTXT. no reference to symbol '%S'
:MSGJTXT. �V���{��'%S'�ւ̎Q�Ƃ�����܂���
:WARNING. 2
There are no references to the declared variable.
The declaration for the variable can be deleted.
If the variable is a parameter to a function, all calls to the function
must also have the value for that parameter deleted.
.np
In some cases, there may be a valid reason for retaining the variable.
You can prevent the message from being issued through use of
.us #pragma off(unreferenced),
or adding a statement that assigns the variable to itself.

:MSGSYM. WARN_NESTED_COMMENT
:MSGTXT. nested comment found in comment started on line %u
:MSGJTXT. %u�s����n�҂����R�����g�̒��Ƀl�X�g�ɂ��ꂽ�R�����g������܂�
:WARNING. 3
While scanning a comment for its end, the compiler detected
.id /*
for the start of another comment.
Nested comments are not allowed in ISO/ANSI C.
You may be missing the
.id */
for the previous comment.

:MSGSYM. ERR_TEMPLATE_MUST_HAVE_ARGS
:MSGTXT. template argument list cannot be empty
:MSGJTXT. �e���v���[�g�������X�g�́C��ł��邱�Ƃ��ł��܂���
An empty template argument list would result in a template that
could only define a single class or function.

:MSGSYM. WARN_UNREFERENCED_LABEL
:MSGTXT. label '%s' has not been referenced by a goto
:MSGJTXT. ���x��'%s'��goto�ɂ���ĎQ�Ƃł��܂���
:WARNING. 3
The indicated label has not been referenced and, as such, is useless.
This warning can be safely ignored.
:errgood.
int foo( int a, int b )
{
un_refed:
    return a + b;
}
:eerrgood.

:MSGSYM. WARN_ANON_NOT_REFERENCED
:MSGTXT. no reference to anonymous union member '%S'
:MSGJTXT. ���O�����̋��p�̂̃����o�['%S'�ւ̎Q�Ƃ�����܂���
:WARNING. 2
The declaration for the anonymous member can be safely deleted without
any effect.

:MSGSYM. ERR_MISPLACED_BREAK
:MSGTXT. 'break' may only appear in a for, do, while, or switch statement
:MSGJTXT. 'break'��for, do, switch���̒��ł̂ݎg���܂�
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
:errbad.
int foo( int a, int b )
{
    break;  // illegal
    return a+b;
}
:eerrbad.

:MSGSYM. ERR_MISPLACED_CASE
:MSGTXT. 'case' may only appear in a switch statement
:MSGJTXT. 'case'��switch���ł̂ݎg���܂�
A
.kw case
label has been found that is not inside a
.kw switch
statement.
:errbad.
int foo( int a, int b )
{
    case 4:    // illegal
    return a+b;
}
:eerrbad.

:MSGSYM. ERR_MISPLACED_CONTINUE
:MSGTXT. 'continue' may only appear in a for, do, or while statement
:MSGJTXT. 'continue'��for, do, while���ł̂ݎg���܂�
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
:errbad.
int foo( int a, int b )
{
    continue;   // illegal
    return a+b;
}
:eerrbad.

:MSGSYM. ERR_MISPLACED_DEFAULT
:MSGTXT. 'default' may only appear in a switch statement
:MSGJTXT. 'default'��switch���ł̂ݎg���܂�
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
:errbad.
int foo( int a, int b )
{
    default: // illegal
    return a+b;
}
:eerrbad.

:MSGSYM. ERR_MISPLACED_RIGHT_BRACE
:MSGTXT. misplaced '}' or missing earlier '{'
:MSGJTXT. '}'�̈ʒu���Ԉ���Ă��邩�C�����ƑO�ɂ���͂���'{'������܂���
An extra
.id }
has been found which cannot be matched up with an earlier
.id {.

:MSGSYM. ERR_MISPLACED_ELIF
:MSGTXT. misplaced #elif directive
:MSGJTXT. #elif�[�����߂̈ʒu���Ԉ���Ă��܂�
The
.kw #elif
directive must be inside an
.kw #if
preprocessing group and before the
.kw #else
directive if present.
:errbad.
int a;
#else
int c;
#elif IN_IF
int b;
#endif
:eerrbad.
The
.kw #else,
.kw #elif,
and
.kw #endif
statements are all illegal because there is no
.kw #if
that corresponds to them.

:MSGSYM. ERR_MISPLACED_ELSE
:MSGTXT. misplaced #else directive
:MSGJTXT. #else�[�����߂̈ʒu���Ԉ���Ă��܂�
The
.kw #else
directive must be inside an
.kw #if
preprocessing group and follow all
.kw #elif
directives if present.
:errbad.
int a;
#else
int c;
#elif IN_IF
int b;
#endif
:eerrbad.
The
.kw #else,
.kw #elif,
and
.kw #endif
statements are all illegal because there is no
.kw #if
that corresponds to them.

:MSGSYM. ERR_MISPLACED_ENDIF
:MSGTXT. misplaced #endif directive
:MSGJTXT. #endif�[�����߂̈ʒu���Ԉ���Ă��܂�
A
.kw #endif
preprocessing directive has been found without a matching
.kw #if
directive.
You either have an extra
.kw #endif
or you are missing an
.kw #if
directive earlier in the file.
:errbad.
int a;
#else
int c;
#elif IN_IF
int b;
#endif
:eerrbad.
The
.kw #else,
.kw #elif,
and
.kw #endif
statements are all illegal because there is no
.kw #if
that corresponds to them.

:MSGSYM. ERR_ONLY_1_DEFAULT
:MSGTXT. only one 'default' per switch statement is allowed
:MSGJTXT. 1��switch���ɋ������'default'��1�����ł�
You cannot have more than one
.kw default
label in a
.kw switch
statement.
:errbad.
int translate( int a )
{
    switch( a ) {
      case 1:
    a = 8;
    break;
      default:
    a = 9;
    break;
      default: // illegal
    a = 10;
    break;
    }
    return a;
}
:eerrbad.

:MSGSYM. ERR_EXPECTING_BUT_FOUND
:MSGTXT. expecting '%s' but found '%s'
:MSGJTXT. '%s'������͂��ł����C'%s'������܂���
A syntax error has been detected.
The tokens displayed in the message should help you to determine the problem.

:MSGSYM. ERR_UNDECLARED_SYM
:MSGTXT. symbol '%N' has not been declared
:MSGJTXT. �V���{��'%N'�͐錾����܂���ł���
The compiler has found a symbol which has not been previously declared.
The symbol may be spelled differently than the declaration, or you may
need to
.kw #include
a header file that contains the declaration.
:errbad.
int a = b;  // b has not been declared
:eerrbad.

:MSGSYM. ERR_NOT_A_FUNCTION
:MSGTXT. left expression must be a function or a function pointer
:MSGJTXT. ���ӂ͊֐����֐��|�C���^�łȂ���΂Ȃ�܂���
The compiler has found an expression that looks like a function call,
but it is not defined as a function.
:errbad.
int a;
int b = a( 12 );
:eerrbad.

:MSGSYM. ERR_MUST_BE_LVALUE
:MSGTXT. operand must be an lvalue
:MSGJTXT. �I�y�����h��'���Ӓl'�łȂ���΂Ȃ�܂���
The operand on the left side of an "=" sign must be a variable or
memory location which can have a value assigned to it.
:errbad.
void foo( int a )
{
    ( a + 1 ) = 7;
    int b = ++ ( a + 6 );
}
:eerrbad.
Both statements within the function are erroneous, since lvalues are
expected where the additions are shown.

:MSGSYM. ERR_LABEL_ALREADY_DEFINED
:MSGTXT. label '%s' already defined
:MSGJTXT. ���x��'%s'�͂��łɒ�`����Ă��܂�
All labels within a function must be unique.
:errbad.
void bar( int *p )
{
label:
    *p = 0;
label:
    return;
}
:eerrbad.
The second label is illegal.

:MSGSYM. ERR_UNDEFINED_LABEL
:MSGTXT. label '%s' is not defined in function
:MSGJTXT. ���x��'%s'�͊֐��̒��Œ�`����Ă��܂���
A
.kw goto
statement has referenced a label that is not defined in the function.
Add the necessary label or check the spelling of the label(s) in the
function.
:errbad.
void bar( int *p )
{
labl:
    *p = 0;
    goto label;
}
:eerrbad.
The label referenced in the
.kw goto
is not defined.

:MSGSYM. ERR_ZERO_DIMENSION
:MSGTXT. dimension cannot be zero
:MSGJTXT. �����̓[���ł��邱�Ƃ��ł��܂���
The dimension of an array must be non-zero.
:errbad.
int array[0];   // not allowed
:eerrbad.

:MSGSYM. ERR_NEGATIVE_DIMENSION
:MSGTXT. dimension cannot be negative
:MSGJTXT. �����͕��̐��ł��邱�Ƃ��ł��܂���
The dimension of an array must be positive.
:errbad.
int array[-1];  // not allowed
:eerrbad.

:MSGSYM. ERR_DIMENSION_REQUIRED
:MSGTXT. dimensions of multi-dimension array must be specified
:MSGJTXT. �������z��̎����͎w�肳��Ȃ���΂Ȃ�܂���
All dimensions of a multiple dimension array must be specified.
The only exception is the first dimension which can declared as "[]".
:errbad.
int array[][];   // not allowed
:eerrbad.

:MSGSYM. ERR_INVALID_STG_CLASS_FOR_FUNC
:MSGTXT. invalid storage class for function
:MSGJTXT. �֐��ɑ΂��ĕs�K�؂ȋL���N���X�ł�
If a storage class is given for a function, it must be
.kw static
or
.kw extern.
:errbad.
auto void foo()
{
}
:eerrbad.

:MSGSYM. ERR_EXPR_MUST_BE_POINTER_TO
:MSGTXT. expression must have pointer type
:MSGJTXT. ����'...�ւ̃|�C���^'�łȂ���΂Ȃ�܂���
An attempt has been made to de-reference a variable or expression
which is not declared to be a pointer.
:errbad.
int a;
int b = *a;
:eerrbad.

:MSGSYM. ERR_CANT_TAKE_ADDR_OF_RVALUE
:MSGTXT. cannot take address of an rvalue
:MSGJTXT. �E�Ӓl�̃A�h���X���Ƃ邱�Ƃ��ł��܂���
You can only take the address of a variable or memory location.
:errbad.
char c;
char *p1 = & & c;   // not allowed
char *p2 = & (c+1); // not allowed
:eerrbad.

:MSGSYM. ERR_MUST_BE_STRUCT_OR_UNION
:MSGTXT. expression for '.' must be a class, struct or union
:MSGJTXT. '.'�ɑ΂��鎮��class, struct, �܂���union�łȂ���΂Ȃ�܂���
The compiler has encountered the pattern "expression" "." "field_name"
where the expression is not a
.kw class,
.kw struct
or
.kw union
type.
:errbad.
struct S
{
    int a;
};
int &fun();
int a = fun().a;
:eerrbad.

:MSGSYM. ERR_MUST_BE_PTR_TO_STRUCT_OR_UNION
:MSGTXT. expression for '->' must be pointer to class, struct or union
:MSGJTXT. '->'�ɑ΂��鎮��class, struct, �܂���union�ւ̃|�C���^�łȂ���΂Ȃ�܂���
The compiler has encountered the pattern "expression" "->"
"field_name" where the expression is not a pointer to
.kw class,
.kw struct
or
.kw union
type.
:errbad.
struct S
{
    int a;
};
int *fun();
int a = fun()->a;
:eerrbad.

:MSGSYM. ERR_SYM_ALREADY_DEFINED
:MSGTXT. symbol '%S' already defined
:MSGJTXT. '%S'�͂��łɃV���{����`����Ă��܂�
The specified symbol has already been defined.
:errbad.
char a = 2;
char a = 2; // not allowed
:eerrbad.

:MSGSYM. ERR_FUNCTION_NOT_DEFINED
:MSGTXT. static function '%S' has not been defined
:MSGJTXT. �X�^�e�B�b�N�֐�'%S'�͒�`����܂���ł���
A prototype has been found for a
.kw static
function, but a definition for the
.kw static
function has not been found in the file.
:errbad.
static int fun( void );
int k = fun();
// fun not defined by end of program
:eerrbad.

:MSGSYM. ERR_EXPECTING_LABEL
:MSGTXT. expecting label for goto statement
:MSGJTXT. goto���ɑΉ����郉�x��������͂��ł�
The
.kw goto
statement requires the name of a label.
:errbad.
int fun( void )
{
    goto;
}
:eerrbad.

:MSGSYM. ERR_DUPLICATE_CASE_VALUE
:MSGTXT. duplicate case value '%s' found
:MSGJTXT. case�̒l'%s'��2����܂�
Every case value in a
.kw switch
statement must be unique.
:errbad.
int fun( int a )
{
    switch( a ) {
      case 1:
    return 7;
      case 2:
    return 9;
      case 1: // duplicate not allowed
    return 7;
    }
    return 79;
}
:eerrbad.

:MSGSYM. ERR_FIELD_TOO_WIDE
:MSGTXT. bit-field width is too large
:MSGJTXT. �r�b�g�t�B�[���h�����傫�����܂�
The maximum field width allowed is 16 bits in the 16-bit compiler
and 32 bits in the 32-bit compiler.
:errbad.
struct S
{
    unsigned bitfield :48;  // too wide
};
:eerrbad.

:MSGSYM. ERR_WIDTH_0
:MSGTXT. width of a named bit-field must not be zero
:MSGJTXT. ���O������ꂽ�r�b�g�t�B�[���h�̕��̓[���ł����Ă͂Ȃ�܂���
A bit field must be at least one bit in size.
:errbad.
struct S {
    int bitfield :10;
    int :0;   // okay, aligns to int
    int h :0; // error, field is named
};
:eerrbad.

:MSGSYM. ERR_WIDTH_NEGATIVE
:MSGTXT. bit-field width must be positive
:MSGJTXT. �r�b�g�t�B�[���h���͐��̐��łȂ���΂Ȃ�܂���
You cannot have a negative field width.
:errbad.
struct S
{
    unsigned bitfield :-10; // cannot be negative
};
:eerrbad.

:MSGSYM. ERR_BITFIELD_BAD_BASE_TYPE
:MSGTXT. bit-field base type must be an integral type
:MSGJTXT. �r�b�g�t�B�[���h�̊�{�^�͐����^�łȂ���΂Ȃ�܂���
The types allowed for bit fields are
.kw signed
or
.kw unsigned
varieties of
.kw char,
.kw short
and
.kw int.
:errbad.
struct S
{
    float bitfield : 10;    // must be integral
};
:eerrbad.

:MSGSYM. ERR_EXPR_MUST_BE_ARRAY
:MSGTXT. subscript on non-array
:MSGJTXT. ��z��ւ̓Y�����ł�
One of the operands of '[]' must be an array or a pointer.
:errbad.
int array[10];
int i1 = array[0];  // ok
int i2 = 0[array];  // same as above
int i3 = 0[1];      // illegal
:eerrbad.

:MSGSYM. ERR_INCOMPLETE_COMMENT
:MSGTXT. incomplete comment
:MSGJTXT. �s���S�ȃR�����g�ł�
The compiler did not find
.id */
to mark the end of a comment.

:MSGSYM. ERR_MUST_BE_MACRO_PARM
:MSGTXT. argument for # must be a macro parm
:MSGJTXT. #�̈����̓}�N���̃p�����[�^�łȂ���΂Ȃ�܂���
The argument for the stringize operator '#' must be a macro parameter.

:MSGSYM. ERR_UNKNOWN_DIRECTIVE
:MSGTXT. unknown preprocessing directive '#%s'
:MSGJTXT. ���m�̑O�����[������'#%s'�ł�
An unrecognized preprocessing directive has been encountered.
Check for correct spelling.
:errbad.
#i_goofed   // not valid
:eerrbad.

:MSGSYM. ERR_INVALID_INCLUDE
:MSGTXT. invalid #include directive
:MSGJTXT. �s�K�؂�#include�[�����߂ł�
A syntax error has been encountered in a
.kw #include
directive.
:errbad.
#include    // no header file
#include stdio.h
:eerrbad.
Both examples are illegal.

:MSGSYM. ERR_TOO_FEW_MACRO_PARMS
:MSGTXT. not enough parameters given for macro '%s'
:MSGJTXT. �}�N��'%s'�ɗ^����ꂽ�p�����[�^�[���s�\���ł�
You have not supplied enough parameters to the specified macro.
:errbad.
#define mac(a,b) a+b
int i = mac(123);   // needs 2 parameters
:eerrbad.

:MSGSYM. ERR_NOT_EXPECTING_RETURN_VALUE
:MSGTXT. not expecting a return value
:MSGJTXT. �߂�l�͂Ȃ��͂��ł�
The specified function is declared as a
.kw void
function.
Delete the
.kw return
value, or change the type of the function.
:errbad.
void fun()
{
    return 14;  // not expecting return value
}
:eerrbad.

:MSGSYM. ERR_CANT_TAKE_ADDR_OF_BIT_FIELD
:MSGTXT. cannot take address of a bit-field
:MSGJTXT. �r�b�g�t�B�[���h�̃A�h���X���Ƃ邱�Ƃ��ł��܂���
The smallest addressable unit is a byte.
You cannot take the address of a bit field.
:errbad.
struct S
{   int bits :6;
    int bitfield :10;
};
S var;
void* p = &var.bitfield;    // illegal
:eerrbad.

:MSGSYM. ERR_NOT_A_CONSTANT_EXPR
:MSGTXT. expression must be a constant
:MSGJTXT. ���͒萔�łȂ���΂Ȃ�܂���
The compiler expects a constant expression.
This message can occur during static initialization if you are
trying to initialize a non-pointer type with an address expression.

:MSGSYM. ERR_CANT_OPEN_FILE
:MSGTXT. unable to open '%s'
:MSGJTXT. '%s'���I�[�v�����邱�Ƃ��ł��܂���
The file specified in an
.kw #include
directive could not be located.
Make sure that the file name is spelled correctly, or that the
appropriate path for the file is included in the list of paths
specified in the
.ev &incvarup
or
.ev INCLUDE
environment variables or in the "i=" option on the command line.

:MSGSYM. ANSI_TOO_MANY_MACRO_PARMS
:MSGTXT. too many parameters given for macro '%s'
:MSGJTXT. �}�N��'%s'�ɗ^����ꂽ�p�����[�^���������܂�
:ANSI. 1
You have supplied too many parameters for the specified macro.
The extra parameters are ignored.
:errbad.
#define mac(a,b) a+b
int i = mac(1,2,3); // needs 2 parameters
:eerrbad.

:MSGSYM. ERR_CANNOT_USE_PCPTR
:MSGTXT. cannot use __based or __far16 pointers in this context
:MSGJTXT. ���̃R���e�L�X�g�ɂ����āC__based���邢��__far16�|�C���^���g�����Ƃ͂ł��܂���
The use of
.kw __based
and
.kw __far16
pointers is prohibited in
.kw throw
expressions and
.kw catch
statements.
:errbad.
extern int __based( __segname( "myseg" ) ) *pi;

void bad()
{
    try {
    throw pi;
    } catch( int __far16 *p16 ) {
    *p16 = 87;
    }
}
:eerrbad.
Both the
.kw throw
expression and
.kw catch
statements cause this error to be diagnosed.

:MSGSYM. ERR_TOO_MANY_TYPES_IN_DSPEC
:MSGTXT. only one type is allowed in declaration specifiers
:MSGJTXT. ��̌^�������錾�w��q�̒��ŋ�����܂�
Only one type is allowed for the first part of a declaration.
A common cause of this message is that there
may be a missing semi-colon (';') after a class definition.
:errbad.
class C
{
public:
    C();
}           // needs ";"

int foo() { return 7; }
:eerrbad.

:MSGSYM. ERR_OUT_OF_MEMORY
:MSGTXT. out of memory
:MSGJTXT. �������[�s���ł�
The compiler has run out of memory to store information about the file
being compiled.
Try reducing the number of data declarations and or the size of the file
being compiled.
Do not
.kw #include
header files that are not required.

:MSGSYM. ERR_INV_CHAR_CONSTANT
:MSGTXT. invalid character constant
:MSGJTXT. �s�K�؂ȕ����萔�ł�
This message is issued for an improperly formed character constant.
:errbad.
char c = '12345';
char d = ''';
:eerrbad.

:MSGSYM. WARN_CANT_TAKE_ADDR_OF_REGISTER
:MSGTXT. taking address of variable with storage class 'register'
:MSGJTXT. �L���N���X'register'�̕ϐ��̃A�h���X������Ă��܂�
:WARNING. 2
You can take the address of a
.kw register
variable in C++ (but not in ISO/ANSI C). If there is a chance that the source
will be compiled using a C compiler, change the storage
class from
.kw register
to
.kw auto.
:errgood.
extern int foo( char* );
int bar()
{
    register char c = 'c';
    return foo( &c );
}
:eerrgood.

:MSGSYM. ANSI_NO_DELETE_SIZE_EXPR
:MSGTXT. 'delete' expression size is not allowed
:MSGJTXT. 'delete'���̑傫���̎w��͋�����܂���
:ANSI. 1
The C++ language has evolved to the point where the
.kw delete
expression size is no longer required for a correct deletion of an
array.
:errbad.
void fn( unsigned n, char *p ) {
    delete [n] p;
}
:eerrbad.

:MSGSYM. ERR_MISSING_QUOTE
:MSGTXT. ending " missing for string literal
:MSGJTXT. ������萔�ɑ΂���I����"������܂���
The compiler did not find a second double quote to end the string literal.
:errbad.
char *a = "no_ending_quote;
:eerrbad.

:MSGSYM. ERR_INVALID_OPTION
:MSGTXT. invalid option
:MSGJTXT. �s�K�؂ȃI�v�V�����ł�
The specified option is not recognized by the compiler.

:MSGSYM. ERR_INVALID_OPTIMIZATION
:MSGTXT. invalid optimization option
:MSGJTXT. �s�K�؂ȍœK���I�v�V�����ł�
The specified option is an unrecognized optimization option.

:MSGSYM. ERR_INVALID_MEMORY_MODEL
:MSGTXT. invalid memory model
:MSGJTXT. �s�K�؂ȃ��������f���ł�
Memory model option must be one of "ms", "mm", "mc", "ml", "mh" or
"mf" which selects the Small, Medium, Compact, Large, Huge or Flat
memory model.

:MSGSYM. ERR_EXPR_MUST_BE_INTEGRAL
:MSGTXT. expression must be integral
:MSGJTXT. ���͐����łȂ���΂Ȃ�܂���
An integral expression is required.
:errbad.
int foo( int a, float b, int *p )
{
    switch( a ) {
      case 1.3:     // must be integral
    return p[b];    // index not integer
      case 2:
    b <<= 2;    // can only shift integers
      default:
    return b;
    }
}
:eerrbad.

:MSGSYM. ERR_EXPR_MUST_BE_ARITHMETIC
:MSGTXT. expression must be arithmetic
:MSGJTXT. ���͎Z�p���łȂ���΂Ȃ�܂���
Arithmetic operations, such as "/" and "*",  require arithmetic operands
unless the operation has been overloaded
or unless the operands can be converted to arithmetic operands.
:errbad.
class C
{
public:
    int c;
};
C cv;
int i = cv / 2;
:eerrbad.

:MSGSYM. ERR_STMT_REQUIRED_AFTER_LABEL
:MSGTXT. statement required after label
:MSGJTXT. ���x���̌�ɕ����K�v�ł�
The C language definition requires a statement following a label.
You can use a null statement which consists of just a semicolon (";").
:errbad.
extern int bar( int );
void foo( int a )
{
    if( a ) goto ending;
    bar( a );
ending:
    // needs statement following
}
:eerrbad.

:MSGSYM. ERR_STMT_REQUIRED_AFTER_DO
:MSGTXT. statement required after 'do'
:MSGJTXT. 'do'�̌�ɕ����K�v�ł�
A statement is required between the
.kw do
and
.kw while
keywords.

:MSGSYM. ERR_STMT_REQUIRED_AFTER_CASE
:MSGTXT. statement required after 'case'
:MSGJTXT. 'case'�̌�ɕ����K�v�ł�
The C language definition requires a statement following a
.kw case
label.
You can use a null statement which consists of just a semicolon (";").
:errbad.
int foo( int a )
{
    switch( a ) {
      default:
    return 7;
      case 1: // needs statement following
    }
    return 18;
}
:eerrbad.

:MSGSYM. ERR_STMT_REQUIRED_AFTER_DEFAULT
:MSGTXT. statement required after 'default'
:MSGJTXT. 'default'�̌�ɕ����K�v�ł�
The C language definition requires a statement following a
.kw default
label.
You can use a null statement which consists of just a semicolon (";").
:errbad.
int foo( int a )
{
    switch( a ) {
      case 7:
    return 7;
      default:
    // needs statement following
    }
    return 18;
}
:eerrbad.

:MSGSYM. ERR_MISSING_CENDIF
:MSGTXT. missing matching #endif directive
:MSGJTXT. �Ή�����#endif�[�����߂�����܂���
You are missing a
.kw #endif
to terminate a
.kw #if,
.kw #ifdef
or
.kw #ifndef
preprocessing directive.
:errbad.
#if 1
int a;
// needs #endif
:eerrbad.

:MSGSYM. ERR_INVALID_MACRO_DEFN
:MSGTXT. invalid macro definition, missing ')'
:MSGJTXT. �s�K�؂ȃ}�N����`�ł��G �j������܂���
The right parenthesis ")" is required for a function-like macro definition.
:errbad.
#define bad_mac( a, b
:eerrbad.

:MSGSYM. ERR_INCOMPLETE_MACRO
:MSGTXT. missing ')' for expansion of '%s' macro
:MSGJTXT. '%s'�}�N���̓W�J�ɑ΂��āj������܂���
The compiler encountered end-of-file while collecting up the argument for a
function-like macro.
A right parenthesis ")" is required to mark the end of the argument(s) for
a function-like macro.
:errbad.
#define mac( a, b) a+b
int d = mac( 1, 2
:eerrbad.

:MSGSYM. ERR_USER_ERROR_MSG
:MSGTXT. %s
:MSGJTXT. %s
This is a user message generated with the
.kw #error
preprocessing directive.
:errbad.
#error my very own error message
:eerrbad.

:MSGSYM. ERR_CANT_HAVE_AN_ARRAY_OF_FUNCTIONS
:MSGTXT. cannot define an array of functions
:MSGJTXT. �֐��̔z����`���邱�Ƃ͂ł��܂���
You can have an array of pointers to functions, but not an array
of functions.
:errbad.
typedef int TD(float);
TD array[12];
:eerrbad.

:MSGSYM. ERR_FUNCTION_CANT_RETURN_AN_ARRAY
:MSGTXT. function cannot return an array
:MSGJTXT. �֐��͔z���߂����Ƃ��ł��܂���
A function cannot return an array.
You can return a pointer to an array.
:errbad.
typedef int ARR[10];
ARR fun( float );
:eerrbad.

:MSGSYM. ERR_FUNCTION_CANT_RETURN_A_FUNCTION
:MSGTXT. function cannot return a function
:MSGJTXT. �֐��͊֐���߂����Ƃ��ł��܂���
You cannot return a function.
You can return a pointer to a function.
:errbad.
typedef int TD();
TD fun( float );
:eerrbad.

:MSGSYM. ERR_FUNCTION_TEMPLATE_ONLY_GENERICS
:MSGTXT. function templates can only have type arguments
:MSGJTXT. �֐��e���v���[�g�͌^�����������Ƃ��ł��邾���ł�
A function template argument can only be a generic type (e.g.,
.id template < class
T
.id >
).
This is a restriction in the C++ language that allows compilers to
automatically instantiate functions purely from the argument types of calls.

:MSGSYM. ERR_MAX_STRUCT_EXCEEDED
:MSGTXT. maximum class size has been exceeded
:MSGJTXT. �N���X�T�C�Y���ő�l���z���܂���
The 16-bit compiler limits the size of a
.kw struct
or
.kw union
to 64K so that the compiler
can represent the offset of a member in a 16-bit register.
This error also occurs if the size of a structure overflows
the size of an
.kw unsigned
integer.
:errbad.
struct S
{
    char arr1[ 0xfffe ];
    char arr2[ 0xfffe ];
    char arr3[ 0xfffe ];
    char arr4[ 0xfffffffe ];
};
:eerrbad.

:MSGSYM. ANSI_MACRO_DEFN_NOT_IDENTICAL
:MSGTXT. definition of macro '%s' not identical to previous definition
:MSGJTXT. �}�N��'%s'�̒�`���ȑO�̒�`�ƈ�v���܂���
:ANSI. 1
If a macro is defined more than once, the definitions must be identical.
If you want to redefine a macro to have a different definition, you must
.kw #undef
it before you can define it with a new definition.
:errbad.
#define CON 123
#define CON 124     // not same as previous
:eerrbad.

:MSGSYM. ERR_CANNOT_INIT_IN_NON_FILE_SCOPE
:MSGTXT. initialization of '%S' must be in file scope
:MSGJTXT. '%S'�̏������ݒ�̓t�@�C���E�X�R�[�v�̒��ɂȂ���΂Ȃ�܂���
A file scope variable must be initialized in file scope.
:errbad.
void fn()
{
    extern int v = 1;
}
:eerrbad.

:MSGSYM. WARN_DEFAULT_ARG_ADDED_TO_MEMBER_FN
:MSGTXT. default argument for '%S' declared outside of class definition
:MSGJTXT. '%S'�̃f�t�H���g�������N���X��`�̊O���Ő錾����܂���
:WARNING. 1
Problems can occur with member functions that do not declare all of their
default arguments during the class definition.  For instance, a copy
constructor is declared if a class does not define a copy constructor.
If a default argument is added later on to a constructor that makes it
a copy constructor, an ambiguity results.
:errbad.
struct S {
    S( S const &, int );
    // S( S const & );  <-- declared by compiler
};
// ambiguity with compiler
// generated copy constructor
// S( S const & );
S::S( S const &, int = 0 )
{
}
:eerrbad.

:MSGSYM. ERR_MISPLACED_SHARP_SHARP
:MSGTXT. ## must not be at start or end of replacement tokens
:MSGJTXT. ##�͒u�������g�[�N���̐擪�܂��͍Ō�ɂ����Ă͂Ȃ�܂���
There must be a token on each side of the "##" (token pasting) operator.
:errbad.
#define badmac( a, b ) ## a ## b
:eerrbad.

:MSGSYM. ERR_INVALID_FLOATING_POINT_CONSTANT
:MSGTXT. invalid floating-point constant
:MSGJTXT. �s�K�؂ȕ��������_�萔�ł�
The exponent part of the floating-point constant is not formed correctly.
:errbad.
float f = 123.9E+Q;
:eerrbad.

:MSGSYM. ERR_CANT_TAKE_SIZEOF_FIELD
:MSGTXT. 'sizeof' is not allowed for a bit-field
:MSGJTXT. 'sizeof'�̓r�b�g�t�B�[���h�ɑ΂��Ă͎g���܂���
The smallest object that you can ask for the size of is a char.
:errbad.
struct S
{   int a;
    int b :10;
} v;
int k = sizeof( v.b );
:eerrbad.

:MSGSYM. ERR_INVALID_OPTION_PATH
:MSGTXT. option requires a path
:MSGJTXT. �I�v�V�����ɂ̓p�X���K�v�ł�
The specified option is not recognized by the compiler
since there was no path after it (i.e., "-i=d:\include;d:\path" ).

:MSGSYM. ERR_MUST_BE_VAR_PARM_FUNC
:MSGTXT. must use 'va_start' macro inside function with variable arguments
:MSGJTXT. 'va_start'�}�N���͉ό̈������������֐��̒��Ŏg��Ȃ���΂Ȃ�܂���
The
.id va_start
macro is used to setup access to the parameters in a function
that takes a variable number of parameters.
A function is defined with a variable number of parameters by declaring
the last parameter in the function as "...".
:errbad.
#include <stdarg.h>
int foo( int a, int b )
{
    va_list args;
    va_start( args, a );
    va_end( args );
    return b;
}
:eerrbad.

:MSGSYM. ERR_FATAL_ERROR
:MSGTXT. ***FATAL*** %s
:MSGJTXT. ***�v���I*** %s
A fatal error has been detected during code generation time.
The type of error is displayed in the message.

:MSGSYM. ERR_BACK_END_ERROR
:MSGTXT. internal compiler error %d
:MSGJTXT. �R���p�C�������G���[ %d
A bug has been encountered in the compiler.
Please report the specified internal compiler error number and any other
helpful details about the program being compiled to the Open Watcom
development team so that we
can fix the problem. See http://www.openwatcom.org/.

:MSGSYM. ERR_BAD_PARM_REGISTER
:MSGTXT. argument number %d - invalid register in #pragma
:MSGJTXT. �����ԍ�%d - #pragma�̒��̕s�K�؂ȃ��W�X�^�w��
The designated registers cannot hold the value for the parameter.

:MSGSYM. ERR_BAD_RETURN_REGISTER
:MSGTXT. procedure '%s' has invalid return register in #pragma
:MSGJTXT. �v���V�[�W��'%s'�́C#pragma�̒��ŕs�K�؂ȃ��^�[���E���W�X�^���w�肳��Ă��܂�
The size of the return register does not match the size of the result
returned by the function.

:MSGSYM. ERR_BAD_SAVE
:MSGTXT. illegal register modified by '%s' #pragma
:MSGJTXT. '%s'#pragma�ɂ���Ĉ�@�ȃ��W�X�^���C������Ă��܂�
.us For the 16-bit Open Watcom C/C++ compiler:
The BP, CS, DS, and SS registers cannot be modified in small data models.
The BP, CS, and SS registers cannot be modified in large data models.
.np
.us For the 32-bit Open Watcom C/C++ compiler:
The EBP, CS, DS, ES, and SS registers cannot be modified in flat
memory models.
The EBP, CS, DS, and SS registers cannot be modified in small data
models.
The EBP, CS, and SS registers cannot be modified in large data models.

:MSGSYM. ANSIERR_NO_EXTERNAL_DEFNS_FOUND
:MSGTXT. file must contain at least one external definition
:MSGJTXT. �t�@�C���͏����Ƃ�1�̊O����`���܂܂Ȃ���΂Ȃ�܂���
:ANSIERR.
Every file must contain at least one global object, (either a data variable
or a function).
.np
Note: This message has been disabled starting with Open Watcom v1.4. The
ISO 1998 C++ standard allows empty translation units.

:MSGSYM. ERR_OUT_OF_MACRO_MEMORY
:MSGTXT. out of macro space
:MSGJTXT. �}�N����Ԃ��s���ł�
The compiler ran out of memory for storing macro definitions.

:MSGSYM. ERR_BREAK_KEY_HIT
:MSGTXT. keyboard interrupt detected
:MSGJTXT. �L�[�{�[�h���荞�݂����o����܂���
The compilation has been aborted with Ctrl/C or Ctrl/Break.

:MSGSYM. ERR_DUPLICATE_MACRO_PARM
:MSGTXT. duplicate macro parameter '%s'
:MSGJTXT. �}�N���p�����[�^'%s'��2����܂�
The parameters specified in a macro definition must be unique.
:errbad.
#define badmac( a, b, a ) a ## b
:eerrbad.

:MSGSYM. ERR_UNABLE_TO_OPEN_WORK_FILE
:MSGTXT. unable to open work file: error code = %d
:MSGJTXT. ���[�N�E�t�@�C�����I�[�v�����邱�Ƃ��ł��܂���F�G���[�R�[�h = %d
The compiler tries to open a new work file by the name "__wrkN__.tmp" where
N is the digit 0 to 9.
This message will be issued if all of those files already exist.

:MSGSYM. ERR_WORK_FILE_WRITE_ERROR
:MSGTXT. write error on work file: error code = %d
:MSGJTXT. ���[�N�E�t�@�C���̏����݃G���[�F�G���[�R�[�h = %d
An error was encountered trying to write information to the work file.
The disk could be full.

:MSGSYM. ERR_WORK_FILE_READ_ERROR
:MSGTXT. read error on work file: error code = %d
:MSGJTXT. ���[�N�E�t�@�C���̓ǎ��G���[�F�G���[�R�[�h = %d
An error was encountered trying to read information from the work file.

:MSGSYM. ERR_TOKEN_TRUNCATED
:MSGTXT. token too long; truncated
:MSGJTXT. �g�[�N�����������܂��G�؂�l�߂��܂�
The token must be less than 510 bytes in length.

:MSGSYM. ERR_FILENAME_REQUIRED
:MSGTXT. filename required on command line
:MSGJTXT. �R�}���h�E���C����Ńt�@�C�����̎w�肪�K�v�ł�
The name of a file to be compiled must be specified on the command line.

:MSGSYM. ERR_CAN_ONLY_COMPILE_ONE_FILE
:MSGTXT. command line contains more than one file to compile
:MSGJTXT. �R�}���h�E���C���ɂ̓R���p�C������2�ȏ�̃t�@�C��������܂�
You have more than one file name specified on the command line to be
compiled.
The compiler can only compile one file at a time.
You can use the &wclname. utility to compile multiple files with a
single command.

:MSGSYM. ERR_UNION_NO_VIRTUAL_FUNCTIONS
:MSGTXT. virtual member functions are not allowed in a union
:MSGJTXT. ���z�����o�֐���union�̒��ŋ�����܂���
A union can only be used to overlay the storage of data.
The storage of virtual function
information (in a safe manner) cannot be done if storage is overlaid.
:errbad.
struct S1{ int f( int ); };
struct S2{ int f( int ); };
union un { S1 s1;
       S2 s2;
       virtual int vf( int );
     };
:eerrbad.

:MSGSYM. ERR_UNION_CANNOT_BE_BASE
:MSGTXT. union cannot be used as a base class
:MSGJTXT. union�͊��N���X�Ƃ��Ďg���܂���
This restriction prevents C++ programmers from viewing a
.kw union
as an encapsulation unit.
If it is necessary, one can encapsulate the union into a
.kw class
and achieve the same effect.
:errbad.
union U { int a; int b; };
class S : public U { int s; };
:eerrbad.

:MSGSYM. ERR_UNION_CANNOT_HAVE_BASE
:MSGTXT. union cannot have a base class
:MSGJTXT. union�͊��N���X�������Ƃ��ł��܂���
This restriction prevents C++ programmers from viewing a
.kw union
as an encapsulation unit.
If it is necessary, one can encapsulate the union into a
.kw class
and inherit the base classes normally.
:errbad.
class S { public: int s; };
union U : public S { int a; int b; };
:eerrbad.

:MSGSYM. ERR_CANNOT_INHERIT_UNDEFINED
:MSGTXT. cannot inherit an undefined base class '%T'
:MSGJTXT. ����`���N���X'%T'���󂯌p�����Ƃ͂ł��܂���
The storage requirements for a
.kw class
type must be known when inheritance is involved because
the layout of the final class depends on knowing the complete
contents of all base classes.
:errbad.
class Undefined;
class C : public Undefined {
    int c;
};
:eerrbad.

:MSGSYM. ERR_REPEATED_BASE_CLASS
:MSGTXT. repeated direct base class will cause ambiguities
:MSGJTXT. �J��Ԃ��ꂽ���ڂ̃x�[�X�N���X���B�����������N�����Ă��܂�
Almost all accesses will be ambiguous.  This restriction
is useful in catching programming errors.  The repeated base class
can be encapsulated in another class if the repetition is required.
:errbad.
class Dup
{
    int d;
};
class C : public Dup, public Dup
{
    int c;
};
:eerrbad.

:MSGSYM. ERR_ONLY_GLOBAL_TEMPLATES
:MSGTXT. templates may only be declared in namespace scope
:MSGJTXT. �e���v���[�g�̓t�@�C���E�X�R�[�v�̒��Ő錾����邾���ł�
Currently, templates can only be declared in namespace scope.
This simple restriction was chosen in favour of more freedom with
possibly subtle restrictions.

:MSGSYM. ERR_ONLY_GLOBAL_LINKAGES
:MSGTXT. linkages may only be declared in file scope
:MSGJTXT. �����N�̓t�@�C���E�X�R�[�v�̒��Ő錾����邾���ł�
A common source of errors for C and C++ result from the use of prototypes
inside of functions.  This restriction attempts to prevent
such errors.

:MSGSYM. ERR_UNKNOWN_LINKAGE
:MSGTXT. unknown linkage '%s'
:MSGJTXT. ���m�̃����N'%s'�ł�
Only the linkages "C" and "C++" are supported by Open Watcom C++.
:errbad.
extern "APL" void AplFunc( int* );
:eerrbad.

:MSGSYM. ERR_TOO_MANY_SC_SPECIFIERS
:MSGTXT. too many storage class specifiers
:MSGJTXT. �L���N���X�w��q���������܂�
This message is a result of duplicating a previous storage class or
having a different storage class.
You can only have one of the following storage classes,
.kw extern,
.kw static,
.kw auto,
.kw register,
or
.kw typedef.
:errbad.
extern typedef int (*fn)( void );
:eerrbad.

:MSGSYM. ERR_NO_DECLARATOR
:MSGTXT. nameless declaration is not allowed
:MSGJTXT. ���O�Ȃ��錾�͂ł��܂���
A type was used in a declaration but no name was given.
:errbad.
static int;
:eerrbad.

:MSGSYM. ERR_ILLEGAL_TYPE_COMBO
:MSGTXT. illegal combination of type specifiers
:MSGJTXT. �^�w��q�̈�@�ȑg�����ł�
An incorrect scalar type was found.  Either a scalar keyword
was repeated or the combination is illegal.
:errbad.
short short x;
short long y;
:eerrbad.

:MSGSYM. ERR_ILLEGAL_QUALIFIER_COMBO
:MSGTXT. illegal combination of type qualifiers
:MSGJTXT. �^�C����̈�@�ȑg�����ł�
A repetition of a type qualifier has been detected.  Some compilers may
ignore repetitions but strictly speaking it is incorrect code.
:errbad.
const const x;
struct S {
    int virtual virtual fn();
};
:eerrbad.

:MSGSYM. ERR_SYNTAX
:MSGTXT. syntax error
:MSGJTXT. �\���G���[
The C++ compiler was unable to interpret the text starting at the location
of the message.  The C++ language is sufficiently complicated that
it is difficult for a compiler to correct the error itself.

:MSGSYM. ERR_PARSER_DIED
:MSGTXT. parser stack corrupted
:MSGJTXT. �\����̓v���O�����̃X�^�b�N�����Ă��܂�
The C++ parser has detected an internal problem that usually indicates
a compiler problem.  Please report this directly to the Open Watcom
development team. See http://www.openwatcom.org/.

:MSGSYM. ERR_NO_NESTED_TEMPLATES
:MSGTXT. template declarations cannot be nested within each other
:MSGJTXT. �e���v���[�g�錾�́C���݂��͈͓̔��Ńl�X�g���邱�Ƃ��ł��܂���
Currently, templates can only be declared in namespace scope.
Furthermore, a template declaration must be finished before another
template can be declared.

:MSGSYM. ERR_COMPLICATED_EXPRESSION
:MSGTXT. expression is too complicated
:MSGJTXT. �������G�����܂�
The expression contains too many levels of nested parentheses.
Divide the expression up into two or more sub-expressions.

:MSGSYM. ERR_INVALID_TYPEDEF_REDEFINITION
:MSGTXT. invalid redefinition of the typedef name '%S'
:MSGJTXT. typedef��'%S'�̍Ē�`�͂ł��܂���
Redefinition of typedef names is only allowed if you are redefining a
typedef name to itself.
Any other redefinition is illegal.
You should delete the duplicate
.kw typedef
definition.
:errbad.
typedef int TD;
typedef float TD;   // illegal
:eerrbad.

:MSGSYM. ERR_CLASS_REDEFINED
:MSGTXT. class '%T' has already been defined
:MSGJTXT. �N���X'%T'�͊��ɒ�`����Ă��܂�
This message usually results from the definition of two classes in the
same scope.  This is illegal regardless of whether the class definitions
are identical.
:errbad.
class C {
};
class C {
};
:eerrbad.

:MSGSYM. ERR_CANT_TAKE_SIZEOF_UNDEFINED
:MSGTXT. 'sizeof' is not allowed for an undefined type
:MSGJTXT. 'sizeof'�͖���`�̌^�ɂ͎g�p�ł��܂���
If a type has not been defined, the compiler cannot know how large it is.
:errbad.
class C;
int x = sizeof( C );
:eerrbad.

:MSGSYM. ANSI_INIT_BYPASSED
:MSGTXT. initializer for variable '%S' cannot be bypassed
:MSGJTXT. �ϐ�'%S'�̏������q�̓o�C�p�X�ł��܂���
:ANSI. 1
The variable may not be initialized when code is executing
at the position indicated in the message.  The C++ language
places these restrictions to prevent the use of uninitialized
variables.
:errbad.
int foo( int a )
{
    switch( a ) {
      case 1:
    int b = 2;
    return b;
      default: // b bypassed
    return b + 5;
    }
}
:eerrbad.

:MSGSYM. ERR_DIVISION_BY_ZERO
:MSGTXT. division by zero in a constant expression
:MSGJTXT. �萔���̒��Ń[���ɂ�鏜�Z���N����܂�
Division by zero is not allowed in a constant expression.
The value of the expression cannot be used with this error.
:errbad.
int foo( int a )
{
    switch( a ) {
      case 4 / 0:  // illegal
    return a;
    }
    return a + 2;
}
:eerrbad.

:MSGSYM. ANSI_ARITHMETIC_OVERFLOW
:MSGTXT. arithmetic overflow in a constant expression
:MSGJTXT. �萔���̒��ŎZ�p�I�[�o�[�t���[���N����܂�
:ANSI. 3
The multiplication of two integral values cannot be represented.
The value of the expression cannot be used with this error.
:errbad.
int foo( int a )
{
    switch( a ) {
      case 0x7FFF * 0x7FFF * 0x7FFF:  // overflow
    return a;
    }
    return a + 2;
}
:eerrbad.

:MSGSYM. WARN_CG_MEM_PROC
:MSGTXT. not enough memory to fully optimize procedure '%s'
:MSGJTXT. �v���V�[�W��'%s'���\���ɍœK�����邽�߂̃��������s�����Ă��܂�
:WARNING. 4
The indicated procedure cannot be fully optimized with the
amount of memory available.  The code generated will still be
correct and execute properly.  This message is purely informational
(i.e., buy more memory).

:MSGSYM. WARN_CG_MEM_PEEPHOLE
:MSGTXT. not enough memory to maintain full peephole
:MSGJTXT. ���S�Ƀs�[�v�z�[�����ێ����邽�߂̃��������s�����Ă��܂�
:WARNING. 4
Certain optimizations benefit from being able to store the entire module
in memory during optimization.
All functions will be individually optimized but the optimizer will
not be able to share code between functions if this message appears.
The code generated will still be
correct and execute properly.  This message is purely informational
(i.e., buy more memory).

:MSGSYM. ERR_EXCEEDED_LIMIT
:MSGTXT. too many errors: compilation aborted
:MSGJTXT. �G���[���������܂��F�R���p�C���͒��~���܂���
The Open Watcom C++ compiler sets a limit to the number of error messages
it will issue.
Once the number of messages reaches the limit the above message is issued.
This limit can be changed via the "/e" command line option.

:MSGSYM. ERR_TOO_MANY_PARM_SETS
:MSGTXT. too many parm sets
:MSGJTXT. parm�̐ݒ肪�������܂�
An extra parameter passing description has been found in the aux pragma text.
Only one parameter passing description is allowed.

:MSGSYM. ERR_BAD_FN_MODIFIER
:MSGTXT. 'friend', 'virtual' or 'inline' modifiers may only be used on functions
:MSGJTXT. 'friend'�C'virtual'�C���邢�́C'inline'�C���q���֐��ɑ΂��Ă̂ݎg�p�ł��܂�
This message indicates that you are trying to declare a strange entity like
an
.kw inline
variable.  These qualifiers can only be used on function declarations and
definitions.

:MSGSYM. ERR_MULTIPLE_PRAGMA_MODS
:MSGTXT. more than one calling convention has been specified
:MSGJTXT. 2�ȏ�̌ďo���K�񂪎w�肳��܂���
A function cannot have more than one #pragma modifier applied to it.
Combine the pragmas into one pragma and apply it once.

:MSGSYM. ERR_MUST_BE_ZERO
:MSGTXT. pure member function constant must be '0'
:MSGJTXT. ���������o�֐��萔��'0'�łȂ���΂Ȃ�܂���
The constant must be changed to '0' in order for the Open Watcom C++ compiler
to accept the pure virtual member function declaration.
:errbad.
struct S {
    virtual int wrong( void ) = 91;
};
:eerrbad.

:MSGSYM. ERR_REPEATED_BASED_MODS
:MSGTXT. based modifier has been repeated
:MSGJTXT. based�C���q���J��Ԃ���܂���
A repeated based modifier has been detected.  There are no semantics for
combining base modifiers so this is not allowed.
:errbad.
char *ptr;
char __based( void ) __based( ptr ) *a;
:eerrbad.

:MSGSYM. ERR_BAD_ENUM_ASSIGNMENT
:MSGTXT. enumeration variable is not assigned a constant from its enumeration
:MSGJTXT. �񋓕ϐ��ɂ��̗񋓂̒萔���������Ă��܂���
In C++ (as opposed to C), enums represent values of distinct types.
Thus, the compiler will not automatically convert an integer value
to an enum type.
:errbad.
enum Days { sun, mod, tues, wed, thur, fri, sat };
enum Days day = 2;
:eerrbad.

:MSGSYM. ERR_BITFIELD_STG_CLASS
:MSGTXT. bit-field declaration cannot have a storage class specifier
:MSGJTXT. �r�b�g�t�B�[���h�錾�͋L���N���X�w��q�������Ƃ��ł��܂���
Bit-fields (along with most members) cannot have storage class specifiers
in their declaration.  Remove the storage class specifier to correct the
code.
:errbad.
class C
{
public:
    extern unsigned bitf :10;
};
:eerrbad.

:MSGSYM. ERR_BITFIELD_NO_BASE_TYPE
:MSGTXT. bit-field declaration must have a base type specified
:MSGJTXT. �r�b�g�t�B�[���h�錾�͊�{�^���w�肵�Ȃ���΂Ȃ�܂���
A bit-field cannot make use of a default integer type.  Specify the type
.kw int
to correct the code.
:errbad.
class C
{
public:
    bitf :10;
};
:eerrbad.

:MSGSYM. ERR_BITFIELD_QUALIFIER
:MSGTXT. illegal qualification of a bit-field declaration
:MSGJTXT. �r�b�g�t�B�[���h�錾�̈�@�ȏC���q�ł�
A bit-field can only be declared
.kw const
or
.kw volatile.
Qualifications like
.kw friend
are not allowed.
:errbad.
struct S {
    friend int bit1 :10;
    inline int bit2 :10;
    virtual int bit3 :10;
};
:eerrbad.
All three declarations of bit-fields are illegal.

:MSGSYM. ERR_REPEATED_BASE_QUALIFIERS
:MSGTXT. duplicate base qualifier
:MSGJTXT. ��{�C���q���x�J��Ԃ��Ă��܂�
The compiler has found a repetition of base qualifiers like
.kw protected
or
.kw virtual.
:errbad.
struct Base { int b; };
struct Derived : public public Base { int d; };
:eerrbad.

:MSGSYM. ERR_TOO_MANY_ACCESS_SPECIFIERS
:MSGTXT. only one access specifier is allowed
:MSGJTXT. �A�N�Z�X�w��q��1����������܂�
The compiler has found more than one access specifier for a base class.
Since the compiler cannot choose one over the other, remove the unwanted
access specifier to correct the code.
:errbad.
struct Base { int b; };
struct Derived : public protected Base { int d; };
:eerrbad.

:MSGSYM. ANSI_TYPE_SPECIFIER_QUALIFIER
:MSGTXT. unexpected type qualifier found
:MSGJTXT. �Ԉ�����^�C���q������܂�
:ANSI. 1
Type specifiers cannot have
.kw const
or
.kw volatile
qualifiers.  This shows up in
.kw new
expressions because one cannot allocate a
.kw const
object.

:MSGSYM. ANSI_TYPE_SPECIFIER_STGCLASS
:MSGTXT. unexpected storage class specifier found
:MSGJTXT. �Ԉ�����L���N���X�w��q������܂�
:ANSI. 1
Type specifiers cannot have
.kw auto
or
.kw static
storage class specifiers.  This shows up in
.kw new
expressions because one cannot allocate a
.kw static
object.

:MSGSYM. ERR_AMBIGUOUS_MEMBER
:MSGTXT. access to '%S' is not allowed because it is ambiguous
:MSGJTXT. '%S'�ւ̃A�N�Z�X�́C�B�����̂��ߋ�����܂���
There are two ways that this error can show up in C++ code.
The first way a member can be ambiguous is that the same name
can be used in two different classes.  If these classes are
combined with multiple inheritance, accesses of the name
will be ambiguous.
:errbad.
struct S1 { int s; };
struct S2 { int s; };
struct Der : public S1, public S2
{
    void foo() { s = 2; };  // s is ambiguous
};
:eerrbad.
The second way a member can be ambiguous involves multiple inheritance.
If a class is inherited non-virtually by two different classes which
then get combined with multiple inheritance, an access of the member
is faced with deciding which copy of the member is intended.
Use the '::' operator to clarify what
member is being accessed or access the member with a different
class pointer or reference.
:errbad.
struct Top { int t; };
struct Mid : public Top { int m; };
struct Bot : public Top, public Mid
{
    void foo() { t = 2; };  // t is ambiguous
};
:eerrbad.

:MSGSYM. ERR_PRIVATE_MEMBER
:MSGTXT. access to private member '%S' is not allowed
:MSGJTXT. �v���C�x�[�g�E�����o'%S'�ւ̃A�N�Z�X�͋�����܂���
The indicated member is being accessed by an expression that does not
have permission to access private members of the class.
:errbad.
struct Top { int t; };
class Bot : private Top
{
    int foo() { return t; };  // t is private
};
Bot b;
int k = b.foo();    // foo is private
:eerrbad.

:MSGSYM. ERR_PROTECTED_MEMBER
:MSGTXT. access to protected member '%S' is not allowed
:MSGJTXT. �v���e�N�g�E�����o'%S'�ւ̃A�N�Z�X�͋�����܂���
The indicated member is being accessed by an expression that does not
have permission to access protected members of the class.
The compiler also requires that
.kw protected
members be accessed through a derived class to ensure that
an unrelated base class cannot be quietly modified.
This is a fairly recent change to the C++ language that may
cause Open Watcom C++ to not accept older C++ code.
See Section 11.5 in the ARM for a discussion of protected access.
:errbad.
struct Top { int t; };
struct Mid : public Top { int m; };
class Bot : protected Mid
{
protected:
    // t cannot be accessed
    int foo() { return t; };
};
Bot b;
int k = b.foo(); // foo is protected
:eerrbad.

:MSGSYM. ERR_BOTH_PTRS
:MSGTXT. operation does not allow both operands to be pointers
:MSGJTXT. �����̃I�y�����h���|�C���^�ł��鉉�Z�͂ł��܂���
There may be a missing indirection in the code exhibiting this error.
An example of this error is adding two pointers.
:errbad.
void fn()
{
    char *p, *q;

    p += q;
}
:eerrbad.

:MSGSYM. ERR_NOT_PTR_ARITH
:MSGTXT. operand is neither a pointer nor an arithmetic type
:MSGJTXT. �I�y�����h�̓|�C���^�ł��Z�p�^�ł�����܂���
An example of this error is incrementing a class that does not
have any overloaded operators.
:errbad.
struct S { } x;
void fn()
{
    ++x;
}
:eerrbad.

:MSGSYM. ERR_LEFT_NOT_PTR_ARITH
:MSGTXT. left operand is neither a pointer nor an arithmetic type
:MSGJTXT. ���I�y�����h�̓|�C���^�ł��Z�p�^�ł�����܂���
An example of this error is trying to add 1 to a class that does not
have any overloaded operators.
:errbad.
struct S { } x;
void fn()
{
    x = x + 1;
}
:eerrbad.

:MSGSYM. ERR_RIGHT_NOT_PTR_ARITH
:MSGTXT. right operand is neither a pointer nor an arithmetic type
:MSGJTXT. �E�I�y�����h�̓|�C���^�ł��Z�p�^�ł�����܂���
An example of this error is trying to add 1 to a class that does not
have any overloaded operators.
:errbad.
struct S { } x;
void fn()
{
    x = 1 + x;
}
:eerrbad.

:MSGSYM. ERR_SUB_PTR_FROM_ARITH
:MSGTXT. cannot subtract a pointer from an arithmetic operand
:MSGJTXT. �|�C���^���Z�p�I�y�����h����������Ƃ͂ł��܂���
The subtract operands are probably in the wrong order.
:errbad.
int fn( char *p )
{
    return( 10 - p );
}
:eerrbad.

:MSGSYM. ERR_LEFT_EXPR_MUST_BE_ARITHMETIC
:MSGTXT. left expression must be arithmetic
:MSGJTXT. ���̎��͎Z�p���łȂ���΂Ȃ�܂���
Certain operations like multiplication require both operands to be
of arithmetic types.
:errbad.
struct S { } x;
void fn()
{
    x = x * 1;
}
:eerrbad.

:MSGSYM. ERR_RIGHT_EXPR_MUST_BE_ARITHMETIC
:MSGTXT. right expression must be arithmetic
:MSGJTXT. �E�̎��͎Z�p���łȂ���΂Ȃ�܂���
Certain operations like multiplication require both operands to be
of arithmetic types.
:errbad.
struct S { } x;
void fn()
{
    x = 1 * x;
}
:eerrbad.

:MSGSYM. ERR_LEFT_EXPR_MUST_BE_INTEGRAL
:MSGTXT. left expression must be integral
:MSGJTXT. ���̎��͐����łȂ���΂Ȃ�܂���
Certain operators like the bit manipulation operators require both operands
to be of integral types.
:errbad.
struct S { } x;
void fn()
{
    x = x ^ 1;
}
:eerrbad.

:MSGSYM. ERR_RIGHT_EXPR_MUST_BE_INTEGRAL
:MSGTXT. right expression must be integral
:MSGJTXT. �E�̎��͐����łȂ���΂Ȃ�܂���
Certain operators like the bit manipulation operators require both operands
to be of integral types.
:errbad.
struct S { } x;
void fn()
{
    x = 1 ^ x;
}
:eerrbad.

:MSGSYM. ERR_PTR_TO_ARITH_ASSIGNMENT
:MSGTXT. cannot assign a pointer value to an arithmetic item
:MSGJTXT. �Z�p�ϐ��Ƀ|�C���^�l�����ł��܂���
The pointer value must be cast to the desired type before the assignment
takes place.
:errbad.
void fn( char *p )
{
    int a;

    a = p;
}
:eerrbad.

:MSGSYM. ERR_DTOR_OBJ_MEM_MODEL
:MSGTXT. attempt to destroy a far object when data model is near
:MSGJTXT. �f�[�^���f����near�ł���Ƃ��Cfar�I�u�W�F�N�g�Ƀf�X�g���N�^��K�p���Ă��܂�
Destructors cannot be applied to objects which are stored in far memory
when the default memory model for data is near.
:errbad.
struct Obj
{   char *p;
    ~Obj();
};

Obj far obj;
:eerrbad.
.np
The last line causes this error to be displayed when the memory model is
small (switch -ms), since the memory model for data is near.

:MSGSYM. ERR_THIS_OBJ_MEM_MODEL
:MSGTXT. attempt to call member function for far object when the data model is near
:MSGJTXT. �f�[�^���f����near�ł���Ƃ��Cfar�I�u�W�F�N�g�̃����o�֐����Ă�ł��܂�
Member functions cannot be called for objects which are stored in
far memory when the default memory model for data is near.
:errbad.
struct Obj
{   char *p;
    int foo();
};

Obj far obj;
int integer = obj.foo();
:eerrbad.
.np
The last line causes this error to be displayed when the memory model is
small (switch -ms), since the memory model for data is near.

:MSGSYM. ERR_NO_TYPE_DEFAULTS
:MSGTXT. template type argument cannot have a default argument
:MSGJTXT. �e���v���[�g�^�����̓f�t�H���g�����������Ƃ��ł��܂���
This message was produced by earlier versions of the Open Watcom C++
compiler. Support for default template arguments was added in version
1.3 and this message was removed at that time.

:MSGSYM. ERR_DLT_OBJ_MEM_MODEL
:MSGTXT. attempt to delete a far object when the data model is near
:MSGJTXT. ���̃f�[�^���f����near�ł���Ƃ��Cfar�I�u�W�F�N�g��'delete'���Ă��܂�
.kw delete
cannot be used to deallocate objects which are stored in
far memory when the default memory model for data is near.
:errbad.
struct Obj
{   char *p;
};

void foo( Obj far *p )
{
    delete p;
}
:eerrbad.
.np
The second last line causes this error to be displayed when the memory model
is small (switch -ms), since the memory model for data is near.

:MSGSYM. ERR_OFFSETOF_CLASS
:MSGTXT. first operand is not a class, struct or union
:MSGJTXT. ��P�I�y�����h��class, struct, �܂���union�ł���܂���
The
.kw offsetof
operation can only be performed on a type that can
have members.  It is meaningless for any other type.
:errbad.
#include <stddef.h>

int fn( void )
{
    return offsetof( double, sign );
}
:eerrbad.

:MSGSYM. ERR_CLASS_TEMPLATE_REWRITE_ERROR
:MSGTXT. syntax error: class template cannot be processed
:MSGJTXT. �\���G���[�G�N���X�E�e���v���[�g�������ł��܂���
The class template contains unbalanced braces.  The class definition
cannot be processed in this form.

:MSGSYM. ERR_PTR_CONVERSION
:MSGTXT. cannot convert right pointer to type of left operand
:MSGJTXT. ���I�y�����h�̌^�ɉE�̃|�C���^��ϊ����邱�Ƃ��ł��܂���
The C++ language will not allow the implicit conversion of
unrelated class pointers.  An explicit cast is required.
:errbad.
class C1;
class C2;

void fun( C1* pc1, C2* pc2 )
{
    pc2 = pc1;
}
:eerrbad.

:MSGSYM. ERR_LEFT_MUST_BE_LVALUE
:MSGTXT. left operand must be an lvalue
:MSGJTXT. ���I�y�����h��'���Ӓl'�łȂ���΂Ȃ�܂���
The left operand must be an expression that is valid
on the left side of an assignment.
Examples of incorrect lvalues include constants and
the results of most operators.
:errbad.
int i, j;
void fn()
{
    ( i - 1 ) = j;
    1 = j;
}
:eerrbad.

:MSGSYM. ERR_UNION_NO_STATIC_MEMBERS
:MSGTXT. static data members are not allowed in an union
:MSGJTXT. �X�^�e�B�b�N�E�f�[�^�E�����o�͋��p�̂̒��ŋ�����܂���
A union should only be used to organize memory in C++.  Enclose
the union in a class if you need a static data member associated with
the union.
:errbad.
union U
{
    static int a;
    int b;
    int c;
};
:eerrbad.

:MSGSYM. ERR_INVALID_STG_CLASS_FOR_MEMBER
:MSGTXT. invalid storage class for a member
:MSGJTXT. �����o�ɑ΂��ĕs�K�؂ȋL���N���X�ł�
A class member cannot be declared with
.kw auto,
.kw register,
or
.kw extern
storage class.
:errbad.
class C
{
    auto int a;     // cannot specify auto
};
:eerrbad.

:MSGSYM. ERR_COMPLICATED_DECLARATION
:MSGTXT. declaration is too complicated
:MSGJTXT. �錾�����G�����܂�
The declaration contains too many declarators (i.e., pointer, array, and
function types).  Break up the declaration into a series of typedefs
ending in a final declaration.
:errgood.
int ************p;
:eerrgood.
:errgood.
// transform this to ...
typedef int ****PD1;
typedef PD1 ****PD2;
PD2 ****p;
:eerrgood.

:MSGSYM. ERR_COMPLICATED_EXCEPTION
:MSGTXT. exception declaration is too complicated
:MSGJTXT. ��O�錾�����G�����܂�
The exception declaration contains too many declarators (i.e., pointer,
array, and function types).  Break up the declaration into a series
of typedefs ending in a final declaration.

:MSGSYM. ERR_FLOATING_CONSTANT_OVERFLOW
:MSGTXT. floating-point constant too large to represent
:MSGJTXT. ���������_�萔���傫�����܂�
The Open Watcom C++ compiler cannot represent the floating-point
constant because the magnitude of the positive exponent is too large.
:errbad.
float f = 1.2e78965;
:eerrbad.

:MSGSYM. ERR_FLOATING_CONSTANT_UNDERFLOW
:MSGTXT. floating-point constant too small to represent
:MSGJTXT. ���������_�萔�����������܂�
The Open Watcom C++ compiler cannot represent the floating-point
constant because the magnitude of the negative exponent is too large.
:errbad.
float f = 1.2e-78965;
:eerrbad.

:MSGSYM. ERR_CANT_OVERLOAD_CLASS_TEMPLATES
:MSGTXT. class template '%M' cannot be overloaded
:MSGJTXT. �N���X�E�e���v���[�g'%M'�̓I�[�o�[���[�h�ł��܂���
A class template name must be unique across the entire C++ program.
Furthermore, a class template cannot coexist with another
class template of the same name.

:MSGSYM. ERR_NO_ENUM_TYPE_POSSIBLE
:MSGTXT. range of enum constants cannot be represented
:MSGJTXT. enum�萔�͕\���\�Ȕ͈͂𒴂��Ă��܂�
If one integral type cannot be chosen to represent all
values of an enumeration, the values cannot be used
reliably in the generated code.  Shrink the range of
enumerator values used in the
.kw enum
declaration.
:errbad.
enum E
{   e1 = 0xFFFFFFFF
,   e2 = -1
};
:eerrbad.

:MSGSYM. ERR_NAME_USED_BY_NON_CLASS_TEMPLATE
:MSGTXT. '%S' cannot be in the same scope as a class template
:MSGJTXT. '%S'���N���X�E�e���v���[�g�Ɠ����X�R�[�v�ɂ��邱�Ƃ��ł��܂���
A class template name must be unique across the entire C++ program.
Any other use of a name cannot be in the same scope as the class template.

:MSGSYM. ERR_INVALID_STG_CLASS_FOR_FILE_SCOPE
:MSGTXT. invalid storage class in file scope
:MSGJTXT. �t�@�C���E�X�R�[�v�ɕs�K�؂ȋL���N���X������܂�
A declaration in file scope cannot have a storage class of
.kw auto
or
.kw register.
:errbad.
auto int a;
:eerrbad.

:MSGSYM. ERR_CONST_MUST_BE_INITIALIZED
:MSGTXT. const object must be initialized
:MSGJTXT. Const�I�u�W�F�N�g�́C����������Ȃ���΂Ȃ�܂���
Constant objects cannot be modified so they must be initialized before use.
:errbad.
const int a;
:eerrbad.

:MSGSYM. ERR_NAME_USED_BY_CLASS_TEMPLATE
:MSGTXT. declaration cannot be in the same scope as class template '%S'
:MSGJTXT. �錾�̓N���X�E�e���v���[�g'%S'�Ɠ����X�R�[�v�ɂ��邱�Ƃ��ł��܂���
A class template name must be unique across the entire C++ program.
Any other use of a name cannot be in the same scope as the class template.

:MSGSYM. ERR_NO_UNNAMED_TEMPLATE_ARGS
:MSGTXT. template arguments must be named
:MSGJTXT. �e���v���[�g�����ɂ͖��O�����Ȃ���΂Ȃ�܂���
A member function of a template class cannot be defined outside
the class declaration unless all template arguments have been named.

:MSGSYM. ERR_CANT_REDEFINE_CLASS_TEMPLATES
:MSGTXT. class template '%M' is already defined
:MSGJTXT. �N���X�E�e���v���[�g'%M'�͊��ɒ�`����Ă��܂�
A class template cannot have its definition repeated regardless of whether
it is identical to the previous definition.

:MSGSYM. ERR_INVALID_STG_CLASS_FOR_PARM
:MSGTXT. invalid storage class for an argument
:MSGJTXT. �����ɑ΂��ĕs�K�؂ȋL���N���X�ł�
An argument declaration cannot have a storage class of
.kw extern,
.kw static,
or
.kw typedef.
:errbad.
int foo( extern int a )
{
    return a;
}
:eerrbad.

:MSGSYM. ERR_UNION_CANT_HAVE_MEMBER_WITH_CTOR
:MSGTXT. unions cannot have members with constructors
:MSGJTXT. ���p�̂̓R���X�g���N�^���������o��L���邱�Ƃ��ł��܂���
A union should only be used to organize memory in C++.
Allowing union members to have constructors would mean
that the same piece of memory could be constructed twice.
:errbad.
class C
{
    C();
};
union U
{
    int a;
    C c;    // has constructor
};
:eerrbad.

:MSGSYM. ERR_COMPLICATED_STATEMENT
:MSGTXT. statement is too complicated
:MSGJTXT. �������G�����܂�
The statement contains too many nested constructs.
Break up the statement into multiple statements.

:MSGSYM. ERR_NAME_NOT_A_CLASS_OR_NAMESPACE
:MSGTXT. '%s' is not the name of a class or namespace
:MSGJTXT. '%s'�̓N���X�܂��͖��O��Ԃ̖��O�ł͂���܂���
The right hand operand of a '::' operator turned out not to reference
a class type or namespace.  Because the name is followed by another '::',
it must name a class or namespace.

:MSGSYM. ERR_MODIFY_CONSTANT
:MSGTXT. attempt to modify a constant value
:MSGJTXT. �萔�l��ύX���悤�Ƃ��Ă��܂�
Modification of a constant value is not allowed.  If you must force
this to work, take the address and cast away the constant nature
of the type.
:errbad.
static int const con = 12;
void foo()
{
    con = 13;       // error
    *(int*)&con = 13;   // ok
}
:eerrbad.

:MSGSYM. ERR_OFFSETOF_BITFIELD
:MSGTXT. 'offsetof' is not allowed for a bit-field
:MSGJTXT. 'offsetof'�̓r�b�g�t�B�[���h�Ɏg���܂���
A bit-field cannot have a simple offset so it cannot be referenced in an
.kw offsetof
expression.
:errbad.
#include <stddef.h>
struct S
{
    unsigned b1 :10;
    unsigned b2 :15;
    unsigned b3 :11;
};
int k = offsetof( S, b2 );
:eerrbad.

:MSGSYM. WARN_PRIVATE_BASE_ASSUMED
:MSGTXT. base class is inherited with private access
:MSGJTXT. ���N���X�̓v���C�x�[�g�E�A�N�Z�X�Ōp������Ă��܂�
:WARNING. 1
This warning indicates that the base class was originally declared
as a
.kw class
as opposed to a
.kw struct.
Furthermore, no access was specified so the base class defaults to
.kw private
inheritance.
Add the
.kw private
or
.kw public
access specifier to prevent this message depending on the intended access.

:MSGSYM. ERR_FUNCTION_NO_OVERLOAD
:MSGTXT. overloaded function cannot be selected for arguments used in call
:MSGJTXT. �Ăяo���̒��Ŏg��������ɂ���ăI�[�o�[���[�h�֐�������ł��܂���
Either conversions were not possible for an argument to the function or
a function with the right number of arguments was not available.
:errbad.
class C1;
class C2;
int foo( C1* );
int foo( C2* );
int k = foo( 5 );
:eerrbad.

:MSGSYM. ERR_SEGOP_OPERANDS
:MSGTXT. base operator operands must be " __segment :> pointer "
:MSGJTXT. �x�[�X���Z�q�̃I�y�����h��"__segment�F>�|�C���^"�łȂ���΂Ȃ�܂���
The base operator (:>) requires the left operand to be of type __segment
and the right operand to be a pointer.
:errbad.
char _based( void ) *pcb;
char __far *pcf = pcb;      // needs :> operator
:eerrbad.
Examples of typical uses are as follows:
:errgood.
const __segment mySegAbs = 0x4000;
char __based( void ) *c_bv = 24;
char __far *c_fp_1 = mySegAbs :> c_bv;
char __far *c_fp_2 = __segname( "_DATA" ) :> c_bv;
:eerrgood.

:MSGSYM. ERR_NOT_PTR_OR_ZERO
:MSGTXT. expression must be a pointer or a zero constant
:MSGJTXT. ���̓|�C���^�܂��̓[���萔�łȂ���΂Ȃ�܂���
In a conditional expression, if one side of the ':' is a pointer then
the other side must also be a pointer or a zero constant.
:errbad.
extern int a;
int *p = ( a > 7 ) ? &a : 12;
:eerrbad.

:MSGSYM. ERR_PTR_SCALES_LEFT
:MSGTXT. left expression pointer type cannot be incremented or decremented
:MSGJTXT. �����̃|�C���^�̓C���N�������g�܂��̓f�B�N�������g�ł��܂���
The expression requires that the scaling size of the pointer be known.
Pointers to
functions,
arrays of unknown size, or
.kw void
cannot be incremented because there is no size defined for
functions,
arrays of unknown size, or
.kw void.
:errbad.
void *p;
void *q = p + 2;
:eerrbad.

:MSGSYM. ERR_PTR_SCALES_RIGHT
:MSGTXT. right expression pointer type cannot be incremented or decremented
:MSGJTXT. �E���̃|�C���^�̓C���N�������g�܂��̓f�B�N�������g�ł��܂���
The expression requires that the scaling size of the pointer be known.
Pointers to
functions,
arrays of unknown size, or
.kw void
cannot be incremented because there is no size defined for
functions,
arrays of unknown size, or
.kw void.
:errbad.
void *p;
void *q = 2 + p;
:eerrbad.

:MSGSYM. ERR_PTR_SCALES
:MSGTXT. expression pointer type cannot be incremented or decremented
:MSGJTXT. ���̃|�C���^�̓C���N�������g�܂��̓f�B�N�������g�ł��܂���
The expression requires that the scaling size of the pointer be known.
Pointers to
functions,
arrays of unknown size, or
.kw void
cannot be incremented because there is no size defined for
functions,
arrays of unknown size, or
.kw void.
:errbad.
void *p;
void *q = ++p;
:eerrbad.

:MSGSYM. ERR_CANT_TAKE_SIZEOF_FUNC
:MSGTXT. 'sizeof' is not allowed for a function
:MSGJTXT. 'sizeof'�͊֐��ɑ΂��Ďg�p�ł��܂���
A function has no size defined for it by the C++ language specification.
:errbad.
typedef int FT( int );

unsigned y = sizeof( FT );
:eerrbad.

:MSGSYM. ERR_CANT_TAKE_SIZEOF_VOID
:MSGTXT. 'sizeof' is not allowed for type void
:MSGJTXT. 'sizeof'��'void'�^�ɑ΂��Ďg�p�ł��܂���
The type
.kw void
has no size defined for it by the C++ language specification.
:errbad.
void *p;
unsigned size = sizeof( *p );
:eerrbad.

:MSGSYM. ERR_TYPE_SPECIFIER_DEFINES
:MSGTXT. type cannot be defined in this context
:MSGJTXT. �����Ō^���`���邱�Ƃ͂ł��܂���
A type cannot be defined in certain contexts.  For example,
a new type cannot be defined in an argument list, a
.kw new
expression, a conversion function identifier, or a catch handler.
:errbad.
extern int goop();
int foo()
{
    try {
    return goop();
    } catch( struct S { int s; } ) {
    return 2;
    }
}
:eerrbad.

:MSGSYM. ERR_INVALID_TEMPLATE_PARM
:MSGTXT. expression cannot be used as a class template parameter
:MSGJTXT. ���̓N���X�E�e���v���[�g�E�p�����[�^�Ƃ��Ďg���܂���
The compiler has to be able to compare expressions during
compilation so this limits the complexity of expressions
that can be used for template parameters.
The only types of expressions that can be used for template parameters
are constant integral expressions and addresses.
Any symbols must have external linkage or must be static class members.

:MSGSYM. ERR_PREMATURE_ENDFILE
:MSGTXT. premature end-of-file encountered during compilation
:MSGJTXT. �R���p�C���̓r���Ńt�@�C�����I���܂���
The compiler expects more source code at this point.  This can be due
to missing parentheses (')') or missing closing braces ('}').

:MSGSYM. ERR_DUPLICATE_CONV_CASE_VALUE
:MSGTXT. duplicate case value '%s' after conversion to type of switch expression
:MSGJTXT. switch���Ō^�ϊ����s������C����case�l'%s'��2���݂��܂�
A duplicate
.kw case
value has been found.  Keep in mind that all case values must be converted
to the type of the switch expression.  Constants that may be different
initially may convert to the same value.
:errbad.
enum E { e1, e2 };
void foo( short a )
{
    switch( a ) {
      case 1:
      case 0x10001:    // converts to 1 as short
    break;
    }
}
:eerrbad.

:MSGSYM. WARN_DCL_IF
:MSGTXT. declaration statement follows an if statement
:MSGJTXT. �錾���̑O��if��������܂�
:WARNING. 1
:cmt was illegal in the ARM but clarified in X3J16 and it is now legal
There are implicit scopes created for most control structures.
Because of this, no code can access any of the names declared
in the declaration.
Although the code is legal it may not be
what the programmer intended.
:errbad.
void foo( int a )
{
    if( a )
    int b = 14;
}
:eerrbad.

:MSGSYM. WARN_DCL_ELSE
:MSGTXT. declaration statement follows an else statement
:MSGJTXT. �錾���̑O��else��������܂�
:WARNING. 1
:cmt was illegal in the ARM but clarified in X3J16 and it is now legal
There are implicit scopes created for most control structures.
Because of this, no code can access any of the names declared
in the declaration.
Although the code is legal it may not be
what the programmer intended.
:errbad.
void foo( int a )
{
    if( a )
    int c = 15;
    else
    int b = 14;
}
:eerrbad.

:MSGSYM. WARN_DCL_SWITCH
:MSGTXT. declaration statement follows a switch statement
:MSGJTXT. �錾���̑O��switch��������܂�
:WARNING. 1
:cmt was illegal in the ARM but clarified in X3J16 and it is now legal
There are implicit scopes created for most control structures.
Because of this, no code can access any of the names declared
in the declaration.
Although the code is legal it may not be
what the programmer intended.
:errbad.
void foo( int a )
{
    switch( a )
    int b = 14;
}
:eerrbad.

:MSGSYM. ERR_NO_THIS_PTR_DEFINED
:MSGTXT. 'this' pointer is not defined
:MSGJTXT. 'this'�|�C���^�͒�`����܂���
The
.kw this
value can only be used from within non-static member functions.
:errbad.
void *fn()
{
    return this;
}
:eerrbad.

:MSGSYM. WARN_DCL_WHILE
:MSGTXT. declaration statement cannot follow a while statement
:MSGJTXT. �錾����while���ɑ������Ƃ��ł��܂���
:WARNING. 1
:cmt was illegal in the ARM but clarified in X3J16 and it is now legal
There are implicit scopes created for most control structures.
Because of this, no code can access any of the names declared
in the declaration.
Although the code is legal it may not be
what the programmer intended.
:errbad.
void foo( int a )
{
    while( a )
    int b = 14;
}
:eerrbad.

:MSGSYM. WARN_DCL_DO
:MSGTXT. declaration statement cannot follow a do statement
:MSGJTXT. �錾����do���ɑ������Ƃ��ł��܂���
:WARNING. 1
:cmt was illegal in the ARM but clarified in X3J16 and it is now legal
There are implicit scopes created for most control structures.
Because of this, no code can access any of the names declared
in the declaration.
Although the code is legal it may not be
what the programmer intended.
:errbad.
void foo( int a )
{
    do
    int b = 14;
    while( a );
}
:eerrbad.

:MSGSYM. WARN_DCL_FOR
:MSGTXT. declaration statement cannot follow a for statement
:MSGJTXT. �錾����for���ɑ������Ƃ��ł��܂���
:WARNING. 1
:cmt was illegal in the ARM but clarified in X3J16 and it is now legal
There are implicit scopes created for most control structures.
Because of this, no code can access any of the names declared
in the declaration.
Although the code is legal it may not be
what the programmer intended.
A
.kw for
loop with an initial declaration is allowed to be used within another
.kw for
loop, so this code is legal C++:
:errgood.
void fn( int **a )
{
    for( int i = 0; i < 10; ++i )
    for( int j = 0; j < 10; ++j )
        a[i][j] = i + j;
}
:eerrgood.
The following example, however, illustrates a potentially erroneous situation.
:errbad.
void foo( int a )
{
    for( ; a<10; )
    int b = 14;
}
:eerrbad.

:MSGSYM. ERR_CONVERT_FROM_VIRTUAL_BASE
:MSGTXT. pointer to virtual base class converted to pointer to derived class
:MSGJTXT. ���z���N���X�ւ̃|�C���^�́C�h���N���X�ւ̃|�C���^�ɕϊ�����܂���
Since the relative position of a virtual base can change through repeated
derivations, this conversion is very dangerous.  All C++ translators must
report an error for this type of conversion.
:errbad.
struct VBase { int v; };
struct Der : virtual public VBase { int   d; };
extern VBase *pv;
Der *pd = (Der *)pv;
:eerrbad.

:MSGSYM. ERR_USE_FAR
:MSGTXT. cannot use far pointer in this context
:MSGJTXT. ������far�|�C���^���g�����Ƃ͂ł��܂���
Only near pointers can be thrown when the data memory model is near.
:errbad.
extern int __far *p;
void foo()
{
    throw p;
}
:eerrbad.
When the small memory model (-ms switch) is selected, the
.kw throw
expression is diagnosed as erroneous.
Similarly, only near pointers can be specified in
.kw catch
statements when the data memory model is near.

:MSGSYM. ERR_RET_AUTO_REF
:MSGTXT. returning reference to function argument or to auto or register variable
:MSGJTXT. �֐������ւ̎Q�ƁC�܂��͎����^���W�X�^�ϐ��ւ̎Q�Ƃ�Ԃ��܂�
The storage for the automatic variable will be destroyed immediately upon
function return.  Returning a reference effectively allows the caller
to modify storage which does not exist.
:errbad.
class C
{
    char *p;
public:
    C();
    ~C();
};

C& foo()
{
    C auto_var;
    return auto_var;    // not allowed
}
:eerrbad.

:MSGSYM. WARN_PRAGMA_MERGE
:MSGTXT. #pragma attributes for '%S' may be inconsistent
:MSGJTXT. '%S'�ɑ΂���#pragma�����͖������Ă��邩������܂���
:WARNING. 1
A pragma attribute was changed to a value which matches neither the current
default not the previous value for that attribute.
A warning is issued since this usually indicates an attribute is being
set twice (or more) in an inconsistent way.
The warning can also occur when the default attribute is changed between
two pragmas for the same object.

:MSGSYM. ERR_NO_VOID_PARMS
:MSGTXT. function arguments cannot be of type void
:MSGJTXT. �֐�������'void'�^�ł��邱�Ƃ��ł��܂���
Having more than one
.kw void
argument is not allowed.  The special case of one
.kw void
argument indicates that the function accepts no parameters.
:errbad.
void fn1( void )        // OK
{
}
void fn2( void, void, void )    // Error!
{
}
:eerrbad.

:MSGSYM. ERR_TOO_FEW_TEMPLATE_PARAMETERS
:MSGTXT. class template '%M' requires more parameters for instantiation
:MSGJTXT.
The class template instantiation has too few parameters supplied
so the class cannot be instantiated properly.

:MSGSYM. ERR_TOO_MANY_TEMPLATE_PARAMETERS
:MSGTXT. class template '%M' requires fewer parameters for instantiation
:MSGJTXT.
The class template instantiation has too many parameters supplied
so the class cannot be instantiated properly.

:MSGSYM. ERR_NEW_OVERLOAD_FAILURE
:MSGTXT. no declared 'operator new' has arguments that match
:MSGJTXT. �錾���ꂽ'operator new'�͈�v��������������܂���
An
.kw operator new
could not be found to match the
.kw new
expression.
Supply the correct arguments for special
.kw operator new
functions that are defined with the placement syntax.
:errgood.
#include <stddef.h>

struct S {
    void *operator new( size_t, char );
};

void fn()
{
    S *p = new ('a') S;
}
:eerrgood.

:MSGSYM. ERR_MISMATCHED_WIDE_STRING_CONCATENATION
:MSGTXT. wide character string concatenated with a simple character string
:MSGJTXT. ���C�h�L�����N�^������ɃV���O���E�L�����N�^�������A�����܂���
There are no semantics defined for combining a wide character string
with a simple character string.  To correct the problem, make the simple
character string a wide character string by prefixing it with a
.kw L.
:errbad.
char *p = "1234" L"5678";
:eerrbad.

:MSGSYM. ERR_OFFSETOF_STATIC
:MSGTXT. 'offsetof' is not allowed for a static member
:MSGJTXT. 'offsetof'�̓X�^�e�B�b�N�ȃ����o�Ɏg�p�ł��܂���
A
.kw static
member does not have an offset like simple data members.
If this is required, use the address of the
.kw static
member.
:errbad.
#include <stddef.h>
class C
{
public:
    static int stat;
    int memb;
};

int size_1 = offsetof( C, stat );   // not allowed
int size_2 = offsetof( C, memb );   // ok
:eerrbad.

:MSGSYM. ERR_CANT_HAVE_AN_ARRAY_OF_VOID
:MSGTXT. cannot define an array of void
:MSGJTXT. 'void'�̔z����`���邱�Ƃ͂ł��܂���
Since the
.kw void
type has no size and there are no values of
.kw void
type, one cannot declare an array of
.kw void.
:errbad.
void array[24];
:eerrbad.

:MSGSYM. ERR_CANT_HAVE_AN_ARRAY_OF_REFERENCES
:MSGTXT. cannot define an array of references
:MSGJTXT. �Q�Ƃ̔z����`���邱�Ƃ͂ł��܂���
References are not objects, they are simply a way of
creating an efficient alias to another name.
Creating an array of references is currently not
allowed in the C++ language.
:errbad.
int& array[24];
:eerrbad.

:MSGSYM. ERR_CANT_HAVE_REFERENCE_TO_VOID
:MSGTXT. cannot define a reference to void
:MSGJTXT. 'void'�̎Q�Ƃ��`���邱�Ƃ͂ł��܂���
One cannot create a reference to a
.kw void
because there can be no
.kw void
variables to supply for initializing the reference.
:errbad.
void& ref;
:eerrbad.

:MSGSYM. ERR_CANT_HAVE_REFERENCE_TO_REFERENCE
:MSGTXT. cannot define a reference to another reference
:MSGJTXT. ���̎Q�Ƃւ̎Q�Ƃ��`���邱�Ƃ͂ł��܂���
References are not objects, they are simply a way of
creating an efficient alias to another name.
Creating a reference to another reference is currently not
allowed in the C++ language.
:errbad.
int & & ref;
:eerrbad.

:MSGSYM. ERR_CANT_HAVE_POINTER_TO_REFERENCE
:MSGTXT. cannot define a pointer to a reference
:MSGJTXT. �Q�Ƃւ̃|�C���^���`���邱�Ƃ͂ł��܂���
References are not objects, they are simply a way of
creating an efficient alias to another name.
Creating a pointer to a reference is currently not
allowed in the C++ language.
:errbad.
char& *ptr;
:eerrbad.

:MSGSYM. ERR_CANT_INIT_NEW_ARRAY
:MSGTXT. cannot initialize array with 'operator new'
:MSGJTXT. 'operator new'�Ŕz������������邱�Ƃ͂ł��܂���
The initialization of arrays created with
.kw operator new
can only be done with default constructors.
The capability of using another constructor with arguments
is currently not allowed in the C++ language.
:errbad.
struct S
{
    S( int );
};
S *p = new S[10] ( 12 );
:eerrbad.

:MSGSYM. ERR_CANT_HAVE_VOID_VARIABLE
:MSGTXT. '%N' is a variable of type void
:MSGJTXT. '%N'��'void'�^�̕ϐ��ł�
A variable cannot be of type
.kw void.
The
.kw void
type can only be used in restricted circumstances because it has no size.
For instance, a function returning
.kw void
means that it does not return any value.
A pointer to
.kw void
is used as a generic pointer but it cannot be dereferenced.

:MSGSYM. ERR_CANT_HAVE_MEMBER_POINTER_TO_REFERENCE
:MSGTXT. cannot define a member pointer to a reference
:MSGJTXT. �Q�Ƃւ̃����o�E�|�C���^���`���邱�Ƃ͂ł��܂���
References are not objects, they are simply a way of
creating an efficient alias to another name.
Creating a member pointer to a reference is currently not
allowed in the C++ language.
:errbad.
struct S
{
    S();
    int &ref;
};

int& S::* p;
:eerrbad.

:MSGSYM. ERR_FUNCTION_NOT_DISTINCT
:MSGTXT. function '%S' is not distinct
:MSGJTXT. �֐�'%S'�͋�ʂ����܂���
The function being declared is not distinct enough from the
other functions of the same name.  This means that all function
overloads involving the function's argument types will be ambiguous.
:errbad.
struct S {
    int s;
};
extern int foo( S* );
extern int foo( S* const ); // not distinct enough
:eerrbad.

:MSGSYM. ERR_FUNCTION_AMBIGUOUS_OVERLOAD
:MSGTXT. overloaded function is ambiguous for arguments used in call
:MSGJTXT. �Ăяo���ɗp�����Ă�����������d��`�֐��ɑ΂��ĞB���ł�
The compiler could not find an unambiguous choice for
the function being called.
:errbad.
extern int foo( char );
extern int foo( short );
int k = foo( 4 );
:eerrbad.

:MSGSYM. ERR_NEW_OVERLOAD_AMBIGUOUS
:MSGTXT. declared 'operator new' is ambiguous for arguments used
:MSGJTXT. �g�p���ꂽ�����͐錾���ꂽ'operator new'�ɑ΂��ĞB���ł�
The compiler could not find an unambiguous choice for
.kw operator new.
:errbad.
#include <stdlib.h>
struct Der
{
    int s[2];
    void* operator new( size_t, char );
    void* operator new( size_t, short );
};
Der *p = new(10) Der;
:eerrbad.

:MSGSYM. ERR_FUNCTION_REDEFINITION
:MSGTXT. function '%S' has already been defined
:MSGJTXT. �֐�'%S'�͊��ɒ�`����܂���
The function being defined has already been defined elsewhere.
Even if the two function bodies are identical, there must be only
one definition for a particular function.
:errbad.
int foo( int s ) { return s; }
int foo( int s ) { return s; }  // illegal
:eerrbad.

:MSGSYM. ERR_ARRAY_LEFT
:MSGTXT. expression on left is an array
:MSGJTXT. �����͔z��ł�
The array expression is being used in a context where only pointers
are allowed.
:errbad.
void fn( void *p )
{
    int a[10];

    a = 0;
    a = p;
    a++;
}
:eerrbad.

:MSGSYM. ERR_USER_CONV_BAD_RETURN
:MSGTXT. user-defined conversion has a return type
:MSGJTXT. ���[�U�[��`�ϊ��͖߂�^�������܂�
A user-defined conversion cannot be declared with a return type.
The "return type" of the user-defined conversion is implicit in the
name of the user-defined conversion.
:errbad.
struct S {
    int operator int(); // cannot have return type
};
:eerrbad.

:MSGSYM. ERR_USER_CONV_BAD_DECL
:MSGTXT. user-defined conversion must be a function
:MSGJTXT. ���[�U�[��`�ϊ��͊֐��łȂ���΂Ȃ�܂���
The operator name describing a user-defined conversion can only be
used to designate functions.
:errbad.
// operator char can only be a function
int operator char = 9;
:eerrbad.

:MSGSYM. ERR_USER_CONV_BAD_FUNC
:MSGTXT. user-defined conversion has an argument list
:MSGJTXT. ���[�U�[��`�ϊ��͈������X�g�������܂�
A user-defined conversion cannot have an argument list.
Since user-defined conversions can only be non-static member
functions, they have an implicit
.kw this
argument.
:errbad.
struct S {
    operator int( S& ); // cannot have arguments
};
:eerrbad.

:MSGSYM. ERR_DESTRUCTOR_BAD_RETURN
:MSGTXT. destructor cannot have a return type
:MSGJTXT. �f�X�g���N�^�͖߂�l�����Ă܂���
A destructor cannot have a return type (even
.kw void
).
The destructor is a special member function that is not
required to be identical in form to all other member functions.
This allows different
implementations to have different uses for any return values.
:errbad.
struct S {
    void* ~S();
};
:eerrbad.

:MSGSYM. ERR_DESTRUCTOR_BAD_DECL
:MSGTXT. destructor must be a function
:MSGJTXT. �f�X�g���N�^�͊֐��łȂ���΂Ȃ�܂���
The tilde ('~') style of name is reserved for declaring destructor functions.
Variable names cannot make use of the destructor style of names.
:errbad.
struct S {
    int ~S; // illegal
};
:eerrbad.

:MSGSYM. ERR_DESTRUCTOR_BAD_FUNC
:MSGTXT. destructor has an argument list
:MSGJTXT. �f�X�g���N�^�͈������X�g�������܂�
A destructor cannot have an argument list.
Since destructors can only be non-static member
functions, they have an implicit
.kw this
argument.
:errbad.
struct S {
    ~S( S& );
};
:eerrbad.

:MSGSYM. ERR_OPERATOR_BAD_DECL
:MSGTXT. '%N' must be a function
:MSGJTXT. '%N'�͊֐��łȂ���΂Ȃ�܂���
The
.kw operator
style of name is reserved for declaring operator functions.
Variable names cannot make use of the
.kw operator
style of names.
:errbad.
struct S {
    int operator+;  // illegal
};
:eerrbad.

:MSGSYM. ERR_INCORRECT_FUNCTION_DECL
:MSGTXT. '%N' is not a function
:MSGJTXT. '%N'�͊֐��ł͂���܂���
The compiler has detected what looks like a function body.
The message is a result of not finding a function being declared.
This can happen in many ways, such as dropping the ':' before
defining base classes, or dropping the '=' before initializing
a structure via a braced initializer.
:errbad.
struct D B { int i; };
:eerrbad.

:MSGSYM. ERR_UNDECLARED_NESTED_CLASS_SYM
:MSGTXT. nested type class '%s' has not been declared
:MSGJTXT. �l�X�g�ɂ��ꂽclass '%s'�^�͐錾����Ă��܂���
A nested class has not been found but is required by the use of
repeated '::' operators.  The construct "A::B::C" requires that
'A' be a class type, and 'B' be a nested class within the scope
of 'A'.
:errbad.
struct B {
    static int b;
};
struct A : public B {
};
int A::B::b = 2;    // B not nested in A
:eerrbad.
The preceding example is illegal; the following is legal
:errgood.
struct A {
    struct B {
    static int b;
    };
};
int A::B::b = 2;    // B nested in A
:eerrgood.

:MSGSYM. ERR_UNDECLARED_ENUM_SYM
:MSGTXT. enum '%s' has not been declared
:MSGJTXT. enum '%s'�͐錾����Ă��܂���
An elaborated reference to an
.kw enum
could not be satisfied.  All enclosing scopes have been searched for an
.kw enum
name.  Visible variable declarations do not affect the search.
:errbad.
struct D {
    int i;
    enum E { e1, e2, e3 };
};
enum E enum_var;    // E not visible
:eerrbad.

:MSGSYM. ERR_UNDECLARED_CLASSNAMESPACE_SYM
:MSGTXT. class or namespace '%s' has not been declared
:MSGJTXT. '�N���X%s'�͐錾����Ă��܂���
The construct "A::B::C" requires that
'A' be a class type or a namespace, and 'B' be a nested class or
namespace within the scope of 'A'.
The reference to 'A' could not be satisfied.
All enclosing scopes have been searched for a
.kw class
or
.kw namespace
name.  Visible variable declarations do not affect the search.
:errbad.
struct A{ int a; };

int b;
int c = B::A::b;
:eerrbad.

:MSGSYM. ERR_ONE_CTOR_ARG_REQD
:MSGTXT. only one initializer argument allowed
:MSGJTXT. �X�J���[�^�ɑ΂��ẮC��̏���������������������܂�
The comma (',') in a function like cast is treated like
an argument list comma (',').
If a comma expression is desired, use parentheses to enclose the comma
expression.
:errbad.
void fn()
{
    int a;

    a = int( 1, 2 );        // Error!
    a = int( ( 1, 2 ) );    // OK
}
:eerrbad.

:MSGSYM. ERR_DEFAULT_ARGS_IN_A_TYPE
:MSGTXT. default arguments are not part of a function's type
:MSGJTXT. �f�t�H���g�����͊֐��̌^�̈ꕔ�ł͂���܂���
This message indicates that a declaration has been found that
requires default arguments to be part of a function's type.
Either declaring a function
.kw typedef
or a pointer to a function with default arguments are examples
of incorrect declarations.
:errbad.
typedef int TD( int, int a = 14 );
int (*p)( int, int a = 14 ) = 0;
:eerrbad.

:MSGSYM. ERR_DEFAULT_ARGS_MISSING
:MSGTXT. missing default arguments
:MSGJTXT. �f�t�H���g����������܂���
Gaps in a succession of default arguments are not allowed in the
C++ language.
:errbad.
void fn( int = 1, int, int = 3 );
:eerrbad.

:MSGSYM. ERR_DEFAULT_ARGS_OPERATOR
:MSGTXT. overloaded operator cannot have default arguments
:MSGJTXT. �I�[�o�[���[�h���Z�q�̓f�t�H���g�����������Ƃ��ł��܂���
Preventing overloaded operators from having default arguments
enforces the property that binary operators will only be called
from a use of a binary operator.  Allowing default arguments
would allow a binary
.kw operator +
to function as a unary
.kw operator +.
:errbad.
class C
{
public:
    C operator +( int a = 10 );
};
:eerrbad.

:MSGSYM. ERR_LEFT_NOT_CONST_PTR
:MSGTXT. left expression is not a pointer to a constant object
:MSGJTXT. �����͒萔�I�u�W�F�N�g�ւ̃|�C���^�ł͂���܂���
One cannot assign a pointer to a constant type to a pointer to a
non-constant type.  This would allow a constant object to be modified
via the non-constant pointer.  Use a cast if this is absolutely
necessary.
:errbad.
char* fun( const char* p )
{
    char* q;
    q = p;
    return q;
}
:eerrbad.

:MSGSYM. ERR_DEFAULT_ARG_REDEFINED
:MSGTXT. cannot redefine default argument for '%S'
:MSGJTXT. '%S'�ɑ΂���f�t�H���g�������Ē�`���邱�Ƃ��ł��܂���
Default arguments can only be defined once in a program regardless of
whether the value of the default argument is identical.
:errbad.
static int foo( int a = 10 );
static int foo( int a = 10 )
{
    return a+a;
}
:eerrbad.

:MSGSYM. WARN_DEFAULT_ARG_HITS_ANOTHER_SYM
:MSGTXT. using default arguments would be overload ambiguous with '%S'
:MSGJTXT. �f�t�H���g�����̎g�p�́C'%S'�ŞB���ȃI�[�o�[���[�h�ƂȂ�܂�
:WARNING. 3
The declaration declares enough default arguments that the function
is indistinguishable from another function of the same name.
:errbad.
void fn( int );
void fn( int, int = 1 );
:eerrbad.
.np
Calling the function 'fn' with one argument is ambiguous because
it could match either the first 'fn' without any default arguments or
the second 'fn' with a default argument applied.

:MSGSYM. WARN_DEFAULT_ARG_HITS_ANOTHER_ARG
:MSGTXT. using default arguments would be overload ambiguous with '%S' using default arguments
:MSGJTXT. �f�t�H���g�����̎g�p�́C�f�t�H���g�������g���Ă���'%S'�ŞB���ȃI�[�o�[���[�h�ƂȂ�܂�
:WARNING. 3
The declaration declares enough default arguments that the function
is indistinguishable from another function of the same name with default arguments.
:errbad.
void fn( int, int = 1 );
void fn( int, char = 'a' );
:eerrbad.
.np
Calling the function 'fn' with one argument is ambiguous because
it could match either the first 'fn' with a default argument or
the second 'fn' with a default argument applied.

:MSGSYM. ERR_DEFAULT_ARGS_MISSING_FOR_SYM
:MSGTXT. missing default argument for '%S'
:MSGJTXT. '%S'�ɑ΂���f�t�H���g����������܂���
In C++, one is allowed to add default arguments to the right hand arguments
of a function declaration in successive declarations.
The message indicates that the declaration is only valid if there
was a default argument previously declared for the next argument.
:errbad.
void fn1( int    , int     );
void fn1( int    , int = 3 );
void fn1( int = 2, int     );   // OK

void fn2( int    , int     );
void fn2( int = 2, int     );   // Error!
:eerrbad.

:MSGSYM. ERR_CANNOT_REFERENCE_UNNAMED_ENUM
:MSGTXT. enum references must have an identifier
:MSGJTXT. enum�Q�Ƃ͎��ʎq�������Ȃ���΂Ȃ�܂���
There is no way to reference an anonymous
.kw enum.
If all enums are named, the cause of this message is most likely
a missing identifier.
:errbad.
enum   { X, Y, Z }; // anonymous enum
void fn()
{
    enum *p;
}
:eerrbad.

:MSGSYM. ERR_INVALID_DESTRUCTOR_NAME
:MSGTXT. class declaration has not been seen for '~%s'
:MSGJTXT. '~%s'�ɑ΂���N���X�錾�͌�����܂���
A destructor has been used in a context where its class is
not visible.
:errbad.
class C;

void fun( C* p )
{
    p->~S();
}
:eerrbad.

:MSGSYM. ERR_CANNOT_USE_QUALIFIED_DECLARATOR
:MSGTXT. '::' qualifier cannot be used in this context
:MSGJTXT. '::'�C���q�͂����ł͎g���܂���
Qualified identifiers in a class context are allowed for declaring
.kw friend
member functions.
The Open Watcom C++ compiler also allows code that is qualified with its own
class so that declarations can be moved in and out of class definitions
easily.
:errbad.
struct N {
    void bar();
};
struct S {
    void S::foo() { // OK
    }
    void N::bar() { // error
    }
};
:eerrbad.

:MSGSYM. ERR_NOT_A_MEMBER
:MSGTXT. '%S' has not been declared as a member
:MSGJTXT. '%S'�̓����o�Ƃ��Đ錾����Ă��܂���
In a definition of a class member, the indicated declaration must already
have been declared when the class was defined.
:errbad.
class C
{
public:
    int c;
    int goop();
};
int C::x = 1;
C::not_decled() { }
:eerrbad.

:MSGSYM. ERR_DEFAULT_ARG_USES_ARG
:MSGTXT. default argument expression cannot use function argument '%S'
:MSGJTXT. �f�t�H���g�������́C�֐�����'%S'���g���܂���
Default arguments must be evaluated at each call.  Since the order
of evaluation for arguments is undefined, a compiler must diagnose
all default arguments that depend on other arguments.
:errbad.
void goop( int d )
{
    struct S {
    // cannot access "d"
    int foo( int c, int b = d )
        {
        return b + c;
        };
    };
}
:eerrbad.

:MSGSYM. ERR_DEFAULT_ARG_USES_LOCAL
:MSGTXT. default argument expression cannot use local variable '%S'
:MSGJTXT. �f�t�H���g�������́C�Ǐ��ϐ�'%S'���g���܂���
Default arguments must be evaluated at each call.  Since a local
variable is not always available in all contexts (e.g., file scope
initializers), a compiler must diagnose all default arguments that
depend on local variables.
:errbad.
void goop( void )
{
    int a;
    struct S {
    // cannot access "a"
    int foo( int c, int b = a )
        {
        return b + c;
        };
    };
}
:eerrbad.

:MSGSYM. ERR_ACCESS_DECL_IN_PRIVATE
:MSGTXT. access declarations may only be 'public' or 'protected'
:MSGJTXT. �A�N�Z�X�錾��'public'�܂���'protected'�݂̂ł�
Access declarations are used to increase access.  A
.kw private
access declaration is useless because there is no access level for which
.kw private
is an increase in access.
:errbad.
class Base
{
    int pri;
protected:
    int pro;
public:
    int pub;
};
class Derived : public Base
{
    private: Base::pri;
};
:eerrbad.

:MSGSYM. ERR_MIXING_FUNCTIONS_AND_VARS
:MSGTXT. cannot declare both a function and variable of the same name ('%N')
:MSGJTXT. �������O�i'%N'�j�̊֐��ƕϐ���錾���邱�Ƃ͂ł��܂���
Functions can be overloaded in C++ but they cannot be overloaded in
the presence of a variable of the same name.  Likewise, one cannot
declare a variable in the same scope as a set of overloaded functions
of the same name.
:errbad.
int foo();
int foo;
struct S {
    int bad();
    int bad;
};
:eerrbad.

:MSGSYM. ERR_NOT_A_DIRECT_BASE_CLASS
:MSGTXT. class in access declaration ('%T') must be a direct base class
:MSGJTXT. �A�N�Z�X�錾�i'%T'�j�̒��̃N���X�́C���ڂ̊��N���X�łȂ���΂Ȃ�܂���
Access declarations can only be applied to direct (immediate) base classes.
:errbad.
struct B {
    int f;
};
struct C : B {
    int g;
};
struct D : private C {
    B::f;
};
:eerrbad.
.np
In the above example, "C" is a direct base class of "D" and "B" is a direct
base class of "C", but "B" is not a direct base class of "D".

:MSGSYM. ERR_ACCESS_DECL_ALL_SAME
:MSGTXT. overloaded functions ('%N') do not have the same access
:MSGJTXT. �I�[�o�[���[�h�֐��i'%N'�j�͓����A�N�Z�X�������܂���
If an access declaration is referencing a set of overloaded functions,
then they all must have the same access.  This is due to the lack
of a type in an access declaration.
:errbad.
class C
{
    static int foo( int );     // private
public:
    static int foo( float );   // public
};

class B : private C
{
public: C::foo;
};
:eerrbad.

:MSGSYM. ERR_ACCESS_DECL_INCREASE
:MSGTXT. cannot grant access to '%N'
:MSGJTXT. '%N'�ɃA�N�Z�X��^���邱�Ƃ��ł��܂���
A derived class cannot change the access of a base class member
with an access declaration.  The access declaration can only be used
to restore access changed by inheritance.
:errbad.
class Base
{
public:
    int pub;
protected:
    int pro;
};
class Der : private Base
{
    public: Base::pub;       // ok
    public: Base::pro;       // changes access
};
:eerrbad.

:MSGSYM. ERR_ACCESS_DECL_DECREASE
:MSGTXT. cannot reduce access to '%N'
:MSGJTXT. '%N'�ւ̃A�N�Z�X��ύX���邱�Ƃ͂ł��܂���
A derived class cannot change the access of a base class member
with an access declaration.  The access declaration can only be used
to restore access changed by inheritance.
:errbad.
class Base
{
public:
    int pub;
protected:
    int pro;
};
class Der : public Base
{
    protected: Base::pub;   // changes access
    protected: Base::pro;   // ok
};
:eerrbad.

:MSGSYM. ERR_NESTED_CLASS_NOT_DEFINED
:MSGTXT. nested class '%N' has not been defined
:MSGJTXT. �l�X�g���ꂽ�N���X'%N'�͒�`����Ă��܂���
The current state of the C++ language supports nested types.
Unfortunately, this means that some working C code will not work
unchanged.
:errbad.
struct S {
    struct T;
    T *link;
};
:eerrbad.
.np
In the above example, the class "T" will be reported as not being defined
by the end of the class declaration.
The code can be corrected in the following manner.
:errgood.
struct S {
    struct T;
    T *link;
    struct T {
    };
};
:eerrgood.

:MSGSYM. ERR_USER_CONV_NOT_MEMBER
:MSGTXT. user-defined conversion must be a non-static member function
:MSGJTXT. ���[�U-��`�ϊ��́C��X�^�e�B�b�N�E�����o�֐��łȂ���΂Ȃ�܂���
A user-defined conversion is a special member function that allows the
class to be converted implicitly (or explicitly) to an arbitrary type.
In order to do this, it must have access to an instance of the class
so it is restricted to being a non-static member function.
:errbad.
struct S
{
    static operator int();
};
:eerrbad.

:MSGSYM. ERR_DESTRUCTOR_NOT_MEMBER
:MSGTXT. destructor must be a non-static member function
:MSGJTXT. �f�X�g���N�^�́C��X�^�e�B�b�N�E�����o�֐��łȂ���΂Ȃ�܂���
A destructor is a special member function that will perform cleanup on a
class before the storage for the class will be released.  In order to do
this, it must have access to an instance of the class so it is
restricted to being a non-static member function.
:errbad.
struct S
{
    static ~S();
};
:eerrbad.

:MSGSYM. ERR_OPERATOR_NOT_MEMBER
:MSGTXT. '%N' must be a non-static member function
:MSGJTXT. '%N'�͔�X�^�e�B�b�N�E�����o�֐��łȂ���΂Ȃ�܂���
The operator function in the message is restricted to being a non-static
member function.  This usually means that the operator function is treated
in a special manner by the compiler.
:errbad.
class C
{
public:
    static operator =( C&, int );
};
:eerrbad.

:MSGSYM. ERR_OPERATOR_MUST_HAVE_1_ARG
:MSGTXT. '%N' must have one argument
:MSGJTXT. '%N'��1�̈����������Ȃ���΂Ȃ�܂���
The operator function in the message is only allowed to have one
argument.  An operator like
.kw operator ~
is one such example because it represents a unary operator.
:errbad.
class C
{
public: int c;
};
C& operator~( const C&, int );
:eerrbad.

:MSGSYM. ERR_OPERATOR_MUST_HAVE_2_ARGS
:MSGTXT. '%N' must have two arguments
:MSGJTXT. '%N'��2�̈����������Ȃ���΂Ȃ�܂���
The operator function in the message must have two
arguments.  An operator like
.kw operator +=
is one such example because it represents a binary operator.
:errbad.
class C
{
public: int c;
};
C& operator += ( const C& );
:eerrbad.

:MSGSYM. ERR_OPERATOR_MUST_HAVE_1_OR_2_ARGS
:MSGTXT. '%N' must have either one argument or two arguments
:MSGJTXT. '%N'��1�܂���2�̈����������Ȃ���΂Ȃ�܂���
The operator function in the message must have either one argument or two
arguments.  An operator like
.kw operator +
is one such example because it represents either a unary or a binary operator.
:errbad.
class C
{
public: int c;
};
C& operator+( const C&, int, float );
:eerrbad.

:MSGSYM. ERR_OPERATOR_NEW_MUST_HAVE_ARGS
:MSGTXT. '%N' must have at least one argument
:MSGJTXT. '%N'�͏����Ƃ�1�̈����������Ȃ���΂Ȃ�܂���
The
.kw operator new
and
.kw operator new []
member functions must have at least one argument for the size of
the allocation.  After that, any arguments are up to the programmer.
The extra arguments can be supplied in a
.kw new
expression via the placement syntax.
:errgood.
#include <stddef.h>

struct S {
    void * operator new( size_t, char );
};

void fn()
{
    S *p = new ('a') S;
}
:eerrgood.

:MSGSYM. ERR_OPERATOR_DEL_RETURNS_VOID
:MSGTXT. '%N' must have a return type of void
:MSGJTXT. '%N��'void'�̖߂�^�������Ȃ���΂Ȃ�܂���
The C++ language requires that
.kw operator delete
and
.kw operator delete []
have a return type of
.kw void.
:errbad.
class C
{
public:
    int c;
    C* operator delete( void* );
    C* operator delete []( void* );
};
:eerrbad.

:MSGSYM. ERR_OPERATOR_NEW_RETURNS_VOID
:MSGTXT. '%N' must have a return type of pointer to void
:MSGJTXT. '%N��'void *'�̖߂�^�������Ȃ���΂Ȃ�܂���
The C++ language requires that both
.kw operator new
and
.kw operator new []
have a return type of
.id void *.
:errbad.
#include <stddef.h>
class C
{
public:
    int c;
    C* operator new( size_t size );
    C* operator new []( size_t size );
};
:eerrbad.

:MSGSYM. ERR_OPERATOR_NEW_FIRST_ARG
:MSGTXT. the first argument of '%N' must be of type size_t
:MSGJTXT. '%N'�̍ŏ��̈�����'size_t'�^�łȂ���΂Ȃ�܂���
The C++ language requires that the first argument for
.kw operator new
and
.kw operator new []
be of the type "size_t".  The definition for "size_t" can be included
by using the standard header file <stddef.h>.
:errbad.
void *operator new( int size );
void *operator new( double size, char c );
void *operator new []( int size );
void *operator new []( double size, char c );
:eerrbad.

:MSGSYM. ERR_OPERATOR_DEL_FIRST_ARG
:MSGTXT. the first argument of '%N' must be of type pointer to void
:MSGJTXT. '%N'�̍ŏ��̈�����'void *'�łȂ���΂Ȃ�܂���
The C++ language requires that the first argument for
.kw operator delete
and
.kw operator delete []
be a
.id void *.
:errbad.
class C;
void operator delete( C* );
void operator delete []( C* );
:eerrbad.

:MSGSYM. ERR_OPERATOR_DEL_SECOND_ARG
:MSGTXT. the second argument of '%N' must be of type size_t
:MSGJTXT. '%N'�̓�Ԗڂ̈�����'size_t'�^�łȂ���΂Ȃ�܂���
The C++ language requires that the second argument for
.kw operator delete
and
.kw operator delete []
be of type "size_t".
The two argument form of
.kw operator delete
and
.kw operator delete []
is optional and it can only be present inside of a class declaration.
The definition for "size_t" can be included
by using the standard header file <stddef.h>.
:errbad.
struct S {
    void operator delete( void *, char );
    void operator delete []( void *, char );
};
:eerrbad.

:MSGSYM. ERR_OPERATOR_INC_DEC_SECOND_ARG
:MSGTXT. the second argument of 'operator ++' or 'operator --' must be int
:MSGJTXT. 'operator ++'��'operator --'�̓�Ԗڂ̈�����int�łȂ���΂Ȃ�܂���
The C++ language requires that the second argument for
.kw operator ++
be
.kw int.
The two argument form of
.kw operator ++
is used to overload the postfix operator "++".
The postfix operator "--" can be overloaded similarly.
:errbad.
class C {
public:
    long cv;
};
C& operator ++( C&, unsigned );
:eerrbad.

:MSGSYM. ERR_OPERATOR_ARROW_RETURN_BAD
:MSGTXT. return type of '%S' must allow the '->' operator to be applied
:MSGJTXT. '%S'�̖߂�^�́C'->'���Z�q���K�p�����̂������Ȃ���΂Ȃ�܂���
This restriction is a result of the transformation that the compiler performs
when the
.kw operator ->
is overloaded.  The transformation involves transforming the expression
to invoke the operator with "->" applied to the result of
.kw operator ->.
:errgood.
struct S {
    int a;
    S *operator ->();
};

void fn( S &q )
{
    q->a = 1; // becomes (q.operator ->())->a = 1;
}
:eerrgood.

:MSGSYM. ERR_OPERATOR_MUST_TAKE_CLASS_OR_ENUM
:MSGTXT. '%N' must take at least one argument of a class/enum or a reference to a class/enum
:MSGJTXT. '%N'�́Cclass/enum��class/enum�ւ̎Q�Ƃ̏����Ƃ�1�̈������Ƃ�Ȃ���΂Ȃ�܂���
Overloaded operators can only be defined for classes and enumerations.
At least one argument, must be a class or an enum type in order for the
C++ compiler to distinguish the operator from the built-in operators.
:errbad.
class C {
public:
    long cv;
};
C& operator ++( unsigned, int );
:eerrbad.

:MSGSYM. ERR_TOO_MANY_INITIALIZERS
:MSGTXT. too many initializers
:MSGJTXT. �������w�肪�������܂�
The compiler has detected extra initializers.
:errbad.
int a[3] = { 1, 2, 3, 4 };
:eerrbad.

:MSGSYM. ERR_TOO_MANY_STRING_INITIALIZERS
:MSGTXT. too many initializers for character string
:MSGJTXT. ������ɑ΂��ď������w�肪�������܂�
A string literal used in an initialization of a character
array is viewed as providing the terminating null character.
If the number of array elements isn't enough to accept the
terminating character, this message is output.
:errbad.
char ac[3] = "abc";
:eerrbad.

:MSGSYM. ERR_EXPECTING_BUT_FOUND_EXPRESSION
:MSGTXT. expecting '%s' but found expression
:MSGJTXT. '%s'������͂��ł����C��������܂���
This message is output when some bracing or punctuation is expected but
an expression was encountered.
:errbad.
int b[3] = 3;
:eerrbad.

:MSGSYM. ERR_ANONYMOUS_SAME_NAME_AS_CLASS
:MSGTXT. anonymous struct/union member '%N' cannot be declared in this class
:MSGJTXT. ���O�Ȃ���struct/union�����o�['%N'�́C���̃N���X�ɂ����Đ錾�ł��܂���
An anonymous member cannot be declared with the same name as its containing
class.
:errbad.
struct S {
    union {
    int S;      // Error!
    char b;
    };
};
:eerrbad.

:MSGSYM. ERR_UNEXPECTED_DURING_INITIALIZATION
:MSGTXT. unexpected '%s' during initialization
:MSGJTXT. ������Ԑݒ�̊Ԃ�'%s'��������܂���
This message is output when some unexpected bracing or punctuation
is encountered during initialization.
:errbad.
int e = { { 1 };
:eerrbad.

:MSGSYM. ERR_TYPEDEF_SAME_NAME_AS_CLASS
:MSGTXT. nested type '%N' cannot be declared in this class
:MSGJTXT. �l�X�g�ɂ��ꂽ�^'%N'�́C���̃N���X�ɂ����Đ錾����邱�Ƃ��ł��܂���
A nested type cannot be declared with the same name as its containing
class.
:errbad.
struct S {
    typedef int S;  // Error!
};
:eerrbad.

:MSGSYM. ERR_ENUM_SAME_NAME_AS_CLASS
:MSGTXT. enumerator '%N' cannot be declared in this class
:MSGJTXT. �񋓎q'%N'�́C���̃N���X�ɂ����Đ錾����邱�Ƃ��ł��܂���
An enumerator cannot be declared with the same name as its containing
class.
:errbad.
struct S {
    enum E {
    S,  // Error!
    T
    };
};
:eerrbad.

:MSGSYM. ERR_STATIC_SAME_NAME_AS_CLASS
:MSGTXT. static member '%N' cannot be declared in this class
:MSGJTXT. �X�^�e�B�b�N�E�����o'%N'�́C���̃N���X�ɂ����Đ錾����邱�Ƃ��ł��܂���
A static member cannot be declared with the same name as its containing
class.
:errbad.
struct S {
    static int S;   // Error!
};
:eerrbad.

:MSGSYM. ERR_CTOR_RETURNS_NOTHING
:MSGTXT. constructor cannot have a return type
:MSGJTXT. �R���X�g���N�^�͖߂�l�����Ă܂���
A constructor cannot have a return type (even
.kw void
).
The constructor is a special member function that is not
required to be identical in form to all other member functions.
This allows different
implementations to have different uses for any return values.
:errbad.
class C {
public:
    C& C( int );
};
:eerrbad.

:MSGSYM. ERR_CTOR_CANT_BE_STATIC
:MSGTXT. constructor cannot be a static member
:MSGJTXT. �R���X�g���N�^�̓X�^�e�B�b�N�֐������Ă܂���
A constructor is a special member function that takes raw storage and
changes it into an instance of a class.  In order to do this, it must
have access to storage for the instance of the class so it is restricted
to being a non-static member function.
:errbad.
class C {
public:
    static C( int );
};
:eerrbad.

:MSGSYM. ERR_CTOR_BAD_ARG_LIST
:MSGTXT. invalid copy constructor argument list (causes infinite recursion)
:MSGJTXT. �s�K�؂ȃR�s�[�E�R���X�g���N�^�������X�g�i�����F�����̍ċA�j
A copy constructor's first argument must be a reference argument.
Furthermore, any default arguments must also be reference arguments.
Without the reference, a copy constructor would require a
copy constructor to execute in order to prepare its arguments.
Unfortunately, this would be calling itself since it is the
copy constructor.
:errgood.
struct S {
    S( S const & );   // copy constructor
};
:eerrgood.

:MSGSYM. ERR_CTOR_CANT_BE_CONST_VOLATILE
:MSGTXT. constructor cannot be declared const or volatile
:MSGJTXT. �R���X�g���N�^��'const'�܂���'volatile'�Ƃ��Đ錾�ł��܂���
A constructor must be able to operate on all instances of classes
regardless of whether they are
.kw const
or
.kw volatile.
:errbad.
class C {
public:
    C( int ) const;
    C( float ) volatile;
};
:eerrbad.

:MSGSYM. ERR_CTOR_CANT_BE_VIRTUAL
:MSGTXT. constructor cannot be virtual
:MSGJTXT. �R���X�g���N�^��'virtual'�ɂ͂ł��܂���
Virtual functions cannot be called for an object before it is
constructed.  For this reason, a virtual constructor is not
allowed in the C++ language.  Techniques for simulating a
virtual constructor are known, one such technique is described
in the ARM p.263.
:errbad.
class C {
public:
    virtual C( int );
};
:eerrbad.

:MSGSYM. ERR_INVALID_SCALAR_DESTRUCTOR
:MSGTXT. types do not match in simple type destructor
:MSGJTXT. �^�́C�P���Ȍ^�f�X�g���N�^�̒��ň�v���܂���
A simple type destructor is available for "destructing" simple
types.  The destructor has no effect.  Both of the types
must be identical, for the destructor to have meaning.
:errbad.
void foo( int *p )
{
    p->int::~double();
}
:eerrbad.

:MSGSYM. ERR_OPERATOR_AMBIGUOUS_OVERLOAD
:MSGTXT. overloaded operator is ambiguous for operands used
:MSGJTXT. �g���Ă���I�y�����h�̂��߁C�I�[�o�[���[�h���Z�q���B���ł�
The Open Watcom C++ compiler performs exhaustive analysis using formalized
techniques in order to decide what implicit conversions should be
applied for overloading operators.  Because of this, Open Watcom C++
detects ambiguities that may escape other C++ compilers.
The most common ambiguity that Open Watcom C++ detects involves
classes having constructors with single arguments and
a user-defined conversion.
:errbad.
struct S {
    S(int);
    operator int();
    int a;
};

int fn( int b, int i, S s )
{
    //    i    : s.operator int()
    // OR S(i) : s
    return b ? i : s;
}
:eerrbad.
.np
In the above example, "i" and "s" must be brought to a common
type.  Unfortunately, there are two common types so the compiler
cannot decide which one it should choose, hence an ambiguity.

:MSGSYM. ERR_NOT_IMPLEMENTED
:MSGTXT. feature not implemented
:MSGJTXT. �@�\�͎�������Ă��܂���
The compiler does not support the indicated feature.

:MSGSYM. ERR_FRIEND_BAD
:MSGTXT. invalid friend declaration
:MSGJTXT. �s�K�؂ȃt�����h�錾�ł�
This message indicates that the compiler found
extra declaration specifiers like
.kw auto,
.kw float,
or
.kw const
in the friend declaration.
:errbad.
class C
{
    friend float;
};
:eerrbad.

:MSGSYM. ERR_FRIEND_NOT_IN_CLASS
:MSGTXT. friend declarations may only be declared in a class
:MSGJTXT. �t�����h�錾�̓N���X�̒��ł̂ݐ錾�ł��܂�
This message indicates that a
.kw friend
declaration was found outside a class scope (i.e., a class
definition).  Friends are only meaningful for class types.
:errbad.
extern void foo();
friend void foo();
:eerrbad.

:MSGSYM. ANSI_CLASS_FRIEND_NEEDS_CLASS
:MSGTXT. class friend declaration needs 'class' or 'struct' keyword
:MSGJTXT. �N���X�E�t�����h�錾�́C'class'��'struct'�L�[���[�h��K�v�Ƃ��܂�
:ANSI. 4
The C++ language has evolved to require that all friend class declarations
be of the form "class S" or "struct S".  The Open Watcom C++ compiler accepts
the older syntax with a warning but rejects the syntax in pure ISO/ANSI
C++ mode.
:errbad.
struct S;
struct T {
    friend S;   // should be "friend class S;"
};
:eerrbad.

:MSGSYM. ERR_CLASS_FRIEND_DEFINES_SOMETHING
:MSGTXT. class friend declarations cannot contain a class definition
:MSGJTXT. �N���X�E�t�����h�錾�́C�N���X��`���܂ނ��Ƃ��ł��܂���
A class friend declaration cannot define a new class.  This is a restriction
required in the C++ language.
:errbad.
struct S {
    friend struct X {
    int f;
    };
};
:eerrbad.

:MSGSYM. WARN_CLASS_FRIEND_REPEATED
:MSGTXT. '%T' has already been declared as a friend
:MSGJTXT. '%T'�̓t�����h�Ƃ��Ċ��ɐ錾����܂���
:WARNING. 2
The class in the message has already been declared as a friend.
Remove the extra friend declaration.
:errbad.
class S;
class T {
    friend class S;
    int tv;
    friend class S;
};
:eerrbad.

:MSGSYM. WARN_FN_FRIEND_REPEATED
:MSGTXT. function '%S' has already been declared as a friend
:MSGJTXT. �֐�'%S'�̓t�����h�Ƃ��Ċ��ɐ錾����܂���
:WARNING. 2
The function in the message has already been declared as a friend.
Remove the extra friend declaration.
:errbad.
extern void foo();
class T {
    friend void foo();
    int tv;
    friend void foo();
};
:eerrbad.

:MSGSYM. ERR_BAD_FIV_MODIFIER
:MSGTXT. 'friend', 'virtual' or 'inline' modifiers are not part of a function's type
:MSGJTXT. 'friend'�C'virtual'�C���邢�́C'inline'�C���q�́C�֐��̌^�̈ꕔ�ł͂���܂���
This message indicates that the modifiers may be incorrectly placed in
the declaration.  If the declaration is intended, it cannot be accepted
because the modifiers can only be applied to functions that have code
associated with them.
:errbad.
typedef friend (*PF)( void );
:eerrbad.

:MSGSYM. ERR_ASSIGN_IMPOSSIBLE
:MSGTXT. cannot assign right expression to element on left
:MSGJTXT. ���̗v�f�ɉE�̎��������邱�Ƃ͂ł��܂���
This message indicates that the assignment cannot be performed.
It usually arises in assignments of a class type to an arithmetic type.
:errbad.
struct S
{   int sv;
};
S s;
int foo()
{
    int k;
    k = s;
    return k;
}
:eerrbad.

:MSGSYM. ERR_CTOR_AMBIGUOUS
:MSGTXT. constructor is ambiguous for operands used
:MSGJTXT. �g�p����Ă���I�y�����h�̂��߁C�R���X�g���N�^���B���ł�
The operands provided for the constructor did not select a unique constructor.
:errbad.
struct S {
    S(int);
    S(char);
};

S x = S(1.0);
:eerrbad.

:MSGSYM. ERR_CLASS_NOT_DEFINED
:MSGTXT. class '%s' has not been defined
:MSGJTXT. class '%s'�͒�`����Ă��܂���
The name before a '::' scope resolution operator must be defined
unless a member pointer is being declared.
:errbad.
struct S;

int S::* p; // OK
int S::a = 1;   // Error!
:eerrbad.

:MSGSYM. ERR_UNION_UNNAMED_BITFIELD
:MSGTXT. all bit-fields in a union must be named
:MSGJTXT. ���p�̒��̂��ׂẴr�b�g�t�B�[���h�͖��O�������Ȃ���΂Ȃ�܂���
This is a restriction in the C++ language.
The same effect can be achieved with a named bitfield.
:errbad.
union u
{   unsigned bit1 :10;
    unsigned :6;
};
:eerrbad.

:MSGSYM. ERR_CAST_ILLEGAL
:MSGTXT. cannot convert expression to type of cast
:MSGJTXT. �L���X�g�̌^�Ɏ���ϊ����邱�Ƃ��ł��܂���
The cast is trying to convert an expression to a completely unrelated
type.  There is no way the compiler can provide any meaning for the
intended cast.
:errbad.
struct T {
};

void fn()
{
    T y = (T) 0;
}
:eerrbad.

:MSGSYM. ERR_CAST_AMBIGUOUS
:MSGTXT. conversion ambiguity: [expression] to [cast type]
:MSGJTXT. �ϊ��̞B�����F[��]����[�L���X�g�^]��
The cast caused a constructor overload to occur.
The operands provided for the constructor did not select a unique constructor.
:errbad.
struct S {
    S(int);
    S(char);
};

void fn()
{
    S x = (S) 1.0;
}
:eerrbad.

:MSGSYM. ERR_UNNAMED_CLASS_USELESS
:MSGTXT. an anonymous class without a declarator is useless
:MSGJTXT. �錾�q�̂Ȃ����O�Ȃ��N���X�͖��ɗ����܂���
There is no way to reference the type in this kind of declaration.
A name must be provided for either the class or a variable using
the class as its type.
:errbad.
struct {
    int a;
    int b;
};
:eerrbad.

:MSGSYM. ERR_GLOBAL_ANONYMOUS_UNION_MUST_BE_STATIC
:MSGTXT. global anonymous union must be declared static
:MSGJTXT. �O���[�o���Ȗ��O�Ȃ����p�̂́C'static'�Ɛ錾����Ȃ���΂Ȃ�܂���
This is a restriction in the C++ language.  Since there is no unique
name for the anonymous union, it is difficult for C++ translators
to provide a correct implementation of external linkage anonymous unions.
:errgood.
static union {
    int a;
    int b;
};
:eerrgood.

:MSGSYM. ERR_NONGLOBAL_ANONYMOUS_UNION_CANT_BE_ANYTHING
:MSGTXT. anonymous struct/union cannot have storage class in this context
:MSGJTXT. ���O�Ȃ�struct/union�́C�����ŋL���N���X�������Ƃ��ł��܂���
Anonymous unions (or structs) declared in class scopes cannot be
.kw static.
Any other storage class is also disallowed.
:errbad.
struct S {
    static union {
    int iv;
    unsigned us;
    };
};
:eerrbad.

:MSGSYM. WARN_UNION_PROTECTED_MEMBER
:MSGTXT. union contains a protected member
:MSGJTXT. ���p�̂�'protected'�����o���܂݂܂�
:WARNING. 1
A union cannot have a
.kw protected
member because a union cannot be a base class.
:errbad.
static union {
    int iv;
protected:
    unsigned sv;
} u;
:eerrbad.

:MSGSYM. ERR_UNION_PRIVATE_MEMBER
:MSGTXT. anonymous struct/union contains a private member '%S'
:MSGJTXT. ���O�Ȃ�struct/union�́C�v���C�x�[�g�E�����o'%S'���܂݂܂�
An anonymous union (or struct) cannot have member functions or friends
so it cannot have
.kw private
members since no code could access them.
:errbad.
static union {
    int iv;
private:
    unsigned sv;
};
:eerrbad.

:MSGSYM. ERR_UNION_FUNCTION_MEMBER
:MSGTXT. anonymous struct/union contains a function member '%S'
:MSGJTXT. ���O�Ȃ�struct/union�́C�֐������o'%S'���܂݂܂�
An anonymous union (or struct) cannot have any function members.
This is a restriction in the C++ language.
:errbad.
static union {
    int iv;
    void foo();     // error
    unsigned sv;
};
:eerrbad.

:MSGSYM. ERR_UNION_TYPEDEF_MEMBER
:MSGTXT. anonymous struct/union contains a typedef member '%S'
:MSGJTXT. ���O�Ȃ�struct/union�́Ctypedef�����o'%S'���܂݂܂�
An anonymous union (or struct) cannot have any nested types.
This is a restriction in the C++ language.
:errbad.
static union {
    int iv;
    unsigned sv;
    typedef float F;
    F fv;
};
:eerrbad.

:MSGSYM. ERR_UNION_ENUMERATION_MEMBER
:MSGTXT. anonymous struct/union contains an enumeration member '%S'
:MSGJTXT. ���O�Ȃ�struct/union�́C�񋓌^�����o'%S'���܂݂܂�
An anonymous union (or struct) cannot have any enumeration members.
This is a restriction in the C++ language.
:errbad.
static union {
    int iv;
    enum choice { good, bad, indifferent };
    choice c;
    unsigned sv;
};
:eerrbad.

:MSGSYM. ERR_UNION_COLLISION_MEMBER
:MSGTXT. anonymous struct/union member '%s' is not distinct in enclosing scope
:MSGJTXT. ���O�Ȃ�struct/union�����o�['%s'�̓X�R�[�v�̒��ŋ�ʂ����܂���
Since an anonymous union (or struct) provides its member names to the
enclosing scope, the names must not collide with other names in the
enclosing scope.
:errbad.
int iv;
unsigned sv;
static union {
    int iv;
    unsigned sv;
};
:eerrbad.

:MSGSYM. ERR_UNION_CANT_HAVE_MEMBER_WITH_DTOR
:MSGTXT. unions cannot have members with destructors
:MSGJTXT. ���p�̂̓f�X�g���N�^���������o��L���邱�Ƃ��ł��܂���
A union should only be used to organize memory in C++.
Allowing union members to have destructors would mean
that the same piece of memory could be destructed twice.
:errbad.
struct S {
    int sv1, sv2, sv3;
};
struct T {
    ~T();
};
static union
{
    S su;
    T tu;
};
:eerrbad.

:MSGSYM. ERR_UNION_CANT_HAVE_MEMBER_WITH_ASSIGN
:MSGTXT. unions cannot have members with user-defined assignment operators
:MSGJTXT. ���p�̂́C���[�U-��`������Z�q���������o��L���邱�Ƃ��ł��܂���
A union should only be used to organize memory in C++.
Allowing union members to have assignment operators would mean
that the same piece of memory could be assigned twice.
:errbad.
struct S {
    int sv1, sv2, sv3;
};
struct T {
    int tv;
    operator = ( int );
    operator = ( float );
};
static union
{
    S su;
    T tu;
} u;
:eerrbad.

:MSGSYM. ERR_UNION_CANT_HAVE_FRIENDS
:MSGTXT. anonymous struct/union cannot have any friends
:MSGJTXT. ���O�Ȃ�struct/union�̓t�����h�������Ƃ��ł��܂���
An anonymous union (or struct) cannot have any friends.
This is a restriction in the C++ language.
:errbad.
struct S {
    int sv1, sv2, sv3;
};
static union {
    S su1;
    S su2;
    friend class S;
};
:eerrbad.

:MSGSYM. ERR_ONLY_GLOBAL_SPECIFICS
:MSGTXT. specific versions of template classes can only be defined in file scope
:MSGJTXT. �e���v���[�g�E�N���X�̓���̃o�[�W�����́C�t�@�C����X�R�[�v�̒��ł̂ݒ�`�ł��܂�
Currently, specific versions of class templates can only be declared at
file scope.  This simple restriction was chosen in favour of more
freedom with possibly subtle restrictions.
:errbad.
template <class G> class S {
    G x;
};

struct Q {
    struct S<int> {
    int x;
    };
};

void foo()
{
    struct S<double> {
    double x;
    };
}
:eerrbad.

:MSGSYM. ERR_FUNCTION_ANONYMOUS_UNION
:MSGTXT. anonymous union in a function may only be static or auto
:MSGJTXT. �֐��̒��̖��O�Ȃ����p�̂�'static'�܂���'auto'�łȂ���΂��܂���
The current C++ language definition only allows
.kw auto
anonymous unions.  The Open Watcom C++ compiler allows
.kw static
anonymous unions.
Any other storage class is not allowed.

:MSGSYM. ERR_LOCAL_CLASS_NO_STATIC_MEMBERS
:MSGTXT. static data members are not allowed in a local class
:MSGJTXT. �X�^�e�B�b�N�E�f�[�^�E�����o�́C���[�J���E�N���X�ł͋�����܂���
Static data members are not allowed in a local class because there
is no way to define the static member in file scope.
:errbad.
int foo()
{
    struct local {
    static int s;
    };

    local lv;

    lv.s = 3;
    return lv.s;
}
:eerrbad.

:MSGSYM. ERR_RETURN_AMBIGUOUS
:MSGTXT. conversion ambiguity: [return value] to [return type of function]
:MSGJTXT. �ϊ��̞B�����F[�߂�l]����[�֐��̖߂�^]��
The cast caused a constructor overload to occur.
The operands provided for the constructor did not select a unique constructor.
:errbad.
struct S {
    S(int);
    S(char);
};

S fn()
{
    return 1.0;
}
:eerrbad.

:MSGSYM. ERR_RETURN_IMPOSSIBLE
:MSGTXT. conversion of return value is impossible
:MSGJTXT. �߂�l�̕ϊ��͕s�\�ł�
The return is trying to convert an expression to a completely unrelated
type.  There is no way the compiler can provide any meaning for the
intended return type.
:errbad.
struct T {
};

T fn()
{
    return 0;
}
:eerrbad.

:MSGSYM. ERR_FUNCTION_CANT_RETURN_BASED_SELF
:MSGTXT. function cannot return a pointer based on __self
:MSGJTXT. �֐��́C__self�Ɋ�Â��|�C���^��߂����Ƃ��ł��܂���
A function cannot return a pointer that is based on
.kw __self.
:errbad.
void __based(__self) *fn( unsigned );
:eerrbad.

:MSGSYM. ERR_CANNOT_DEFINE_VARIABLE
:MSGTXT. defining '%S' is not possible because its type has unknown size
:MSGJTXT. ���̌^�����m�̑傫���������܂��̂ŁC'%S'���`���邱�Ƃ͂ł��܂���
In order to define a variable, the size must be known so that the
correct amount of storage can be reserved.
:errbad.
class S;
S sv;
:eerrbad.

:MSGSYM. ERR_CANNOT_INIT_TYPEDEF
:MSGTXT. typedef cannot be initialized
:MSGJTXT. typedef�͏������ł��܂���
Initializing a
.kw typedef
is meaningless in the C++ language.
:errbad.
typedef int INT = 15;
:eerrbad.

:MSGSYM. ERR_CONFLICTING_STORAGE_CLASSES
:MSGTXT. storage class of '%S' conflicts with previous declaration
:MSGJTXT. '%S'�̋L���N���X�́C�O�̐錾�Ɩ������܂�
The symbol declaration conflicts with a previous declaration with regard
to storage class.  A symbol cannot be both
.kw static
and
.kw extern.

:MSGSYM. ERR_CONFLICTING_MODIFIERS
:MSGTXT. modifiers of '%S' conflict with previous declaration
:MSGJTXT. '%S'�̏C���q�́C�O�̐錾�Ɩ������܂�
The symbol declaration conflicts with a previous declaration with regard
to modifiers.  Correct the program by using the same modifiers for
both declarations.

:MSGSYM. ERR_CANNOT_INIT_FUNCTION
:MSGTXT. function cannot be initialized
:MSGJTXT. �֐��͏������ł��܂���
A function cannot be initialized with an initializer syntax intended for
variables.  A function body is the only way to provide a definition
for a function.

:MSGSYM. ERR_CLASS_ACCESS
:MSGTXT. access permission of nested class '%T' conflicts with previous declaration
:MSGJTXT. �l�X�g�ɂ��ꂽ�N���X'%T'�̃A�N�Z�X���́C�O�̐錾�Ɩ������܂�
:errbad.
struct S {
    struct N;   // public
private:
    struct N {  // private
    };
};
:eerrbad.

:MSGSYM. ERR_FRONT_END
:MSGTXT. *** FATAL *** internal error in front end
:MSGJTXT. *** �v���I *** �t�����g�G���h�̒��œ����G���[
If this message appears, please report the problem directly to the
Open Watcom development team. See http://www.openwatcom.org/.

:MSGSYM. ERR_PARM_IMPOSSIBLE
:MSGTXT. cannot convert argument to type specified in function prototype
:MSGJTXT. �֐��̃v���g�^�C�v�̒��Ŏw�肳�ꂽ�^�Ɉ�����ϊ����邱�Ƃ��ł��܂���
It is impossible to convert the indicated argument in the function.
:errbad.
extern int foo( int& );

extern int m;
extern int n;

int k = foo( m + n );
:eerrbad.
In the example, the value of "m+n" cannot be converted to a reference
(it could be converted to a constant reference), as shown in the following
example.
:errgood.
extern int foo( const int& );

extern int m;
extern int n;

int k = foo( m + n );
:eerrgood.

:MSGSYM. ERR_PARM_AMBIGUOUS
:MSGTXT. conversion ambiguity: [argument] to [argument type in prototype]
:MSGJTXT. �ϊ��̞B�����F[����]����[�v���g�^�C�v�̒��̈����̌^]��
An argument in the function call could not be converted since
there is more than one constructor or user-defined conversion which could be
used to convert the argument.
:errbad.
struct S;

struct T
{
    T( S& );
};

struct S
{
    operator T();
};

S s;
extern int foo( T );
int k = foo( s );   // ambiguous
:eerrbad.
In the example, the argument "s" could be converted by both the constructor
in class "T" and by the user-conversion in class "S".

:MSGSYM. ERR_BASED_ON_BASED
:MSGTXT. cannot be based on based pointer '%S'
:MSGJTXT. �x�[�X�E�|�C���^'%S'�Ɋ�Â����Ƃ��ł��܂���
A based pointer cannot be based on another based pointer.
:errbad.
__segment s;
void __based(s) *p;
void __based(p) *q;
:eerrbad.

:MSGSYM. ERR_MISSING_DECL_SPECS
:MSGTXT. declaration specifiers are required to declare '%N'
:MSGJTXT. �錾�w��q�́C'%N'��錾���Ȃ��΂Ȃ�܂���
The compiler has detected that the name does not represent a function.
Only function declarations can leave out declaration specifiers.
This error also shows up when a typedef name declaration is missing.
:errbad.
x;
typedef int;
:eerrbad.

:MSGSYM. ERR_STATIC_FN_DECL_IN_FUNCTION
:MSGTXT. static function declared in block scope
:MSGJTXT. �X�^�e�B�b�N�֐����u���b�N��X�R�[�v���Ő錾����Ă��܂�
The C++ language does not allow static functions to be declared in block scope.
This error can be triggered when the intent is to define a
.kw static
variable.  Due to the complexities of parsing C++, statements that
appear to be variable definitions may actually parse as function
prototypes.  A work-around for this problem is contained in the example.
:errbad.
struct C {
};
struct S {
    S( C );
};
void foo()
{
    static S a( C() );  // function prototype!
    static S b( (C()) );// variable definition
}
:eerrbad.

:MSGSYM. ERR_CANT_HAVE_BASED_REFERENCE
:MSGTXT. cannot define a __based reference
:MSGJTXT. __based�Q�Ƃ��`���邱�Ƃ��ł��܂���
A C++ reference cannot be based on anything.
Based modifiers can only be used with pointers.
:errbad.
__segment s;
void fn( int __based(s) & x );
:eerrbad.

:MSGSYM. ERR_PTR_COMMON_AMBIGUOUS
:MSGTXT. conversion ambiguity: conversion to common pointer type
:MSGJTXT. �ϊ��̞B�����F���ʂ̃|�C���^�^�ւ̕ϊ�
A conversion to a common base class of two different pointers has been attempted.
The pointer conversion could not be performed because the destination
type points to an ambiguous base class of one of the source types.

:MSGSYM. ERR_CTOR_IMPOSSIBLE
:MSGTXT. cannot construct object from argument(s)
:MSGJTXT. ��������I�u�W�F�N�g�����邱�Ƃ��ł��܂��� (�K�؂ȃR���X�g���N�^������܂���)
There is not an appropriate constructor for the set of arguments provided.

:MSGSYM. ERR_PARM_COUNT_MISMATCH
:MSGTXT. number of arguments for function '%S' is incorrect
:MSGJTXT. �֐�'%S'�ɑ΂�������̐����s���m�ł�
The number of arguments in the function call does not match the number
declared for the indicated non-overloaded function.
:errbad.
extern int foo( int, int );
int k = foo( 1, 2, 3 );
:eerrbad.
In the example, the function was declared to have two arguments.
Three arguments were used in the call.

:MSGSYM. ERR_CAST_PRIVATE
:MSGTXT. private base class accessed to convert cast expression
:MSGJTXT. �v���C�x�[�g���N���X�́C�L���X�g����ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a private base class.  The access check did not succeed so the
conversion is not allowed.
:errbad.
struct Priv
{
    int p;
};
struct Der : private Priv
{
    int d;
};

extern Der *pd;
Priv *pp = (Priv*)pd;
:eerrbad.

:MSGSYM. ERR_RETURN_PRIVATE
:MSGTXT. private base class accessed to convert return expression
:MSGJTXT. �v���C�x�[�g���N���X�́C���^�[������ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a private base class.  The access check did not succeed so the
conversion is not allowed.
:errbad.
struct Priv
{
    int p;
};
struct Der : private Priv
{
    int d;
};

Priv *foo( Der *p )
{
    return p;
}
:eerrbad.

:MSGSYM. ERR_BAD_PTR_MINUS_OPERANDS
:MSGTXT. cannot subtract pointers to different objects
:MSGJTXT. �قȂ�I�u�W�F�N�g�ւ̃|�C���^�̍������߂邱�Ƃ͂ł��܂���
Pointer subtraction can be performed only for objects of the same type.
:errbad.
#include <stddef.h>
ptrdiff_t diff( float *fp, int *ip )
{
    return fp - ip;
}
:eerrbad.
In the example, a diagnostic results from the attempt to subtract a
pointer to an
.kw int
object from a pointer to a
.kw float
object.

:MSGSYM. ERR_PTR_COMMON_PRIVATE
:MSGTXT. private base class accessed to convert to common pointer type
:MSGJTXT. �v���C�x�[�g���N���X�́C���ʂ̃|�C���^�^�ɕϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a private base class.  The access check did not succeed so the
conversion is not allowed.
:errbad.
struct Priv
{
    int p;
};
struct Der : private Priv
{
    int d;
};

int foo( Der *pd, Priv *pp )
{
    return pd == pp;
}
:eerrbad.

:MSGSYM. ERR_CAST_PROTECTED
:MSGTXT. protected base class accessed to convert cast expression
:MSGJTXT. �v���e�N�g���N���X�́C�L���X�g����ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a protected base class.  The access check did not succeed so the
conversion is not allowed.
:errbad.
struct Prot
{
    int p;
};
struct Der : protected Prot
{
    int d;
};

extern Der *pd;
Prot *pp = (Prot*)pd;
:eerrbad.

:MSGSYM. ERR_RETURN_PROTECTED
:MSGTXT. protected base class accessed to convert return expression
:MSGJTXT. �v���e�N�g���N���X�́C���^�[������ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a protected base class.  The access check did not succeed so the
conversion is not allowed.
:errbad.
struct Prot
{
    int p;
};
struct Der : protected Prot
{
    int d;
};

Prot *foo( Der *p )
{
    return p;
}
:eerrbad.

:MSGSYM. ERR_CANT_HAVE_MEMBER_POINTER_TO_MEM_MODEL
:MSGTXT. cannot define a member pointer with a memory model modifier
:MSGJTXT. �������[���f���C���q�Ƌ��Ƀ����o�E�|�C���^���`���邱�Ƃ͂ł��܂���
A member pointer describes how to access a field from a class.
Because of this a member pointer must be independent of any memory
model considerations.
:errbad.
struct S;

int near S::*mp;
:eerrbad.

:MSGSYM. ERR_PTR_COMMON_PROTECTED
:MSGTXT. protected base class accessed to convert to common pointer type
:MSGJTXT. �v���e�N�g���N���X�́C���ʂ̃|�C���^�^�ɕϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a protected base class.  The access check did not succeed so the
conversion is not allowed.
:errbad.
struct Prot
{
    int p;
};
struct Der : protected Prot
{
    int d;
};

int foo( Der *pd, Prot *pp )
{
    return pd == pp;
}
:eerrbad.

:MSGSYM. ERR_NON_TYPE_PROVIDED_FOR_TYPE
:MSGTXT. non-type parameter supplied for a type argument
:MSGJTXT. �^���w�肷��p�����[�^�ɑ΂��āC�^���w�肳��܂���ł���
A non-type parameter (e.g., an address or a constant expression) has
been supplied for a template type argument.  A type should be used
instead.

:MSGSYM. ERR_TYPE_PROVIDED_FOR_NON_TYPE
:MSGTXT. type parameter supplied for a non-type argument
:MSGJTXT. �^���w�肵�Ȃ��p�����[�^�ɑ΂��Č^���w�肳��܂���
A type parameter (e.g.,
.kw int
) has
been supplied for a template non-type argument.  An address or a
constant expression should be used instead.

:MSGSYM. ERR_LOCAL_ACCESSING_AUTO
:MSGTXT. cannot access enclosing function's auto variable '%S'
:MSGJTXT. �͂�ł���֐��̎����ϐ�'%S'�ɃA�N�Z�X���邱�Ƃ��ł��܂���
A local class member function cannot access its enclosing function's
automatic variables.
:errbad.
void goop( void )
{
    int a;
    struct S
    {
    int foo( int c, int b )
        {
        return b + c + a;
        };
    };
}
:eerrbad.

:MSGSYM. ERR_CONST_PTR_INIT
:MSGTXT. cannot initialize pointer to non-constant with a pointer to constant
:MSGJTXT. ��萔�ւ̃|�C���^�Œ萔�ւ̃|�C���^�����������邱�Ƃ��ł��܂���
A pointer to a non-constant type cannot be initialized with a pointer to
a constant type because this would allow constant data to be modified
via the non-constant pointer to it.
:errbad.
extern const int *pic;
extern int *pi = pic;
:eerrbad.

:MSGSYM. WARN_POINTER_GE_0
:MSGTXT. pointer expression is always >= 0
:MSGJTXT. �|�C���^���͏�� >= 0 �ł�
:WARNING. 1
The indicated pointer expression will always be true because the pointer
value is always treated as an unsigned quantity, which will be greater or
equal to zero.
:errbad.
extern char *p;
unsigned k = ( 0 <= p );    // always 1
:eerrbad.

:MSGSYM. WARN_POINTER_LT_0
:MSGTXT. pointer expression is never < 0
:MSGJTXT. �|�C���^���͏�� < 0 �ł�
:WARNING. 1
The indicated pointer expression will always be false because the pointer
value is always treated as an unsigned quantity, which will be greater or
equal zero.
:errbad.
extern char *p;
unsigned k = ( 0 >= p );   // always 0
:eerrbad.

:MSGSYM. ERR_ILLEGAL_TYPE_USE
:MSGTXT. type cannot be used in this context
:MSGJTXT. �����ł͌^���͎g�p�ł��܂���
This message is issued when a type name is being used in a context
where a non-type name should be used.
:errbad.
struct S {
    typedef int T;
};

void fn( S *p )
{
    p->T = 1;
}
:eerrbad.

:MSGSYM. ERR_VIRTUAL_ONLY_IN_CLASS
:MSGTXT. virtual function may only be declared in a class
:MSGJTXT. ���z�֐��̓N���X�̒��ł̂ݐ錾�ł��܂�
Virtual functions can only be declared inside of a class.
This error may be a result of forgetting the "C::" qualification
of a virtual function's name.
:errbad.
virtual void foo();
struct S
{
    int f;
    virtual void bar();
};
virtual void bar()
{
    f = 9;
}
:eerrbad.

:MSGSYM. ERR_CLASS_REF_UNION_CLASS
:MSGTXT. '%T' referenced as a union
:MSGJTXT. '%T'�����p�̂Ƃ��ĎQ�Ƃ���Ă��܂�
A class type defined as a
.kw class
or
.kw struct
has been referenced as a
.kw union
(i.e., union S).
:errbad.
struct S
{
    int s1, s2;
};
union S var;
:eerrbad.

:MSGSYM. ERR_CLASS_REF_CLASS_UNION
:MSGTXT. union '%T' referenced as a class
:MSGJTXT. union '%T'���N���X�Ƃ��ĎQ�Ƃ���Ă��܂�
A class type defined as a
.kw union
has been referenced as a
.kw struct
or a
.kw class
(i.e., class S).
:errbad.
union S
{
    int s1, s2;
};
struct S var;
:eerrbad.

:MSGSYM. ERR_TYPEDEF_ONLY_DEFAULT_INT
:MSGTXT. typedef '%N' defined without an explicit type
:MSGJTXT. typedef '%N' �������I�Ȍ^�Ȃ��ɒ�`����Ă��܂�
The typedef declaration was found to not have an explicit type
in the declaration.  If
.kw int
is the desired type, use an explicit
.kw int
keyword to specify the type.
:errbad.
typedef T;
:eerrbad.

:MSGSYM. WARN_LOCAL_CLASS_FUNCTION
:MSGTXT. member function was not defined in its class
:MSGJTXT. �����o�[�֐������̃N���X�̒��Œ�`����Ă��܂���
:WARNING. 1
Member functions of local classes must be
defined in their class if they will be defined at all.
This is a result of the C++ language not allowing
nested function definitions.
:errbad.
void fn()
{
    struct S {
    int bar();
    };
}
:eerrbad.

:MSGSYM. ERR_LOCAL_CLASS_NO_FRIEND_FNS
:MSGTXT. local class can only have its containing function as a friend
:MSGJTXT. ���[�J����N���X�͂����Ɋ܂܂�Ă���֐����t�����h�Ƃ��Ď����Ƃ������ł��܂�
A local class can only be referenced from within its containing function.
It is impossible to define an external function that can reference
the type of the local class.
:errbad.
extern void ext();
void foo()
{
    class S
    {
    int s;
    public:
    friend void ext();
    int q;
    };
}
:eerrbad.

:MSGSYM. ERR_LOCAL_CLASS_FRIEND_CLASS
:MSGTXT. local class cannot have '%S' as a friend
:MSGJTXT. ���[�J���ȃN���X�́C�t�����h�Ƃ���'%S'�������Ƃ��ł��܂���
The only classes that a local class can have as a friend are classes within
its own containing scope.
:errbad.
struct ext
{
    goop();
};
void foo()
{
    class S
    {
    int s;
    public:
    friend class ext;
    int q;
    };
}
:eerrbad.

:MSGSYM. WARN_ADJACENT_RELN_OPS
:MSGTXT. adjacent >=, <=, >, < operators
:MSGJTXT. >=, <=, >, < ���Z�q���אڂ��Ă��܂�
:WARNING. 3
This message is warning about the possibility that the code may not do
what was intended.  An expression like "a > b > c" evaluates one relational
operator to a 1 or a 0 and then compares it against the other variable.
:errbad.
extern int a;
extern int b;
extern int c;
int k = a > b > c;
:eerrbad.


:MSGSYM. ERR_LOCAL_ACCESSING_ARG
:MSGTXT. cannot access enclosing function's argument '%S'
:MSGJTXT. �͂�ł���֐��̈���'%S'�ɃA�N�Z�X���邱�Ƃ��ł��܂���
A local class member function cannot access its enclosing function's
arguments.
:errbad.
void goop( int d )
{
    struct S
    {
    int foo( int c, int b )
        {
        return b + c + d;
        };
    };
}
:eerrbad.

:MSGSYM. WARN_SWITCH_NOT_IMPL
:MSGTXT. support for switch '%s' is not implemented
:MSGJTXT. switch '%s'�̃T�|�[�g�͎�������Ă��܂���
:WARNING. 1
Actions for the indicated switch have not been implemented.
The switch is supported for compatibility with the Open Watcom C compiler.

:MSGSYM. WARN_IF_ALWAYS_TRUE
:MSGTXT. conditional expression in if statement is always true
:MSGJTXT. if���̒��̏������͏�ɐ^�ł�
:WARNING. 3
The compiler has detected that the expression will always be true.
If this is not the expected behaviour, the code may contain a
comparison of an unsigned value against zero (e.g., unsigned
integers are always greater than or equal to zero).  Comparisons
against zero for addresses can also result in trivially true expressions.
:errbad.
#define TEST 143
int foo( int a, int b )
{
    if( TEST ) return a;
    return b;
}
:eerrbad.

:MSGSYM. WARN_IF_ALWAYS_FALSE
:MSGTXT. conditional expression in if statement is always false
:MSGJTXT. if���̒��̏������͏�ɋU�ł�
:WARNING. 3
The compiler has detected that the expression will always be false.
If this is not the expected behaviour, the code may contain a
comparison of an unsigned value against zero (e.g., unsigned
integers are always greater than or equal to zero).  Comparisons
against zero for addresses can also result in trivially false expressions.
:errbad.
#define TEST 14-14
int foo( int a, int b )
{
    if( TEST ) return a;
    return b;
}
:eerrbad.

:MSGSYM. WARN_SWITCH_ALWAYS_CONSTANT
:MSGTXT. selection expression in switch statement is a constant value
:MSGJTXT. switch���̒��̑I�����͒萔�l�ł�
:WARNING. 3
The expression in the
.kw switch
statement is a constant.  This means that only one case label
will be executed.  If this is not the expected behaviour,
check the switch expression.
:errbad.
#define TEST 0
int foo( int a, int b )
{
    switch ( TEST ) {
      case 0:
    return a;
      default:
    return b;
    }
}
:eerrbad.

:MSGSYM. ERR_CONST_MEMBER_MEANS_CTOR
:MSGTXT. constructor is required for a class with a const member
:MSGJTXT. �R���X�g���N�^�ɂ́Cconst�����o�[���������N���X���K�v�ł�
If a class has a constant member, a constructor is required in order
to initialize it.
:errbad.
struct S
{
    const int s;
    int i;
};
:eerrbad.

:MSGSYM. ERR_REFERENCE_MEMBER_MEANS_CTOR
:MSGTXT. constructor is required for a class with a reference member
:MSGJTXT. �R���X�g���N�^�ɂ́C�Q�ƃ����o�[���������N���X���K�v�ł�
If a class has a reference member, a constructor is required in order
to initialize it.
:errbad.
struct S
{
    int& r;
    int i;
};
:eerrbad.

:MSGSYM. ERR_INLINE_MEMBER_FRIEND
:MSGTXT. inline member friend function '%S' is not allowed
:MSGJTXT. �C�����C���E�����o�E�t�����h�֐�'%S'�͋�����܂���
A friend that is a member function of another class
cannot be defined.  Inline friend rules are currently
in flux so it is best to avoid inline friends.

:MSGSYM. ERR_BAD_MODIFY_AUTO
:MSGTXT. invalid modifier for auto variable
:MSGJTXT. �����ϐ��ɑ΂��ĕs�K�؂ȏC���q�ł�
An automatic variable cannot have a memory model adjustment because
they are always located on the stack (or in a register).
There are also other types of modifiers that are not allowed for auto
variables such as thread-specific data modifiers.
:errbad.
int fn( int far x )
{
    int far y = x + 1;
    return y;
}
:eerrbad.

:MSGSYM. ERR_INVALID_NONSTATIC_ACCESS
:MSGTXT. object (or object pointer) required to access non-static data member
:MSGJTXT. �I�u�W�F�N�g�i���邢�́C�I�u�W�F�N�g�E�|�C���^�j�́C��X�^�e�B�b�N�E�f�[�^�E�����o�ɃA�N�Z�X���邽�߂ɕK�v�ł�
A reference to a member in a class has occurred.
The member is non-static so in order to access it, an object
of the class is required.
:errbad.
struct S {
    int m;
    static void fn()
    {
    m = 1;  // Error!
    }
};
:eerrbad.

:MSGSYM. ERR_CONVERSION_NOT_DEFINED
:MSGTXT. user-defined conversion has not been declared
:MSGJTXT. ���[�U�[��`�ϊ��͐錾����Ă��܂���
The named user-defined conversion has not been declared in the
class of any of its base classes.
:errbad.
struct S {
    operator int();
    int a;
};

double fn( S *p )
{
    return p->operator double();
}
:eerrbad.

:MSGSYM. ERR_NO_STATIC_VIRTUAL
:MSGTXT. virtual function must be a non-static member function
:MSGJTXT. ���z�֐��͔�X�^�e�B�b�N������o�[�֐��łȂ���΂Ȃ�܂���
A member function cannot be both a
.kw static
function and a
.kw virtual
function.
A static member function does not have a
.kw this
argument whereas a
.kw virtual
function must have a
.kw this
argument so that the virtual function table can be accessed in order
to call it.
:errbad.
struct S
{
    static virtual int foo();   // error
    virtual int bar();      // ok
    static int stat();      // ok
};
:eerrbad.

:MSGSYM. ERR_PARM_PROTECTED
:MSGTXT. protected base class accessed to convert argument expression
:MSGJTXT. �v���e�N�g���N���X�́C��������ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a protected base class.  The access check did not succeed so the
conversion is not allowed.
:errbad.
class C
{
protected:
    C( int );
public:
    int c;
};

int cfun( C );

int i = cfun( 14 );
:eerrbad.
The last line is erroneous since the constructor is protected.

:MSGSYM. ERR_PARM_PRIVATE
:MSGTXT. private base class accessed to convert argument expression
:MSGJTXT. �v���C�x�[�g���N���X�́C��������ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a private base class.  The access check did not succeed so the
conversion is not allowed.
:errbad.
class C
{
    C( int );
public:
    int c;
};

int cfun( C );

int i = cfun( 14 );
:eerrbad.
The last line is erroneous since the constructor is private.

:MSGSYM. WARN_VIRTUAL_DTOR_DELETE
:MSGTXT. delete expression will invoke a non-virtual destructor
:MSGJTXT. delete���́C�񉼑z�f�X�g���N�^���N�����܂�
:WARNING. 1
In C++, it is possible to assign a base class pointer the value
of a derived class pointer so that code that makes use of
base class virtual functions can be used.
A problem that occurs is that a
.kw delete
has to know the correct size of the type in some instances
(i.e., when a two argument version of
.kw operator delete
is defined for a class).
This problem is solved by requiring that a destructor be defined as
.kw virtual
if polymorphic deletes must work.
The
.kw delete
expression will virtually call the correct destructor, which knows
the correct size of the complete object.
This message informs you that the class you are deleting has virtual
functions but it has a non-virtual destructor.  This means that
the delete will not work correctly in all circumstances.
:errbad.
#include <stddef.h>

struct B {
    int b;
    void operator delete( void *, size_t );
    virtual void fn();
    ~B();
};
struct D : B {
    int d;
    void operator delete( void *, size_t );
    virtual void fn();
    ~D();
};

void dfn( B *p )
{
    delete p;   // could be a pointer to D!
}
:eerrbad.

:MSGSYM. ERR_OFFSETOF_FUNCTION
:MSGTXT. 'offsetof' is not allowed for a function
:MSGJTXT. 'offsetof'�͊֐��ɑ΂��Ďg�p�ł��܂���
A member function does not have an offset like simple data members.
If this is required, use a member pointer.
:errbad.
#include <stddef.h>

struct S
{
    int fun();
};

int s = offsetof( S, fun );
:eerrbad.

:MSGSYM. ERR_OFFSETOF_ENUM
:MSGTXT. 'offsetof' is not allowed for an enumeration
:MSGJTXT. 'offsetof'�͗񋓂ɑ΂��Ďg�p�ł��܂���
An enumeration does not have an offset like simple data members.
:errbad.
#include <stddef.h>

struct S
{
    enum SE { S1, S2, S3, S4 };
    SE var;
};

int s = offsetof( S, SE );
:eerrbad.

:MSGSYM. ERR_CODEGEN_CANT_INITIALIZE
:MSGTXT. could not initialize for code generation
:MSGJTXT. �R�[�h�����ŏ������ł��܂���ł���
The source code has been parsed and fully analysed when this
error is emitted.
The compiler attempted to start generating object code
but due to some problem (e.g., out of memory, no file handles)
could not initialize itself.
Try changing the compilation environment to eliminate this error.

:MSGSYM. ERR_OFFSETOF_UNDEFINED_TYPE
:MSGTXT. 'offsetof' is not allowed for an undefined type
:MSGJTXT. 'offsetof'�͖���`�̌^�ɑ΂��Ďg�p�ł��܂���
The class type used in
.kw offsetof
must be completely defined,
otherwise data member offsets will not be known.
:errbad.
#include <stddef.h>

struct S {
    int a;
    int b;
    int c[ offsetof( S, b ) ];
};
:eerrbad.

:MSGSYM. ERR_ATTEMPT_TO_OVERRIDE_RETURN
:MSGTXT. attempt to override virtual function '%S' with a different return type
:MSGJTXT. �قȂ�߂�^�����������z�֐�'%S'���I�[�o�[���C�h���悤�Ƃ��܂���
A function cannot be overloaded with identical argument types and a
different return type.  This is due to the fact that the C++ language
does not consider the function's return type when overloading.
The exception to this rule in the C++ language involves restricted
changes in the return type of virtual functions.
The derived virtual function's return type can be derived from
the return type of the base virtual function.
:errgood.
struct B {
    virtual B *fn();
};
struct D : B {
    virtual D *fn();
};
:eerrgood.

:MSGSYM. ERR_ATTEMPT_TO_OVERLOAD_RETURN
:MSGTXT. attempt to overload function '%S' with a different return type
:MSGJTXT. �قȂ�߂�^���������֐�'%S'���I�[�o�[���[�h���悤�Ƃ��܂���
A function cannot be overloaded with identical argument types and a
different return type.  This is due to the fact that the C++ language
does not consider the function's return type when overloading.
:errbad.
int foo( char );
unsigned foo( char );
:eerrbad.

:MSGSYM. ERR_UNDEFED_CLASS_PTR
:MSGTXT. attempt to use pointer to undefined class
:MSGJTXT. ����`�̃N���X�ւ̃|�C���^���g�p���悤�Ƃ��Ă��܂�
An attempt was made to indirect or increment a pointer to an undefined
class.  Since the class is undefined, the size is not known so the
compiler cannot compile the expression properly.
:errbad.
class C;
extern C* pc1;
C* pc2 = ++pc1;     // C not defined

int foo( C*p )
{
    return p->x;    // C not defined
}
:eerrbad.

:MSGSYM. WARN_ONLY_SIDE_EFFECT
:MSGTXT. expression is useful only for its side effects
:MSGJTXT. ���́C���̕���p�������ʂ�����܂���
:WARNING. 3
The indicated expression is not meaningful.
The expression, however, does contain one or more side effects.
:errbad.
extern int* i;
void func()
{
    *(i++);
}
:eerrbad.
In the example, the expression is a reference to an integer which is
meaningless in itself.
The incrementation of the pointer in the expression is a side effect.

:MSGSYM. WARN_TRUNC_INT_CONSTANT
:MSGTXT. integral constant will be truncated during assignment or initialization
:MSGJTXT. �����萔�́C������������̊Ԃɐ؂�l�߂��܂�
:WARNING. 1
This message indicates that the compiler knows that a constant value will
not be preserved after the assignment.  If this is acceptable, cast the
constant value to the appropriate type in the assignment.
:errbad.
unsigned char c = 567;
:eerrbad.

:MSGSYM. WARN_TRUNC_INT_VALUE
:MSGTXT. integral value may be truncated during assignment or initialization
:MSGJTXT. �����l�́C������������̊Ԃɐ؂�l�߂��邩������܂���
:WARNING. 4
This message indicates that the compiler knows that all values will
not be preserved after the assignment.  If this is acceptable, cast the
value to the appropriate type in the assignment.
:errbad.
extern unsigned s;
unsigned char c = s;
:eerrbad.

:MSGSYM. ERR_NO_DEFAULT_INIT_CTOR
:MSGTXT. cannot generate default constructor to initialize '%T' since constructors were declared
:MSGJTXT. �R���X�g���N�^���錾����Ă����̂ŁC'%T'��������Ԃɂ��邽�߂Ƀf�t�H���g�E�R���X�g���N�^�𐶐����邱�Ƃ��ł��܂���
A default constructor will not be generated by the compiler if there
are already constructors declared.  Try using default arguments to
change one of the constructors to a default constructor or define a
default constructor explicitly.
:errbad.
class C {
    C( const C& );
public :
    int c;
};
C cv;
:eerrbad.

:MSGSYM. WARN_ASSIGN_VALUE_IN_BOOL_EXPR
:MSGTXT. assignment found in boolean expression
:MSGJTXT. �_�����̒��ɑ��������܂�
:WARNING. 3
This is a construct that can lead to errors if it was intended
to be an equality (using "==") test.
:errbad.
int foo( int a, int b )
{
    if( a = b ) {
    return b;
    }
    return a;       // always return 1 ?
}
:eerrbad.

:MSGSYM. INF_SYMBOL_DECLARATION
:MSGTXT. definition: '%F'
:MSGJTXT. '%F'�͒�`����Ă��܂��F
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

:MSGSYM. INF_FILE_NEST
:MSGTXT. included from %s(%u)
:MSGJTXT. %s(%u)����C���N���[�h����Ă��܂�
:INFO.
This informational message indicates the line number of the file including
the file in which an error or warning was diagnosed.
A number of such messages will allow you to trace back through the
.kw #include
directives which are currently being processed.

:MSGSYM. ERR_REFERENCE_MUST_BE_INITIALIZED
:MSGTXT. reference object must be initialized
:MSGJTXT. �Q�ƃI�u�W�F�N�g�͏���������Ȃ���΂Ȃ�܂���
A reference cannot be set except through initialization.
Also references cannot be 0 so they must always be initialized.
:errbad.
int & ref;
:eerrbad.

:MSGSYM. ERR_INVALID_OPTION_ID
:MSGTXT. option requires an identifier
:MSGJTXT. �I�v�V�����ɂ͎��ʎq���K�v�ł�
The specified option is not recognized by the compiler
since there was no identifier after it (i.e., "-nt=module" ).

:MSGSYM. ERR_MAIN_CANNOT_BE_OVERLOADED
:MSGTXT. 'main' cannot be overloaded
:MSGJTXT. 'main'�̓I�[�o�[���[�h�ł��܂���
There can only be one entry point for a C++ program.  The "main"
function cannot be overloaded.
:errbad.
int main();
int main( int );
:eerrbad.

:MSGSYM. ERR_NEW_TYPE_VOID
:MSGTXT. 'new' expression cannot allocate a void
:MSGJTXT. 'new'����void�^������t�����܂���
Since the
.kw void
type has no size and there are no values of
.kw void
type, one cannot allocate an instance of
.kw void.
:errbad.
void *p = new void;
:eerrbad.

:MSGSYM. ERR_NEW_TYPE_FUNCTION
:MSGTXT. 'new' expression cannot allocate a function
:MSGJTXT. 'new'���͊֐�������t�����܂���
A function type cannot be allocated since there is no meaningful
size that can be used.
The
.kw new
expression can allocate a pointer to a function.
:errbad.
typedef int tdfun( int );
tdfun *tdv = new tdfun;
:eerrbad.

:MSGSYM. WARN_NEW_TYPE_CONST_VOLATILE
:MSGTXT. 'new' expression allocates a const or volatile object
:MSGJTXT. 'new'���́Cconst��volatile�I�u�W�F�N�g�����蓖�Ă܂�
:WARNING. 3
The pool of raw memory cannot be guaranteed to support
.kw const
or
.kw volatile
semantics.  Usually
.kw const
and
.kw volatile
are used for statically allocated objects.
:errbad.
typedef const int con_int;
con_int* p = new con_int;
:eerrbad.

:MSGSYM. ERR_INIT_IMPOSSIBLE
:MSGTXT. cannot convert right expression for initialization
:MSGJTXT. ������Ԑݒ�̂��߂ɉE�̎���ϊ����邱�Ƃ��ł��܂���
The initialization is trying to convert an argument expression to
a completely unrelated
type.  There is no way the compiler can provide any meaning for the
intended conversion.
:errbad.
struct T {
};

T x = 0;
:eerrbad.

:MSGSYM. ERR_INIT_AMBIGUOUS
:MSGTXT. conversion ambiguity: [initialization expression] to [type of object]
:MSGJTXT. �ϊ��̞B�����F[��������]����[�I�u�W�F�N�g�̌^]��
The initialization caused a constructor overload to occur.
The operands provided for the constructor did not select a unique constructor.
:errbad.
struct S {
    S(int);
    S(char);
};

S x = 1.0;
:eerrbad.

:MSGSYM. WARN_CLASS_TEMPLATE_FRIEND_REPEATED
:MSGTXT. class template '%S' has already been declared as a friend
:MSGJTXT. �N���X�E�e���v���[�g'%S'�́C�t�����h�Ƃ��Ă��łɐ錾����܂���
:WARNING. 2
The class template in the message has already been declared as a friend.
Remove the extra friend declaration.
:errbad.
template <class T>
    class S;

class X {
    friend class S;
    int f;
    friend class S;
};
:eerrbad.

:MSGSYM. ERR_INIT_PRIVATE
:MSGTXT. private base class accessed to convert initialization expression
:MSGJTXT. �v���C�x�[�g���N���X�́C����������ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a private base class.  The access check did not succeed so the
conversion is not allowed.

:MSGSYM. ERR_INIT_PROTECTED
:MSGTXT. protected base class accessed to convert initialization expression
:MSGJTXT. �v���e�N�g���N���X�́C����������ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a protected base class.  The access check did not succeed so the
conversion is not allowed.

:MSGSYM. ERR_CONST_PTR_RETURN
:MSGTXT. cannot return a pointer or reference to a constant object
:MSGJTXT. �萔�̃I�u�W�F�N�g�ւ̃|�C���^�܂��͎Q�Ƃ�߂����Ƃ��ł��܂���
A pointer or reference to a constant object cannot be returned.
:errbad.
int *foo( const int *p )
{
    return p;
}
:eerrbad.

:MSGSYM. ERR_CONST_PTR_ARG
:MSGTXT. cannot pass a pointer or reference to a constant object
:MSGJTXT. �萔�̃I�u�W�F�N�g�ւ̃|�C���^���Q�Ƃ�n�����Ƃ��ł��܂���
A pointer or reference to a constant object could not be passed as
an argument.
:errbad.
int *bar( int * );
int *foo( const int *p )
{
    return bar( p );
}
:eerrbad.

:MSGSYM. ERR_CLASS_TEMPLATE_MUST_BE_NAMED
:MSGTXT. class templates must be named
:MSGJTXT. �N���X�E�e���v���[�g�ɂ͖��O���K�v�ł�
There is no syntax in the C++ language to reference an
unnamed class template.
:errbad.
template <class T>
    class {
    };
:eerrbad.

:MSGSYM. ERR_NO_VARIABLE_TEMPLATES
:MSGTXT. function templates can only name functions
:MSGJTXT. �֐��e���v���[�g�́C�֐��ɖ��O�����邱�Ƃ������ł��܂�
Variables cannot be overloaded in C++ so it is not
possible to have many different instances of a variable
with different types.
:errbad.
template <class T>
    T x[1];
:eerrbad.

:MSGSYM. ERR_FUNCTION_TEMPLATE_MUST_USE_ALL_ARGS
:MSGTXT. template argument '%S' is not used in the function argument list
:MSGJTXT. �e���v���[�g����'%S'���֐��������X�g�̒��Ŏg���Ă��܂���
This restriction ensures that function templates can be bound to
types during overload resolution.
Functions currently can only be overloaded based on argument types.
:errbad.
template <class T>
    int foo( int * );
template <class T>
    T bar( int * );
:eerrbad.

:MSGSYM. ERR_DTOR_CANT_BE_CONST_VOLATILE
:MSGTXT. destructor cannot be declared const or volatile
:MSGJTXT. �f�X�g���N�^�́Cconst�܂���volatile�Ɛ錾���邱�Ƃ͂ł��܂���
A destructor must be able to operate on all instances of classes
regardless of whether they are
.kw const
or
.kw volatile.

:MSGSYM. ERR_STATIC_CANT_BE_CONST_VOLATILE
:MSGTXT. static member function cannot be declared const or volatile
:MSGJTXT. �X�^�e�B�b�N�E�����o�֐��́C'const'�܂���'volatile'�Ɛ錾���邱�Ƃ͂ł��܂���
A static member function does not have an implicit
.kw this
argument so the
.kw const
and
.kw volatile
function qualifiers cannot be used.

:MSGSYM. ERR_NON_MEMBER_CANT_BE_CONST_VOLATILE
:MSGTXT. only member functions can be declared const or volatile
:MSGJTXT. �����o�֐������́C'const'�܂���'volatile'�Ɛ錾���邱�Ƃ��ł��܂�
A non-member function does not have an implicit
.kw this
argument so the
.kw const
and
.kw volatile
function qualifiers cannot be used.

:MSGSYM. ERR_CONST_VOLATILE_IN_A_TYPE
:MSGTXT. 'const' or 'volatile' modifiers are not part of a function's type
:MSGJTXT. 'const'�܂���'volatile'�C���q�́C�֐��̌^�̈ꕔ�ł͂���܂���
The
.kw const
and
.kw volatile
qualifiers for a function cannot be used in typedefs or pointers to
functions.  The trailing qualifiers are used to change the type of
the implicit
.kw this
argument so that member functions that do not modify the object can
be declared accurately.
:errbad.
// const is illegal
typedef void (*baddcl)() const;

struct S {
    void fun() const;
    int a;
};

// "this" has type "S const *"
void S::fun() const
{
    this->a = 1;    // Error!
}
:eerrbad.

:MSGSYM. ERR_TYPE_CREATED_IN_ARG
:MSGTXT. type cannot be defined in an argument
:MSGJTXT. �^�͈����̒��Œ�`�ł��܂���
A new type cannot be defined in an argument because the type will only
be visible within the function.  This amounts to defining a function
that can never be called because C++ uses name equivalence for type checking.
:errbad.
extern foo( struct S { int s; } );
:eerrbad.

:MSGSYM. ERR_TYPE_CREATED_IN_RETURN
:MSGTXT. type cannot be defined in return type
:MSGJTXT. �^�͖߂�^�̒��Œ�`�܂��͐錾�ł��܂���
This is a restriction in the current C++ language.  A function
prototype should only use previously declared types in order
to guarantee that it can be called from other functions.
The restriction is required for templates because the compiler would
have to wait until the end of a class definition before it could
decide whether a class template or function template is being defined.
:errbad.
template <class T>
    class C {
    T value;
    } fn( T x ) {
    C y;

    y.x = 0;
    return y;
    };
:eerrbad.
.np
A common problem that results in this error is to forget to terminate
a class or enum definition with a semicolon.
:errbad.
struct S {
    int x,y;
    S( int, int );
} // missing semicolon ';'

S::S( int x, int y ) : x(x), y(y) {
}
:eerrbad.

:MSGSYM. ERR_PURE_FUNCTIONS_ONLY
:MSGTXT. data members cannot be initialized inside a class definition
:MSGJTXT. �f�[�^�E�����o�́C�N���X��`�̓����ŏ������ł��܂���
This message appears when an initialization is attempted inside
of a class definition.  In the case of static data members, initialization
must be done outside the class definition.
Ordinary data members can be initialized in a constructor.
:errbad.
struct S {
    static const int size = 1;
};
:eerrbad.

:MSGSYM. ERR_PURE_VIRTUAL_FUNCTIONS_ONLY
:MSGTXT. only virtual functions may be declared pure
:MSGJTXT. ���z�֐�����pure�Ɛ錾�ł��܂�
The C++ language requires that all pure functions be declared virtual.
A pure function establishes an interface that must consist of virtual
functions because the functions are required to be defined in the
derived class.
:errbad.
struct S {
    void foo() = 0;
};
:eerrbad.

:MSGSYM. ERR_DESTRUCTOR_IN_WRONG_CLASS
:MSGTXT. destructor is not declared in its proper class
:MSGJTXT. �f�X�g���N�^�́C���̓K���ȃN���X�ɂ����Đ錾����܂���
The destructor name is not declared in its own class or qualified
by its own class.  This is required in the C++ language.

:MSGSYM. ERR_CONST_PTR_THIS
:MSGTXT. cannot call non-const function for a constant object
:MSGJTXT. �萔�̃I�u�W�F�N�g�ɑ΂���const�łȂ��֐����ĂԂ��Ƃ��ł��܂���
A function that does not promise to not modify an object
cannot be called for a constant object.
A function can declare its intention to not modify an object
by using the
.kw const
qualifier.
:errbad.
struct S {
    void fn();
};

void cfn( const S *p )
{
    p->fn();    // Error!
}
:eerrbad.

:MSGSYM. ERR_MEM_INIT_MUST_BE_CTOR
:MSGTXT. memory initializer list may only appear in a constructor definition
:MSGJTXT. �������������q�̓R���X�g���N�^��`�̒��ł̂ݎg���܂�
A memory initializer list should be declared along with the body
of the constructor function.

:MSGSYM. ERR_DUPLICATE_MEMBER_INIT
:MSGTXT. cannot initialize member '%N' twice
:MSGJTXT. �����o'%N'��2�񏉊������邱�Ƃ͂ł��܂���
A member cannot be initialized twice in a member initialization list.

:MSGSYM. ERR_DUPLICATE_BASE_INIT
:MSGTXT. cannot initialize base class '%T' twice
:MSGJTXT. ���N���X'%T'��2�񏉊������邱�Ƃ͂ł��܂���
A base class cannot be constructed twice in a member initialization list.

:MSGSYM. ERR_NOT_DIRECT_BASE_INIT
:MSGTXT. '%T' is not a direct base class
:MSGJTXT. '%T'�͒��ڂ̊��N���X�ł͂���܂���
A base class initializer in a member initialization list must either
be a direct base class or a virtual base class.

:MSGSYM. ERR_NOT_MEMBER_MEMBER_INIT
:MSGTXT. '%N' cannot be initialized because it is not a member
:MSGJTXT. '%N'�̓����o�ł͂���܂���̂ŁC�������ł��܂���
The name used in the member initialization list does not name a member
in the class.

:MSGSYM. ERR_FN_MEMBER_MEMBER_INIT
:MSGTXT. '%N' cannot be initialized because it is a member function
:MSGJTXT. '%N'�̓����o�֐��ł��̂ŁC�������ł��܂���
The name used in the member initialization list does not name
a non-static data member in the class.

:MSGSYM. ERR_STATIC_MEMBER_MEMBER_INIT
:MSGTXT. '%N' cannot be initialized because it is a static member
:MSGJTXT. '%N'�̓X�^�e�B�b�N�E�����o�ł��̂ŁC�������ł��܂���
The name used in the member initialization list does not name
a non-static data member in the class.

:MSGSYM. ERR_NOT_A_MEMBER_NAME
:MSGTXT. '%N' has not been declared as a member
:MSGJTXT. '%N'�̓����o�Ƃ��Đ錾����Ă��܂���
This message indicates that the member does not exist in the qualified
class.  This usually occurs in the context of access declarations.

:MSGSYM. ERR_MEMBER_WILL_NOT_BE_INIT
:MSGTXT. const/reference member '%S' must have an initializer
:MSGJTXT. const/�Q�ƃ����o�['%S'�͏������q�������Ȃ���΂Ȃ�܂���
The
.kw const
or reference member does not have an initializer so the constructor
is not completely defined.  The member initialization list is the only
way to initialize these types of members.

:MSGSYM. ERR_ARG_ABSTRACT_TYPE
:MSGTXT. abstract class '%T' cannot be used as an argument type
:MSGJTXT. ���ۓI�ȃN���X'%T'�́C�����̌^�Ƃ��Ďg���܂���
An abstract class can only exist as a base class of another class.
The C++ language does not allow an abstract class to be used as
an argument type.

:MSGSYM. ERR_FUNCTION_CANT_RETURN_AN_ABSTRACT
:MSGTXT. abstract class '%T' cannot be used as a function return type
:MSGJTXT. ���ۓI�ȃN���X'%T'�́C�֐��߂�^�Ƃ��Ďg���܂���
An abstract class can only exist as a base class of another class.
The C++ language does not allow an abstract class to be used as
a return type.

:MSGSYM. ERR_CANNOT_DEFINE_ABSTRACT_VARIABLE
:MSGTXT. defining '%S' is not possible because '%T' is an abstract class
:MSGJTXT. '%T'�����ۓI�ȃN���X�ł��̂ŁC'%S'���`���邱�Ƃ͂ł��܂���
An abstract class can only exist as a base class of another class.
The C++ language does not allow an abstract class to be used as
either a member or a variable.

:MSGSYM. ERR_CONVERT_TO_ABSTRACT_TYPE
:MSGTXT. cannot convert to an abstract class '%T'
:MSGJTXT. ���ۓI�ȃN���X'%T'�ɕϊ����邱�Ƃ��ł��܂���
An abstract class can only exist as a base class of another class.
The C++ language does not allow an abstract class to be used as
the destination type in a conversion.

:MSGSYM. WARN_MANGLED_NAME_TOO_LONG
:MSGTXT. mangled name for '%S' has been truncated
:MSGJTXT. '%S'�ɑ΂���}���O�����͐؂�l�߂��܂���
:WARNING. 9
The name used in the object file that encodes the name and full type
of the symbol is often called a mangled name.  The warning indicates
that the mangled name had to be truncated due to limitations in the
object file format.

:MSGSYM. ERR_CONVERT_TO_UNDEFD_TYPE
:MSGTXT. cannot convert to a type of unknown size
:MSGJTXT. ���m�̑傫���̌^�ɕϊ����邱�Ƃ��ł��܂���
A completely unknown type cannot be used in a conversion
because its size is not known.
The behaviour of the conversion would be undefined also.

:MSGSYM. ERR_CONVERT_FROM_UNDEFD_TYPE
:MSGTXT. cannot convert a type of unknown size
:MSGJTXT. ���m�̑傫���̌^��ϊ����邱�Ƃ��ł��܂���
A completely unknown type cannot be used in a conversion
because its size is not known.
The behaviour of the conversion would be undefined also.

:MSGSYM. ERR_CONSTRUCT_AN_ABSTRACT_TYPE
:MSGTXT. cannot construct an abstract class
:MSGJTXT. ���ۃN���X���C���X�^���X�����邱�Ƃ͂ł��܂���
An instance of an abstract class cannot be created because an abstract
class can only be used as a base class.

:MSGSYM. ERR_CONSTRUCT_AN_UNDEFD_TYPE
:MSGTXT. cannot construct an undefined class
:MSGJTXT. ����`�N���X���C���X�^���X�����邱�Ƃ͂ł��܂���
An instance of an undefined class cannot be created because
the size is not known.

:MSGSYM. WARN_STRING_CONCAT_IN_ARRAY
:MSGTXT. string literal concatenated during array initialization
:MSGJTXT. �z��̏��������C�����񃊃e�����͘A������܂���
:WARNING. 3
This message indicates that a missing comma (',') could have
made a quiet change in the program.  Otherwise, ignore this
message.

:MSGSYM. ERR_MAX_SEGMENT_EXCEEDED
:MSGTXT. maximum size of segment '%s' has been exceeded for '%S'
:MSGJTXT. �Z�O�����g'%s'�̍ő�T�C�Y���C'%S'�̂��߂ɉz���܂���
The indicated symbol has grown in size to a point where it
has caused the segment it is defined inside of to be exhausted.

:MSGSYM. ERR_DATA_TOO_BIG
:MSGTXT. maximum data item size has been exceeded for '%S'
:MSGJTXT. �ő�f�[�^���ڃT�C�Y���C'%S'�̂��߂ɉz���܂���
A non-huge data item is larger than 64k bytes in size.
This message only occurs during 16-bit compilation of C++ code.

:MSGSYM. WARN_REPEATED_FUNCTION_MODS
:MSGTXT. function attribute has been repeated
:MSGJTXT. �֐��������J��Ԃ��g���Ă��܂�
:WARNING. 1
A function attribute (like the
.kw __export
attribute) has been repeated.
Remove the extra attribute to correct the declaration.

:MSGSYM. WARN_REPEATED_DATA_MODS
:MSGTXT. modifier has been repeated
:MSGJTXT. �C���q���J��Ԃ��g���Ă��܂�
:WARNING. 1
A modifier (like the
.kw far
modifier) has been repeated.
Remove the extra modifier to correct the declaration.

:MSGSYM. ERR_UNCOMBINABLE_MEM_MODS
:MSGTXT. illegal combination of memory model modifiers
:MSGJTXT. �������E���f���C���q�̈�@�ȑg�����ł�
Memory model modifiers must be used individually because they cannot
be combined meaningfully.

:MSGSYM. ERR_DUPLICATE_ARG_NAME
:MSGTXT. argument name '%N' has already been used
:MSGJTXT. ������'%N'�͊��Ɏg���Ă��܂�
The indicated argument name has already been used in the same argument
list.  This is not allowed in the C++ language.

:MSGSYM. ERR_BAD_FUNCTION_TYPE
:MSGTXT. function definition for '%S' must be declared with an explicit argument list
:MSGJTXT. '%S'�ɑ΂���֐���`�́C�����I�Ȉ������X�g�Ő錾����Ȃ���΂Ȃ�܂���
A function cannot be defined with a typedef.  The argument list
must be explicit.

:MSGSYM. ANSI_USER_CONV_REF_DERIVED
:MSGTXT. user-defined conversion cannot convert to its own class or base class
:MSGJTXT. ���[�U��`�ϊ��́C���ꎩ�g�̃N���X�܂��̓x�[�X�N���X��ϊ��ł��܂���
:ANSI. 1
A user-defined conversion cannot be declared as a conversion either to
its own class or to a base class of itself.
:errbad.
struct B {
};
struct D : private B {
    operator B();
};
:eerrbad.

:MSGSYM. ERR_USER_CONV_VOID
:MSGTXT. user-defined conversion cannot convert to void
:MSGJTXT. ���[�U��`�ϊ���'void'�^�ɕϊ��ł��܂���
A user-defined conversion cannot be declared as a conversion to
.kw void.
:errbad.
struct S {
    operator void();
};
:eerrbad.

:MSGSYM. ERR_EXPECTING_ID
:MSGTXT. expecting identifier
:MSGJTXT. ���ʎq������͂��ł�
An identifier was expected during processing.

:MSGSYM. ERR_BASED_EXTRACT_NO_SEG
:MSGTXT. symbol '%S' does not have a segment associated with it
:MSGJTXT. �V���{��'%S'�́C����Ɗ֘A����Z�O�����g�������܂���
A pointer cannot be based on a member because it has no segment associated
with it.  A member describes a layout of storage that can occur in any
segment.

:MSGSYM. ERR_BASED_FETCH_NO_SEG
:MSGTXT. symbol '%S' must have integral or pointer type
:MSGJTXT. �V���{��'%S'�́C�����^���|�C���^�^�łȂ���΂Ȃ�܂���
If a symbol is based on another symbol, it must be integral or a pointer
type.  An integral type indicates the segment value that will be used.
A pointer type means that all accesses will be added to the pointer value
to construct a full pointer.

:MSGSYM. ERR_CANNOT_ALWAYS_ACCESS
:MSGTXT. symbol '%S' cannot be accessed in all contexts
:MSGJTXT. �V���{��'%S'�́C���ׂẴR���e�N�X�g�̒��ŃA�N�Z�X�ł��܂���
The symbol that the pointer is based on is in another class so it cannot
be accessed in all contexts that the based pointer can be accessed.

:MSGSYM. ERR_COPYCTOR_IMPOSSIBLE
:MSGTXT. cannot convert class expression to be copied
:MSGJTXT. �R�s�[����邽�߂ɃN���X����ϊ����邱�Ƃ��ł��܂���
:cmt JWW to look at
A convert class expression could not be copied.

:MSGSYM. ERR_COPYCTOR_AMBIGUOUS
:MSGTXT. conversion ambiguity: multiple copy constructors
:MSGJTXT. �ϊ��̞B�����F�����̃R�s�[�E�R���X�g���N�^������܂�
:cmt JWW to look at
More than one constructor could be used to copy a class object.

:MSGSYM. ERR_FUNCTION_TEMPLATE_ALREADY_HAS_DEFN
:MSGTXT. function template '%S' already has a definition
:MSGJTXT. �֐��e���v���[�g'%S'�͊��ɒ�`����Ă��܂�
The function template has already been defined with a
function body.  A function template cannot be
defined twice even if the function body is identical.
:errbad.
template <class T>
    void f( T *p )
    {
    }
template <class T>
    void f( T *p )
    {
    }
:eerrbad.

:MSGSYM. ERR_FUNCTION_TEMPLATE_NO_DEFARGS
:MSGTXT. function templates cannot have default arguments
:MSGJTXT. �֐��e���v���[�g�́C�f�t�H���g�����������Ƃ��ł��܂���
A function template must not have default arguments because
there are certain types of default arguments that do not force
the function argument to be a specific type.
:errbad.
template <class T>
    void f2( T *p = 0 )
    {
    }
:eerrbad.

:MSGSYM. ERR_MAIN_CANNOT_BE_FN_TEMPLATE
:MSGTXT. 'main' cannot be a function template
:MSGJTXT. 'main'�͊֐��e���v���[�g�ł��邱�Ƃ��ł��܂���
This is a restriction in the C++ language because "main"
cannot be overloaded.  A function template provides the
possibility of having more than one "main" function.

:MSGSYM. ERR_PREV_MUST_BE_ELABORATED_TYPEDEF
:MSGTXT. '%S' was previously declared as a typedef
:MSGJTXT. '%S'��typedef�Ƃ��ĈȑO�ɐ錾����܂���
The C++ language only allows function and variable names
to coexist with names of classes or enumerations.
This is due to the fact that the class and enumeration
names can still be referenced in their elaborated form
after the non-type name has been declared.
:errbad.
typedef int T;
int T( int )        // error!
{
}

enum E { A, B, C };
void E()
{
    enum E x = A;   // use "enum E"
}

class C { };
void C()
{
    class C x;      // use "class C"
}
:eerrbad.

:MSGSYM. ERR_CURR_MUST_BE_ELABORATED_TYPEDEF
:MSGTXT. '%S' was previously declared as a variable/function
:MSGJTXT. '%S'�͕ϐ��^�֐��Ƃ��ĈȑO�ɐ錾����܂���
The C++ language only allows function and variable names
to coexist with names of classes or enumerations.
This is due to the fact that the class and enumeration
names can still be referenced in their elaborated form
after the non-type name has been declared.
:errbad.
int T( int )
{
}
typedef int T;      // error!

void E()
{
}
enum E { A, B, C };

enum E x = A;       // use "enum E"

void C()
{
}
class C { };

class C x;      // use "class C"
:eerrbad.

:MSGSYM. ERR_ASSIGN_PRIVATE
:MSGTXT. private base class accessed to convert assignment expression
:MSGJTXT. �v���C�x�[�g���N���X�́C�������ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a private base class.  The access check did not succeed so the
conversion is not allowed.

:MSGSYM. ERR_ASSIGN_PROTECTED
:MSGTXT. protected base class accessed to convert assignment expression
:MSGJTXT. �v���e�N�g���N���X�́C�������ϊ����邽�߂ɃA�N�Z�X����܂���
A conversion involving the inheritance hierarchy required access
to a protected base class.  The access check did not succeed so the
conversion is not allowed.

:MSGSYM. ERR_MAX_DGROUP_EXCEEDED
:MSGTXT. maximum size of DGROUP has been exceeded for '%S' in segment '%s'
:MSGJTXT. DGROUP�̍ő�T�C�Y�́C'%S'�̂��߂ɃZ�O�����g'%s'�̒��ŉz���܂���
The indicated symbol's size has caused the DGROUP contribution of this
module to exceed 64k.  Changing memory models or declaring some data as
.kw far
data are two ways of fixing this problem.

:MSGSYM. ERR_BAD_ENUM_RETURN
:MSGTXT. type of return value is not the enumeration type of function
:MSGJTXT. �߂�l�̌^�́C�֐��̗񋓌^�ł͂���܂���
The return value does not have the proper enumeration type.
Keep in mind that integral values are not automatically converted
to enum types like the C language.

:MSGSYM. ERR_MISPLACED_EXTERN_LINKAGE
:MSGTXT. linkage must be first in a declaration; probable cause: missing ';'
:MSGJTXT. �����N�͐錾�̒��ōŏ��łȂ���΂Ȃ�܂���G�l�����錴���F';'������܂���
This message usually indicates a missing semicolon (';').  The linkage
specification must be the first part of a declaration if it is used.

:MSGSYM. ERR_MAIN_CANNOT_BE_STATIC
:MSGTXT. 'main' cannot be a static function
:MSGJTXT. 'main'�̓X�^�e�B�b�N�֐��ł��邱�Ƃ��ł��܂���
This is a restriction in the C++ language because "main"
must have external linkage.

:MSGSYM. ERR_MAIN_CANNOT_BE_INLINE
:MSGTXT. 'main' cannot be an inline function
:MSGJTXT. 'main'�̓C�����C���֐��ł��邱�Ƃ��ł��܂���
This is a restriction in the C++ language because "main"
must have external linkage.

:MSGSYM. ERR_REFERENCED_MAIN
:MSGTXT. 'main' cannot be referenced
:MSGJTXT. 'main'�͎Q�Ƃł��܂���
This is a restriction in the C++ language to prevent implementations
from having to work around multiple invocations of "main".
This can occur if an implementation has to generate special code in
"main" to construct all of the statically allocated classes.

:MSGSYM. ERR_VOLATILE_PTR_THIS
:MSGTXT. cannot call a non-volatile function for a volatile object
:MSGJTXT. �������I�u�W�F�N�g�ɑ΂��ĕs�������֐����ĂԂ��Ƃ��ł��܂���
A function that does not promise to not modify an object using
.kw volatile
semantics cannot be called for a volatile object.
A function can declare its intention to modify an object
only through
.kw volatile
semantics by using the
.kw volatile
qualifier.
:errbad.
struct S {
    void fn();
};

void cfn( volatile S *p )
{
    p->fn();    // Error!
}
:eerrbad.

:MSGSYM. ERR_CNV_VOID_STAR
:MSGTXT. cannot convert pointer to constant or volatile objects to pointer to void
:MSGJTXT. �萔�܂��͊������I�u�W�F�N�g�ւ̃|�C���^��'void *'�ɕϊ����邱�Ƃ��ł��܂���
You cannot convert a pointer to constant or volatile objects to 'void*'.
:errbad.
extern const int* pci;
extern void *vp;

int k = ( pci == vp );
:eerrbad.

:MSGSYM. ERR_CNV_VOID_STAR_VOLATILE
:MSGTXT. cannot convert pointer to constant or non-volatile objects to pointer to volatile void
:MSGJTXT. �萔���s�������I�u�W�F�N�g�ւ̃|�C���^��'volatile void *'�ɕϊ����邱�Ƃ��ł��܂���
You cannot convert a pointer to constant or non-volatile objects
to 'volatile void*'.
:errbad.
extern const int* pci;
extern volatile void *vp;

int k = ( pci == vp );
:eerrbad.

:MSGSYM. ERR_FUNC_ADDR_TOO_BIG
:MSGTXT. address of function is too large to be converted to pointer to void
:MSGJTXT. �֐��̃A�h���X��'void*'�ɕς���ɂ͑傫�����܂�
The address of a function can be converted to 'void*' only when the size
of a 'void*' object is large enough to contain the function pointer.
:errbad.
void __far foo();
void __near *v = &foo;
:eerrbad.

:MSGSYM. ERR_OBJECT_ADDR_TOO_BIG
:MSGTXT. address of data object is too large to be converted to pointer to void
:MSGJTXT. �f�[�^�I�u�W�F�N�g�̃A�h���X�́C'void*'�ɕς���ɂ͑傫�����܂�
The address of an object can be converted to 'void*' only when the size
of a 'void*' object is large enough to contain the pointer.
:errbad.
int __far *ip;
void __near *v = ip;
:eerrbad.

:MSGSYM. WARN_SIZEOF_SIDE_EFFECT
:MSGTXT. expression with side effect in sizeof discarded
:MSGJTXT. 'sizeof'�̒��̕���p�������͔j������܂�
:WARNING. 1
The indicated expression will be discarded; consequently, any side effects
in that expression will not be executed.
:errbad.
int a = 14;
int b = sizeof( a++ );
:eerrbad.
In the example, the variable
.id a
will still have a value 14 after
.id b
has been initialized.

:MSGSYM. ERR_FUNCTION_NO_MATCH
:MSGTXT. function argument(s) do not match those in prototype
:MSGJTXT. �֐������́C�v���g�^�C�v�̒��ł����ƈ�v���܂���
The C++ language requires great precision in specifying arguments
for a function.  For instance, a pointer to
.id char
is considered different than a pointer to
.id unsigned char
regardless of whether
.id char
is an unsigned quantity.
This message occurs when a non-overloaded function is invoked and one or more
of the arguments cannot be converted.
It also occurs when the number of arguments differs from the number specified
in the prototype.

:MSGSYM. ERR_ASSIGN_OPR_AMBIGUOUS
:MSGTXT. conversion ambiguity: [expression] to [class object]
:MSGJTXT. �ϊ��̞B�����F[��]����[�N���X�I�u�W�F�N�g]��
:cmt JWW to look at
The conversion of the expression to a class object is ambiguous.

:MSGSYM. ERR_ASSIGN_OPR_NO_MATCH
:MSGTXT. cannot assign right expression to class object
:MSGJTXT. �N���X�I�u�W�F�N�g�ɉE�̎������ł��܂���
:cmt JWW to look at
The expression on the right cannot be assigned to the indicated class object.

:MSGSYM. INF_NUMBER_ARGS_HAS_THIS
:MSGTXT. argument count is %d since there is an implicit 'this' argument
:MSGJTXT. �Öق�'this'����������̂ŁC�����̐���%d�ł�
:INFO.
This informational message indicates the number of arguments
for the function mentioned in the error message.
The function is a member function with a
.kw this
argument so it may have one more argument than expected.

:MSGSYM. INF_NUMBER_ARGS_NO_THIS
:MSGTXT. argument count is %d since there is no implicit 'this' argument
:MSGJTXT. �Öق�'this'�������Ȃ��̂ŁC�����̐���%d�ł�
:INFO.
This informational message indicates the number of arguments
for the function mentioned in the error message.
The function is a member function without a
.kw this
argument so it may have one less argument than expected.

:MSGSYM. INF_NUMBER_ARGS
:MSGTXT. argument count is %d for a non-member function
:MSGJTXT. �����̐��́C�񃁃��o�[�֐��ɑ΂���%d�ł�
:INFO.
This informational message indicates the number of arguments
for the function mentioned in the error message.
The function is not a member function but it could be declared as a
.kw friend
function.

:MSGSYM. ERR_ARRAY_COPY_CTOR_AMBIGUOUS
:MSGTXT. conversion ambiguity: multiple copy constructors to copy array '%S'
:MSGJTXT. �ϊ��̞B�����F�z��'%S'���R�s�[���镡���̃R�s�[�E�R���X�g���N�^
:cmt JWW to look at
More than one constructor to copy the indicated array exists.

:MSGSYM. WARN_TYPEDEF_HAS_SAME_NAME
:MSGTXT. variable/function has the same name as the class/enum '%S'
:MSGJTXT. �ϐ�/�֐��́C����class/enum'%S'�Ɠ������������܂�
:WARNING. 3
In C++, a class or enum name can coexist with a variable or function of
the same name in a scope.  This warning is indicating that the current
declaration is making use of this feature but the typedef name was
declared in another file.  This usually means that there are two
unrelated uses of the same name.

:MSGSYM. WARN_NON_TYPEDEF_HAS_SAME_NAME
:MSGTXT. class/enum has the same name as the function/variable '%S'
:MSGJTXT. class/enum�́C���̊֐�/�ϐ�'%S'�Ɠ������������܂�
:WARNING. 1
In C++, a class or enum name can coexist with a variable or function of
the same name in a scope.  This warning is indicating that the current
declaration is making use of this feature but the function/variable name
was declared in another file.  This usually means that there are two
unrelated uses of the same name.  Furthermore, all references to the
class or enum must be elaborated (i.e., use 'class C' instead of 'C') in
order for subsequent references to compile properly.

:MSGSYM. ERR_DEF_CTOR_IMPOSSIBLE
:MSGTXT. cannot create a default constructor
:MSGJTXT. �f�t�H���g�E�R���X�g���N�^�����邱�Ƃ��ł��܂���
A default constructor could not be created, because other constructors were
declared for the class in question.
:errbad.
struct X {
    X(X&);
};
struct Y {
    X a[10];
};
Y yvar;
:eerrbad.
.np
In the example, the variable "yvar" causes a default constructor for the
class "Y" to be generated.  The default constructor for "Y" attempts to call
the default constructor for "X" in order to initialize the array "a" in
class "Y". The default constructor for "X" cannot be defined because another
constructor has been declared.

:MSGSYM. INF_DEF_CTOR
:MSGTXT. attempting to access default constructor for %T
:MSGJTXT. %T�̂��߂Ƀf�t�H���g�E�R���X�g���N�^�ɃA�N�Z�X���悤�Ƃ��Ă��܂�
:INFO.
This informational message indicates that a default constructor
was referenced but could not be generated.

:MSGSYM. ERR_HUGE_SYM_ALIGN
:MSGTXT. cannot align symbol '%S' to segment boundary
:MSGJTXT. �Z�O�����g���E�ɃV���{��'%S'�𐮗�ł��܂���
The indicated symbol requires more than one segment of storage and the
symbol's components cannot be aligned to the segment boundary.

:MSGSYM. ERR_CLASS_FRIEND_NO_CLASS
:MSGTXT. friend declaration does not specify a class or function
:MSGJTXT. �t�����h�錾�́C�N���X�܂��͊֐����w�肵�Ă��܂���
A class or function must be declared as a friend.
:errbad.
struct T {
    // should be class or function declaration
    friend int;
};
:eerrbad.

:MSGSYM. ERR_ADDR_OF_OVERLOADED_FUN
:MSGTXT. cannot take address of overloaded function
:MSGJTXT. �I�[�o�[���[�h�֐��̃A�h���X���Ƃ邱�Ƃ��ł��܂���
This message indicates that an overloaded function's name was
used in a context where a final type could not be found.
Because a final type was not specified, the compiler cannot
select one function to use in the expression.
Initialize a properly-typed temporary with the appropriate function and
use the temporary in the expression.
:errbad.
int foo( char );
int foo( unsigned );
extern int (*p)( char );
int k = ( p == &foo );          // fails
:eerrbad.
The first
.id foo
can be passed as follows:
:errgood.
int foo( char );
int foo( unsigned );
extern int (*p)( char );

// introduce temporary
static int (*temp)( char ) = &foo;

// ok
int k = ( p == temp );
:eerrgood.

:MSGSYM. ERR_ELLIPSE_ADDR_OVERLOAD
:MSGTXT. cannot use address of overloaded function as a variable argument
:MSGJTXT. �I�[�o�[���[�h�֐��̃A�h���X��ϐ������Ƃ��Ďg�����Ƃ��ł��܂���
This message indicates that an overloaded function's name was
used as a argument for a "..." style function.
Because a final function type is not present, the compiler cannot
select one function to use in the expression.
Initialize a properly-typed temporary with the appropriate function and
use the temporary in the call.
:errbad.
int foo( char );
int foo( unsigned );
int ellip_fun( int, ... );
int k = ellip_fun( 14, &foo );      // fails
:eerrbad.
The first
.id foo
can be passed as follows:
:errgood.
int foo( char );
int foo( unsigned );
int ellip_fun( int, ... );

static int (*temp)( char ) = &foo;  // introduce temporary

int k = ellip_fun( 14, temp );      // ok
:eerrgood.

:MSGSYM. ERR_FUNCTION_CANNOT_BE_OVERLOADED
:MSGTXT. '%N' cannot be overloaded
:MSGJTXT. '%N'�̓I�[�o�[���[�h�ł��܂���
The indicated function cannot be overloaded.  Functions that fall into
this category include
.kw operator delete.

:MSGSYM. ERR_CANNOT_INIT_AGAIN
:MSGTXT. symbol '%S' has already been initialized
:MSGJTXT. �V���{��'%S'�͊��ɏ���������Ă��܂�
The indicated symbol has already been initialized.  It cannot be
initialized twice even if the initialization value is identical.

:MSGSYM. ERR_DLT_PTR_TO_FUNCTION
:MSGTXT. delete expression is a pointer to a function
:MSGJTXT. delete���͊֐��ւ̃|�C���^�ł�
A pointer to a function cannot be allocated so it cannot be
deleted.

:MSGSYM. WARN_DLT_PTR_TO_CONST
:MSGTXT. delete of a pointer to const data
:MSGJTXT. delete���Z�q��const�f�[�^�ւ̃|�C���^���폜���Ă��܂�
:WARNING. 1
Since deleting a pointer may involve modification
of data, it is not always safe to delete a pointer
to const data.
:errbad.
struct S { };
void fn( S const *p, S const *q ) {
    delete p;
    delete [] q;
}
:eerrbad.

:MSGSYM. ERR_DLT_NOT_PTR_TO_DATA
:MSGTXT. delete expression is not a pointer to data
:MSGJTXT. delete���̓f�[�^�ւ̃|�C���^�ł͂���܂���
A
.kw delete
expression can only delete pointers.
For example, trying to delete an
.kw int
is not allowed in the C++ language.
:errbad.
void fn( int a )
{
    delete a;   // Error!
}
:eerrbad.

:MSGSYM. ERR_TEMPLATE_ARG_NON_CONSTANT
:MSGTXT. template argument is not a constant expression
:MSGJTXT. �e���v���[�g�����͒萔���ł͂���܂���
The compiler has found an incorrect expression provided
as the value for a constant value template argument.
The only expressions allowed for scalar template arguments
are integral constant expressions.

:MSGSYM. ERR_TEMPLATE_ARG_NOT_SYMBOL
:MSGTXT. template argument is not an external linkage symbol
:MSGJTXT. �e���v���[�g�����͊O���̃����N�E�V���{���ł͂���܂���
The compiler has found an incorrect expression provided
as the value for a pointer value template argument.
The only expressions allowed for pointer template arguments
are addresses of symbols.
Any symbols must have external linkage or must be static class members.

:MSGSYM. ERR_REF_CNV_CONST_VOLATILE
:MSGTXT. conversion of const reference to volatile reference
:MSGJTXT. const�Q�Ƃ���������Q�Ƃւ̕ϊ��ł�
The constant value can be modified by assigning into the
volatile reference.  This would allow constant data to be
modified quietly.
:errbad.
void fn( const int &rci )
{
    int volatile &r = rci;  // Error!
}
:eerrbad.

:MSGSYM. ERR_REF_CNV_VOLATILE_CONST
:MSGTXT. conversion of volatile reference to const reference
:MSGJTXT. �������Q�Ƃ���const�Q�Ƃւ̕ϊ��ł�
The volatile value can be read incorrectly by accessing the
const reference.  This would allow volatile data to be
accessed without correct volatile semantics.
:errbad.
void fn( volatile int &rvi )
{
    int const &r = rvi; // Error!
}
:eerrbad.

:MSGSYM. ERR_REF_CNV_CV_PLAIN
:MSGTXT. conversion of const or volatile reference to plain reference
:MSGJTXT. const�܂���volatile�Q�Ƃ���C���q�Ȃ��̎Q�Ƃւ̕ϊ��ł�
The constant value can be modified by assigning into the
plain reference.  This would allow constant data to be
modified quietly.  In the case of volatile data, any access to
the plain reference will not respect the volatility of the data
and thus would be incorrectly accessing the data.
:errbad.
void fn( const int &rci, volatile int &rvi )
{
    int &r1 = rci;  // Error!
    int &r2 = rvi;  // Error!
}
:eerrbad.

:MSGSYM. ERR_SYNTAX_UNDECLARED_ID
:MSGTXT. syntax error before '%s'; probable cause: incorrectly spelled type name
:MSGJTXT. '%s'�̑O�̍\���G���[�G�l�����錴���F�Ԉ���ĂÂ�ꂽ�^��
The identifier in the error message has not been declared as a type name
in any scope at this point in the code.  This may be the cause of the
syntax error.

:MSGSYM. ERR_BARE_FUNCTION_ACCESS
:MSGTXT. object (or object pointer) required to access non-static member function
:MSGJTXT. �I�u�W�F�N�g�i���邢�́C�I�u�W�F�N�g�E�|�C���^�j�́C��X�^�e�B�b�N�E�����o�֐��ɃA�N�Z�X���邽�߂ɕK�v�ł�
A reference to a member function in a class has occurred.
The member is non-static so in order to access it, an object
of the class is required.
:errbad.
struct S {
    int m();
    static void fn()
    {
    m();    // Error!
    }
};
:eerrbad.

:MSGSYM. ERR_EXTRA_THIS_FOR_FUNCTION
:MSGTXT. object (or object pointer) cannot be used to access function
:MSGJTXT. �I�u�W�F�N�g�i���邢�́C�I�u�W�F�N�g�|�C���^�j�́C�֐��ɃA�N�Z�X���邽�߂Ɏg�p���邱�Ƃ͂ł��܂���
:cmt JWW to look at
The indicated object (or object pointer) cannot be used to access function.

:MSGSYM. ERR_EXTRA_THIS_FOR_DATA
:MSGTXT. object (or object pointer) cannot be used to access data
:MSGJTXT. �I�u�W�F�N�g�i���邢�́C�I�u�W�F�N�g�|�C���^�j���C�f�[�^�ɃA�N�Z�X���邽�߂Ɏg�p���邱�Ƃ͂ł��܂���
:cmt JWW to look at
The indicated object (or object pointer) cannot be used to access data.

:MSGSYM. ERR_ENCLOSING_THIS_FUNCTION
:MSGTXT. cannot access member function in enclosing class
:MSGJTXT. �͂�ł���N���X�̃����o�֐��ɃA�N�Z�X���邱�Ƃ��ł��܂���
:cmt JWW to look at
A member function in enclosing class cannot be accessed.

:MSGSYM. ERR_ENCLOSING_THIS_DATA
:MSGTXT. cannot access data member in enclosing class
:MSGJTXT. �͂�ł���N���X�̃f�[�^�E�����o�ɃA�N�Z�X���邱�Ƃ��ł��܂���
:cmt JWW to look at
A data member in enclosing class cannot be accessed.

:MSGSYM. ERR_SYNTAX_TYPE_NAME
:MSGTXT. syntax error before type name '%s'
:MSGJTXT. �^��'%s'�̑O�̍\���G���[
The identifier in the error message has been declared as a type name
at this point in the code.  This may be the cause of the syntax error.

:MSGSYM. ERR_NO_VIRTUAL_ELLIPSE_FUNCTION_THUNKS
:MSGTXT. implementation restriction: cannot generate thunk from '%S'
:MSGJTXT. ��������: '%S'����T���N�𐶐��ł��܂���
This implementation restriction is due to the use of a shared code generator
between Open Watcom compilers.
The virtual
.kw this
adjustment thunks are generated as functions linked into the virtual
function table.  The functions rely on knowing the correct number of
arguments to pass on to the overriding virtual function but in the case
of ellipsis (...) functions, the number of arguments cannot be known
when the thunk function is being generated by the compiler.
The target symbol is listed in a diagnostic message.
The work around for this problem is to recode the source so that
the virtual functions make use of the va_list type found in the stdarg
header file.
:errbad.
#include <iostream.h>
#include <stdarg.h>

struct B {
    virtual void fun( char *, ... );
};
struct D : B {
    virtual void fun( char *, ... );
};
void B::fun( char *f, ... )
{
    va_list args;

    va_start( args, f );
    while( *f ) {
    cout << va_arg( args, char ) << endl;
    ++f;
    }
    va_end( args );
}
void D::fun( char *f, ... )
{
    va_list args;

    va_start( args, f );
    while( *f ) {
    cout << va_arg( args, int ) << endl;
    ++f;
    }
    va_end( args );
}
:eerrbad.
The previous example can be changed to the following code with
corresponding changes to the contents of the virtual functions.
:errgood.
#include <iostream.h>
#include <stdarg.h>

struct B {
    void fun( char *f, ... )
    {
    va_list args;

    va_start( args, f );
    _fun( f, args );
    va_end( args );
    }
    virtual void _fun( char *, va_list );
};
:errbreak.
struct D : B {
    // this can be removed since using B::fun
    // will result in the same behaviour
    // since _fun is a virtual function
    void fun( char *f, ... )
    {
    va_list args;

    va_start( args, f );
    _fun( f, args );
    va_end( args );
    }
    virtual void _fun( char *, va_list );
};
:errbreak.
void B::_fun( char *f, va_list args )
{
    while( *f ) {
    cout << va_arg( args, char ) << endl;
    ++f;
    }
}
:errbreak.
void D::_fun( char *f, va_list args )
{
    while( *f ) {
    cout << va_arg( args, int ) << endl;
    ++f;
    }
}

:errbreak.
// no changes are required for users of the class
B x;
D y;

void dump( B *p )
{
    p->fun( "1234", 'a', 'b', 'c', 'd' );
    p->fun( "12", 'a', 'b' );
}

:errbreak.
void main()
{
    dump( &x );
    dump( &y );
}
:eerrgood.

:MSGSYM. ERR_VIRTUAL_FOR_BASED_VOID
:MSGTXT. conversion of __based( void ) pointer to virtual base class
:MSGJTXT. __based(void�j�|�C���^���牼�z���N���X�ւ̕ϊ��ł�
An __based(void) pointer to a class object cannot be converted to a pointer
to virtual base class, since this conversion applies only to specific
objects.
:errbad.
struct Base {};
struct Derived : virtual Base {};
Derived __based( void ) *p_derived;
Base __based( void ) *p_base = p_derived; // error
:eerrbad.
The conversion would be allowed if the base class were not virtual.

:MSGSYM. ERR_MEMB_PTR_NOT_DERIVED
:MSGTXT. class for target operand is not derived from class for source operand
:MSGJTXT. �^�[�Q�b�g��I�������h�̃N���X�̓\�[�X��I�y�����h�̃N���X����h������Ă��܂�
A member pointer conversion can only be performed safely when converting
a base class member pointer to a derived class member pointer.

:MSGSYM. ERR_MEMB_PTR_AMBIGUOUS
:MSGTXT. conversion ambiguity: [pointer to class member] to [assignment object]
:MSGJTXT. �ϊ��̞B�����F[�N���X�E�����o�ւ̃|�C���^]����[����I�u�W�F�N�g]��
The base class in the original member pointer is not a unique base class
of the derived class.

:MSGSYM. ERR_MEMB_PTR_PRIVATE
:MSGTXT. conversion of pointer to class member involves a private base class
:MSGJTXT. �N���X�E�����o�ւ̃|�C���^�̕ϊ��́C�v���C�x�[�g���N���X���܂݂܂�
The member pointer conversion required access
to a private base class.  The access check did not succeed so the
conversion is not allowed.

:MSGSYM. ERR_MEMB_PTR_PROTECTED
:MSGTXT. conversion of pointer to class member involves a protected base class
:MSGJTXT. �N���X�E�����o�ւ̃|�C���^�̕ϊ��́C�v���e�N�g���N���X���܂݂܂�
The member pointer conversion required access
to a protected base class.  The access check did not succeed so the
conversion is not allowed.

:MSGSYM. ERR_MEMB_PTR_ADDR_OF
:MSGTXT. item is neither a non-static member function nor data member
:MSGJTXT. ���ڂ́C��X�^�e�B�b�N�E�����o�֐��ł��f�[�^�E�����o�ł�����܂���
A member pointer can only be created for non-static member functions
and non-static data members.  Static members can have their address
taken just like their file scope counterparts.

:MSGSYM. ERR_MEMB_PTR_FUN_IMPOSSIBLE
:MSGTXT. function address cannot be converted to pointer to class member
:MSGJTXT. �֐��A�h���X�́C�N���X�E�����o�ւ̃|�C���^�ɕϊ��ł��܂���
:cmt JWW to look at
The indicated function address cannot be converted to pointer to class member.

:MSGSYM. ERR_MEMB_PTR_FUN_AMBIGUOUS
:MSGTXT. conversion ambiguity: [address of function] to [pointer to class member]
:MSGJTXT. �ϊ��̞B�����F[�֐��̃A�h���X]����[�N���X�E�����o�ւ̃|�C���^]��
:cmt JWW to look at
The indicated conversion is ambiguous.

:MSGSYM. ERR_MEMB_PTR_FUN_PRIVATE
:MSGTXT. addressed function is in a private base class
:MSGJTXT. �A�h���X�w�肳�ꂽ�֐����C�v���C�x�[�g���N���X�ɂ���܂�
:cmt JWW to look at
The addressed function is in a private base class.

:MSGSYM. ERR_MEMB_PTR_FUN_PROTECTED
:MSGTXT. addressed function is in a protected base class
:MSGJTXT. �A�h���X�w�肳�ꂽ�֐����C�v���e�N�g���N���X�ɂ���܂�
:cmt JWW to look at
The addressed function is in a protected base class.

:MSGSYM. ERR_UNDEFINED_CLASS_OBJECT
:MSGTXT. class for object is not defined
:MSGJTXT. �I�u�W�F�N�g�ɑ΂���N���X�́C��`����܂���
The left hand operand for the "." or ".*" operator must be of a class type
that is completely defined.
:errbad.
class C;

int fun( C& x )
{
    return x.y;     // class C not defined
}
:eerrbad.

:MSGSYM. ERR_NOT_CLASS
:MSGTXT. left expression is not a class object
:MSGJTXT. �����̓N���X�I�u�W�F�N�g�ł͂���܂���
The left hand operand for the ".*" operator must be of a class type
since member pointers can only be used with classes.

:MSGSYM. ERR_RIGHT_NOT_MEMBPTR
:MSGTXT. right expression is not a pointer to class member
:MSGJTXT. �E���̓N���X�E�����o�ւ̃|�C���^�ł͂���܂���
The right hand operand for the ".*" operator must be a
member pointer type.

:MSGSYM. ERR_MP_PTR_IMPOSSIBLE
:MSGTXT. cannot convert pointer to class of member pointer
:MSGJTXT. �����o�E�|�C���^�̃N���X�Ƀ|�C���^��ϊ����邱�Ƃ��ł��܂���
The class of the left hand operand cannot be converted to the
class of the member pointer because it is not a derived class.

:MSGSYM. ERR_MP_PTR_AMBIGUOUS
:MSGTXT. conversion ambiguity: [pointer] to [class of pointer to class member]
:MSGJTXT. �ϊ��̞B�����F[�|�C���^]����[�N���X�E�����o�ւ̃|�C���^�̃N���X]��
The class of the pointer to member is an ambiguous base class of the left
hand operand.

:MSGSYM. ERR_MP_PTR_PRIVATE
:MSGTXT. conversion of pointer to class of member pointer involves a private base class
:MSGJTXT. �����o�E�|�C���^�̃N���X�ւ̃|�C���^�̕ϊ��́C�v���C�x�[�g���N���X���܂݂܂�
The class of the pointer to member is a private base class of the left
hand operand.

:MSGSYM. ERR_MP_PTR_PROTECTED
:MSGTXT. conversion of pointer to class of member pointer involves a protected base class
:MSGJTXT. �����o�E�|�C���^�̃N���X�ւ̃|�C���^�̕ϊ��́C�v���e�N�g���N���X���܂݂܂�
The class of the pointer to member is a protected base class of the left
hand operand.

:MSGSYM. ERR_MP_OBJECT_IMPOSSIBLE
:MSGTXT. cannot convert object to class of member pointer
:MSGJTXT. �����o�E�|�C���^�̃N���X�ɃI�u�W�F�N�g��ϊ����邱�Ƃ��ł��܂���
The class of the left hand operand cannot be converted to the
class of the member pointer because it is not a derived class.

:MSGSYM. ERR_MP_OBJECT_AMBIGUOUS
:MSGTXT. conversion ambiguity: [object] to [class object of pointer to class member]
:MSGJTXT. �ϊ��̞B�����F[�I�u�W�F�N�g]����[�N���X�E�����o�ւ̃|�C���^�̃N���X�I�u�W�F�N�g]��
The class of the pointer to member is an ambiguous base class of the left
hand operand.

:MSGSYM. ERR_MP_OBJECT_PRIVATE
:MSGTXT. conversion of object to class of member pointer involves a private base class
:MSGJTXT. �����o�E�|�C���^�̃N���X�ւ̃I�u�W�F�N�g�̕ϊ��́C�v���C�x�[�g���N���X���܂݂܂�
The class of the pointer to member is a private base class of the left
hand operand.

:MSGSYM. ERR_MP_OBJECT_PROTECTED
:MSGTXT. conversion of object to class of member pointer involves a protected base class
:MSGJTXT. �����o�E�|�C���^�̃N���X�ւ̃I�u�W�F�N�g�̕ϊ��́C�v���e�N�g���N���X���܂݂܂�
The class of the pointer to member is a protected base class of the left
hand operand.

:MSGSYM. ERR_MEMB_PTR_DERIVED
:MSGTXT. conversion of pointer to class member from a derived to a base class
:MSGJTXT. �N���X�����o�ւ̃|�C���^�̔h���N���X������N���X�ւ̕ϊ��ł�
A member pointer can only be converted from a base class to a derived class.
This is the opposite of the conversion rule for pointers.

:MSGSYM. ERR_PRAG_INLINE_RECURSION
:MSGTXT. form is '#pragma inline_recursion en' where 'en' is 'on' or 'off'
:MSGJTXT. ������'#pragma inline_recursion en'�ł��B������'en'��'on'�܂���'off'�ł�
This
.kw pragma
indicates whether inline expansion will occur for an inline
function which is called (possibly indirectly) a subsequent time
during an inline expansion.
Either 'on' or 'off' must be specified.

:MSGSYM. ERR_NEW_ARRAY_EXPRESSION
:MSGTXT. expression for number of array elements must be integral
:MSGJTXT. �z��v�f�̐��ɑ΂��鎮�́C�����łȂ���΂Ȃ�܂���
The expression for the number of elements in a
.kw new
expression must be integral because it is used to calculate the size
of the allocation (which is an integral quantity).  The compiler will
not automatically convert to an integer because of rounding and
truncation issues with floating-point values.

:MSGSYM. ERR_MEMB_PTR_FUNC_NOT_CALLED
:MSGTXT. function accessed with '.*' or '->*' can only be called
:MSGJTXT. '.*'��'->*'�ŃA�N�Z�X���ꂽ�֐��́C�Ăяo�����݂̂ł�
The result of the ".*" and "->*" operators can only be called
because it is often specific to the instance used for the left hand
operand.

:MSGSYM. ERR_LEFT_NOT_PTR_ARITH_MP
:MSGTXT. left operand must be a pointer, pointer to class member, or arithmetic
:MSGJTXT. ���I�y�����h�́C�|�C���^�C�N���X�E�����o�ւ̃|�C���^���邢�͎Z�p���Z�^�łȂ���΂Ȃ�܂���
:cmt JWW to look at
The left operand must be a pointer, pointer to class member, or arithmetic.

:MSGSYM. ERR_RIGHT_NOT_PTR_ARITH_MP
:MSGTXT. right operand must be a pointer, pointer to class member, or arithmetic
:MSGJTXT. �E�I�y�����h�́C�|�C���^�C�N���X�E�����o�ւ̃|�C���^���邢�͎Z�p���Z�^�łȂ���΂Ȃ�܂���
:cmt JWW to look at
The right operand must be a pointer, pointer to class member, or arithmetic.

:MSGSYM. ERR_MEMB_PTR_CMP_NOT_DERIVED
:MSGTXT. neither pointer to class member can be converted to the other
:MSGJTXT. �N���X�E�����o�ւ̂ǂ���̃|�C���^���C��������ɕϊ��ł��܂���
The two member pointers being compared are from two unrelated classes.
They cannot be compared since their members can never be related.

:MSGSYM. ERR_LEFT_MEMB_PTR_OPERAND
:MSGTXT. left operand is not a valid pointer to class member
:MSGJTXT. ���I�y�����h�́C�N���X�E�����o�ւ̗L���ȃ|�C���^�ł͂���܂���
The specified operator requires a pointer to member as the left operand.
:errbad.
struct S;
void fn( int S::* mp, int *p )
{
    if( p == mp )
    p[0] = 1;
}
:eerrbad.

:MSGSYM. ERR_RIGHT_MEMB_PTR_OPERAND
:MSGTXT. right operand is not a valid pointer to class member
:MSGJTXT. �E�I�y�����h�́C�N���X�E�����o�ւ̗L���ȃ|�C���^�ł͂���܂���
The specified operator requires a pointer to member as the right operand.
:errbad.
struct S;
void fn( int S::* mp, int *p )
{
    if( mp == p )
    p[0] = 1;
}
:eerrbad.

:MSGSYM. ERR_MEMB_PTR_DEREF_ZERO
:MSGTXT. cannot use '.*' nor '->*' with pointer to class member with zero value
:MSGJTXT. �l��0�̃N���X�E�����o�ւ̃|�C���^�ƂƂ��ɁC'.*'��'->*'���g�����Ƃ��ł��܂���
The compiler has detected a NULL pointer use with a member pointer
dereference.

:MSGSYM. ERR_MEMB_PTR_OPERAND
:MSGTXT. operand is not a valid pointer to class member
:MSGJTXT. �I�y�����h�́C�N���X�E�����o�ւ̗L���ȃ|�C���^�ł͂���܂���
The operand cannot be converted to a valid pointer to class member.
:errbad.
struct S;
int S::* fn()
{
    int a;
    return a;
}
:eerrbad.

:MSGSYM. ERR_DTOR_NO_OBJECT
:MSGTXT. destructor can be invoked only with '.' or '->'
:MSGJTXT. �f�X�g���N�^��'.'��'->'�ŋN���ł��܂�
This is a restriction in the C++ language.  An explicit invocation
of a destructor is not recommended for objects that have their
destructor called automatically.

:MSGSYM. ERR_DTOR_NOT_SAME
:MSGTXT. class of destructor must be class of object being destructed
:MSGJTXT. �f�X�g���N�^�̃N���X�́C���������I�u�W�F�N�g�̃N���X�łȂ���΂Ȃ�܂���
Destructors can only be called for the exact static type of the object being
destroyed.

:MSGSYM. ERR_DTOR_BAD_QUAL
:MSGTXT. destructor is not properly qualified
:MSGJTXT. �f�X�g���N�^�͐������C������Ă��܂���
An explicit destructor invocation can only be qualified with its own
class.

:MSGSYM. ERR_MEMB_PTR_OBJS_MISMATCH
:MSGTXT. pointers to class members reference different object types
:MSGJTXT. �N���X�E�����o�ւ̃|�C���^�͈قȂ�I�u�W�F�N�g�^���Q�Ƃ��܂�
Conversion of member pointers can only occur if the object types
are identical.  This is necessary to ensure type safety.

:MSGSYM. ERR_NOT_CLASS_PTR
:MSGTXT. operand must be pointer to class or struct
:MSGJTXT. �I�y�����h�́Cclass��struct�ւ̃|�C���^�łȂ���΂Ȃ�܂���
The left hand operand of a '->*' operator must be a pointer to a class.
This is a restriction in the C++ language.

:MSGSYM. ERR_NOT_VOID
:MSGTXT. expression must have void type
:MSGJTXT. ����'void'�^�������Ȃ���΂Ȃ�܂���
If one operand of the ':' operator has
.kw void
type, then the other operand must also have
.kw void
type.

:MSGSYM. ERR_BAD_COLON_OPERANDS
:MSGTXT. expression types do not match for ':' operator
:MSGJTXT. ���̌^��'�F'���Z�q�ɑ΂��Ĉ�v���܂���
The compiler could not bring both operands to a common type.
This is necessary because the result of the conditional
operator must be a unique type.

:MSGSYM. ERR_CANT_NEW_UNDEFD
:MSGTXT. cannot create an undefined type with 'operator new'
:MSGJTXT. ����`�̌^��'operator new'�ō�邱�Ƃ��ł��܂���
A
.kw new
expression cannot allocate an undefined type because it must know how
large an allocation is required and it must also know whether there
are any constructors to execute.

:MSGSYM. WARN_CANT_DEL_UNDEFD
:MSGTXT. delete of a pointer to an undefined type
:MSGJTXT. delete���Z�q�Ŗ���`�^�ւ̃|�C���^���폜���Ă��܂�
:WARNING. 1
A
.kw delete
expression cannot safely deallocate an undefined type
because it must know whether there are any destructors to execute.
In spite of this, the ISO/ANSI C++ Working Paper requires
that an implementation support this usage.
:errbad.
struct U;

void foo( U *p, U *q ) {
    delete p;
    delete [] q;
}
:eerrbad.

:MSGSYM. ERR_ACCESS_THROUGH_PRIVATE
:MSGTXT. cannot access '%S' through a private base class
:MSGJTXT. �v���C�x�[�g���N���X��ʂ���'%S'�ɃA�N�Z�X���邱�Ƃ��ł��܂���
The indicated symbol cannot be accessed because it requires
access to a private base class.

:MSGSYM. ERR_ACCESS_THROUGH_PROTECTED
:MSGTXT. cannot access '%S' through a protected base class
:MSGJTXT. �v���e�N�g���N���X��ʂ���'%S'�ɃA�N�Z�X���邱�Ƃ��ł��܂���
The indicated symbol cannot be accessed because it requires
access to a protected base class.

:MSGSYM. WARN_CLASS_HAS_SPECIAL_FIELDS
:MSGTXT. 'sizeof' operand contains compiler generated information
:MSGJTXT. 'sizeof'�I�y�����h�́C�R���p�C�����������ꂽ�����܂݂܂�
:WARNING. 3
The type used in the 'sizeof' operand contains compiler generated
information.  Clearing a struct with a call to memset() would
invalidate all of this information.

:MSGSYM. ERR_COLON_REF_CNV_IMPOSSIBLE
:MSGTXT. cannot convert ':' operands to a common reference type
:MSGJTXT. '�F'�̃I�y�����h�����ʂ̎Q�ƌ^�֕ϊ����邱�Ƃ��ł��܂���
The two reference types cannot be converted to a common reference type.
This can happen when the types are not related through base class inheritance.

:MSGSYM. ERR_COLON_REF_CNV_AMBIGUOUS
:MSGTXT. conversion ambiguity: [reference to object] to [type of opposite ':' operand]
:MSGJTXT. �ϊ��̞B�����F[�I�u�W�F�N�g�ւ̎Q��]���� [�����΂̌^�I�y�����h�ɂƂ���]��
One of the reference types is an ambiguous base class of the other.
This prevents the compiler from converting the operand to a unique
common type.

:MSGSYM. ERR_COLON_REF_CNV_PRIVATE
:MSGTXT. conversion of reference to ':' object involves a private base class
:MSGJTXT. '�F'�I�u�W�F�N�g�ւ̎Q�Ƃ̕ϊ��́C�v���C�x�[�g���N���X���܂݂܂�
The conversion of the reference operands requires a conversion through
a private base class.

:MSGSYM. ERR_COLON_REF_CNV_PROTECTED
:MSGTXT. conversion of reference to ':' object involves a protected base class
:MSGJTXT. '�F'�I�u�W�F�N�g�ւ̎Q�Ƃ̕ϊ��́C�v���e�N�g���N���X���܂݂܂�
The conversion of the reference operands requires a conversion through
a protected base class.

:MSGSYM. ERR_NOT_BOOLEAN
:MSGTXT. expression must have type arithmetic, pointer, or pointer to class member
:MSGJTXT. ���́C�Z�p���Z�^�C�|�C���^�܂��̓N���X�E�����o�ւ̃|�C���^�������Ȃ���΂Ȃ�܂���
This message means that the type cannot be converted to any of these
types, also.  All of the mentioned types can be compared against zero ('0')
to produce a true or false value.

:MSGSYM. WARN_WHILE_FALSE
:MSGTXT. expression for 'while' is always false
:MSGJTXT. 'while'�ɑ΂��鎮�́C��ɋU�ł�
:WARNING. 3
The compiler has detected that the expression will always be false.
If this is not the expected behaviour, the code may contain a
comparison of an unsigned value against zero (e.g., unsigned
integers are always greater than or equal to zero).  Comparisons
against zero for addresses can also result in trivially false expressions.

:MSGSYM. WARN_FOR_FALSE
:MSGTXT. testing expression for 'for' is always false
:MSGJTXT. 'for'�ɑ΂��鎮�̃e�X�g�́C��ɋU�ł�
:WARNING. 3
The compiler has detected that the expression will always be false.
If this is not the expected behaviour, the code may contain a
comparison of an unsigned value against zero (e.g., unsigned
integers are always greater than or equal to zero).  Comparisons
against zero for addresses can also result in trivially false expressions.

:MSGSYM. ERR_PRAG_WARNING_BAD_MESSAGE
:MSGTXT. message number '%d' is invalid
:MSGJTXT. ���b�Z�[�W�ԍ�'%d'�͕s�K�؂ł�
The message number used in the #pragma does not match the message number
for any warning message.  This message can also indicate that a number
or '*' (meaning all warnings) was not found when it was expected.

:MSGSYM. ERR_PRAG_WARNING_BAD_LEVEL
:MSGTXT. warning level must be an integer in range 0 to 9
:MSGJTXT. �x�����x���́C0�`9�͈̔͂̐����łȂ���΂Ȃ�܂���
The new warning level that can be used for the warning can be in the range
0 to 9.  The level 0 means that the warning will be treated as an error
(compilation will not succeed).  Levels 1 up to 9 are used to classify
warnings.  The -w option sets an upper limit on the level for warnings.
By setting the level above the command line limit, you effectively
ignore all cases where the warning shows up.

:MSGSYM. ERR_CANNOT_DEFINE_DEFAULT
:MSGTXT. function '%S' cannot be defined because it is generated by the compiler
:MSGJTXT. �֐�'%S'�́C�R���p�C���ɂ���Đ�������܂��̂ŁC��`�ł��܂���
The indicated function cannot be defined because it is generated by the compiler.
The compiler will automatically generate default constructors, copy constructors,
assignment operators, and destructors according to the rules of the C++ language.
This message indicates that you did not declare the function in the class
definition.

:MSGSYM. ERR_BAD_CMD_INDIRECTION
:MSGTXT. neither environment variable nor file found for '@' name
:MSGJTXT. '@' name �Ŏw�肳�ꂽ���ϐ����t�@�C����������܂���
The indirection operator for the command line will first check for an
environment variable of the name and use the contents for the command line.
If an environment variable is not found, a check for a file
with the same name will occur.

:MSGSYM. ERR_MAX_CMD_INDIRECTION
:MSGTXT. more than 5 indirections during command line processing
:MSGJTXT. �R�}���h���C����������6�ȏ�̃l�X�g���ꂽ�ԐڎQ�Ƃ�����܂�
The Open Watcom C++ compiler only allows a fixed number nested indirections using
files or environment variables,
to prevent runaway chains of indirections.

:MSGSYM. ERR_ADDR_NONSTAT_MEMBER_FUNC
:MSGTXT. cannot take address of non-static member function
:MSGJTXT. ��X�^�e�B�b�N�E�����o�֐��̃A�h���X���Ƃ邱�Ƃ��ł��܂���
The only way to create a value that described the non-static member function
is to use a member pointer.

:MSGSYM. ERR_CANNOT_GENERATE_DEFAULT
:MSGTXT. cannot generate default '%S' because class contains either a constant or a reference member
:MSGJTXT. �N���X���萔���Q�ƃ����o���܂݂܂��̂ŁC�f�t�H���g'%S'�𐶐��ł��܂���
An assignment operator cannot be generated because the class contains
members that cannot be assigned into.

:MSGSYM. ERR_CNV_VOID_STAR_CONST
:MSGTXT. cannot convert pointer to non-constant or volatile objects to pointer to const void
:MSGJTXT. ��萔���������I�u�W�F�N�g�ւ̃|�C���^��'const void*'�ɕϊ����邱�Ƃ��ł��܂���
:cmt JWW to look at
A pointer to non-constant or volatile objects cannot be converted
to 'const void*'.

:MSGSYM. ERR_CNV_VOID_STAR_CONST_VOLATILE
:MSGTXT. cannot convert pointer to non-constant or non-volatile objects to pointer to const volatile void
:MSGJTXT. ��萔���s�������I�u�W�F�N�g�ւ̃|�C���^��'const volatile void*'�ɕϊ����邱�Ƃ��ł��܂���
:cmt JWW to look at
A pointer to non-constant or non-volatile objects cannot be converted
to 'const volatile void*'.

:MSGSYM. ERR_VOLATILE_PTR_INIT
:MSGTXT. cannot initialize pointer to non-volatile with a pointer to volatile
:MSGJTXT. �s�������I�u�W�F�N�g�ւ̃|�C���^���������ւ̃|�C���^�ŏ�����Ԃɂ��邱�Ƃ��ł��܂���
A pointer to a non-volatile type cannot be initialized with a pointer to
a volatile type because this would allow volatile data to be modified
without volatile semantics via the non-volatile pointer to it.

:MSGSYM. ERR_VOLATILE_PTR_ARG
:MSGTXT. cannot pass a pointer or reference to a volatile object
:MSGJTXT. �������I�u�W�F�N�g�ւ̃|�C���^���Q�Ƃ�n�����Ƃ��ł��܂���
:cmt JWW to look at
A pointer or reference to a volatile object cannot be passed in this context.

:MSGSYM. ERR_VOLATILE_PTR_RETURN
:MSGTXT. cannot return a pointer or reference to a volatile object
:MSGJTXT. �������I�u�W�F�N�g�ւ̃|�C���^���Q�Ƃ�߂����Ƃ��ł��܂���
:cmt JWW to look at
A pointer or reference to a volatile object cannot be returned.

:MSGSYM. ERR_LEFT_NOT_VOLATILE_PTR
:MSGTXT. left expression is not a pointer to a volatile object
:MSGJTXT. ���̎��́C�������I�u�W�F�N�g�ւ̃|�C���^�ł͂���܂���
One cannot assign a pointer to a volatile type to a pointer to a
non-volatile type.  This would allow a volatile object to be modified
via the non-volatile pointer.  Use a cast if this is absolutely
necessary.

:MSGSYM. ERR_VFTABLE_CONSTRUCTION_AMBIGUITY
:MSGTXT. virtual function override for '%S' is ambiguous
:MSGJTXT. '%S'���I�[�o�[���C�h���鉼�z�֐��͞B���ł�
This message indicates that there are at least two overrides for the function
in the base class.  The compiler cannot arbitrarily choose one so it
is up to the programmer to make sure there is an unambiguous choice.
Two of the overriding functions are listed as informational messages.

:MSGSYM. ERR_PRAG_INITIALIZE_PRIORITY
:MSGTXT. initialization priority must be number 0-255, 'library', or 'program'
:MSGJTXT. �������D�揇�ʂ́C�ԍ�0-255�C'library'��'program'�łȂ���΂Ȃ�܂���
An incorrect module initialization priority has been provided.
Check the User's Guide for the correct format of the priority directive.

:MSGSYM. INF_PREVIOUS_CASE
:MSGTXT. previous case label defined %L
:MSGJTXT. �O��'case'���x���́C���Œ�`����Ă��܂��F%L
:INFO.
This informational message indicates where a preceding
.kw case
label is defined.

:MSGSYM. INF_PREVIOUS_DEFAULT
:MSGTXT. previous default label defined %L
:MSGJTXT. �O��'default'���x���́C���Œ�`����Ă��܂��F%L
:INFO.
This informational message indicates where a preceding
.kw default
label is defined.

:MSGSYM. INF_PREVIOUS_LABEL
:MSGTXT. label defined %L
:MSGJTXT. ���x���́C���Œ�`����Ă��܂��F%L
:INFO.
This informational message indicates where a label is defined.

:MSGSYM. INF_PREVIOUS_LABEL_REF
:MSGTXT. label referenced %L
:MSGJTXT. ���x���́C���Œ�`����Ă��܂��F%L
:INFO.
This informational message indicates where a label is referenced.

:MSGSYM. INF_THROW_TYPE
:MSGTXT. object thrown has type: %T
:MSGJTXT. ���o���ꂽ�I�u�W�F�N�g�́C���̌^�������܂��F%T
:INFO.
This informational message indicates the type of the object being thrown.

:MSGSYM. ERR_THR_AMBIGUOUS_CLASS
:MSGTXT. object thrown has an ambiguous base class %T
:MSGJTXT. ���o���ꂽ�I�u�W�F�N�g�́C�B���Ȋ��N���X%T�������܂�
It is illegal to throw an object with a base class to which a conversion
would be ambiguous.
:errbad.
struct ambiguous{ };
struct base1 : public ambiguous { };
struct base2 : public ambiguous { };
struct derived : public base1, public base2 { };

foo( derived &object )
{
    throw object;
}
:eerrbad.
.np
The
.kw throw
will cause an error to be displayed because an object of type "derived"
cannot be converted to an object of type "ambiguous".

:MSGSYM. ERR_PRAG_INLINE_DEPTH
:MSGTXT. form is '#pragma inline_depth level' where 'level' is 0 to 255
:MSGJTXT. �����́C'#pragma inline_depth level'�ł��B������'level'��0�`255�ł�
This
.kw pragma
sets the number of times inline expansion will occur for an inline
function which contains calls to inline functions.
The level must be a number from zero to 255.
When the level is zero, no inline expansion occurs.

:MSGSYM. WARN_POINTER_TRUNCATION_CAST
:MSGTXT. pointer or reference truncated by cast
:MSGJTXT. �|�C���^���Q�Ƃ̓L���X�g�ɂ���Đ؂�l�߂��܂���
:WARNING. 10
The cast expression causes a conversion of a pointer value to another
pointer value of smaller size.  This can be caused by
.kw __near
or
.kw __far
qualifiers (i.e., casting a
.kw far
pointer to a
.kw near
pointer).
Function pointers can also have a different size than data pointers
in certain memory models.
Because this message indicates that some information is being lost, check the
code carefully.

:MSGSYM. ERR_NO_CTOR_FOR_NEW
:MSGTXT. cannot find a constructor for given initializer argument list
:MSGJTXT. �^����ꂽ�������������X�g�̂��߂̃R���X�g���N�^������܂���
The initializer list provided for the
.kw new
expression does not uniquely identify a single constructor.

:MSGSYM. ERR_CANT_HAVE_BASED_VARIABLE
:MSGTXT. variable '%N' can only be based on a string in this context
:MSGJTXT. �ϐ�'%N'�́C���̃R���e�L�X�g�ɂ����āC��������x�[�X�ɂł��邾���ł�
All of the based modifiers can only be applied to pointer types.
The only based modifier that can be applied to non-pointer types
is the '__based(__segname("WATCOM"))' style.

:MSGSYM. ERR_MEM_MODEL_CLASS
:MSGTXT. memory model modifiers are not allowed for class members
:MSGJTXT. �������E���f���C���q�́C�N���X�E�����o�ɑ΂��ċ�����܂���
Class members describe the arrangement and interpretation of memory
and, as such, assume the memory model of the address used to access
the member.

:MSGSYM. WARN_BENIGN_TYPEDEF_REDEFN
:MSGTXT. redefinition of the typedef name '%S' ignored
:MSGJTXT. typedef��'%S'�̍Ē�`�𖳎����܂���
:WARNING. 2
The compiler has detected that a slightly different type has been assigned
to a typedef name.  The type is functionally equivalent but typedef
redefinitions should be precisely identical.

:MSGSYM. ERR_CTOR_BYPASSED
:MSGTXT. constructor for variable '%S' cannot be bypassed
:MSGJTXT. �ϐ�'%S'�̃R���X�g���N�^�̓o�C�p�X�ł��܂���
The variable may not be constructed when code is executing
at the position the message indicated.  The C++ language
places these restrictions to prevent the use of unconstructed
variables.

:MSGSYM. ERR_MEM_INIT_REWRITE_ERROR
:MSGTXT. syntax error; missing start of function body after constructor initializer
:MSGJTXT. �\���G���[�G�R���X�g���N�^�������̌�̊֐��̖{�̂̃X�^�[�g������܂���
Member initializers can only be used in a constructor's definition.
:errgood.
struct S {
    int a;
    S( int x = 1 ) : a(x)
    {
    }
};
:eerrgood.

:MSGSYM. ERR_DEFARG_AMBIGUOUS
:MSGTXT. conversion ambiguity: [expression] to [type of default argument]
:MSGJTXT. �ϊ��̞B�����F[��]����[�f�t�H���g�����̌^]��
A conversion to an ambiguous base class was detected in the default
argument expression.

:MSGSYM. ERR_DEFARG_IMPOSSIBLE
:MSGTXT. conversion of expression for default argument is impossible
:MSGJTXT. �f�t�H���g�����ɑ΂��鎮�̕ϊ��́C�s�\�ł�
A conversion to a unrelated class was detected in the default
argument expression.

:MSGSYM. ERR_SYNTAX_TEMPLATE_NAME
:MSGTXT. syntax error before template name '%s'
:MSGJTXT. �e���v���[�g��'%s'�̑O�̍\���G���[
The identifier in the error message has been declared as a template name
at this point in the code.  This may be the cause of the syntax error.

:MSGSYM. ERR_DEFARG_PRIVATE
:MSGTXT. private base class accessed to convert default argument
:MSGJTXT. �f�t�H���g������ϊ����邽�߂Ƀv���C�x�[�g���N���X�̓A�N�Z�X����܂���
A conversion to a private base class was detected in the default
argument expression.

:MSGSYM. ERR_DEFARG_PROTECTED
:MSGTXT. protected base class accessed to convert default argument
:MSGJTXT. �f�t�H���g������ϊ����邽�߂Ƀv���e�N�g���N���X�̓A�N�Z�X����܂���
A conversion to a protected base class was detected in the default
argument expression.

:MSGSYM. ERR_MUST_BE_LVALUE_CAST
:MSGTXT. operand must be an lvalue (cast produces rvalue)
:MSGJTXT. �I�y�����h�́C'���Ӓl'�łȂ���΂Ȃ�܂���i�L���X�g��'�E�Ӓl'�𐶂��܂��j
The compiler is expecting a value which can be assigned into.
The result of a cast cannot be assigned into because a brand new
value is always created.  Assigning a new value to a temporary
is a meaningless operation.

:MSGSYM. ERR_LEFT_MUST_BE_LVALUE_CAST
:MSGTXT. left operand must be an lvalue (cast produces rvalue)
:MSGJTXT. ���I�y�����h�́C'���Ӓl'�łȂ���΂Ȃ�܂���i�L���X�g��'�E�Ӓl'�𐶂��܂��j
The compiler is expecting a value which can be assigned into.
The result of a cast cannot be assigned into because a brand new
value is always created.  Assigning a new value to a temporary
is a meaningless operation.

:MSGSYM. ERR_RIGHT_MUST_BE_LVALUE_CAST
:MSGTXT. right operand must be an lvalue (cast produces rvalue)
:MSGJTXT. �E�I�y�����h�́C'���Ӓl'�łȂ���΂Ȃ�܂���i�L���X�g��'�E�Ӓl'�𐶂��܂��j
The compiler is expecting a value which can be assigned into.
The result of a cast cannot be assigned into because a brand new
value is always created.  Assigning a new value to a temporary
is a meaningless operation.

:MSGSYM. WARN_AMBIGUOUS_CONSTRUCT_DECL
:MSGTXT. construct resolved as a declaration/type
:MSGJTXT. �R���X�g���N�g�͐錾/�^�Ƃ��ĉ�������܂���
:WARNING. 9
The C++ language contains language ambiguities that force compilers to
rely on extra information in order to understand certain language
constructs.  The extra information required to disambiguate the
language can be deduced by looking ahead in the source file.
Once a single interpretation has been found, the compiler can continue
analysing source code.  See the ARM p.93 for more details.

This warning is intended to inform the programmer that an ambiguous
construct has been resolved in a certain direction.
In this case, the construct has been determined to be part of a type.
The final resolution varies between compilers
so it is wise to change the source code so that the construct is not
ambiguous.  This is especially important in cases where the resolution
is more than three tokens away from the start of the ambiguity.

:MSGSYM. WARN_AMBIGUOUS_CONSTRUCT_EXPR
:MSGTXT. construct resolved as an expression
:MSGJTXT. �R���X�g���N�g�͎��Ƃ��ĉ�������܂���
:WARNING. 9
The C++ language contains language ambiguities that force compilers to
rely on extra information in order to understand certain language
constructs.  The extra information required to disambiguate the
language can be deduced by looking ahead in the source file.
Once a single interpretation has been found, the compiler can continue
analysing source code.  See the ARM p.93 for more details.

This warning is intended to inform the programmer that an ambiguous
construct has been resolved in a certain direction.
In this case, the construct has been determined to be part of an expression
(a function-like cast).
The final resolution varies between compilers
so it is wise to change the source code so that the construct is not
ambiguous.  This is especially important in cases where the resolution
is more than three tokens away from the start of the ambiguity.

:MSGSYM. WARN_AMBIGUOUS_CONSTRUCT_UNKNOWN
:MSGTXT. construct cannot be resolved
:MSGJTXT. �R���X�g���N�g�͉�������܂���
:WARNING. 9
The C++ language contains language ambiguities that force compilers to
rely on extra information in order to understand certain language
constructs.  The extra information required to disambiguate the
language can be deduced by looking ahead in the source file.
Once a single interpretation has been found, the compiler can continue
analysing source code.  See the ARM p.93 for more details.

This warning is intended to inform the programmer that an ambiguous
construct could not be resolved by the compiler.
Please report this to the Open Watcom developement team so that the
problem can be analysed. See http://www.openwatcom.org/.

:MSGSYM. WARN_AMBIGUOUS_CONSTRUCT_AGAIN
:MSGTXT. encountered another ambiguous construct during disambiguation
:MSGJTXT. �B��������������r���ŁC������̞B���ȃR���X�g���N�g������܂���
:WARNING. 9
The C++ language contains language ambiguities that force compilers to
rely on extra information in order to understand certain language
constructs.  The extra information required to disambiguate the
language can be deduced by looking ahead in the source file.
Once a single interpretation has been found, the compiler can continue
analysing source code.  See the ARM p.93 for more details.

This warning is intended to inform the programmer that another ambiguous
construct was found inside an ambiguous construct.  The compiler will
correctly disambiguate the construct.  The programmer is advised to
change code that exhibits this warning because this is definitely
uncharted territory in the C++ language.

:MSGSYM. WARN_ELLIPSIS_CLASS_ARG
:MSGTXT. ellipsis (...) argument contains compiler generated information
:MSGJTXT. �ȗ��L���i...�j�����́C�R���p�C�����������������܂݂܂�
:WARNING. 1
A class with virtual functions or virtual bases is being
passed to a function that will not know the type of the argument.
Since this information can be encoded in a variety of ways,
the code may not be portable to another environment.
:errbad.
struct S
{   virtual int foo();
};

static S sv;

extern int bar( S, ... );

static int test = bar( sv, 14, 64 );
:eerrbad.
.np
The call to "bar" causes a warning, since the structure S contains
information associated with the virtual function for that class.

:MSGSYM. ERR_ELLIPSIS_IMPOSSIBLE
:MSGTXT. cannot convert argument for ellipsis (...) argument
:MSGJTXT. �ȗ��L���i...�j�����ɑ΂��������ϊ����邱�Ƃ��ł��܂���
:cmt JWW to look at
This argument cannot be used as an ellipsis (...) argument to a function.

:MSGSYM. ERR_ELLIPSIS_AMBIGUOUS
:MSGTXT. conversion ambiguity: [argument] to [ellipsis (...) argument]
:MSGJTXT. �ϊ��̞B�����F[����]����[�ȗ��L���i...�j����]��
:cmt JWW to look at
A conversion ambiguity was detected while converting an argument
to an ellipsis (...) argument.

:MSGSYM. WARN_CNV_FUNC_PRAGMA
:MSGTXT. converted function type has different #pragma from original function type
:MSGJTXT. �ϊ����ꂽ�֐��^�́C�I���W�i���̊֐��^�ƈقȂ�#pragma�������܂�
:WARNING. 1
Since a #pragma can affect calling conventions, one must be very careful
performing casts involving different calling conventions.

:MSGSYM. WARN_CNV_PRO_CLASS_VALUE
:MSGTXT. class value used as return value or argument in converted function type
:MSGJTXT. �N���X�l���ϊ����ꂽ�֐��^�̒��Ŗ߂�l�������Ƃ��Ďg���܂�
:WARNING. 1
The compiler has detected a cast between "C" and "C++" linkage function
types.  The calling conventions are different because of the different
language rules for copying structures.

:MSGSYM. WARN_CNV_ARG_CLASS_VALUE
:MSGTXT. class value used as return value or argument in original function type
:MSGJTXT. �N���X�l���I���W�i���̊֐��^�̒��Ŗ߂�l�������Ƃ��Ďg���܂�
:WARNING. 1
The compiler has detected a cast between "C" and "C++" linkage function
types.  The calling conventions are different because of the different
language rules for copying structures.

:MSGSYM. WARN_AMBIGUOUS_CONSTRUCT
:MSGTXT. must look ahead to determine whether construct is a declaration/type or an expression
:MSGJTXT. �R���X�g���N�g���錾/�^�����ł��邩�ǂ������肷�邽�߂ɑO���Q�Ƃ��Ȃ���΂Ȃ�܂���
:WARNING. 9
The C++ language contains language ambiguities that force compilers to
rely on extra information in order to understand certain language
constructs.  The extra information required to disambiguate the
language can be deduced by looking ahead in the source file.
Once a single interpretation has been found, the compiler can continue
analysing source code.  See the ARM p.93 for more details.

This warning is intended to inform the programmer that an ambiguous
construct has been used.  The final resolution varies between compilers
so it is wise to change the source code so that the construct is not
ambiguous.

:MSGSYM. ERR_ASSEMBLER_ERROR
:MSGTXT. assembler: '%s'
:MSGJTXT. �A�Z���u��: '%s'
An error has been detected by the #pragma inline assembler.

:MSGSYM. ERR_DEFAULT_ARG_USES_THIS
:MSGTXT. default argument expression cannot reference 'this'
:MSGJTXT. �f�t�H���g�������́C'this'���Q�Ƃł��܂���
The order of evaluation for function arguments is unspecified in the
C++ language document.  Thus, a default argument must be able to
be evaluated before the 'this' argument (or any other
argument) is evaluated.

:MSGSYM. ERR_PRAGMA_AUX_CANNOT_OVERLOAD
:MSGTXT. #pragma aux must reference a "C" linkage function '%S'
:MSGJTXT. #pragma aux�́C"�b"�����P�[�W�֐�'%S'���Q�Ƃ��Ȃ���΂Ȃ�܂���
The method of assigning pragma information via the #pragma syntax
is provided for compatibility with Open Watcom C.  Because C only
allows one function per name, this was adequate for the C language.
Since C++ allows functions to be overloaded, a new method of
referencing pragmas has been introduced.
:errgood.
#pragma aux this_in_SI parm caller [si] [ax];

struct S {
    void __pragma("this_in_SI") foo( int );
    void __pragma("this_in_SI") foo( char );
};
:eerrgood.

:MSGSYM. ERR_ASSIGN_AMBIGUOUS
:MSGTXT. assignment is ambiguous for operands used
:MSGJTXT. �g����I�y�����h�̂��߁C������B���ł�
An ambiguity was detected while attempting to convert the right operand
to the type of the left operand.
:errbad.
struct S1 {
    int a;
};

struct S2 : S1 {
    int b;
};

struct S3 : S2, S1 {
    int c;
};

S1* fn( S3 *p )
{
    return p;
}
:eerrbad.
.np
In the example,
.kw class
.id S1
occurs ambiguously for an object or pointer to an object of type
.id S3.
A pointer to an
.id S3
object cannot be converted to a pointer to an
.id S1
object.

:MSGSYM. ERR_PRAGMA_NOT_FOUND
:MSGTXT. pragma name '%s' is not defined
:MSGJTXT. �v���O�}��'%s'�͒�`����܂���
Pragmas are defined with the #pragma aux syntax.  See the User's
Guide for the details of defining a pragma name.  If the pragma
has been defined then check the spelling between the definition
and the reference of the pragma name.

:MSGSYM. ERR_DURING_GEN
:MSGTXT. '%S' could not be generated by the compiler
:MSGJTXT. '%S'�̓R���p�C���ɂ���Đ����ł��܂���ł���
An error occurred while the compiler tried to generate the specified
function.  The error prevented the compiler from generating the
function properly so the compilation cannot continue.

:MSGSYM. ERR_MISPLACED_CATCH
:MSGTXT. 'catch' does not immediately follow a 'try' or 'catch'
:MSGJTXT. 'catch'�́C'try'�܂���'catch'�̂�����ɂ͑����܂���
The catch handler syntax must be used in conjunction with a try block.
:errgood.
void f()
{
    try {
    // code that may throw an exception
    } catch( int x ) {
    // handle 'int' exceptions
    } catch( ... ) {
    // handle all other exceptions
    }
}
:eerrgood.

:MSGSYM. ERR_CATCH_FOLLOWS_ELLIPSIS
:MSGTXT. preceding catch specified '...'
:MSGJTXT. '...'���w�肵��'catch'���O�ɂ���܂�
Since an ellipsis "..." catch handler will handle any type
of exception, no further catch handlers can exist afterwards
because they will never execute.  Reorder the catch handlers
so that the "..." catch handler is the last handler.

:MSGSYM. WARN_EXTERN_C_CLASS_ARG
:MSGTXT. argument to extern "C" function contains compiler generated information
:MSGJTXT. extern "C"�֐��ւ̈����́C�R���p�C�����������ꂽ�����܂݂܂�
:WARNING. 1
A class with virtual functions or virtual bases is being
passed to a function that will not know the type of the argument.
Since this information can be encoded in a variety of ways,
the code may not be portable to another environment.
:errbad.
struct S
{   virtual int foo();
};

static S sv;

extern "C" int bar( S );

static int test = bar( sv );
:eerrbad.
.np
The call to "bar" causes a warning, since the structure S contains
information associated with the virtual function for that class.

:MSGSYM. INF_PREVIOUS_TRY
:MSGTXT. previous try block defined %L
:MSGJTXT. �O�̃g���C�E�u���b�N�́C���Œ�`����Ă��܂��F%L
:INFO.
This informational message indicates where a preceding
.kw try
block is defined.

:MSGSYM. INF_PREVIOUS_CATCH
:MSGTXT. previous catch block defined %L
:MSGJTXT. �O�̃L���b�`�E�u���b�N�́C���Œ�`����Ă��܂��F%L
:INFO.
This informational message indicates where a preceding
.kw catch
block is defined.

:MSGSYM. WARN_CATCH_PREVIOUS
:MSGTXT. catch handler can never be invoked
:MSGJTXT. 'catch'�n���h���́C�����ċN������邱�Ƃ��ł��܂���
:WARNING. 1
Because the handlers for a
.kw try
block are tried in order of appearance, the type specified in a preceding
.kw catch
can ensure that the current handler will never be invoked.
This occurs when a base class (or reference) precedes a derived
class (or reference); when a pointer to a base class (or reference to the
pointer) precedes a pointer to a derived class (or reference to the
pointer); or, when "void*" or "void*&" precedes a pointer or a reference
to the pointer.
:errbad.
struct BASE {};
struct DERIVED : public BASE {};

foo()
{
    try {
    // code for try
    } catch( BASE b ) {     // [1]
    // code
    } catch( DERIVED ) {    // warning: [1]
    // code
    } catch( BASE* pb ) {   // [2]
    // code
    } catch( DERIVED* pd ) {// warning: [2]
    // code
    } catch( void* pv ) {   // [3]
    // code
    } catch( int* pi ) {    // warning: [3]
    // code
    } catch( BASE& br ) {   // warning: [1]
    // code
    } catch( float*& pfr ) {// warning: [3]
    // code
    }
}
:eerrbad.
.np
Each erroneous catch specification indicates the preceding catch block
which caused the error.

:MSGSYM. ERR_ONLY_ONE_C_LINKAGE
:MSGTXT. cannot overload extern "C" functions (the other function is '%S')
:MSGJTXT. extern "C"�֐����I�[�o�[���[�h�ł��܂���i���̊֐���'%S'�ł��j
The C++ language only allows you to overload functions that are strictly
C++ functions.  The compiler will automatically generate the correct
code to distinguish each particular function based on its argument types.
The extern "C" linkage mechanism only allows you to
define one "C" function of a particular name because the C language
does not support function overloading.

:MSGSYM. WARN_FN_HITS_ANOTHER_ARG
:MSGTXT. function will be overload ambiguous with '%S' using default arguments
:MSGJTXT. �֐��́C�f�t�H���g�������g���Ă���'%S'�ŞB���ȃI�[�o�[���[�h�ł�
:WARNING. 3
The declaration declares a function that is indistinguishable from
another function of the same name with default arguments.
:errbad.
void fn( int, int = 1 );
void fn( int );
:eerrbad.
.np
Calling the function 'fn' with one argument is ambiguous because it
could match either the first 'fn' with a default argument applied or
the second 'fn' without any default arguments.

:MSGSYM. ERR_CONFLICTING_LINKAGE_SPEC
:MSGTXT. linkage specification is different than previous declaration '%S'
:MSGJTXT. �����N�d�l���O�̐錾'%S'�ƈقȂ�܂�
The linkage specification affects the binding of names throughout a
program.  It is important to maintain consistency because subtle
problems could arise when the incorrect function is called.  Usually
this error prevents an unresolved symbol error during linking because
the name of a declaration is affected by its linkage specification.
:errbad.
extern "C" void fn( void );
void fn( void )
{
}
:eerrbad.

:MSGSYM. ERR_NO_SEG_REGS
:MSGTXT. not enough segment registers available to generate '%s'
:MSGJTXT. '%s'�𐶐�����̂ɏ\���ɗ��p�ł���Z�O�����g�E���W�X�^������܂���
Through a combination of options, the number of available segment registers
is too small.   This can occur when too many segment registers are pegged.
This can be fixed by changing the command line options to only peg the
segment registers that must absolutely be pegged.

:MSGSYM. WARN_VDTOR_MUST_BE_DEFINED
:MSGTXT. pure virtual destructors must have a definition
:MSGJTXT. �������z�f�X�g���N�^�͒�`�������Ȃ���΂Ȃ�܂���
:WARNING. 10
This is an anomaly for pure virtual functions.  A destructor is
the only special function that is inherited and allowed to be
virtual.  A derived class must be able to call the base class
destructor so a pure virtual destructor must be defined in a
C++ program.

:MSGSYM. ERR_JUMP_INTO_TRY
:MSGTXT. jump into try block
:MSGJTXT. �g���C�E�u���b�N�ɔ�т܂�
Jumps cannot enter
.kw try
blocks.
:errbad.
foo( int a )
{
    if(a) goto tr_lab;

    try {
tr_lab:
    throw 1234;
    } catch( int ) {
    if(a) goto tr_lab;
    }

    if(a) goto tr_lab;
}
:eerrbad.
.np
All the preceding goto's are illegal. The error is detected at the label
for forward jumps and at the goto's for backward jumps.

:MSGSYM. ERR_JUMP_INTO_CATCH
:MSGTXT. jump into catch handler
:MSGJTXT. �L���b�`�E�n���h���ɔ�т܂�
Jumps cannot enter
.kw catch
handlers.
:errbad.
foo( int a )
{
    if(a)goto ca_lab;

    try {
    if(a)goto ca_lab;
    } catch( int ) {
ca_lab:
    }

    if(a)goto ca_lab;
}
:eerrbad.
.np
All the preceding goto's are illegal. The error is detected at the label
for forward jumps and at the goto's for backward jumps.

:MSGSYM. ERR_CATCH_MISSING
:MSGTXT. catch block does not immediately follow try block
:MSGJTXT. �L���b�`�E�u���b�N�̓g���C�E�u���b�N�̒���ɂ͒u���܂���
At least one
.kw catch
handler must immediately follow the "}" of a
.kw try
block.
:errbad.
extern void goop();
void foo()
{
    try {
    goop();
    }       // a catch block should follow!
}
:eerrbad.
.np
In the example, there were no catch blocks after the
.kw try
block.

:MSGSYM. ERR_EXCEPTIONS_DISABLED
:MSGTXT. exceptions must be enabled to use feature (use 'xs' option)
:MSGJTXT. �@�\���g�p���邽�߂ɗ�O���g�p�\�ɂ��Ȃ���΂Ȃ�܂���('xs'�I�v�V�������g�p���Ă�������)
Exceptions are enabled by specifying the 'xs' option when the compiler is
invoked.  The error message indicates that a feature such as
.kw try,
.kw catch,
.kw throw,
or function exception specification has been used without enabling
exceptions.

:MSGSYM. ERR_IO_ERR
:MSGTXT. I/O error reading '%s': %s"
:MSGJTXT. '%s'�̓ǂݍ��݂h�^�n�G���[�F%s"
When attempting to read data from a source or header file, the
indicated system error occurred. Likely there is a hardware problem,
or the file system has become corrupt.

:MSGSYM. ANSI_JUNK_FOLLOWS_DIRECTIVE
:MSGTXT. text following pre-processor directive
:MSGJTXT. �v���v���Z�b�T�[�����߂Ƀe�L�X�g�������Ă��܂�
:ANSI. 4
A
.kw #else
or
.kw #endif
directive was found which had tokens following it rather than an
end of line. Some UNIX style preprocessors allowed this, but it
is not legal under standard C or C++. Make the tokens into a comment.

:MSGSYM. WARN_EXPR_NOT_MEANINGFUL
:MSGTXT. expression is not meaningful
:MSGJTXT. ���͈Ӗ�������܂���
:WARNING. 1
This message indicates that the indicated expression is not meaningful.
An expression is meaningful when a function is invoked, when an
assignment or initialization is performed, or when the expression is casted
to void.
:errbad.
void foo( int i, int j )
{
    i + j;  // not meaningful
}
:eerrbad.

:MSGSYM. WARN_EXPR_NO_SIDE_EFFECT
:MSGTXT. expression has no side effect
:MSGJTXT. ���͕���p�������܂���
:WARNING. 1
The indicated expression does not cause a side effect.
A side effect is caused by invoking a function, by an assignment or an
initialization, or by reading a
.kw volatile
variable.
:errbad.
int k;
void foo( int i, int j )
{
    i + j,  // no side effect (note comma)
    k = 3;
}
:eerrbad.

:MSGSYM. INF_SRC_CNV_TYPE
:MSGTXT. source conversion type is '%T'
:MSGJTXT. �ϊ��\�[�X�̌^��"%T"�ł�
:INFO.
This informational message indicates the type of the source operand, for the
preceding conversion diagnostic.

:MSGSYM. INF_TGT_CNV_TYPE
:MSGTXT. target conversion type is '%T'
:MSGJTXT. �ϊ��^�[�Q�b�g�̌^��"%T"�ł�
:INFO.
This informational message indicates the target type of the conversion,
for the preceding conversion diagnostic.

:MSGSYM. ERR_CANNOT_REDECLARE_FUNCTION_PROPERTIES
:MSGTXT. redeclaration of '%S' has different attributes
:MSGJTXT. '%S'�̍Đ錾�͈قȂ鑮���������܂�
A function cannot be made
.kw virtual
or pure
.kw virtual
in a subsequent declaration.
All properties of a function should be described in the first declaration
of a function.  This is especially important for member functions because
the properties of a class are affected by its member functions.
:errbad.
struct S {
    void fun();
};

virtual void S::fun()
{
}
:eerrbad.

:MSGSYM. INF_TEMPLATE_CLASS_DEFN_TRACEBACK
:MSGTXT. template class instantiation for '%T' was %L
:MSGJTXT. '%T'�ɑ΂���e���v���[�g�E�N���X�E�C���X�^���X���́C%L�ł���
:INFO.
This informational message indicates that the error or warning was
detected during the instantiation of a class template.  The final type
of the template class is shown as well as the location in the source
where the instantiation was initiated.

:MSGSYM. INF_TEMPLATE_FN_DEFN_TRACEBACK
:MSGTXT. template function instantiation for '%S' was %L
:MSGJTXT. '%S'�ɑ΂���e���v���[�g�֐��C���X�^���X����%L�ł���
:INFO.
This informational message indicates that the error or warning was
detected during the instantiation of a function template.  The final
type of the template function is shown as well as the location in the
source where the instantiation was initiated.

:MSGSYM. INF_TEMPLATE_MEMBER_DEFN_TRACEBACK
:MSGTXT. template class member instantiation was %L
:MSGJTXT. �e���v���[�g�E�N���X�E�����o�E�C���X�^���X����%L�ł���
:INFO.
This informational message indicates that the error or warning was
detected during the instantiation of a member of a class template.  The
location in the source where the instantiation was initiated is shown.

:MSGSYM. INF_TEMPLATE_FN_BIND_TRACEBACK
:MSGTXT. function template binding for '%S' was %L
:MSGJTXT. '%S'�ɑ΂���֐��e���v���[�g�E�o�C���h��%L�ł���
:INFO.
This informational message indicates that the error or warning was
detected during the binding process of a function template.
The binding process occurs at the point where arguments are analysed
in order to infer what types should be used in a function template
instantiation.
The function template in question is shown along with the location
in the source code that initiated the binding process.

:MSGSYM. INF_TEMPLATE_FN_BIND_AND_GEN_TRACEBACK
:MSGTXT. function template binding of '%S' was %L
:MSGJTXT. '%S'�̊֐��e���v���[�g�E�o�C���h��%L�ł���
:INFO.
This informational message indicates that the error or warning was
detected during the binding process of a function template.
The binding process occurs at the point where a function prototype
is analysed in order to see if the prototype matches any function
template of the same name.
The function template in question is shown along with the location
in the source code that initiated the binding process.

:MSGSYM. INF_CLASS_DECLARATION
:MSGTXT. '%s' defined %L
:MSGJTXT. '%s'�͎��Œ�`����Ă��܂��F%L
:INFO.
This informational message indicates where the class in question was defined.
The message is displayed following an error or warning diagnostic for the
class in question.
:errbad.
class S;
int foo( S*p )
{
    return p->x;
}
:eerrbad.
The variable
.id p
is a pointer to an undefined class and so will
cause an error to be generated.
Following the error, the informational message indicates the line at
which the class S was declared.

:MSGSYM. ERR_PRAG_TEMPLATE_DEPTH
:MSGTXT. form is '#pragma template_depth level' where 'level' is a non-zero number
:MSGJTXT. ������'#pragma template_depth level'�ł��B������'level'��0�ȊO�̐��l�ł�
This
.kw pragma
sets the number of times templates will be instantiated for nested
instantiations.
The depth check prevents infinite compile times for incorrect programs.

:MSGSYM. ERR_TEMPLATE_DEPTH_EXHAUSTED
:MSGTXT. possible non-terminating template instantiation (use "#pragma template_depth %d" to increase depth)
:MSGJTXT. �e���v���[�g�E�C���X�^���X���������ł��܂���i�W�J�[�x�𑝂₷���߂�"#pragma template_depth %d"���g���܂��j
This message indicates that a large number of expansions were required
to complete a template class or template function instantiation.  This
may indicate that there is an erroneous use of a template.  If the program
will complete given more depth, try using the suggested #pragma in the error
message to increase the depth.  The number provided is double the previous
value.

:MSGSYM. ERR_CANNOT_INHERIT_PARTIALLY_DEFINED
:MSGTXT. cannot inherit a partially defined base class '%T'
:MSGJTXT. �����I�ɒ�`���ꂽ���N���X'%T'���p���ł��܂���
This message indicates that the base class was in the midst of being
defined when it was inherited.
The storage requirements for a
.kw class
type must be known when inheritance is involved because
the layout of the final class depends on knowing the complete
contents of all base classes.
:errbad.
struct Partial {
    struct Nested : Partial {
    int n;
    };
};
:eerrbad.

:MSGSYM. INF_FUNC_AMBIGUOUS
:MSGTXT. ambiguous function: %F defined %L
:MSGJTXT. �B���Ȋ֐��F%F�́C���Œ�`����Ă��܂��F%L
:INFO.
This informational message shows the functions that were detected to be
ambiguous.
:errbad.
int amb( char );        // will be ambiguous
int amb( unsigned char );   // will be ambiguous
int amb( char, char );
int k = amb( 14 );
:eerrbad.
The constant value 14 has an
.kw int
type and so the attempt to invoke the function
.id amb
is ambiguous.
The first two functions are ambiguous (and will be  displayed); the third
is not considered (nor displayed) since it is declared to have
a different number of arguments.

:MSGSYM. INF_FUNC_PARM_MISMATCH
:MSGTXT. cannot convert argument %d defined %L
:MSGJTXT. ����%d(%L�Œ�`����Ă��܂�)��ϊ����邱�Ƃ��ł��܂���
:INFO.
This informational message indicates the first argument which could not be
converted to the corresponding type for the declared function.
It is displayed when there is exactly one function declared with the indicated
name.

:MSGSYM. INF_THIS_MISMATCH
:MSGTXT. 'this' cannot be converted
:MSGJTXT. 'this'�͕ϊ����邱�Ƃ��ł��܂���
:INFO.
This informational message indicates the
.kw this
pointer for the function which could not be
converted to the type of the
.kw this
pointer for the declared function.
It is displayed when there is exactly one function declared with the indicated
name.

:MSGSYM. INF_FUNC_REJECTED
:MSGTXT. rejected function: %F defined %L
:MSGJTXT. ���O���ꂽ�֐��F%F�͎��Œ�`����Ă��܂��F%L
:INFO.
This informational message shows the overloaded functions which were
rejected from consideration during function-overload resolution.
These functions are displayed when there is more than one function
with the indicated name.

:MSGSYM. INF_CONV_AMBIG_SCALAR
:MSGTXT. '%T' operator can be used
:MSGJTXT. '%T'���Z�q���g���܂�
:INFO.
Following a diagnosis of operator ambiguity, this information message indicates
that the operator can be applied with operands of the type indicated in the
message.
:errbad.
struct S {
    S( int );
    operator int();
    S operator+( int );
};
S s(15);
int k = s + 123;    // "+" is ambiguous
:eerrbad.
In the example, the "+" operation is ambiguous because it can implemented as
by the addition of two integers (with
.id S::operator int
applied to the second operand) or by a call to
.id S::operator+.
This informational message indicates that the first is possible.

:MSGSYM. ERR_UNDEF_IMPOSSIBLE
:MSGTXT. cannot #undef '%s'
:MSGJTXT. '%s'��#undef�ł��܂���
The predefined macros
.id __cplusplus, __DATE__, __FILE__, __LINE__, __STDC__, __TIME__, __FUNCTION__
and
.id __func__
cannot be undefined using the
.kw #undef
directive.
:errbad.
#undef __cplusplus
#undef __DATE__
#undef __FILE__
#undef __LINE__
#undef __STDC__
#undef __TIME__
#undef __FUNCTION__
#undef __func__
:eerrbad.
All of the preceding directives are not permitted.

:MSGSYM. ERR_DEFINE_IMPOSSIBLE
:MSGTXT. cannot #define '%s'
:MSGJTXT. '%s'��#define�ł��܂���
The predefined macros
.id __cplusplus, __DATE__, __FILE__, __LINE__, __STDC__,
and
.id  __TIME__
cannot be defined using the
.kw #define
directive.
:errbad.
#define __cplusplus     1
#define __DATE__        2
#define __FILE__        3
#define __LINE__        4
#define __STDC__        5
#define __TIME__        6
:eerrbad.
All of the preceding directives are not permitted.

:MSGSYM. INF_TEMPLATE_FN_DECL
:MSGTXT. template function '%F' defined %L
:MSGJTXT. �e���v���[�g�֐�'%F'�͎��Œ�`����Ă��܂��F%L
:INFO.
This informational message indicates where the function template in question
was defined.
The message is displayed following an error or warning diagnostic for the
function template in question.
:errbad.
template <class T>
    void foo( T, T * )
    {
    }

void bar()
{
    foo(1);     // could not instantiate
}
:eerrbad.
The function template for
.id foo
cannot be instantiated for a single argument causing an error to be generated.
Following the error, the informational message indicates the line at
which
.id foo
was declared.

:MSGSYM. INF_TEMPLATE_FN_AMBIGUOUS
:MSGTXT. ambiguous function template: %F defined %L
:MSGJTXT. �B���Ȋ֐��e���v���[�g�F%F�͎��Œ�`����Ă��܂��F%L
:INFO.
This informational message shows the function templates that were detected
to be ambiguous for the arguments at the call point.

:MSGSYM. ERR_TEMPLATE_FN_MISMATCH
:MSGTXT. cannot instantiate %S
:MSGJTXT. %S���C���X�^���X�����邱�Ƃ��ł��܂���
This message indicates that the function template could not be instantiated
for the arguments supplied.
It is displayed when there is exactly one function template declared with
the indicated name.

:MSGSYM. INF_TEMPLATE_FN_REJECTED
:MSGTXT. rejected function template: %F defined %L
:MSGJTXT. ���O���ꂽ�֐��e���v���[�g�F%F�͎��Œ�`����Ă��܂��F%L
:INFO.
This informational message shows the overloaded function template which was
rejected from consideration during function-overload resolution.
These functions are displayed when there is more than one function or
function template with the indicated name.

:MSGSYM. ERR_CANT_BE_FUNC
:MSGTXT. operand cannot be a function
:MSGJTXT. �I�y�����h�͊֐��ł͂��肦�܂���
The indicated operation cannot be applied to a function.
:errbad.
int Fun();
int j = ++Fun;  // illegal
:eerrbad.
In the example, the attempt to increment a function is illegal.

:MSGSYM. ERR_CANT_BE_FUNC_LEFT
:MSGTXT. left operand cannot be a function
:MSGJTXT. ���I�y�����h�͊֐��ł͂��肦�܂���
The indicated operation cannot be applied to the left operand which
is a function.
:errbad.
extern int Fun();
void foo()
{
    Fun = 0;    // illegal
}
:eerrbad.
In the example, the attempt to assign zero to a function is illegal.

:MSGSYM. ERR_CANT_BE_FUNC_RIGHT
:MSGTXT. right operand cannot be a function
:MSGJTXT. �E�I�y�����h�͊֐��ł͂��肦�܂���
The indicated operation cannot be applied to the right operand which
is a function.
:errbad.
extern int Fun();
void foo()
{
    void* p = 3[Fun];   // illegal
}
:eerrbad.
In the example, the attempt to subscript a function is illegal.

:MSGSYM. WARN_OPTIMIZE_IF_INLINE
:MSGTXT. define this function inside its class definition (may improve code quality)
:MSGJTXT. ���̃N���X��`�̓����ł��̊֐����`���܂��i�R�[�h�i�������P���邩������܂���j
:WARNING. 5
The Open Watcom C++ compiler has found a constructor or destructor with
an empty function body.  An empty function body can usually provide
optimization opportunities so the compiler is indicating that by
defining the function inside its class definition,
the compiler may be able to perform some important optimizations.
:errgood.
struct S {
    ~S();
};

S::~S() {
}
:eerrgood.

:MSGSYM. WARN_OPTIMIZE_IF_EARLIER
:MSGTXT. define this function inside its class definition (could have improved code quality)
:MSGJTXT. ���̃N���X��`�̓����ł��̊֐����`���܂��i�R�[�h�i�������P�ł�����������܂���j
:WARNING. 5
The Open Watcom C++ compiler has found a constructor or destructor with
an empty function body.  An empty function body can usually provide
optimization opportunities so the compiler is indicating that by
defining the function inside its class definition,
the compiler may be able to perform some important optimizations.
This particular warning indicates that the compiler has already found an
opportunity in previous code but it found out too late that the
constructor or destructor had an empty function body.
:errgood.
struct S {
    ~S();
};
struct T : S {
    ~T() {}
};

S::~S() {
}
:eerrgood.

:MSGSYM. INF_BAD_FN_OVERLOAD
:MSGTXT. cannot convert address of overloaded function '%S'
:MSGJTXT. �I�[�o�[���[�h�֐�'%S'�̃A�h���X��ϊ����邱�Ƃ��ł��܂���
:INFO.
This information message indicates that
an address of an overloaded function cannot be converted to the
indicated type.
:errbad.
int ovload( char );
int ovload( float );
int routine( int (*)( int );
int k = routine( ovload );
:eerrbad.
The first argument for the function
.id routine
cannot be converted, resulting in the informational message.

:MSGSYM. ERR_EXPR_IS_VOID
:MSGTXT. expression cannot have void type
:MSGJTXT. ����'void'�^�������Ƃ��ł��܂���
The indicated expression cannot have a
.kw void
type.
:errbad.
main( int argc, char* argv )
{
    if( (void)argc ) {
    return 5;
    } else {
    return 9;
    }
}
:eerrbad.
Conditional expressions, such as the one illustrated in the
.kw if
statement cannot have a
.kw void
type.

:MSGSYM. ERR_CANT_REFERENCE_A_BIT_FIELD
:MSGTXT. cannot reference a bit field
:MSGJTXT. �r�b�g�t�B�[���h�Q�Ƃł��܂���
The smallest addressable unit is a byte.
You cannot reference a bit field.
:errbad.
struct S
{   int bits :6;
    int bitfield :10;
};
S var;
int& ref = var.bitfield;    // illegal
:eerrbad.

:MSGSYM. ERR_ASSIGN_TO_UNDEF_CLASS
:MSGTXT. cannot assign to object having an undefined class
:MSGJTXT. ����`�̃N���X�����I�u�W�F�N�g�������邱�Ƃ͂ł��܂���
An assignment cannot be be made to an object whose class has not been
defined.
:errbad.
class X;        // declared, but not defined
extern X& foo();    // returns reference (ok)
extern X obj;
void goop()
{
    obj = foo();    // error
}
:eerrbad.

:MSGSYM. ERR_ADDR_OF_CTOR
:MSGTXT. cannot create member pointer to constructor
:MSGJTXT. �R���X�g���N�^�ւ̃����o�E�|�C���^�����邱�Ƃ��ł��܂���
A member pointer value cannot reference a constructor.
:errbad.
class C {
    C();
};
int foo()
{
    return 0 == &C::C;
}
:eerrbad.

:MSGSYM. ERR_ADDR_OF_DTOR
:MSGTXT. cannot create member pointer to destructor
:MSGJTXT. �f�X�g���N�^�ւ̃����o�E�|�C���^�����邱�Ƃ��ł��܂���
A member pointer value cannot reference a destructor.
:errbad.
class C {
    ~C();
};
int foo()
{
    return 0 == &C::~C;
}
:eerrbad.

:MSGSYM. ERR_TEMP_AS_NONCONST_REF
:MSGTXT. attempt to initialize a non-constant reference with a temporary object
:MSGJTXT. �ꎞ�I�u�W�F�N�g�Ŕ�萔�̎Q�Ƃ�������Ԃɂ��悤�Ƃ��܂�
A temporary value cannot be converted to a non-constant reference type.
:errbad.
struct C {
    C( C& );
    C( int );
};

C & c1 = 1;
C c2 = 2;
:eerrbad.
The initializations of
.id c1
and
.id c2
are erroneous, since temporaries are being bound to non-const references.
In the case of
.id c1,
an implicit constructor call is required to convert the integer to
the correct object type.  This results in a temporary object being created
to initialize the reference.  Subsequent code can modify this temporary's
state.
The initialization of
.id c2,
is erroneous for a similar reason.  In this case, the temporary is being
bound to the non-const reference argument of the copy constructor.

:MSGSYM. ANSI_TEMP_USED_TO_INIT_NONCONST_REF
:MSGTXT. temporary object used to initialize a non-constant reference
:MSGJTXT. �ꎞ�I�u�W�F�N�g�́C��萔�̎Q�Ƃ��������邽�߂Ɏg���܂���
:ANSI. 1
Ordinarily, a temporary value cannot be bound to a non-constant
reference.  There is enough legacy code present that the Open Watcom C++
compiler issues a warning in cases that should be errors.  This
may change in the future so it is advisable to correct the code
as soon as possible.

:MSGSYM. WARN_ASSUMING_NO_OVERLOADED_OP_ADDR
:MSGTXT. assuming unary 'operator &' not overloaded for type '%T'
:MSGJTXT. �Öق̒P����'operator &'��'%T'�^�ŃI�[�o�[���[�h����܂���
:WARNING. 3
An explicit address operator can be applied to a reference
to an undefined class.  The Open Watcom C++ compiler will assume
that the address is required but it does not know whether
this was the programmer's intention because the class definition
has not been seen.
:errbad.
struct S;

S * fn( S &y ) {
    // assuming no operator '&' defined
    return &y;
}
:eerrbad.

:MSGSYM. WARN_NO_ARG_BEFORE_ELLIPSE
:MSGTXT. 'va_start' macro will not work without an argument before '...'
:MSGJTXT. 'va_start'�}�N���́C"..."�̑O�Ɉ������Ȃ��Ɠ����܂���
:WARNING. 3
The warning indicates that it is impossible to access the arguments
passed to the function without declaring an argument before the "..."
argument.  The "..." style of argument list (without any other arguments)
is only useful as a prototype or if the function is designed to
ignore all of its arguments.
:errbad.
void fn( ... )
{
}
:eerrbad.

:MSGSYM. WARN_REF_ARG_BEFORE_ELLIPSE
:MSGTXT. 'va_start' macro will not work with a reference argument before '...'
:MSGJTXT. 'va_start'�}�N���́C"..."�̑O�ɎQ�ƈ���������Ɠ����܂���
:WARNING. 1
The warning indicates that taking the address of the argument before the
"..." argument, which 'va_start' does in order to access the
variable list of arguments, will not give the expected result.
The arguments will have to be rearranged so that an acceptable
argument is declared before the "..." argument or a dummy
.kw int
argument can be inserted after the reference argument with
the corresponding adjustments made to the callers of the function.
:errbad.
#include <stdarg.h>

void fn( int &r, ... )
{
    va_list args;

    // address of 'r' is address of
    // object 'r' references so
    // 'va_start' will not work properly
    va_start( args, r );
    va_end( args );
}
:eerrbad.

:MSGSYM. WARN_CLASS_ARG_BEFORE_ELLIPSE
:MSGTXT. 'va_start' macro will not work with a class argument before '...'
:MSGJTXT. 'va_start'�}�N���́C"..."�̑O�ɃN���X����������Ɠ����܂���
:WARNING. 1
This warning is specific to C++ compilers that quietly convert
class arguments to class reference arguments.  The warning
indicates that taking the address of the argument before the
"..." argument, which 'va_start' does in order to access the
variable list of arguments, will not give the expected result.
The arguments will have to be rearranged so that an acceptable
argument is declared before the "..." argument or a dummy
.kw int
argument can be inserted after the class argument with
the corresponding adjustments made to the callers of the function.
:errbad.
#include <stdarg.h>

struct S {
    S();
};

void fn( S c, ... )
{
    va_list args;

    // Open Watcom C++ passes a pointer to
    // the temporary created for passing
    // 'c' rather than pushing 'c' on the
    // stack so 'va_start' will not work
    // properly
    va_start( args, c );
    va_end( args );
}
:eerrbad.

:MSGSYM. ERR_CONFLICTING_PRAGMA_MODIFIERS
:MSGTXT. function modifier conflicts with previous declaration '%S'
:MSGJTXT. �֐��C���q�͑O�̐錾'%S'�Ɩ������܂�
The symbol declaration conflicts with a previous declaration with regard
to function modifiers.  Either the previous declaration did not have a
function modifier or it had a different one.
:errbad.
#pragma aux never_returns aborts;

void fn( int, int );
void __pragma("never_returns") fn( int, int );
:eerrbad.

:MSGSYM. ERR_FUNCTION_MOD_ON_VAR
:MSGTXT. function modifier cannot be used on a variable
:MSGJTXT. �֐��C���q�͕ϐ��ɑ΂��Ďg�p�ł��܂���
The symbol declaration has a function modifier being applied to
a variable or non-function.  The cause of this may be a
declaration with a missing function argument list.
:errbad.
int (* __pascal ok)();
int (* __pascal not_ok);
:eerrbad.

:MSGSYM. INF_CLASS_CONTAINS_PURE
:MSGTXT. '%T' contains the following pure virtual functions
:MSGJTXT. '%T'�͈ȉ��̏����ȉ��z�֐����܂݂܂�
:INFO.
This informational message indicates that the class contains pure
virtual function declarations.  The class is definitely abstract
as a result and cannot be used to declare variables.
The pure virtual functions declared in the class are displayed
immediately following this message.
:errbad.
struct A {
    void virtual fn( int ) = 0;
};

A x;
:eerrbad.

:MSGSYM. INF_CLASS_DIDNT_DEFINE_PURE
:MSGTXT. '%T' has no implementation for the following pure virtual functions
:MSGJTXT. '%T'�́C�ȉ��̏����ȉ��z�֐��ɑΉ���������������܂���
:INFO.
This informational message indicates that the class is derived
from an abstract class but the class did not override enough
virtual function declarations.
The pure virtual functions declared in the class are displayed
immediately following this message.
:errbad.
struct A {
    void virtual fn( int ) = 0;
};
struct D : A {
};

D x;
:eerrbad.

:MSGSYM. INF_PURE_FUNCTION
:MSGTXT. pure virtual function '%F' defined %L
:MSGJTXT. �������z�֐�'%F'�͒�`����Ă��܂�: %L
:INFO.
This informational message indicates that the pure virtual function
has not been overridden.  This means that the class is abstract.
:errbad.
struct A {
    void virtual fn( int ) = 0;
};
struct D : A {
};

D x;
:eerrbad.


:MSGSYM. ERR_BAD_CALL_CONVENTION
:MSGTXT. restriction: standard calling convention required for '%S'
:MSGJTXT. �����F�W���̌ďo���K���'%S'�ɑ΂��ĕK�v�ł�
The indicated function may be called by the C++ run-time system using
the standard calling convention.
The calling convention specified for the function is incompatible with
the standard convention.
This message may result when
.id __pascal
is specified for a default constructor, a copy constructor, or a
destructor.
It may also result when
.id parm reverse
is specified in a
.kw #pragma
for the function.

:MSGSYM. ERR_PARM_COUNT_MISMATCH_POINTER
:MSGTXT. number of arguments in function call is incorrect
:MSGJTXT. �֐��ďo���̒��̈����̐��͕s���m�ł�
The number of arguments in the function call does not match the number
declared for the function type.
:errbad.
extern int (*pfn)( int, int );
int k = pfn( 1, 2, 3 );
:eerrbad.
In the example, the function pointer was declared to have two arguments.
Three arguments were used in the call.

:MSGSYM. INF_FUNCTION_TYPE
:MSGTXT. function has type '%T'
:MSGJTXT. �֐���'%T'�^�������܂�
:INFO.
This informational message indicates the type of the function being called.

:MSGSYM. ERR_INVALID_OCTAL_CONSTANT
:MSGTXT. invalid octal constant
:MSGJTXT. �s�K�؂ȂW�i�萔�ł�
The constant started with a '0' digit which makes it look like an octal
constant but the constant contained the digits '8' and '9'.  The problem
could be an incorrect octal constant or a missing '.' for a floating
constant.
:errbad.
int i = 0123456789; // invalid octal constant
double d = 0123456789;  // missing '.'?
:eerrbad.

:MSGSYM. INF_CLASS_TEMPLATE_STARTED_HERE
:MSGTXT. class template definition started %L
:MSGJTXT. �N���X�E�e���v���[�g��`��%L�Ŏn�܂�܂���
:INFO.
This informational message indicates where the class template
definition started so that any problems with missing braces can
be fixed quickly and easily.
:errbad.
template <class T>
    struct S {
    void f1() {
    // error missing '}'
    };

template <class T>
    struct X {
    void f2() {
    }
    };
:eerrbad.

:MSGSYM. INF_CTOR_INIT_STARTED_HERE
:MSGTXT. constructor initializer started %L
:MSGJTXT. �R���X�g���N�^�E�C�j�V�����C�U��%L�Ŏn�܂�܂���
:INFO.
This informational message indicates where the constructor initializer
started so that any problems with missing parenthesis can
be fixed quickly and easily.
:errbad.
struct S {
    S( int x ) : a(x), b(x // missing parenthesis
    {
    }
};
:eerrbad.

:MSGSYM. ERR_ZERO_ARRAY_MUST_BE_LAST
:MSGTXT. zero size array must be the last data member
:MSGJTXT. �傫�������w��̔z��́C�Ō�̃f�[�^�E�����o�łȂ���΂Ȃ�܂���
The language extension that allows a zero size array to be declared
in a class definition requires that the array be the last data
member in the class.
:errbad.
struct S {
    char a[];
    int b;
};
:eerrbad.

:MSGSYM. ERR_CANNOT_INHERIT_CLASS_WITH_ZERO_ARRAY
:MSGTXT. cannot inherit a class that contains a zero size array
:MSGJTXT. �傫�������w��̔z����܂ރN���X���p�����邱�Ƃ͂ł��܂���
The language extension that allows a zero size array to be declared
in a class definition disallows the use of the class as a base
class.
This prevents the programmer from corrupting storage in derived classes
through the use of the zero size array.
:errbad.
struct B {
    int b;
    char a[];
};
struct D : B {
    int d;
};
:eerrbad.

:MSGSYM. ERR_CANNOT_HAVE_ZERO_ARRAY_AND_BASES
:MSGTXT. zero size array '%S' cannot be used in a class with base classes
:MSGJTXT. �傫�������w��̔z��'%S'�́C���N���X���������N���X�ɂ����Ďg�����Ƃ͂ł��܂���
The language extension that allows a zero size array to be declared
in a class definition requires that the class not have any base classes.
This is required because the C++ compiler must be free to organize base
classes in any manner for optimization purposes.
:errbad.
struct B {
    int b;
};
struct D : B {
    int d;
    char a[];
};
:eerrbad.

:MSGSYM. ERR_CATCH_ABSTRACT
:MSGTXT. cannot catch abstract class object
:MSGJTXT. ���ۃN���X�̃I�u�W�F�N�g��'catch'�ŕ߂��邱�Ƃ͂ł��܂���
C++ does not allow abstract classes to be instantiated and so an abstract
class object cannot be specified in a
.kw catch
clause.  It is permissible to catch a reference to an abstract class.
:errbad.
class Abstract {
public:
    virtual int foo() = 0;
};

class Derived : Abstract {
public:
    int foo();
};

int xyz;

void func( void ) {
    try {
    throw Derived();
    } catch( Abstract abstract ) {   // object
    xyz = 1;
    }
}
:eerrbad.
The catch clause in the preceding example would be diagnosed as improper,
since an abstract class is specified.  The example could be coded as follows.
:errgood.
class Abstract {
public:
    virtual int foo() = 0;
};

class Derived : Abstract {
public:
    int foo();
};

int xyz;

void func( void ) {
    try {
    throw Derived();
    } catch( Abstract & abstract ) {  // reference
    xyz = 1;
    }
}
:eerrgood.

:MSGSYM. ERR_CANT_BE_MEMB_FUN
:MSGTXT. non-static member function '%S' cannot be specified
:MSGJTXT. ��X�^�e�B�b�N�E�����o�֐�'%S'�͎w��ł��܂���
The indicated non-static member function cannot be used in this context.
For example, such a function cannot be used as the second or third operand
of the conditional operator.
:errbad.
struct S {
    int foo();
    int bar();
    int fun();
};

int S::fun( int i ) {
    return (i ? foo : bar)();
}
:eerrbad.
Neither
.id foo
nor
.id bar
can be specified as shown in the example.
The example can be properly coded as follows:
:errgood.
struct S {
    int foo();
    int bar();
    int fun();
};

int S::fun( int i ) {
    return i ? foo() : bar();
}
:eerrgood.

:MSGSYM. ERR_CONV_BASE_TO_DERIVED
:MSGTXT. attempt to convert pointer or reference from a base to a derived class
:MSGJTXT. ���N���X����h���N���X�փ|�C���^���Q�Ƃ�ϊ����悤�Ƃ��܂�
A pointer or reference to a base class cannot be converted to a pointer
or reference, respectively, of a derived class, unless there is an explicit
cast.  The
.id return
statements in the following example will be diagnosed.
:errbad.
struct Base {};
struct Derived : Base {};

Base b;

Derived* ReturnPtr() { return &b; }
Derived& ReturnRef() { return b; }
:eerrbad.
The following program would be acceptable:
:errgood.
struct Base {};
struct Derived : Base {};

Base b;

Derived* ReturnPtr() { return (Derived*)&b; }
Derived& ReturnRef() { return (Derived&)b; }
:eerrgood.

:MSGSYM. WARN_WHILE_TRUE
:MSGTXT. expression for 'while' is always true
:MSGJTXT. 'while'�ɑ΂��鎮�͏�ɐ^�ł�
:WARNING. 3
The compiler has detected that the expression will always be true.
Consequently, the loop will execute infinitely unless there is a
.kw break
statement within the loop
or a
.kw throw
statement is executed while executing within the loop.
If such an infinite loop is required, it can be coded as
.id for( ; ; )
without causing warnings.

:MSGSYM. WARN_FOR_TRUE
:MSGTXT. testing expression for 'for' is always true
:MSGJTXT. 'for'�̏������͏�ɐ^�ł�
:WARNING. 3
The compiler has detected that the expression will always be true.
Consequently, the loop will execute infinitely unless there is a
.kw break
statement within the loop
or a
.kw throw
statement is executed while executing within the loop.
If such an infinite loop is required, it can be coded as
.id for( ; ; )
without causing warnings.


:MSGSYM. WARN_ALWAYS_TRUE
:MSGTXT. conditional expression is always true (non-zero)
:MSGJTXT. �������͏�ɐ^�i��[���j�ł�
:WARNING. 4
The indicated expression is a non-zero constant and so will always be true.


:MSGSYM. WARN_ALWAYS_FALSE
:MSGTXT. conditional expression is always false (zero)
:MSGJTXT. �������͏�ɋU�i�[���j�ł�
:WARNING. 4
The indicated expression is a zero constant and so will always be false.

:MSGSYM. ERR_INVALID_TEMPLATE_MEMBER
:MSGTXT. expecting a member of '%T' to be defined in this context
:MSGJTXT. '%T'�̃����o�́C�����Œ�`�����͂��ł�
A class template member definition must define a member of the
associated class template.  The complexity of the C++ declaration
syntax can make this error hard to identify visually.
:errbad.
template <class T>
    struct S {
    typedef int X;
    static X fn( int );
    static X qq;
    };

template <class T>
    S<T>::X fn( int ) {// should be 'S<T>::fn'

    return fn( 2 );
    }

template <class T>
    S<T>::X qq = 1; // should be 'S<T>::q'

S<int> x;
:eerrbad.

:MSGSYM. ERR_THROW_ABSTRACT
:MSGTXT. cannot throw an abstract class
:MSGJTXT. ���ۃN���X��throw���邱�Ƃ͂ł��܂���
An abstract class cannot be thrown since copies of that object may have to be
made (which is impossible );
:errbad.
struct abstract_class {
    abstract_class( int );
    virtual int foo() = 0;
};

void goop()
{
    throw abstract_class( 17 );
}
:eerrbad.
The
.kw throw
expression is illegal since it specifies an abstract class.

:MSGSYM. ERR_PCH_CREATE_ERROR
:MSGTXT. cannot create pre-compiled header file '%s'
:MSGJTXT. �v���R���p�C���E�w�b�_�[�t�@�C��'%s'�����邱�Ƃ��ł��܂���
The compiler has detected a problem while trying to open the
pre-compiled header file for write access.

:MSGSYM. ERR_PCH_WRITE_ERROR
:MSGTXT. error occurred while writing pre-compiled header file
:MSGJTXT. �v���R���p�C����w�b�_�[�������Ă���Œ��ɃG���[���������܂���
The compiler has detected a problem while trying to write some data
to the pre-compiled header file.

:MSGSYM. ERR_PCH_READ_ERROR
:MSGTXT. error occurred while reading pre-compiled header file
:MSGJTXT. �v���R���p�C����w�b�_�[��ǂ�ł���Œ��ɃG���[���������܂���
The compiler has detected a problem while trying to read some data
from the pre-compiled header file.

:MSGSYM. WARN_PCH_CONTENTS_HEADER_ERROR
:MSGTXT. pre-compiled header file being recreated
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C�����č쐬����Ă��܂�
:WARNING. 1
The existing pre-compiled header file may either be corrupted or is a version
that the compiler cannot use due to updates to the compiler.
A new version of the pre-compiled header file will be created.

:MSGSYM. WARN_PCH_CONTENTS_OPTIONS
:MSGTXT. pre-compiled header file being recreated (different compile options)
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C�����č쐬����Ă��܂�(�R���p�C����I�v�V�������قȂ�܂�)
:WARNING. 1
The compiler has detected that the command line options have changed
enough so the contents of the pre-compiled header file cannot be used.
A new version of the pre-compiled header file will be created.

:MSGSYM. WARN_PCH_CONTENTS_INCFILE
:MSGTXT. pre-compiled header file being recreated (different #include file)
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C�����č쐬����Ă��܂�(#include�t�@�C�����قȂ�܂�)
:WARNING. 1
The compiler has detected that the first
.kw #include
file name is different so
the contents of the pre-compiled header file cannot be used.
A new version of the pre-compiled header file will be created.

:MSGSYM. WARN_PCH_CONTENTS_CWD
:MSGTXT. pre-compiled header file being recreated (different current directory)
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C�����č쐬����Ă��܂�(�J�����g��f�B���N�g�����قȂ�܂�)
:WARNING. 1
The compiler has detected that the working directory is different so
the contents of the pre-compiled header file cannot be used.
A new version of the pre-compiled header file will be created.

:MSGSYM. WARN_PCH_CONTENTS_INCLUDE
:MSGTXT. pre-compiled header file being recreated (different INCLUDE path)
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C�����č쐬����Ă��܂�(INCLUDE�p�X���قȂ�܂�)
:WARNING. 1
The compiler has detected that the INCLUDE path is different so
the contents of the pre-compiled header file cannot be used.
A new version of the pre-compiled header file will be created.

:MSGSYM. WARN_PCH_CONTENTS_HFILE
:MSGTXT. pre-compiled header file being recreated ('%s' has been modified)
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C�����č쐬����Ă��܂�('%s'���C������܂���)
:WARNING. 1
The compiler has detected that an include file has changed so
the contents of the pre-compiled header file cannot be used.
A new version of the pre-compiled header file will be created.

:MSGSYM. WARN_PCH_CONTENTS_MACRO_DIFFERENT
:MSGTXT. pre-compiled header file being recreated (macro '%s' is different)
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C�����č쐬����Ă��܂�(�}�N��'%s'���قȂ�܂�)
:WARNING. 1
The compiler has detected that a macro definition is different so
the contents of the pre-compiled header file cannot be used.
The macro was referenced during processing of the header file
that created the pre-compiled header file so the contents of the
pre-compiled header may be affected.
A new version of the pre-compiled header file will be created.

:MSGSYM. WARN_PCH_CONTENTS_MACRO_NOT_PRESENT
:MSGTXT. pre-compiled header file being recreated (macro '%s' is not defined)
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C�����č쐬����Ă��܂�(�}�N��'%s'����`����Ă��܂���)
:WARNING. 1
The compiler has detected that a macro has not been defined so
the contents of the pre-compiled header file cannot be used.
The macro was referenced during processing of the header file
that created the pre-compiled header file so the contents of the
pre-compiled header may be affected.
A new version of the pre-compiled header file will be created.

:MSGSYM. ERR_ZWS_MUST_HAVE_SS_DS_SAME
:MSGTXT. command line specifies smart windows callbacks and DS not equal to SS
:MSGJTXT. �R�}���h�E���C���ŁC�X�}�[�g�E�E�B���h�E�E�R�[���o�b�N�ƁCSS�Ɠ����Ȃ�DS�̐ݒ肪�w�肳��Ă��܂�
An illegal combination of switches has been detected.  The windows smart
callbacks option cannot be combined with either of the build DLL or DS not
equal to SS options.

:MSGSYM. ERR_DUMP_OBJ_MODEL
:MSGTXT. class '%N' cannot be used with #pragma dump_object_model
:MSGJTXT. �N���X'%N'�́C#pragma dump_object_model�ƂƂ��ɂ͎g���܂���
The indicated name has not yet been declared or has been declared but not
yet been defined as a class.
Consequently, the object model cannot be dumped.

:MSGSYM. INF_REPEATED_ITEM
:MSGTXT. repeated modifier is '%s'
:MSGJTXT. �J��Ԃ��ꂽ�C���q��'%s'�ł�
:INFO.
This informational message indicates what modifier was repeated
in the declaration.
:errbad.
typedef int __far FARINT;
FARINT __far *p;    // repeated __far modifier
:eerrbad.

:MSGSYM. INF_MISSING_SEMICOLON_AFTER_CLASS_ENUM_DEFN
:MSGTXT. semicolon (';') may be missing after class/enum definition
:MSGJTXT. class/enum��`�̌�C�Z�~�R�����i';'�j������܂���
:INFO.
This informational message indicates that a missing semicolon (';')
may be the cause of the error.
:errbad.
struct S {
    int x,y;
    S( int, int );
} // missing semicolon ';'

S::S( int x, int y ) : x(x), y(y) {
}
:eerrbad.

:MSGSYM. ERR_RETURN_UNDEFD_TYPE
:MSGTXT. cannot return a type of unknown size
:MSGJTXT. ���m�̑傫���̌^��߂����Ƃ��ł��܂���
A value of an unknown type cannot be returned.
:errbad.
class S;
S foo();

int goo()
{
    foo();
}
:eerrbad.
In the example, foo cannot be invoked because the class which it
returns has not been defined.

:MSGSYM. ERR_MEM_INIT_MEMBER
:MSGTXT. cannot initialize array member '%S'
:MSGJTXT. �z�񃁃��o'%S'�����������邱�Ƃ��ł��܂���
An array class member cannot be specified as a constructor initializer.
:errbad.
class S {
public:
    int arr[3];
    S();
};
S::S() : arr( 1, 2, 3 ) {}
:eerrbad.
In the example,
.id arr
cannot be specified as a constructor initializer.
Instead, the array may be initialized within the body of the constructor.
:errgood.
class S {
public:
    int arr[3];
    S();
};
S::S()
{
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
}
:eerrgood.

:MSGSYM. ERR_RECURSIVE_FILE_INCLUDE
:MSGTXT. file '%s' will #include itself forever
:MSGJTXT. �t�@�C��'%s'�͉i���ɂ��ꎩ�g��#include���܂�
The compiler has detected that the file in the message has been
.kw #include
from within itself without protecting against infinite inclusion.
This can happen if
.kw #ifndef
and
.kw #define
header file protection has not been used properly.
:errbad.
#include __FILE__
:eerrbad.

:MSGSYM. ERR_INVALID_USE_OF_MUTABLE
:MSGTXT. 'mutable' may only be used for non-static class members
:MSGJTXT. 'mutable'�͔�X�^�e�B�b�N�ȃN���X������o�[�ɑ΂��Ă̂ݎg�p�ł��܂�
A declaration in file scope or block scope cannot have a storage class of
.kw mutable.
:errbad.
mutable int a;
:eerrbad.

:MSGSYM. ERR_MUTABLE_CANT_BE_CONST
:MSGTXT. 'mutable' member cannot also be const
:MSGJTXT. 'mutable'�����o�[��'const'�ɂ͂Ȃ蓾�܂���
A
.kw mutable
member can be modified even if its class object is
.kw const.
Due to the semantics of
.kw mutable,
the programmer must decide whether a member
will be
.kw const
or
.kw mutable
because it cannot be both at the same time.
:errbad.
struct S {
    mutable const int * p;  // OK
    mutable int * const q;  // error
};
:eerrbad.

:MSGSYM. ERR_BAD_BOOL_ASSIGNMENT
:MSGTXT. left operand cannot be of type bool
:MSGJTXT. ���I�y�����h��'bool'�^�ɂ͂Ȃ�܂���
The left hand side of an assignment operator cannot be of type
.kw bool
except for simple assignment.
This is a restriction required in the C++ language.
:errbad.
bool q;

void fn()
{
    q += 1;
}
:eerrbad.

:MSGSYM. ERR_CANT_DEC_BOOL
:MSGTXT. operand cannot be of type bool
:MSGJTXT. �I�y�����h��'bool'�^�ɂ͂Ȃ�܂���
The operand of both postfix and prefix "--" operators
cannot be of type
.kw bool.
This is a restriction required in the C++ language.
:errbad.
bool q;

void fn()
{
    --q;   // error
    q--;   // error
}
:eerrbad.

:MSGSYM. ERR_UNDECLARED_MEMBER
:MSGTXT. member '%N' has not been declared in '%T'
:MSGJTXT. �����o�['%N'��'%T'�̒��Ő錾����Ă��܂���
The compiler has found a member which has not been previously declared.
The symbol may be spelled differently than the declaration, or the declaration
may simply not be present.
:errbad.
struct X { int m; };

void fn( X *p )
{
    p->x = 1;
}
:eerrbad.

:MSGSYM. WARN_TRUNC_INT_VALUE_PROMOTED
:MSGTXT. integral value may be truncated
:MSGJTXT. �����l�͐؂�l�߂��܂�
:WARNING. 9
This message indicates that the compiler knows that all values will
not be preserved after the assignment or initialization.
If this is acceptable, cast the
value to the appropriate type in the assignment or initialization.
:errbad.
char inc( char c )
{
    return c + 1;
}
:eerrbad.

:MSGSYM. INF_LEFT_OPERAND_TYPE
:MSGTXT. left operand type is '%T'
:MSGJTXT. ���I�y�����h�̌^��'%T'�ł�
:INFO.
This informational message indicates the type of the left hand
side of the expression.

:MSGSYM. INF_RIGHT_OPERAND_TYPE
:MSGTXT. right operand type is '%T'
:MSGJTXT. �E�I�y�����h�̌^��'%T'�ł�
:INFO.
This informational message indicates the type of the right hand
side of the expression.

:MSGSYM. INF_OPERAND_TYPE
:MSGTXT. operand type is '%T'
:MSGJTXT. �I�y�����h�̌^��'%T'�ł�
:INFO.
This informational message indicates the type of the operand.

:MSGSYM. INF_EXPR_TYPE
:MSGTXT. expression type is '%T'
:MSGJTXT. ���̌^��'%T'�ł�
:INFO.
This informational message indicates the type of the expression.

:MSGSYM. ERR_CANT_GENERATE_RETURN_THUNK
:MSGTXT. virtual function '%S' cannot have its return type changed
:MSGJTXT. ���z�֐�'%S'�́C���̖߂�^��ύX�ł��܂���ł���
This restriction is due to the relatively new feature in the C++
language that allows return values to be changed when a virtual
function has been overridden.  It is not possible to support both
features because in order to support changing the return value of
a function, the compiler must construct a "wrapper" function that
will call the virtual function first and then change the return value
and return.  It is not possible to do this with "..." style functions
because the number of parameters is not known.
:errbad.
struct B {
};
struct D : virtual B {
};

struct X {
    virtual B *fn( int, ... );
};
struct Y : X {
    virtual D *fn( int, ... );
};
:eerrbad.

:MSGSYM. ERR_UNSUPPORTED_DECLSPEC
:MSGTXT. __declspec( '%N' ) is not supported
:MSGJTXT. __declspec('%N'�j�̓T�|�[�g����Ă��܂���
The identifier used in the
.kw __declspec
declaration modifier is not supported by Open Watcom C++.

:MSGSYM. ERR_CTOR_OBJ_MEM_MODEL
:MSGTXT. attempt to construct a far object when data model is near
:MSGJTXT. �f�[�^���f����near�ł���Ƃ��Cfar�I�u�W�F�N�g�����낤�Ƃ��Ă��܂�
Constructors cannot be applied to objects which are stored in far memory
when the default memory model for data is near.
:errbad.
struct Obj
{   char *p;
    Obj();
};

Obj far obj;
:eerrbad.
.np
The last line causes this error to be displayed when the memory model is
small (switch -ms), since the memory model for data is near.

:MSGSYM. WARN_ZO_OBSOLETE
:MSGTXT. -zo is an obsolete switch (has no effect)
:MSGJTXT. -zo�͋����̃X�C�b�`�ł�(���ʂ͂���܂���)
:WARNING. 1
The
.kw -zo
option was required in an earlier version of the compiler but is no
longer used.

:MSGSYM. WARN_USER_WARNING_MSG
:MSGTXT. "%s"
:MSGJTXT. "%s"
:WARNING. 1
This is a user message generated with the
.kw #pragma
.kw message
preprocessing directive.
:errbad.
#pragma message( "my very own warning" );
:eerrbad.

:MSGSYM. WARN_PARM_NOT_REFERENCED
:MSGTXT. no reference to formal parameter '%S'
:MSGJTXT. �`������'%S'�͎Q�Ƃ���Ă��܂���
:WARNING. 4
There are no references to the declared formal parameter.
The simplest way to remove this warning in C++ is to
remove the name from the argument declaration.
:errbad.
int fn1( int a, int b, int c )
{
    // 'b' not referenced
    return a + c;
}
int fn2( int a, int /* b */, int c )
{
    return a + c;
}
:eerrbad.

:MSGSYM. ERR_VOID_INDIRECTION
:MSGTXT. cannot dereference a pointer to void
:MSGJTXT. 'void'�^�ւ̃|�C���^�͎Q�Ƃł��܂���
A pointer to
.kw void
is used as a generic pointer but it cannot be dereferenced.
:errbad.
void fn( void *p )
{
    return *p;
}
:eerrbad.

:MSGSYM. WARN_CONFLICTING_CLASS_MODS
:MSGTXT. class modifiers for '%T' conflict with class modifiers for '%T'
:MSGJTXT. '%T'�ɑ΂���N���X�C���q�́C'%T'�̂��߂ɃN���X�C���q�Ɩ������܂�
:WARNING. 1
A conflict between class modifiers for classes related through inheritance
has been detected.
A conflict will occur if two base classes have
class modifiers that are different.
The conflict can be resolved by ensuring that all classes related
through inheritance have the same class modifiers.
The default resolution is to have no class modifier for the derived base.
:errbad.
struct __cdecl B1 {
    void fn( int );
};
struct __stdcall B2 {
    void fn( int );
};
struct D : B1, B2 {
};
:eerrbad.

:MSGSYM. ERR_INVALID_HEX_CONSTANT
:MSGTXT. invalid hexadecimal constant
:MSGJTXT. �s�K�؂�16�i�萔�ł�
The constant started with a '0x' prefix which makes it look like a hexadecimal
constant but the constant was not followed by any hexadecimal digits.
:errbad.
unsigned i = 0x;     // invalid hex constant
:eerrbad.

:MSGSYM. WARN_OPERATOR_ARROW_WONT_WORK
:MSGTXT. return type of 'operator ->' will not allow '->' to be applied
:MSGJTXT. 'operator ->'�̖߂�^�́C'->'���K�p�ł��܂���
:WARNING. 1
This restriction is a result of the transformation that the compiler performs
when the
.kw operator ->
is overloaded.  The transformation involves transforming the expression
to invoke the operator with "->" applied to the result of
.kw operator ->.
This warning indicates that the
.kw operator ->
can never be used as an overloaded operator.
The only way the operator can be used is to explicitly call it by name.
:errbad.
struct S {
    int a;
    void *operator ->();
};

void *fn( S &q )
{
    return q.operator ->();
}
:eerrbad.

:MSGSYM. WARN_UNNAMED_CLASS_HAS_SPECIAL_MEMBER
:MSGTXT. class should have a name since it needs a constructor or a destructor
:MSGJTXT. �N���X�̓R���X�g���N�^�܂��̓f�X�g���N�^��K�v�Ƃ���̂ŁC���O�������Ȃ���΂Ȃ�܂���
:WARNING. 1
The class definition does not have a class name but it includes members
that have constructors or destructors.
Since the class has C++ semantics, it should be have a name in case the
constructor or destructor needs to be referenced.
:errbad.
struct P {
    int x,y;
    P();
};

typedef struct {
    P c;
    int v;
} T;
:eerrbad.

:MSGSYM. WARN_UNNAMED_CLASS_INHERITS
:MSGTXT. class should have a name since it inherits a class
:MSGJTXT. �N���X�͌p������̂ŁC���O�����ׂ��ł�
:WARNING. 1
The class definition does not have a class name but it inherits a class.
Since the class has C++ semantics, it should be have a name in case the
constructor or destructor needs to be referenced.
:errbad.
struct P {
    int x,y;
    P();
};

typedef struct : P {
    int v;
} T;
:eerrbad.

:MSGSYM. ERR_PCH_OPEN_ERROR
:MSGTXT. cannot open pre-compiled header file '%s'
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C��'%s'���J���܂���
The compiler has detected a problem while trying to open the
pre-compiled header file for read/write access.

:MSGSYM. ERR_INVALID_VASTART_SYMBOL
:MSGTXT. invalid second argument to va_start
:MSGJTXT. 'va_start'�ւ̑�2�������s�K�؂ł�
The second argument to the va_start macro should be
the name of the argument just before the "..." in the
argument list.

:MSGSYM. WARN_SPLICE_IN_CPP_COMMENT
:MSGTXT. '//' style comment continues on next line
:MSGJTXT. '//'�`���̃R�����g�����s�Ɍp�����܂�
:WARNING. 1
The compiler has detected a line continuation during the processing
of a C++ style comment ("//").  The warning can be removed by switching to
a C style comment ("/**/").  If you require the comment to be terminated
at the end of the line, make sure that the backslash character is not
the last character in the line.
:errbad.
#define XX 23 // comment start \
comment \
end

int x = XX; // comment start ...\
comment end
:eerrbad.

:MSGSYM. ERR_CANNOT_CREATE_OUTPUT_FILE
:MSGTXT. cannot open file '%s' for write access
:MSGJTXT. �������݃A�N�Z�X�ŁC�t�@�C��'%s'���I�[�v���ł��܂���
The compiler has detected a problem while trying to open the indicated
file for write access.

:MSGSYM. ERR_PTR_INTEGER_EXTENSION
:MSGTXT. implicit conversion of pointers to integral types of same size
:MSGJTXT. �����傫�̐����^�ւ̃|�C���^�ւ̈Öق̕ϊ��ł�
According to the ISO/ANSI Draft Working Paper, a string literal is an array of
.kw char.
Consequently, it is illegal to initialize or assign the pointer resulting
from that literal to a pointer of either
.kw unsigned char
or
.kw signed char,
since these pointers point at objects of a different type.

:MSGSYM. ERR_INVALID_OPTION_NUMBER
:MSGTXT. option requires a number
:MSGJTXT. �I�v�V�����ɐ��l���K�v�ł�
The specified option is not recognized by the compiler
since there was no number after it (i.e., "-w=1").
Numbers must be non-negative decimal numbers.

:MSGSYM. ERR_FC_MORE_THAN_ONCE
:MSGTXT. option -fc specified more than once
:MSGJTXT. �I�v�V����-fc��2��ȏ�w�肳��Ă��܂�
The -fc option can be specified at most once on a command line.

:MSGSYM. ERR_FC_IN_BATCH_FILE
:MSGTXT. option -fc specified in batch file of commands
:MSGJTXT. �I�v�V����-fc�̓R�}���h�̃o�b�`��t�@�C���̒��Ŏw�肳��Ă��܂�
The -fc option cannot be specified on a line in the
batch file of command lines specified by the -fc option on the
command line used to invoke the compiler.

:MSGSYM. ERR_FC_EMPTY
:MSGTXT. file specified by -fc is empty or cannot be read
:MSGJTXT. -fc�Ŏw�肳�ꂽ�t�@�C���͋󂩁C�ǂݍ��݂ł��܂���
The file specified using the -fc option is either empty or an input/output
error was diagnosed for the file.

:MSGSYM. ERR_FC_OPEN
:MSGTXT. cannot open file specified by -fc option
:MSGJTXT. -fc�I�v�V�����Ŏw�肳�ꂽ�t�@�C�����I�[�v���ł��܂���
The compiler was unable to open the indicated file.  Most likely, the file
does not exist.  An input/output error is also possible.

:MSGSYM. ERR_FC_READ
:MSGTXT. input/output error reading the file specified by -fc option
:MSGJTXT. -fc�I�v�V�����Ŏw�肳�ꂽ�t�@�C����ǂݍ��ނƂ��C���o�̓G���[���o�܂���
The compiler was unable to open the indicated file.  Most likely, the file
does not exist.  An input/output error is also possible.

:MSGSYM. WARN_OPERATOR_BAD_RETURN
:MSGTXT. '%N' does not have a return type specified (int assumed)
:MSGJTXT. '%N'�͎w�肳�ꂽ�߂�^�������܂���('int'�����肵�܂�)
:WARNING. 1
In C++, operator functions should have an explicit return type
specified.  In future revisions of the ISO/ANSI C++ standard,
the use of default int type specifiers may be prohibited so
removing any dependencies on default int early will prevent problems
in the future.
:errbad.
struct S {
    operator = ( S const & );
    operator += ( S const & );
};
:eerrbad.

:MSGSYM. ERR_CONST_REF_INIT
:MSGTXT. cannot initialize reference to non-constant with a constant object
:MSGJTXT. ��萔�ւ̎Q�Ƃ�萔�I�u�W�F�N�g�ŏ��������邱�Ƃ͂ł��܂���
A reference to a non-constant object cannot be initialized with a reference to
a constant type because this would allow constant data to be modified
via the non-constant pointer to it.
:errbad.
extern const int *pic;
extern int & ref = pic;
:eerrbad.

:MSGSYM. INF_SWITCH
:MSGTXT. processing %s
:MSGJTXT. %s���������܂�
:INFO.
This informational message indicates where an error or warning was detected
while processing the switches specified on the command line, in environment
variables, in command files (using the '@' notation), or in the batch command
file (specified using the -fc option).

:MSGSYM. INF_CLASS_NOT_DEFINED
:MSGTXT. class '%T' has not been defined
:MSGJTXT. class '%T'�͒�`����܂���ł���
:INFO.
This informational message indicates a class which was not defined.  This is
noted following an error or warning message because it often helps to a user
to determine the cause of that diagnostic.

:MSGSYM. ERR_CATCH_UNDEFED
:MSGTXT. cannot catch undefined class object
:MSGJTXT. ����`�N���X��I�u�W�F�N�g��catch�ł��܂���
C++ does not allow abstract classes to be copied and so an undefined
class object cannot be specified in a
.kw catch
clause.  It is permissible to catch a reference to an undefined class.

:MSGSYM. ERR_CLASS_CORRUPTED
:MSGTXT. class '%T' cannot be used since its definition has errors
:MSGJTXT. �N���X��`���G���[�Ȃ̂ŁC�N���X'%T'�͎g�p�ł��܂���
The analysis of the expression could not continue due to previous
errors diagnosed in the class definition.

:MSGSYM. WARN_LOCAL_FN_PROTOTYPE
:MSGTXT. function prototype in block scope missing 'extern'
:MSGJTXT. �u���b�N��X�R�[�v�̒��̊֐��v���g�^�C�v��'extern'������܂���
:WARNING. 1
This warning can be triggered when the intent is to define a variable
with a constructor.
Due to the complexities of parsing C++, statements that
appear to be variable definitions may actually parse as a function
prototype.  A work-around for this problem is contained in the example.
If a prototype is desired, add the
.kw extern
storage class to remove this warning.
:errbad.
struct C {
};
struct S {
    S( C );
};
void foo()
{
    S a( C() ); // function prototype!
    S b( (C()) );// variable definition

    int bar( int );// warning
    extern int sam( int ); // no warning
}
:eerrbad.

:MSGSYM. INF_FUNCTION_PROTOTYPE
:MSGTXT. function prototype is '%T'
:MSGJTXT. �֐��v���g�^�C�v��'%T'�ł�
:INFO.
This informational message indicates what the type of the function
prototype is for the message in question.

:MSGSYM. WARN_ZERO_ARRAY_CLASS_USED
:MSGTXT. class '%T' contains a zero size array
:MSGJTXT. �N���X'%T'�ɂ͑傫�����[���̔z�񂪂���܂�
:WARNING. 1
This warning is triggered when a class with a zero sized array
is used in an array or as a class member.
This is a questionable practice since a zero sized array at the
end of a class often indicates a class that is dynamically sized
when it is constructed.
:errbad.
struct C {
    C *next;
    char name[];
};

struct X {
    C q;
};

C a[10];
:eerrbad.

:MSGSYM. ERR_INVALID_NEW_MODIFIER
:MSGTXT. invalid 'new' modifier
:MSGJTXT. 'new'�ɑ΂��ĕs�K�؂ȏC���q�ł�
The Open Watcom C++ compiler does not support new expression modifiers
but allows them to match the ambient memory model for compatibility.
Invalid memory model modifiers are also rejected by the compiler.
:errbad.
int *fn( unsigned x )
{
    return new __interrupt int[x];
}
:eerrbad.

:MSGSYM. ERR_THREAD_CODE_REQD
:MSGTXT. '__declspec(thread)' data '%S' must be link-time initialized
:MSGJTXT. '__declspec(thread)'�f�[�^'%S'�̓����N���ɏ���������Ȃ���΂Ȃ�܂���
This error message indicates that the data item in question either
requires a constructor, destructor, or run-time initialization.
This cannot be supported for thread-specific data at this time.
:errbad.
#include <stdlib.h>

struct C {
    C();
};
struct D {
    ~D();
};

C __declspec(thread) c;
D __declspec(thread) d;
int __declspec(thread) e = rand();
:eerrbad.

:MSGSYM. WARN_CODE_MAY_BE_SPLIT_ACROSS_SEGS
:MSGTXT. code may not work properly if this module is split across a code segment
:MSGJTXT. ���̃��W���[�����R�[�h��Z�O�����g���܂������ĕ��������ƁC�R�[�h�͐��������삵�Ȃ���������܂���
:WARNING. 4
The "zm" option allows the compiler to generate functions into separate
segments that have different names so that more than 64k of code can
be generated in one object file.  Unfortunately, if an explicit near
function is coded in a large code model, the possibility exists that
the linker can place the near function in a separate code segment than
a function that calls it.  This would cause a linker error followed by
an execution error if the executable is executed.
The "zmf" option can be used if you require explicit near functions
in your code.
:errbad.
// These functions may not end up in the
// same code segment if the -zm option
// is used. If this is the case, the near
// call will not work since near functions
// must be in the same code segment to
// execute properly.
static int near near_fn( int x )
{
    return x + 1;
}

int far_fn( int y )
{
    return near_fn( y * 2 );
}
:eerrbad.

:MSGSYM. ERR_PRAG_EXTREF_NONE
:MSGTXT. #pragma extref: symbol '%N' not declared
:MSGJTXT. #pragma extref: �V���{��'%N'���錾����Ă��܂���
This error message indicates that the symbol referenced by
.kw #pragma extref
has not been declared in the context where the pragma was
encountered.

:MSGSYM. ERR_PRAG_EXTREF_OVERLOADED
:MSGTXT. #pragma extref: overloaded function '%S' cannot be used
:MSGJTXT. #pragma extref: �I�[�o�[���[�h�֐�'%S'���g�p�ł��܂���
An external reference can be emitted only for
external functions which are not overloaded.

:MSGSYM. ERR_PRAG_EXTREF_BAD
:MSGTXT. #pragma extref: '%N' is not a function or data
:MSGJTXT. #pragma extref: '%N'�͊֐��܂��̓f�[�^�ł͂���܂���
This error message indicates that the symbol referenced by
.kw #pragma extref
cannot have an external reference emitted for it
because the referenced symbol is neither a function nor a data item.
An external reference can be emitted only for external functions which are
not overloaded and for external data items.

:MSGSYM. ERR_PRAG_EXTREF_EXTERN
:MSGTXT. #pragma extref: '%S' is not external
:MSGJTXT. #pragma extref: '%S'�͊O���Q�Ƃł͂���܂���
This error message indicates that the symbol referenced by
.kw #pragma extref
cannot have an external reference emitted for it
because the symbol is not external.
An external reference can be emitted only for external functions which are
not overloaded and for external data items.

:MSGSYM. WARN_PCH_DEBUG_OPTIMIZE
:MSGTXT. pre-compiled header file being recreated (debugging info may change)
:MSGJTXT. �v���R���p�C����w�b�_�[��t�@�C���͍č쐬����܂�(�f�o�b�O��񂪕ς������������܂���)
:WARNING. 1
The compiler has detected that the module being compiled was used to create
debugging information for use by other modules.  In order to maintain
correctness, the pre-compiled header file must be recreated along with
the object file.

:MSGSYM. ANSI_INVALID_OCTAL_ESCAPE
:MSGTXT. octal escape sequence out of range; truncated
:MSGJTXT. 8�i���̃G�X�P�[�v��V�[�P���X���͈͊O�ł�; �؂�l�߂��܂�
:ANSI. 1
This message indicates that the octal escape sequence
produces an integer that cannot fit into the required character type.
:errbad.
char *p = "\406";
:eerrbad.

:MSGSYM. ERR_BINARY_MISSING_RIGHT_OPERAND
:MSGTXT. binary operator '%s' missing right operand
:MSGJTXT. 2�����Z�q'%s'�ɉE�I�y�����h������܂���
There is no expression to the right of the indicated binary operator.

:MSGSYM. ERR_BINARY_MISSING_LEFT_OPERAND
:MSGTXT. binary operator '%s' missing left operand
:MSGJTXT. 2�����Z�q'%s'�ɍ��I�y�����h������܂���
There is no expression to the left of the indicated binary operator.

:MSGSYM. ERR_EXTRA_OPERAND
:MSGTXT. expression contains extra operand(s)
:MSGJTXT. ���ɗ]���ȃI�y�����h������܂�
The expression contains operand(s) without an operator

:MSGSYM. ERR_CONSECUTIVE_OPERANDS
:MSGTXT. expression contains consecutive operand(s)
:MSGJTXT. ���ɘA�������I�y�����h������܂�
More than one operand found in a row.

:MSGSYM. ERR_UNMATCHED_RIGHT_PAREN
:MSGTXT. unmatched right parenthesis ')'
:MSGJTXT. �E����")"����v���܂���
The expression contains a right parenthesis ")" without a matching left
parenthesis.

:MSGSYM. ERR_UNMATCHED_LEFT_PAREN
:MSGTXT. unmatched left parenthesis '('
:MSGJTXT. ������"("����v���܂���
The expression contains a left parenthesis "(" without a matching right
parenthesis.

:MSGSYM. ERR_EMPTY_PAREN
:MSGTXT. no expression between parentheses '( )'
:MSGJTXT. ����"()"�̒��Ɏ�������܂���
There is a matching set of parenthesis "()" which do not contain an expression.

:MSGSYM. ERR_CONDITIONAL_MISSING_COLON
:MSGTXT. expecting ':' operator in conditional expression
:MSGJTXT. ��������':'���Z�q������܂���
A conditional expression exists without the ':' operator.

:MSGSYM. ERR_CONDITIONAL_MISSING_QUESTION
:MSGTXT. expecting '?' operator in conditional expression
:MSGJTXT. ��������'?'���Z�q������܂���
A conditional expression exists without the '?' operator.

:MSGSYM. ERR_CONDITIONAL_MISSING_FIRST_OPERAND
:MSGTXT. expecting first operand in conditional expression
:MSGJTXT. �������̑�1�I�y�����h������܂���
A conditional expression exists without the first operand.

:MSGSYM. ERR_CONDITIONAL_MISSING_SECOND_OPERAND
:MSGTXT. expecting second operand in conditional expression
:MSGJTXT. �������̑�2�I�y�����h������܂���
A conditional expression exists without the second operand.

:MSGSYM. ERR_CONDITIONAL_MISSING_THIRD_OPERAND
:MSGTXT. expecting third operand in conditional expression
:MSGJTXT. �������̑�3�I�y�����h������܂���
A conditional expression exists without the third operand.

:MSGSYM. ERR_UNARY_OPERATOR_MISSING_OPERAND
:MSGTXT. expecting operand after unary operator '%s'
:MSGJTXT. �P�����Z�q'%s'�̌�̃I�y�����h������܂���
A unary operator without being followed by an operand.

:MSGSYM. ERR_UNEXPECTED_IN_CONSTANT_EXPRESSION
:MSGTXT. '%s' unexpected in constant expression
:MSGJTXT. �������̒���'%s'������܂�
'%s' not allowed in constant expression

:MSGSYM. WARN_ASSEMBLER_WARNING
:MSGTXT. assembler: '%s'
:MSGJTXT. �A�Z���u��: '%s'
:WARNING. 1
A warning has been issued by the #pragma inline assembler.

:MSGSYM. ERR_COLON_COLON_SYNTAX
:MSGTXT. expecting 'id' after '::' but found '%s'
:MSGJTXT. '::'�̌��'id'������͂��ł����C'%s'������܂�
The '::' operator has an invalid token following it.
:errbad.
#define fn( x ) ((x)+1)

struct S {
    int inc( int y ) {
    return ::fn( y );
    }
};
:eerrbad.

:MSGSYM. ERR_EXPLICIT_FNS
:MSGTXT. only constructors can be declared explicit
:MSGJTXT. �R���X�g���N�^�̂�'explicit'�Ɛ錾�ł��܂�
Currently, only constructors can be declared with the
.kw explicit
keyword.
:errbad.
int explicit fn( int x ) {
    return x + 1;
}
:eerrbad.


:MSGSYM. ERR_CONST_CAST_TYPE
:MSGTXT. const_cast type must be pointer, member pointer, or reference
:MSGJTXT. const_cast�^�̓|�C���^�C�����o�[�|�C���^�C�Q�Ƃ̂����ꂩ�łȂ���΂Ȃ�܂���
The type specified in a
.kw const_cast
operator must be a pointer, a pointer to a member of a class, or a reference.
:errbad.
extern int const *p;
long lp = const_cast<long>( p );
:eerrbad.

:MSGSYM. ERR_CONST_CAST_PTR_TYPE
:MSGTXT. const_cast expression must be pointer to same kind of object
:MSGJTXT. const_cast���͓�����ނ̃I�u�W�F�N�g�ւ̃|�C���^�łȂ���΂Ȃ�܂���
Ignoring
.kw const
and
.kw volatile
qualification, the expression must be a pointer to the same type of object
as that specified in the
.kw const_cast
operator.
:errbad.
extern int const * ip;
long* lp = const_cast<long*>( ip );
:eerrbad.

:MSGSYM. ERR_CONST_CAST_REF_TYPE
:MSGTXT. const_cast expression must be lvalue of the same kind of object
:MSGJTXT. const_cast���͓�����ނ̃I�u�W�F�N�g�̍��Ӓl�łȂ���΂Ȃ�܂���
Ignoring
.kw const
and
.kw volatile
qualification, the expression must be an lvalue or reference to the same type
of object as that specified in the
.kw const_cast
operator.
:errbad.
extern int const i;
long& lr = const_cast<long&>( i );
:eerrbad.

:MSGSYM. ERR_CONST_CAST_MPTR_CLASS
:MSGTXT. expression must be pointer to member from same class in const_cast
:MSGJTXT. ����const_cast�̓����N���X����̃����o�[�ւ̃|�C���^�łȂ���΂Ȃ�܂���
The expression must be a pointer to member from the same class
as that specified in the
.kw const_cast
operator.
:errbad.
struct B {
    int ib;
};
struct D : public B {
};
extern int const B::* imb;
int D::* imd const_cast<int D::*>( imb );
:eerrbad.

:MSGSYM. ERR_CONST_CAST_MPTR_TYPE
:MSGTXT. expression must be member pointer to same type as specified in const_cast
:MSGJTXT. ����const_cast�Ŏw�肳�ꂽ�̂Ɠ����^�ւ̃����o�[�|�C���^�łȂ���΂Ȃ�܂���
Ignoring
.kw const
and
.kw volatile
qualification, the expression must be a pointer to member of the same type
as that specified in the
.kw const_cast
operator.
:errbad.
struct B {
    int ib;
    long lb;
};
int D::* imd const_cast<int D::*>( &B::lb );
:eerrbad.

:MSGSYM. ERR_REINT_CAST_PTR_TYPE
:MSGTXT. reinterpret_cast expression must be pointer or integral object
:MSGJTXT. reinterpret_cast���|�C���^�܂��͐����I�u�W�F�N�g�łȂ���΂Ȃ�܂���
When a pointer type is specified in the
.kw reinterpret_cast
operator, the expression must be a pointer or an integer.
:errbad.
extern float fval;
long* lp = const_cast<long*>( fval );
:eerrbad.
The expression has
.kw float
type and so is illegal.

:MSGSYM. ERR_REINT_CAST_REF_TYPE
:MSGTXT. reinterpret_cast expression cannot be casted to reference type
:MSGJTXT. reinterpret_cast���͎Q�ƌ^�ɃL���X�g���邱�Ƃ͂ł��܂���
When a reference type is specified in the
.kw reinterpret_cast
operator, the expression must be an lvalue (or have reference type).
Additionally, constness cannot be casted away.
:errbad.
extern long f;
extern const long f2;
long& lr1 = const_cast<long&>( f + 2 );
long& lr2 = const_cast<long&>( f2 );
:eerrbad.
Both initializations are illegal.
The first cast expression is not an lvalue.
The second cast expression attempts to cast away constness.

:MSGSYM. ERR_REINT_CAST_MPTR_TYPE
:MSGTXT. reinterpret_cast expression cannot be casted to pointer to member
:MSGJTXT. reinterpret_cast���̓����o�[�ւ̃|�C���^�ɃL���X�g���邱�Ƃ͂ł��܂���
When a pointer to member type is specified in the
.kw reinterpret_cast
operator, the expression must be a pointer to member.
Additionally, constness cannot be casted away.
:errbad.
extern long f;
struct S {
    const long f2;
    S();
};
long S::* mp1 = const_cast<long S:: *>( f );
long S::* mp2 = const_cast<long S:: *>( &S::f2 );
:eerrbad.
Both initializations are illegal.
The first cast expression does not involve a member pointer.
The second cast expression attempts to cast away constness.

:MSGSYM. ERR_REINT_CAST_INT_TYPE
:MSGTXT. only integral arithmetic types can be used with reinterpret_cast
:MSGJTXT. �����Z�p�^�݂̂�reinterpret_cast�ƂƂ��Ɏg�p�ł��܂�
Pointers can only be casted to sufficiently large integral types.
:errbad.
void* p;
float f = reinterpret_cast<float>( p );
:eerrbad.
The cast is illegal because
.kw float
type is specified.

:MSGSYM. ERR_REINT_CAST_ARITH_PTR
:MSGTXT. only integral arithmetic types can be used with reinterpret_cast
:MSGJTXT. �����Z�p�^�݂̂�reinterpret_cast�ƂƂ��Ɏg�p�ł��܂�
Only integral arithmetic types can be casted to pointer types.
:errbad.
float flt;
void* p = reinterpret_cast<void*>( flt );
:eerrbad.
The cast is illegal because
.id flt
has
.kw float
type which is not integral.

:MSGSYM. ERR_CAST_AWAY_CONST
:MSGTXT. cannot cast away constness
:MSGJTXT. const���͂����L���X�g�͂ł��܂���
A cast or implicit conversion is illegal because a conversion to the target
type would remove constness from a pointer, reference, or pointer to member.
:errbad.
struct S {
    int s;
};
extern S const * ps;
extern int const S::* mps;
S* ps1 = ps;
S& rs1 = *ps;
int S::* mp1 = mps;
:eerrbad.
The three initializations are illegal since they are attempts to remove
constness.

:MSGSYM. ERR_REINT_INTEGRAL_PTR
:MSGTXT. size of integral type in cast less than size of pointer
:MSGJTXT. �L���X�g�̐����^�̑傫�����|�C���^�̑傫����菬�����ł�
An object of the indicated integral type is too small to contain the value
of the indicated pointer.
:errbad.
int x;
char p = reinterpret_cast<char>( &x );
char q = (char)( &x );
:eerrbad.
Both casts are illegal since a
.kw char
is smaller than a pointer.

:MSGSYM. ERR_REINT_CAST_TYPE
:MSGTXT. type cannot be used in reinterpret_cast
:MSGJTXT. ���̌^��reinterpret_cast�Ŏg�p�ł��܂���
The type specified with reinterpret_cast must be an integral type, a pointer
type, a pointer to a member of a class, or a reference type.
:errbad.
void* p;
float f = reinterpret_cast<float>( p );
void* q = ( reinterpret_cast<void>( p ), p );
:eerrbad.
The casts specify illegal types.

:MSGSYM. ERR_REINT_TO_INT_TYPE
:MSGTXT. only pointers can be casted to integral types with reinterpret_cast
:MSGJTXT. �|�C���^�݂̂�reinterpret_cast�Ő����^�ɃL���X�g�ł��܂�
The expression must be a pointer type.
:errbad.
void* p;
float f = reinterpret_cast<float>( p );
void* q = ( reinterpret_cast<void>( p ), p );
:eerrbad.
The casts specify illegal types.

:MSGSYM. ERR_REINT_TO_PTR_TYPE
:MSGTXT. only integers and pointers can be casted to pointer types with reinterpret_cast
:MSGJTXT. �����ƃ|�C���^�݂̂�reinterpret_cast�Ń|�C���^�ɃL���X�g�ł��܂�
The expression must be a pointer or integral type.
:errbad.
void* x;
void* p = reinterpret_cast<void*>( 16 );
void* q = ( reinterpret_cast<void*>( x ), p );
:eerrbad.
The casts specify illegal types.

:MSGSYM. ERR_STATIC_CAST_EXPR
:MSGTXT. static_cast cannot convert the expression
:MSGJTXT. static_cast�͎���ϊ��ł��܂���
The indicated expression cannot be converted to the type specified with the
.kw static_cast
operator.
Perhaps reinterpret_cast or dynamic_cast should be used instead;

:MSGSYM. ERR_STATIC_CAST_TYPE
:MSGTXT. static_cast cannot be used with the type specified
:MSGJTXT. static_cast�͎w�肳�ꂽ�^�Ƃ�������ɂ͎g���܂���
A static cast cannot be used with a function type or array type.
:errbad.
typedef int fun( int );
extern int poo( long );
int i = ( static_cast<fun)( poo ) )( 22 );
:eerrbad.
Perhaps reinterpret_cast or dynamic_cast should be used instead;

:MSGSYM. ERR_STATIC_CAST_REF_TYPE
:MSGTXT. static_cast cannot be used with the reference type specified
:MSGJTXT. static_cast�͎w�肳�ꂽ�Q�ƌ^�Ƃ�������ɂ͎g���܂���
The expression could not be converted to the specified type using static_cast.
:errbad.
long lng;
int& ref = static_cast<int&>( lng );
:eerrbad.
Perhaps reinterpret_cast or dynamic_cast should be used instead;

:MSGSYM. ERR_STATIC_CAST_PTR_TYPE
:MSGTXT. static_cast cannot be used with the pointer type specified
:MSGJTXT. static_cas�͎w�肳�ꂽ�|�C���^�^�Ƃ�������ɂ͎g���܂���
The expression could not be converted to the specified type using static_cast.
:errbad.
long lng;
int* ref = static_cast<int*>( lng );
:eerrbad.
Perhaps reinterpret_cast or dynamic_cast should be used instead;

:MSGSYM. ERR_STATIC_CAST_MPTR_TYPE
:MSGTXT. static_cast cannot be used with the member pointer type specified
:MSGJTXT. static_cast�w�肳�ꂽ�����o�[�|�C���^�^�Ƃ�������ɂ͎g���܂���
The expression could not be converted to the specified type using static_cast.
:errbad.
struct S {
    long lng;
};
int S::* mp = static_cast<int S::*>( &S::lng );
:eerrbad.
Perhaps reinterpret_cast or dynamic_cast should be used instead;

:MSGSYM. ERR_STATIC_CAST_AMBIG
:MSGTXT. static_cast type is ambiguous
:MSGJTXT. static_cast���ꂽ�^�͞B���ł�
More than one constructor and/or used-defined conversion function can be used
to convert the expression to the indicated type.

:MSGSYM. ERR_CAST_FROM_AMBIGUITY
:MSGTXT. cannot cast from ambiguous base class
:MSGJTXT. �B���ȃx�[�X�N���X����L���X�g���邱�Ƃ͂ł��܂���
When more than one base class of a given type exists, with respect to a
derived class, it is impossible to cast from the base class to the
derived class.
:errbad.
struct Base { int b1; };
struct DerA public Base { int da; };
struct DerB public Base { int db; };
struct Derived public DerA, public DerB { int d; }
Derived* foo( Base* p )
{
    return static_cast<Derived*>( p );
}
:eerrbad.
The cast fails since
.id Base
is an ambiguous base class for
.id Derived.

:MSGSYM. ERR_CAST_TO_AMBIGUITY
:MSGTXT. cannot cast to ambiguous base class
:MSGJTXT. �B���ȃx�[�X�N���X�փL���X�g���邱�Ƃ͂ł��܂���
When more than one base class of a given type exists, with respect to a
derived class, it is impossible to cast from the derived class to the
base class.
:errbad.
struct Base { int b1; };
struct DerA public Base { int da; };
struct DerB public Base { int db; };
struct Derived public DerA, public DerB { int d; }
Base* foo( Derived* p )
{
    return (Base*)p;
}
:eerrbad.
The cast fails since
.id Base
is an ambiguous base class for
.id Derived.

:MSGSYM. ERR_STATIC_CAST_OTHER_TO_ENUM
:MSGTXT. can only static_cast integers to enumeration type
:MSGJTXT. static_cast�݂̂�������񋓌^�փL���X�g�ł��܂�
When an enumeration type is specified with
.kw static_cast,
the expression must be an integer.
:errbad.
enum sex { male, female };
sex father = static_cast<sex>( 1.0 );
:eerrbad.
The cast is illegal because the expression is not an integer.

:MSGSYM. ERR_DYNAMIC_CAST_TYPE
:MSGTXT. dynamic_cast cannot be used with the type specified
:MSGJTXT. dynamic_cast�͎w�肳�ꂽ�^�Ǝ����ɂ͎g���܂���
A dynamic cast can only specify a reference to a class or a pointer to
a class or
.kw void.
When a class is referenced, it must have virtual functions defined within
that class or a base class of that class.

:MSGSYM. ERR_DYNAMIC_CAST_EXPR
:MSGTXT. dynamic_cast cannot convert the expression
:MSGJTXT. dynamic_cast�͂��̎���ϊ��ł��܂���
The indicated expression cannot be converted to the type specified with the
.kw dynamic_cast
operator. Only a pointer or reference to a class object can be converted.
When a class object is referenced, it must have virtual functions defined
within that class or a base class of that class.

:MSGSYM. ERR_DYNAMIC_CAST_NO_VFN
:MSGTXT. dynamic_cast requires class '%T' to have virtual functions
:MSGJTXT. dynamic_cast���g�p����ɂ́C�N���X'%T'�͉��z�֐��������Ȃ���΂Ȃ�܂���
The indicated class must have virtual functions defined
within that class or a base class of that class.

:MSGSYM. WARN_DYNAMIC_CAST_AMBIGUOUS
:MSGTXT. base class for type in dynamic_cast is ambiguous (will fail)
:MSGJTXT. dynamic_cast�̒��̌^�̃x�[�X�N���X���B���ł�(�G���[�ɂȂ�܂�)
:WARNING. 1
The type in the
.kw dynamic_cast
is a pointer or reference to an ambiguous base class.
:errbad.
struct A { virtual void f(){}; };
struct D1 : A { };
struct D2 : A { };
struct D : D1, D2 { };

A *foo( D *p ) {
    // will always return NULL
    return( dynamic_cast< A* >( p ) );
}
:eerrbad.

:MSGSYM. WARN_DYNAMIC_CAST_PRIVATE
:MSGTXT. base class for type in dynamic_cast is private (may fail)
:MSGJTXT. dynamic_cast�̒��̌^�̃x�[�X�N���X�̓v���C�x�[�g�����ł�(�G���[�ɂȂ�܂�)
:WARNING. 1
The type in the
.kw dynamic_cast
is a pointer or reference to a private base class.
:errbad.
struct V { virtual void f(){}; };
struct A : private virtual V { };
struct D : public virtual V, A { };

V *foo( A *p ) {
    // returns NULL if 'p' points to an 'A'
    // returns non-NULL if 'p' points to a 'D'
    return( dynamic_cast< V* >( p ) );
}
:eerrbad.

:MSGSYM. WARN_DYNAMIC_CAST_PROTECTED
:MSGTXT. base class for type in dynamic_cast is protected (may fail)
:MSGJTXT. dynamic_cast�̒��̌^�̃x�[�X�N���X�̓v���e�N�g�����ł�(�G���[�ɂȂ邩������܂���)
:WARNING. 1
The type in the
.kw dynamic_cast
is a pointer or reference to a protected base class.
:errbad.
struct V { virtual void f(){}; };
struct A : protected virtual V { };
struct D : public virtual V, A { };

V *foo( A *p ) {
    // returns NULL if 'p' points to an 'A'
    // returns non-NULL if 'p' points to a 'D'
    return( dynamic_cast< V* >( p ) );
}
:eerrbad.

:MSGSYM. ERR_EXPLICIT_CAST_TYPE
:MSGTXT. type cannot be used with an explicit cast
:MSGJTXT. ���̌^�͖����I�L���X�g�ƂƂ��Ɏg�p�ł��܂���
The indicated type cannot be specified as the type of an explicit cast. For
example, it is illegal to cast to an array or function type.

:MSGSYM. ERR_CAST_TO_ARRAY
:MSGTXT. cannot cast to an array type
:MSGJTXT. �z��^�ւ̃L���X�g�͂ł��܂���
It is not permitted to cast to an array type.
:errbad.
typedef int array_type[5];
int array[5];
int* p = (array_type)array;
:eerrbad.

:MSGSYM. ERR_CAST_TO_FUNCTION
:MSGTXT. cannot cast to a function type
:MSGJTXT. �֐��^�ւ̃L���X�g�͂ł��܂���
It is not permitted to cast to a function type.
:errbad.
typedef int fun_type( void );
void* p = (fun_type)0;
:eerrbad.

:MSGSYM. ERR_TOO_MUCH_FOR_RTTI
:MSGTXT. implementation restriction: cannot generate RTTI info for '%T' (%d classes)
:MSGJTXT. ��������: '%T'�ɑ΂���RTTI���𐶐��ł��܂��� (%d �N���X)
The information for one class must fit into one segment.
If the segment size is restricted to 64k, the compiler may not be able
to emit the correct information properly if it requires more than 64k
of memory to represent the class hierarchy.

:MSGSYM. ERR_NO_UNIQUE_DEFAULT_CTOR
:MSGTXT. more than one default constructor for '%T'
:MSGJTXT. '%T'�ɑ΂���2�ȏ�̃f�t�H���g�E�R���X�g���N�^������܂�
The compiler found more than one default constructor signature
in the class definition.
There must be only one constructor declared that accepts no arguments.
:errbad.
struct C {
    C();
    C( int = 0 );
};
C cv;
:eerrbad.

:MSGSYM. ERR_UDC_AMBIGUOUS
:MSGTXT. user-defined conversion is ambiguous
:MSGJTXT. ���[�U��`�ϊ����B���ł�
The compiler found more than one user-defined conversion which could be
performed.  The indicated functions that could be used are shown.
:errbad.
struct T {
    T( S const& );
};
struct S {
    operator T const& ();
};
extern S sv;
T const & tref = sv;
:eerrbad.
Either the constructor or the conversion function could be used; consequently,
the conversion is ambiguous.
:MSGSYM. INF_UNSIGNED_TYPE_RANGE
:MSGTXT. range of possible values for type '%T' is %s to %s
:MSGJTXT. '%T'�^�ɑ΂��ĉ\�Ȓl�͈̔͂�%s����%s�܂łł�
:INFO.
This informational message indicates the range of values possible for the
indicated unsigned type.
:errbad.
unsigned char uc;
if( uc >= 0 );
:eerrbad.
Being unsigned, the char is always >= 0, so a warning will be issued.
Following the warning, this informational message indicates the possible range
of values for the unsigned type involved.

:MSGSYM. INF_SIGNED_TYPE_RANGE
:MSGTXT. range of possible values for type '%T' is %s to %s
:MSGJTXT. '%T'�^�ɑ΂��ĉ\�Ȓl�͈̔͂�%s����%s�܂łł�
:INFO.
This informational message indicates the range of values possible for the
indicated signed type.
:errbad.
signed char c;
if( c <= 127 );
:eerrbad.
Because the value of signed char is always <= 127, a warning will be issued.
Following the warning, this informational message indicates the possible range
of values for the signed type involved.

:MSGSYM. INF_SIGNED_CONST_EXPR_VALUE
:MSGTXT. constant expression in comparison has value %s
:MSGJTXT. ��r�̒��̒萔���́C�l%s�������܂�
:INFO.
This informational message indicates the value of the constant expression
involved in a comparison which caused a warning to be issued.
:errbad.
unsigned char uc;
if( uc >= 0 );
:eerrbad.
Being unsigned, the char is always >= 0, so a warning will be issued.
Following the warning, this informational message indicates the constant
value (0 in this case) involved in the comparison.

:MSGSYM. INF_UNSIGNED_CONST_EXPR_VALUE
:MSGTXT. constant expression in comparison has value %s
:MSGJTXT. ��r�̒��̒萔���́C�l%s�������܂�
:INFO.
This informational message indicates the value of the constant expression
involved in a comparison which caused a warning to be issued.
:errbad.
signed char c;
if( c <= 127 );
:eerrbad.
Because the value of char is always <= 127, a warning will be issued.
Following the warning, this informational message indicates the constant
value (127 in this case) involved in the comparison.

:MSGSYM. ERR_REF_CNV_ADDS_CONST
:MSGTXT. conversion of const reference to non-const reference
:MSGJTXT. const�Q�Ƃ����const�Q�Ƃւ̕ϊ��ł�
A reference to a constant object is being converted to a reference to
a non-constant object.
This can only be accomplished by using an explicit or
.id const_cast
cast.
:errbad.
extern int const & const_ref;
int & non_const_ref = const_ref;
:eerrbad.

:MSGSYM. ERR_REF_CNV_ADDS_VOLATILE
:MSGTXT. conversion of volatile reference to non-volatile reference
:MSGJTXT. volatile�Q�Ƃ����volatile�Q�Ƃւ̕ϊ��ł�
A reference to a volatile object is being converted to a reference to
a non-volatile object.
This can only be accomplished by using an explicit or
.id const_cast
cast.
:errbad.
extern int volatile & volatile_ref;
int & non_volatile_ref = volatile_ref;
:eerrbad.

:MSGSYM. ERR_REF_CNV_ADDS_BOTH
:MSGTXT. conversion of const volatile reference to plain reference
:MSGJTXT. const volatile�Q�Ƃ���C���q�Ȃ��̎Q�Ƃւ̕ϊ��ł�
A reference to a constant and volatile object is being converted to a reference to
a non-volatile and non-constant object.
This can only be accomplished by using an explicit or
.id const_cast
cast.
:errbad.
extern int const volatile & const_volatile_ref;
int & non_const_volatile_ref = const_volatile_ref;
:eerrbad.

:MSGSYM. INF_CURR_DECL_TYPE
:MSGTXT. current declaration has type '%T'
:MSGJTXT. ���݂̐錾��'%T'�^�ł�
:INFO.
This informational message indicates the type of the current declaration
that caused the message to be issued.
:errbad.
extern int __near foo( int );
extern int __far foo( int );
:eerrbad.

:MSGSYM. ERR_NOT_CONST_REF
:MSGTXT. only a non-volatile const reference can be bound to temporary
:MSGJTXT. ��volatile��const�Q�Ƃ݂̂��ꎞ�I�Ƀo�C���h�ł��܂�
The expression being bound to a reference will need to be converted to a
temporary of the type referenced.  This means that the reference will be
bound to that temporary and so the reference must be a non-volatile const
reference.
:errbad.
extern int * pi;
void * & r1 = pi;       // error
void * const & r2 = pi;     // ok
void * volatile & r3 = pi;  // error
void * const volatile & r4 = pi;// error
:eerrbad.

:MSGSYM. ANSI_MPTR_ACROSS_VIRTUAL
:MSGTXT. conversion of pointer to member across a virtual base
:MSGJTXT. ���z�x�[�X���܂����郁���o�[�ւ̃|�C���^�ł�
:ANSI. 1
In November 1995, the Draft Working Paper was amended to disallow pointer
to member conversions when the source class is a virtual base of the target
class.
This situation is treated as a warning (unless -za is specified to require
strict conformance), as a temporary measure.
In the future, an error will be diagnosed for this situation.
:errbad.
struct B {
    int b;
};

struct D : virtual B {
    int d;
};
int B::* mp_b = &B::b;
int D::* mp_d = mp_b;       // conversion across a virtual base
:eerrbad.

:MSGSYM. ERR_NAME_USED_BY_NAMESPACE
:MSGTXT. declaration cannot be in the same scope as namespace '%S'
:MSGJTXT. �錾�̓l�[���X�y�[�X'%S'�Ɠ����X�R�[�v�ɓ�����܂���
A namespace name must be unique across the entire C++ program.
Any other use of a name cannot be in the same scope as the namespace.
:errbad.
namespace x {
    int q;
};
int x;
:eerrbad.

:MSGSYM. ERR_NAME_USED_BY_NON_NAMESPACE
:MSGTXT. '%S' cannot be in the same scope as a namespace
:MSGJTXT. '%S'�̓l�[���X�y�[�X�Ɠ����X�R�[�v�ɓ�����܂���
A namespace name must be unique across the entire C++ program.
Any other use of a name cannot be in the same scope as the namespace.
:errbad.
int x;
namespace x {
    int q;
};
:eerrbad.

:MSGSYM. INF_FILE_LOCATION
:MSGTXT. File: %s
:MSGJTXT. �t�@�C��: %s
:INFO.
This informative message is written when the -ew switch is specified on a
command line.
It indicates the name of the file in which an error
or warning was detected.
The message precedes a group of one or more messages written for the file
in question.
Within each group, references within the file have the format
.id (line[,column]).

:MSGSYM. INF_ERR_LOCATION
:MSGTXT. %s
:MSGJTXT. %s
:INFO.
This informative message is written when the -ew switch is specified on a
command line.
It indicates the location of an error when the error was detected either
before or after the source file was read during the compilation process.

:MSGSYM. INF_SWITCH_LOCATION
:MSGTXT. %s: %s
:MSGJTXT. %s: %s
:INFO.
This informative message is written when the -ew switch is specified on a
command line.
It indicates the location of an error when the error was detected while
processing the switches specified in a command file or by the contents
of an environment variable.
The switch that was being processed is displayed following the name of the
file or the environment variable.

:MSGSYM. INF_CG_LOCATION
:MSGTXT. %s: %S
:MSGJTXT. %s: %S
:INFO.
This informative message is written when the -ew switch is specified on a
command line.
It indicates the location of an error when the error was detected while
generating a function, such as a constructor, destructor, or assignment
operator or while generating the machine instructions for a function
which has been analysed.
The name of the function is given following text indicating the context
from which the message originated.

:MSGSYM. INF_VFTABLE_OVERRIDE
:MSGTXT. possible override is '%S'
:MSGJTXT. �\�ȃI�[�o�[���C�h��'%S'�ł�
:INFO.
The indicated function is ambiguous since that name was defined in more than
one base class and one or more of these functions is virtual.
Consequently, it cannot be decided which is the virtual function to be used
in a class derived from these base classes.

:MSGSYM. INF_THUNK_TARGET
:MSGTXT. function being overridden is '%S'
:MSGJTXT. �I�[�o�[���C�h����Ă���֐���'%S'�ł�
:INFO.
This informational message indicates a function which cannot be overridden
by a virtual function which has ellipsis parameters.

:MSGSYM. ERR_NAME_DOESNT_REF_NAMESPACE
:MSGTXT. name does not reference a namespace
:MSGJTXT. ���O�̓l�[���X�y�[�X���Q�Ƃł��܂���
A
.kw namespace
alias definition must reference a
.kw namespace
definition.
:errbad.
typedef int T;
namespace a = T;
:eerrbad.

:MSGSYM. ERR_NAMESPACE_ALIAS_DIFFERENT
:MSGTXT. namespace alias cannot be changed
:MSGJTXT. �l�[���X�y�[�X��G�C���A�X�͕ύX�ł��܂���
A
.kw namespace
alias definition cannot change which
.kw namespace
it is referencing.
:errbad.
namespace ns1 { int x; }
namespace ns2 { int x; }
namespace a = ns1;
namespace a = ns2;
:eerrbad.

:MSGSYM. ERR_THROW_UNDEFED
:MSGTXT. cannot throw undefined class object
:MSGJTXT. ����`�N���X�I�u�W�F�N�g��throw�ł��܂���
C++ does not allow undefined classes to be copied and so an undefined
class object cannot be specified in a
.kw throw
expression.

:MSGSYM. WARN_DECL_NOT_SAME_TYPE
:MSGTXT. symbol has different type than previous symbol in same declaration
:MSGJTXT. �V���{���͓����錾�̒��̑O�̃V���{���ƈقȂ�^�������܂�
:WARNING. 4
This warning indicates that two symbols in the same declaration
have different types.  This may be intended but it is often
due to a misunderstanding of the C++ declaration syntax.
:errbad.
// change to:
//   char *p;
//   char q;
// or:
//   char *p, *q;
char* p, q;
:eerrbad.

:MSGSYM. INF_OTHER_DEFN
:MSGTXT. companion definition is '%S'
:MSGJTXT. ��r�̒�`��'%S'�ł�
:INFO.
This informational message indicates the other symbol
that shares a common base type in the same declaration.

:MSGSYM. ERR_DEF_ARG_REWRITE_ERROR
:MSGTXT. syntax error; default argument cannot be processed
:MSGJTXT. ���@�G���[; �f�t�H���g�����������ł��܂���
The default argument contains unbalanced braces or parenthesis.  The
default argument cannot be processed in this form.

:MSGSYM. INF_DEF_ARG_STARTED_HERE
:MSGTXT. default argument started %L
:MSGJTXT. �f�t�H���g�����̐擪�ł� %L
:INFO.
This informational message indicates where the default argument
started so that any problems with missing braces or parenthesis can
be fixed quickly and easily.
:errbad.
struct S {
    int f( int t= (4+(3-7), // missing parenthesis
    );
};
:eerrbad.

:MSGSYM. ANSI_NAME_CANT_BE_IN_NAMESPACE
:MSGTXT. '%N' cannot be declared in a namespace
:MSGJTXT. '%N'���l�[���X�y�[�X�̒��Ő錾�ł��܂���
:ANSI. 1
A
.kw namespace
cannot contain declarations or definitions of
.kw operator new
or
.kw operator delete
since they will never be called implicitly
in a
.kw new
or
.kw delete
expression.
:errbad.
namespace N {
    void *operator new( unsigned );
    void operator delete( void * );
};
:eerrbad.

:MSGSYM. ERR_NAMESPACE_MUST_BE_GLOBAL
:MSGTXT. namespace cannot be defined in a non-namespace scope
:MSGJTXT. �l�[���X�y�[�X���l�[���X�y�[�X��X�R�[�v�̒��Œ�`�ł��܂���
A
.kw namespace
can only be defined in either the
global namespace scope (file scope)
or a namespace scope.
:errbad.
struct S {
    namespace N {
    int x;
    };
}
:eerrbad.

:MSGSYM. ERR_CANNOT_USE_NAMESPACE_QUALIFIED_DECLARATOR
:MSGTXT. namespace '::' qualifier cannot be used in this context
:MSGJTXT. �l�[���X�y�[�X '::' �C���q�͂����ł͎g�p�ł��܂���
Qualified identifiers in a class context are allowed for declaring
.kw friend
functions.
A
.kw namespace
qualified name can only be declared in a namespace
scope that encloses the qualified name's namespace.
:errbad.
namespace M {
    namespace N {
    void f();
    void g();
    namespace O {
        void N::f() {
          // error
        }
    }
    }
    void N::g() {
      // OK
    }
}
:eerrbad.

:MSGSYM. ERR_CAST_AWAY_VOLATILE
:MSGTXT. cannot cast away volatility
:MSGJTXT. volatile���͂����L���X�g�͂ł��܂���
A cast or implicit conversion is illegal because a conversion to the target
type would remove volatility from a pointer, reference, or pointer to member.
:errbad.
struct S {
    int s;
};
extern S volatile * ps;
extern int volatile S::* mps;
S* ps1 = ps;
S& rs1 = *ps;
int S::* mp1 = mps;
:eerrbad.
The three initializations are illegal since they are attempts to remove
volatility.

:MSGSYM. ERR_CAST_AWAY_CONSTVOL
:MSGTXT. cannot cast away constness and volatility
:MSGJTXT. const��volatile���͂����L���X�g�͂ł��܂���
A cast or implicit conversion is illegal because a conversion to the target
type would remove constness and volatility from a pointer, reference, or
pointer to member.
:errbad.
struct S {
    int s;
};
extern S const volatile * ps;
extern int const volatile S::* mps;
S* ps1 = ps;
S& rs1 = *ps;
int S::* mp1 = mps;
:eerrbad.
The three initializations are illegal since they are attempts to remove
constness and volatility.

:MSGSYM. ERR_CAST_AWAY_UNALIGNED
:MSGTXT. cannot cast away unaligned
:MSGJTXT. unaligned���͂����L���X�g�͂ł��܂���
A cast or implicit conversion is illegal because a conversion to the target
type would add alignment to a pointer, reference, or pointer to member.
:errbad.
struct S {
    int s;
};
extern S _unaligned * ps;
extern int _unaligned S::* mps;
S* ps1 = ps;
S& rs1 = *ps;
int S::* mp1 = mps;
:eerrbad.
The three initializations are illegal since they are attempts to add
alignment.

:MSGSYM. ERR_INDEX_MUST_BE_INTEGRAL
:MSGTXT. subscript expression must be integral
:MSGJTXT. �Y�����͐����łȂ���΂Ȃ�܂���
Both of the operands of the indicated index expression are pointers.
There may be a missing indirection or function call.
:errbad.
int f();
int *p;
int g() {
    return p[f];
}
:eerrbad.

:MSGSYM. ANSI_EXTENDED_CONVERSION_UDC
:MSGTXT. extension: non-standard user-defined conversion
:MSGJTXT. �g��: ��W���̃��[�U�[��`�ϊ��ł�
:ANSI. 1
An extended conversion was allowed.
The latest draft of the C++ working paper does not allow a user-defined
conversion to be used in this context.
As an extension, the WATCOM compiler supports the conversion since substantial
legacy code would not compile without the extension.

:MSGSYM. WARN_USELESS_USING_DIRECTIVE
:MSGTXT. useless using directive ignored
:MSGJTXT. �Ӗ��̂Ȃ��[�����߂̎g�p�͖������܂�
:WARNING. 1
This warning indicates that for most purposes,
the
.kw using namespace
directive can be removed.
:errbad.
namespace A {
    using namespace A;  // useless
};
:eerrbad.

:MSGSYM. WARN_HIDDEN_VIRTUAL
:MSGTXT. base class virtual function has not been overridden
:MSGJTXT. �x�[�X�N���X�̉��z�֐��̓I�[�o�[���C�h����܂���ł���
:WARNING. 4
This warning indicates that a virtual function name has been
overridden but in an incomplete manner, namely,
a virtual function signature has been omitted in the
overriding class.
:errbad.
struct B {
    virtual void f() const;
};
struct D : B {
    virtual void f();
};
:eerrbad.

:MSGSYM. INF_BASE_VFN
:MSGTXT. virtual function is '%S'
:MSGJTXT. ���z�֐���'%S'�ł�
:INFO.
This message indicates which virtual function has not
been overridden.

:MSGSYM. INF_MACRO_DECLARATION
:MSGTXT. macro '%s' defined %L
:MSGJTXT. �}�N��'%s'�͒�`����Ă��܂� %L
:INFO.
This informational message indicates where the macro in question was defined.
The message is displayed following an error or warning diagnostic for the
macro in question.
:errbad.
#define mac(a,b,c) a+b+c

int i = mac(6,7,8,9,10);
:eerrbad.
The expansion of macro
.id mac
is erroneous because it contains too many arguments.
The informational message will indicate where the macro was defined.

:MSGSYM. INF_MACRO_EXPANSION
:MSGTXT. expanding macro '%s' defined %L
:MSGJTXT. �W�J���Ă���}�N��'%s'�͒�`����Ă��܂� %L
:INFO.
These informational messages indicate the macros that are currently being
expanded, along with the location at which they were defined.
The message(s) are displayed following a diagnostic which is issued during
macro expansion.

:MSGSYM. ERR_COMMON_CL_IMPOSSIBLE
:MSGTXT. conversion to common class type is impossible
:MSGJTXT. ���ʂ̃N���X�^�ւ̕ϊ��͂ł��܂���
The conversion to a common class is impossible.
One or more of the left and right operands are class types.
The informational messages indicate these types.
:errbad.
class A { A(); };
class B { B(); };
extern A a;
extern B b;
int i = ( a == b );
:eerrbad.
The last statement is erroneous since a conversion to a common class type is
impossible.

:MSGSYM. ERR_COMMON_CL_AMBIGUOUS
:MSGTXT. conversion to common class type is ambiguous
:MSGJTXT. ���ʂ̃N���X�^�ւ̕ϊ��͞B���ł�
The conversion to a common class is ambiguous.
One or more of the left and right operands are class types.
The informational messages indicate these types.
:errbad.
class A { A(); };
class B : public A { B(); };
class C : public A { C(); };
class D : public B, public C { D(); };
extern A a;
extern D d;
int i = ( a == d );
:eerrbad.
The last statement is erroneous since a conversion to a common class type is
ambiguous.

:MSGSYM. ERR_COMMON_CL_PRIVATE
:MSGTXT. conversion to common class type requires private access
:MSGJTXT. ���ʂ̃N���X�^�ւ̕ϊ��ɂ̓v���C�x�[�g�����ւ̃A�N�Z�X���K�v�ł�
The conversion to a common class violates the access permission which was
private.
One or more of the left and right operands are class types.
The informational messages indicate these types.
:errbad.
class A { A(); };
class B : private A { B(); };
extern A a;
extern B b;
int i = ( a == b );
:eerrbad.
The last statement is erroneous since a conversion to a common class type
violates the (private) access permission.

:MSGSYM. ERR_COMMON_CL_PROTECTED
:MSGTXT. conversion to common class type requires protected access
:MSGJTXT. ���ʂ̃N���X�^�ւ̕ϊ��ɂ̓v���e�N�g�����ւ̃A�N�Z�X���K�v�ł�
The conversion to a common class violates the access permission which was
protected.
One or more of the left and right operands are class types.
The informational messages indicate these types.
:errbad.
class A { A(); };
class B : protected A { B(); };
extern A a;
extern B b;
int i = ( a == b );
:eerrbad.
The last statement is erroneous since a conversion to a common class type
violates the (protected) access permission.

:MSGSYM. ERR_AMBIGUOUS_NAMESPACE_LOOKUP
:MSGTXT. namespace lookup is ambiguous
:MSGJTXT. �l�[���X�R�[�v����b�N�A�b�v���B���ł�
A lookup for a name resulted in two or more non-function
names being found.  This is not allowed according to the
C++ working paper.
:errbad.
namespace M {
    int i;
}
namespace N {
    int i;
    using namespace M;
}
void f() {
    using namespace N;
    i = 7;  // error
}
:eerrbad.

:MSGSYM. INF_AMBIGUOUS_NAMESPACE_SYM
:MSGTXT. ambiguous namespace symbol is '%S'
:MSGJTXT. �B���ȃl�[���X�R�[�v��V���{����'%S'�ł�
:INFO.
This informational message shows a symbol that conflicted with another
symbol during a lookup.

:MSGSYM. ERR_STATIC_CAST_ACROSS_PRIVATE
:MSGTXT. attempt to static_cast from a private base class
:MSGJTXT. �v���C�x�[�g��x�[�X�N���X����static_cast���悤�Ƃ��Ă��܂�
An attempt was made to static_cast a pointer or reference to a private base
class to a derived class.
:errbad.
struct PrivateBase {
};

struct Derived : private PrivateBase {
};

extern PrivateBase* pb;
extern PrivateBase& rb;
Derived* pd = static_cast<Derived*>( pb );
Derived& rd = static_cast<Derived&>( rb );
:eerrbad.
The last two statements are erroneous since they would involve a
.kw static_cast
from a private base class.

:MSGSYM. ERR_STATIC_CAST_ACROSS_PROTECTED
:MSGTXT. attempt to static_cast from a protected base class
:MSGJTXT. �v���e�N�g��x�[�X�N���X����static_cast���悤�Ƃ��Ă��܂�
An attempt was made to static_cast a pointer or reference to a protected base
class to a derived class.
:errbad.
struct ProtectedBase {
};

struct Derived : protected ProtectedBase {
};

extern ProtectedBase* pb;
extern ProtectedBase& rb;
Derived* pd = static_cast<Derived*>( pb );
Derived& rd = static_cast<Derived&>( rb );
:eerrbad.
The last two statements are erroneous since they would involve a
.kw static_cast
from a protected base class.

:MSGSYM. ERR_CURRSCOPE_DOESNT_ENCLOSE
:MSGTXT. qualified symbol cannot be defined in this scope
:MSGJTXT. �C�����ꂽ�V���{���͂��̃X�R�[�v�̒��Œ�`�ł��܂���
This message indicates that the scope of the symbol is not
nested in the current scope.  This is a restriction in the
C++ language.
:errbad.
namespace A {
    struct S {
    void ok();
    void bad();
    };
    void ok();
    void bad();
};
void A::S::ok() {
}
void A::ok() {
}
namespace B {
    void A::S::bad() {
    // error!
    }
    void A::bad() {
    // error!
    }
};
:eerrbad.

:MSGSYM. ERR_MEMBER_USING_DECL_REFS_NON_MEMBER
:MSGTXT. using declaration references non-member
:MSGJTXT. �g�p���Ă���錾�͔񃁃��o�[���Q�Ƃ��Ă��܂�
This message indicates that the entity referenced
by the
.kw using
declaration is not a class member
even though the
.kw using
declaration is in class scope.
:errbad.
namespace B {
    int x;
};
struct D {
    using B::x;
};
:eerrbad.

:MSGSYM. ERR_USING_DECL_REFS_MEMBER
:MSGTXT. using declaration references class member
:MSGJTXT. �g�p���Ă���錾�̓N���X������o�[���Q�Ƃ��Ă��܂�
This message indicates that the entity referenced
by the
.kw using
declaration is a class member
even though the
.kw using
declaration is not in class scope.
:errbad.
struct B {
    int m;
};
using B::m;
:eerrbad.

:MSGSYM. ERR_INVALID_CONSTANT_SUFFIX
:MSGTXT. invalid suffix for a constant
:MSGJTXT. �萔�ɕs�K�؂Ȑڔ��������Ă��܂�
An invalid suffix was coded for a constant.
:errbad.
__int64 a[] = {
    0i7, // error
    0i8,
    0i15, // error
    0i16,
    0i31, // error
    0i32,
    0i63, // error
    0i64,
};
:eerrbad.

:MSGSYM. ERR_USING_DECL_NOT_A_BASE_CLASS
:MSGTXT. class in using declaration ('%T') must be a base class
:MSGJTXT. �g�p���Ă���錾�̒��̃N���X('%T')�̓x�[�X�N���X�łȂ���΂Ȃ�܂���
A
.kw using
declaration declared in a class scope
can only reference entities in a base class.
:errbad.
struct B {
    int f;
};
struct C {
    int g;
};
struct D : private C {
    B::f;
};
:eerrbad.

:MSGSYM. ERR_USING_DECL_NAME_SAME
:MSGTXT. name in using declaration is already in scope
:MSGJTXT. �g�p���Ă���錾�̒��̖��O�͊��ɃX�R�[�v�̒��ɂ���܂�
A
.kw using
declaration can only reference entities in other scopes.
It cannot reference entities within its own scope.
:errbad.
namespace B {
    int f;
    using B::f;
};
:eerrbad.

:MSGSYM. ERR_PREV_USING_DECL
:MSGTXT. conflict with a previous using-decl '%S'
:MSGJTXT. �O�Ɏg�p���Ă���錾'%S'�ƏՓ˂��Ă��܂�
A
.kw using
declaration can only reference entities in other scopes.
It cannot reference entities within its own scope.
:errbad.
namespace B {
    int f;
    using B::f;
};
:eerrbad.

:MSGSYM. ERR_CURR_USING_DECL
:MSGTXT. conflict with current using-decl '%S'
:MSGJTXT. ���ݎg�p���Ă���錾'%S'�ƏՓ˂��Ă��܂�
A
.kw using
declaration can only reference entities in other scopes.
It cannot reference entities within its own scope.
:errbad.
namespace B {
    int f;
    using B::f;
};
:eerrbad.

:MSGSYM. WARN_MUST_BE_MULTITHREADED
:MSGTXT. use of '%N' requires build target to be multi-threaded
:MSGJTXT. '%N'���g�p����ɂ́C�}���`��X���b�h�̃^�[�Q�b�g�Ƃ��č쐬���Ȃ���΂Ȃ�܂���
:WARNING. 1
The compiler has detected a use of a run-time function that
will create a new thread but the current build target indicates
only single-threaded C++ source code is expected.
Depending on the user's environment, enabling multi-threaded
applications can involve using the "-bm" option or
selecting multi-threaded applications through a dialogue.

:MSGSYM. ERR_64BIT_SWITCH
:MSGTXT. implementation restriction: cannot use 64-bit value in switch statement
:MSGJTXT. ��������: switch���̒���64�r�b�g�̒l���g�p���邱�Ƃ͂ł��܂���
The use of 64-bit values in switch statements has not been implemented.

:MSGSYM. ERR_64BIT_CASE
:MSGTXT. implementation restriction: cannot use 64-bit value in case statement
:MSGJTXT. ��������: case���̒���64�r�b�g�̒l���g�p���邱�Ƃ͂ł��܂���
The use of 64-bit values in case statements has not been implemented.

:MSGSYM. ERR_64BIT_BITFIELD
:MSGTXT. implementation restriction: cannot use __int64 as bit-field base type
:MSGJTXT. ��������: __int64�̓r�b�g�t�B�[���h�̃x�[�X�^�Ƃ��Ďg�p�ł��܂���
The use of
.kw __int64
for the base type of a bit-field has not been implemented.

:MSGSYM. ERR_CODE_IN_NONCODE_SEG
:MSGTXT. based function object cannot be placed in non-code segment "%s".
:MSGJTXT. 'based'�֐��I�u�W�F�N�g���R�[�h��Z�O�����g"%s"�ɒu�����Ƃ͂ł��܂���
Use
.kw __segname
with the default code segment "_CODE", or a code segment with the appropriate
suffix (indicated by informational message).
:errbad.
int __based(__segname("foo")) f() {return 1;}
:eerrbad.
:errgood.
int __based(__segname("_CODE")) f() {return 1;}
:eerrgood.

:MSGSYM. INF_CODE_SEGMENT_SUFFIX
:MSGTXT. Use a segment name ending in "%s", or the default code segment "_CODE".
:MSGJTXT. "%s"�ŏI���Z�O�����g�����g�p���邩�C�f�t�H���g��R�[�h�Z�O�����g"_CODE"���g�p���܂�
:INFO.
This informational message explains how to use
.kw __segname
to name a code segment.

:MSGSYM. ERR_RTTI_DISABLED
:MSGTXT. RTTI must be enabled to use feature (use 'xr' option)
:MSGJTXT. RTTI���g���ɂ́C���̋@�\���g�p�ł���悤�ɂ��Ȃ���΂Ȃ�܂���('xr'�I�v�V�������g�p���Ă�������)
RTTI must be enabled by specifying the 'xr' option when the compiler is
invoked.  The error message indicates that a feature such as
.kw dynamic_cast,
or
.kw typeid
has been used without enabling RTTI.

:MSGSYM. ERR_TYPEID_CLASS_MUST_BE_DEFINED
:MSGTXT. 'typeid' class type must be defined
:MSGJTXT. 'typeid'�N���X�^�͒�`���Ȃ���΂Ȃ�܂���
The compile-time type of the expression or type must be
completely defined if it is a class type.
:errbad.
struct S;
void foo( S *p ) {
    typeid( *p );
    typeid( S );
}
:eerrbad.

:MSGSYM. WARN_REINT_FOR_EXPLICIT_MPTR_UNRELATED
:MSGTXT. cast involves unrelated member pointers
:MSGJTXT. �L���X�g�ɖ��֌W�ȃ����o�[�|�C���^���܂܂�Ă��܂�
:WARNING. 4
This warning is issued to indicate that a dangerous cast of a member pointer
has been used.
This occurs when there is an explicit cast between sufficiently unrelated
types of member pointers that the cast must be implemented using a
reinterpret_cast.
These casts were illegal, but became legal when the new-style casts were
added to the draft working paper.
:errbad.
struct C1 {
    int foo();
};
struct D1 {
    int poo();
};

typedef int (C1::* C1mp )();

C1mp fmp = (C1mp)&D1::poo;
:eerrbad.
The cast on the last line of the example would be diagnosed.

:MSGSYM. ERR_UNEXPECTED_DECLSPEC_MOD
:MSGTXT. unexpected type modifier found
:MSGJTXT. �T�|�[�g���Ă��Ȃ��^�C���q������܂�
A
.kw __declspec
modifier was found that could not be applied to an object
or could not be used in this context.
:errbad.
__declspec(thread) struct S {
};
:eerrbad.

:MSGSYM. ERR_INVALID_BITFIELD_ID
:MSGTXT. invalid bit-field name '%N'
:MSGJTXT. �s�K�؂ȃr�b�g�t�B�[���h��'%N'�ł�
A bit-field can only have a simple identifier
as its name.  A qualified name is also not
allowed for a bit-field.
:errbad.
struct S {
    int operator + : 1;
};
:eerrbad.

:MSGSYM. WARN_PADDING_ADDED
:MSGTXT. %u padding byte(s) added
:MSGJTXT. %u�o�C�g�̃p�f�B���O(���ߍ���)���ǉ�����܂���
:WARNING. 1
This warning indicates that some extra bytes
have been added to a class in order to align
member data to its natural alignment.
:errbad.
#pragma pack(push,8)
struct S {
    char c;
    double d;
};
#pragma pack(pop);
:eerrbad.

:MSGSYM. INF_HIDDEN_WHY
:MSGTXT. cannot be called with a '%T *'
:MSGJTXT. '%T *'�ŌĂяo�����Ƃ͂ł��܂���
:INFO.
This message indicates that the virtual function cannot be called
with a pointer or reference to the current class.

:MSGSYM. WARN_REINT_FOR_EXPLICIT_MPTR_UNDEFD
:MSGTXT. cast involves an undefined member pointer
:MSGJTXT. �L���X�g�ɖ���`�̃����o�[�|�C���^���܂܂�Ă��܂�
:WARNING. 1
This warning is issued to indicate that a dangerous cast of a member pointer
has been used.
This occurs when there is an explicit cast between sufficiently unrelated
types of member pointers that the cast must be implemented using a
reinterpret_cast.
In this case, the host class of at least one member pointer was
not a fully defined class and, as such, it is unknown whether
the host classes are related through derivation.
These casts were illegal, but became legal when the new-style casts were
added to the draft working paper.
:errbad.
struct C1 {
    int foo();
};
struct D1;

typedef int (C1::* C1mp )();
typedef int (D1::* D1mp )();

C1mp fn( D1mp x ) {
    return (C1mp) x;
}
// D1 may derive from C1
:eerrbad.
The cast on the last line of the example would be diagnosed.

:MSGSYM. WARN_REINT_FOR_EXPLICIT_MPTR_DERIVED
:MSGTXT. cast changes both member pointer object and class type
:MSGJTXT. �L���X�g�̓����o�[�|�C���^��I�u�W�F�N�g�ƃN���X�^�̗�����ς��Ă��܂�
:WARNING. 1
This warning is issued to indicate that a dangerous cast of a member pointer
has been used.
This occurs when there is an explicit cast between sufficiently unrelated
types of member pointers that the cast must be implemented using a
reinterpret_cast.
In this case, the host classes of the member pointers are related
through derivation and the object type is also being changed.
The cast can be broken up into two casts, one that changes the
host class without changing the object type, and another that
changes the object type without changing the host class.
:errbad.
struct C1 {
    int fn1();
};
struct D1 : C1 {
    int fn2();
};

typedef int (C1::* C1mp )();
typedef void (D1::* D1mp )();

C1mp fn( D1mp x ) {
    return (C1mp) x;
}
:eerrbad.
The cast on the last line of the example would be diagnosed.

:MSGSYM. ERR_OVERRIDE_CHANGES_CONVENTION
:MSGTXT. virtual function '%S' has a different calling convention
:MSGJTXT. ���z�֐�'%S'�ɈقȂ�Ăяo���K�񂪐錾����Ă��܂�
This error indicates that the calling conventions specified
in the virtual function prototypes are different.
This means that virtual function calls will not
function properly since the caller and callee
may not agree on how parameters should be passed.
Correct the problem by deciding on one calling
convention and change the erroneous declaration.
:errbad.
struct B {
    virtual void __cdecl foo( int, int );
};
struct D : B {
    void foo( int, int );
};
:eerrbad.

:MSGSYM. WARN_WEIRD_ENDIF_ENCOUNTER
:MSGTXT. #endif matches #if in different source file
:MSGJTXT. #endif�ɑΉ�����#if���قȂ�\�[�X�t�@�C���ɂ���܂�
:WARNING. 1
This warning may indicate a
.kw #endif
nesting problem since the traditional usage of
.kw #if
directives is confined to the same source file.
This warning may often come before an error
and it is hoped will provide information to
solve a preprocessing directive problem.

:MSGSYM. INF_PP_DIRECTIVE_LOCN
:MSGTXT. preprocessing directive found %L
:MSGJTXT. �O�����[�����߂�%L�s�ɂ���܂�
:INFO.
This informational message indicates the location of a preprocessing
directive associated with the error or warning message.

:MSGSYM. WARN_NEGATE_UNSIGNED
:MSGTXT. unary '-' of unsigned operand produces unsigned result
:MSGJTXT. �����Ȃ��I�y�����h�ɒP�����Z�q��'-'��t���Ă����ʂ͕����Ȃ��ɂȂ�܂�
:WARNING. 3
When a unary minus ('-') operator is applied
to an unsigned operand, the result has an unsigned
type rather than a signed type.
This warning often occurs because of the misconception
that '-' is part of a numeric token rather than as
a unary operator.  The work-around for the warning
is to cast the unary minus operand to the appropriate
signed type.
:errbad.
extern void u( int );
extern void u( unsigned );
void fn( unsigned x ) {
    u( -x );
    u( -2147483648 );
}
:eerrbad.

:MSGSYM. WARN_EXPANDED_TRIGRAPH
:MSGTXT. trigraph expansion produced '%c'
:MSGJTXT. 3�����\�L(�g���C�O���t)��W�J���ĕ���'%c'�ɂȂ�܂���
:WARNING. 1
Trigraph expansion occurs at a very low-level
so it can affect string literals that contain
question marks.  This warning can be disabled
via the command line or
.kw #pragma warning
directive.
:errbad.
// string expands to "(?]?~????"!
char *e = "(???)???-????";
// possible work-arounds
char *f = "(" "???" ")" "???" "-" "????";
char *g = "(\?\?\?)\?\?\?-\?\?\?\?";
:eerrbad.

:MSGSYM. ANSI_INVALID_HEX_ESCAPE
:MSGTXT. hexadecimal escape sequence out of range; truncated
:MSGJTXT. 16�i���̃G�X�P�[�v�V�[�P���X���͈͊O�ł��G�؂�l�߂��܂���
:ANSI. 1
This message indicates that the hexadecimal escape sequence
produces an integer that cannot fit into the required character type.
:errbad.
char *p = "\x0aCache Timings\x0a";
:eerrbad.

:MSGSYM. WARN_UNDEFD_MACRO_IS_ZERO
:MSGTXT. undefined macro '%s' evaluates to 0
:MSGJTXT. ����`�}�N�� '%s' ��0�Ƃ݂Ȃ��܂�
:WARNING. 10
The ISO C/C++ standard requires that undefined
macros evaluate to zero during preprocessor
expression evaluation.
This default behaviour can often mask incorrectly
spelled macro references.
The warning is useful when used in critical
environments where all macros will be defined.
:errbad.
#if _PRODUCTI0N // should be _PRODUCTION
#endif
:eerrbad.

:MSGSYM. WARN_CHAR_VALUE_LARGE
:MSGTXT. char constant has value %u (more than 8 bits)
:MSGJTXT. char�萔�̒l�� %u (8�r�b�g���傫��)�ł��B
:WARNING. 9
The ISO C/C++ standard requires that multi-char character
constants be accepted with an implementation defined
value.
This default behaviour can often mask incorrectly
specified character constants.
:errbad.
int x = '\0x1a'; // warning
int y = '\x1a';
:eerrbad.

:MSGSYM. WARN_CHAR_PROMOTION
:MSGTXT. promotion of unadorned char type to int
:MSGJTXT. ���������w���char�^����int�^�ւ̕ϊ�������܂�
:WARNING. 1
This message is enabled by the hidden -jw option.
The warning may be used to locate all places where an
unadorned char type (i.e., a type that is specified as
.kw char
and neither
.kw signed char
nor
.kw unsigned char
).
This may cause portability problems since compilers have
freedom to specify whether the unadorned char type is to
be signed or unsigned.  The promotion to
.kw int
will have different
values, depending on the choice being made.

:MSGSYM. WARN_SWITCH_NO_CASE_LABELS
:MSGTXT. switch statement has no case labels
:MSGJTXT. switch���̒���case���x��������܂���
:WARNING. 4
The switch
statement referenced in the warning did not have
any case labels.
Without case labels, a switch statement will always
jump to the default case code.
:errbad.
void fn( int x )
{
    switch( x ) {
    default:
    ++x;
    }
}
:eerrbad.

:MSGSYM. WARN_WEIRD_CHARACTER
:MSGTXT. unexpected character (%u) in source file
:MSGJTXT. �\�[�X�t�@�C���ɗ\������Ȃ�����(%u)������܂�
:WARNING. 1
The compiler has encountered a character in the
source file that is not in the allowable set of
input characters.  The decimal representation of the
character byte is output for diagnostic purposes.
:errbad.
// invalid char '\0'
:eerrbad.

:MSGSYM. WARN_WHITE_AFTER_SPLICE
:MSGTXT. ignoring whitespace after line splice
:MSGJTXT. �s�����̌�̋󔒂͖�������܂�
:WARNING. 10
The compiler is ignoring some whitespace characters
that occur after the line splice.  This warning
is useful when the source code must be compiled
with other compilers that do not allow this extension.
:errbad.
#define XXXX int \
x;

XXXX
:eerrbad.

:MSGSYM. WARN_EMPTY_MEMBER_DECL
:MSGTXT. empty member declaration
:MSGJTXT. �����o�錾�̓��e����ł�
:WARNING. 10
The compiler is warning about an extra semicolon
found in a class definition.  The extra semicolon
is valid C++ but some C++ compilers do not accept
this as valid syntax.
:errbad.
struct S { ; };
:eerrbad.

:MSGSYM. WARN_NON_PORTABLE_DECL_ZERO_SIZED_ARRAY
:MSGTXT. '%S' makes use of a non-portable feature (zero-sized array)
:MSGJTXT. '%S'�͈ڐA���̂Ȃ��@�\�𗘗p���Ă��܂�(�T�C�Y���[���̔z��)
:WARNING. 10
The compiler is warning about the use of a non-portable feature
in a declaration or definition.
This warning is available for
environments where diagnosing the use of non-portable features is useful
in improving the portability of the code.
:errbad.
struct D {
    int d;
    char a[];
};
:eerrbad.

:MSGSYM. ERR_MUST_BE_CONST_STATIC_INTEGRAL
:MSGTXT. in-class initialization is only allowed for const static integral members
:MSGJTXT. �N���X��`���ɏ������ł���̂́Cstatic const�̐����^�����o�[�ϐ��Ɍ����܂�
:errbad.
struct A {
    static int i = 0;
};
:eerrbad.

:MSGSYM. ERR_IMPLICIT_CAST_ILLEGAL
:MSGTXT. cannot convert expression to target type
:MSGJTXT. �w�肵���^�ɕϊ����邱�Ƃ��ł��܂���
The implicit cast is trying to convert an expression to a completely
unrelated type.  There is no way the compiler can provide any meaning
for the intended cast.
:errbad.
struct T {
};

void fn()
{
    bool b = T;
}
:eerrbad.

:MSGSYM. ERR_UNKNOWN_TEMPLATE_SPECIALIZATION
:MSGTXT. unknown template specialization of '%S'
:MSGJTXT. '%S'�̃e���v���[�g���ꉻ�͖��m�ł�
:errbad.
template<class T>
struct A { };

template<class T>
void A<T *>::f() {
}
:eerrbad.

:MSGSYM. ERR_WRONG_NR_TEMPLATE_ARGUMENTS
:MSGTXT. wrong number of template arguments for '%S'
:MSGJTXT. '%S'�ɑ΂���e���v���[�g�����̐����Ԉ���Ă��܂�
:errbad.
template<class T>
struct A { };

template<class T, class U>
struct A<T, U> { };
}
:eerrbad.

:MSGSYM. ERR_CANNOT_EXPLICITLY_SPECIALIZE_MEMBER
:MSGTXT. cannot explicitly specialize member of '%S'
:MSGJTXT. �����o�['%S'�𖾎��I�ɓ��ꉻ���邱�Ƃ��ł��܂���
:errbad.
template<class T>
struct A { };

template<>
struct A<int> {
    void f();
};

template<>
void A<int>::f() {
}
:eerrbad.

:MSGSYM. ERR_TEMPLATE_SPECIALIZATION_MATCHES_PRIMARY
:MSGTXT. specialization arguments for '%S' match primary template
:MSGJTXT.
:errbad.
template<class T>
struct A { };

template<class T>
struct A<T> { };
:eerrbad.

:MSGSYM. ERR_TEMPLATE_SPECIALIZATION_AMBIGUOUS
:MSGTXT. partial template specialization for '%S' ambiguous
:MSGJTXT. '%S'�ɑ΂��镔���I�e���v���[�g���ꉻ���s���m�ł�
:errbad.
template<class T, class U>
struct A { };

template<class T, class U>
struct A<T *, U> { };

template<class T, class U>
struct A<T, U *> { };

A<int *, int *> a;
:eerrbad.

:MSGSYM. ERR_STATIC_ASSERTION_FAILURE
:MSGTXT. static assertion failed '%s'
:MSGJTXT. �ÓI�A�T�[�g'%s'�����s���܂���
:errbad.
static_assert( false, "false" );
:eerrbad.

:MSGSYM. WARN_UNSUPPORTED_TEMPLATE_EXPORT
:MSGTXT. Exported templates are not supported by Open Watcom C++
:MSGJTXT. �G�N�X�|�[�g���ꂽ�e���v���[�g��Open Watcom C++�ł̓T�|�[�g����Ă��܂���
:WARNING. 1
:errbad.
export template< class T >
struct A {
};
:eerrbad.

:MSGSYM. ERR_CANNOT_REDECLARE_MEMBER_FUNCTION
:MSGTXT. redeclaration of member function '%S' not allowed
:MSGJTXT. �����o�[�֐�'%S'�̍Đ錾�͔F�߂��܂���
:errbad.
struct A {
    void f();
    void f();
};
:eerrbad.

:MSGSYM. INF_CANDIATE_DEFINITION
:MSGTXT. candidate defined %L
:MSGJTXT. candidate��%L���`���܂���
:INFO.

:MSGSYM. ERR_BAD_REGISTER_NAME
:MSGTXT. Invalid register name '%s' in #pragma
:MSGJTXT. #pragma�̒��ɖ����ȃ��W�X�^��'%s'������܂�
The register name is invalid/unknown.

:MSGSYM. WARN_MISSING_KEYWORD_IN_EXPLICT_INSTANTIATION
:MSGTXT. Archaic syntax: class/struct missing in explicit template instantiation
:MSGJTXT. �����ȍ\���Fexplicit�ȃe���v���[�g�̃C���X�^���X����class/struct������܂���
:WARNING. 1
Archaic syntax has been used.  The standard requires a 
.kw class
or
.kw struct
keyword to be used.
:errbad.
template< class T >
class MyTemplate { };

template MyTemplate< int >;
:eerrbad.
:errgood.
template class MyTemplate< int >;
:eerrgood.

:MSGSYM. ERR_DTOR_TYPE_VOID
:MSGTXT. destructor for type void cannot be called
:MSGJTXT.
Since the
.kw void
type has no size and there are no values of
.kw void
type, one cannot destruct an instance of
.kw void.

:MSGSYM. ERR_TYPENAME_OUTSIDE_TEMPLATE
:MSGTXT. 'typename' keyword used outside template
:MSGJTXT.
The
.kw typename
keyword is only allowed inside templates.

:MSGSYM. ERR_FUNCTION_BAD_RETURN
:MSGTXT. '%N' does not have a return type specified
:MSGJTXT.
In C++, functions must have an explicit return type specified,
default int type is no longer assumed.
:errbad.
f ();
:eerrbad.

:MSGSYM. ANSI_MAIN_MUST_RETURN_INT
:MSGTXT. 'main' must return 'int'
:MSGJTXT.
:ANSI. 1
The "main" function shall have a return type of type int.
:errbad.
void main()
{ }
:eerrbad.

:MSGSYM. ERR_OUT_OF_CLASS_EXPLICIT
:MSGTXT. explicit may only be used within class definition
:MSGJTXT.
The explicit specifier shall be used only in the declaration of a
constructor within its class definition.
:errbad.
struct A {
    explicit A();
};

explicit A::A()
{ }
:eerrbad.

:MSGSYM. ERR_OUT_OF_CLASS_VIRTUAL
:MSGTXT. virtual may only be used within class definition
:MSGJTXT.
The virtual specifier shall be used only in the initial declaration of a
class member function.
:errbad.
struct A {
    virtual void f();
};

virtual void A::f()
{ }
:eerrbad.

:MSGSYM. ERR_DEFAULT_TEMPLATE_ARG_REDEFINED
:MSGTXT. cannot redefine default template argument '%N'
:MSGJTXT.
A template-parameter shall not be given default arguments by two
different declarations in the same scope.
:errbad.
template< class T = int >
class X;

template< class T = int >
class X {
};
:eerrbad.

:MSGSYM. ERR_DEFAULT_ARG_IN_PARTIAL_SPEC
:MSGTXT. cannot have default template arguments in partial specializations
:MSGJTXT.
A partial specialization cannot have default template arguments.
:errbad.
template< class T >
class X {
};

template< class T = int >
class X< T * > {
};
:eerrbad.

:MSGSYM. WARN_DLT_PTR_TO_VOID
:MSGTXT. delete of a pointer to void
:MSGJTXT.
:WARNING. 1
If the dynamic type of the object to be deleted differs from its
static type, the behavior is undefined. This implies that an object
cannot be deleted using a pointer of type void* because there are no
objects of type void.
:errbad.
void fn( void *p, void *q ) {
    delete p;
    delete [] q;
}
:eerrbad.

:MSGSYM. WARN_LONG_CHAR_DEPRECATED
:MSGTXT. 'long char' is deprecated, use wchar_t instead
:MSGJTXT.
:WARNING. 1
The standard C++ 'wchar_t' type specifier should be used instead of the Open
Watcom specific 'long char' type specifier.
:errbad.
void fn( ) {
    long char c;
}
:eerrbad.

:MSGSYM. ERR_NAMESPACE_NOT_ALLOWED_IN_USING_DECL
:MSGTXT. namespace '%I' not allowed in using-declaration
:MSGJTXT.
Specifying a namespace-name is not allowed in a using-declaration, a
using-directive must be used instead.
:errbad.
namespace ns { }
using ns;
:eerrbad.

:MSGSYM. INF_CANDIDATE_TEMPLATE_DEFINITION
:MSGTXT. candidate %C defined %L
:MSGJTXT.
:INFO.

:MSGSYM. ERR_QUALIFIED_NAME_NOT_CLASS
:MSGTXT. qualified name '%I' does not name a class
:MSGJTXT.
:errbad.
namespace ns {
}
struct ns::A {
};
:eerrbad.

:MSGSYM. ERR_EXPECTED_CLASS_TYPE
:MSGTXT. expected class type, but got '%T'
:MSGJTXT.
:errbad.
template< class T >
struct A : public T {
};

A< int > a;
:eerrbad.

:MSGSYM. ERR_SYNTAX_SCOPED_ID
:MSGTXT. syntax error near '%s'; probable cause: incorrectly spelled type name
:MSGJTXT.
The identifier in the error message has not been declared as a type name
in any scope at this point in the code.  This may be the cause of the
syntax error.

:MSGSYM. ERR_SYNTAX_UNDECLARED_SCOPED_ID
:MSGTXT. syntax error: '%s' has not been declared as a member
:MSGJTXT.
The identifier in the error message has not been declared as member.  This
may be the cause of the syntax error.
:errbad.
struct A { };

void fn() {
    A::undeclared = 0;
}
:eerrbad.

:MSGSYM. ERR_SYNTAX_UNDECLARED_GLOBAL_ID
:MSGTXT. syntax error: '%s' has not been declared
:MSGJTXT.
The identifier in the error message has not been declared.  This may be the
cause of the syntax error.
:errbad.
void fn() {
    ::undeclared = 0;
}
:eerrbad.

:MSGSYM. ERR_SYNTAX_UNEXPECTED_ID
:MSGTXT. syntax error: identifier '%s', but expected: '%s'
:MSGJTXT.

:MSGSYM. ERR_SYNTAX_UNEXPECTED_TOKEN
:MSGTXT. syntax error: token '%s', but expected: '%s'
:MSGJTXT.

:MSGSYM. ERR_MEMBER_SAME_NAME_AS_CLASS
:MSGTXT. member '%S' cannot be declared in this class
:MSGJTXT.
A member cannot be declared with the same name as its containing class if
the class has a user-declared constructor.
:errbad.
struct S {
    S() { }
    int S;   // Error!
};
:eerrbad.

:MSGSYM. WARN_MEANINGLESS_QUALIFIER_IN_CAST
:MSGTXT. cv-qualifier in cast to '%T' is meaningless
:MSGJTXT.
:WARNING. 10
A top-level cv-qualifier for a non-class rvalue is meaningless.
:errbad.
const int i = (const int) 0;
:eerrbad.

:MSGSYM. WARN_MEANINGLESS_QUALIFIER_IN_RETURN_TYPE
:MSGTXT. cv-qualifier in return type '%T' is meaningless
:MSGJTXT.
:WARNING. 10
A top-level cv-qualifier for a non-class rvalue is meaningless.
:errbad.
const int f() {
    return 0;
}
:eerrbad.

:MSGSYM. WARN_C_STYLE_CAST
:MSGTXT. use of C-style cast to '%T' is discouraged
:MSGJTXT.
:WARNING. 10
Use of C-style casts "(type) (expr)" is discouraged in favour of
explicit C++ casts like static_cast, const_cast, dynamic_cast and
reinterpret_cast.
:errbad.
const signed int *f( unsigned int *psi ) {
    return ( signed int * ) psi;
}
:eerrbad.

:MSGSYM. ERR_UNMATCHED_FUNCTION_TEMPLATE_DEFN
:MSGTXT. unable to match function template definition '%S'
:MSGJTXT.
The function template definition cannot be matched to an earlier declaration.
:errbad.
template< class T >
struct A
{
    A( );
};

template< class T >
A< int >::A( )
{ }
:eerrbad.

:MSGSYM. ERR_PRAG_ENABLE_MESSAGE
:MSGTXT. form is '#pragma enable_message( msgnum )'
:MSGJTXT. �����́C'#pragma enable_message( msgnum )'
This
.kw pragma
enables the specified warning message.

:MSGSYM. ERR_PRAG_DISABLE_MESSAGE
:MSGTXT. form is '#pragma disable_message( msgnum )'
:MSGJTXT. �����́C'#pragma disable_message( msgnum )'
This
.kw pragma
disables the specified warning message.
