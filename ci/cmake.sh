#!/bin/bash


if [ "$RUNNER_OS" == "Windows" ]; then
export CC=cl.exe
export CXX=cl.exe
fi



cmake   -G "Ninja"  \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS \
      -DBUILD_TESTING=ON \
      -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} \
      -Dgtest_force_shared_crt=ON \
      ..
