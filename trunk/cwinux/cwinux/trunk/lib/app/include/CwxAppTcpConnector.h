#ifndef __CWX_APP_TCP_CONNECTOR_H__
#define __CWX_APP_TCP_CONNECTOR_H__

/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppTcpConnector.h
@brief Tcp的连接管理对象，负责主动连接的建立与管理
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxAppHandler4TcpConn.h"
#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE
class CwxAppFramework;
/**
@class CwxAppTcpConnector
@brief Tcp的连接管理对象，负责主动连接的建立与管理
*/
class CWX_API CwxAppTcpConnector
{
public:
	///构造函数
	CwxAppTcpConnector (CwxAppFramework* pApp, CwxAppReactor *reactor);
    ///析构函数
	~CwxAppTcpConnector ();
public:
    //建立连接， -1：失败；0：等待连接；1：建立连接
    int connect (CwxAppHandler4TcpConn* pHandler, ///<conn handler
        char const* szAddr, ///<connect addr
        CWX_UINT16 unPort, ///<connect port
        int iFamily=AF_UNSPEC, ///<family
        CWX_UINT32 uiSockSndBuf=0, ///<socket snd buf
        CWX_UINT32 uiSockRecvBuf=0 ///<socket recv buf
        );
private:
    CwxAppFramework* m_pApp;
    CwxAppReactor * m_reactor;

};

CWINUX_END_NAMESPACE

#include "CwxPost.h"


#endif
