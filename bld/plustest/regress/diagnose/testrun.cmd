pushd
if exist *.sav del *.sav
wmake -h global
wmake -h
wmake -h save
wmake -h global
if [%extra_arch%] == [] goto no_extra
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
wmake -h global
:no_extra
popd