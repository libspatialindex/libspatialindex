include_directories(../include)

set (DIR geometry)
set (SOURCES
        Intersection
        )

foreach (test ${SOURCES})
    add_executable(test-${DIR}-${test} ${DIR}/${test}.cc)
    target_link_libraries(test-${DIR}-${test} ${SIDX_LIB_NAME})
endforeach()

set (DIR rtree)
set (SOURCES
        Exhaustive
        Generator
        RTreeBulkLoad
        RTreeLoad
        RTreeQuery)


foreach (test ${SOURCES})
    add_executable(test-${DIR}-${test} ${DIR}/${test}.cc)
    target_link_libraries(test-${DIR}-${test} ${SIDX_LIB_NAME})
endforeach()

set (DIR mvrtree)
set (SOURCES
        Exhaustive
        Generator
        MVRTreeLoad
        MVRTreeQuery
        )


foreach (test ${SOURCES})
    add_executable(test-${DIR}-${test} ${DIR}/${test}.cc)
    target_link_libraries(test-${DIR}-${test} ${SIDX_LIB_NAME})
endforeach()

set (DIR tprtree)
set (SOURCES
        Exhaustive
        Generator
        TPRTreeLoad
        TPRTreeQuery
        )


foreach (test ${SOURCES})
    add_executable(test-${DIR}-${test} ${DIR}/${test}.cc ${DIR}/RandomGenerator.cc)
    target_link_libraries(test-${DIR}-${test} ${SIDX_LIB_NAME})
endforeach()

add_subdirectory(gtest)
