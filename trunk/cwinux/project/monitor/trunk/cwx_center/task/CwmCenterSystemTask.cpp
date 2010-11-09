#include "CwmCenterSystemTask.h"
#include "CwmCenterApp.h"
#include "CwmCenterSystemLoop.h"
#include "CwmCenterServiceLoop.h"
void CwmCenterSystemTask::noticeTimeout(CwxAppTss* )
{
    ///不需要taskboard的管理
//    CWX_ASSERT(0);
}

void CwmCenterSystemTask::noticeRecvMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
}

void CwmCenterSystemTask::noticeFailSendMsg(CwxMsgBlock*& , CwxAppTss* )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
}

void CwmCenterSystemTask::noticeEndSendMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
}

void CwmCenterSystemTask::noticeConnClosed(CWX_UINT32 , CWX_UINT32 , CWX_UINT32 , CwxAppTss* )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
}

int CwmCenterSystemTask::noticeActive(CwxAppTss* )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
    return 0;
}

void CwmCenterSystemTask::execute(CwxAppTss* pThrEnv)
{
    CWX_DEBUG(("Start a system query, system=%s, task-id=%u", m_strSystem.c_str(), getTaskId()));
    m_pTss = (CwmCenterTss*) pThrEnv;
    ///回复查询结构
    reply();
    ///删除自己
    delete this;
}


int CwmCenterSystemTask::next()
{
    ///没有下一个记录
    return 0;
}

char const* CwmCenterSystemTask::val(char const* szName,
                                      bool,
                                      CWX_UINT8 ,
                                      char const* ,
                                      int ,
                                      CwxHtmlTmplEscape* ,
                                      char const* ,
                                      CwxReplBase* ,
                                      bool )
{
    if (strcmp("g_retcode", szName)==0) return "0";
    if (strcmp("g_system", szName)==0) return m_strSystem.c_str();
    if (strcmp("g_url", szName) == 0) return m_pApp->getConfig().getWebUrl().c_str();
    char const* pValue = "";
    if (memcmp(szName, "system.", strlen("system."))==0)
    {
        pValue = CwmCenterSystemLoop::getSystemAttr(m_pSystem, szName+strlen("system."), m_pTss->m_szKeyValue64K);
        if (!pValue) pValue="";
    }
    return pValue;
}

CwxHtmlTmplLoop* CwmCenterSystemTask::loop(char const* szLoop,
                                            list<pair<char*, char*> >const& )
{
    if (strcmp(szLoop, "service")==0) return m_pTss->m_pServiceLoop;
    return NULL;
}

void CwmCenterSystemTask::reply()
{
    string strType;
    int iRet = outputTemplate(strType);
    CwmCenterUiQuery::replyQuery(m_pApp,
        m_pTss,
        m_queryMsg->event().getConnId(),
        m_queryMsg->event().getMsgHeader(),
        iRet,
        strType.c_str(),
        m_pTss->m_szOutputBuf,
        m_pTss->m_uiOutputDataLen        
        );
}

int CwmCenterSystemTask::outputTemplate(string& strType)
{
    strType = CWX_CMN_DEF_CONTENT_TYPE;
    //get tmpl
    CwmCenterTemplate const* pTmpl = m_pApp->getConfig().getTemplate(CWM_CMN_TMPL_SYSTEM);
    if (!pTmpl)
    {
        m_pTss->m_uiOutputDataLen = CwxCommon::snprintf(m_pTss->m_szOutputBuf, 2048, "Not find systems's template [%s]", CWM_CMN_TMPL_SYSTEMS);
        return -1;
    }
    m_pSystem = m_pApp->getConfig().getSystem(m_strSystem);
    if (!m_pSystem)
    {
        m_pTss->m_uiOutputDataLen = CwxCommon::snprintf(m_pTss->m_szOutputBuf, 2048, "System[%s] doesn't exist.", m_strSystem.c_str());
        CWX_ERROR((m_pTss->m_szOutputBuf));
        return -1;
    }
    m_pTss->m_pServiceLoop->init(m_pTss, &m_pSystem->getServices());
    CwxHtmlTmplEnv env;
    env.init(this, m_pTss->m_szOutputBuf, CwmCenterTss::MAX_PACKAGE_SIZE);

    if (!pTmpl->getParser()->output(env))
    {
        strcpy(m_pTss->m_szOutputBuf, env.getErrMsg());
        m_pTss->m_uiOutputDataLen = strlen(m_pTss->m_szOutputBuf);
        CWX_ERROR(("Failure to output template, task-id=%u, err=%s", getTaskId(), m_pTss->m_szOutputBuf));
        return -1;
    }
    else
    {
        m_pTss->m_uiOutputDataLen = env.getBufLen();
        strType = pTmpl->getType();
    }
    return 0;
}
