#include "CwmCenterNerveQuery.h"
#include "CwmCenterApp.h"

int CwmCenterNerveQuery::onConnCreated(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    map<CWX_UINT32, CwmCenterNerveStatus*>::iterator iter = m_pApp->getNerves().find(msg->event().getConnId());
    CWX_ASSERT(iter != m_pApp->getNerves().end());
    CWX_DEBUG(("Create a nerve connection, nerve=%s", iter->second->getHostInfo()->getIp().c_str()));

    iter->second->setConnected(true);
    if (iter->second->getStatus().update("0", CWM_CMN_DEFAULT, "Connected"))
    {
        CwmCmnAlarm alarm;
        iter->second->getStatus().formActionAlarm("Center",
            m_pApp->getDataMgr()->nextWid(),
            0,
            alarm);
        iter->second->getStatus().setWid(alarm.getWid());
        m_pApp->getDataMgr()->insert(alarm);
        CWX_DEBUG(("Clear a nerve disconnect alarm, host=%s, wid=%u, pid=%u, system=%s, service=%s, action=%s, type=%s, level=%s, pwid=%u, plevel=%s, err=%s",
            iter->second->getHostInfo()->getIp().c_str(),
            alarm.getWid(),
            alarm.getPid(),
            alarm.getSystem().c_str(),
            alarm.getService().c_str(),
            alarm.getAction().c_str(),
            alarm.getType().c_str(),
            alarm.getAlarmString(alarm.getLevel()).c_str(),
            alarm.getPWid(),
            alarm.getAlarmString(alarm.getPLevel()).c_str(),
            alarm.getError().c_str()));
        CwmCenterAlarmReportor::reportNextAlarm(m_pApp, pThrEnv);
    }
    return 1;
}

int CwmCenterNerveQuery::onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    map<CWX_UINT32, CwmCenterNerveStatus*>::iterator iter = m_pApp->getNerves().find(msg->event().getConnId());
    CWX_ASSERT(iter != m_pApp->getNerves().end());
    CWX_DEBUG(("Close a nerve connection, nerve=%s", iter->second->getHostInfo()->getIp().c_str()));

    list<CwxAppTaskBoardTask*> tasks;
    m_pApp->getTaskBoard().noticeConnClosed(msg, pThrEnv, tasks);
    if (!tasks.empty())
    {
        list<CwxAppTaskBoardTask*>::iterator iter = tasks.begin();
        while(iter != tasks.end())
        {
            (*iter)->execute(pThrEnv);
            iter++;
        }
        tasks.clear();
    }


    iter->second->setConnected(false);
    if (iter->second->getStatus().update("-1", CWM_CMN_DEFAULT, "No Connected"))
    {
        CwmCmnAlarm alarm;
        iter->second->getStatus().formActionAlarm("Center",
            m_pApp->getDataMgr()->nextWid(),
            0,
            alarm);
        iter->second->getStatus().setWid(alarm.getWid());
        m_pApp->getDataMgr()->insert(alarm);
        CWX_DEBUG(("Rise a nerve disconnect alarm, host=%s, wid=%u, pid=%u, system=%s, service=%s, action=%s, type=%s, level=%s, pwid=%u, plevel=%s, err=%s",
            iter->second->getHostInfo()->getIp().c_str(),
            alarm.getWid(),
            alarm.getPid(),
            alarm.getSystem().c_str(),
            alarm.getService().c_str(),
            alarm.getAction().c_str(),
            alarm.getType().c_str(),
            alarm.getAlarmString(alarm.getLevel()).c_str(),
            alarm.getPWid(),
            alarm.getAlarmString(alarm.getPLevel()).c_str(),
            alarm.getError().c_str()));
        CwmCenterAlarmReportor::reportNextAlarm(m_pApp, pThrEnv);
    }
    return 1;
}

int CwmCenterNerveQuery::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    CWX_DEBUG(("Recv a nerve query reply"));
    m_pApp->getTaskBoard().noticeRecvMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

int CwmCenterNerveQuery::onEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    CWX_DEBUG(("Finish send a nerve query msg"));
    m_pApp->getTaskBoard().noticeEndSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

int CwmCenterNerveQuery::onFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    CWX_DEBUG(("Failure to send a nerve query msg"));
    m_pApp->getTaskBoard().noticeFailSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

bool CwmCenterNerveQuery::queryAction(CwmCenterApp* pApp, 
                        CwxAppTss* pThrEnv,
                        CWX_UINT32 uiConnId,
                        CWX_UINT32 uiTaskId,
                        char const* szSystem,
                        char const* szService,
                        char const* szAction)
{
    CwmCenterTss* pTss = (CwmCenterTss*)pThrEnv;
    pTss->m_pSndPackage->beginPack();
    pTss->m_pSndPackage->addKeyValue(CWM_CMN_SYSTEM, szSystem, strlen(szSystem));
    pTss->m_pSndPackage->addKeyValue(CWM_CMN_SERVICE, szService, strlen(szService));
    pTss->m_pSndPackage->addKeyValue(CWM_CMN_ACTION, szAction, strlen(szAction));
    pTss->m_pSndPackage->pack();
    CwxMsgHead local(0, 0, MSG_TYPE_SERVICE_QUERY, uiTaskId, pTss->m_pSndPackage->getMsgSize());
    CwxMsgBlock* block = CwxMsgBlockAlloc::pack(local, pTss->m_pSndPackage->getMsg(), pTss->m_pSndPackage->getMsgSize());
    block->send_ctrl().setSvrId(CwmCenterApp::SVR_TYPE_UI_QUERY);
    block->send_ctrl().setHostId(0);
    block->send_ctrl().setConnId(uiConnId);
    block->event().setTaskId(uiTaskId);
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::FAIL_FINISH_NOTICE);
    if (0 != pApp->sendMsgByConn(block))
    {
        CWX_ERROR(("Failure to send ui query"));
        CwxMsgBlockAlloc::free(block);
        return false;
    }
    return true;
}
