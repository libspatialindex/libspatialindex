#!/bin/bash

ls `pwd`/bin
ls `pwd`/test
ls `pwd`/test/gtest
env

ctest -VV --rerun-failed --output-on-failure
