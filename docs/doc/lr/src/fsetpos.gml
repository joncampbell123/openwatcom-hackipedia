.func fsetpos
#include <stdio.h>
int fsetpos( FILE *fp, fpos_t *pos );
.ixfunc2 '&StrIo' &func
.funcend
.desc begin
The &func function positions the file
.arg fp
according to the value of the object pointed to by
.arg pos
.ct , which shall be a value returned by an earlier call to the
.kw fgetpos
function on the same file.
.desc end
.return begin
The &func function returns zero if successful, otherwise, the
&func function returns a non-zero value.
.im errnoref
.return end
.see begin
.seelist fsetpos fgetpos fopen fseek ftell
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
