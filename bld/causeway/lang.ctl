# Causeway Builder Control file
# =============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <PROJDIR>/cwc.exe                 <RELROOT>/binw/
    <CPCMD> <PROJDIR>/cw32/dosi86/cwstub.exe  <RELROOT>/binw/
    <CPCMD> <PROJDIR>/cw32/dos386/cwdll.lib   <RELROOT>/lib386/dos/
    <CPCMD> <PROJDIR>/inc/cwdllfnc.h          <RELROOT>/h/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
