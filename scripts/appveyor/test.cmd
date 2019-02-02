pushd build

set PYTHONHOME=C:\\Miniconda3-x64\
set PYTHONPATH=C:\\Miniconda3-x64\\Lib
cd "c:\projects\libspatialindex\build"
ctest -V --output-on-failure
echo %CD%

popd
