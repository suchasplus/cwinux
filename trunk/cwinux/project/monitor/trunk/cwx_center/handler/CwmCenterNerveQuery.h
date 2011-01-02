#ifndef __CWM_CENTER_NERVE_QUERY_H__
#define __CWM_CENTER_NERVE_QUERY_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmCenterNerveQuery.h
@brief Center对Nerve进行查询的处理handle。
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
@class CwmCenterNerveQuery
@brief  Center查询Nerve的处理handle。
*/
class CwmCenterNerveQuery: public CwxAppCmdOp
{
public:
    enum
    {
        MSG_TYPE_SERVICE_QUERY = 1 ///<查询nerve的消息类型
    };
public:
    ///构造函数
    CwmCenterNerveQuery(CwmCenterApp* pApp):m_pApp(pApp)
    {

    }
    ///析构函数
    ~CwmCenterNerveQuery()
    {

    }
public:
    /**
    @brief Nerve的连接建立的处理函数。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onConnCreated(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief Nerve的连接关闭的处理函数。
    @param [in] msg 连接关闭的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 处理Nerve查询回复的处理函数。
    @param [in] msg 收到通信数据包的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief Nerve查询消息发送完毕的事件的处理函数。
    @param [in] msg 消息发送完毕的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief Nerve查询消息发送失败的处理函数。
    @param [in] msg 消息发送失败的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
public:
    ///发送Nerve的查询消息
    static bool queryAction(CwmCenterApp* pApp, 
        CwxAppTss* pThrEnv,
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiTaskId,
        char const* szSystem,
        char const* szService,
        char const* szAction);
private:
    CwmCenterApp*      m_pApp; ///<app对象
};


#endif
