#include "CwmCenterActionNameLoop.h"
#include "CwmCenterApp.h"

void CwmCenterActionNameLoop::init(CwmCenterTss* pTss, list<string> const* pActions)
{
    m_bFirst = true;
    m_pTss = pTss;
    m_pActions = pActions;
}


int CwmCenterActionNameLoop::next()
{
    if (m_bFirst)
    {
        m_bFirst = false;
        m_action_iter = m_pActions->begin();
    }
    else
    {
        if (m_action_iter == m_pActions->end()) return 0;
        m_action_iter++;
    }
    if (m_action_iter == m_pActions->end()) return 0;
    return 1;
}

char const* CwmCenterActionNameLoop::val(char const* szName,
                                     bool bAttrName,
                                     CWX_UINT8 ,
                                     char const* ,
                                     int ,
                                     CwxHtmlTmplEscape* ,
                                     char const* ,
                                     CwxReplBase* ,
                                     bool )
{
    if (m_action_iter != m_pActions->end())
    {
        if (strcmp(szName, "name") == 0)
        {
            return m_action_iter->c_str();
        }
    }
    return bAttrName?"":NULL;
}

CwxHtmlTmplLoop* CwmCenterActionNameLoop::loop(char const* ,
                                           list<pair<char*, char*> >const& )
{
    return NULL;

}

