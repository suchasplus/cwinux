#include "CwmCenterAlarmReportor.h"
#include "CwmCenterApp.h"
#include "CwmCenterTss.h"

int CwmCenterAlarmReportor::onConnCreated(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CWX_ERROR(("One alarm report connection is created, conn=%u", msg->event().getConnId()));
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        m_connIds.push_back(msg->event().getConnId());
    }
    reportNextAlarm(m_pApp, pThrEnv);
    return 1;
}

int CwmCenterAlarmReportor::onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CWX_ERROR(("One alarm report connection is closed, conn=%u", msg->event().getConnId()));
    bool bNeedReport = false;
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        list<CWX_UINT32>::iterator iter = m_connIds.begin();
        while(iter != m_connIds.end())
        {
            if (*iter == msg->event().getConnId())
            {///若是当前的告警连接，则切换下一条连接继续发送滞留告警
                if (iter == m_connIds.begin())
                {
                    bNeedReport = true;
                }
                m_connIds.erase(iter);
                break;
            }
            iter++;
        }
    }
    ///通知taskbaord，一条连接已经关闭，若有报告告警的task，则会完成
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
    ///发送下一条告警
    if (bNeedReport)
    {
        reportNextAlarm(m_pApp, pThrEnv);
    }
    return 1;
}

int CwmCenterAlarmReportor::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CWX_DEBUG(("Recieve a alarm-report's reply, conn=%u", msg->event().getConnId()));
    CwxAppTaskBoardTask* pTask = NULL;
    m_pApp->getTaskBoard().noticeRecvMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

int CwmCenterAlarmReportor::onEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CWX_DEBUG(("A alarm is finish report, conn=%u", msg->event().getConnId()));
    CwxAppTaskBoardTask* pTask = NULL;
    m_pApp->getTaskBoard().noticeEndSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

int CwmCenterAlarmReportor::onFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CWX_DEBUG(("A alarm is failure to send, conn=%u", msg->event().getConnId()));
    CwxAppTaskBoardTask* pTask = NULL;
    m_pApp->getTaskBoard().noticeFailSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

void CwmCenterAlarmReportor::reportNextAlarm(CwmCenterApp* pApp,
                                             CwxAppTss* pThrEnv)
{
    CwxMutexGuard<CwxMutexLock> lock(&pApp->getAlarmReportor()->getLock());
    if (pApp->getAlarmReportor()->_isEnableReport())
    {
        CwmCmnAlarm alarm;
        if (1 == pApp->getDataMgr()->nextReportAlarm(alarm))
        {
            pApp->getAlarmReportor()->_setReporting(true);
            CwmCenterAlarmTask* pTask = new CwmCenterAlarmTask(pApp, &pApp->getTaskBoard());
            pTask->m_alarm = alarm;
            pTask->m_uiAlarmConnId = pApp->getAlarmReportor()->_getReportConnId();
            pTask->setTaskId(pApp->nextTaskId());
            pTask->setTimeoutValue(CwxDate::getTimestamp() + pApp->getConfig().getAlarmTimeout() * 1000000);
            pTask->execute(pThrEnv);
        }
    }
}

bool CwmCenterAlarmReportor::sendAlarm(CwmCenterApp* pApp,
                                       CwxAppTss* pThrEnv,
                                       CWX_UINT32 uiConnId,
                                       CWX_UINT32 uiTaskId, 
                                       string const& strMails,
                                       CwmCmnAlarm const& alarm)
{
    CwmCenterTss* pTss = (CwmCenterTss*)pThrEnv;
    CwxPackageWriter* pWriter = pTss->m_pSndPackage;
    pWriter->beginPack();
    ///add mails
    pWriter->addKeyValue(CWX_CMN_MAILS, strMails);
    ///add host
    pWriter->addKeyValue(CWM_CMN_HOST, alarm.getHost());
    ///add wid
    pWriter->addKeyValue(CWM_CMN_WID, alarm.getWid());
    ///add pid
    pWriter->addKeyValue(CWM_CMN_PID, alarm.getPid());
    ///add datetime
    pWriter->addKeyValue(CWM_CMN_DATETIME, alarm.getDatetime());
    ///add level
    pWriter->addKeyValue(CWM_CMN_LEVEL, alarm.getLevel());
    ///add system
    pWriter->addKeyValue(CWM_CMN_SYSTEM, alarm.getSystem());
    ///add service
    pWriter->addKeyValue(CWM_CMN_SERVICE, alarm.getService());
    ///add action
    pWriter->addKeyValue(CWM_CMN_ACTION, alarm.getAction());
    ///add type
    pWriter->addKeyValue(CWM_CMN_TYPE, alarm.getType());
    ///add pwid
    pWriter->addKeyValue(CWM_CMN_PWID, alarm.getPWid());
    ///add plevel
    pWriter->addKeyValue(CWM_CMN_PLEVEL, alarm.getPLevel());
    ///add error
    pWriter->addKeyValue(CWM_CMN_ERROR, alarm.getError());
    pWriter->pack();

    CwxMsgHead header(0, 0, MSG_TYPE_ALARM_REPORT, uiTaskId, pWriter->getMsgSize());
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::pack(header, pWriter->getMsg(), pWriter->getMsgSize());
    pBlock->send_ctrl().setConnId(uiConnId);
    pBlock->send_ctrl().setSvrId(CwmCenterApp::SVR_TYPE_ALARM_REPORT);
    pBlock->send_ctrl().setHostId(0);
    pBlock->send_ctrl().setMsgAttr(CwxMsgSendCtrl::FAIL_FINISH_NOTICE);
    pBlock->event().setTaskId(uiTaskId);
    //设置发送包的Task ID，当发送失败或完成时，可以获取。
    pBlock->event().setTaskId(header.getTaskId());
    if (0 != pApp->sendMsgByConn(pBlock))
    {
        CWX_ERROR(("Failure to report alarm msg"));
        CwxMsgBlockAlloc::free(pBlock);
        return false;
    }
    return true;
}
