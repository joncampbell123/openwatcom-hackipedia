#pmake: build lcl os_rdos cpu_386 trap x86

host_os  = rdos
host_cpu = 386

#
# Create the RDOS STD.DLL
#

os      = rdos
srv     = lcl
cpu     = x86
which = TRAP

extra_objs = &
        x86cpu.obj &
        task.obj &
        globals.obj &
        debug.obj &
        accmisc.obj &
        accload.obj &
        accio.obj &
        accfinfo.obj &
        accenv.obj &
        accbrwat.obj &
        accrun.obj &
        accregs.obj &
        accmap.obj &
        accredir.obj &
        accmem.obj &
        accrdos.obj &
        thread.obj &
        supphdlr.obj &
        supp.obj &
        core.obj

inc_dirs += -I"$(trap_dir)/lcl/dos/h" -I"$(clib_dir)/h"

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif

globals.obj :: ../h/globals.h

