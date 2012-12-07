.gfuncwxy getviewcoord
.if '&lang' eq 'C' .do begin
struct xycoord _FAR _getviewcoord( short x, short y );

struct xycoord _FAR _getviewcoord_w( double x, double y );

struct xycoord _FAR _getviewcoord_wxy(
                    struct _wxycoord _FAR *p );
.do end
.el .do begin
record /xycoord/ function _getviewcoord( x, y )
integer*2 x, y

record /xycoord/ function _getviewcoord_w( x, y )
double precision x, y

record /xycoord/ function _getviewcoord_wxy( p )
record /_wxycoord/ p
.do end
.gfuncend
.desc begin
The &func &routines translate a point
from one coordinate system to viewport coordinates.
The &func &routine translates the point
.coord x y
from physical coordinates.
The &func2 and &func3 &routines translate the point from the
window coordinate system.
.np
Viewport coordinates are defined by the
.kw _setvieworg
and
.kw _setviewport
&routines..
Window coordinates are defined by the
.kw _setwindow
&routine..
.if '&lang' eq 'C' .do begin
.np
.bd Note:
In previous versions of the software, the &func &routine was called
.mono _getlogcoord.
.uindex getlogcoord
.do end
.desc end
.return begin
The &func &routines return the viewport coordinates,
as an
.id xycoord
structure,
of the given point.
.return end
.see begin
.seelist &function. _getphyscoord _setvieworg _setviewport _setwindow
.see end
.grexam begin eg_getlc.&langsuff
.grexam end
.class &grfun
.system
