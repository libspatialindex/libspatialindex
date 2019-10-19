del /s /q build

mkdir build
cd build
cmake -G "Ninja" ^
	-DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF ^
	-DBUILD_SHARED_LIBS=ON ^
	..