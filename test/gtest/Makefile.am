## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = gtest-1.7.0

AM_CPPFLAGS = -I$(srcdir)/gtest-1.7.0

noinst_PROGRAMS = test
INCLUDES = -I../../include -I./gtest-1.7.0/include
test_SOURCES = main.cc
test_LDADD = ../../libspatialindex_c.la
test_LDFLAGS = -lpthread -L./gtest-1.7.0/lib -lgtest
