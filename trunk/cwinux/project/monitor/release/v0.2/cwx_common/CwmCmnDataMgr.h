#ifndef __CWM_CMN_DATA_MGR_H__
#define __CWM_CMN_DATA_MGR_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCmnDataMgr.h
@brief 告警数据与性能数据的数据管理器，实现数据的存储、查询、修改。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-27
@warning
@bug
*/
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwxMutexLock.h"
#include "CwxLockGuard.h"
#include "CwmCmnMacro.h"
#include "CwmCmnAlarm.h"
#include "CwmCmnRecord.h"


/**
@class CwmCmnDataMgr
@brief 数据管理对象，实现告警、性能数据的存储、查询等操作
*/

class CwmCmnDataMgr
{
public:
    ///构造函数
    CwmCmnDataMgr()
    {
        m_uiMaxPid = time(NULL);
        m_uiMaxWid = time(NULL);
        memset(m_strErrMsg, 0x00, sizeof(m_strErrMsg));
    }
    ///析构函数
    ~CwmCmnDataMgr()
    {

    }
public:
    ///初始化管理器。返回值：true，成功；false，失败
    bool init(string const& strFileName);
    ///添加一个告警。返回值：-1：失败；0：存在；1：添加一个
    int insert(CwmCmnAlarm const& alarm);
    ///添加一个数据。返回值：-1：失败；0：存在；1：添加一个
    int insert(CwmCmnRecord const& record);
    ///获取下一个未报告的告警。返回值：-1，失败；0，不存在；1，获取一个
    int nextReportAlarm(CwmCmnAlarm& alarm);
    ///设置已完成一个告警的报告。返回值：-1，失败；0，不存在；1，修改了告警的状态
    int setAlarmReported(string const& strHost, CWX_UINT32 uiWid);
    ///获取下一个wid
    CWX_UINT32 nextWid()
    {
        return m_uiMaxWid++;
    }
    ///获取下一个pid
    CWX_UINT32 nextPid()
    {
        return m_uiMaxPid++;
    }
    ///获取错误消息
    char const* getErrMsg() const
    {
        return m_strErrMsg;
    }
private:
    ///禁止拷贝构造
    CwmCmnDataMgr(CwmCmnDataMgr const& )
    {

    }
    ///禁止赋值操作
    CwmCmnDataMgr& operator=(CwmCmnDataMgr const& )
    {
        return *this;
    }

private:
    CwxMutexLock      m_lock; ///<保护锁
    CWX_UINT32        m_uiMaxWid;///<当前最大的wid
    CWX_UINT32        m_uiMaxPid;///<当前最大的pid
    list<CwmCmnAlarm>  m_alarms;///<滞留的告警列表
    string            m_strFile;///<数据文件名字
    char              m_strErrMsg[512];///<错误信息
    
};

#endif
