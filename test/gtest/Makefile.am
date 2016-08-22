## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = gtest-1.7.0

AM_CPPFLAGS = -I$(srcdir)/gtest-1.7.0

noinst_PROGRAMS = libsidxtest
INCLUDES = -I../../include -I./gtest-1.7.0/include
libsidxtest_SOURCES = main.cc
libsidxtest_LDADD = ../../libspatialindex_c.la
libsidxtest_LDFLAGS = -lpthread -L./gtest-1.7.0/lib -lgtest
# Override Automake's installation targets with the command ":" that does nothing.
install:; @:
install-exec:; @:
install-data:; @:
uninstall:; @:
