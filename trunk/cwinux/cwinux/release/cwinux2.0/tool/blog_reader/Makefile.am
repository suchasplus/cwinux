##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir =

BUILD_DATE=$(shell date +%F)


bin_PROGRAMS = blog_reader
blog_reader_CPPFLAGS = -D_BUILD_DATE="$(BUILD_DATE)"
blog_reader_SOURCES = \
        binlog.cpp\
        CwxBinlogOp.cpp
blog_reader_LDADD=
blog_reader_LDFLAGS =-L@xml_expat_dir@/lib -L$(srcdir)/../../lib/cmn/src/.libs  -L$(srcdir)/../../lib/store/src/.libs -L$(srcdir)/../../lib/parse/src/.libs
LIBS=-lcwxstore -lcwxparse -lcwxcmn -lexpat -lpthread -ldl -lrt -lz
INCLUDES=-I$(srcdir)/../../lib/cmn/include -I$(srcdir)/../../lib/store/include -I$(srcdir)/../../lib/parse/include -I@xml_expat_dir@/include

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
