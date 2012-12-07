.func abort
#include <stdlib.h>
void abort( void );
.ixfunc2 '&Process' &func
.funcend
.desc begin
.if '&machsys' eq 'PP' .do begin
The &func function terminates program execution, returning control
to the process that started the calling program (usually the operating
system).
.do end
.if '&machsys' ne 'PP' .do begin
The &func function raises the signal SIGABRT.
The default action for SIGABRT is to terminate program execution,
returning control
to the process that started the calling program (usually the operating
system).
The status
.us unsuccessful termination
is returned to the invoking process by means of the function call
.mono raise(SIGABRT).
.do end
.if '&machsys' eq 'PP' .do begin
:cmt. Under PenPoint, the status value is ??????.
.do end
.el .if '&machsys' eq 'QNX' .do begin
Under QNX, the status value is 12.
.do end
.el .do begin
The exit code returned to the invoking process is
.kw EXIT_FAILURE
which is defined in the
.hdrfile stdlib.h
header file.
.do end
.desc end
.return begin
The &func function does not return to its caller.
.return end
.see begin
.im seeproc abort
.see end
.exmp begin
#include <stdlib.h>

void main()
  {
    int major_error = 1;
.exmp break
    if( major_error )
      abort();
  }
.exmp end
.class ANSI
.system
