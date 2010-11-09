#ifndef __CWM_NERVE_SERVICE_STATUS_H__
#define __CWM_NERVE_SERVICE_STATUS_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmNerveServiceStatus.h
@brief Service状态定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-24
@warning
@bug
*/
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwmCmnMacro.h"
#include "CwmCmnAlarmStatus.h"
#include "CwmNerveActionStatus.h"
#include "CwmCmnAlarmRule.h"

/**
@class CwmNerveServiceStatus
@brief Service的状态对象定义
*/
class CwmNerveServiceStatus
{
public:
    ///构造函数
    CwmNerveServiceStatus();
    ///析构函数
    ~CwmNerveServiceStatus();
public:
    ///设置service的连接ID
    void setConnId(CWX_UINT32 uiConnId)
    {
        m_uiServerConnId = uiConnId;
    }
    ///获取连接id
    CWX_UINT32 getConnId() const
    {
        return m_uiServerConnId;
    }
    ///设置service的service-info
    void setServiceInfo(CwmNerveServiceInfo const* pServiceInfo)
    {
        m_pServiceInfo = pServiceInfo;
        if (pServiceInfo)
        {
            m_alarmStatus.setSystemName(pServiceInfo->getSystem());
            m_alarmStatus.setServiceName(pServiceInfo->getName());
            m_alarmStatus.setAlarmType("lost");
            m_alarmStatus.setActionName(pServiceInfo->isCgi()?"cgi":"server");
        }
    }
    ///获取service的service-info
    CwmNerveServiceInfo const* getServiceInfo() const
    {
        return m_pServiceInfo;
    }
    ///获取server类型的service自身的action
    CwmCmnAlarmStatus& getSelfStatus()
    {
        return m_alarmStatus;
    }
    ///添加管理的CwmNerveActionStatus
    void addActionStatus(CwmNerveActionStatus* pActionStatus)
    {
        m_lsActStatus.push_back(pActionStatus);
    }
    ///获取全部的action-status
    list<CwmNerveActionStatus*>& getActionStatus()
    {
        return m_lsActStatus;
    }
    ///设置连接状态
    void setConnected(bool bConnected)
    {
        m_bConnected = bConnected;
    }
    ///获取是否连接
    bool isConnected() const
    {
        return m_bConnected;
    }
private:
    ///禁止拷贝构造
    CwmNerveServiceStatus(CwmNerveServiceStatus const& )
    {

    }
    ///禁止复制操作
    CwmNerveServiceStatus& operator=(CwmNerveServiceStatus const& )
    {
        return *this;
    }

private:
    CWX_UINT32                  m_uiServerConnId; ///<对于服务类型的service的连接id
    CwmNerveServiceInfo const*    m_pServiceInfo; ///<服务的信息
    CwmCmnAlarmStatus            m_alarmStatus; ///<service自身的状态，用来控制server类型的连接的告警
    list<CwmNerveActionStatus*>    m_lsActStatus; ///<service的action的状态
    bool                       m_bConnected; ///<连接是否建立
};

#endif
