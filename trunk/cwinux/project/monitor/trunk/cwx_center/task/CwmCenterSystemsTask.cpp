#include "CwmCenterSystemsTask.h"
#include "CwmCenterApp.h"
#include "CwmCenterSystemLoop.h"

void CwmCenterSystemsTask::noticeTimeout(CwxAppTss* )
{
    ///不需要taskboard的管理
}

void CwmCenterSystemsTask::noticeRecvMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
}

void CwmCenterSystemsTask::noticeFailSendMsg(CwxMsgBlock*& , CwxAppTss* )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
}

void CwmCenterSystemsTask::noticeEndSendMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
}

void CwmCenterSystemsTask::noticeConnClosed(CWX_UINT32 , CWX_UINT32 , CWX_UINT32 , CwxAppTss* )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
}

int CwmCenterSystemsTask::noticeActive(CwxAppTss* )
{
    ///不需要taskboard的管理
    CWX_ASSERT(0);
    return 0;
}

void CwmCenterSystemsTask::execute(CwxAppTss* pThrEnv)
{
    m_pTss = (CwmCenterTss*) pThrEnv;
    CWX_DEBUG(("Execute a system's query, task-id=%u", getTaskId()));
    reply();
    ///删除自己
    delete this;
}


int CwmCenterSystemsTask::next()
{
    ///不是loop对喜爱那个
    return 0;
}

char const* CwmCenterSystemsTask::val(char const* szName,
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
    if (strcmp("g_url", szName) == 0) return m_pApp->getConfig().getWebUrl().c_str();
    return "";
}

CwxHtmlTmplLoop* CwmCenterSystemsTask::loop(char const* szLoop,
                              list<pair<char*, char*> >const& )
{
    if (strcmp(szLoop, "system")==0) return m_pTss->m_pSystemLoop;
    return NULL;

}

void CwmCenterSystemsTask::reply()
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

int CwmCenterSystemsTask::outputTemplate(string& strType)
{
    strType = CWX_CMN_DEF_CONTENT_TYPE;
    //get tmpl
    CwmCenterTemplate const* pTmpl = m_pApp->getConfig().getTemplate(CWM_CMN_TMPL_SYSTEMS);
    if (!pTmpl)
    {
        m_pTss->m_uiOutputDataLen = CwxCommon::snprintf(m_pTss->m_szOutputBuf, 2048, "Not find systems's template [%s]", CWM_CMN_TMPL_SYSTEMS);
        return -1;
    }
    m_pTss->m_pSystemLoop->init(m_pTss, &m_pApp->getConfig().getSystems());
    CwxHtmlTmplEnv env;
    env.init(this, m_pTss->m_szOutputBuf, CwmCenterTss::MAX_PACKAGE_SIZE);

    if (!pTmpl->getParser()->output(env))
    {
        strcpy(m_pTss->m_szOutputBuf, env.getErrMsg());
        m_pTss->m_uiOutputDataLen = strlen(m_pTss->m_szOutputBuf);
        return -1;
    }
    else
    {
        m_pTss->m_uiOutputDataLen = env.getBufLen();
        strType = pTmpl->getType();
    }
    return 0;
}


