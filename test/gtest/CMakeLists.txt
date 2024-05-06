set(GOOGLETEST_VERSION "1.14.0")

add_subdirectory(gtest-1.14.0)
include_directories(./gtest-1.14.0/include)

set(SOURCES
    test.h
    sidx_api_test.h
    main.cc)

set(TESTNAME libsidxtest)
add_executable(${TESTNAME} ${SOURCES})
target_link_libraries(${TESTNAME} PRIVATE spatialindex_c gtest )

add_test(NAME ${TESTNAME}
         COMMAND "${CMAKE_BINARY_DIR}/bin/${TESTNAME}"
         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set_target_properties( ${TESTNAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin)
