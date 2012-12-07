.func jstrchr _fjstrchr
#include <jstring.h>
JSTRING jstrchr( const JCHAR *s, JMOJI c );
.ixfunc2 '&Jstring' &func
.ixfunc2 '&Jsearch' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrchr( const JCHAR __far *s, JMOJI c );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jsearch' &ffunc
.do end
.funcend
.desc begin
.if &farfnc eq 0 .do begin
The &func function locates
.do end
.el .do begin
The &func and &ffunc functions locate
.do end
the first occurrence of the single- or double-byte character
.arg c
in the Kanji string pointed to by
.arg s
.ct .li .
The terminating null character is considered to be part of the string.
.im ffarfunc
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
a pointer to the located character, or
.mono NULL
if the character does not occur in the Kanji string.
.return end
.see begin
.seelist &function. jstrchr jstrcspn jstrrchr jstrspn jstrstr jstrtok
.seelist &function. memchr strcspn strrchr strspn strstr strtok
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <jstring.h>

void main()
  {
    JCHAR buffer[80];
    JSTRING where;
.exmp break
    strcpy( buffer, "video x-rays" );
    where = jstrchr( buffer, 'x' );
    if( where == NULL ) {
        printf( "'x' not found\n" );
    }
  }
.exmp end
.class WATCOM
.system
