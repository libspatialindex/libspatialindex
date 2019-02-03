#!/bin/bash
# Builds and tests libspatialindex
./autogen.sh

tmstamp() { echo -n "[$(date '+%H:%M:%S')]" ; }

./autogen.sh
CXXFLAGS="-std=c++11" ./configure

make
pushd test/geometry/test1
./run
popd

pushd test/rtree/test1
./run
popd
pushd test/rtree/test2
./run
popd
pushd test/rtree/test3
./run
popd
pushd test/rtree/test4
./run
popd

pushd test/tprtree/test1
./run
popd
pushd test/tprtree/test2
./run
popd

pushd test/mvrtree/test1
./run
popd
pushd test/mvrtree/test2
./run
popd
echo "$(tmstamp) *** finished $(date) ***"
