#ifndef __CWM_NERVE_QUERY_HANDLER_H__
#define __CWM_NERVE_QUERY_HANDLER_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmNerveQueryHandler.h
@brief Nerve接收查询的处理handle。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-26
@warning
@bug
*/
#include "CwxAppCommander.h"
#include "CwxAppTss.h"
#include "CwmNerveTss.h"
class CwmNerveApp;
/**
@class CwmNerveQueryHandler
@brief  Nerve接收查询的处理handle。
*/
class CwmNerveQueryHandler : public CwxAppCmdOp
{
public:
    enum
    {
        MSG_TYPE_ONLINE_DATA = 1 ///查询的消息类型
    };
public:
    ///构造函数
    CwmNerveQueryHandler(CwmNerveApp* pApp):m_pApp(pApp)
    {

    }
    ///析构函数
    ~CwmNerveQueryHandler()
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
    ///执行查询
    void queryOnlineData(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
public:
    ///查询发生错误时，回复查询错误
    static void replyError(CwmNerveApp* pApp,
        CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        char const* szErrMsg);
    ///查询正确返回时，回复查询的内容。
    static void replyQuery(CwmNerveApp* pApp,
        CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        int status,
        string const& strLevel,
        string const& strErr,
        CwxKeyValueItem& record);
private:
    CwmNerveApp*      m_pApp;
};


#endif
