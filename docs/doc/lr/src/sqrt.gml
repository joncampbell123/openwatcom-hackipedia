.func sqrt
#include <math.h>
double sqrt( double x );
.ixfunc2 '&Math' &func
.funcend
.desc begin
The &func function computes the non-negative square root of
.arg x
.ct .li .
A domain error occurs if the argument is negative.
.desc end
.return begin
The &func function returns the value of the square root.
.im errnodom
.return end
.see begin
.seelist sqrt exp log pow matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", sqrt(.5) );
  }
.exmp output
0.707107
.exmp end
.class ANSI
.system
