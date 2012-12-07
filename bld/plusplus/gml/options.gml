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
:cmt    :noequal.                               args can't have option '='
:cmt    :argequal. <char>                       args use <char> instead of '='
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

:cmt    where:
:cmt        <arch>:     i86, 386, axp, any, dbg, qnx, ppc, linux, sparc

:cmt    Translations are required for the :jtitle. and :jusage. tags
:cmt    if there is no text associated with the tag.

:title. Usage: wpp [options] file [options]
:jtitle. �g�p���@: wpp [options] file [options]
:target. i86
:title. Usage: wpp386 [options] file [options]
:jtitle. �g�p���@: wpp386 [options] file [options]
:target. 386
:title. Usage: wppaxp [options] file [options]
:jtitle. �g�p���@: wppaxp [options] file [options]
:target. axp
:title. Usage: wppppc [options] file [options]
:jtitle. �g�p���@: wppppc [options] file [options]
:target. ppc
:title. Options:
:jtitle. �I�v�V����:
:target. any
:title. \t    ( /option is also accepted )
:jtitle. \t    ( /option���g�p�ł��܂� )
:target. i86 386 axp ppc
:ntarget. qnx linux
:title. \t    ( '=' is always optional, i.e., -w4 -zp4 )
:jtitle. \t    ( '='�͏�ɏȗ��\�ł��B�܂� -w4 -zp4 )
:target. any

:page. (Press return to continue)
:jusage. (���s���邽�߂ɉ����L�[�������ĉ�����)

:chain. p preprocess source file
:jusage. p �\�[�X�t�@�C����O�������܂�
:chain. o optimization
:jusage. o �œK��

:option. 0
:target. i86
:enumerate. arch_i86
:usage. 8086 instructions
:jusage. 8086 ����

:option. 1
:target. i86
:enumerate. arch_i86
:usage. 186 instructions
:jusage. 186 ����

:option. 2
:target. i86
:enumerate. arch_i86
:usage. 286 instructions
:jusage. 286 ����

:option. 3
:target. i86
:enumerate. arch_i86
:usage. 386 instructions
:jusage. 386 ����

:option. 4
:target. i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for 486
:jusage. 386 ����, 486�p�œK��

:option. 5
:target. i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for Pentium
:jusage. 386 ����, Pentium�p�œK��

:option. 6
:target. i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for Pentium Pro
:jusage. 386 ����, Pentium Pro�p�œK��

:option. 3r 3
:target. 386
:enumerate. arch_386
:usage. 386 register calling conventions
:jusage. 386 ���W�X�^�Ăяo���K��

:option. 3s
:target. 386
:enumerate. arch_386
:usage. 386 stack calling conventions
:jusage. 386 �X�^�b�N�Ăяo���K��

:option. 4r 4
:target. 386
:enumerate. arch_386
:usage. 486 register calling conventions
:jusage. 486 ���W�X�^�Ăяo���K��

:option. 4s
:target. 386
:enumerate. arch_386
:usage. 486 stack calling conventions
:jusage. 486 �X�^�b�N�Ăяo���K��

:option. 5r 5
:target. 386
:enumerate. arch_386
:usage. Pentium register calling conventions
:jusage. Pentium ���W�X�^�Ăяo���K��

:option. 5s
:target. 386
:enumerate. arch_386
:usage. Pentium stack calling conventions
:jusage. Pentium �X�^�b�N�Ăяo���K��

:option. 6r 6
:target. 386
:enumerate. arch_386
:usage. Pentium Pro register calling conventions
:jusage. Pentium Pro ���W�X�^�Ăяo���K��

:option. 6s
:target. 386
:enumerate. arch_386
:usage. Pentium Pro stack calling conventions
:jusage. Pentium Pro �X�^�b�N�Ăяo���K��

:option. as
:target. axp
:usage. assume short integers are aligned
:jusage. short ���������񂵂Ă���Ɖ��肵�܂�

:option. bc
:target. any
:usage. build target is a console application
:jusage. �\�z�^�[�Q�b�g�̓R���\�[����A�v���P�[�V�����ł�

:option. bd
:target. any
:usage. build target is a dynamic link library (DLL)
:jusage. �\�z�^�[�Q�b�g�̓_�C�i�~�b�N������N����C�u�����ł�(DLL)

:option. bg
:target. any
:usage. build target is a GUI application
:jusage. �\�z�^�[�Q�b�g��GUI�A�v���P�[�V�����ł�

:option. bm
:target. any
:usage. build target is a multi-thread environment
:jusage. �\�z�^�[�Q�b�g�̓}���`�X���b�h���ł�

:option. br
:target. 386 axp ppc
:usage. build target uses DLL version of C/C++ run-time library
:jusage. �\�z�^�[�Q�b�g��DLL�ł�C/C++���s�����C�u�������g�p���܂�

:option. bw
:target. any
:usage. build target is a default windowing application
:jusage. �\�z�^�[�Q�b�g�̓f�t�H���g��E�B���h�E��A�v���P�[�V�����ł�

:option. bt
:target. any
:id.
:optional.
:usage. build target is operating system <id>
:jusage. �\�z�^�[�Q�b�g�̓I�y���[�e�B���O��V�X�e�� <id>

:option. d0
:target. any
:enumerate. debug_info
:timestamp.
:usage. no debugging information
:jusage. �f�o�b�O���͂���܂���

:option. d1
:target. any
:enumerate. debug_info
:timestamp.
:usage. line number debugging information
:jusage. �s�ԍ��f�o�b�O���

:option. d2
:target. any
:enumerate. debug_info
:timestamp.
:usage. symbolic debugging information
:jusage. ���S�V���{���f�o�b�O���

:option. d2i
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d2 and debug inlines; emit inlines as COMDATs
:jusage. �C�����C���֐��̓W�J�Ȃ���-d2;�C�����C���֐���COMDAT�Ƃ��ďo��

:option. d2s
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d2 and debug inlines; emit inlines as statics
:jusage. �C�����C���֐��̓W�J�Ȃ���-d2;�C�����C���֐���static�Ƃ��ďo��

:option. d2t d2~
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d2 but without type names
:jusage. �^���Ȃ��̊��S�V���{���f�o�b�O���

:option. d3
:target. any
:enumerate. debug_info
:timestamp.
:usage. symbolic debugging information with unreferenced type names
:jusage. �Q�Ƃ���Ă��Ȃ��^�����܂ފ��S�V���{���f�o�b�O���

:option. d3i
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d3 and debug inlines; emit inlines as COMDATs
:jusage. �C�����C���֐��̓W�J�Ȃ���-d3;�C�����C���֐���COMDAT�Ƃ��ďo��

:option. d3s
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d3 and debug inlines; emit inlines as statics
:jusage. �C�����C���֐��̓W�J�Ȃ���-d3;�C�����C���֐���static�Ƃ��ďo��

:option. d+
:target. any
:special. scanDefinePlus
:usage. allow extended -d macro definitions
:jusage. �g�����ꂽ -d �}�N����`�������܂�

:option. db
:target. any
:prefix.
:usage. generate browsing information
:jusage. �u���E�Y���𐶐����܂�

:option. d
:target. any
:special. scanDefine <name>[=text]
:usage. same as #define name [text] before compilation
:jusage. �R���p�C���O�� #define name [text] �Ɠ���

:option. ecc
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __cdecl
:jusage.

:option. ecd
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __stdcall
:jusage.

:option. ecf
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __fastcall
:jusage.

:option. eco
:target. i86 386
:enumerate. intel_call_conv
:internal.
:usage. set default calling convention to _Optlink
:jusage.

:option. ecp
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __pascal
:jusage.

:option. ecr
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __fortran
:jusage.

:option. ecs
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __syscall
:jusage.

:option. ecw
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __watcall (default)
:jusage.

:option. ee
:target. any
:usage. call epilogue hook routine
:jusage. �G�s���[�O��t�b�N���[�`�����Ăяo���܂�

:option. ef
:target. any
:usage. use full path names in error messages
:jusage. �G���[���b�Z�[�W�Ɋ��S�p�X�����g�p���܂�

:option. ei
:target. any
:enumerate. enum_size
:usage. force enum base type to use at least an int
:jusage. enum�^�̃x�[�X�^�Ƃ���int�^�ȏ�̑傫�����g�p���܂�

:option. em
:target. any
:enumerate. enum_size
:usage. force enum base type to use minimum integral type
:jusage. enum�^�̃x�[�X�^�Ƃ��čŏ��̐����^���g�p���܂�

:option. en
:target. any
:usage. emit routine names in the code segment
:jusage. ���[�`�������R�[�h�Z�O�����g�ɏo�͂��܂�

:option. ep
:target. any
:number. checkPrologSize 0
:usage. call prologue hook routine with <num> stack bytes available
:jusage. <num>�o�C�g�̃X�^�b�N���g�p����v�����[�O��t�b�N����[�`�����Ăяo���܂�

:option. eq
:target. any
:immediate. handleOptionEQ
:usage. do not display error messages (but still write to .err file)
:jusage. �G���[���b�Z�[�W��\�����܂���(������.err�t�@�C���ɂ͏������݂܂�)

:option. er
:target. any
:usage. do not recover from undefined symbol errors
:jusage. ����`�V���{���G���[����񕜂��܂���

:option. et
:target. 386
:usage. emit Pentium profiling code
:jusage. Pentium�v���t�@�C�����O��R�[�h�𐶐����܂�

:option. et0
:target. 386
:usage. emit Pentium-CTR0 profiling code
:jusage. Pentium-CTR0�v���t�@�C�����O��R�[�h�𐶐����܂�

:option. etp
:target. 386
:internal.
:usage. emit Timing for Profiler
:jusage. �v���t�@�C���̃^�C�~���O���o�͂��܂�

:option. esp
:target. 386
:internal.
:usage. emit Statement counting for Profiler
:jusage. �v���t�@�C���p�X�e�[�g�����g��J�E���e�B���O���o�͂��܂�

:option. ew
:target. any
:immediate. handleOptionEW
:usage. alternate error message formatting
:jusage. �ʂ̃G���[���b�Z�[�W�`�����g�p���܂�

:option. ez
:target. 386
:usage. generate PharLap EZ-OMF object files
:jusage. PharLap EZ-OMF�I�u�W�F�N�g��t�@�C���𐶐����܂�

:option. e
:target. any
:number. checkErrorLimit
:usage. set limit on number of error messages
:jusage. �G���[���b�Z�[�W���̐�����ݒ肵�܂�

:option. fbi
:target. any
:special. scanFBI
:internal.
:usage. generate browsing information
:jusage. �u���E�Y���𐶐����܂�

:option. fbx
:target. any
:special. scanFBX
:internal.
:usage. do not generate browsing information
:jusage. �u���E�Y���𐶐����܂���

:option. fc
:target. any
:file.
:immediate. handleOptionFC
:usage. specify file of command lines to be batch processed
:jusage. �o�b�`��������R�}���h���C���̃t�@�C�����w�肵�܂�

:option. fh
:target. any
:file.
:optional.
:timestamp.
:usage. use pre-compiled header (PCH) file
:jusage. �v���R���p�C����w�b�_�[(PCH)���g�p���܂�

:option. fhd
:target. any
:usage. store debug info for PCH once (DWARF only)
:jusage. PCH�p�f�o�b�O����1�x�i�[���܂�(DWARF�̂�)

:option. fhq
:target. any
:file.
:optional.
:timestamp.
:usage. do not display PCH activity warnings
:jusage. PCH�g�p���b�Z�[�W��\�����܂���

:option. fhw
:target. any
:usage. force compiler to write PCH (will never read)
:jusage. PCH���������݂ɋ������܂� (�ǂ݂܂���)

:option. fhwe
:target. any
:usage. don't count PCH activity warnings (see -we option)
:jusage. PCH�g�p���b�Z�[�W���G���[�Ƃ��Ĉ����܂���(-we�I�v�V�����Q��)

:option. fhr
:target. any
:usage. force compiler to read PCH (will never write)
:jusage. PCH��ǂݍ��݂ɋ������܂� (�����܂���)

:option. fhr!
:target. any
:usage. compiler will read PCH without checking include files
:jusage. �R���p�C���̓C���N���[�h�t�@�C�����`�F�b�N���邱�ƂȂ��APCH�ǂݍ��݂܂�
:internal.

:option. fi
:target. any
:file.
:usage. force <file> to be included
:jusage. �����I��<file>���C���N���[�h���܂�

:option. fo
:target. any
:file.
:optional.
:usage. set object or preprocessor output file name
:jusage. �I�u�W�F�N�g�܂��̓v���v���Z�b�T�̏o�̓t�@�C������ݒ肵�܂�

:option. fr
:target. any
:file.
:optional.
:usage. set error file name
:jusage. �G���[��t�@�C������ݒ肵�܂�

:option. ft
:target. any
:enumerate. file_83
:usage. check for truncated versions of file names
:jusage. �؂�l�߂��t�@�C�������`�F�b�N���܂�

:option. fx
:target. any
:enumerate. file_83
:usage. do not check for truncated versions of file names
:jusage. �؂�l�߂��t�@�C�������`�F�b�N���܂���

:option. fzh
:target. any
:usage. do not automatically postfix include file names
:jusage. do not automatically postfix include file names

:option. fzs
:target. any
:usage. do not automatically postfix source file names
:jusage. do not automatically postfix source file names

:option. 87d
:target. i86 386
:number. CmdX86CheckStack87 0
:enumerate. intel_fpu_model
:internal.
:usage. inline 80x87 instructions with specified depth
:jusage. �w�肵���[���̃C�����C��80x87����

:option. fpc
:target. i86 386
:enumerate. intel_fpu_model
:usage. calls to floating-point library
:jusage. ���������_���C�u�������Ăяo���܂�

:option. fpi
:target. i86 386
:enumerate. intel_fpu_model
:usage. inline 80x87 instructions with emulation
:jusage. �G�~�����[�V�����t���C�����C��80x87����

:option. fpi87
:target. i86 386
:enumerate. intel_fpu_model
:usage. inline 80x87 instructions
:jusage. �C�����C��80x87����

:option. fp2 fp287
:target. i86 386
:enumerate. intel_fpu_level
:usage. generate 287 floating-point code
:jusage. 287���������_�R�[�h�𐶐����܂�

:option. fp3 fp387
:target. i86 386
:enumerate. intel_fpu_level
:usage. generate 387 floating-point code
:jusage. 387���������_�R�[�h�𐶐����܂�

:option. fp5
:target. i86 386
:enumerate. intel_fpu_level
:usage. optimize floating-point for Pentium
:jusage. Pentium�p���������_�œK�������܂�

:option. fp6
:target. i86 386
:enumerate. intel_fpu_level
:usage. optimize floating-point for Pentium Pro
:jusage. Pentium Pro�p���������_�œK�������܂�

:option. fpr
:target. i86 386
:usage. generate backward compatible 80x87 code
:jusage. �o�[�W����9.0�ȑO�ƌ݊���80x87�R�[�h�𐶐����܂�

:option. fpd
:target. i86 386
:usage. enable Pentium FDIV check
:jusage. Pentium FDIV�`�F�b�N�����܂�

:option. g
:target. i86 386
:id.
:usage. set code group name
:jusage. �R�[�h��O���[�v����ݒ肵�܂�

:option. hw
:target. i86 386 
:enumerate. dbg_output
:usage. generate Watcom debugging information
:jusage. Watcom�f�o�b�O���𐶐����܂�

:option. hd
:target. i86 386 axp ppc
:enumerate. dbg_output
:usage. generate DWARF debugging information
:jusage. DWARF�f�o�b�O���𐶐����܂�

:option. hda
:target. i86 386 axp ppc
:enumerate. dbg_output
:usage. generate DWARF debugging information
:jusage. DWARF�f�o�b�O���𐶐����܂�
:internal.

:option. hc
:target. i86 386 axp ppc
:enumerate. dbg_output
:usage. generate Codeview debugging information
:jusage. Codeview�f�o�b�O���𐶐����܂�

:option. i
:target. any
:path.
:usage. add another include path
:jusage. �C���N���[�h��p�X��ǉ����܂�

:option. j
:target. any
:usage. change char default from unsigned to signed
:jusage. char�^�̃f�t�H���g��unsigned����signed�ɕύX���܂�

:option. jw
:target. any
:usage. warn about default char promotion to int
:jusage.
:internal.

:option. k
:target. any
:usage. continue processing files (ignore errors)
:jusage. �t�@�C�������𑱍s���܂�(�G���[�𖳎����܂�)

:option. la
:target. axp
:usage. output Alpha assembly listing
:jusage. Alpha�A�Z���u������X�g���o�͂��܂�
:internal.

:option. lo
:target. axp
:usage. output OWL listing
:jusage. OWL���X�g���o�͂��܂�
:internal.

:option. mc
:target. i86 386
:enumerate. mem_model
:usage. compact memory model (small code/large data)
:jusage. �R���p�N�g�����������f��(�X���[����R�[�h/���[�W��f�[�^)

:option. mf
:target. 386
:enumerate. mem_model
:usage. flat memory model (small code/small data assuming CS=DS=SS=ES)
:jusage. �t���b�g�����������f��(�X���[����R�[�h/CS=DS=SS=ES�����肵���X���[����f�[�^)

:option. mfi
:target. 386
:enumerate. mem_model
:usage. flat memory model (interrupt functions will assume flat model)
:jusage. �t��;�g����������?��(���荞�݊֐����t��;�g��?���ł���Ɖ��肷��)

:option. mh
:target. i86
:enumerate. mem_model
:usage. huge memory model (large code/huge data)
:jusage. �q���[�W�����������f��(���[�W��R�[�h/�q���[�W��f�[�^)

:option. ml
:target. i86 386
:enumerate. mem_model
:usage. large memory model (large code/large data)
:jusage. ���[�W�����������f��(���[�W��R�[�h/���[�W��f�[�^)

:option. mm
:target. i86 386
:enumerate. mem_model
:usage. medium memory model (large code/small data)
:jusage. �~�f�B�A�������������f��(���[�W��R�[�h/�X���[����f�[�^)

:option. ms
:target. i86 386
:enumerate. mem_model
:usage. small memory model (small code/small data)
:jusage. �X���[�������������f��(�X���[����R�[�h/�X���[����f�[�^)

:option. na
:target. any
:usage. disable automatic inclusion of _ialias.h
:jusage. �X���[�������������f��(�X���[����R�[�h/�X���[����f�[�^)

:option. nc
:target. i86 386
:id.
:usage. set code class name
:jusage. �R�[�h��N���X����ݒ肵�܂�

:option. nd
:target. i86 386
:id.
:usage. set data segment name
:jusage. �f�[�^��Z�O�����g����ݒ肵�܂�

:option. nm
:target. i86 386 axp ppc
:file.
:usage. set module name
:jusage. ���W���[������ݒ肵�܂�

:option. nt
:target. i86 386
:id.
:usage. set name of text segment
:jusage. �e�L�X�g��Z�O�����g����ݒ肵�܂�

:option. oa
:target. any
:usage. relax aliasing constraints
:jusage. �G�C���A�X�̐�����ɘa���܂�

:option. ob
:target. any
:usage. enable branch prediction
:jusage. ����\���ɂ������R�[�h�𐶐����܂�

:option. oc
:target. i86 386 axp ppc
:usage. disable <call followed by return> to <jump> optimization
:jusage. <call followed by return>����<jump>�̍œK���𖳌��ɂ��܂�

:option. od
:target. any
:enumerate. opt_level
:timestamp.
:usage. disable all optimizations
:jusage. ���ׂĂ̍œK���𖳌��ɂ��܂�

:option. oe
:target. any
:number. checkOENumber 100
:usage. expand user functions inline (<num> controls max size)
:jusage. ���[�U�֐����C�����C���W�J���܂�(<num>�͍ő廲�ނ𐧌䂵�܂�)

:option. of
:target. i86 386
:usage. generate traceable stack frames as needed
:jusage. �K�v�ɉ����ăg���[�X�\�ȃX�^�b�N��t���[���𐶐����܂�

:option. of+
:target. i86 386
:usage. always generate traceable stack frames
:jusage. ��Ƀg���[�X�\�ȃX�^�b�N��t���[���𐶐����܂�

:option. oh
:target. any
:usage. enable expensive optimizations (longer compiles)
:jusage. �œK�����J��Ԃ��܂�(�R���p�C���������Ȃ�܂�)

:option. oi
:target. any
:usage. expand intrinsic functions inline
:jusage. �g���݊֐����C�����C���W�J���܂�

:option. oi+
:target. any
:usage. enable maximum inlining depth
:jusage. �C�����C���W�J�̐[�����ő�ɂ��܂�

:option. ok
:target. any
:usage. include prologue/epilogue in flow graph
:jusage. �v�����[�O�ƃG�s���[�O���t���[����\�ɂ��܂�

:option. ol
:target. any
:usage. enable loop optimizations
:jusage. ���[�v�œK�����\�ɂ��܂�

:option. ol+
:target. any
:usage. enable loop unrolling optimizations
:jusage. ���[�v�E�A�����[�����O�Ń��[�v�œK�����\�ɂ��܂�

:option. om
:target. i86 386 axp ppc
:usage. generate inline code for math functions
:jusage. �Z�p�֐����C�����C����80x87�R�[�h�œW�J���Đ������܂�

:option. on
:target. any
:usage. allow numerically unstable optimizations
:jusage. ���l�I�ɂ��s���m�ɂȂ邪��荂���ȍœK�����\�ɂ��܂�

:option. oo
:target. any
:usage. continue compilation if low on memory
:jusage. ������������Ȃ��Ȃ��Ă��R���p�C�����p�����܂�

:option. op
:target. any
:usage. generate consistent floating-point results
:jusage. ��т������������_�v�Z�̌��ʂ𐶐����܂�

:option. or
:target. any
:usage. reorder instructions for best pipeline usage
:jusage. �œK�ȃp�C�v���C�����g�p���邽�߂ɖ��߂���בւ��܂�

:option. os
:target. any
:enumerate. opt_size_time
:timestamp.
:usage. favor code size over execution time in optimizations
:jusage. ���s���Ԃ��R�[�h�T�C�Y�̍œK����D�悵�܂�

:option. ot
:target. any
:enumerate. opt_size_time
:timestamp.
:usage. favor execution time over code size in optimizations
:jusage. �R�[�h�T�C�Y�����s���Ԃ̍œK����D�悵�܂�

:option. ou
:target. any
:usage. all functions must have unique addresses
:jusage. ���ׂĂ̊֐��͂��ꂼ��ŗL�̃A�h���X��K�������܂�

:option. ox
:target. any
:enumerate. opt_level
:timestamp.
:usage. equivalent to -obmiler -s
:jusage. -obmiler -s�Ɠ���

:option. oz
:target. any
:usage. NULL points to valid memory in the target environment
:jusage. NULL�́A�^�[�Q�b�g�����̗L���ȃ��������w���܂�

:option. ad
:target. any
:file.
:optional.
:usage. generate make style automatic dependency file
:jusage. generate make style automatic dependency file

:option. adbs
:target. any
:usage. force path separators to '\\' in auto-depend file
:jusage. force path separators to '\\' in auto-depend file

:option. adfs
:target. any
:usage. force path separators to '/' in auto-depend file
:jusage. force path separators to '/' in auto-depend file

:option. add
:target. any
:file.
:optional.
:usage. specify first dependency in make style auto-depend file
:jusage. specify first dependency in make style auto-depend file

:option. adhp
:target. any
:file.
:optional.
:usage. specify default path for headers without one
:jusage. specify default path for headers without one

:option. adt
:target. any
:file.
:optional.
:usage. specify target in make style auto-depend file
:jusage. specify target in make style auto-depend file

:option. pil
:target. any
:nochain.
:usage. preprocessor ignores #line directives
:jusage. preprocessor ignores #line directives

:option. p
:target. any
:usage.
:jusage.

:option. pe
:target. any
:usage. encrypt identifiers
:jusage. ���ʎq���Í������܂�

:option. pl
:target. any
:usage. insert #line directives
:jusage. #line�[�����߂�}�����܂�

:option. pj
:target. any
:internal.
:usage. insert // #line comments
:jusage. // #line�[�����߂�}�����܂�

:option. pc
:target. any
:usage. preserve comments
:jusage. �R�����g���c���܂�

:option. pw
:target. any
:number. checkPPWidth
:usage. wrap output lines at <num> columns. Zero means no wrap.
:jusage. �o�͍s��<num>���Ő܂�Ԃ��܂�. 0�͐܂�Ԃ��܂���.

:option. p#
:target. any
:char.
:internal.
:usage. set preprocessor delimiter to something other than '#'
:jusage. �v���v���Z�b�T�̋�؂�L����'#'�ȊO�̉����ɐݒ肵�܂�

:option. q
:target. any
:usage. operate quietly (display only error messages)
:jusage. �����b�Z�[�W���[�h�œ��삵�܂�(�G���[���b�Z�[�W�̂ݕ\������܂�)

:option. r
:target. i86 386
:usage. save/restore segment registers across calls
:jusage. �֐��Ăяo���̑O��ŃZ�O�����g���W�X�^��ޔ�/���X�g�A���܂�

:option. re
:target. 386
:internal.
:usage. special Neutrino R/W data access code generation
:jusage.

:option. ri
:target. i86 386
:usage. return chars and shorts as ints
:jusage. �S�Ă̊֐��̈����Ɩ߂�l��int�^�ɕϊ����܂�

:option. rod
:target. any
:path.
:internal.
:usage. specified <path> contains read-only files
:jusage. �w�肳�ꂽ<path>�ɂ͓ǂݍ��ݐ�p�t�@�C�����܂܂�Ă��܂�

:option. s
:target. any
:usage. remove stack overflow checks
:jusage. �X�^�b�N�I�[�o�t���[�E�`�F�b�N���폜���܂�

:option. sg
:target. i86 386
:usage. generate calls to grow the stack
:jusage. �X�^�b�N�𑝉�����Ăяo���𐶐����܂�

:option. si
:target. axp
:usage. generate calls to initialize local storage
:jusage. ���[�J���������������������Ăяo���𐶐����܂�

:option. st
:target. i86 386
:usage. touch stack through SS first
:jusage. �܂��ŏ���SS��ʂ��ăX�^�b�N�E�^�b�`���܂�

:option. t
:target. any
:number. checkTabWidth
:usage. set number of spaces in a tab stop
:jusage. �^�u��X�g�b�v�ɑΉ�����󔒂̐���ݒ肵�܂�

:option. tp
:target. dbg
:id.
:usage. set #pragma on( <id> )
:jusage. #pragma on( <id> )��ݒ肵�܂�

:option. u
:target. any
:special. scanUndefine [=<name>]
:usage. undefine macro name
:jusage. �}�N�����𖢒�`�ɂ��܂�

:option. v
:target. any
:usage. output function declarations to .def file
:jusage. .def�t�@�C���Ɋ֐��錾���o�͂��܂�

:option. vcap
:target. 386 axp
:usage. VC++ compatibility: alloca allowed in argument lists
:jusage. VC++ �݊���: �������X�g�̒���alloca���g�p�ł��܂�

:option. w
:target. any
:enumerate. warn_level
:number. checkWarnLevel
:usage. set warning level number
:jusage. �x�����x���ԍ���ݒ肵�܂�

:option. wcd
:target. any
:number.
:multiple.
:usage. warning control: disable warning message <num>
:jusage. �x������: �x�����b�Z�[�W<num>���֎~���܂�

:option. wce
:target. any
:number.
:multiple.
:usage. warning control: enable warning message <num>
:jusage. �x������: �x�����b�Z�[�W <num> �̕\�������܂�

:option. we
:target. any
:usage. treat all warnings as errors
:jusage. ���ׂĂ̌x�����G���[�Ƃ��Ĉ����܂�

:option. wx
:target. any
:enumerate. warn_level
:usage. set warning level to maximum setting
:jusage. �x�����x�����ő�ݒ�ɂ��܂�

:option. xbnm
:target. any
:internal.
:usage. use latest (incompatible) name mangling algorithms
:jusage. 

:option. xbsa
:target. any
:internal.
:usage. do not align segments if at all possible
:jusage. 

:option. xbov1
:target. any
:internal.
:usage. WP 13.3.3.2 change
:jusage. 

:option. xcmb
:target. any
:internal.
:usage. bind modifiers during template instantiation
:jusage. �e���v���[�g���C���X�^���X������ԂɏC���q���o�C���h���܂�

:option. xcpi
:target. any
:internal.
:usage. instantiate template functions if a prototype is visible
:jusage. �v���g�^�C�v������΁A�e���v���[�g�֐����C���X�^���X�����܂�

:option. xd
:target. any
:enumerate. exc_level
:usage. disable exception handling (default) 
:jusage. ��O�������g�p�s�ɂ��܂�(�f�t�H���g) 

:option. xds
:target. any
:enumerate. exc_level
:usage. disable exception handling (table-driven destructors)
:jusage. ��O�������g�p�s�ɂ��܂�(�e�[�u����h���u���̃f�X�g���N�^)

:option. xdt
:target. any
:enumerate. exc_level
:usage. disable exception handling (same as -xd)
:jusage. ��O�������g�p�s�ɂ��܂�(-xd�Ɠ���)

:option. xr
:target. any
:usage. enable RTTI
:jusage. RTTI���g�p�\�ɂ��܂�

:option. xs
:target. any
:enumerate. exc_level
:usage. enable exception handling 
:jusage. ��O�������g�p�\�ɂ��܂�

:option. xss
:target. any
:enumerate. exc_level
:usage. enable exception handling (table-driven destructors)
:jusage. ��O�������g�p�\�ɂ��܂�(�e�[�u����h���u���̃f�X�g���N�^)

:option. xst
:target. any
:enumerate. exc_level
:usage. enable exception handling (direct calls for destruction)
:jusage. ��O�������g�p�\�ɂ��܂�(�f�X�g���N�^�ւ̒��ڌďo��)

:option. xgls
:target. i86
:internal.
:usage. only check seg of i86 far pointers when checking NULL equality
:jusage.

:option. xgv
:target. 386
:internal.
:usage. indexed global variables
:jusage. �C���f�b�N�X�t���O���[�o���ϐ�

:option. xto
:target. any
:internal.
:usage. obfuscate type signature names
:jusage.

:option. za
:target. any
:enumerate. iso
:usage. disable extensions (i.e., accept only ISO/ANSI C++)
:jusage. �g���@�\���g�p�s�ɂ��܂�(�܂�, ISO/ANSI C++�̂ݎ󂯕t���܂�)

:option. za0x
:target. any
:usage. enable some features of the upcoming ISO C++0x standard
:jusage.

:option. zat
:target. any
:usage. disable alternative tokens (e.g. and, or, not)
:jusage.

:option. zc
:target. i86 386
:usage. place const data into the code segment
:jusage. ���e������������R�[�h�Z�O�����g�ɓ���܂�

:option. zdf
:target. i86 386
:enumerate. ds_peg
:usage. DS floats (i.e. not fixed to DGROUP)
:jusage. DS�𕂓��ɂ��܂�(�܂�DGROUP�ɌŒ肵�܂���)

:option. zdp
:target. i86 386
:enumerate. ds_peg
:usage. DS is pegged to DGROUP
:jusage. DS��DGROUP�ɌŒ肵�܂�

:option. zdl
:target. 386
:usage. load DS directly from DGROUP
:jusage. DGROUP����DS�ɒ��ڃ��[�h���܂�

:option. ze
:target. any
:enumerate. iso
:usage. enable extensions (i.e., near, far, export, etc.)
:jusage. �g���@�\���g�p�\�ɂ��܂�(�܂�, near, far, export, ��.)

:option. zf
:target. any
:usage. scope of for loop initializer extends beyond loop
:jusage. FIX ME

:option. zfw
:target. i86
:usage. generate FWAIT instructions on 386 and later
:jusage.

:option. zfw
:target. 386
:usage. generate FWAIT instructions
:jusage.

:option. zff
:target. i86 386
:enumerate. fs_peg
:usage. FS floats (i.e. not fixed to a segment)
:jusage. FS�𕂓��ɂ��܂�(�܂�, 1�̃Z�O�����g�ɌŒ肵�܂���)

:option. zfp
:target. i86 386
:enumerate. fs_peg
:usage. FS is pegged to a segment
:jusage. FS��1�̃Z�O�����g�ɌŒ肵�܂�

:option. zg
:target. any
:usage. generate function prototypes using base types
:jusage. ��{�^���g�p�����֐��v���g�^�C�v�𐶐����܂�

:option. zgf
:target. i86 386
:enumerate. gs_peg
:usage. GS floats (i.e. not fixed to a segment)
:jusage. GS�𕂓��ɂ��܂�(�܂�, 1�̃Z�O�����g�ɌŒ肵�܂���)

:option. zgp
:target. i86 386
:enumerate. gs_peg
:usage. GS is pegged to a segment
:jusage. GS��1�̃Z�O�����g�ɌŒ肵�܂�

:option. zi
:target. dbg
:usage. dump informational statistics to stdout
:jusage. ���Ƃ��ē��v�l��stdout�ɏo�͂��܂�

:option. zk0 zk
:target. any
:enumerate. char_set
:usage. double-byte character support: Kanji
:jusage. 2�o�C�g�����T�|�[�g: ���{��

:option. zk1
:target. any
:enumerate. char_set
:usage. double-byte character support: Chinese/Taiwanese
:jusage. 2�o�C�g�����T�|�[�g: ������/��p��

:option. zk2
:target. any
:enumerate. char_set
:usage. double-byte character support: Korean
:jusage. 2�o�C�g�����T�|�[�g: �؍���

:option. zk0u
:target. any
:enumerate. char_set
:usage. translate double-byte Kanji to Unicode
:jusage. 2�o�C�g������Unicode�ɕϊ����܂�

:option. zkl
:target. any
:enumerate. char_set
:usage. double-byte character support: local installed language
:jusage. 2�o�C�g�����T�|�[�g: ���[�J���ɃC���X�g�[�����ꂽ����

:option. zku
:target. any
:enumerate. char_set
:number.
:usage. load Unicode translate table for specified code page
:jusage. �w�肵���R�[�h�y�[�W��Unicode�ϊ��e�[�u�������[�h���܂�

:option. zl
:target. any
:usage. remove default library information
:jusage. �f�t�H���g����C�u���������폜���܂�

:option. zld
:target. any
:usage. remove file dependency information
:jusage. �t�@�C���ˑ������폜���܂�

:option. zlf
:target. any
:usage. always generate default library information
:jusage. �f�t�H���g����C�u����������ɐ������܂�

:option. zls
:target. any
:usage. remove automatically inserted symbols
:jusage. remove automatically inserted symbols

:option. zm
:target. i86 386
:usage. emit functions in separate segments
:jusage. �e�֐���ʂ̃Z�O�����g�ɓ���܂�

:option. zm
:target. axp
:usage. emit functions in separate sections
:jusage. �e�֐���ʂ̃Z�O�����g�ɓ���܂�

:option. zmf
:target. i86 386
:usage. emit functions in separate segments (near functions allowed)
:jusage. �e�֐���ʂ̃Z�O�����g�ɓ���܂�(near�֐����\�ł�)

:option. zo
:target. 386
:usage. use exception-handling for a specific operating system
:jusage. �w�肳�ꂽ�I�y���[�e�B���O��V�X�e���p�̗�O�������g�p���܂�

:option. zp
:target. any
:number. checkPacking
:usage. pack structure members with alignment {1,2,4,8,16}
:jusage. �\���̃����o�[��{1,2,4,8,16}�ɐ��񂵂ăp�b�N���܂�

:option. zpw
:target. any
:usage. output warning when padding is added in a class
:jusage. �N���X�Ƀp�f�B���O���ǉ����ꂽ�Ƃ��Ɍx�����܂�

:option. zq
:target. any
:usage. operate quietly (display only error messages)
:jusage. �����b�Z�[�W���[�h�œ��삵�܂�(�G���[���b�Z�[�W�̂ݕ\������܂�)

:option. zro
:target. any
:usage. omit floating point rounding calls (non ANSI)
:jusage. omit floating point rounding calls (non ANSI)

:option. zri
:target. 386
:usage. inline floating point rounding calls
:jusage. inline floating point rounding calls

:option. zs
:target. any
:usage. syntax check only
:jusage. �\���`�F�b�N�݂̂��s���܂�

:option. zt
:target. i86 386
:number. CmdX86CheckThreshold 256
:usage. far data threshold (i.e., larger objects go in far memory)
:jusage. far�f�[�^�~���l(�܂�, �~���l���傫���I�u�W�F�N�g��far�������ɒu���܂�)

:option. zu
:target. i86 386
:usage. SS != DGROUP (i.e., do not assume stack is in data segment)
:jusage. SS != DGROUP (�܂�, �X�^�b�N���f�[�^�Z�O�����g�ɂ���Ɖ��肵�܂���)

:option. zv
:target. any
:usage. enable virtual function removal optimization
:jusage. ���z�֐����폜����œK�����s���܂�

:option. zw
:target. i86 386
:enumerate. win
:usage. generate code for Microsoft Windows
:jusage. Microsoft Windows�p�̃R�[�h�𐶐����܂�

:option. zws
:target. i86
:enumerate. win
:usage. generate code for Microsoft Windows with smart callbacks
:jusage. �X�}�[�g��R�[���o�b�N������Microsoft Windows�p�R�[�h�𐶐����܂�

:option. z\W
:target. i86
:enumerate. win
:usage. more efficient Microsoft Windows entry sequences
:jusage. �����ʓI��Microsoft Windows�G���g���R�[�h��𐶐����܂�

:option. z\Ws
:target. i86
:enumerate. win
:usage. generate code for Microsoft Windows with smart callbacks
:jusage. �X�}�[�g��R�[���o�b�N������Microsoft Windows�p�R�[�h�𐶐����܂�

:option. zx
:target. i86 386
:usage. assume functions will modify FPU registers
:jusage. �֐���FPU���W�X�^��ύX����Ɖ��肵�܂�
:internal.

:option. zz
:target. 386
:usage. remove "@size" from __stdcall function names (10.0 compatible)
:jusage. "@size"��__stdcall�֐�������폜���܂�(10.0�Ƃ̌݊���)
