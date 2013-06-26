# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

m4_define([sidx_version_major], [1])
m4_define([sidx_version_minor], [8])
m4_define([sidx_version_micro], [1])
m4_define([sidx_version],
          [sidx_version_major.sidx_version_minor.sidx_version_micro])

AC_INIT([spatialindex], [sidx_version], [mhadji@gmail.com], [spatialindex-src])
AC_CANONICAL_BUILD

debug_default="no"

CFLAGS="-Wall -Wno-long-long -pedantic $CFLAGS"
CXXFLAGS="-Wall -Wno-long-long -pedantic -std=c++98 $CXXFLAGS"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE([dist-bzip2 subdir-objects])

# Checks for header files.
AC_CHECK_HEADERS(fcntl.h,, [AC_MSG_ERROR([cannot find fcntl.h, bailing out])])
AC_CHECK_HEADERS(unistd.h,, [AC_MSG_ERROR([cannot find unistd.h, bailing out])])
AC_CHECK_HEADERS(sys/types.h,, [AC_MSG_ERROR([cannot find sys/types.h, bailing out])])
AC_CHECK_HEADERS(sys/stat.h,, [AC_MSG_ERROR([cannot find sys/stat.h, bailing out])])

AC_CHECK_HEADERS(pthread.h, [
#ifndef ANDROID
LIBS="$LIBS -lpthread"
#endif])

AC_CHECK_HEADERS(sys/resource.h,, [AC_MSG_ERROR([cannot find sys/resource.h, bailing out])])
AC_CHECK_HEADERS(sys/time.h,, [AC_MSG_ERROR([cannot find sys/time.h, bailing out])])
AC_CHECK_HEADERS(stdint.h,, [AC_MSG_ERROR([cannot find stdint.h, bailing out])])
AC_CHECK_HEADERS(features.h)

#MH_CXX_HEADER_TOOLS

LIBS="$LIBS"

AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes] turn on debugging [default=$debug_default]],, enable_debug=$debug_default)

if test "x$enable_debug" = "xyes"; then
	CXXFLAGS="$CXXFLAGS -g -DDEBUG"
	AC_MSG_RESULT(checking wether debug information is enabled... yes)
else
	CXXFLAGS="$CXXFLAGS -O2 -DNDEBUG"
	AC_MSG_RESULT(checking wether debug information is enabled... no)
fi

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday memset memcpy bcopy srand48])

AM_CONDITIONAL([RAND_IS_CONFIG], [test "x$HAVE_SRAND48" = xtrue])

AC_CONFIG_FILES([	Makefile
					include/Makefile
					src/Makefile
					src/libspatialindex.pc
					src/capi/Makefile
					src/spatialindex/Makefile
					src/storagemanager/Makefile
					src/rtree/Makefile
					src/mvrtree/Makefile
					src/tprtree/Makefile
		 			src/tools/Makefile
		 			test/Makefile
					test/geometry/Makefile
					test/rtree/Makefile
					test/mvrtree/Makefile
					test/tprtree/Makefile])

AC_OUTPUT

