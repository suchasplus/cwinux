#include "CwmCmnDataMgr.h"

///初始化管理器。返回值：true，成功；false，失败
bool CwmCmnDataMgr::init(string const& )
{
    m_alarms.clear();
    return true;
}
///添加一个告警。返回值：-1：失败；0：存在；1：添加一个
int CwmCmnDataMgr::insert(CwmCmnAlarm const& alarm)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    if (alarm.getState() == CwmCmnAlarm::ALARM_STATE_INIT)
    {
        m_alarms.push_back(alarm);
        return 1;
    }
    return 0;
}
///添加一个数据。返回值：-1：失败；0：存在；1：添加一个
int CwmCmnDataMgr::insert(CwmCmnRecord const& )
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    return 1;
}
///获取下一个未报告的告警。返回值：-1，失败；0，不存在；1，获取一个
int CwmCmnDataMgr::nextReportAlarm(CwmCmnAlarm& alarm)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    if (m_alarms.begin() != m_alarms.end())
    {
        alarm = *m_alarms.begin();
        m_alarms.pop_front();
        return 1;
    }
    return 0;
}
///设置已完成一个告警的报告。返回值：-1，失败；0，不存在；1，修改了告警的状态
int CwmCmnDataMgr::setAlarmReported(string const& strHost, CWX_UINT32 uiWid)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    list<CwmCmnAlarm>::iterator iter = m_alarms.begin();
    while(iter != m_alarms.end())
    {
        if ((strHost == iter->getHost())&& (uiWid == iter->getWid()))
        {
            m_alarms.erase(iter);
            return 1;
        }
        iter++;
    }
    return 0;
}
