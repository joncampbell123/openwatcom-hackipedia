.func printf wprintf _uprintf
.funcw wprintf
#include <stdio.h>
int printf( const char *format, ... );
.ixfunc2 '&StrIo' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int wprintf( const wchar_t *format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _uprintf( const wchar_t *format, ... );
.ixfunc2 '&StrIo' &ufunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function writes output to the file designated by
.kw stdout
under control of the argument
.arg format
.ct .li .
The
.arg format
string is described below.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts a
wide-character string argument for
.arg format
.ct .li .
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
accepts a Unicode string argument for
.arg format
and produces Unicode character output.
.do end
.desc end
.*
.return begin
The &func function returns the number of characters written, or a
negative value if an output error occurred.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function returns the number of wide characters written, or
a negative value if an output error occurred.
.do end
.im errnoref
.return end
.*
.see begin
.im seeprtf printf
.see end
.*
.exmp begin
#include <stdio.h>

void main( void )
{
    char *weekday, *month;
.exmp break
    weekday = "Saturday";
    month = "April";
    printf( "%s, %s %d, %d\n",
          weekday, month, 18, 1987 );
    printf( "f1 = %8.4f f2 = %10.2E x = %#08x i = %d\n",
            23.45,      3141.5926,   0x1db,     -1 );
}
.exmp output
Saturday, April 18, 1987
f1 =  23.4500 f2 =  3.14E+003 x = 0x0001db i = -1
.exmp end
.*
.im printfs
