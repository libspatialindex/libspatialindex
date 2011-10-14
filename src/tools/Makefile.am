## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_LTLIBRARIES = libtools.la
INCLUDES = -I../../include
libtools_la_SOURCES = Tools.cc 

if !RAND_IS_CONFIG
libtools_la_SOURCES += rand48.cc
endif