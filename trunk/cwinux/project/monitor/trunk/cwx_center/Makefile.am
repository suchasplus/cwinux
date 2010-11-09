##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir =

BUILD_DATE=$(shell date +%F)

bin_PROGRAMS = cwx_center
cwx_center_CPPFLAGS = -D_BUILD_DATE="$(BUILD_DATE)" -W -Wall
cwx_center_SOURCES = ../cwx_common/CwmCmnAlarm.cpp  ../cwx_common/CwmCmnAlarmStatus.cpp  ../cwx_common/CwmCmnDataMgr.cpp\
	CwmCenterApp.cpp  CwmCenterConfig.cpp  cwm_center.cpp  CwmCenterTss.cpp\
	handler/CwmCenterAlarmAcceptor.cpp  handler/CwmCenterAlarmReportor.cpp\
        handler/CwmCenterNerveQuery.cpp  handler/CwmCenterUiQuery.cpp\
	task/CwmCenterAlarmTask.cpp  task/CwmCenterServiceTask.cpp\
	task/CwmCenterSystemsTask.cpp  task/CwmCenterSystemTask.cpp\
	loop/CwmCenterActionLoop.cpp  loop/CwmCenterActionNameLoop.cpp\
        loop/CwmCenterServiceLoop.cpp  loop/CwmCenterSystemLoop.cpp
cwx_center_LDADD=
cwx_center_LDFLAGS =-L@xml_expat_dir@/lib -L@cwinux_dir@/lib -L@libevent_dir@/lib


LIBS=-lcwxapp -lcwxnet -lcwxparse -lcwxcmn -lexpat -levent -lpthread -ldl -lrt
INCLUDES=-I@cwinux_dir@/include/app -I@cwinux_dir@/include/cmn -I@cwinux_dir@/include/net -I@cwinux_dir@/include/parse -I@cwinux_dir@/include/store -I. -Ihandler -Iloop -Itask -I../cwx_common -I@xml_expat_dir@/include -I@libevent_dir@/include 

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
