#include "CwmNerveCgiHandler.h"
#include "CwmNerveApp.h"
#include "task/CwmNerveCgiTask.h"
#include "CwmNerveForkEnv.h"

int CwmNerveCgiHandler::onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv) 
{
    CWX_DEBUG(("Cgi's pipe is closed."));
    list<CwxAppTaskBoardTask*> tasks;
    m_pApp->getTaskBoard().noticeConnClosed(msg, pThrEnv, tasks);
    list<CwxAppTaskBoardTask*>::iterator iter = tasks.begin();
    while(iter != tasks.end())
    {
        (*iter)->execute(pThrEnv);
        iter++;
    }
    return 1;
}

int CwmNerveCgiHandler::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    ///连接的USER-DATA就是task-id
    CWX_UINT32 uiTaskId = (long)msg->event().getConnUserData();
    CWX_DEBUG(("Recv msg from Cgi's pipe, task_id=%u", uiTaskId));
    CwxAppTaskBoardTask* pTask = NULL;
    m_pApp->getTaskBoard().noticeRecvMsg(uiTaskId, msg, pThrEnv, pTask);
    CWX_ASSERT(!pTask);
    return 1;
}

bool CwmNerveCgiHandler::queryCgi(CwmNerveApp* pApp,
                                  CwxAppTss* ,
                                  string const& strSystem,
                                  string const& strService,
                                  string const& strAction,
                                  string& strErr,
                                  CwxMsgBlock*& queryMsg)
{
    //获取service名字对应的service
    CwmNerveServiceInfo const* pServiceInfo= pApp->getConfig().getService(strSystem, strService);
    if (!pServiceInfo)
    {
        strErr = "System[";
        strErr += strSystem;
        strErr += "]; Service[";
        strErr += strService;
        strErr += "] doesn't exist.";
        CWX_ERROR((strErr.c_str()));
        return false;
    }
    //根据action的名字，获取action信息对象
    CwmNerveActionInfo const* pActionInfo = pServiceInfo->getAction(strAction);
    if (!pActionInfo)
    {
        strErr ="Action[";
        strErr += strAction;
        strErr += "] doesn't exist.";
        CWX_ERROR((strErr.c_str()));
        return false;
    }
    CwmNerveForkEnv* pForkEnv = new CwmNerveForkEnv(pApp);
    pForkEnv->m_strCgiScript = pActionInfo->getCgiCmd();
    pForkEnv->m_pCgiTask = new CwmNerveCgiTask(pApp, &pApp->getTaskBoard());
    pForkEnv->m_pCgiTask->setTaskId(pApp->nextTaskId());
    pForkEnv->m_pCgiTask->m_pMsg = queryMsg;
    queryMsg = NULL;
    pForkEnv->m_pCgiTask->m_pServiceInfo = pServiceInfo;
    pForkEnv->m_pCgiTask->m_pActionInfo = pActionInfo;
    pForkEnv->m_pCgiTask->setTimeoutValue(CwxDate::getTimestamp() + pApp->getConfig().getTimeout() * 1000000);
    CWX_DEBUG(("Notice forking for cgi"));
    if (0 != pApp->noticeFork(pForkEnv))
    {
        strErr = "Failure to notice fork";
        return false;
    }
    return true;
}

bool CwmNerveCgiHandler::queryCgi(CwmNerveApp* pApp,
                                  CwxAppTss* ,
                                  CwmNerveServiceStatus* pService,
                                  CwmNerveActionStatus* pAction,
                                  string& strErr)
{

    CwmNerveForkEnv* pForkEnv = new CwmNerveForkEnv(pApp);
    pForkEnv->m_strCgiScript = pAction->getActionInfo()->getCgiCmd();
    pForkEnv->m_pCgiTask = new CwmNerveCgiTask(pApp, &pApp->getTaskBoard());
    pForkEnv->m_pCgiTask->setTaskId(pApp->nextTaskId());
    pForkEnv->m_pCgiTask->m_pMsg = NULL;
    pForkEnv->m_pCgiTask->m_pService = pService;
    pForkEnv->m_pCgiTask->m_pServiceInfo = pService->getServiceInfo();
    pForkEnv->m_pCgiTask->m_pAction = pAction;
    pForkEnv->m_pCgiTask->m_pActionInfo = pAction->getActionInfo();
    pForkEnv->m_pCgiTask->setTimeoutValue(CwxDate::getTimestamp() + pApp->getConfig().getTimeout() * 1000000);
    CWX_DEBUG(("Notice forking for cgi"));
    if (0 != pApp->noticeFork(pForkEnv))
    {
        strErr = "Failure to notice fork";
        return false;
    }
    return true;
}


