# wlib Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<OBJDIR>
    rm -f -r <PROJDIR>/<OBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the wlib bootstrap
    mkdir <PROJDIR>/<OBJDIR>
    cdsay <PROJDIR>/<OBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> wlib.exe <DEVDIR>/build/bin/wlib
    <CPCMD> wlib.exe <DEVDIR>/build/bin/bwlib
    <CPCMD> wlib.exe <DEVDIR>/build/binl/bwlib
    cdsay <PROJDIR>
