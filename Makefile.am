## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = src . test include

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

libspatialindex_la_LDFLAGS = -version-info 3:0:0 -no-undefined -lstdc++ 
libspatialindex_c_la_LDFLAGS = -version-info 3:0:0 -no-undefined -lstdc++

EXTRA_DIST = INSTALL.WIN \
             spatialindex.sln \
             spatialindex-vc/spatialindex.vcproj \
             CMakeLists.txt \
             src/CMakeLists.txt \
             test/CMakeLists.txt \
             test/mvrtree/test1/run \
             test/mvrtree/test2/run \
             test/rtree/test1/run \
             test/rtree/test2/run \
             test/rtree/test3/run \
             test/rtree/test4/run \
             test/tprtree/test1/run \
             test/tprtree/test2/run
