cmake_minimum_required(VERSION 3.5)
project(test_c LANGUAGES C)


find_package(libspatialindex)

# Show some target properties
get_cmake_property(_variableNames VARIABLES)
list(SORT _variableNames)
foreach(_variableName ${_variableNames})
  string(REGEX MATCH "^SIDX_" _matched ${_variableName})
  if(NOT ${_matched} STREQUAL "")
    message(STATUS "${_variableName}=${${_variableName}}")
  endif()
endforeach()

add_executable(test_c test_c.c)
target_link_libraries(test_c ${SIDX_LIBRARIES})

include(CTest)

if(APPLE)
  set(LDD_CL "otool -L")
  # set(EXPECTED_LDD_CL_OUT "@rpath/libspatialindex")
  set(EXPECTED_LDD_CL_OUT "${CMAKE_PREFIX_PATH}/lib/libspatialindex")
elseif(UNIX)
  set(LDD_CL "ldd")
  set(EXPECTED_LDD_CL_OUT "${CMAKE_PREFIX_PATH}/lib/libspatialindex")
endif()

if(LDD_CL)
  add_test(NAME test_ldd
    COMMAND sh -c "${LDD_CL} ${CMAKE_BINARY_DIR}/test_c | grep libspatialindex")
  set_tests_properties(test_ldd PROPERTIES
    PASS_REGULAR_EXPRESSION ${EXPECTED_LDD_CL_OUT}
  )
else()
  # Dummy test
  add_test(NAME test_ldd COMMAND test_c)
  set_tests_properties(test_ldd PROPERTIES SKIP_RETURN_CODE 0)
endif()

add_test(NAME test_version COMMAND test_c)
set(SIDX_VERSION_STRING
  "${SIDX_VERSION_MAJOR}.${SIDX_VERSION_MINOR}.${SIDX_VERSION_PATCH}")
set_tests_properties(test_version PROPERTIES
  PASS_REGULAR_EXPRESSION ${SIDX_VERSION_STRING}
)
