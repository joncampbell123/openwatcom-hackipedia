.gfunc getactivepage
.if '&lang' eq 'C' .do begin
short _FAR _getactivepage( void );
.do end
.el .do begin
integer*2 function _getactivepage()
.do end
.gfuncend
.desc begin
The &func &routine returns the number of the currently selected
active graphics page.
.im gr_pages
.desc end
.return begin
The &func &routine returns the number of the currently selected
active graphics page.
.return end
.see begin
.seelist &function. _setactivepage _setvisualpage _getvisualpage _getvideoconfig
.see end
.grexam begin eg_setap.&langsuff
.grexam end
.class &grfun
.system
