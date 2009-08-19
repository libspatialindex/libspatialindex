## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = src . regressiontest include

lib_LTLIBRARIES = libspatialindex.la libspatialindex_c.la

libspatialindex_la_SOURCES = 
libspatialindex_la_LIBADD = \
	src/spatialindex/liblibrary.la \
	src/storagemanager/libstoragemanager.la \
	src/rtree/librtree.la \
	src/mvrtree/libmvrtree.la \
	src/tprtree/libtprtree.la \
	src/tools/libtools.la

libspatialindex_c_la_SOURCES =
libspatialindex_c_la_LIBADD = \
	libspatialindex.la \
	src/capi/libsidxc.la

libspatialindex_la_LDFLAGS = -version-info 1:0:0 
libspatialindex_c_la_LDFLAGS = -version-info 1:0:0 -lstdc++

EXTRA_DIST = makefile.vc INSTALL.WIN
