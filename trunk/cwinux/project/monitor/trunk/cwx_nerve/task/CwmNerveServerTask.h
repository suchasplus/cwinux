#ifndef __CWM_NERVE_SERVER_TASK_H__
#define __CWM_NERVE_SERVER_TASK_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmNerveServerTask.h
@brief Server的ACTION的Task定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-25
@warning
@bug
*/

#include "CwmNerveTss.h"
#include "CwxAppTaskBoard.h"
#include "CwmNerveServiceStatus.h"
#include "CwmNerveActionStatus.h"
class CwmNerveApp;
/**
@class CwmNerveServerTask
@brief Server类型的ACTION的task
*/
class CwmNerveServerTask : public CwxAppTaskBoardTask
{
public:
    CwmNerveServerTask(CwmNerveApp* pApp, CwxAppTaskBoard* pTaskBoard):CwxAppTaskBoardTask(pTaskBoard),m_pApp(pApp)
    {
        m_queryMsg = NULL;
        m_pService = NULL;
        m_pAction = NULL;
        m_replyMsg = NULL;
        m_bSuccess = false;

    }
    ~CwmNerveServerTask()
    {
        if (m_queryMsg) CwxMsgBlockAlloc::free(m_queryMsg);
        if (m_replyMsg) CwxMsgBlockAlloc::free(m_replyMsg);
    }
public:
    /**
    @brief 通知Task已经超时
    @param [in] pThrEnv 调用线程的Thread-env
    @return void
    */
    virtual void noticeTimeout(CwxAppTss* pThrEnv);
    /**
    @brief 通知Task的收到一个数据包。
    @param [in] msg 收到的消息
    @param [in] pThrEnv 调用线程的Thread-env
    @param [out] bConnAppendMsg 收到消息的连接上，是否还有待接收的其他消息。true：是；false：没有
    @return void
    */
    virtual void noticeRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv, bool& bConnAppendMsg);
    /**
    @brief 通知Task往外发送的一个数据包发送失败。
    @param [in] msg 收到的消息
    @param [in] pThrEnv 调用线程的Thread-env
    @return void
    */
    virtual void noticeFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 通知Task通过某条连接，发送了一个数据包。
    @param [in] msg 发送的数据包的信息
    @param [in] pThrEnv 调用线程的Thread-env
    @param [out] bConnAppendMsg 发送消息的连接上，是否有等待回复的消息。true：是；false：没有
    @return void
    */
    virtual void noticeEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv, bool& bConnAppendMsg);
    /**
    @brief 通知Task等待回复消息的一条连接关闭。
    @param [in] uiSvrId 关闭连接的SVR-ID
    @param [in] uiHostId 关闭连接的HOST-ID
    @param [in] uiConnId 关闭连接的CONN-ID
    @param [in] pThrEnv 调用线程的Thread-env
    @return void
    */
    virtual void noticeConnClosed(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CwxAppTss* pThrEnv);
    /**
    @brief 激活Task。在Task启动前，Task有Task的创建线程所拥有。
    在启动前，Task可以接受自己的异步消息，但不能处理。
    此时有Taskboard的noticeActiveTask()接口调用的。
    @param [in] pThrEnv 调用线程的Thread-env
    @return 0：成功；-1：失败
    */
    virtual int noticeActive(CwxAppTss* pThrEnv);
    /**
    @brief 执行Task。在调用此API前，Task在Taskboard中不存在，也就是说对别的线程不可见。
    Task要么是刚创建状态，要么是完成了前一个阶段的处理，处于完成状态。
    通过此接口，由Task自己控制自己的step的跳转而外界无需关系Task的类型及处理过程。
    @param [in] pTaskBoard 管理Task的Taskboard
    @param [in] pThrEnv 调用线程的Thread-env
    @return void
    */
    virtual void execute(CwxAppTss* pThrEnv);
public:
    CwxMsgBlock*     m_queryMsg;
    CwmNerveServiceStatus* m_pService;
    CwmNerveActionStatus*  m_pAction;
private:
    CwxMsgBlock*     m_replyMsg;
    CwmNerveApp*    m_pApp;
    bool           m_bSuccess;
    string         m_strErrMsg;

};


#endif
