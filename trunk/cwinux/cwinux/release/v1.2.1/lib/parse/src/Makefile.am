##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir = 

lib_LTLIBRARIES = libcwxparse.la
libcwxparse_la_CPPFLAGS = -W -Wall

libcwxparse_la_SOURCES = \
	CwxHtmlTmplCmp.cpp\
	CwxJsonParser.cpp\
	CwxXmlFileConfigParser.cpp\
	CwxXmlPackageConv.cpp\
	CwxHtmlTmpl.cpp\
	CwxXmlConfigParser.cpp\
	CwxXmlFileParser.cpp\
	CwxXmlParser.cpp

libcwxparse_la_LDFLAGS = 

cwxparseincludedir=$(prefix)/include/parse
cwxparseinclude_HEADERS=\
	../include/CwxHtmlTmplCmp.h\
	../include/CwxHtmlTmpl.h\
	../include/CwxJsonParser.h\
	../include/CwxParseMacro.h\
	../include/CwxXmlFileConfigParser.h\
	../include/CwxXmlPackageConv.h\
	../include/CwxHtmlTmplEscape.h\
	../include/CwxHtmlTmplLoop.h\
	../include/CwxXmlTreeNode.h\
	../include/CwxXmlConfigParser.h\
	../include/CwxXmlFileParser.h\
	../include/CwxXmlParser.h
	
INCLUDES=-I$(srcdir)/../include -I$(srcdir)/../../cmn/include -I@xml_expat_dir@/include

doc:
	doxygen ../doc/parse.dxyg

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
