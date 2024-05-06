#!/bin/bash

set -e
echo "path: " $PATH
printenv

pushd geometry/test1
./run
popd
pushd rtree/test1
./run
popd
pushd rtree/test2
./run
popd
pushd rtree/test3
./run
popd
pushd rtree/test4
./run
popd
pushd tprtree/test1
./run
popd
pushd tprtree/test2
./run
popd
pushd mvrtree/test1
./run
popd
pushd mvrtree/test2
./run
popd
