include_directories(../include)

set(DIR geometry)
set(SOURCES
        Intersection
        )

foreach(test ${SOURCES})
    add_executable(test-${DIR}-${test} ${DIR}/${test}.cc)
    target_link_libraries(test-${DIR}-${test} PRIVATE spatialindex)
endforeach()

set(DIR rtree)
set(SOURCES
        Exhaustive
        Generator
        RTreeBulkLoad
        RTreeLoad
        RTreeQuery)


foreach(test ${SOURCES})
    add_executable(test-${DIR}-${test} ${DIR}/${test}.cc)
    target_link_libraries(test-${DIR}-${test} PRIVATE spatialindex)
endforeach()

set(DIR mvrtree)
set(SOURCES
        Exhaustive
        Generator
        MVRTreeLoad
        MVRTreeQuery
        )


foreach(test ${SOURCES})
    add_executable(test-${DIR}-${test} ${DIR}/${test}.cc)
    target_link_libraries(test-${DIR}-${test} PRIVATE spatialindex)
endforeach()

set(DIR tools)
set(SOURCES
        TemporaryFile
        )


foreach(test ${SOURCES})
    add_executable(test-${DIR}-${test} ${DIR}/${test}.cc)
    target_link_libraries(test-${DIR}-${test} PRIVATE spatialindex)
endforeach()

set(DIR tprtree)
set(SOURCES
        Exhaustive
        Generator
        TPRTreeLoad
        TPRTreeQuery
        )


foreach(test ${SOURCES})
    add_executable(test-${DIR}-${test}
        ${DIR}/${test}.cc ${DIR}/RandomGenerator.cc)
    target_link_libraries(test-${DIR}-${test} PRIVATE spatialindex)

endforeach()

if (UNIX)
    add_test(NAME index-tests
             COMMAND /bin/sh "${CMAKE_CURRENT_SOURCE_DIR}/index-tests.sh"
             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_property(TEST index-tests PROPERTY ENVIRONMENT
             "PATH=${CMAKE_BINARY_DIR}/test/:$ENV{PATH}")
endif (UNIX)
add_subdirectory(gtest)
