#ifndef __CWM_NERVE_ALARM_HANDLER_H__
#define __CWM_NERVE_ALARM_HANDLER_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmNerveAlarmHandler.h
@brief Nerve的告警报告处理handle。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-26
@warning
@bug
*/
#include "CwxAppCommander.h"
#include "CwmNerveTss.h"
#include "CwmCmnAlarm.h"
class CwmNerveApp;

/**
@class CwmNerveAlarmHandler
@brief Nerve的告警报告处理handle，包括告警协议的封装
*/
class CwmNerveAlarmHandler : public CwxAppCmdOp
{
public:
    enum
    {
        MSG_TYPE_ALARM_REPORT = 1 ///<发送告警的消息类型
    };
public:
    ///构造函数
    CwmNerveAlarmHandler(CwmNerveApp* pApp):m_pApp(pApp)
    {
        ///初始状态是告警没有报告，告警的报告时串行进行的。
        m_bReply = true;
        ///告警的连接ID，初始值为无连接状态
        m_uiAlarmConnId = CWX_APP_INVALID_CONN_ID;
    }
    ///析构函数
    ~CwmNerveAlarmHandler()
    {

    }
public:
    /**
    @brief 处理告警连接的建立。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onConnCreated(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 处理告警连接的关闭。
    @param [in] msg 连接关闭的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 处理收到告警报告的回复消息。
    @param [in] msg 收到通信数据包的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 告警发送完毕事件的处理函数。
    @param [in] msg 消息发送完毕的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 告警发送失败事件的处理函数。
    @param [in] msg 消息发送失败的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
public:
    ///是否收到告警报告的回复，若没有，则不发送下一个告警，除非超时 
    bool isReportReply() const
    {
        return m_bReply;
    }
    ///设置报告的告警是否回复
    void setReportReply(bool bReply)
    {
        m_bReply = bReply;
    }
    ///获取告警报告的连接id
    CWX_UINT32 getAlarmConnId() const
    {
        return m_uiAlarmConnId;
    }
public:
    ///静态函数，封装告警报告的协议
    static bool sendAlarm(CwmNerveApp* pApp, CwxAppTss* pThrEnv, CwmCmnAlarm const& alarm);
    ///静态函数，封装报告下一个告警的逻辑
    static void reportNextAlarm(CwmNerveApp* pApp, CwxAppTss* pThrEnv);
private:
    CwmNerveApp*      m_pApp; ///<应用的APP对象
    bool             m_bReply; ///<是否收到上一个告警的回复
    CWX_UINT32        m_uiAlarmConnId; ///<告警报告的连接ID
};


#endif
