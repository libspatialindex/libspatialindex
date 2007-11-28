# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(spatialindex, 1.1.1, mhadji@gmail.com)
AC_CANONICAL_BUILD

# Checks for programs.
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE

# Checks for header files.
AC_CHECK_HEADERS(fcntl.h,, [AC_MSG_ERROR([cannot find fcntl.h, bailing out])])
AC_CHECK_HEADERS(unistd.h,, [AC_MSG_ERROR([cannot find unistd.h, bailing out])])
AC_CHECK_HEADERS(sys/types.h,, [AC_MSG_ERROR([cannot find sys/types.h, bailing out])])
AC_CHECK_HEADERS(sys/stat.h,, [AC_MSG_ERROR([cannot find sys/stat.h, bailing out])])
AC_CHECK_HEADERS(pthread.h, [LIBS="$LIBS -lpthread"])
AC_CHECK_HEADERS(sys/resource.h,, [AC_MSG_ERROR([cannot find sys/resource.h, bailing out])])
AC_CHECK_HEADERS(sys/time.h,, [AC_MSG_ERROR([cannot find sys/time.h, bailing out])])
AC_CHECK_HEADERS(stdint.h,, [AC_MSG_ERROR([cannot find stdint.h, bailing out])])
#MH_CXX_HEADER_TOOLS

LIBS="$LIBS"

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday bzero memset memcpy bcopy])

AC_CONFIG_FILES([	Makefile
					include/Makefile
					include/tools/Makefile
					src/Makefile
					src/spatialindex/Makefile
					src/storagemanager/Makefile
					src/rtree/Makefile
					src/mvrtree/Makefile
					src/tprtree/Makefile
		 			src/tools/Makefile
		 			src/tools/geometry/Makefile
		 			src/tools/tools/Makefile
		 			regressiontest/Makefile
					regressiontest/rtree/Makefile
					regressiontest/mvrtree/Makefile
					regressiontest/tprtree/Makefile])

AC_OUTPUT

