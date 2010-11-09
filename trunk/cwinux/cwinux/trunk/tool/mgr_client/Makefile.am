##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir =

BUILD_DATE=$(shell date +%F)


bin_PROGRAMS = mgr_client
mgr_client_CPPFLAGS = -D_BUILD_DATE="$(BUILD_DATE)"
mgr_client_SOURCES = CwxMgrClient.cpp  mgr_client.cpp
mgr_client_LDADD=
mgr_client_LDFLAGS =-L@libevent_dir@/lib -L@xml_expat_dir@/lib -L$(srcdir)/../../lib/cmn/src/.libs  -L$(srcdir)/../../lib/net/src/.libs -L$(srcdir)/../../lib/parse/src/.libs -L$(srcdir)/../../lib/app/src/.libs
LIBS=-lcwxapp -lcwxnet -lcwxparse -lcwxcmn -levent -lexpat -lpthread -ldl -lrt -lz
INCLUDES=-I$(srcdir)/../../lib/cmn/include -I$(srcdir)/../../lib/net/include -I$(srcdir)/../../lib/parse/include -I$(srcdir)/../../lib/app/include -I@xml_expat_dir@/include -I@libevent_dir@/include

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
