#ifndef __CWM_NERVE_ACTION_STATUS_H__
#define __CWM_NERVE_ACTION_STATUS_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmNerveActionStatus.h
@brief Action的状态管理对象定义。
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
#include "CwmCmnAlarmStatus.h"
#include "CwmNerveConfig.h"

class CwmNerveServiceStatus;
/**
@class CwmNerveActionStatus
@brief Action状态对象定义
*/
class CwmNerveActionStatus
{
public:
    ///构造函数
    CwmNerveActionStatus();
    ///析构函数
    ~CwmNerveActionStatus();
public:
    ///获取告警的状态
    CwmCmnAlarmStatus& getAlarmStatus()
    {
        return m_alarmStatus;
    }
    ///获取Action的配置信息
    CwmNerveActionInfo const* getActionInfo() const
    {
        return m_pActionInfo;
    }
    ///设置Action的配置信息
    void setActionInfo(CwmNerveActionInfo const* pActionInfo)
    {
        if (pActionInfo)
        {
            m_alarmStatus.setAlarmRule(&pActionInfo->getAlarmRule());
            m_alarmStatus.setActionName(pActionInfo->getName());
            m_alarmStatus.setAlarmType(pActionInfo->getAlarmType());
        }
        m_pActionInfo = pActionInfo;
    }
    ///获取service status
    CwmNerveServiceStatus const* getServiceStatus() const
    {
        return m_pServiceStatus;
    }
    ///设置service status
    void setServiceStatus(CwmNerveServiceStatus const* pServiceStatus);
private:
    ///禁止拷贝构造
    CwmNerveActionStatus(CwmNerveActionStatus const& )
    {

    }
    ///禁止复制操作
    CwmNerveActionStatus & operator=(CwmNerveActionStatus const& )
    {
        return *this;
    }
private:
    CwmCmnAlarmStatus        m_alarmStatus; ///<action的告警控制对象
    CwmNerveActionInfo const* m_pActionInfo; ///<action的信息
    CwmNerveServiceStatus const* m_pServiceStatus; ///<action所属的service对象
};



#endif
