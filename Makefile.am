## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = src . test include

ACLOCAL_AMFLAGS= -I m4
lib_LTLIBRARIES = libspatialindex.la libspatialindex_c.la

libspatialindex_la_SOURCES =
libspatialindex_la_LIBADD = \
	src/spatialindex/liblibrary.la \
    src/capi/libsidxc.la \
	src/storagemanager/libstoragemanager.la \
	src/rtree/librtree.la \
	src/mvrtree/libmvrtree.la \
	src/tprtree/libtprtree.la \
	src/tools/libtools.la

libspatialindex_c_la_SOURCES =
libspatialindex_c_la_LIBADD = \
	libspatialindex.la \
	src/capi/libsidxc.la

# Dummy file to force C++ linking, see this page for more details:
# https://www.gnu.org/software/automake/manual/html_node/Libtool-Convenience-Libraries.html
nodist_EXTRA_libspatialindex_la_SOURCES = dummy.cxx
nodist_EXTRA_libspatialindex_c_la_SOURCES = dummy.cxx

libspatialindex_la_LDFLAGS = -version-info 5:0:0 -no-undefined
libspatialindex_c_la_LDFLAGS = -version-info 5:0:0 -no-undefined

EXTRA_DIST = CMakeLists.txt \
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
