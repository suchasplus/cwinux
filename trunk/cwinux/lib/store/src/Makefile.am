##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir = 

lib_LTLIBRARIES = libcwxstore.la
libcwxstore_la_CPPFLAGS = -W -Wall

libcwxstore_la_SOURCES = \
	CwxBinLogMgr.cpp\
	CwxSysLogFile.cpp
libcwxstore_la_LDFLAGS = 

cwxstoreincludedir=$(prefix)/include/store
cwxstoreinclude_HEADERS=\
	../include/CwxBinLogMgr.h\
	../include/CwxBinLogMgr.inl\
	../include/CwxSysLogFile.h\
	../include/CwxStoreMacro.h

	
INCLUDES=-I$(srcdir)/../include -I$(srcdir)/../../cmn/include

doc:
	doxygen ../doc/store.dxyg

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
