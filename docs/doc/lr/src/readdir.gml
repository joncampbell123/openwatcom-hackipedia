.func readdir _wreaddir _ureaddir
#include <&dirhdr>
.if '&machsys' eq 'QNX' .do begin
struct dirent *readdir( DIR *dirp );
.do end
.el .do begin
struct dirent *readdir( struct dirent *dirp );
.do end
.ixfunc2 '&Direct' &func
.if &'length(&wfunc.) ne 0 .do begin
struct _wdirent *_wreaddir( _wdirent *dirp );
.ixfunc2 '&Direct' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
struct udirent *_ureaddir( UDIR *dirp );
.ixfunc2 '&Direct' &ufunc
.do end
.funcend
.desc begin
The &func function obtains information about the next matching file
name from the argument
.arg dirp
.ct .li .
The argument
.arg dirp
is the value returned from the
.kw opendir
function.
The &func function can be called repeatedly to obtain the list of file
names contained in the directory specified by the pathname given to
.kw opendir
.ct .li .
The function
.kw closedir
must be called to close the directory and free the memory allocated by
.kw opendir
.ct .li .
.im dirent
.if '&machsys' eq 'QNX' .do begin
.np
The result of using a directory stream after one of the
.kw exec
or
.kw spawn
family of functions is undefined.
After a call to the
.kw fork
function, either the parent or the child (but not both) may continue
processing the directory stream using
.kw readdir
or
.kw rewinddir
or both.
If both the parent and child processes use these functions, the result
is undefined.
Either or both processes may use
.kw closedir
.ct .li .
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it reads a
directory of wide-character filenames.
.im _wdirent
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it reads
a directory of Unicode filenames.
.im udirent
.do end
.desc end
.return begin
When successful, &func returns a pointer to an object of type
.us struct dirent.
When an error occurs, &func returns the value
.mono NULL
and
.kw errno
is set to indicate the error.
When the end of the directory is encountered, &func returns the value
.mono NULL
and
.kw errno
is unchanged.
.if &'length(&wfunc.) ne 0 .do begin
.np
When successful, &wfunc returns a pointer to an object of type
.us struct _wdirent.
When an error occurs, &wfunc returns the value
.mono NULL
and
.kw errno
is set to indicate the error.
When the end of the directory is encountered, &wfunc returns the value
.mono NULL
and
.kw errno
is unchanged.
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
When successful, &ufunc returns a pointer to an object of type
.us struct udirent.
When an error occurs, &ufunc returns the value
.mono NULL
and
.kw errno
is set to indicate the error.
When the end of the directory is encountered, &ufunc returns the value
.mono NULL
and
.kw errno
is unchanged.
.do end
.return end
.error begin
.begterm 12
.term EBADF
The argument
.arg dirp
does not refer to an open directory stream.
.endterm
.error end
.see begin
.im seeiodir readdir
.see end
.exmp begin
.blktext begin
To get a list of files contained in the directory
.if '&machsys' eq 'PP' .do begin
.filename \\foo\fred
of your computer:
.do end
.el .if '&machsys' eq 'QNX' .do begin
.filename /home/fred
of your node:
.do end
.el .do begin
.filename \watcom\h
on your default disk:
.do end
.blktext end
.blkcode begin
#include <stdio.h>
#include <&dirhdr>
.if '&machsys' ne 'QNX' .do begin

typedef struct {
    unsigned short  twosecs : 5;    /* seconds / 2 */
    unsigned short  minutes : 6;
    unsigned short  hours   : 5;
} ftime_t;

typedef struct {
    unsigned short  day     : 5;
    unsigned short  month   : 4;
    unsigned short  year    : 7;
} fdate_t;
.do end

void main()
  {
    DIR *dirp;
    struct dirent *direntp;
.if '&machsys' ne 'QNX' .do begin
    ftime_t *f_time;
    fdate_t *f_date;
.do end
.exmp break
.if '&machsys' eq 'PP' .do begin
    dirp = opendir( "\\\\foo\\fred" );
.do end
.el .if '&machsys' eq 'QNX' .do begin
    dirp = opendir( "/home/fred" );
.do end
.el .do begin
    dirp = opendir( "\\watcom\\h" );
.do end
    if( dirp != NULL ) {
      for(;;) {
        direntp = readdir( dirp );
        if( direntp == NULL ) break;
.if '&machsys' eq 'QNX' .do begin
        printf( "%s\n", direntp->d_name );
.do end
.el .do begin
        f_time = (ftime_t *)&direntp->d_time;
        f_date = (fdate_t *)&direntp->d_date;
        printf( "%-12s %d/%2.2d/%2.2d "
                "%2.2d:%2.2d:%2.2d \n",
            direntp->d_name,
            f_date->year + 1980,
            f_date->month,
            f_date->day,
            f_time->hours,
            f_time->minutes,
            f_time->twosecs * 2 );
.do end
      }
      closedir( dirp );
    }
  }
.blkcode end
.im dblslash
.exmp end
.class POSIX 1003.1
.system
