#ifndef __CWM_CMN_ALARM_RULE_H__
#define __CWM_CMN_ALARM_RULE_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCmnAlarmRule.h
@brief 告警规则定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-24
@warning
@bug
*/
#include "CwxType.h"
#include "CwxStl.h"
#include "CwmCmnMacro.h"
#include "CwmCmnAlarm.h"

/**
@class CwmCmnAlarmRule
@brief 告警定义对象
*/
class CwmCmnAlarmRule
{
    enum{
        DEF_POLL_SECOND = 10, ///<缺省的poll时间间隔
        DEF_CLEAR_TIME  = 2, ///<缺省告警清除的正常次数
        DEF_WARNING_TIME = 2, ///<在异常状态，多少次异常升级为warning告警
        DEF_MINOR_TIME = 30, ///<在warning状态，多少次异常升级为MINOR告警
        DEF_MAJOR_TIME = 60, ///<在minor状态，多少次异常升级为MAJOR告警
        DEF_CRITICAL_TIME = 120, ///<在major状态，多少次异常升级为CRITICAL告警
        DEF_FATAL_TIME = 240, ///<在Critical状态，多少次异常升级为FATAL告警
    };
public:
    ///默认构造函数
    CwmCmnAlarmRule()
    {
        m_uiPollSecond = DEF_POLL_SECOND;
        m_uiClearTime = DEF_CLEAR_TIME;
        m_uiWarningTime = DEF_WARNING_TIME;
        m_uiMinorTime = DEF_MINOR_TIME;
        m_uiMajorTime = DEF_MAJOR_TIME;
        m_uiCriticalTime = DEF_CRITICAL_TIME;
        m_uiFatalTime = DEF_FATAL_TIME;
    }
    ///构造函数
    CwmCmnAlarmRule(string const& name,
                   CWX_UINT32  uiPollSecond = DEF_POLL_SECOND,
                   CWX_UINT32  uiClearTime = DEF_CLEAR_TIME,
                   CWX_UINT32  uiWarningTime = DEF_WARNING_TIME,
                   CWX_UINT32  uiMinorTime = DEF_MINOR_TIME,
                   CWX_UINT32  uiMajorTime = DEF_MAJOR_TIME,
                   CWX_UINT32  uiCriticalTime = DEF_CRITICAL_TIME,
                   CWX_UINT32  uiFatalTime = DEF_FATAL_TIME)
    {
        m_strName = name;
        m_uiPollSecond = uiPollSecond;
        m_uiClearTime = uiClearTime;
        m_uiWarningTime = uiWarningTime;
        m_uiMinorTime = uiMinorTime;
        m_uiMajorTime = uiMajorTime;
        m_uiCriticalTime = uiCriticalTime;
        m_uiFatalTime = uiFatalTime;
    }
    ///拷贝构造
    CwmCmnAlarmRule(CwmCmnAlarmRule const& obj)
    {
        m_strName = obj.m_strName;
        m_uiPollSecond = obj.m_uiPollSecond;
        m_uiClearTime = obj.m_uiClearTime;
        m_uiWarningTime = obj.m_uiWarningTime;
        m_uiMinorTime = obj.m_uiMinorTime;
        m_uiMajorTime = obj.m_uiMajorTime;
        m_uiCriticalTime = obj.m_uiCriticalTime;
        m_uiFatalTime = obj.m_uiFatalTime;
    }
    ///赋值操作
    CwmCmnAlarmRule& operator=(CwmCmnAlarmRule const& obj)
    {
        if (this != &obj)
        {
            m_strName = obj.m_strName;
            m_uiPollSecond = obj.m_uiPollSecond;
            m_uiClearTime = obj.m_uiClearTime;
            m_uiWarningTime = obj.m_uiWarningTime;
            m_uiMinorTime = obj.m_uiMinorTime;
            m_uiMajorTime = obj.m_uiMajorTime;
            m_uiCriticalTime = obj.m_uiCriticalTime;
            m_uiFatalTime = obj.m_uiFatalTime;
        }
        return *this;
    }
    ///相等操作
    inline bool operator == (CwmCmnAlarmRule const& obj) const
    {
        return m_strName == obj.m_strName;
    }
public:
    ///设置名字
    inline void setName(string const& strName)
    {
        m_strName = strName;
    }
    ///获取名字
    inline string const& getName() const
    {
        return m_strName;
    }
    ///设置poll间隔
    inline void setPollSecond(CWX_UINT32 uiSecond)
    {
        m_uiPollSecond = uiSecond;
    }
    ///获取poll间隔
    inline CWX_UINT32 getPollSecond() const
    {
        return m_uiPollSecond;
    }
    ///设置告警消除的门限
    inline void setClearTime(CWX_UINT32 uiTime)
    {
        m_uiClearTime = uiTime;
    }
    ///获取告警清除的门限
    inline CWX_UINT32 getClearTime() const
    {
        return m_uiClearTime;
    }
    ///设置Warning告警的门限
    inline void setWarningTime(CWX_UINT32 uiTime)
    {
        m_uiWarningTime = uiTime;
    }
    ///获取Warning告警的门限
    inline CWX_UINT32 getWarningTime() const
    {
        return m_uiWarningTime;
    }
    ///设置Minor告警的门限
    inline void setMinorTime(CWX_UINT32 uiTime)
    {
        m_uiMinorTime = uiTime;
    }
    ///获取Minor告警的门限
    inline CWX_UINT32 getMinorTime() const
    {
        return m_uiMinorTime;
    }
    ///设置Major告警的门限
    inline void setMajorTime(CWX_UINT32 uiTime)
    {
        m_uiMajorTime = uiTime;
    }
    ///获取Major告警的门限
    inline CWX_UINT32 getMajorTime() const
    {
        return m_uiMajorTime;
    }
    ///设置Critical告警的门限
    inline void setCriticalTime(CWX_UINT32 uiTime)
    {
        m_uiCriticalTime = uiTime;
    }
    ///获取Critical的告警门限
    inline CWX_UINT32 getCriticalTime() const
    {
        return m_uiCriticalTime;
    }
    ///设置fatal的告警门限
    inline void setFatalTime(CWX_UINT32 uiTime)
    {
        m_uiFatalTime = uiTime;
    }
    ///获取fatal的告警门限
    inline CWX_UINT32 getFatalTime() const
    {
        return m_uiFatalTime;
    }
    ///判断告警是否可以clear
    inline bool isAlarmClear(CWX_UINT32 uiSuccessTime) const
    {
        return uiSuccessTime>=m_uiClearTime;
    }
    ///判断告警是否需要升级
    inline bool isAlarmUp(CWX_UINT8 ucLevel,
        CWX_UINT32 uiFailTime,
        CWX_UINT8& ucNewAlarm) const
    {
        bool bUp=false;
        switch(ucLevel)
        {
            case CwmCmnAlarm::ALARM_CLEAR:
                if (uiFailTime >= m_uiWarningTime)
                {
                    bUp = true;
                    ucNewAlarm = CwmCmnAlarm::ALARM_WARNING;
                }
                break;
            case CwmCmnAlarm::ALARM_WARNING:
                if (uiFailTime >= m_uiMinorTime)
                {
                    bUp = true;
                    ucNewAlarm = CwmCmnAlarm::ALARM_MINOR;
                }
                break;
            case CwmCmnAlarm::ALARM_MINOR:
                if (uiFailTime >= m_uiMajorTime)
                {
                    bUp = true;
                    ucNewAlarm = CwmCmnAlarm::ALARM_MAJOR;
                }
                break;
            case CwmCmnAlarm::ALARM_MAJOR:
                if (uiFailTime >= m_uiCriticalTime)
                {
                    bUp = true;
                    ucNewAlarm = CwmCmnAlarm::ALARM_CRITICAL;
                }
                break;
            case CwmCmnAlarm::ALARM_CRITICAL:
                if (uiFailTime >= m_uiFatalTime)
                {
                    bUp = true;
                    ucNewAlarm = CwmCmnAlarm::ALARM_FATAL;
                }
                break;
            case CwmCmnAlarm::ALARM_FATAL:
                if (uiFailTime >= m_uiFatalTime)
                {
                    bUp = true;
                    ucNewAlarm = CwmCmnAlarm::ALARM_FATAL;
                }
                break;
            default: ///按ALARM_FATAL方式处理
                if (uiFailTime >= m_uiFatalTime)
                {
                    bUp = true;
                    ucNewAlarm = CwmCmnAlarm::ALARM_FATAL;
                }
                break;
        }
        return bUp;
    }
public:
    ///清空对象
    inline void reset()
    {
        m_strName.erase();
        m_uiPollSecond = DEF_POLL_SECOND;
        m_uiClearTime = DEF_CLEAR_TIME;
        m_uiWarningTime = DEF_WARNING_TIME;
        m_uiMinorTime = DEF_MINOR_TIME;
        m_uiMajorTime = DEF_MAJOR_TIME;
        m_uiCriticalTime = DEF_CRITICAL_TIME;
        m_uiFatalTime = DEF_FATAL_TIME;
    }
private:
    string         m_strName; ///<rule的名字
    CWX_UINT32     m_uiPollSecond; ///<poll's internal
    CWX_UINT32     m_uiClearTime; ///<reset alarm's poll count
    CWX_UINT32     m_uiWarningTime; ///<轮询失败多少次后，升级为告警
    CWX_UINT32     m_uiMinorTime; ///<从warning开始，轮询失败多少次后，升级为Minor
    CWX_UINT32     m_uiMajorTime; ///<从Minor开始，轮询失败多少次后，升级为Major
    CWX_UINT32     m_uiCriticalTime; ///<从Major开始，轮询失败多少次后，升级为Critical
    CWX_UINT32     m_uiFatalTime; ///<从Critical开始，轮询失败多少次后，升级为Fatal
};



#endif
