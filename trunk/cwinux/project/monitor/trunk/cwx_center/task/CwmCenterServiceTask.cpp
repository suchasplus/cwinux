#include "CwmCenterServiceTask.h"
#include "CwmCenterSystemLoop.h"
#include "CwmCenterServiceLoop.h"
#include "CwmCenterActionNameLoop.h"
#include "CwmCenterActionLoop.h"
#include "CwmCenterApp.h"

void CwmCenterServiceTask::noticeTimeout(CwxAppTss* )
{
    CWX_DEBUG(("service query task istimeout, task-id=%u", getTaskId()));
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
}

void CwmCenterServiceTask::noticeRecvMsg(CwxMsgBlock*& msg, CwxAppTss* , bool& )
{
    CWX_DEBUG(("recieve a nerve query-reply, task-id=%u", getTaskId()));
    CWX_UINT32 i = 0;
    for (i=0; i<m_uiHostNum; i++)
    {
        if (msg->event().getConnId() == m_pHostAction[i].m_pNerve->getConnId())
        {
            m_pHostAction[i].m_pRecvMsg = msg;
            msg = NULL;
            break;
        }
    }
    CWX_ASSERT(i != m_uiHostNum);
    m_uiRecvMsgNum ++;
    if (m_uiRecvMsgNum == m_uiSendMsgNum)
    {
        this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    }
}

void CwmCenterServiceTask::noticeFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* )
{
    CWX_DEBUG(("A nerve query-msg is failure to send, task-id=%u", getTaskId()));
    CWX_UINT32 i = 0;
    for (i=0; i<m_uiHostNum; i++)
    {
        if (msg->send_ctrl().getConnId() == m_pHostAction[i].m_pNerve->getConnId())
        {
            m_pHostAction[i].m_strErrMsg = "Failure to query nerve, connection is lost";
            break;
        }
    }
    CWX_ASSERT(i != m_uiHostNum);
    m_uiRecvMsgNum ++;
    if (m_uiRecvMsgNum == m_uiSendMsgNum)
    {
        this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    }
}

void CwmCenterServiceTask::noticeEndSendMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
    CWX_DEBUG(("A nerve query-msg is send, task-id=%u", getTaskId()));
}

void CwmCenterServiceTask::noticeConnClosed(CWX_UINT32 , CWX_UINT32 , CWX_UINT32 uiConnId, CwxAppTss* )
{
    CWX_DEBUG(("A nerve query-msg's connection is closed, task-id=%u", getTaskId()));
    CWX_UINT32 i = 0;
    for (i=0; i<m_uiHostNum; i++)
    {
        if (uiConnId == m_pHostAction[i].m_pNerve->getConnId())
        {
            m_pHostAction[i].m_strErrMsg = "Nerve's connection is lost";
            break;
        }
    }
    CWX_ASSERT(i != m_uiHostNum);
    m_uiRecvMsgNum ++;
    if (m_uiRecvMsgNum == m_uiSendMsgNum)
    {
        this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    }
}

int CwmCenterServiceTask::noticeActive(CwxAppTss* pThrEnv)
{
    //fetch action
    m_pSystem = m_pApp->getConfig().getSystem(m_strSystem);
    if (!m_pSystem)
    {
        m_bSuccess = false;
        CwxCommon::snprintf(pThrEnv->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "System [%s] doesn't exist", m_strSystem.c_str());
        m_strErrMsg = pThrEnv->m_szBuf2K;
        CWX_ERROR((m_strErrMsg.c_str()));
        return -1;
    }
    m_pService = m_pSystem->getService(m_strService);
    if (!m_pService)
    {
        m_bSuccess = false;
        CwxCommon::snprintf(pThrEnv->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "Service doesn't exist, System [%s], Service[%s]", m_strSystem.c_str(), m_strService.c_str());
        m_strErrMsg = pThrEnv->m_szBuf2K;
        CWX_ERROR((m_strErrMsg.c_str()));
        return -1;
    }
    if (!m_strAction.length())
    {
        if (m_pService->getActions().begin() != m_pService->getActions().end())
        {
            m_strAction = *m_pService->getActions().begin();
        }
    }

    m_uiHostNum = m_pService->getHosts().size();
    if (0 == m_uiHostNum)
    {
        this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
        CWX_DEBUG(("No host to query, service query task is finished, task-id=%u", getTaskId()));
        return 0;
    }
    m_pHostAction = new CwmCenterActionLoopData[m_uiHostNum];
    m_uiSendMsgNum = 0;
    m_uiRecvMsgNum = 0;
    CWX_UINT32 uiIndex = 0;
    map<string/*ip*/, CwmCenterHost const*>::const_iterator iter = m_pService->getHosts().begin();
    while(iter != m_pService->getHosts().end())
    {
        m_pHostAction[uiIndex].m_pNerve = m_pApp->getNerve(iter->first);
        if (!m_pHostAction[uiIndex].m_pNerve->isConnected())
        {
            m_pHostAction[uiIndex].m_strErrMsg = "Lost connected.";
        }
        else
        {
            //query never
            if (!CwmCenterNerveQuery::queryAction(m_pApp,
                m_pTss,
                m_pHostAction[uiIndex].m_pNerve->getConnId(),
                getTaskId(),
                m_strSystem.c_str(),
                m_strService.c_str(),
                m_strAction.c_str()))
            {
                m_pHostAction[uiIndex].m_strErrMsg = "Failure send query msg";
                CWX_ERROR(("Failure to send service query msg."));
            }
            else
            {
                m_uiSendMsgNum++;
            }
        }
        uiIndex++;
        iter++;
    }
    if (!m_uiSendMsgNum)
    {
        this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
        return 0;
    }
    return 0;
}

void CwmCenterServiceTask::execute(CwxAppTss* pThrEnv)
{
    m_pTss = (CwmCenterTss*) pThrEnv;
    if (CwxAppTaskBoardTask::TASK_STATE_INIT == getTaskState())
    {
        CWX_ERROR(("Start to execute a service query task, task-id=%u", getTaskId()));
        m_bSuccess = true;
        m_strErrMsg.erase();
        getTaskBoard()->noticeActiveTask(this, pThrEnv);
    }
    if (isFinish())
    {
        CWX_ERROR(("Finish to execute a service query task, task-id=%u", getTaskId()));
        reply();
        delete this;
    }
}


int CwmCenterServiceTask::next()
{
    ///全局loop对象没有下一个记录
    return 0;
}

char const* CwmCenterServiceTask::val(char const* szName,
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
    if (strcmp("g_system", szName)==0) return m_strSystem.c_str();
    if (strcmp("g_service", szName)==0) return m_strService.c_str();
    if (strcmp("g_action", szName)==0) return m_strAction.c_str();
    char const* pValue = "";
    if (memcmp(szName, "system.", strlen("system."))==0)
    {
        pValue = CwmCenterSystemLoop::getSystemAttr(m_pSystem, szName+strlen("system."), m_pTss->m_szKeyValue64K);
        if (!pValue) pValue="";
    }
    else
    {
        if (memcmp(szName, "service.", strlen("service."))==0)
        {
            pValue = CwmCenterServiceLoop::getServiceAttr(m_pService, szName+strlen("service."), m_pTss->m_szKeyValue64K);
            if (!pValue) pValue="";
        }
    }
    return pValue;
}

CwxHtmlTmplLoop* CwmCenterServiceTask::loop(char const* szLoop,
                                           list<pair<char*, char*> >const& )
{
    if (strcmp(szLoop, "action")==0) return m_pTss->m_pActionLoop;
    if (strcmp(szLoop, "action_name")==0) return m_pTss->m_pActionNameLoop;
    return NULL;
}

void CwmCenterServiceTask::reply()
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
int  CwmCenterServiceTask::outputTemplate(string& strType)
{
    strType = CWX_CMN_DEF_CONTENT_TYPE;
    if (!m_bSuccess)
    {
        strcpy(m_pTss->m_szOutputBuf, m_strErrMsg.c_str());
        m_pTss->m_uiOutputDataLen = m_strErrMsg.length();
        return -1;
    }
    //get tmpl
    CwmCenterTemplate const* pTmpl = m_pApp->getConfig().getTemplate(CWM_CMN_TMPL_SERVICE);
    if (!pTmpl)
    {
        m_pTss->m_uiOutputDataLen = CwxCommon::snprintf(m_pTss->m_szOutputBuf, 2048, "Not find systems's template [%s]", CWM_CMN_TMPL_SYSTEMS);
        return -1;
    }
    m_pTss->m_pActionNameLoop->init(m_pTss, &m_pService->getActions());
    m_pTss->m_pActionLoop->init(m_pTss, m_strAction, m_pHostAction, m_uiHostNum);
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

