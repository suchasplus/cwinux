#include "CwmNerveQueryHandler.h"
#include "CwmNerveApp.h"

int CwmNerveQueryHandler::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CWX_DEBUG(("Recieve query"));
    switch(msg->event().getMsgHeader().getMsgType())
    {
    case MSG_TYPE_ONLINE_DATA:
        queryOnlineData(msg, pThrEnv);
        break;
    default:
        CWX_ERROR(("No support query msg type, msg_type=%u", msg->event().getMsgHeader().getMsgType()));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        break;

    }
    return 1;
}


int CwmNerveQueryHandler::onTimeoutCheck(CwxMsgBlock*& , CwxAppTss* pThrEnv)
{
    //    list<CwmNerveServiceStatus*>& getServices();
    //check timeout task
    list<CwxAppTaskBoardTask*> tasks;
    m_pApp->getTaskBoard().noticeCheckTimeout(pThrEnv, tasks);
    if (!tasks.empty())
    {
        list<CwxAppTaskBoardTask*>::iterator iter=tasks.begin();
        CWX_DEBUG(("Task[%u] is timeout", (*iter)->getTaskId()));
        (*iter)->execute(pThrEnv);
        iter++;
    }
    string strErr;
    bool bRet = false;
    bool bAlarm = false;
    CwmCmnAlarm alarm;
    //check new poll
    {
        pid_t pid = getpid();
        list<CwmNerveServiceStatus*>::iterator iter = m_pApp->getServices().begin();
        list<CwmNerveActionStatus*>::iterator act_iter;
        CWX_UINT64 ullNow = CwxDate::getTimestamp();
        while (iter != m_pApp->getServices().end())
        {
            if ((*iter)->getServiceInfo()->isCgi())
            {
                act_iter = (*iter)->getActionStatus().begin();
                while(act_iter != (*iter)->getActionStatus().end())
                {
                    if (!(*act_iter)->getAlarmStatus().isPolling() &&
                        (*act_iter)->getAlarmStatus().isNeedPoll(ullNow)&&
                        (*act_iter)->getActionInfo()->isPoll())
                    {
                        CWX_DEBUG(("Begin poll, service=%s, action=%s", (*iter)->getServiceInfo()->getName().c_str(), (*act_iter)->getActionInfo()->getName().c_str()));
                        (*act_iter)->getAlarmStatus().setLastPollTime(ullNow);
                        (*act_iter)->getAlarmStatus().setPolling(true);
                        bRet = CwmNerveCgiHandler::queryCgi(m_pApp, pThrEnv, *iter, *act_iter, strErr);
                        if (pid != getpid()) return 1;
                        if (!bRet)
                        {
                            CWX_ERROR(("Failure to poll, service=%s, action=%s, error=%s", (*iter)->getServiceInfo()->getName().c_str(), (*act_iter)->getActionInfo()->getName().c_str(), strErr.c_str()));
                            (*act_iter)->getAlarmStatus().setPolling(false);
                            if ((*act_iter)->getAlarmStatus().update("-1", CWM_CMN_DEFAULT, strErr))
                            {
                                (*act_iter)->getAlarmStatus().formActionAlarm(m_pApp->getConfig().getHostName(),
                                    m_pApp->getDataMgr()->nextWid(),
                                    m_pApp->getDataMgr()->nextPid(),
                                    alarm);
                                (*act_iter)->getAlarmStatus().setWid(alarm.getWid());
                                m_pApp->getDataMgr()->insert(alarm);
                                bAlarm = true;
                            }
                        }
                    }
                    act_iter++;
                }
            }
            else if ((*iter)->isConnected())
            {
                if ((*iter)->getSelfStatus().isFault() &&
                    (*iter)->getSelfStatus().isNeedPoll(ullNow))
                {//connect alarm
                    (*iter)->getSelfStatus().setLastPollTime(ullNow);
                    if ((*iter)->getSelfStatus().update("0", CWM_CMN_DEFAULT, "Connected"))
                    {
                        CWX_DEBUG(("Service's disconnect alarm clear, service[%s]", (*iter)->getServiceInfo()->getName().c_str()));
                        (*iter)->getSelfStatus().formActionAlarm(m_pApp->getConfig().getHostName(),
                            m_pApp->getDataMgr()->nextWid(),
                            0,
                            alarm);
                        (*iter)->getSelfStatus().setWid(alarm.getWid());
                        m_pApp->getDataMgr()->insert(alarm);
                        bAlarm = true;
                    }
                }
                act_iter = (*iter)->getActionStatus().begin();
                while(act_iter != (*iter)->getActionStatus().end())
                {
                    if (!(*act_iter)->getAlarmStatus().isPolling() &&
                        (*act_iter)->getAlarmStatus().isNeedPoll(ullNow)&&
                        (*act_iter)->getActionInfo()->isPoll())
                    {
                        CWX_DEBUG(("Begin poll, service=%s, action=%s", (*iter)->getServiceInfo()->getName().c_str(), (*act_iter)->getActionInfo()->getName().c_str()));
                        (*act_iter)->getAlarmStatus().setLastPollTime(ullNow);
                        (*act_iter)->getAlarmStatus().setPolling(true);
                        CwxMsgBlock* dummy_block=NULL;
                        bRet = CwmNerveServerHandler::queryServer(pThrEnv,
                            m_pApp,
                            *iter,
                            *act_iter,
                            strErr,
                            dummy_block);
                        if (!bRet)
                        {
                            CWX_ERROR(("Failure to poll, service=%s, action=%s, error=%s", (*iter)->getServiceInfo()->getName().c_str(), (*act_iter)->getActionInfo()->getName().c_str(), strErr.c_str()));
                            (*act_iter)->getAlarmStatus().setPolling(false);
                            if ((*act_iter)->getAlarmStatus().update("-1", CWM_CMN_DEFAULT, strErr))
                            {
                                (*act_iter)->getAlarmStatus().formActionAlarm(m_pApp->getConfig().getHostName(),
                                    m_pApp->getDataMgr()->nextWid(),
                                    m_pApp->getDataMgr()->nextPid(),
                                    alarm);
                                (*act_iter)->getAlarmStatus().setWid(alarm.getWid());
                                m_pApp->getDataMgr()->insert(alarm);
                                bAlarm = true;
                            }
                        }
                    }
                    act_iter++;
                }
            }
            else if ((*iter)->getSelfStatus().isNeedPoll(ullNow))
            {//not connected
                (*iter)->getSelfStatus().setLastPollTime(ullNow);
                if ((*iter)->getSelfStatus().update("-1", CWM_CMN_DEFAULT, "No connected"))
                {
                    (*iter)->getSelfStatus().formActionAlarm(m_pApp->getConfig().getHostName(),
                        m_pApp->getDataMgr()->nextWid(),
                        m_pApp->getDataMgr()->nextPid(),
                        alarm);
                    (*iter)->getSelfStatus().setWid(alarm.getWid());
                    CWX_DEBUG(("Server's disconnect alarm is upper, service[%s],level[%s]", alarm.getService().c_str(), CwmCmnAlarm::getAlarmString(alarm.getLevel()).c_str()));
                    m_pApp->getDataMgr()->insert(alarm);
                    bAlarm = true;
                }
            }
            iter++;
        }
    }
    if (bAlarm)
    {
        CwmNerveAlarmHandler::reportNextAlarm(m_pApp, pThrEnv);
    }
    return 1;
}

void CwmNerveQueryHandler::queryOnlineData(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxPackageReader reader(false);
    //对收到的数据包解包
    if (!reader.unpack(msg->rd_ptr(), msg->length(), false, false))
    {
        CWX_ERROR(("Failure to unpack query msg, err=%s", reader.getErrMsg()));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return;
    }
    string strSystem;
    string strService;
    string strAction;
    CwxKeyValueItem const* pItem = NULL;
    ///get system
    pItem = reader.getKey(CWM_CMN_SYSTEM);
    if (!pItem)
    {
        CWX_ERROR(("Key[%s] doesn't exist in query-online-data package", CWM_CMN_SYSTEM));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return;
    }
    strSystem = pItem->m_szData;

    ///get service
    pItem = reader.getKey(CWM_CMN_SERVICE);
    if (!pItem)
    {
        CWX_ERROR(("Key[%s] doesn't exist in query-online-data package", CWM_CMN_SERVICE));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return;
    }
    strService = pItem->m_szData;
    ///get action
    pItem = reader.getKey(CWM_CMN_ACTION);
    if (!pItem)
    {
        CWX_ERROR(("Key[%s] doesn't exist in query-online-data package", CWM_CMN_ACTION));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return;
    }

    strAction = pItem->m_szData;
    CwmNerveServiceStatus* pService = m_pApp->getService(strSystem, strService);
    if (!pService)
    {
        CWX_ERROR(("Service[%s] doesn't exist.", strService.c_str()));
        replyError(m_pApp, msg, pThrEnv, "Service does not exist.");
        return;
    }
    CwmNerveActionInfo const* pActionInfo = pService->getServiceInfo()->getAction(strAction);
    if (!pActionInfo)
    {
        CWX_ERROR(("Action[%s] doesn't exist in Service[%s].", strAction.c_str(), strService.c_str()));
        replyError(m_pApp, msg, pThrEnv, "Action does not exist.");
        return;
    }
    bool bRet = true;
    string strErrMsg;
    pid_t pid = getpid();
    if (pService->getServiceInfo()->isCgi())
    {
        bRet = CwmNerveCgiHandler::queryCgi(m_pApp,
            pThrEnv,
            strSystem,
            strService,
            strAction,
            strErrMsg,
            msg);
        if (pid != getpid()) return;
    }else
    {
        if (!pService->isConnected())
        {
            CWX_ERROR(("Service[%s]'s server[ip=%s, port=%u] isn't connected.", strService.c_str(), pService->getServiceInfo()->getServerIp().c_str(), pService->getServiceInfo()->getPort()));
            replyError(m_pApp, msg, pThrEnv, "Server is losed.");
            return;
        }
        bRet = CwmNerveServerHandler::queryServer(pThrEnv,
            m_pApp,
            strSystem,
            strService,
            strAction,
            strErrMsg,
            msg);
    }
    if (!bRet)
    {
        replyError(m_pApp, msg, pThrEnv, strErrMsg.c_str());
    }
}

void CwmNerveQueryHandler::replyError(CwmNerveApp* pApp,
                                      CwxMsgBlock*& msg,
                                      CwxAppTss* pThrEnv,
                                      char const* szErrMsg)
{
    CwmNerveTss* pTss = (CwmNerveTss*)pThrEnv;
    CwxPackageWriter* pWriter = pTss->getSndPackage();
    pWriter->beginPack();
    ///add status
    pWriter->addKeyValue(CWM_CMN_STATUS, (CWX_INT32)-1);
    ///add level
    string strAlarm = CwmCmnAlarm::getAlarmString(CwmCmnAlarm::ALARM_WARNING);
    pWriter->addKeyValue(CWM_CMN_LEVEL, strAlarm);
    ///add record
    pWriter->addKeyValue(CWM_CMN_RECORD, "", 0, true);
    ///add ret_err
    pWriter->addKeyValue(CWM_CMN_ERROR, szErrMsg, strlen(szErrMsg), false);
    pWriter->pack();

    CwxMsgHead header(0,
        0,
        msg->event().getMsgHeader().getMsgType() + 1,
        msg->event().getMsgHeader().getTaskId(),
        pWriter->getMsgSize());
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::pack(header, pWriter->getMsg(), pWriter->getMsgSize());
    pBlock->send_ctrl().setConnId(msg->event().getConnId());
    pBlock->send_ctrl().setSvrId(msg->event().getSvrId());
    pBlock->send_ctrl().setHostId(msg->event().getHostId());
    pBlock->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != pApp->sendMsgByConn(pBlock))
    {
        CWX_ERROR(("Failure to reply error msg"));
        CwxMsgBlockAlloc::free(pBlock);
    }
}

void CwmNerveQueryHandler::replyQuery(CwmNerveApp* pApp,
                                      CwxMsgBlock*& msg,
                                      CwxAppTss* pThrEnv,
                                      int status,
                                      string const& strLevel,
                                      string const& strErr,
                                      CwxKeyValueItem& record)
{
    CwmNerveTss* pTss = (CwmNerveTss*)pThrEnv;
    CwxPackageWriter* pWriter = pTss->getSndPackage();
    pWriter->beginPack();
    ///add status
    pWriter->addKeyValue(CWM_CMN_STATUS, status);
    ///add level
    pWriter->addKeyValue(CWM_CMN_LEVEL, strLevel);
    ///add record
    pWriter->addKeyValue(CWM_CMN_RECORD, record.m_szData, record.m_uiDataLen, true);
    ///add alarm_err
    pWriter->addKeyValue(CWM_CMN_ERROR, strErr);
    pWriter->pack();

    CwxMsgHead header(0,
        0,
        msg->event().getMsgHeader().getMsgType() + 1,
        msg->event().getMsgHeader().getTaskId(),
        pWriter->getMsgSize());
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::pack(header, pWriter->getMsg(), pWriter->getMsgSize());
    pBlock->send_ctrl().setConnId(msg->event().getConnId());
    pBlock->send_ctrl().setSvrId(msg->event().getSvrId());
    pBlock->send_ctrl().setHostId(msg->event().getHostId());
    pBlock->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != pApp->sendMsgByConn(pBlock))
    {
        CWX_ERROR(("Failure to reply error msg"));
        CwxMsgBlockAlloc::free(pBlock);
    }
}
