.func setenv _setenv _wsetenv _usetenv
#include <env.h>
int setenv( const char *name,
            const char *newvalue,
            int overwrite );
.ixfunc2 '&Process' &func
.if &'length(&_func.) ne 0 .do begin
int _setenv( const char *name,
             const char *newvalue,
             int overwrite );
.ixfunc2 '&Process' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _wsetenv( const wchar_t *name,
              const wchar_t *newvalue,
              int overwrite );
.ixfunc2 '&Process' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _usetenv( const wchar_t *name,
            const wchar_t *newvalue ,
            int overwrite );
.ixfunc2 '&Process' &ufunc
.do end
.funcend
.desc begin
.ix 'environment'
The environment list consists of a number of environment names,
each of which has a value associated with it.
Entries can be added to the environment list
.if '&machsys' eq 'PP' .do begin
in the
.mono environ.ini
file.
.do end
.el .if '&machsys' eq 'QNX' .do begin
with the
.qnxcmd export
or with the &func function.
.do end
.el .do begin
with the
.doscmd set
or with the &func function.
.do end
.if '&machsys' eq 'PP' .do begin
.do end
.el .if '&machsys' eq 'QNX' .do begin
.ix 'environment'
All entries in the environment list can be displayed by using the
.qnxcmd export
with no arguments.
.do end
.el .do begin
.ix 'environment'
All entries in the environment list can be displayed by using the
.doscmd set
with no arguments.
.do end
A program can obtain the value for an environment variable by using
the
.kw getenv
function.
.np
The &func function searches the environment list for an entry of the
form
.arg name=value
.ct .li .
If no such string is present, &func adds an entry of the form
.arg name=newvalue
to the environment list.
Otherwise, if the
.arg overwrite
argument is non-zero, &func either will change the existing value to
.arg newvalue
or will delete the string
.arg name=value
and add the string
.arg name=newvalue
.ct .li .
.np
If the
.arg newvalue
pointer is NULL, all strings of the form
.arg name=value
in the environment list will be deleted.
.if '&machsys' ne 'PP' .do begin
.np
The value of the pointer
.kw environ
may change across a call to the &func function.
.do end
.np
The &func function will make copies of the strings associated with
.arg name
and
.arg newvalue
.ct .li .
.np
.if '&machsys' eq 'QNX' .do begin
The matching is case-sensitive; all lowercase letters are treated
as different from uppercase letters.
.do end
.el .do begin
The matching is case-insensitive; all lowercase letters are treated
as if they were in upper case.
.do end
.np
Entries can also be added to the environment list
.if '&machsys' eq 'PP' .do begin
in the
.mono environ.ini
file.
.do end
.el .if '&machsys' eq 'QNX' .do begin
with the
.qnxcmd export
or with the
.kw putenv
or &func functions.
.do end
.el .do begin
with the
.doscmd set
or with the
.kw putenv
or &func functions.
.do end
.ix 'environment'
All entries in the environment list can be obtained by using the
.kw getenv
function.
.np
To assign a string to a variable and place it in the environment list:
.millust begin
.if '&machsys' eq 'PP' .do begin
    TZ=PST8PDT
.do end
.el .if '&machsys' eq 'QNX' .do begin
    % export INCLUDE=/usr/include
.do end
.el .do begin
    C>SET INCLUDE=C:\WATCOM\H
.do end
.millust end
.if '&machsys' eq 'PP' .do begin
.do end
.el .if '&machsys' eq 'QNX' .do begin
.np
To see what variables are in the environment list, and their current
assignments:
.millust begin
    % export
    SHELL=ksh
    TERM=qnx
    LOGNAME=fred
    PATH=:/bin:/usr/bin
    HOME=/home/fred
    INCLUDE=/usr/include
    LIB=/usr/lib
    %
.millust end
.do end
.el .do begin
.np
To see what variables are in the environment list, and their current
assignments:
.millust begin
    C>SET
    COMSPEC=C:\COMMAND.COM
    PATH=C:\;C:\WATCOM
    INCLUDE=C:\WATCOM\H
    C>
.millust end
.do end
.if &'length(&_func.) ne 0 .do begin
.np
The &_func function is identical to &func..
Use &_func for ANSI naming conventions.
.do end
.im widefunc
.im unifunc
.desc end
.return begin
The &func function returns zero upon successful completion.
Otherwise, it will return a non-zero value and set
.kw errno
to indicate the error.
.return end
.error begin
.begterm 12
.term ENOMEM
Not enough memory to allocate a new environment string.
.endterm
.error end
.see begin
.im seeenv setenv
.see end
.exmp begin
.blktext begin
The following will change the string assigned to
.kw INCLUDE
and then display the new string.
.blktext end
.blkcode begin
#include <stdio.h>
#include <stdlib.h>
#include <env.h>

void main()
  {
    char *path;
.exmp break
.if '&machsys' eq 'PP' .do begin
    if( setenv( "INCLUDE",
                "\\usr\\include;\\usr\\fred\\include",
                1
              ) == 0 )
.do end
.el .if '&machsys' eq 'QNX' .do begin
    if( setenv( "INCLUDE",
                "/usr/include:/home/fred/include",
                1
              ) == 0 )
.do end
.el .do begin
    if( setenv( "INCLUDE", "D:\\WATCOM\\H", 1 ) == 0 )
.do end
      if( (path = getenv( "INCLUDE" )) != NULL )
        printf( "INCLUDE=%s\n", path );
  }
.blkcode end
.exmp end
.class WATCOM
.system
