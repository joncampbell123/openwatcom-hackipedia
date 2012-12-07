@echo off
REM *****************************************************************
REM SETVARS.BAT - Windows NT version
REM *****************************************************************
REM NOTE: Do not use this batch file directly, but copy it and
REM       modify it as necessary for your own use!!

REM Setup environment variables for Perforce
set P4PORT=perforce.openwatcom.org:3488
set P4USER=YourName
set P4CLIENT=YOURCLIENT
set P4PASSWD=YourPassword

REM Change this to point to your Open Watcom source tree - must be an 8.3 name!
set OWROOT=d:\ow

REM Change this to point to your existing Open Watcom installation
set WATCOM=c:\c\ow10

REM Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
set GHOSTSCRIPT=c:\gs\gs7.04\bin;c:\gs\gs7.04\lib

REM Set this variable to 1 to get debug build
set DEBUG_BUILD=0

REM Set this variable to 1 to get default windowing support in clib
set DEFAULT_WINDOWING=0

REM Set this variable to 0 to suppress documentation build
set DOC_BUILD=1

REM Documentation related variables
REM set appropriate variables to blank for help compilers which you haven't installed
set WIN95HC=hcrtf
set OS2HC=ipfc

REM Subdirectory to be used for bootstrapping
set OBJDIR=bootstrp

REM Subdirectory to be used for building prerequisite utilities
set PREOBJDIR=prebuild

REM Invoke the batch file for the common environment
call %OWROOT%\cmnvars.bat

cd %DEVDIR%
