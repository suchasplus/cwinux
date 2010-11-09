#include "CwmCenterUiQuery.h"
#include "CwmCenterApp.h"
#include "CwmCenterSystemsTask.h"
#include "CwmCenterSystemTask.h"
#include "CwmCenterServiceTask.h"
#include "CwmCenterAlarmReportor.h"

int CwmCenterUiQuery::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    switch(msg->event().getMsgHeader().getMsgType())
    {
    case MSG_TYPE_QUERY:
        uiQuery(msg, pThrEnv);
        break;
    default:
        CwxCommon::snprintf(pThrEnv->m_szBuf2K,
            CwxAppTss::TSS_2K_BUF,
            "No support query msg type, msg_type=%u",
            msg->event().getMsgHeader().getMsgType());
        replyQuery(m_pApp, pThrEnv,
            msg->event().getConnId(),
            msg->event().getMsgHeader(),
            -1,
            CWX_CMN_DEF_CONTENT_TYPE,
            pThrEnv->m_szBuf2K,
            strlen(pThrEnv->m_szBuf2K));
        CWX_ERROR((pThrEnv->m_szBuf2K));
        break;
    }
    return 1;
}


int CwmCenterUiQuery::onTimeoutCheck(CwxMsgBlock*& , CwxAppTss* pThrEnv)
{
    list<CwxAppTaskBoardTask*> tasks;
//    CWX_DEBUG(("Timeout check"));
    m_pApp->getTaskBoard().noticeCheckTimeout(pThrEnv, tasks);
    if (!tasks.empty())
    {
        list<CwxAppTaskBoardTask*>::iterator iter=tasks.begin();
        (*iter)->execute(pThrEnv);
        iter++;
    }
    string strErr;
    bool bAlarm = false;
    CwmCmnAlarm alarm;
    CWX_UINT64 ullNow = CwxDate::getTimestamp();
    //check new poll
    {
        map<CWX_UINT32, CwmCenterNerveStatus*>::iterator iter = m_pApp->getNerves().begin();
        while(iter != m_pApp->getNerves().end())
        {
            if (!iter->second->isConnected() && iter->second->getStatus().isNeedPoll(ullNow))
            {
                iter->second->getStatus().setLastPollTime(ullNow);
                if (iter->second->getStatus().update("-1", CWM_CMN_DEFAULT, "No connected"))
                {
                    iter->second->getStatus().formActionAlarm("Center", m_pApp->getDataMgr()->nextWid(), 0, alarm);
                    iter->second->getStatus().setWid(alarm.getWid());
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
                    m_pApp->getDataMgr()->insert(alarm);
                    bAlarm = true;
                }
            }
            else if(iter->second->getStatus().isFault() && iter->second->getStatus().isNeedPoll(ullNow))
            {
                iter->second->getStatus().setLastPollTime(ullNow);
                if (iter->second->getStatus().update("0", CWM_CMN_DEFAULT, "Connected"))
                {
                    iter->second->getStatus().formActionAlarm("Center", m_pApp->getDataMgr()->nextWid(), 0, alarm);
                    iter->second->getStatus().setWid(alarm.getWid());
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
                    m_pApp->getDataMgr()->insert(alarm);
                    bAlarm = true;
                }
            }
            iter++;
        }
    }
    if (bAlarm)
    {
        CwmCenterAlarmReportor::reportNextAlarm(m_pApp, pThrEnv);
    }
    return 1;
}

void CwmCenterUiQuery::uiQuery(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxPackageReader reader(false);
    if (!reader.unpack(msg->rd_ptr(), msg->length(), true, false))
    {
        CwxCommon::snprintf(pThrEnv->m_szBuf2K,
            CwxAppTss::TSS_2K_BUF,
            "Failure to unpack the query msg, err=%s",
            reader.getErrMsg());
        replyQuery(m_pApp, pThrEnv,
            msg->event().getConnId(),
            msg->event().getMsgHeader(),
            -1,
            CWX_CMN_DEF_CONTENT_TYPE,
            pThrEnv->m_szBuf2K,
            strlen(pThrEnv->m_szBuf2K));
        CWX_ERROR((pThrEnv->m_szBuf2K));
        return ;
    }

    CwxKeyValueItem const* pItem = NULL;
    char const* szCmd="";
    char const* szSystem="";
    char const* szService="";
    char const* szAction="";

    pItem =reader.getKey(CWM_CMN_CMD);
    if (pItem) szCmd = pItem->m_szData;
    pItem = reader.getKey(CWM_CMN_SYSTEM);
    if (pItem) szSystem = pItem->m_szData;
    pItem = reader.getKey(CWM_CMN_SERVICE);
    if (pItem) szService = pItem->m_szData;
    pItem = reader.getKey(CWM_CMN_ACTION);
    if (pItem) szAction = pItem->m_szData;

    if (strcasecmp(szCmd, CWM_CMN_SYSTEM) == 0)
    {//query system
        CWX_DEBUG(("Recv a system ui query"));
        CwmCenterSystemTask* pTask = new CwmCenterSystemTask(m_pApp, &m_pApp->getTaskBoard());
        pTask->setTaskId(m_pApp->nextTaskId());
        pTask->setTimeoutValue(CwxDate::getTimestamp() + m_pApp->getConfig().getQueryTimeout() * 1000000);
        pTask->m_queryMsg = msg;
        pTask->m_strSystem = szSystem;
        msg = NULL;
        pTask->execute(pThrEnv);
    }
    else if (strcasecmp(szCmd, CWM_CMN_SERVICE)==0)
    {//query service
        CWX_DEBUG(("Recv a service ui query"));
        CwmCenterServiceTask* pTask = new CwmCenterServiceTask(m_pApp, &m_pApp->getTaskBoard());
        pTask->setTaskId(m_pApp->nextTaskId());
        pTask->setTimeoutValue(CwxDate::getTimestamp() + m_pApp->getConfig().getQueryTimeout() * 1000000);
        pTask->m_queryMsg = msg;
        pTask->m_strSystem = szSystem;
        pTask->m_strService = szService;
        pTask->m_strAction = szAction;
        msg = NULL;
        pTask->execute(pThrEnv);
    }
    else
    {//query systems
        CWX_DEBUG(("Recv a systems ui query"));
        CwmCenterSystemsTask* pTask = new CwmCenterSystemsTask(m_pApp, &m_pApp->getTaskBoard());
        pTask->setTaskId(m_pApp->nextTaskId());
        pTask->setTimeoutValue(CwxDate::getTimestamp() + m_pApp->getConfig().getQueryTimeout() * 1000000);
        pTask->m_queryMsg = msg;
        msg = NULL;
        pTask->execute(pThrEnv);
    }  
}



void CwmCenterUiQuery::replyQuery(CwmCenterApp* pApp,
                                  CwxAppTss* pThrEnv,
                                  CWX_UINT32 uiConnId,
                                  CwxMsgHead const& recvHeader,
                                  int code,
                                  char const* szType,
                                  char const* szContent,
                                  CWX_UINT32 uiContentLen)
{
    CwmCenterTss* pTss = (CwmCenterTss*)pThrEnv;
    pTss->m_pSndPackage->beginPack();
    pTss->m_pSndPackage->addKeyValue(CWM_CMN_CODE, (CWX_INT32)code);
    pTss->m_pSndPackage->addKeyValue(CWM_CMN_CONTENT_TYPE, szType, strlen(szType));
    pTss->m_pSndPackage->addKeyValue(CWM_CMN_CONTENT, szContent, uiContentLen);
    pTss->m_pSndPackage->pack();
    CwxMsgHead local(recvHeader);
    local.setMsgType(recvHeader.getMsgType() + 1);
    local.setDataLen(pTss->m_pSndPackage->getMsgSize());
    CwxMsgBlock* block = CwxMsgBlockAlloc::pack(local, pTss->m_pSndPackage->getMsg(), pTss->m_pSndPackage->getMsgSize());
    block->send_ctrl().setSvrId(CwmCenterApp::SVR_TYPE_UI_QUERY);
    block->send_ctrl().setHostId(0);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != pApp->sendMsgByConn(block))
    {
        CWX_ERROR(("Failure to send ui query"));
        CwxMsgBlockAlloc::free(block);
    }
}
