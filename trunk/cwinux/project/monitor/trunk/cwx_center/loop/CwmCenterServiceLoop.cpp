#include "CwmCenterServiceLoop.h"
#include "CwmCenterApp.h"

void CwmCenterServiceLoop::init(CwmCenterTss* pTss, map<string/*service name*/, CwmCenterService*> const* pServices)
{
    m_bFirst = true;
    m_pTss = pTss;
    m_pService = pServices;
}


int CwmCenterServiceLoop::next()
{
    if (m_bFirst)
    {
        m_bFirst = false;
        m_service_iter = m_pService->begin();
    }
    else
    {
        if (m_service_iter == m_pService->end()) return 0;
        m_service_iter++;
    }
    if (m_service_iter == m_pService->end()) return 0;
    return 1;
}

char const* CwmCenterServiceLoop::val(char const* szName,
                                      bool bAttrName,
                                     CWX_UINT8 ,
                                     char const* ,
                                     int ,
                                     CwxHtmlTmplEscape* ,
                                     char const* ,
                                     CwxReplBase* ,
                                     bool )
{
    if (m_service_iter != m_pService->end())
    {
       char const* szValue = getServiceAttr(m_service_iter->second, szName, m_pTss->m_szKeyValue64K);
       if (szValue) return szValue;
    }
    return bAttrName?"":NULL;
}

char const* CwmCenterServiceLoop::getServiceAttr(CwmCenterService const* pService, char const* szAttr, char* szBuf64k)
{
    string strEmails;
    CWX_UINT32 uiPos = 0;
    if (strcmp(szAttr, "name") == 0)
    {
        return pService->getName().c_str();
    }
    else if (strcmp(szAttr, "mail_rule") == 0)
    {
        return pService->getMailRule()->getName().c_str();
    }
    else if (strcmp(szAttr, "emails") == 0)
    {
        uiPos += sprintf(szBuf64k + uiPos, "warning:%s, ", pService->getMailRule()->getAlarmEmails(CwmCmnAlarm::ALARM_WARNING, strEmails).c_str());
        uiPos += sprintf(szBuf64k + uiPos, "minor:%s, ", pService->getMailRule()->getAlarmEmails(CwmCmnAlarm::ALARM_MINOR, strEmails).c_str());
        uiPos += sprintf(szBuf64k + uiPos, "major:%s, ", pService->getMailRule()->getAlarmEmails(CwmCmnAlarm::ALARM_MAJOR, strEmails).c_str());
        uiPos += sprintf(szBuf64k + uiPos, "critical:%s, ", pService->getMailRule()->getAlarmEmails(CwmCmnAlarm::ALARM_CRITICAL, strEmails).c_str());
        uiPos += sprintf(szBuf64k + uiPos, "fatal:%s ", pService->getMailRule()->getAlarmEmails(CwmCmnAlarm::ALARM_FATAL, strEmails).c_str());
        return szBuf64k;
    }
    else if (strcmp(szAttr, "host_num") == 0)
    {
        sprintf(szBuf64k, "%u", (unsigned int)pService->getHosts().size());
        return szBuf64k;
    }
    else if (strcmp(szAttr, "act_num") == 0)
    {
        sprintf(szBuf64k, "%u", (unsigned int)pService->getActions().size());
        return szBuf64k;
    }
    return NULL;

}

CwxHtmlTmplLoop* CwmCenterServiceLoop::loop(char const* ,
                                           list<pair<char*, char*> >const& )
{
    return NULL;

}

