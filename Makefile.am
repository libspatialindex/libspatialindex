## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = src . regressiontest include

lib_LTLIBRARIES = libspatialindex.la

libspatialindex_la_SOURCES = 
libspatialindex_la_LIBADD = \
	src/spatialindex/liblibrary.la \
	src/storagemanager/libstoragemanager.la \
	src/rtree/librtree.la \
	src/mvrtree/libmvrtree.la \
	src/tprtree/libtprtree.la \
	src/tools/geometry/libgeometry.la \
	src/tools/tools/libtools.la

libspatialindex_la_LDFLAGS = -no-undefined -version-info 1:0:1
