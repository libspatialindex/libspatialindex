#
# top-level CMake configuration file for PDAL
#
# (based originally on the libLAS files copyright Mateusz Loskot)

SET(MSVC_INCREMENTAL_DEFAULT OFF)
cmake_minimum_required(VERSION 3.5.0)
project(spatialindex)

#------------------------------------------------------------------------------
# internal cmake settings
#------------------------------------------------------------------------------

set(CMAKE_COLOR_MAKEFILE ON)

# C++11 required
set (CMAKE_CXX_STANDARD 11)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CMAKE_VERBOSE_MAKEFILE)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

cmake_policy(SET CMP0054 NEW) # Make string comparison behave like you'd expect

if (WIN32)
    if(${CMAKE_VERSION} VERSION_GREATER "3.14.5")
        cmake_policy(SET CMP0092 NEW) # don't put /w3 in flags
    endif()
endif()

if (APPLE)
	set(CMAKE_MACOSX_RPATH ON)
endif (APPLE)

#------------------------------------------------------------------------------
# libspatialindex general settings
#------------------------------------------------------------------------------

SET(SIDX_VERSION_MAJOR "1")
SET(SIDX_VERSION_MINOR "9")
SET(SIDX_VERSION_PATCH "3")
SET(SIDX_LIB_VERSION "6.1.1")
SET(SIDX_LIB_SOVERSION "6")
SET(BUILD_SHARED_LIBS ON)


set(SIDX_VERSION_STRING "${SIDX_VERSION_MAJOR}.${SIDX_VERSION_MINOR}.${SIDX_VERSION_PATCH}")

#------------------------------------------------------------------------------
# libspatialindex general cmake options
#------------------------------------------------------------------------------

option(SIDX_BUILD_TESTS "Enables integrated test suites" OFF)


# Name of C++ library

set(SIDX_LIB_NAME spatialindex)
set(SIDX_C_LIB_NAME spatialindex_c)

if(WIN32)
  if (MSVC)
	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		set( SIDX_LIB_NAME "spatialindex-64" )
		set( SIDX_C_LIB_NAME "spatialindex_c-64" )
	else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		set( SIDX_LIB_NAME "spatialindex-32"  )
		set( SIDX_C_LIB_NAME "spatialindex_c-32"  )
	endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  endif()
endif()


set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

include (CheckFunctionExists)

check_function_exists(srand48 HAVE_SRAND48)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(memcpy HAVE_MEMCPY)
check_function_exists(bcopy HAVE_BCOPY)


INCLUDE (CheckIncludeFiles)


#------------------------------------------------------------------------------
# General build settings
#------------------------------------------------------------------------------

# note we default to RelWithDebInfo mode
if(NOT MSVC_IDE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
  message(STATUS "Setting libspatialindex build type - ${CMAKE_BUILD_TYPE}")
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

if(NOT WIN32)
  # Recommended C++ compilation flags
  set(SIDX_COMMON_CXX_FLAGS
    "-pedantic -Wall -Wpointer-arith -Wcast-align -Wcast-qual  -Wredundant-decls -Wno-long-long")
endif(NOT WIN32)

if (APPLE)
  set(SO_EXT dylib)
  set(CMAKE_FIND_FRAMEWORK "LAST")
elseif(WIN32)
  set(SO_EXT dll)
else()
  set(SO_EXT so)
endif(APPLE)


enable_testing()

#------------------------------------------------------------------------------
# installation path settings
#------------------------------------------------------------------------------

if(WIN32)
  set(DEFAULT_LIB_SUBDIR lib)
  set(DEFAULT_DATA_SUBDIR .)
  set(DEFAULT_INCLUDE_SUBDIR include)

  if (MSVC)
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

if(SIDX_BUILD_TESTS)
  add_subdirectory(test)
endif()

#------------------------------------------------------------------------------
# CPACK controls
#------------------------------------------------------------------------------

SET(CPACK_PACKAGE_VERSION_MAJOR ${SIDX_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${SIDX_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${SIDX_VERSION_MINOR})
SET(CPACK_PACKAGE_NAME "libspatialindex")

SET(CPACK_SOURCE_GENERATOR "TBZ2;TGZ")
SET(CPACK_PACKAGE_VENDOR "libspatialindex Development Team")
SET(CPACK_RESOURCE_FILE_LICENSE    "${PROJECT_SOURCE_DIR}/COPYING")

set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-src-${SIDX_VERSION_STRING}")

set(CPACK_SOURCE_IGNORE_FILES
"/\\\\.gitattributes;/\\\\.vagrant;/\\\\.DS_Store;/CVS/;/\\\\.git/;\\\\.swp$;~$;\\\\.\\\\#;/\\\\#")

list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeScripts/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "_CPack_Packages")
list(APPEND CPACK_SOURCE_IGNORE_FILES "cmake_install.cmake")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/bin/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/scripts/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/azure-pipelines.yml")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".gitignore")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".ninja*")
list(APPEND CPACK_SOURCE_IGNORE_FILES "HOWTORELEASE.txt")

list(APPEND CPACK_SOURCE_IGNORE_FILES "README")
list(APPEND CPACK_SOURCE_IGNORE_FILES "build/")

list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeFiles")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CTestTestfile.cmake")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/docs/build/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/doc/presentations/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "package-release.sh")
list(APPEND CPACK_SOURCE_IGNORE_FILES "docker-package.sh")

list(APPEND CPACK_SOURCE_IGNORE_FILES ".gz2")

list(APPEND CPACK_SOURCE_IGNORE_FILES ".bz2")

include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
