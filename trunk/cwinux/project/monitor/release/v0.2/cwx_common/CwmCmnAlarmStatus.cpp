#include "CwmCmnAlarmStatus.h"
#include "CwxDate.h"
bool CwmCmnAlarmStatus::update(string const& strStatus,
                               string const& strLevel,
                               string const& strErrMsg)
{
    CWX_UINT8 ucNewLevel = CwmCmnAlarm::ALARM_CLEAR;
    m_strErrMsg = strErrMsg;
    m_llLastPollTime = CwxDate::getTimestamp();
    if (strLevel == CWM_CMN_DEFAULT)
    {
        if (strStatus == "0")
        {//success
           m_uiSuccessCount ++;
           if (m_bFault && m_pAlarmRule->isAlarmClear(m_uiSuccessCount))
           {
               toAlarmLevel(CwmCmnAlarm::ALARM_CLEAR);
               return true;
           }
           return false;
        }
        else
        {//failure
            m_uiSuccessCount = 0;
            m_uiFailCount ++;
            m_bFault = true;
            if (m_pAlarmRule->isAlarmUp(m_ucAlarm, m_uiFailCount, ucNewLevel))
            {
                toAlarmLevel(ucNewLevel);
                return true;
            }
            return false;
        }
    }
    ucNewLevel = CwmCmnAlarm::getAlarmCode(strLevel);
    if (ucNewLevel != m_ucAlarm)
    {
        m_ucAlarm = ucNewLevel;
        toAlarmLevel(m_ucAlarm);
        if (ucNewLevel == CwmCmnAlarm::ALARM_CLEAR) m_bFault = false;
        return true;
    }
    ///user's level
    if (strStatus == "0")
    {//success
        m_uiSuccessCount++;
        if (m_bFault && m_pAlarmRule->isAlarmClear(m_uiSuccessCount))
        {
            toAlarmLevel(CwmCmnAlarm::ALARM_CLEAR);
            return m_ucAlarm != CwmCmnAlarm::ALARM_CLEAR;
        }
        return false;
    }
    else
    {
        m_uiSuccessCount = 0;
        m_uiFailCount ++;
        m_bFault = true;
        if (m_pAlarmRule->isAlarmUp(m_ucAlarm, m_uiFailCount, ucNewLevel))
        {
            toAlarmLevel(m_ucAlarm);
            return true;
        }
        return false;
    }

}


void CwmCmnAlarmStatus::toAlarmLevel(CWX_UINT8 ucNewLevel)
{
    m_bFault = CwmCmnAlarm::ALARM_CLEAR != ucNewLevel;
    m_uiPWid = m_uiWid;
    m_ucPAlarm = m_ucAlarm;
    m_ucAlarm = ucNewLevel;
    m_uiFailCount = 0;
    m_uiSuccessCount = 0;
}

void CwmCmnAlarmStatus::formActionAlarm(string const& strHost,
                                        CWX_UINT32 uiWid,
                                        CWX_UINT32 uiPid,
                                        CwmCmnAlarm& alarm) const
{
    alarm.setHost(strHost);
    alarm.setWid(uiWid);
    alarm.setPid(uiPid);
    time_t tt=getLastPollTime()/1000000;
    string strTime;
    CwxDate::getDate(tt, strTime);
    alarm.setDatetime(strTime);
    alarm.setLevel(getAlarm());
    alarm.setSystem(m_strSystemName);
    alarm.setService(m_strServiceName);
    alarm.setAction(m_strActionName);
    alarm.setType(m_strAlarmType);
    alarm.setError(getErrMsg());
    alarm.setPWid(m_uiPWid);
    alarm.setPLevel(m_ucPAlarm);
    alarm.setState(CwmCmnAlarm::ALARM_STATE_INIT);
}

void CwmCmnAlarmStatus::formActionRecord(string const& strHost,
                                         CWX_UINT32 uiWid,
                                         CWX_UINT32 uiPid,
                                         CwxKeyValueItem const& kvRecord,
                                         CwmCmnRecord& record) const
{
    record.setHost(strHost);
    record.setPid(uiPid);
    record.setSystem(m_strSystemName);
    record.setService(m_strServiceName);
    record.setAction(m_strActionName);
    time_t tt=getLastPollTime()/1000000;
    string strTime;
    CwxDate::getDate(tt, strTime);
    record.setDatetime(strTime);
    record.setWid(uiWid);
    record.setError(getErrMsg());
    record.setData(kvRecord.m_szData, kvRecord.m_uiDataLen);
}
