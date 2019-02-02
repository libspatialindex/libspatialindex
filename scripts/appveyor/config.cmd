@echo off

SET CMAKE_VERBOSE_MAKEFILE=OFF
SET CMAKE_BUILD_TYPE=RelWithDebInfo

mkdir build
pushd build


cmake -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE% ^
    -DCMAKE_VERBOSE_MAKEFILE=%CMAKE_VERBOSE_MAKEFILE% ^
    -DCMAKE_LIBRARY_PATH:FILEPATH="=%CONDA_ROOT%/Library/lib" ^
    -DCMAKE_INCLUDE_PATH:FILEPATH="%CONDA_ROOT%/Library/include" ^
    -Dgtest_force_shared_crt=ON ^
    ..

popd

