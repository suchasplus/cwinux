#ifndef __CWM_CENTER_ALARM_REPORTOR_H__
#define __CWM_CENTER_ALARM_REPORTOR_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCenterAlarmReportor.h
@brief 告警报告的处理handle。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-05
@warning
@bug
*/
#include "CwxAppCommander.h"
#include "CwxMutexLock.h"
#include "CwxLockGuard.h"
#include "CwmCenterTss.h"
#include "CwmCmnAlarm.h"
#include "CwmCenterAlarmTask.h"
class CwmCenterApp;
/**
@class CwmCenterAlarmReportor
@brief 告警报告的处理handle
*/
class CwmCenterAlarmReportor : public CwxAppCmdOp
{
public:
    enum
    {
        MSG_TYPE_ALARM_REPORT = 1 ///<告警报告的消息类型
    };
public:
    ///构造函数
    CwmCenterAlarmReportor(CwmCenterApp* pApp):m_pApp(pApp)
    {
        m_bReporting = false;
    }
    ///析构函数
    ~CwmCenterAlarmReportor()
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
    @brief 处理收到告警的回复。
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
    ///获取锁对象
    CwxMutexLock& getLock()
    {
        return m_lock;
    };
    ///设置正在报告告警
    void _setReporting(bool bReporting)
    {
        m_bReporting = bReporting;
    }
    ///检查是否正在报告告警
    bool _isReporting() const
    {
        return m_bReporting;
    }
    ///检查能否报告告警
    bool _isEnableReport() const 
    {
        return !m_bReporting && !m_connIds.empty();
    }
    ///获取当前报告告警的连接id
    CWX_UINT32 _getReportConnId()
    {
        if (m_connIds.empty()) return CWX_APP_INVALID_CONN_ID;
        return *m_connIds.begin();
    }
    ///对于avg告警报告模式，将已经报告告警的连接放到连接池的尾部
    void _switchConnToEnd()
    {
        if (!m_connIds.empty())
        {
            m_connIds.push_back(*m_connIds.begin());
            m_connIds.pop_front();
        }
    }
public:
    ///报告下一个告警
    static void reportNextAlarm(CwmCenterApp* pApp, CwxAppTss* pThrEnv);
    ///发送一个报警
    static bool sendAlarm(CwmCenterApp* pApp,
        CwxAppTss* pThrEnv,
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiTaskId,
        string const& strMails,
        CwmCmnAlarm const& alarm);
private:
    CwxMutexLock       m_lock; ///<保护m_bReporting与m_connIds的锁
    CwmCenterApp*      m_pApp; ///<app对象
    bool              m_bReporting; ///<是否正在报告告警
    list<CWX_UINT32>    m_connIds; ///<报告告警的连接ID的队列
};


#endif
