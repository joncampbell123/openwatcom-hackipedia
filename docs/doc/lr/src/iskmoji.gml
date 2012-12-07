.func iskmoji
#include <jctype.h>
int iskmoji( int c );
.ixfunc2 '&CharTest' &func
.funcend
.desc begin
The &func function tests if the argument
.arg c
is a single-byte Katakana character (excluding the Katakana
punctuation characters).
A single-byte Katakana non-punctuation character is any character for
which the following expression is true:
.millust begin
0xA6 <= c <= 0xDF
.millust end
.desc end
.return begin
The &func function returns zero if the argument is not a single-byte Katakana
non-punctuation character;
otherwise, a non-zero value is returned.
.return end
.see begin
.im seejis &function.
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>
#include <jctype.h>

JMOJI chars[] = {
    'A',
    0x9941,
    0xA4,
    0xA6
};
.exmp break
#define SIZE sizeof( chars ) / sizeof( JMOJI )
.exmp break
void main()
  {
    int   i;
.exmp break
    for( i = 0; i < SIZE; i++ ) {
      printf( "Char is %sa single-byte "
              "Katakana non-punctuation character\n",
            ( iskmoji( chars[i] ) ) ? "" : "not " );
    }
  }
.exmp output
Char is not a single-byte Katakana non-punctuation character
Char is not a single-byte Katakana non-punctuation character
Char is not a single-byte Katakana non-punctuation character
Char is a single-byte Katakana non-punctuation character
.exmp end
.class WATCOM
.system
