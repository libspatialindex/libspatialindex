#!/bin/sh

#  Post-install tests with pkg-config
#
#  First required argument is the installed prefix, which
#  is used to set PKG_CONFIG_PATH and LD_LIBRARY_PATH

echo Running post-install tests with pkg-config

prefix=$1
if [ -z "$prefix" ]; then
    echo First positional argument to the the installed prefix is required
    exit 1
fi

export PKG_CONFIG_PATH=$prefix/lib/pkgconfig
export LD_LIBRARY_PATH=$prefix/lib

# Run tests from shell, count any errors
ERRORS=0
NTESTS=0

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

check_ldd(){
  NTESTS=$(($NTESTS + 1))
  printf "Testing expected ldd output ... "
  LDD_OUTPUT=$(ldd ./$1 | grep libspatialindex)
  LDD_SUBSTR=$LD_LIBRARY_PATH/libspatialindex.
  case "$LDD_OUTPUT" in
    *$LDD_SUBSTR*)
      echo "passed" ;;
    *)
      ERRORS=$(($ERRORS + 1))
      echo "failed: ldd output '$LDD_OUTPUT' does not contain '$LDD_SUBSTR'" ;;
  esac
}

cd $(dirname $0)


cd test_c
make clean
make

check_ldd test_c

printf "Testing expected version ... "
NTESTS=$(($NTESTS + 1))
VERSION_OUTPUT=$(./test_c)
PKG_CONFIG_MODVERSION=$(pkg-config libspatialindex --modversion)
case "$VERSION_OUTPUT" in
  $PKG_CONFIG_MODVERSION)
    echo "passed" ;;
  *)
    ERRORS=$(($ERRORS + 1))
    echo "failed: '$VERSION_OUTPUT' != '$PKG_CONFIG_MODVERSION'" ;;
esac

make clean
cd ..


cd test_cc
make clean
make

check_ldd test_cc

printf "Testing expected outputs ... "
NTESTS=$(($NTESTS + 1))
OUTPUT=$(./test_cc)
case "$OUTPUT" in
  "done")
    echo "passed" ;;
  *)
    ERRORS=$(($ERRORS + 1))
    echo "failed: '$OUTPUT' != 'done'" ;;
esac

make clean
cd ..

echo "$ERRORS tests failed out of $NTESTS"
exit $ERRORS
