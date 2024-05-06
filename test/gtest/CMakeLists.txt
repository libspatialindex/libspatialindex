set(GOOGLETEST_VERSION "1.10.0")

add_subdirectory(gtest-1.10.0)
include_directories(./gtest-1.10.0/include)

set(SOURCES
    test.h
    sidx_api_test.h
    main.cc)

set(TESTNAME libsidxtest)
add_executable(${TESTNAME} ${SOURCES})
target_link_libraries(${TESTNAME} PRIVATE spatialindex_c gtest)

add_test(NAME ${TESTNAME}
         COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME}"
         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


if (WIN32)
  set_property(TEST libsidxtest PROPERTY ENVIRONMENT
    "PATH=$<SHELL_PATH:${CMAKE_BINARY_DIR}>\\bin;$ENV{PATH}")

endif(WIN32)