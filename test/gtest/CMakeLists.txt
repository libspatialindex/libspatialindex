

add_subdirectory(gtest-1.7.0)
include_directories(./gtest-1.7.0/include)

set (SOURCES
    test.h
    sidx_api_test.h
    main.cc )



add_executable(libsidxtest ${SOURCES})
target_link_libraries(libsidxtest ${SIDX_C_LIB_NAME} gtest)

