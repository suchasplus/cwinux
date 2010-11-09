#include "CwmNerveServerHandler.h"
#include "CwmNerveServerTask.h"
#include "CwmNerveApp.h"
int CwmNerveServerHandler::onConnCreated(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    list<CwmNerveServiceStatus*>::iterator iter= m_pApp->getServices().begin();
    while(iter != m_pApp->getServices().end())
    {
        if (msg->event().getConnId() == (*iter)->getConnId())
        {
            CWX_DEBUG(("Server is connected, service=%s", (*iter)->getServiceInfo()->getName().c_str()));
            (*iter)->setConnected(true);
            if ((*iter)->getSelfStatus().update("0", CWM_CMN_DEFAULT, "Connected"))
            {
                CwmCmnAlarm alarm;
                (*iter)->getSelfStatus().formActionAlarm(m_pApp->getConfig().getHostName(),
                    m_pApp->getDataMgr()->nextWid(),
                    0,
                    alarm);
                (*iter)->getSelfStatus().setWid(alarm.getWid());
                m_pApp->getDataMgr()->insert(alarm);
                CWX_DEBUG(("Clear server's disconnect alarm, service=%s", (*iter)->getServiceInfo()->getName().c_str()));
                CwmNerveAlarmHandler::reportNextAlarm(m_pApp, pThrEnv);
            }
            break;
        }
        iter++;
    }
    return 1;
}

int CwmNerveServerHandler::onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv) 
{
    list<CwmNerveServiceStatus*>::iterator iter= m_pApp->getServices().begin();
    while(iter != m_pApp->getServices().end())
    {
        if (msg->event().getConnId() == (*iter)->getConnId())
        {
            CWX_DEBUG(("Server is disconnected, service=%s", (*iter)->getServiceInfo()->getName().c_str()));
            (*iter)->setConnected(false);
            if ((*iter)->getSelfStatus().update("-1", CWM_CMN_DEFAULT, "No connected"))
            {
                CwmCmnAlarm alarm;
                (*iter)->getSelfStatus().formActionAlarm(m_pApp->getConfig().getHostName(),
                    m_pApp->getDataMgr()->nextWid(),
                    0,
                    alarm);
                (*iter)->getSelfStatus().setWid(alarm.getWid());
                m_pApp->getDataMgr()->insert(alarm);
                CWX_DEBUG(("upper server's disconnect alarm, service=%s", (*iter)->getServiceInfo()->getName().c_str()));
                CwmNerveAlarmHandler::reportNextAlarm(m_pApp, pThrEnv);
            }
            break;
        }
        iter++;
    }
    return 1;
}

int CwmNerveServerHandler::onEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    CWX_DEBUG(("End to send query server msg"));
    m_pApp->getTaskBoard().noticeEndSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

int CwmNerveServerHandler::onFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    CWX_DEBUG(("Failure to send query server msg"));
    m_pApp->getTaskBoard().noticeFailSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

int CwmNerveServerHandler::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    CWX_DEBUG(("Recv server's reply msg"));
    m_pApp->getTaskBoard().noticeRecvMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask)
    {
        CWX_DEBUG(("Task[%u] is finished", pTask->getTaskId()));
        pTask->execute(pThrEnv);
    }
    return 1;
}
bool CwmNerveServerHandler::queryServer(CwxAppTss* pThrEnv,
                                        CwmNerveApp* pApp,
                                        string const& strSystem,
                                        string const& strService,
                                        string const& strAction,
                                        string& strErr,
                                        CwxMsgBlock*& queryMsg)
{
    CwmNerveServiceStatus* pService = NULL;
    CwmNerveActionStatus* pAction = NULL;
    {
        list<CwmNerveServiceStatus*>::iterator iter= pApp->getServices().begin();
        while(iter != pApp->getServices().end())
        {
            if (((*iter)->getServiceInfo()->getName() == strService) &&
                ((*iter)->getServiceInfo()->getSystem() == strSystem))
            {
               pService = *iter;
               break;
            }
            iter++;
        }
    }
    if (!pService)
    {
        strErr = "System[";
        strErr +=strSystem;
        strErr +="], Service[";
        strErr += strService;
        strErr += "] doesn't exist.";
        CWX_ERROR((strErr.c_str()));
        return false;
    }
    if (!pService->isConnected())
    {
        strErr = "Service[";
        strErr += strService;
        strErr += "] is disconnected.";
        CWX_ERROR((strErr.c_str()));
        return false;
    }
    {
        list<CwmNerveActionStatus*>::iterator iter=pService->getActionStatus().begin();
        while(iter != pService->getActionStatus().end())
        {
            if ((*iter)->getActionInfo()->getName() == strAction)
            {
                pAction = *iter;
                break;
            }
            iter++;
        }
    }
    if (!pAction)
    {
        strErr ="Action[";
        strErr += strAction;
        strErr += "] doesn't exist.";
        CWX_ERROR((strErr.c_str()));
        return false;
    }
    return queryServer(pThrEnv, pApp, pService, pAction, strErr, queryMsg);
}

bool CwmNerveServerHandler::queryServer(CwxAppTss* pThrEnv,
                                        CwmNerveApp* pApp,
                                        CwmNerveServiceStatus* pService,
                                        CwmNerveActionStatus* pAction,
                                        string& ,
                                        CwxMsgBlock*& queryMsg)
{
    CwmNerveServerTask* pTask = new CwmNerveServerTask(pApp, &pApp->getTaskBoard());
    pTask->setTaskId(pApp->nextTaskId());
    pTask->setTimeoutValue(CwxDate::getTimestamp() + pApp->getConfig().getTimeout() * 1000000);
    pTask->m_queryMsg = queryMsg;
    pTask->m_pAction = pAction;
    pTask->m_pService = pService;
    pTask->execute(pThrEnv);
    queryMsg = NULL;
    return true;
}

bool CwmNerveServerHandler::execQuery(CwxAppTss* pThrEnv, CwmNerveApp* pApp, CwmNerveServiceStatus* pService, CwmNerveActionStatus* pAction, CWX_UINT32 uiTaskId)
{

    CwmNerveTss* pTss = (CwmNerveTss*)pThrEnv;
    CwxPackageWriter* pWriter = pTss->getSndPackage();
    pWriter->beginPack();
    list<pair<string, string> >::const_iterator iter = pAction->getActionInfo()->getArg().begin();
    while(iter != pAction->getActionInfo()->getArg().end())
    {
        pWriter->addKeyValue(iter->first.c_str(), iter->second);
        iter++;
    }
    pWriter->pack();

    CwxMsgHead header(0, 0, pAction->getActionInfo()->getMsgType(), uiTaskId, pWriter->getMsgSize());
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::pack(header, pWriter->getMsg(), pWriter->getMsgSize());
    pBlock->send_ctrl().setConnId(pService->getConnId());
    pBlock->send_ctrl().setSvrId(CwmNerveApp::SVR_TYPE_SERVER_ACTION);
    pBlock->send_ctrl().setHostId(0);
    pBlock->send_ctrl().setMsgAttr(CwxMsgSendCtrl::FAIL_FINISH_NOTICE);
    //设置发送包的Task ID，当发送失败或完成时，可以获取。
    pBlock->event().setTaskId(header.getTaskId());
    if (0 != pApp->sendMsgByConn(pBlock))
    {
        CWX_ERROR(("Failure to send server query msg"));
        CwxMsgBlockAlloc::free(pBlock);
        return false;
    }
    return true;
}

bool CwmNerveServerHandler::analyseReply(CwxAppTss* pThrEnv, int& status, string& strLevel, CwxKeyValueItem & record, string& strErr)
{
    CwmNerveTss* pTss = (CwmNerveTss*)pThrEnv;
    CwxPackageReader reader(true);
    CwxKeyValueItem const*  pItem = NULL;
    if (!reader.unpack(pTss->getBuf(), pTss->getBufDataLen(), false, true))
    {
        CwxCommon::snprintf(pTss->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "Failure to unpack package, reason=%s", reader.getErrMsg());
        strErr = pTss->m_szBuf2K;
        return false;
    }
    //get CWM_CMN_STATUS
    CWX_INT32 iTmp = 0;
    if (!reader.getKey(CWM_CMN_STATUS, iTmp))
    {
        CwxCommon::snprintf(pTss->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "No key[%s] in reply data", CWM_CMN_STATUS);
        strErr = pTss->m_szBuf2K;
        return false;
    }
    status = iTmp;
    if (0 != status) status = -1;
    //get CWM_CMN_LEVEL
    if (!reader.getKey(CWM_CMN_LEVEL, strLevel))
    {
        CwxCommon::snprintf(pTss->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "No key[%s] in reply data", CWM_CMN_LEVEL);
        strErr = pTss->m_szBuf2K;
        return false;
    }
    if (!CwmCmnAlarm::isExistAlarm(strLevel))
    {
        strLevel = CWM_CMN_DEFAULT;
    }
    //get CWM_CMN_RECORD
    pItem = reader.getKey(CWM_CMN_RECORD);
    if (pItem)
    {
        record = *pItem;
    }
    else
    {
        record.reset();
    }
    //get errmsg
    if (!reader.getKey(CWM_CMN_ERROR, strErr))
    {
        strErr = "No error msg";
    }
    return true;
    
}
