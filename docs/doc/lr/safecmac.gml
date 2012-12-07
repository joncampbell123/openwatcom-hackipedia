.*
.* Safer C Library macros
.*
.* NB: The setsfunc macro and resulting func_s symbol are only meaningful
.* in the documentation of the 'unsafe' function.
.dm setsfunc begin
.   :set symbol=function_s value="&function._s".
.   :set symbol="func_s" value=";.sf4 &function_s.;.esf ".
.dm setsfunc end
.*
.dm rtconst begin
.if '&*' eq 'begin' .do begin
.newtext Constraints:
If any of the following runtime-constraints is violated, the currently
active runtime-constraint handler will be invoked and &func will return
a non-zero value to indicate an error, or the runtime-constraint handler
aborts the program.
.np
.do end
.el .if '&*' eq 'end' .oldtext
.el .if '&*' eq 'freetext' .do begin
.newtext Constraints:
.do end
.el .if '&*' eq 'none' .do begin
.newtext Constraints:
None.
.np
.oldtext
.do end
.dm rtconst end
.*
.dm saferet begin
The &func function returns zero if there was no runtime-constraint violation.
Otherwise, a non-zero value is returned.
.dm saferet end
.*
.dm safehint begin
.newtext Safer C:
The Safer C Library provides functions with extended parameter checking.
It should be used in preference to the 'unsafe' functions.
See xxx for a list of available functions.
.oldtext
.dm safehint end
.*
.dm safealt begin
.newtext Safer C:
.setsfunc
The Safer C Library extension provides the
.seekw &function &function_s
function which is a safer alternative to &func.. This newer &func_s function
is recommended to be used instead of the traditional "unsafe" &func function.
.oldtext
.dm safealt end
.*
