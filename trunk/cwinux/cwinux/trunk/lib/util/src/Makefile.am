##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir = 

lib_LTLIBRARIES = libcwxutil.la
libcwxutil_la_CPPFLAGS = -W -Wall

libcwxutil_la_SOURCES = \
	CwxDayLogger.cpp\
	CwxMd5.cpp
libcwxutil_la_LDFLAGS = 

cwxutilincludedir=$(prefix)/include/util
cwxutilinclude_HEADERS=\
	../include/CwxDayLogger.h\
	../include/CwxMd5.h\
	../include/CwxUtilMacro.h
	
INCLUDES=-I$(srcdir)/../include -I$(srcdir)/../../cmn/include -I$(srcdir)/../../net/include -I$(srcdir)/../../parse/include

doc:
	doxygen ../doc/util.dxyg

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
