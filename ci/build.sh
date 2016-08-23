#!/bin/bash
# Builds and tests libLAS
mkdir -p _build
cd _build

tmstamp() { echo -n "[$(date '+%H:%M:%S')]" ; }

echo "$(tmstamp) *** script::cmake-config starting $(date) ***"
cmake .. -DCMAKE_BUILD_TYPE=Debug
echo "$(tmstamp) *** script::cmake-config finished $(date) ***"

echo "$(tmstamp) *** script::cmake-build make $(date) ***"
make
echo "$(tmstamp) *** script::cmake-build finished $(date) ***"

echo "$(tmstamp) *** script::cmake-test starting $(date) ***"
ctest
echo "$(tmstamp) *** script::cmake-test finished $(date) ***"
