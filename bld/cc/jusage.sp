::
:: Make sure to make changes in ALL the sections affected
::
:segment Taxp
�g�p���@: wccaxp [options] �t�@�C�� [options]
:elsesegment T386
�g�p���@: wcc386 [options] file [options]
:elsesegment
�g�p���@: wcc [options] file [options]
:endsegment
::
Options:
:segment !OS_qnx & !OS_linux
        ( /option is also accepted )
:endsegment
:segment T386
-3r       386���߂����W�X�^������n���K���ŃR�[�h����
-3s       386���߂��X�^�b�N������n���K���ŃR�[�h����
-4r       486���߃^�C�~���O�ƃ��W�X�^������n���K���ŃR�[�h����
-4s       486���߃^�C�~���O�ƃX�^�b�N������n���K���ŃR�[�h����
-5r       Pentium���߃^�C�~���O�ƃ��W�X�^������n���K���ŃR�[�h����
-5s       Pentium���߃^�C�~���O�ƃX�^�b�N������n���K���ŃR�[�h����
-6r       Pentium Pro���߃^�C�~���O�ƃ��W�X�^������n���K���ŃR�[�h����
-6s       Pentium Pro���߃^�C�~���O�ƃX�^�b�N������n���K���ŃR�[�h����
:elsesegment Ti86
-0        8086����
-1        80186����
-2        80286����
-3        386�p�œK��
-4        486�p�œK��
-5        Pentium�p�œK��
-6        Pentium Pro�p�œK��
-7        �C�����C��80x87����
:endsegment
-aa       ���[�J���̏W���̂܂��͋��p�̂ɑ΂����萔�̏������������܂�
-ad[=<file>]  generate make style auto depend file
-add[=<file>] set source dependancy name in auto depend file
-adt[=<file>] set make style auto depend target name
-adhp[=<file>]set default for no header path
-adbs         force slashes to backward '\' slashes (auto dep gen)
-adfs         force slashes to forward '/' slashes (auto dep gen)
-ai       �X�^�e�B�b�N�̏������ւ̌^�`�F�b�N���s���܂���
-aq       const/volatile�ɑ΂���C���q�s��v�̌x�����o���܂���
:segment Taxp
-as       2�o�C�g���������񂳂����̂Ɖ��肵�܂�
:endsegment
-bc       build target is a console application
-bd       �쐬�^�[�Q�b�g�̓_�C�i�~�b�N�E�����N�E���C�u����(DLL)
-bg       build target is a GUI application
-bm       �쐬�^�[�Q�b�g�̓}���`�E�X���b�h��
:segment T386 | Taxp
-br       dll���s�����C�u�������g�p���č쐬���܂�
:endsegment
-bt=<id>      �쐬�^�[�Q�b�g�̓I�y���[�e�B���O�E�V�X�e��<id>�p
-bw       �쐬�^�[�Q�b�g�̓f�t�H���g�E�E�B���h�E�E�A�v���P�[�V����
-d<id>[=text] �\�[�X�̐擪��#define <id> [text]��������̂Ƃ��ď������܂��B
-d0       �f�o�b�O���Ȃ�
-d1{+}        �s�ԍ��f�o�b�O���
-d2       ���S�V���{���b�N�f�o�b�O���
-d3       �Q�Ƃ���Ă��Ȃ��^�����܂ފ��S�V���{���b�N�f�o�b�O���
-d+       �g������-d�}�N����`�����e���܂�
-db       �u���E�U�p���𐶐����܂�
-e=<num>      �G���[���b�Z�[�W���̏����ݒ肵�܂�
:segment HIDDEN
-ec       �R�[�h�E�J�o���W�E�M�A�𐶐����܂�
:endsegment
:segment T386 | Ti86
-ee       �G�s���[�O�E�t�b�N�E���[�`�����Ăт܂�
-ef       �G���[����ьx�����b�Z�[�W�Ŋ��S�p�X�����g�p���܂�
:endsegment
-ei       �񋓌^�������I�ɏ��Ȃ��Ƃ�'int'�Ɠ��������ɂ��܂�
-em       �񋓌^�̃x�[�X�^�Ƃ��ċ����I�ɍŏ��̐����^���g�p���܂�
:segment T386 | Ti86
-en       �R�[�h�E�Z�O�����g�Ƀ��[�`�������o�͂��܂�
-ep[=<num>]   <num>�o�C�g�̃X�^�b�N���m�ۂ���v�����[�O�E�t�b�N�E���[�`�����R�[�����܂�
:endsegment
-eq       �G���[���b�Z�[�W��\�����܂���(�t�@�C���ɂ͏������܂�܂�)
:segment T386
-et       P5�v���t�@�C�����O
-ez       PharLap EZ-OMF�I�u�W�F�N�g�E�t�@�C���𐶐����܂�
:endsegment
-fh[=<file>]  �v���E�R���p�C���E�w�b�_�[���g�p���܂�
-fhq[=<file>] �v���E�R���p�C���E�w�b�_�[�g�p���b�Z�[�W��}�~���܂�
-fi=<file>    <file>�������I�ɃC���N���[�h���܂�
-fo[=<file>]  �I�u�W�F�N�g�܂��̓v���v���Z�b�T�o�͂̃t�@�C�������w�肵�܂�
-fr[=<file>]  �G���[�t�@�C�������w�肵�܂�
:segment T386 | Ti86
-fp2          287���������_�R�[�h�𐶐����܂�
-fp3          387���������_�R�[�h�𐶐����܂�
-fp5          ���������_�R�[�h��Pentium�p�ɍœK�����܂�
-fp6          ���������_�R�[�h��Pentium Pro�p�ɍœK�����܂�
-fpc          ���������_���C�u�����̌ďo���𐶐����܂�
-fpi          �G�~�����[�V�����ɂ��C�����C��80x87���߂𐶐����܂�
-fpi87        �C�����C��80x87���߂𐶐����܂�
-fpr          ���o�[�W�����̃R���p�C���ƌ݊����̂���80x87�R�[�h�𐶐����܂�
-g=<id>       �R�[�h�O���[�v����ݒ肵�܂�
:endsegment
:segment T386 | Ti86
-hc       Codeview�f�o�b�O���𐶐����܂�
-hd       DWARF�f�o�b�O���𐶐����܂�
-hw       Watcom�f�o�b�O���𐶐����܂�
:elsesegment
-hc       Codeview�f�o�b�O���𐶐����܂�
-hd       DWARF�f�o�b�O���𐶐����܂�
:endsegment
-i=<path>     �C���N���[�h��p�X��ǉ����܂�
-j        �f�t�H���g��char�𕄍��Ȃ����畄���t���ɕύX���܂�
:segment T386
-mc       �R���p�N�g�����������f��(�X���[����R�[�h/���[�W��f�[�^)
-mf       �t���b�g�����������f��(�Ӱ٥����/CS=DS=SS=ES�����肵���Ӱ٥�ް�)
-ml       ���[�W�����������f��(���[�W��R�[�h/���[�W��f�[�^)
-mm       �~�f�B�A�������������f��(���[�W��R�[�h/�X���[����f�[�^)
-ms       �X���[�������������f��(�X���[����R�[�h/�X���[����f�[�^)
:elsesegment Ti86
-mc       �R���p�N�g�����������f��(�X���[����R�[�h/���[�W��f�[�^)
-mh       �q���[�W�����������f��(���[�W��R�[�h/�q���[�W��f�[�^)
-ml       ���[�W�����������f��(���[�W��R�[�h/���[�W��f�[�^)
-mm       �~�f�B�A�������������f��(���[�W��R�[�h/�X���[����f�[�^)
-ms       �X���[�������������f��(�X���[����R�[�h/�X���[����f�[�^)
:endsegment
:segment T386 | Ti86
-nc=<id>      �R�[�h��N���X�̖��O��ݒ肵�܂�
-nd=<id>      �f�[�^��Z�O�����g�̖��O��ݒ肵�܂�
:endsegment
-nm=<file>    ���W���[���̖��O��ݒ肵�܂�
:segment T386 | Ti86
-nt=<id>      �e�L�X�g��Z�O�����g�̖��O��ݒ肵�܂�
:endsegment
:segment Taxp
-o{a,b,d,e,h,i,k,l,n,o,r,s,t,u,x,z} �œK���𐧌䂵�܂�
:elsesegment
-o{a,b,c,d,e,f[+],h.i,k,l,m,n,o,p,r,s,t,u,x,z} �œK���𐧌䂵�܂�
:endsegment
  a       -> �G�C���A�X�̐�����ɘa���܂�
  b       -> ����\���ɂ������R�[�h�𐶐����܂�
:segment T386 | Ti86
  c       -> <call followed by return>����<jump>�̍œK���𖳌��ɂ��܂�
:endsegment
  d       -> ���ׂĂ̍œK���𖳌��ɂ��܂�
  e[=<num>]   -> ���[�U�֐����C�����C���W�J���܂�(<num>�͍ő廲�ނ𐧌䂵�܂�)
:segment T386 | Ti86
  f       -> �K�v�ɉ����ăg���[�X�\�ȃX�^�b�N��t���[���𐶐����܂�
  f+          -> ��Ƀg���[�X�\�ȃX�^�b�N��t���[���𐶐����܂�
:endsegment
  h       -> �œK����O�ꂵ�܂�(�R���p�C�����Ԃ͒����Ȃ�܂�)
  i       -> �g���݊֐����C�����C���W�J���܂�
  k       -> ���W�X�^�ޔ����t���[�O���t�ɓ���܂�
  l       -> ���[�v�œK�����\�ɂ��܂�
  l+          -> ���[�v�E�A�����[�����O�Ń��[�v�œK�����\�ɂ��܂�
:segment T386 | Ti86
  m       -> �Z�p�֐����C�����C����80x87�R�[�h�œW�J���Đ������܂�
:endsegment
  n       -> ���l�I�ɂ��s���m�ɂȂ邪��荂���ȍœK�����\�ɂ��܂�
  o       -> ������������Ȃ��Ȃ��Ă��R���p�C�����p�����܂�
  p       -> ��т������������_�v�Z�̌��ʂ𐶐����܂�
  r       -> �œK�ȃp�C�v���C�����g�p���邽�߂ɖ��߂���בւ��܂�
  s       -> ���s���Ԃ��R�[�h�T�C�Y�̍œK����D�悵�܂�
  t       -> �R�[�h�T�C�Y�����s���Ԃ̍œK����D�悵�܂�
  u       -> ���ׂĂ̊֐��͂��ꂼ��ŗL�̃A�h���X��K�������܂�
:segment Taxp
  x       -> -obiler -s�Ɠ���
:elsesegment
  x       -> -obmiler -s�Ɠ���
:endsegment
  z       -> NULL�́A�^�[�Q�b�g�����̗L���ȃ��������w���܂�
-p{c,l,w=<num>} �O�����݂̂����s���܂�
  c       -> �R�����g���܂݂܂�
  l       -> #line�[�����߂�}�����܂�
  w=<num>     -> �o�͍s��<num>���Ő܂�Ԃ��܂�. 0�͐܂�Ԃ��܂���.
:segment T386 | Ti86
-r        �֐��Ăяo���̑O��ŃZ�O�����g���W�X�^��ޔ�/���X�g�A���܂�
-ri       char�����short�̖߂�l��int�ƌ��Ȃ��܂�
:endsegment
-s        �X�^�b�N�I�[�o�t���[�E�`�F�b�N���폜���܂�
:segment T386 | Ti86
-sg       �X�^�b�N�𑝉�����Ăяo���𐶐����܂�
-st       �܂��ŏ���SS��ʂ��ăX�^�b�N�E�^�b�`���܂�
:endsegment
:segment Taxp
-si       �X�^�b�N��t���[���������������p�^�[���ŏ��������܂�
:endsegment
-tp=<id>      #pragma on( <id> )��ݒ肵�܂�
-u<id>        ��`�ς݃}�N��<id>�𖢒�`�ɂ��܂�
-v        .def�t�@�C���Ɋ֐��錾���o�͂��܂�
:segment T386 | Ti86 | Taxp
-vcap         VC++ �݊���: �������X�g�̒���alloca���g�p�ł��܂�
:endsegment
:segment T386 & HIDDEN
-xgv          �C���f�b�N�X�t���O���[�o���ϐ��ɂ��܂�
:endsegment
:segment Taxp & HIDDEN
-xd       �f�t�H���g��O�n���h�����g�p���܂�
:endsegment
-w=<num>      �x�����x���ԍ����w�肵�܂�
-wcd=<num>    �x������: �x�����b�Z�[�W<num>���֎~���܂�
-wce=<num>    �x������: �x�����b�Z�[�W<num>�������܂�
-we       ���ׂĂ̌x�����G���[�Ƃ��Ĉ����܂�
:segment Ti86
-wo       �I�[�o�[���C���ꂽ�R�[�h�Ɋւ�������x�����܂�
:endsegment
-za       �g���@�\���֎~���܂�(�܂�, ISO/ANSI C++���ݎ󂯕t���܂�)
-zc       place const data into the code segment
:segment T386 | Ti86
-zdf          DS�𕂓��ɂ��܂�. �܂�, DGROUP�ɌŒ肵�܂���
-zdp          DS��DGROUP�ɌŒ肵�܂�
:endsegment
:segment T386
-zdl          DGROUP���璼��DS�Ƀ��[�h���܂�
:endsegment
-ze       �g���@�\�������܂�(�܂�, near, far, export, ��)
-zev      enable arithmetic on void derived types
:segment T386 | Ti86
-zff          FS�𕂓��ɂ��܂�. 1�̃Z�O�����g�ɌŒ肵�܂���
-zfp          FS��1�̃Z�O�����g�ɌŒ肵�܂�
-zgf          GS�𕂓��ɂ��܂�. 1�̃Z�O�����g�ɌŒ肵�܂���
-zgp          GS��1�̃Z�O�����g�ɌŒ肵�܂�
:endsegment
-zg       ��{�^���g�p�����֐��v���g�^�C�v�𐶐����܂�
-zk0          2�o�C�g�����T�|�[�g: ���{��
-zk0u         2�o�C�g������Unicode�ɕϊ����܂�
-zk1          2�o�C�g�����T�|�[�g: ������/��p��
-zk2          2�o�C�g�����T�|�[�g: �؍���
:segment !OS_linux
-zkl          2�o�C�g�����T�|�[�g: ���[�J������
:endsegment
-zku=<num>    �w�肵���R�[�h�y�[�W��UNICODE�ϊ��e�[�u�������[�h���܂�
-zl       �f�t�H���g����C�u���������폜���܂�
-zld          �t�@�C���ˑ������폜���܂�
-zlf          add default library information to object files
-zls          remove automatically inserted symbols
-zm       �e�֐���ʂ̃Z�O�����g�ɓ���܂�
-zp=<num>     �\���̃����o�[��{1,2,4,8,16}�ɐ��񂵂ăp�b�N���܂�
-zpw          �\���̂Ƀp�f�B���O���ǉ����ꂽ�Ƃ��Ɍx�����܂�
:segment Taxp
-zps          ��ɍ\���̂�qword���E�ɐ��񂵂܂�
:endsegment
-zq       �����b�Z�[�W���[�h�œ��삵�܂�
-zs       �\���`�F�b�N�݂̂��s���܂�
-zt[=<num>]   far�f�[�^�~���l�̐ݒ�(�~���l���傫���I�u�W�F�N�g��far��������)
:segment T386 | Ti86
-zu       SS != DGROUP (�X�^�b�N���f�[�^�Z�O�����g�ɂ���Ɖ��肵�܂���)
-zw       Microsoft Windows�p�̃R�[�h�𐶐����܂�
:endsegment
:segment Ti86
-zW       �����ʓI��Microsoft Windows�G���g���R�[�h��𐶐����܂�
:endsegment
