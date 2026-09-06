if (SYSTEM_GTEST)
    find_package(GTest REQUIRED)
    # Link the imported target so that GoogleTest's usage requirements are
    # applied: its include directories, and the C++ standard it needs.
    # GoogleTest 1.15 and newer require C++17, which is higher than the
    # C++11 the rest of this project is built with.
    if (TARGET GTest::gtest)
        set(SIDX_GTEST_TARGET GTest::gtest)
    else()
        # CMake's FindGTest module only defines GTest::gtest since 3.20
        set(SIDX_GTEST_TARGET GTest::GTest)
    endif()
else()
    set(GOOGLETEST_VERSION "1.14.0")
    add_subdirectory(gtest-1.14.0)
    set(SIDX_GTEST_TARGET gtest)
endif()

set(SOURCES
    Geometry.cpp
    MVRTree.cpp
    RTree.cpp
    TPRTree.cpp
    test.h
    NativeTestSupport.h
    sidx_api_test.h
    main.cc)

set(TESTNAME libsidxtest)
add_executable(${TESTNAME} ${SOURCES})
target_link_libraries(${TESTNAME} PRIVATE spatialindex_c ${SIDX_GTEST_TARGET})

add_test(NAME ${TESTNAME}
         COMMAND "${CMAKE_BINARY_DIR}/bin/${TESTNAME}"
         WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set_target_properties( ${TESTNAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin)
