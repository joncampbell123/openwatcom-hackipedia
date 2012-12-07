# WIC Builder Control file
# ========================

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
    <CPCMD> <DEVDIR>/wic/dos386/wic.exe <RELROOT>/binw/wic.exe
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <DEVDIR>/wic/os2386/wic.exe <RELROOT>/binp/wic.exe
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/wic/nt386/wic.exe <RELROOT>/binnt/wic.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
