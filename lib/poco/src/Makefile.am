##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir = 

lib_LTLIBRARIES = libcwxpoco.la
libcwxpoco_la_CPPFLAGS = -W -Wall

libcwxpoco_la_SOURCES = \
	CwxHttpCookie.cpp
libcwxpoco_la_LDFLAGS = 

cwxpocoincludedir=$(prefix)/include/poco
cwxpocoinclude_HEADERS=\
	../include/CwxHttpCookie.h\
	../include/CwxHttpCookie.inl\
	../include/CwxPocoMacro.h

	
INCLUDES=-I$(srcdir)/../include -I$(srcdir)/../../cmn/include -I$(srcdir)/../../net/include -I$(srcdir)/../../parse/include

doc:
	doxygen ../doc/poco.dxyg

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
