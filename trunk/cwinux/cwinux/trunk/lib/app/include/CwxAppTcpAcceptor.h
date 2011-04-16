#ifndef __CWX_APP_TCP_ACCEPTOR_H__
#define __CWX_APP_TCP_ACCEPTOR_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppTcpAcceptor.h
@brief Tcp监听对象的定义
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxINetAddr.h"
#include "CwxSockAcceptor.h"
#include "CwxSockStream.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxAppHandler4TcpConn.h"
#include "CwxAppHandler4Base.h"
#include "CwxAppReactor.h"


CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;
/**
@class CwxAppTcpAcceptor
@brief 架构的TCP 连接的监听管理对象。每个对象实例负责一个TCP 端口的监听。
*/
class CWX_API CwxAppTcpAcceptor  :public CwxAppHandler4Base
{
public:
    ///常量定义
    enum{
        MAX_PER_RECIEVE_CONNECT  = 512 ///<tcp listen's backlog value
    };
public:
    ///构造函数
    CwxAppTcpAcceptor(CwxAppFramework* pApp,///<
        CwxAppReactor *reactor,
        char const* szAddr, ///<listen addr
        CWX_UINT32 unPort, ///<listen port
        CWX_UINT32 uiSvrId, ///<connect's svr-id
        CWX_UINT32 uiListenId, ///<acceptor's listen id
        bool      bRawData = false, ///<connect's msg having header
        bool      bKeepAlive = true, ///<keep alive
        CWX_UINT16 unMode = CWX_APP_MSG_MODE,
        CWX_UINT32 uiSockSndBuf = 0,
        CWX_UINT32 uiSockRecvBuf = 0);

    ///析构函数
    ~CwxAppTcpAcceptor();
public:
    int accept(CwxINetAddr const& addr);
    ///handle open
    virtual int open (void * parent= 0);
    /// 事件的回调函数。返回-1：失败，自动释放handle；0：成功
    virtual int handle_event(int event, CWX_HANDLE handle=CWX_INVALID_HANDLE);
    ///handle close
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE);
public:
    ///get listen addr
    string const& getAddr() const
    {
        return m_strAddr;
    }
    ///get port
    CWX_UINT16 getPort() const
    {
        return m_unPort;
    }
    ///get svr id
    CWX_UINT32	getSvrId() const
    {
        return m_uiSvrId;
    }
    ///get Listen id
    CWX_UINT32 getListenId() const
    {
        return m_uiListenId;
    }
    ///get sign for msg-header
    bool isRawData() const
    {
        return m_bRawData;
    }
    ///keepalive sign
    bool isKeepAlive() const
    {
        return m_bKeepAlive;
    }
    ///get conn mode
    CWX_UINT16 getConnMode() const
    {
        return m_unMode;
    }
    ///获取socket snd buf
    CWX_UINT32 getSockSndBuf() const
    {
        return m_uiSockSndBuf;
    }
    ///获取socket recv buf
    CWX_UINT32 getSockRecvBuf() const
    {
        return m_uiSockRecvBuf;
    }
    ///get close all sign
    bool isCloseAll() const
    {
        return m_bCloseAll;
    }
    ///set close all sign
    void setCloseAll(bool bCloseAll)
    {
        m_bCloseAll = bCloseAll;
    }
private:
    CwxAppHandler4TcpConn* makeHander();
private:
    string         m_strAddr;///<监听的地址
    CWX_UINT16     m_unPort;///<监听的端口号
    CWX_UINT32	    m_uiSvrId;///<建立连接的svr-id
    CWX_UINT32     m_uiListenId;///<TCP 连接监听的监听ID
    bool           m_bRawData;///<建立的连接是否按无格式数据流的方式接受
    bool          m_bKeepAlive;///<建立的连接是否需要执行keep-alive check
    CWX_UINT16     m_unMode;///<架构负责建立连接数据接收，还是只检测连接的事件
    bool           m_bCloseAll;///<listen关闭的时候，是否关闭其所建立的连接
    CWX_UINT32     m_uiSockSndBuf; ///<socket的发送buf大小。
    CWX_UINT32     m_uiSockRecvBuf; ///<socket的接受buf大小。
    CwxSockAcceptor m_acceptor; ///<监听器
    CwxSockStream   m_stream;
};
CWINUX_END_NAMESPACE


#include "CwxPost.h"

#endif

