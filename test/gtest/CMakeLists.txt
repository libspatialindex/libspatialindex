set (GOOGLETEST_VERSION "1.10.0")

add_subdirectory(gtest-1.10.0)
include_directories(./gtest-1.10.0/include)

set (SOURCES
    test.h
    sidx_api_test.h
    main.cc )

set (TESTNAME libsidxtest)
add_executable(${TESTNAME} ${SOURCES})
target_link_libraries(${TESTNAME} ${SIDX_C_LIB_NAME} gtest)

add_test(${TESTNAME} "${PROJECT_BINARY_DIR}/bin/${TESTNAME}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/..")

