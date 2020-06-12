#!/bin/sh

#  Post-install tests with CMake
#
#  First required argument is the installed prefix, which
#  is used to set CMAKE_PREFIX_PATH and LD_LIBRARY_PATH
#
#  Second optional argument is CXXSTD, with default 11

set -e
echo Running post-install tests with CMake

prefix=$1
if [ -z "$prefix" ]; then
    echo First positional argument to the installed prefix is required
    exit 1
fi

export LD_LIBRARY_PATH=$prefix/lib

CXXSTD=$2
if [ -z "$CXXSTD" ]; then
    CXXSTD=11
fi

UNAME=$(uname)
case $UNAME in
  Darwin*)
    alias ldd="otool -L" ;;
  Linux*)
    ;;
  *)
    echo "no ldd equivalent found for UNAME=$UNAME"
    exit 1 ;;
esac

cd $(dirname $0)

cd test_c
rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$prefix ..
VERBOSE=1 make
ldd ./test_c
ctest -V --output-on-failure
cd ..
rm -rf build
cd ..

cd test_cc
rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$prefix -DCMAKE_CXX_STANDARD=$CXXSTD ..
VERBOSE=1 make
ldd ./test_cc
ctest -V --output-on-failure
cd ..
rm -rf build
cd ..

