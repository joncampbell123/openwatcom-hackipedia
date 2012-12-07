.func fgetpos
#include <stdio.h>
int fgetpos( FILE *fp, fpos_t *pos );
.ixfunc2 '&StrIo' &func
.funcend
.desc begin
The &func function stores the current position of the file
.arg fp
in the object pointed to by
.arg pos
.ct .li .
The value stored is usable by the
.kw fsetpos
function for repositioning the file to its position at the time of the
call to the &func function.
.desc end
.return begin
The &func function returns zero if successful, otherwise, the
&func function returns a non-zero value.
.im errnoref
.return end
.see begin
.seelist fgetpos fopen fseek fsetpos ftell
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    FILE *fp;
    fpos_t position;
    auto char buffer[80];
.exmp break
    fp = fopen( "file", "r" );
    if( fp != NULL ) {
      fgetpos( fp, &position ); /* get position     */
      fgets( buffer, 80, fp );  /* read record      */
      fsetpos( fp, &position ); /* set position     */
      fgets( buffer, 80, fp );  /* read same record */
      fclose( fp );
    }
  }
.exmp end
.class ANSI
.system
