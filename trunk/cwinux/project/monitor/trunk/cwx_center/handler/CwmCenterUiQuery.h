#ifndef __CWM_CENTER_UI_QUERY_H__
#define __CWM_CENTER_UI_QUERY_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmCenterUiQuery.h
@brief Center接收查询的处理handle。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-05
@warning
@bug
*/
#include "CwxAppCommander.h"
#include "CwxAppTss.h"
#include "CwmCenterTss.h"
class CwmCenterApp;
/**
@class CwmCenterUiQuery
@brief  Center接收查询的处理handle。
*/
class CwmCenterUiQuery : public CwxAppCmdOp
{
public:
    enum
    {
        MSG_TYPE_QUERY = 1 ///<UI查询的消息类型
    };
public:
    ///构造函数
    CwmCenterUiQuery(CwmCenterApp* pApp):m_pApp(pApp)
    {

    }
    ///析构函数
    ~CwmCenterUiQuery()
    {

    }
public:
    /**
    @brief 收到数据查询。
    @param [in] msg 收到通信数据包的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 超时查询超时、action超时。
    @param [in] msg 超时检查的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onTimeoutCheck(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
public:
    ///回复UI查询的结果
    static void replyQuery(CwmCenterApp* pApp, 
        CwxAppTss* pThrEnv,
        CWX_UINT32 uiConnId,
        CwxMsgHead const& recvHeader,
        int code,
        char const* szType,
        char const* szContent,
        CWX_UINT32 uiContentLen);
private:
    ///ui查询的命令解析
    void uiQuery(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
private:
    CwmCenterApp*      m_pApp; ///<app对象
};


#endif
