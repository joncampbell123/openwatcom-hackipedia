.func fputs fputws _ufputs
.funcw fputws
#include <stdio.h>
int fputs( const char *buf, FILE *fp );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
#include <wchar.h>
int fputws( const wchar_t *buf, FILE *fp );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _ufputs( const wchar_t *buf, FILE *fp );
.ixfunc2 '&StrIo' &ufunc
.do end
.funcend
.desc begin
The &func function writes the character string pointed to by
.arg buf
to the output stream designated by
.arg fp
.ct .li .
The terminating null character is not written.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it converts the
wide character string specified by
.arg buf
to a multibyte character string and writes it to the output stream.
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
writes a Unicode character string to the output stream.
.do end
.desc end
.return begin
The &func function returns
.kw EOF
if an error occurs; otherwise, it returns a non-negative value
(the number of characters written).
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function returns
.kw EOF
if a write or encoding error occurs; otherwise, it returns a
non-negative value (the number of characters written).
.do end
.im errnoref
.return end
.see begin
.seelist &function. fopen fputc fputchar fputs putc putchar puts ferror
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *fp;
    char buffer[80];
.exmp break
    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      while( fgets( buffer, 80, fp ) != NULL )
        fputs( buffer, stdout );
      fclose( fp );
    }
  }
.exmp end
.class ANSI
.system
