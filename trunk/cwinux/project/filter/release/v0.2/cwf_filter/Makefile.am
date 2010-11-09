##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir =

BUILD_DATE=$(shell date +%F)


bin_PROGRAMS = cwf_filter
cwf_filter_CPPFLAGS = -D_BUILD_DATE="$(BUILD_DATE)" -W -Wall
cwf_filter_SOURCES = \
	CwfFilterApp.cpp\
	CwfFilterConfig.cpp\
	cwf_filter.cpp\
	CwfFilterHandler.cpp\
	CwfFilterMgr.cpp\
	CwfFilterTss.cpp
cwf_filter_LDADD=
cwf_filter_LDFLAGS =-L@xml_expat_dir@/lib -L@cwinux_dir@/lib -L@libevent_dir@/lib
LIBS=-lcwxapp -lcwxnet -lcwxparse -lcwxcmn -lexpat -levent -lpthread -ldl -lrt
INCLUDES=-I@cwinux_dir@/include/app -I@cwinux_dir@/include/cmn -I@cwinux_dir@/include/net -I@cwinux_dir@/include/parse -I@cwinux_dir@/include/store -I@xml_expat_dir@/include -I@libevent_dir@/include 

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
