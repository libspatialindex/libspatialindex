del /s /q build

mkdir build
cd build
cmake -G "Ninja" ^
	-DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF ^
	-DBUILD_SHARED_LIBS=ON ^
	-DBUILD_TESTING=ON ^
	-DCMAKE_INSTALL_PREFIX=%CONDA_PREFIX\Library ^
	..
