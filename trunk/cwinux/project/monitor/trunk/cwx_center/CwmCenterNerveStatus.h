#ifndef __CWM_CENTER_NERVE_STATUS_H__
#define __CWM_CENTER_NERVE_STATUS_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCenterNerveStatus.h
@brief Nerve节点的连接状态定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-06
@warning
@bug
*/
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwmCmnMacro.h"
#include "CwmCenterConfig.h"
#include "CwmCmnAlarmStatus.h"

///Nerve连接的状态控制对象
class CwmCenterNerveStatus
{
public:
    ///构造函数
    CwmCenterNerveStatus()
    {
        m_uiConnId = 0;
        m_pHost = NULL;
        m_bConnected = false;
    }
    ///析构函数
    ~CwmCenterNerveStatus()
    {

    }
public:
    ///设置nerve节点的连接ID
    void setConnId(CWX_UINT32 uiConnId)
    {
        m_uiConnId = uiConnId;
    }
    ///获取nerve节点的连接id
    CWX_UINT32 getConnId() const
    {
        return m_uiConnId;
    }
    ///设置host的配置信息
    void setHostInfo(CwmCenterHost const* pHost)
    {
        m_pHost = pHost;
        if (pHost)
        {
            m_hostStatus.setAlarmRule(pHost->getAlarmRule());
            m_hostStatus.setSystemName("Center");
            m_hostStatus.setServiceName("Nerve");
            m_hostStatus.setActionName("connection");
            m_hostStatus.setAlarmType("lost");
        }
    }
    ///获取host的配置信息
    CwmCenterHost const* getHostInfo() const
    {
        return m_pHost;
    }
    ///获取host的状态
    CwmCmnAlarmStatus& getStatus()
    {
        return m_hostStatus;
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
    CwmCenterNerveStatus(CwmCenterNerveStatus const& )
    {

    }
    ///禁止赋值操作
    CwmCenterNerveStatus& operator=(CwmCenterNerveStatus const& )
    {
        return *this;
    }

private:
    CWX_UINT32            m_uiConnId; ///<Nerve节点的连接ID
    CwmCenterHost const*   m_pHost; ///<Nerve节点的host信息
    CwmCmnAlarmStatus     m_hostStatus; ///<host的状态
    bool                  m_bConnected; ///<与nerve的连接是否建立
};

#endif
