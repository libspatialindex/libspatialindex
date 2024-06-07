#!/bin/bash

set -e
echo "path: " $PATH
printenv

TEST_ROOT=`pwd`

execute_test () {
    dir="$1"
    pushd "$dir"
    echo "Executing test ------------------ $1 -----------------------------"
    ./run
    echo "Executed test ------------------ $1 -----------------------------"
    $TEST_ROOT/cleanup-test-output.sh
    popd
}

execute_test "geometry/test1"
execute_test "rtree/test1"
execute_test "rtree/test2"
execute_test "rtree/test3"
execute_test "rtree/test4"
#execute_test "tprtree/test1"
#execute_test "tprtree/test2"
execute_test "mvrtree/test1"
execute_test "mvrtree/test2"
