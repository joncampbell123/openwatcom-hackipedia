.func remove _wremove _uremove
#include <stdio.h>
int remove( const char *filename );
.ixfunc2 '&FileOp' &func
.if &'length(&wfunc.) ne 0 .do begin
int _wremove( const wchar_t *filename );
.ixfunc2 '&FileOp' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _uremove( const wchar_t *filename );
.ixfunc2 '&FileOp' &ufunc
.do end
.funcend
.desc begin
The &func function deletes the file whose name is the string pointed
to by
.arg filename
.ct .li .
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
The &func function returns zero if the operation succeeds, non-zero if
it fails.
.im errnoref
.return end
.exmp begin
#include <stdio.h>

void main()
  {
    remove( "vm.tmp" );
  }
.exmp end
.class ANSI
.system
