.func dirname 
#include <libgen.h>
char *dirname( char *path );
.funcend
.*
.desc begin
The &func function takes a pointer to a character string that contains
a pathname, and returns a pointer to a string that is a pathname of the
parent directory of that file. Trailing path separators are not considered
as part of the path.
.np
The &func function may modify the string pointed to by
.arg path
and may return a pointer to static storage that may be overwritten by
a subsequent call to &func..
.np
The &func function is not re-entrant or thread-safe.
.desc end
.*
.return begin
The &func function returns a pointer to a string that is the parent
directory of
.arg path
.ct .li .
If
.arg path
is a null pointer or points to an empty string, a pointer to the string "."
is returned.
.return end
.*
.see begin
.seelist &function. basename
.see end
.*
.exmp begin
#include <stdio.h>
#include <libgen.h>

int main( void )
{
.exmp break
    puts( dirname( "/usr/lib" ) );
    puts( dirname( "/usr/" ) );
    puts( dirname( "usr" ) );
    puts( dirname( "/" ) );
    puts( dirname( ".." ) );
    return( 0 );
}
.exmp output
/usr
/
~.
/
~.
.blkcode end
.exmp end
.*
.class POSIX
.system
