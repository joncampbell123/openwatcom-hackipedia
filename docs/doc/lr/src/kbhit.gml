.func kbhit _kbhit
#include <conio.h>
int kbhit( void );
.ixfunc2 '&KbIo' &func
.if &'length(&_func.) ne 0 .do begin
int _kbhit( void );
.ixfunc2 '&KbIo' &_func
.funcend
.desc begin
The &func function tests whether or not a keystroke is currently
available.
When one is available, the function
.kw getch
or
.kw getche
may be used to obtain the keystroke in question.
.pp
With a stand-alone program, the &func function may be called
continuously until a keystroke is available.
.if '&machsys' eq 'QNX' .do begin
Note that loops involving the &func function are not recommended in
multitasking systems.
.do end
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI/ISO naming conventions.
.do end
.desc end
.return begin
The &func
function returns zero when no keystroke is available; otherwise, a
non-zero value is returned.
.return end
.see begin
.im seeiocon kbhit
.see end
.exmp begin
/*
 * This program loops until a key is pressed
 * or a count is exceeded.
 */
#include <stdio.h>
#include <conio.h>

void main( void )
{
    unsigned long i;
.exmp break
    printf( "Program looping. Press any key.\n" );
    for( i = 0; i < 10000; i++ ) {
        if( kbhit() ) {
            getch();
            break;
        }
    }
}
.exmp end
.class begin WATCOM
.ansiname &_func
.class end
.system
