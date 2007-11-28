## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = src . regressiontest
lib_LTLIBRARIES = libspatialindex.la
pkgincludedir = $(includedir)/spatialindex
nobase_pkginclude_HEADERS = include/SpatialIndex.h include/RTree.h include/MVRTree.h include/TPRTree.h include/TimePoint.h include/TimeRegion.h include/MovingPoint.h include/MovingRegion.h include/tools/Tools.h include/tools/LineSegment.h include/tools/PointerPool.h include/tools/Point.h include/tools/PoolPointer.h include/tools/Region.h include/tools/SmartPointer.h include/tools/TemporaryFile.h include/tools/Tools.h
libspatialindex_la_SOURCES = 
libspatialindex_la_LIBADD = src/spatialindex/liblibrary.la src/storagemanager/libstoragemanager.la src/rtree/librtree.la src/mvrtree/libmvrtree.la src/tprtree/libtprtree.la src/tools/geometry/libgeometry.la src/tools/tools/libtools.la
