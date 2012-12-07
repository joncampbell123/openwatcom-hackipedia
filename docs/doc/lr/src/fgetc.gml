.func fgetc fgetwc _ufgetc
.funcw fgetwc
#include <stdio.h>
int fgetc( FILE *fp );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
#include <wchar.h>
wint_t fgetwc( FILE *fp );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _ufgetc( FILE *fp );
.ixfunc2 '&StrIo' &ufunc
.do end
.funcend
.desc begin
The &func function gets the next character from the file designated by
.arg fp
.ct .li .
The character is
.id signed.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it gets the next
multibyte character (if present) from the input stream pointed to by
.arg fp
and converts it to a wide character.
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
returns a Unicode character.
.do end
.desc end
.return begin
The &func function returns the next character from the input stream
pointed to by
.arg fp
.ct .li .
If the stream is at end-of-file, the end-of-file indicator is set and
&func returns
.kw EOF
.ct .li .
If a read error occurs, the error indicator is set and &func returns
.kw EOF
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function returns the next wide character from the input
stream pointed to by
.arg fp
.ct .li .
If the stream is at end-of-file, the end-of-file indicator is set and
&wfunc returns
.kw WEOF
.ct .li .
If a read error occurs, the error indicator is set and &wfunc returns
.kw WEOF
.ct .li .
If an encoding error occurs,
.kw errno
is set to
.kw EILSEQ
and &wfunc returns
.kw WEOF
.ct .li .
.do end
.np
.im errnoref
.return end
.see begin
.seelist &function. fgetc fgetchar fgets fopen getc getchar gets ungetc
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *fp;
    int c;
.exmp break
    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      while( (c = fgetc( fp )) != EOF )
        fputc( c, stdout );
      fclose( fp );
    }
  }
.exmp end
.class ANSI
.system
