pushd
if exist *.sav del /f *.sav
wmake -h global
wmake -h
wmake -h save
rem
if [%extra_arch%] == [] goto no_extra
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
:no_extra
wmake -h global
popd
