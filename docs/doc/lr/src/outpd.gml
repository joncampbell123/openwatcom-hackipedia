.func outpd
#include <conio.h>
unsigned long outpd( int port,
                    unsigned long value );
.ixfunc2 '&PortIo' &func
.funcend
.desc begin
The &func function writes a double-word (four bytes), determined by
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

void main()
  {
    outpd( DEVICE, 0x12345678 );
  }
.exmp end
.class Intel
.system
