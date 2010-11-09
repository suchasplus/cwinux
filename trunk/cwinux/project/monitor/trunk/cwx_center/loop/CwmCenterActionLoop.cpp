#include "CwmCenterActionLoop.h"
#include "CwmCenterApp.h"

void CwmCenterActionLoop::init(CwmCenterTss* pTss,
                               string const& strName, 
                               CwmCenterActionLoopData* pActions,
                               CWX_UINT32 uiActioNum)
{
    m_strName = strName;
    m_pTss = pTss;
    m_pActions = pActions;
    m_uiActioNum = uiActioNum;
    m_uiIndex = 0;
}


int CwmCenterActionLoop::next()
{
    if (m_uiIndex >= m_uiActioNum) return 0;
    m_bValid = false;
    m_pNerve = m_pActions[m_uiIndex].m_pNerve;
    if (m_pActions[m_uiIndex].m_pRecvMsg)
    {
        if (!m_packReader.unpack(m_pActions[m_uiIndex].m_pRecvMsg->rd_ptr(), 
                              m_pActions[m_uiIndex].m_pRecvMsg->length(),
                              false,
                              true))
        {
            m_strErrMsg = m_packReader.getErrMsg();
            CWX_ERROR(("Failure to unpack nerve's package, err=%s", m_strErrMsg.c_str()));
        }
        else
        {
            m_bValid = true;
        }
    }
    else
    {
        m_strErrMsg = m_pActions[m_uiIndex].m_strErrMsg;
    }
    m_uiIndex++;
    return 1;
}

char const* CwmCenterActionLoop::val(char const* szName,
                                     bool bAttrName,
                                     CWX_UINT8 ,
                                     char const* ,
                                     int ,
                                     CwxHtmlTmplEscape* ,
                                     char const* ,
                                     CwxReplBase* ,
                                     bool )
{
    char const* pValue = NULL;
    if (memcmp(szName, "host.", strlen("host."))==0)
    {
        pValue = getNerveAttr(m_pNerve, szName+strlen("host."), m_pTss->m_szKeyValue64K);
        if (!pValue) pValue = NULL;
        return pValue;
    }
    else
    {
        if (!m_bValid)
        {
            if(strcmp("name", szName)==0)
            {
                return m_strName.c_str();
            }
            else if (strcmp("status", szName)==0)
            {
                return "-1";
            }
            else if (strcmp("error", szName)==0)
            {
                return m_strErrMsg.c_str();
            }
            else if (strcmp("level", szName)==0)
            {
                return "warning";
            }
            else if (strncmp("record", szName, strlen("record"))==0)
            {
                return "";
            }
        }
        else
        {
            pValue = getActionAttr(m_strName, m_packReader, szName, m_pTss->m_szKeyValue64K);
            if (!pValue) pValue="";
            return pValue;
        }
    }
    if (bAttrName) return "";
    return NULL;
}

CwxHtmlTmplLoop* CwmCenterActionLoop::loop(char const* ,
                                           list<pair<char*, char*> >const& )
{
    return NULL;

}

char const* CwmCenterActionLoop::getNerveAttr(CwmCenterNerveStatus const* pNever,
                                              char const* szAttr,
                                              char* szBuf64k)
{
    if (strcmp(szAttr, "name")==0)
    {
        return pNever->getHostInfo()->getName().c_str();
    }
    else if (strcmp(szAttr, "ip") == 0)
    {
        return pNever->getHostInfo()->getIp().c_str();
    }
    else if (strcmp(szAttr, "port") == 0)
    {
        CwxCommon::snprintf(szBuf64k, 2048, "%d", pNever->getHostInfo()->getPort());
        return szBuf64k;
    }
    else if (strcmp(szAttr, "alarm_rule") == 0)
    {
        return pNever->getHostInfo()->getAlarmRule()->getName().c_str();
    }
    else if (strcmp(szAttr, "alarm_mail") == 0)
    {
        return pNever->getHostInfo()->getMailRule()->getName().c_str();
    }
    return NULL;
}
char const* CwmCenterActionLoop::getActionAttr(string const& strName,
                                               CwxPackageReader const& actionReader,
                                               char const* szAttr,
                                               char* szBuf64k)
{
    if (strcmp(szAttr, "name") == 0) return strName.c_str();
    CwxKeyValueItem const* pItem = actionReader.getKey(szAttr, true);
    if (!pItem) return NULL;
    if (!pItem->m_bKeyValue) return pItem->m_szData;
    CWX_UINT32 uiLen = 64 * 1024;
    char const* szData= pItem->m_szData;
    char* szTag="    ";
    char* szKeyBegin=NULL;
    char* szKeyEnd = "<br>\n";
    if (0>CwxPackage::dump(szData,
        pItem->m_uiDataLen,
        szBuf64k,
        uiLen,
        szTag,
        szKeyBegin,
        szKeyEnd,
        NULL))
    {
        return "";
    }
    return szBuf64k;
}
