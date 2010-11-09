#ifndef __CWM_CMN_ALARM_H__
#define __CWM_CMN_ALARM_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCmnAlarm.h
@brief 告警对象的定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-27
@warning
@bug
*/
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwmCmnMacro.h"


/**
@class CwmCmnAlarm
@brief Nerve的告警对象
*/

class CwmCmnAlarm
{
public:
    enum
    {
        ALARM_CLEAR = 0, ///<告警清除
        ALARM_WARNING = 1, ///<警告级告警
        ALARM_MINOR = 2, ///<minor级告警
        ALARM_MAJOR = 3, ///<major级告警
        ALARM_CRITICAL = 4, ///<critical及告警
        ALARM_FATAL = 5, ///<fatal级告警
        ALARM_NUM = 6 ///<告警级别的数量
    };
    enum
    {
        ALARM_STATE_INIT = 0, ///<未报告状态
        ALARM_STATE_REPORTED = 1, ///<已经报告
        ALARM_STATE_DEL = 2 ///<删除状态
    };
public:
    ///构造函数
    CwmCmnAlarm()
    {
        m_uiWid = 0;
        m_uiPid = 0;
        m_ucLevel = ALARM_CLEAR;
        m_ucState = ALARM_STATE_INIT;
        m_uiPWid = 0;
        m_ucPLevel = ALARM_CLEAR;
    }
    ///析构函数
    ~CwmCmnAlarm()
    {

    }
    ///构造函数
    CwmCmnAlarm(string const& strHost,
                 CWX_UINT32 uiWid,
                 CWX_UINT32 uiPid,
                 string const& strDatetime,
                 CWX_UINT8 ucLevel,
                 string const& strSystem,
                 string const& strService,
                 string const& strAction,
                 string const& strType,
                 CWX_UINT8 ucState,
                 CWX_UINT32 uiPWid,
                 CWX_UINT8 ucPLevel,
                 string const& strErr)
    {
        m_strHost = strHost;
        m_uiWid = uiWid;
        m_uiPid = uiPid;
        m_strDatetime = strDatetime;
        m_ucLevel = ucLevel;
        m_strSystem = strSystem;
        m_strService = strService;
        m_strAction = strAction;
        m_strType = strType;
        m_ucState = ucState;
        m_uiPWid = uiPWid;
        m_ucPLevel = ucPLevel;
        m_strError = strErr;
    }
    ///拷贝构造
    CwmCmnAlarm(CwmCmnAlarm const& obj)
    {
        m_strHost = obj.m_strHost;
        m_uiWid = obj.m_uiWid;
        m_uiPid = obj.m_uiPid;
        m_strDatetime = obj.m_strDatetime;
        m_ucLevel = obj.m_ucLevel;
        m_strSystem = obj.m_strSystem;
        m_strService = obj.m_strService;
        m_strAction = obj.m_strAction;
        m_strType = obj.m_strType;
        m_ucState = obj.m_ucState;
        m_uiPWid = obj.m_uiPWid;
        m_ucPLevel = obj.m_ucPLevel;
        m_strError = obj.m_strError;
    }
    ///赋值操作
    CwmCmnAlarm& operator=(CwmCmnAlarm const& obj)
    {
        if (this != &obj)
        {
            m_strHost = obj.m_strHost;
            m_uiWid = obj.m_uiWid;
            m_uiPid = obj.m_uiPid;
            m_strDatetime = obj.m_strDatetime;
            m_ucLevel = obj.m_ucLevel;
            m_strSystem = obj.m_strSystem;
            m_strService = obj.m_strService;
            m_strAction = obj.m_strAction;
            m_strType = obj.m_strType;
            m_ucState = obj.m_ucState;
            m_uiPWid = obj.m_uiPWid;
            m_ucPLevel = obj.m_ucPLevel;
            m_strError = obj.m_strError;
        }
        return *this;
    }
    ///相等比较
    bool operator == (CwmCmnAlarm const& obj) const
    {
        return (m_strHost == obj.m_strHost) && (m_uiWid == obj.m_uiWid);
    }
    ///小于比较
    bool operator < (CwmCmnAlarm const& obj) const
    {
        if (m_strHost < obj.m_strHost) return true;
        if (m_strHost > obj.m_strHost) return false;
        return m_uiWid < obj.m_uiWid;
    }
public:
    ///获取告警的host
    string const& getHost() const
    {
        return m_strHost;
    }
    ///设置告警的host
    void setHost(string const& strHost)
    {
        m_strHost = strHost;
    }
    ///获取告警的id
    CWX_UINT32 getWid() const
    {
        return m_uiWid;
    }
    ///设置告警的id
    void setWid(CWX_UINT32 uiId)
    {
        m_uiWid = uiId;
    }
    ///获取告警对应的数据ID
    CWX_UINT32 getPid() const
    {
        return m_uiPid;
    }
    ///设置告警对应的数据ID
    void setPid(CWX_UINT32 uiId)
    {
        m_uiPid = uiId;
    }
    ///获取告警的时间
    string const& getDatetime() const
    {
        return m_strDatetime;
    }
    ///设置告警的时间
    void setDatetime(string const& strDatetime)
    {
        m_strDatetime = strDatetime;
    }
    ///获取告警的级别
    CWX_UINT8 getLevel() const
    {
        return m_ucLevel;
    }
    ///设置告警的级别
    void setLevel(CWX_UINT8 ucLevel)
    {
        m_ucLevel = ucLevel;
    }
    ///设置告警级别
    void setLevel(string const& strLevel)
    {
        m_ucLevel = getAlarmCode(strLevel);
    }
    ///获取告警所属的system
    string const& getSystem() const 
    {
        return m_strSystem;
    }
    ///设置告警所属的system
    void setSystem(string const& strSystem)
    {
        m_strSystem = strSystem;
    }
    ///获取告警所属的service
    string const& getService() const
    {
        return m_strService;
    }
    ///设置报警所属的service
    void setService(string const& strService)
    {
        m_strService = strService;
    }
    ///获取告警所属的action
    string const& getAction() const
    {
        return m_strAction;
    }
    ///设置告警所属的action
    void setAction(string const& strAction)
    {
        m_strAction = strAction;
    }
    ///获取告警类型
    string const& getType() const
    {
        return m_strType;
    }
    ///设置告警类型
    void setType(string const& strType)
    {
        m_strType = strType;
    }
    ///获取告警的状态
    CWX_UINT8 getState() const
    {
        return m_ucState;
    }
    ///设置告警的状态
    void setState(CWX_UINT8 ucState)
    {
        m_ucState = ucState;
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
    ///获取前一个告警的LEVEL
    CWX_UINT8 getPLevel() const
    {
        return m_ucPLevel;
    }
    ///设置前一个告警的LEVEL
    void setPLevel(CWX_UINT8 ucLevel)
    {
        m_ucPLevel = ucLevel;
    }
    ///获取告警的错误描述
    string const& getError() const
    {
        return m_strError;
    }
    ///设置告警的错误描述
    void setError(string const& strErr)
    {
        m_strError = strErr;
    }
public:
    ///根据告警的code，获取告警的字符串
    inline static string const& getAlarmString(CWX_UINT8 ucLevel)
    {
        if (ucLevel > ALARM_FATAL) ucLevel = ALARM_FATAL;
        return m_arrAlarms[ucLevel];
    }
    ///根据告警的字符串，获取告警的code
    inline static CWX_UINT8 getAlarmCode(string const& strLevel)
    {
        for (CWX_UINT8 i=0; i<ALARM_NUM; i++)
        {
            if (strcasecmp(strLevel.c_str(), m_arrAlarms[i].c_str()) == 0)
            {
                return i;
            }
        }
        return ALARM_FATAL;
    }
    ///判断指定的告警级别是否一个有效的级别
    inline static bool isExistAlarm(string const& strLevel)
    {
        for (CWX_UINT8 i=0; i<ALARM_NUM; i++)
        {
            if (strcasecmp(strLevel.c_str(), m_arrAlarms[i].c_str()) == 0)
            {
                return true;
            }
        }
        return false;
    }
private:
    string      m_strHost; ///<告警的主机
    CWX_UINT32  m_uiWid;///<告警的id
    CWX_UINT32  m_uiPid; ///<告警对应的性能记录id
    string      m_strDatetime; ///<告警发生的时间
    CWX_UINT8   m_ucLevel; ///<告警的级别
    string      m_strSystem; ///<产生告警的系统
    string      m_strService; ///<产生告警的服务
    string      m_strAction; ///<产生告警的action
    string      m_strType; ///<告警类型
    CWX_UINT8   m_ucState; ///<告警的状态
    string      m_strError; ///<告警的描述信息
    CWX_UINT32  m_uiPWid; ///<前一个告警ID；
    CWX_UINT8   m_ucPLevel; ///<前一个告警的Level
    static string  m_arrAlarms[ALARM_NUM];
};

#endif
