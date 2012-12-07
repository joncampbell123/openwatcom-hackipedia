:cmt
:cmt GML Macros used:
:cmt
:cmt    :chain. <char> <usage>                  options that start with <char>
:cmt                                            can be chained together i.e.,
:cmt                                            -oa -ox -ot => -oaxt
:cmt    :option. <option> <synonym> ...         define an option
:cmt    :target. <arch1> <arch2> ...            valid for these architectures
:cmt    :ntarget. <arch1> <arch2> ...           not valid for these architectures
:cmt    :immediate. <fn>                        <fn> is called when option parsed
:cmt    :code. <source-code>                    <source-code> is executed when option parsed
:cmt    :enumerate. <field> [<value>]           option is one value in <name> enumeration
:cmt    :number. [<fn>] [<default>]             =<n> allowed; call <fn> to check
:cmt    :id. [<fn>]                             =<id> req'd; call <fn> to check
:cmt    :char.[<fn>]                            =<char> req'd; call <fn> to check
:cmt    :file.                                  =<file> req'd
:cmt    :path.                                  =<path> req'd
:cmt    :special. <fn> [<arg_usage_text>]       call <fn> to parse option
:cmt    :optional.                              value is optional
:cmt    :internal.                              option is undocumented
:cmt    :prefix.                                prefix of a :special. option
:cmt    :usage. <text>                          English usage text
:cmt    :jusage. <text>                         Japanese usage text
:cmt    :title.                                 English title usage text
:cmt    :jtitle.                                Japanese title usage text
:cmt    :page.                                  text for paging usage message
:cmt    :nochain.                               option isn't chained with other options
:cmt    :timestamp.                             kludge to record "when" an option
:cmt                                            is set so that dependencies
:cmt                                            between options can be simulated
:cmt
:cmt Global macros
:cmt
:cmt    :noequal.                               args can't have option '='
:cmt    :argequal. <char>                       args use <char> instead of '='
:cmt

:cmt    where:
:cmt        <arch>:     i86, 386, axp, any, dbg, qnx, ppc, linux, sparc

:cmt    Translations are required for the :jtitle. and :jusage. tags
:cmt    if there is no text associated with the tag.

:title. Usage:   wasm [options] asm_file [options] [@env_var]
:jtitle. �g�p���@: wasm [options] file [options] [@env_var]
:target. any

:title. Options:
:jtitle. �I�v�V����:
:target. qnx linux
:title. Options:                    ( /option is also supported )
:jtitle. �I�v�V����:                 ( /��߼�� �ł��w��ł��܂� )
:ntarget. qnx linux

:page. (Press return to continue)
:jusage. (Press return to continue)

:option. ? h
:target. any
:usage. print this message
:jusage. ���̃��b�Z�[�W��\�����܂�

:option. 0
:target. any
:usage. 8086 instructions
:jusage. 8086 ����

:option. 1
:target. any
:usage. 80186 instructions
:jusage. 80186 ����

:chain. 2 80286 instructions
:jusage. 80286 ����

:option. 2p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:chain. 3 80386 instructions
:jusage. 80386 ����

:option. 3r
:target. any
:usage. register calling conventions
:jusage. ���W�X�^�Ăяo���K��

:option. 3s
:target. any
:usage. stack calling conventions
:jusage. �X�^�b�N�Ăяo���K��

:option. 3p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:chain. 4 80486 instructions
:jusage. 80486 ����

:option. 4r
:target. any
:usage. register calling conventions
:jusage. ���W�X�^�Ăяo���K��

:option. 4s
:target. any
:usage. stack calling conventions
:jusage. �X�^�b�N�Ăяo���K��

:option. 4p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:chain. 5 Pentium instructions
:jusage. Pentium ����

:option. 5r
:target. any
:usage. register calling conventions
:jusage. ���W�X�^�Ăяo���K��

:option. 5p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:option. 5s
:target. any
:usage. stack calling conventions
:jusage. �X�^�b�N�Ăяo���K��

:chain. 6 Pentium Pro instructions
:jusage. 6 Pentium Pro ����

:option. 6r
:target. any
:usage. register calling conventions
:jusage. ���W�X�^�Ăяo���K��

:option. 6s
:target. any
:usage. stack calling conventions
:jusage. �X�^�b�N�Ăяo���K��

:option. 6p
:target. any
:usage. protected mode instructions
:jusage. protected mode instructions

:option. bt
:target. any
:id.
:optional.
:usage. set the build target to <id>
:jusage. set the build target to <id>

:option. c 
:target. any
:usage. disable output OMF COMMENT record about data in code
:jusage. disable output OMF COMMENT record about data in code

:option. d
:target. any
:special. scanDefine <name>[=text]
:usage. define text macro <name>[=text]
:jusage. �e�L�X�g�}�N�����`���܂� <name>[=text]

:option. d1
:target. any
:usage. line number debugging support
:jusage. �s�ԍ��f�o�b�O�����o�͂��܂�

:option. e 
:target. any
:usage. stop reading ASM file at END directive
:jusage. END�f�B���N�e�B�u��ASM�t�@�C���̓ǂݍ��݂��~�߂܂�

:option. e
:target. any
:number.
:usage. set error limit number
:jusage. set error limit number

:option. fi
:target. any
:file.
:usage. force <file> to be included
:jusage. force <file> to be included

:option. fo
:target. any
:file.
:optional.
:usage. set object file name
:jusage. �I�u�W�F�N�g�t�@�C����ݒ肵�܂�

:option. fp0 
:target. any
:usage. floating-point for 8087
:jusage. floating-point for 8087

:option. fp2 
:target. any
:usage. floating-point for 287
:jusage. floating-point for 287

:option. fp3 
:target. any
:usage. floating-point for 387
:jusage. floating-point for 387

:option. fp5 
:target. any
:usage. floating-point for Pentium
:jusage. floating-point for Pentium

:option. fp6 
:target. any
:usage. floating-point for Pentium Pro
:jusage. floating-point for Pentium Pro

:option. fpc 
:target. any
:usage. calls to floating-point library
:jusage. calls to floating-point library

:option. fpi 
:target. any
:usage. inline 80x87 instructions with emulation
:jusage. inline 80x87 instructions with emulation

:option. fpi87 
:target. any
:usage. inline 80x87 instructions
:jusage. inline 80x87 instructions

:option. fr fe
:target. any
:file.
:optional.
:usage. set error file name
:jusage. �G���[�t�@�C�������w�肵�܂�

:option. i
:target. any
:path.
:usage. add directory to list of include directories
:jusage. �C���N���[�h�E�f�B���N�g���̃��X�g��ǉ����܂�

:option. j s
:target. any
:usage. force signed types to be used for signed values
:jusage. �����t���^�𕄍��t���l�̂��߂Ɏg�p����悤�ɂ��܂�

:chain. m Memory model
:jusage. �������E���f��

:option. mc
:target. any
:usage. Compact
:jusage. Compact

:option. mf
:target. any
:usage. Flat
:jusage. Flat

:option. mh
:target. any
:usage. Huge
:jusage. Huge

:option. ml
:target. any
:usage. Large
:jusage. Large

:option. mm
:target. any
:usage. Medium
:jusage. Medium

:option. ms
:target. any
:usage. Small
:jusage. Small

:option. mt
:target. any
:usage. Tiny
:jusage. Tiny

:option. nc
:target. any
:id.
:internal.
:usage. set code class name
:jusage. set code class name

:option. nd
:target. any
:id.
:usage. set data segment name
:jusage. set data segment name

:option. nm
:target. any
:id.
:usage. set module name
:jusage. set module name

:option. nt
:target. any
:id.
:usage. set name of text segment
:jusage. set name of text segment

:option. o
:target. any
:usage. allow C form of octal constants
:jusage. allow C form of octal constants

:option. zcm
:target. any
:special. scanMode [=<mode>]
:usage. set compatibility mode - watcom (default), masm or tasm
:jusage. set compatibility mode - watcom (default), masm or tasm

:option. zld
:target. any
:usage. suppress file dependency info in object file
:jusage. suppress file dependency info in object file

:option. zq q
:target. any
:usage. operate quietly
:jusage. ���b�Z�[�W���̏o�͂����܂���

:option. zz
:target. any
:usage. remove '@size' from STDCALL function names
:jusage. remove '@size' from STDCALL function names

:option. zzo
:target. any
:usage. don't mangle STDCALL symbols (backward compatible)
:jusage. don't mangle STDCALL symbols (backward compatible)

:option. w
:target. any
:number.
:usage. set warning level number
:jusage. set warning level number

:option. we
:target. any
:usage. treat all warnings as errors
:jusage. treat all warnings as errors

:option. wx
:target. any
:usage. set warning level to the highest level
:jusage. set warning level to the highest level
