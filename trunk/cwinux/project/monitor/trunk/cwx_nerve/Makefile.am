##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir =

BUILD_DATE=$(shell date +%F)


bin_PROGRAMS = cwx_nerve
cwx_nerve_CPPFLAGS = -D_BUILD_DATE="$(BUILD_DATE)" -W -Wall
cwx_nerve_SOURCES = ../cwx_common/CwmCmnAlarm.cpp ../cwx_common/CwmCmnAlarmStatus.cpp ../cwx_common/CwmCmnDataMgr.cpp\
	CwmNerveActionStatus.cpp  CwmNerveApp.cpp  CwmNerveConfig.cpp  cwm_nerve.cpp  CwmNerveForkEnv.cpp\
	CwmNerveServiceStatus.cpp\
	handler/CwmNerveAlarmHandler.cpp  handler/CwmNerveCgiHandler.cpp\
        handler/CwmNerveQueryHandler.cpp  handler/CwmNerveServerHandler.cpp\
	task/CwmNerveAlarmTask.cpp  task/CwmNerveCgiTask.cpp  task/CwmNerveServerTask.cpp
cwx_nerve_LDADD=
cwx_nerve_LDFLAGS =-L@xml_expat_dir@/lib -L@cwinux_dir@/lib -L@libevent_dir@/lib

LIBS=-lcwxapp -lcwxnet -lcwxparse -lcwxcmn -lexpat -levent -lpthread -ldl -lrt
INCLUDES=-I@cwinux_dir@/include/app -I@cwinux_dir@/include/cmn -I@cwinux_dir@/include/net -I@cwinux_dir@/include/parse -I@cwinux_dir@/include/store -I../cwx_common -I. -Ihandler -Itask -I@xml_expat_dir@/include -I@libevent_dir@/include 

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
