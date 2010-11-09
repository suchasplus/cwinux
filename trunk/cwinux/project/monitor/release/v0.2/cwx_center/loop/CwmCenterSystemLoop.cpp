#include "CwmCenterSystemLoop.h"
#include "CwmCenterApp.h"

void CwmCenterSystemLoop::init(CwmCenterTss* pTss, map<string/*system name*/, CwmCenterSystem*> const* pSystems)
{
    m_pTss = pTss;
    m_pSystems = pSystems;
    m_bFirst = true;
}

int CwmCenterSystemLoop::next()
{
    if (m_bFirst)
    {
        m_bFirst = false;
        m_system_iter = m_pSystems->begin();
    }
    else
    {
        if (m_system_iter == m_pSystems->end()) return 0;
        m_system_iter++;
    }
    if (m_system_iter == m_pSystems->end()) return 0;
    return 1;
}

char const* CwmCenterSystemLoop::val(char const* szName,
                                     bool bAttrName,
                                     CWX_UINT8 ,
                                     char const* ,
                                     int ,
                                     CwxHtmlTmplEscape* ,
                                     char const* ,
                                     CwxReplBase* ,
                                     bool )
{
    if (m_system_iter != m_pSystems->end())
    {
        char const* pValue = getSystemAttr(m_system_iter->second, szName, m_pTss->m_szKeyValue64K);
        if (pValue) return pValue;
    }
    return bAttrName?"":NULL;
}

char const* CwmCenterSystemLoop::getSystemAttr(CwmCenterSystem const* pSystem, char const* szAttr, char* szBuf64k)
{
    if (strcmp(szAttr, "name") == 0)
    {
        return pSystem->getName().c_str();
    }
    else if (strcmp(szAttr, "service_num") == 0)
    {
        sprintf(szBuf64k, "%d", pSystem->getServices().size());
        return szBuf64k;
    }
    else if (strcmp(szAttr, "desc") == 0)
    {
        return pSystem->getDesc().c_str();
    }
    return NULL;
}


CwxHtmlTmplLoop* CwmCenterSystemLoop::loop(char const* ,
                                           list<pair<char*, char*> >const& )
{
    return NULL;

}

