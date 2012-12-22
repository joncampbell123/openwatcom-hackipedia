#!/bin/bash
#
# Build script to bootstrap build the Linux version of the compiler
# using the GNU C/C++ compiler tools. If you already have a working
# Open Watcom compiler, you do not need to use the bootstrap process

if [[ !( -f setvars.sh ) ]]; then
	echo setvars.sh not found, create it
fi

. setvars.sh

if [ ! -f $DEVDIR/build/binl/wtouch ]; then
    cp -p `which touch` $DEVDIR/build/binl/wtouch
fi

mkdir -p bld/w32api/win32s/{directx,ddk}

cd bld/wmake
$MAKE -f gnumake
mkdir -p ../builder/$OBJDIR
cd ../builder/$OBJDIR
wmake -h -f ../linux386/makefile builder.exe bootstrap=1
cd ../..
export BUILDMODE=bootstrap
builder rel2 os_linux || true # <- yes, this will eventually fail. their "builder" is doing a terrible job of dependency tracking

die() {
	echo Failed: $*
	sleep 2
	exit 1
}

BLD=`pwd`
# and now the rest of the build process is poking, prodding, and pushing their code to compile
(cd $BLD/os2api && builder rel2) || die "os2api"
(cd $BLD/w32api && builder rel2) || die "w32api"
(cd $BLD/w32api.old && builder rel2) || die "w32api.old"
(cd $BLD/w16api && builder rel2) || die "w16api"
(cd $BLD/wclass && builder rel2) || die "wclass"
(cd $BLD/clib && builder rel2) || die "clib"
(cd $BLD/emu86 && builder rel2) || die "emu86"
(cd $BLD/emu && builder rel2) || die "emu"
(cd $BLD/w32api && builder rel2 windows) || die "w32api windows"
(cd $BLD/clib && builder rel2 os_nt) || die "clib os_nt"
(cd $BLD/clib && builder rel2 os_os2) || die "clib os_os2"
(cd $BLD/clib && builder rel2 os_dos) || die "clib os_dos"
(cd $BLD/clib && builder rel2 windows) || die "clib windows"
(cd $BLD/clib && builder rel2 os_win32s) || die "clib os_win32s"
(cd $BLD/wpi && builder rel2) || die "clib wpi"
(cd $BLD/gui && builder rel2) || die "clib gui"
(cd $BLD/mathlib && builder rel2) || die "clib mathlib"
(cd $BLD/cg && builder rel2) || die "clib cg"
(cd $BLD/dwarf && builder rel2) || die "clib dwarf"
(cd $BLD/cpp && builder rel2) || die "clib cpp"
(cd $BLD/plusplus/cpplib && builder rel2) || die "clib plusplus/cpplib"
(cd $BLD/rtdll && builder rel2 os_nt) || die "clib rtdll os_nt"
(cd $BLD/plusplus && builder rel2 os_nt) || die "clib plusplus os_nt"
(cd $BLD/rtdll && builder rel2) || die "clib rtdll"
(cd $BLD/plusplus && builder rel2) || die "clib plusplus"
(cd $BLD/plusplus && builder rel2 os_dos) || die "clib plusplus os_dos"
(cd $BLD/plusplus && builder rel2 os_os2) || die "clib plusplus os_os2"
(cd $BLD/plusplus/cpplib && builder rel2) || die "clib plusplus/cpplib"
(cd $BLD/rcsdll && builder rel2) || die "clib rcsdll"
(cd $BLD/sdk && builder rel2) || die "clib sdk"
(cd $BLD/cc && builder rel2) || die "clib cc"
(cd $BLD/as && builder rel2) || die "clib as"
(cd $BLD/win386 && builder rel2) || die "clib win386"
(cd $BLD/wmake && builder rel2) || die "clib wmake"
(cd $BLD/wasm && builder rel2) || die "clib wasm"
(cd $BLD/wstrip && builder rel2) || die "clib wstrip"
(cd $BLD/misc && builder rel2) || die "clib misc"
(cd $BLD/causeway && builder rel2) || die "clib causeway"
(cd $BLD/w32loadr && builder rel2) || die "clib w32loadr"
(cd $BLD/nwlib && builder rel2) || die "clib nwlib"
(cd $BLD/editdll && builder rel2) || die "clib editdll"
(cd $BLD/wstub && builder rel2) || die "clib wstub"
(cd $BLD/wstuba && builder rel2) || die "clib wstuba"
(cd $BLD/orl && builder rel2) || die "clib orl"
(cd $BLD/wl && builder rel2) || die "clib wl"
(cd $BLD/cvpack && builder rel2) || die "clib cvpack"
(cd $BLD/cc/wcl && builder rel2) || die "clib cc/wcl"
(cd $BLD/../contrib/wattcp/src && wmake -ms) || die "clib wattcp"
#(cd $BLD/viper && builder rel2) || die "clib viper"
builder rel2 || die "main 2nd"

# done
unset BUILDMODE
