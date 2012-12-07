.func sinh
#include <math.h>
double sinh( double x );
.ixfunc2 '&Math' &func
.ixfunc2 '&Trig' &func
.ixfunc2 '&Hyper' &func
.funcend
.desc begin
The &func function computes the hyperbolic sine of
.arg x
.ct .li .
A range error occurs if the magnitude of
.arg x
is too large.
.desc end
.return begin
The &func function returns the hyperbolic sine value.
.im errnorng
.return end
.see begin
.seelist sinh cosh tanh matherr
.see end
.exmp begin
#include <stdio.h>
#include <math.h>

void main()
  {
    printf( "%f\n", sinh(.5) );
  }
.exmp output
0.521095
.exmp end
.class ANSI
.system
