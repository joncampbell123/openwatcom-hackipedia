.func strcoll wcscoll _mbscoll _ustrcoll
#include <string.h>
int strcoll( const char *s1, const char *s2 );
.ixfunc2 '&String' &func
.ixfunc2 '&Compare' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int wcscoll( const wchar_t *s1, const wchar_t *s2 );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Compare' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
int _mbscoll( const unsigned char *s1, const unsigned char *s2 );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Compare' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _ustrcoll( const wchar_t *s1, const wchar_t *s2 );
.ixfunc2 '&String' &ufunc
.ixfunc2 '&Compare' &ufunc
.do end
.funcend
.desc begin
The &func function compares the string pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct .li .
The comparison uses the collating sequence selected by the
.kw setlocale
function.
The function will be equivalent to the
.kw strcmp
function when the collating sequence is selected from the
.mono "C"
locale.
.im widefunc
.im mbsfunc
.im unifunc
.if &'length(&ufunc.) ne 0 .do begin
The &ufunc comparison uses the collating sequence defined by the
Unicode character set.
.do end
.desc end
.return begin
The &func function returns an integer less than, equal to, or
greater than zero, indicating that the string pointed to by
.arg s1
is less than, equal to, or greater than the string pointed to by
.arg s2
.ct , according to the collating sequence selected.
.return end
.see begin
.seelist strcoll setlocale strcmp strncmp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char buffer[80] = "world";

void main()
  {
    if( strcoll( buffer, "Hello" ) < 0 ) {
        printf( "Less than\n" );
    }
  }
.exmp end
.class ANSI
.system
