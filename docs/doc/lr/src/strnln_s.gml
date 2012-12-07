.func strnlen_s wcsnlen_s
#define __STDC_WANT_LIB_EXT1__  1
#include <string.h>
size_t strnlen_s( const char * s,
                  size_t maxsize );
.ixfunc2 '&String' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
size_t wcsnlen_s( const wchar_t * s,
                  size_t maxsize );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.funcend
.*
.rtconst none
.*
.desc begin
The &func function calculates the length of the string pointed to by
.arg s
.ct .li .

.im widefunc
.desc end
.*
.return begin
If
.arg s
is a null pointer, then the &func function returns zero.
Otherwise, the &func function returns the number of characters that precede the
terminating null character. If there is no null character in the first
.arg maxsize
characters of
.arg s
then &func returns
.arg maxsize
.ct .li .
At most the first
.arg maxsize
characters of
.arg s
shall
be accessed by &func.
.return end
.*
.see begin
.seelist &function. strlen
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>

void main( void )
{
    char    buffer[ 30 ] = "Hello world.";
    size_t  len;
.exmp break
    len = strnlen_s( buffer, sizeof( buffer ) );
    printf( "Length of text: %d\n", emsglen );
    printf( "Text: %s\n", buffer );
}
.exmp end
.*
.class TR 24731
.system
