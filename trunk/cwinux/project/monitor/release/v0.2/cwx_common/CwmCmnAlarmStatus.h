#ifndef __CWM_CMN_ALARM_STATUS_H__
#define __CWM_CMN_ALARM_STATUS_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCmnAlarmStatus.h
@brief Alarm的状态管理对象定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-28
@warning
@bug
*/
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwxPackage.h"
#include "CwmCmnMacro.h"
#include "CwmCmnRecord.h"
#include "CwmCmnAlarm.h"
#include "CwmCmnAlarmRule.h"

/**
@class CwmCmnAlarmStatus
@brief Alarm状态对象定义
*/
class CwmCmnAlarmStatus
{
public:
    ///构造函数
    CwmCmnAlarmStatus()
    {
        m_uiWid =0;
        m_ucAlarm = CwmCmnAlarm::ALARM_CLEAR;
        m_llLastPollTime = 0;
        m_uiSuccessCount = 0;
        m_uiFailCount = 0;
        m_bFault = false;
        m_isPolling = false;
        m_uiPWid = 0;
        m_ucPAlarm = CwmCmnAlarm::ALARM_CLEAR;
        m_pAlarmRule =  NULL;
    }
    ///析构函数
    ~CwmCmnAlarmStatus()
    {

    }
public:
    /**
    @brief 根据poll的返回结果，更新action的状态，若告警Level发生改变，则返回告警。
    @param [in] strStatus poll返回的statues
    @param [in] strLevel poll返回的level
    @param [in] strErrMsg poll返回的错误消息
    @return true：形成了新的告警；false：告警状态没有改变
    */
    bool update(string const& strStatus,
        string const& strLevel,
        string const& strErrMsg);
public:
    ///获取当前告警的WID
    CWX_UINT32 getWid() const
    {
        return m_uiWid;
    }
    ///设置当前告警的WID
    void setWid(CWX_UINT32 uiWid)
    {
        m_uiWid = uiWid;
    }
    ///获取状态
    CWX_UINT8 getAlarm() const
    {
        return m_ucAlarm;
    }
    ///设置alarm
    void setAlarm(CWX_UINT8 ucAlarm)
    {
        m_ucAlarm = ucAlarm;
    }
    ///获取上一次poll的时间
    CWX_UINT64 getLastPollTime() const
    {
        return m_llLastPollTime;
    }
    ///设置上一次poll的时间
    void setLastPollTime(CWX_UINT64 ullTime)
    {
        m_llLastPollTime = ullTime;
    }
    ///获取在失败状态下，连续成功的次数
    CWX_UINT32 getSuccessCount() const
    {
        return m_uiSuccessCount;
    }
    ///设置在失败状态下，连续成功的次数
    void setSuccesCount(CWX_UINT32 uiCount)
    {
        m_uiSuccessCount = uiCount;
    }
    ///获取在当前状态下，积累失败的次数
    CWX_UINT32 getFailCount() const
    {
        return m_uiFailCount;
    }
    ///设置在当前状态下，积累失败的次数
    void setFailCount(CWX_UINT32 uiCount)
    {
        m_uiFailCount = uiCount;
    }
    ///获取前一个告警的WID
    CWX_UINT32 getPWid() const
    {
        return m_uiPWid;
    }
    ///设置前一个告警的WID
    void setPWid(CWX_UINT32 uiWid)
    {
        m_uiPWid = uiWid;
    }
    ///获取前一个告警的状态
    CWX_UINT8 getPAlarm() const
    {
        return m_ucPAlarm;
    }
    ///设置前一个告警的状态
    void setPAlarm(CWX_UINT8 ucPAlarm)
    {
        m_ucPAlarm = ucPAlarm;
    }
    ///获取是否有错误发生
    bool isFault() const
    {
        return m_bFault;
    }
    ///设置有错误发生
    void setFault(bool bFault)
    {
        m_bFault = bFault;
    }
    ///获取出错消息
    string const& getErrMsg() const
    {
        return m_strErrMsg;
    }
    ///设置错误消息
    void setErrMsg(string const& strErrMsg)
    {
        m_strErrMsg = strErrMsg;
    }
    ///获取alarm规则
    CwmCmnAlarmRule const* getAlarmRule() const
    {
        return m_pAlarmRule;
    }
    ///设置alarm规则
    void setAlarmRule(CwmCmnAlarmRule const* pRule)
    {
        m_pAlarmRule = pRule;
    }
    ///获取系统的名字
    string const& getSystemName() const
    {
        return m_strSystemName;
    }
    ///设置系统的名字
    void setSystemName(string const& strName)
    {
        m_strSystemName = strName;
    }
    ///获取服务的名字
    string const& getServiceName() const
    {
        return m_strServiceName;
    }
    ///设置服务的名字
    void setServiceName(string const& strName)
    {
        m_strServiceName = strName;
    }
    ///获取action的名字
    string const& getActionName() const
    {
        return m_strActionName;
    }
    ///设置action的名字
    void setActionName(string const& strName)
    {
        m_strActionName = strName;
    }
    ///获取告警的类型
    string const& getAlarmType() const 
    {
        return m_strAlarmType;
    }
    ///设置告警的类型
    void setAlarmType(string const& strAlarmType)
    {
        m_strAlarmType = strAlarmType;
    }
    ///检查是否正在执行polling
    bool isPolling() const
    {
        return m_isPolling;
    }
    ///设置是否正在polling
    void setPolling(bool bPolling)
    {
        m_isPolling = bPolling;
    }
    ///检查是否应该进行状态轮询
    bool isNeedPoll(CWX_UINT64 ullNow) const
    {
        return m_llLastPollTime/1000000 + m_pAlarmRule->getPollSecond() <= ullNow/1000000;
    }
    ///根据action的信息，初始化告警信息
    void formActionAlarm(string const& strHost,
        CWX_UINT32 uiWid,
        CWX_UINT32 uiPid,
        CwmCmnAlarm& alarm) const;
    ///根据action的信息，初始化记录信息
    void formActionRecord(string const& strHost,
        CWX_UINT32 uiWid,
        CWX_UINT32 uiPid,
        CwxKeyValueItem const& kvRecord,
        CwmCmnRecord& record) const;
private:
    ///设置到指定的告警级别
    void toAlarmLevel(CWX_UINT8 ucNewLevel);
private:
    ///禁止拷贝构造
    CwmCmnAlarmStatus(CwmCmnAlarmStatus const& )
    {

    }
    ///禁止赋值操作
    CwmCmnAlarmStatus & operator=(CwmCmnAlarmStatus const& )
    {
        return *this;
    }
private:
    CWX_UINT32    m_uiWid; ///<当前告警的Wid
    CWX_UINT8      m_ucAlarm;///<当前的告警状态
    CWX_UINT64     m_llLastPollTime;///<上次轮询的时间
    CWX_UINT32     m_uiSuccessCount;///<连续成功的次数
    CWX_UINT32     m_uiFailCount; ///<自无效状态状态以来，积累的失败次数
    bool           m_bFault; ///<即便在ALARM_CLEAR，也可能发生错误，此是记录是否有错误发生
    string         m_strErrMsg; ///<告警的错误信息
    CwmCmnAlarmRule const*  m_pAlarmRule;///<告警规则
    string          m_strSystemName; ///<告警的system名字
    string          m_strServiceName;///<告警的service名字
    string          m_strActionName;///<告警的action名字
    string          m_strAlarmType;///<告警的类型
    CWX_UINT32  m_uiPWid; ///<前一个告警ID；
    CWX_UINT8   m_ucPAlarm; ///<前一个告警的状态
    bool           m_isPolling; ///<当前告警的action是否正在轮询中
};



#endif
