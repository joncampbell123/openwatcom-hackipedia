.func unlink _unlink _wunlink
#include <&iohdr>
int unlink( const char *path );
.ixfunc2 '&FileOp' &func
.if &'length(&_func.) ne 0 .do begin
int _unlink( const char *path );
.ixfunc2 '&FileOp' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _wunlink( const wchar_t *path );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _uunlink( const wchar_t *path );
.ixfunc2 '&FileOp' &ufunc
.do end
.funcend
.desc begin
The &func function deletes the file whose name is the string
pointed to by
.arg path
.ct .li .
This function is equivalent to the
.kw remove
function.
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI/ISO naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts a
wide-character string argument.
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
accepts a Unicode string argument.
.do end
.desc end
.return begin
The &func
function returns zero if the operation succeeds, non-zero if it fails.
.return end
.if '&machsys' eq 'QNX' .do begin
.error begin
.begterm 12
.term EACCES
Search permission is denied for a component of
.arg path
or write permission is denied on the directory containing the link to
be removed.
.if '&machsys' eq 'QNX' .do begin
.term EBUSY
The directory named by the
.arg path
argument cannot be unlinked because it is being used by the system or
another process and the implementation considers this to be an error.
.term ENAMETOOLONG
The argument
.arg path
exceeds {PATH_MAX} in length, or a pathname component is longer than
{NAME_MAX}.
.do end
.term ENOENT
The named file does not exist or
.arg path
is an empty string.
.if '&machsys' eq 'QNX' .do begin
.term ENOTDIR
A component of
.arg path
is not a directory.
.term EPERM
The file named by
.arg path
is a directory and either the calling process does not have the
appropriate privileges, or the implementation prohibits using &func on
directories.
.term EROFS
The directory entry to be unlinked resides on a read-only file system.
.do end
.endterm
.error end
.do end
.see begin
.if '&machsys' eq 'QNX' .do begin
.seelist unlink chdir chmod close getcwd link mkdir open remove rename rmdir stat
.do end
.el .do begin
.seelist unlink chdir chmod close getcwd mkdir open remove rename rmdir stat
.do end
.see end
.exmp begin
#include <&iohdr>

void main( void )
{
    unlink( "vm.tmp" );
}
.exmp end
.class begin POSIX 1003.1
.ansiname &_func
.class end
.system
