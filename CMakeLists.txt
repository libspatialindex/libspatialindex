#
# top-level CMake configuration file for libspatialindex
#
# (based originally on the PDAL and libLAS files copyright Mateusz Loskot)

cmake_minimum_required(VERSION 3.5)

#------------------------------------------------------------------------------
# libspatialindex general settings
#------------------------------------------------------------------------------

set(SIDX_VERSION_MAJOR "1")
set(SIDX_VERSION_MINOR "9")
set(SIDX_VERSION_PATCH "3")
set(SIDX_VERSION_STRING
  "${SIDX_VERSION_MAJOR}.${SIDX_VERSION_MINOR}.${SIDX_VERSION_PATCH}")
set(SIDX_LIB_VERSION "6.1.1")
set(SIDX_LIB_SOVERSION "6")

# disable incremental linker
set(MSVC_INCREMENTAL_DEFAULT NO)

project(spatialindex
  VERSION ${SIDX_VERSION_STRING}
  LANGUAGES CXX
)

message(STATUS
  "Configuring CMake ${CMAKE_VERSION} to build spatialindex ${PROJECT_VERSION}")

#------------------------------------------------------------------------------
# internal cmake settings
#------------------------------------------------------------------------------

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CMAKE_VERBOSE_MAKEFILE)

# Only interpret if() arguments as variables or keywords when unquoted
cmake_policy(SET CMP0054 NEW)

if(WIN32 AND NOT CMAKE_VERSION VERSION_LESS "3.15")
  # MSVC warning flags are not in CMAKE_<LANG>_FLAGS by default
  cmake_policy(SET CMP0092 NEW)
endif()

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

#------------------------------------------------------------------------------
# libspatialindex general cmake options
#------------------------------------------------------------------------------

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTING "Enables integrated test suites" OFF)

if(DEFINED SIDX_BUILD_TESTS)
  message(DEPRECATION "SIDX_BUILD_TESTS has been replaced with BUILD_TESTING")
  set(BUILD_TESTING ${SIDX_BUILD_TESTS})
endif()
include(CTest)

# C++11 required, overridable by user
set(CMAKE_CXX_STANDARD 11
  CACHE STRING "C++ standard version to use (default is 11)")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Name of C++ library
set(SIDX_LIB_NAME spatialindex)
set(SIDX_C_LIB_NAME spatialindex_c)

if(WIN32 AND MSVC)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SIDX_LIB_NAME "${SIDX_LIB_NAME}-64")
    set(SIDX_C_LIB_NAME "${SIDX_C_LIB_NAME}-64")
  else()
    set(SIDX_LIB_NAME "${SIDX_LIB_NAME}-32")
    set(SIDX_C_LIB_NAME "${SIDX_C_LIB_NAME}-32")
  endif()
endif()


set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

include(CheckFunctionExists)

check_function_exists(srand48 HAVE_SRAND48)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(memcpy HAVE_MEMCPY)
check_function_exists(bcopy HAVE_BCOPY)


include(CheckIncludeFiles)


#------------------------------------------------------------------------------
# General build settings
#------------------------------------------------------------------------------

# note we default to RelWithDebInfo mode
if(NOT MSVC_IDE)
  if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: \
None Debug Release RelWithDebInfo MinSizeRel" FORCE)
  endif()
  message(STATUS
    "Setting libspatialindex build type - ${CMAKE_BUILD_TYPE}")
endif()

set(SIDX_BUILD_TYPE ${CMAKE_BUILD_TYPE})

# TODO: Still testing the output paths --mloskot
set(SIDX_BUILD_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Output directory in which to build RUNTIME target files.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SIDX_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build LIBRARY target files
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SIDX_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build ARCHIVE target files.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SIDX_BUILD_OUTPUT_DIRECTORY})


#------------------------------------------------------------------------------
# Platform and compiler specific settings
#------------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # Recommended C++ compilation flags
  set(SIDX_COMMON_CXX_FLAGS
    -pedantic
    -Wall
    -Wpointer-arith
    -Wcast-align
    -Wcast-qual
    -Wredundant-decls
    -Wno-long-long
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  if(WIN32)
    set(SIDX_COMMON_CXX_FLAGS
      /Wall
      /Qdiag-disable:2203  # cast discards qualifiers from target type
    )
  else()
    set(SIDX_COMMON_CXX_FLAGS
      -Wall
      -Wpointer-arith
      -diag-disable=2203  # cast discards qualifiers from target type
    )
  endif()
endif()

if(APPLE)
  set(SO_EXT dylib)
  set(CMAKE_FIND_FRAMEWORK "LAST")
elseif(WIN32)
  set(SO_EXT dll)
else()
  set(SO_EXT so)
endif()


#------------------------------------------------------------------------------
# installation path settings
#------------------------------------------------------------------------------

if(WIN32)
  set(DEFAULT_LIB_SUBDIR lib)
  set(DEFAULT_DATA_SUBDIR .)
  set(DEFAULT_INCLUDE_SUBDIR include)

  if(MSVC)
    set(DEFAULT_BIN_SUBDIR bin)
  else()
    set(DEFAULT_BIN_SUBDIR .)
  endif()
else()
  # Common locations for Unix and Mac OS X
  set(DEFAULT_BIN_SUBDIR bin)
  set(DEFAULT_LIB_SUBDIR lib${LIB_SUFFIX})
  set(DEFAULT_DATA_SUBDIR share/spatialindex)
  set(DEFAULT_INCLUDE_SUBDIR include)
endif()

# Locations are changeable by user to customize layout of SIDX installation
# (default values are platform-specific)
set(SIDX_BIN_SUBDIR ${DEFAULT_BIN_SUBDIR} CACHE STRING
  "Subdirectory where executables will be installed")
set(SIDX_LIB_SUBDIR ${DEFAULT_LIB_SUBDIR} CACHE STRING
  "Subdirectory where libraries will be installed")
set(SIDX_INCLUDE_SUBDIR ${DEFAULT_INCLUDE_SUBDIR} CACHE STRING
  "Subdirectory where header files will be installed")
set(SIDX_DATA_SUBDIR ${DEFAULT_DATA_SUBDIR} CACHE STRING
  "Subdirectory where data will be installed")

# Mark *_SUBDIR variables as advanced and dedicated to use by power-users only.
mark_as_advanced(SIDX_BIN_SUBDIR
  SIDX_LIB_SUBDIR SIDX_INCLUDE_SUBDIR SIDX_DATA_SUBDIR)

# Full paths for the installation
set(SIDX_BIN_DIR ${SIDX_BIN_SUBDIR})
set(SIDX_LIB_DIR ${SIDX_LIB_SUBDIR})
set(SIDX_INCLUDE_DIR ${SIDX_INCLUDE_SUBDIR})
set(SIDX_DATA_DIR ${SIDX_DATA_SUBDIR})

#------------------------------------------------------------------------------
# subdirectory controls
#------------------------------------------------------------------------------

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

#------------------------------------------------------------------------------
# CMake package configuration for find_package(libspatialindex)
#------------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

set(INCLUDE_INSTALL_DIR ${SIDX_INCLUDE_DIR} CACHE PATH "include directory")
set(LIB_INSTALL_DIR ${SIDX_LIB_DIR} CACHE PATH "lib directory")

configure_package_config_file(
  libspatialindexConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/libspatialindexConfig.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/libspatialindex
  PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/libspatialindexConfigVersion.cmake
  VERSION ${SIDX_VERSION_STRING}
  COMPATIBILITY AnyNewerVersion)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/libspatialindexConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/libspatialindexConfigVersion.cmake
  DESTINATION ${LIB_INSTALL_DIR}/cmake/libspatialindex)

#------------------------------------------------------------------------------
# pkg-config support
#------------------------------------------------------------------------------
if(NOT WIN32)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libspatialindex.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libspatialindex.pc
    @ONLY)

  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libspatialindex.pc
    DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif()

#------------------------------------------------------------------------------
# CPack controls
#------------------------------------------------------------------------------

set(CPACK_PACKAGE_VERSION_MAJOR ${SIDX_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SIDX_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SIDX_VERSION_MINOR})
set(CPACK_PACKAGE_NAME "libspatialindex")

set(CPACK_SOURCE_GENERATOR "TBZ2;TGZ")
set(CPACK_PACKAGE_VENDOR "libspatialindex Development Team")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")

set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-src-${SIDX_VERSION_STRING}")

set(CPACK_SOURCE_IGNORE_FILES
  "/\\\\.gitattributes"
  "/\\\\.vagrant"
  "/\\\\.DS_Store"
  "/CVS/"
  "/\\\\.git/"
  "\\\\.swp$"
  "~$"
  "\\\\.\\\\#"
  "/\\\\#"
  "CMakeScripts/"
  "_CPack_Packages"
  "cmake_install.cmake"
  "/bin/"
  "/scripts/"
  "/azure-pipelines.yml"
  ".gitignore"
  ".ninja*"
  "HOWTORELEASE.txt"
  "README"
  "build/"
  "CMakeFiles"
  "CTestTestfile.cmake"
  "/docs/build/"
  "/doc/presentations/"
  "package-release.sh"
  "docker-package.sh"
  ".gz"
  ".bz2"
)

include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
