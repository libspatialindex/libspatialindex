#
# top-level CMake configuration file for PDAL
#
# (based originally on the libLAS files copyright Mateusz Loskot)

cmake_minimum_required(VERSION 2.6.0)
project(spatialindex)

#------------------------------------------------------------------------------
# internal cmake settings
#------------------------------------------------------------------------------

set(CMAKE_COLOR_MAKEFILE ON)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CMAKE_VERBOSE_MAKEFILE)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})


#------------------------------------------------------------------------------
# libspatialindex general settings
#------------------------------------------------------------------------------

SET(SIDX_VERSION_MAJOR "1")
SET(SIDX_VERSION_MINOR "8")
SET(SIDX_VERSION_PATCH "1")
SET(SIDX_LIB_SOVERSION "3.0.0")


set(SIDX_VERSION_STRING "${SIDX_VERSION_MAJOR}.${SIDX_VERSION_MINOR}.${SIDX_VERSION_PATCH}")


# Name of C++ library

set(SIDX_LIB_NAME spatialindex)
set(SIDX_C_LIB_NAME spatialindex_c)

if(WIN32)
  if (MSVC)
    if (MSVC80 OR MSVC90 OR MSVC10)
        if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
            set( SIDX_LIB_NAME "spatialindex-64" )
            set( SIDX_C_LIB_NAME "spatialindex_c-64" )
        else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
            set( SIDX_LIB_NAME "spatialindex-32"  )
            set( SIDX_C_LIB_NAME "spatialindex_c-32"  )
        endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    endif()
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
check_include_files(pthread.h HAVE_PTHREAD_H) 


if (HAVE_SRAND48)
    add_definitions(-DHAVE_SRAND48=1)
endif()

if (HAVE_GETTIMEOFDAY)
    add_definitions(-DHAVE_GETTIMEOFDAY=1)
endif()

if (HAVE_BZERO)
    add_definitions(-DHAVE_BZERO=1)
endif()

if (HAVE_MEMSET)
    add_definitions(-DHAVE_MEMSET=1)
endif()

if (HAVE_MEMCPY)
    add_definitions(-DHAVE_MEMCPY=1)
endif()

if (HAVE_BCOPY)
    add_definitions(-DHAVE_BCOPY=1)
endif()

if (HAVE_PTHREAD_H)
    add_definitions(-DHAVE_PTHREAD_H=1)
endif()

# Choose to use pkg-config or not
#set(WITH_PKGCONFIG FALSE CACHE BOOL
#  "Choose whether a pkgconfig file (PDAL.pc) should be installed")

#------------------------------------------------------------------------------
# General build settings
#------------------------------------------------------------------------------

# note we default to debug mode
if(NOT MSVC_IDE)
  if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
  endif()
  message(STATUS "Setting libspatialindex build type - ${CMAKE_BUILD_TYPE}")
endif()

set(SIDX_BUILD_TYPE ${CMAKE_BUILD_TYPE})

# TODO: Still testing the output paths --mloskot
set(SIDX_BUILD_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Output directory in which to build RUNTIME target files.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SIDX_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build LIBRARY target files
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SIDX_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build ARCHIVE target files.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SIDX_BUILD_OUTPUT_DIRECTORY})


#------------------------------------------------------------------------------
# Platform and compiler specific settings
#------------------------------------------------------------------------------

if(WIN32)
  if (MSVC)
    if (MSVC80 OR MSVC90 OR MSVC10 OR MSVC11)
      add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
      add_definitions(-D_CRT_NONSTDC_NO_WARNING)
      add_definitions(-D_SCL_SECURE_NO_WARNINGS)
      add_definitions(-DSPATIALINDEX_CREATE_DLL=1)

      set(SIDX_COMPILER_MSVC 1)
      if (MSVC11)
        set(SIDX_COMPILER_VC11 1)
      elseif (MSVC10)
        set(SIDX_COMPILER_VC10 1)
      elseif (MSVC9)
        set(SIDX_COMPILER_VC9 1)
      elseif (MSVC8)
        set(SIDX_COMPILER_VC8 1)
      endif()
    endif()

    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
      string(REGEX REPLACE "/W[0-4]" "/W4"
        CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
  endif()

  set(CMAKE_INCLUDE_PATH c:/osgeo4w/include;$ENV{CMAKE_INCLUDE_PATH})
  set(CMAKE_LIBRARY_PATH c:/osgeo4w/lib;$ENV{CMAKE_LIBRARY_PATH})

  set(SIDX_OS_WIN32 1)

else()
  # not WIN32

  # Recommended C++ compilation flags
  # -Weffc++
  set(SIDX_COMMON_CXX_FLAGS
    "-pedantic -Wall -Wpointer-arith -Wcast-align -Wcast-qual  -Wredundant-decls -Wno-long-long")

  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SIDX_COMMON_CXX_FLAGS}")
    if (CMAKE_COMPILER_IS_GNUCXX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -ansi")
    endif()

    set(SIDX_COMPILER_GCC 1)

  elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER}" MATCHES "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SIDX_COMMON_CXX_FLAGS}")
    set(SIDX_COMPILER_CLANG 1)
  endif()
endif(WIN32)

if (APPLE)
  set(SO_EXT dylib)
  set(CMAKE_FIND_FRAMEWORK "LAST")
elseif(WIN32)
  set(SO_EXT dll)
else()
  set(SO_EXT so)
endif(APPLE)


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
add_subdirectory(test)

