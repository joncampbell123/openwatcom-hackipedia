.func outpw
#include <conio.h>
unsigned int outpw( int port,
                    unsigned int value );
.ixfunc2 '&PortIo' &func
.funcend
.desc begin
The &func function writes a word (two bytes), determined by
.arg value
.ct , to the 80x86 hardware port whose number is given by
.arg port
.ct .li .
.im portdesc
.im privity
.desc end
.return begin
The value transmitted is returned.
.return end
.see begin
.seelist &function. inp inpd inpw outp outpd outpw
.see end
.exmp begin
#include <conio.h>
#define DEVICE 34
.exmp break
void main()
  {
    outpw( DEVICE, 0x1234 );
  }
.exmp end
.class Intel
.system
