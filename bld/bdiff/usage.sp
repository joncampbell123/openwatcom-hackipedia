:segment BPATCH
:segment ENGLISH
:segment QNX
Usage: %C [-p] [-q] [-b] <file>
:elsesegment
Usage: %s {-p} {-q} {-b} <file>
:endsegment
Options:
	-p	Do not prompt for confirmation.
	-b	Do not create a .BAK file.
	-q	Print current patch level of file.
:elsesegment JAPANESE
:segment QNX
�g�p���@: %C [-p] [-q] [-b] <file>
:elsesegment
�g�p���@: %s {-p} {-q} {-b} <file>
:endsegment
	-p	�m�F�̉������֎~����
	-b	.BAK �t�@�C���̍쐬���֎~����
	-q	�t�@�C���̃J�����g�p�b�`���x�����o�͂���
:endsegment
:elsesegment BDIFF
usage: %s <old_exe> <new_exe> <patch_file> [options]\n
options:  -p<file>         file to patch
          -c<file>         comment file
          -do<file>        old file's debug info
          -dn<file>        new file's debug info
          -l               don't write patch level into patch file
:endsegment
