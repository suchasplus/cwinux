##  Process this file with automake to create Makefile.in

includedir =
pkgconfigdir = 

lib_LTLIBRARIES = libcwxapp.la
libcwxapp_la_CPPFLAGS = -W -Wall

libcwxapp_la_SOURCES = \
	CwxAppCommander.cpp\
	CwxAppHandler4Signal.cpp\
	CwxAppMgrServer.cpp\
	CwxAppThreadPool.cpp\
	CwxAppConfig.cpp\
	CwxAppHandler4Stdio.cpp\
	CwxAppNoticePipe.cpp\
	CwxAppThreadPoolEx.cpp\
	CwxAppConnInfo.cpp\
	CwxAppHandler4TcpConn.cpp\
	CwxAppPai.cpp\
	CwxAppThreadPoolMgr.cpp\
	CwxAppFramework.cpp\
	CwxAppHandler4Time.cpp\
	CwxAppProcessMgr.cpp\
	CwxAppTss.cpp\
	CwxAppHandler4Base.cpp\
	CwxAppHandler4UnixConn.cpp\
	CwxAppReactor.cpp\
	CwxAppUnixAcceptor.cpp\
	CwxAppHandler4IoEvent.cpp\
	CwxAppHandlerCache.cpp\
	CwxAppTaskBoard.cpp\
	CwxAppUnixConnector.cpp\
	CwxAppHandler4IoMsg.cpp\
	CwxAppListenMgr.cpp\
	CwxAppTcpAcceptor.cpp\
	CwxAppHandler4Msg.cpp\
	CwxAppLogger.cpp\
	CwxAppTcpConnector.cpp\
	CwxAppHandler4Notice.cpp\
	CwxAppMgrInterface.cpp\
	CwxAppThread.cpp

libcwxapp_la_LDFLAGS = 

cwxappincludedir=$(prefix)/include/app
cwxappinclude_HEADERS=\
	../include/CwxAppAioWindow.h\
	../include/CwxAppHandler4IoEvent.h\
	../include/CwxAppHandler4UnixConn.inl\
	../include/CwxAppProcessMgr.h\
	../include/CwxAppThreadPool.h\
	../include/CwxAppCommander.h\
	../include/CwxAppHandler4IoMsg.h\
	../include/CwxAppHandlerCache.h\
	../include/CwxAppReactor.h\
	../include/CwxAppThreadPool.inl\
	../include/CwxAppCommander.inl\
	../include/CwxAppHandler4Msg.h\
	../include/CwxAppHandlerCache.inl\
	../include/CwxAppReactor.inl\
	../include/CwxAppThreadPoolMgr.h\
	../include/CwxAppConfig.h\
	../include/CwxAppHandler4Msg.inl\
	../include/CwxAppListenMgr.h\
	../include/CwxAppTaskBoard.h\
	../include/CwxAppTpi.h\
	../include/CwxAppConnInfo.h\
	../include/CwxAppHandler4Notice.h\
	../include/CwxAppLogger.h\
	../include/CwxAppTaskBoard.inl\
	../include/CwxAppTss.h\
	../include/CwxAppConnInfo.inl\
	../include/CwxAppHandler4Signal.h\
	../include/CwxAppMacro.h\
	../include/CwxAppTcpAcceptor.h\
	../include/CwxAppUnixAcceptor.h\
	../include/CwxAppForkMgr.h\
	../include/CwxAppHandler4Stdio.h\
	../include/CwxAppMgrInterface.h\
	../include/CwxAppTcpConnector.h\
	../include/CwxAppUnixConnector.h\
	../include/CwxAppFramework.h\
	../include/CwxAppHandler4TcpConn.h\
	../include/CwxAppMgrServer.h\
	../include/CwxAppThread.h\
	../include/CwxAppFramework.inl\
	../include/CwxAppHandler4TcpConn.inl\
	../include/CwxAppNoticePipe.h\
	../include/CwxAppThread.inl\
	../include/CwxAppHandler4Base.h\
	../include/CwxAppHandler4Time.h\
	../include/CwxAppNoticePipe.inl\
	../include/CwxAppThreadPoolEx.h\
	../include/CwxAppHandler4Base.inl\
	../include/CwxAppHandler4UnixConn.h\
	../include/CwxAppPai.h\
	../include/CwxAppThreadPoolEx.inl

	
INCLUDES=-I@libevent_dir@/include -I$(srcdir)/../include -I$(srcdir)/../../cmn/include -I$(srcdir)/../../net/include

doc:
	doxygen ../doc/app.dxyg

## Clean up template repositories, etc.
clean-local:
	-rm -f *~ *.bak *.rpo *.sym lib*.*_pure_* core core.*
