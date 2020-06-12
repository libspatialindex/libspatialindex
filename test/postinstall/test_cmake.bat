@echo off

:: Post-install tests with CMake
::
:: First required argument is the installed prefix, which
:: is used to set CMAKE_PREFIX_PATH and PATH
::
:: Second optional argument is CXXSTD, with default 11


echo Running post-install tests with CMake

set prefix=%1
if not defined prefix (
    echo First positional argument to the installed prefix is required
    exit /B 1
)

set PATH=%PATH%;%prefix%\bin

set CXXSTD=%2
if not defined CXXSTD (
    set CXXSTD=11
)


set CC=cl.exe

set CXX=cl.exe

echo %PATH%

dir %prefix%\bin



cd %~dp0

cd test_c
del /f /q build 2> nul
md build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%prefix% .. || exit /B 2
ninja -v || exit /B 3
dumpbin /DEPENDENTS test_c.exe
ctest -V --output-on-failure || exit /B 4
cd ..
del /f /q build
cd ..

cd test_cc
del /f /q build 2> nul
md build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%prefix% -DCMAKE_CXX_STANDARD=%CXXSTD% .. || exit /B 2
ninja -v || exit /B 3
dumpbin /DEPENDENTS test_cc.exe
ctest -V --output-on-failures || exit /B 4
cd ..
del /f /q build
cd ..

