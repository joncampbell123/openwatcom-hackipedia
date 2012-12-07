.func gets _getws _ugets
.funcw _getws
#include <stdio.h>
char *gets( char *buf );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
wchar_t *_getws( wchar_t *buf );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
wchar_t *_ugets( wchar_t *buf );
.ixfunc2 '&StrIo' &ufunc
.do end
.funcend
.desc begin
The &func function gets a string of characters from the file
designated by
.kw stdin
and stores them in the array pointed to by
.arg buf
until end-of-file is encountered or a new-line character is read.
Any new-line character is discarded, and a null character is placed
immediately after the last character read into the array.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it gets a string
of multibyte characters (if present) from the input stream pointed to
by
.kw stdin
.ct , converts them to wide characters, and stores them in the
wide-character array pointed to by
.arg buf
until end-of-file is encountered or a wide-character new-line
character is read.
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it gets
a string of Unicode characters from
.kw stdin
and stores them in the array pointed to by
.arg buf
until end-of-file is encountered or a Unicode new-line character is
read.
.do end
.np
It is recommended that
.kw fgets
be used instead of &func because data beyond the array
.arg buf
will be destroyed if a new-line character is not read from the input stream
.kw stdin
before the end of the array
.arg buf
is reached.
.if '&machsys' ne 'PP' .do begin
.np
A common programming error is to assume the presence of a new-line
character in every string that is read into the array.
A new-line character may not appear as the last character in a file,
just before end-of-file.
.do end
.desc end
.return begin
The &func function returns
.arg buf
if successful.
.mono NULL
is returned if end-of-file is encountered, or if a read error occurs.
.im errnoref
.return end
.see begin
.seelist &function. fgetc fgetchar fgets fopen getc getchar gets ungetc
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    char buffer[80];
.exmp break
    while( gets( buffer ) != NULL )
      puts( buffer );
  }
.exmp end
.class ANSI
.system
