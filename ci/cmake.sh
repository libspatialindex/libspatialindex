#!/bin/bash


cmake   -G "Ninja"  \
      -DCMAKE_BUILD_TYPE=Debug \
      -DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS \
      -DBUILD_TESTING=ON \
      -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} \
      ..
