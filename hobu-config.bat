@echo off

set G="NMake Makefiles"
REM set G="Visual Studio 9 2008"
REM set G="Visual Studio 10"
set LIBLAS=c:\dev\\liblas
set OSGEO4W=C:\OSGeo4W64
set BOOST=C:\OSGeo4W64\boost-1_56
set LASZIP_ROOT=%OSGEO4W%
REM set BUILD_TYPE=RelWithDebInfo
REM set BUILD_TYPE=Debug
set BUILD_TYPE=Release

set PATH=%OSGEO4W%\bin;%PATH%

del CMakeCache.txt
cmake -G %G% ^
	-DCMAKE_BUILD_TYPE=Release  ^
	-DCMAKE_INSTALL_PREFIX=C:\dev\libspatialindex\build
	.
