#ifndef __CWM_CENTER_SYSTEMS_TASK_H__
#define __CWM_CENTER_SYSTEMS_TASK_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmCenterSystemsTask.h
@brief 查询、显示Center所管理的所有system的Task定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-06
@warning
@bug
*/

#include "CwxAppTaskBoard.h"
#include "CwxHtmlTmplLoop.h"
#include "CwxHtmlTmpl.h"
#include "CwmCenterTss.h"
class CwmCenterApp;
/**
@class CwmCenterSystemsTask
@brief 查询、显示Center所管理的所有system的Task
*/
class CwmCenterSystemsTask : public CwxAppTaskBoardTask, CwxHtmlTmplLoop
{
public:
    ///构造函数
    CwmCenterSystemsTask(CwmCenterApp* pApp, CwxAppTaskBoard* pTaskBoard)
        :CwxAppTaskBoardTask(pTaskBoard),m_pApp(pApp)
    {
        m_queryMsg = NULL;
        m_pTss = NULL;
    }
    ///析构函数
    ~CwmCenterSystemsTask()
    {
        if (m_queryMsg) CwxMsgBlockAlloc::free(m_queryMsg);
        m_queryMsg = NULL;
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
    /**
    @brief 此为global的loop对象，没有下一个
    @return 0：没有下一条数据记录；1：移到了下一条数据记录
    */
    virtual int next();
    /**
    @brief 获取global 对象的一个变量。
    @param [in] szName 变量的名字
    @param [in] ucSysAttr 变量的系统属性，在CwxHtmlTmplValSysAttr中做了定义
    @param [in] szUserAttr  用户自己的变量属性
    @param [in] iLengthLimit 变量内容输出的长度限制，<0：输出的字符个数不能超过此绝对值
    0：没有限制；>0：输出的字节数不能超过此数值
    @param [in] pEscape 变量的escape变换对象
    @param [in] szArgs 变量定义时设置的参数
    @param [in] pRepl 模板的字符替换对象
    @param [in] bVal true：此为变量取值；false：此为比较取值
    @return NULL：变量不存在；否则，为变量的数值
    */
    virtual char const* val(char const* szName,
        bool bAttrName,
        CWX_UINT8 ucSysAttr,
        char const* szUserAttr,
        int iLengthLimit,
        CwxHtmlTmplEscape* pEscape,
        char const* szArgs,
        CwxReplBase* pRepl,
        bool bVal);
    /**
    @brief 返回system的loop对象。
    @param [in] szLoop loop对象的名字
    @param [in] attrs loop对象定义的时候设置的LOOP的属性对
    @return NULL：loop对象不存在；否则为名字所对应的LOOP对象
    */
    virtual CwxHtmlTmplLoop* loop(char const* szLoop,
        list<pair<char*, char*> >const& attrs);
private:
    ///回复systems的查询
    void reply();
    ///根据模板，形成返回的内容
    int  outputTemplate(string& strType);
public:
    CwxMsgBlock*    m_queryMsg;///<查询的msg
private:
    CwmCenterApp*    m_pApp; ///<app对象
    CwmCenterTss*    m_pTss; ///<tss对象
};


#endif
