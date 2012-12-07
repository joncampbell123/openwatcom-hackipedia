.func _setmbcp
#include <mbctype.h>
int _setmbcp( int codepage );
.funcend
.desc begin
The &func function sets the current code page number.
.desc end
.return begin
The &func function returns zero if the code page is set successfully.
If an invalid code page value is supplied for
.arg codepage
.ct , the function returns -1 and the code page setting is unchanged.
.return end
.see begin
.im seeismbb &function.
.see end
.exmp begin
#include <stdio.h>
#include <mbctype.h>

void main()
  {
    printf( "%d\n", _setmbcp( 932 ) );
    printf( "%d\n", _getmbcp() );
  }
.exmp output
0
932
.exmp end
.class WATCOM
.system
