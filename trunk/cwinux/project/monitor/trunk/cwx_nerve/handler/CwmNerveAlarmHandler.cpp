#include "CwmNerveAlarmHandler.h"
#include "CwmNerveApp.h"
#include "CwmNerveAlarmTask.h"
int CwmNerveAlarmHandler::onConnCreated(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CWX_DEBUG(("Alarm report's connection is created"));
    ///当告警报告的连接建立时，保存告警的连接ID
    m_uiAlarmConnId = msg->event().getConnId();
    ///检查并发送滞留的未发送告警
    reportNextAlarm(m_pApp, pThrEnv);
    return 1;
}

int CwmNerveAlarmHandler::onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv) 
{
    CWX_DEBUG(("Alarm report's connection is closed"));
    list<CwxAppTaskBoardTask*> tasks;
    ///设置告警的连接无效
    m_uiAlarmConnId = CWX_APP_INVALID_CONN_ID;
    ///通知taskboard，报告告警的连接已经关闭
    m_pApp->getTaskBoard().noticeConnClosed(msg, pThrEnv, tasks);
    ///若有待回复的告警，则应该返回告警报告的task
    if (!tasks.empty())
    {
        list<CwxAppTaskBoardTask*>::iterator iter = tasks.begin();
        while(iter != tasks.end())
        {
            CWX_DEBUG(("Alarm task is finished for conn-closed."));
            ///由于告警是串行报告，因此最多只有一个task
            (*iter)->execute(pThrEnv);
            iter++;
        }
        tasks.clear();
    }
    return 1;
}

int CwmNerveAlarmHandler::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    ///通知taskboard收到告警的回复
    CWX_DEBUG(("Recv alarm report's reply, task-id=%u", msg->event().getTaskId()));
    int ret = m_pApp->getTaskBoard().noticeRecvMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (-1 == ret)
    {
        CWX_DEBUG(("Alarm report's task doesn't exist, task-id=%u", msg->event().getTaskId()));
    }
    else if (0 == ret)
    {
        CWX_DEBUG(("Alarm report's task doesn't finish, task-id=%u", msg->event().getTaskId()));
    }
    else
    {
        CWX_ASSERT(pTask);
        CWX_DEBUG(("Alarm report's task has finished.task-id=%u", msg->event().getTaskId()));
        pTask->execute(pThrEnv);
    }
    return 1;
}

int CwmNerveAlarmHandler::onEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    CWX_DEBUG(("Alarm repot's msg is sent, task-id=%u", msg->event().getTaskId()));
    m_pApp->getTaskBoard().noticeEndSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask)
    {
        CWX_DEBUG(("Task has finished for msg-end-sent, it's strange. task-id=%u", msg->event().getTaskId()));
        pTask->execute(pThrEnv);
    }
    return 1;
}

int CwmNerveAlarmHandler::onFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    CWX_DEBUG(("Alarm repot's msg is failure to be sent, task-id=%u", msg->event().getTaskId()));
    m_pApp->getTaskBoard().noticeFailSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

void CwmNerveAlarmHandler::reportNextAlarm(CwmNerveApp* pApp, CwxAppTss* pThrEnv)
{
    ///若连接无效或上一个报告的告警没有回复，则不发送下一个告警
    if (pApp->getAlarmHandler()->isReportReply() &&
        (CWX_APP_INVALID_CONN_ID != pApp->getAlarmHandler()->getAlarmConnId()))
    {
        CwmCmnAlarm alarm;
        ///若有存在的滞留告警，则进入发送
        if (1 == pApp->getDataMgr()->nextReportAlarm(alarm))
        {
            CwmNerveAlarmTask* pTask = new CwmNerveAlarmTask(pApp, &pApp->getTaskBoard());
            pTask->m_alarm = alarm;
            ///设置task的id
            pTask->setTaskId(pApp->nextTaskId());
            ///设置task的超时时间点
            pTask->setTimeoutValue(CwxDate::getTimestamp() + pApp->getConfig().getTimeout() * 1000000);
            CWX_DEBUG(("Report the next alarm, task's task-id=%u", pTask->getTaskId()));
            pTask->execute(pThrEnv);
        }
    }
}

bool CwmNerveAlarmHandler::sendAlarm(CwmNerveApp* pApp, CwxAppTss* pThrEnv, CwmCmnAlarm const& alarm)
{
    CwmNerveTss* pTss = (CwmNerveTss*)pThrEnv;
    CwxPackageWriter* pWriter = pTss->getSndPackage();
    CWX_DEBUG(("Begin report alarm, service=%s, action=%s, wid=%d, pid=%d, datetime=%s, level=%u",
        alarm.getService().c_str(),
        alarm.getAction().c_str(),
        alarm.getWid(),
        alarm.getPid(),
        alarm.getDatetime().c_str(),
        alarm.getLevel()));
    ///将告警的信息，形成一个key/value的package，作为msg的data部分
    pWriter->beginPack();
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

    ///形成发送的msg block
    CwxMsgHead header(0, 0, MSG_TYPE_ALARM_REPORT, pApp->nextTaskId(), pWriter->getMsgSize());
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::pack(header, pWriter->getMsg(), pWriter->getMsgSize());
    ///设置发送的控制信息
    //设置发送的连接
    pBlock->send_ctrl().setConnId(pApp->getAlarmHandler()->getAlarmConnId());
    //设置发送的连接的svr-id
    pBlock->send_ctrl().setSvrId(CwmNerveApp::SVR_TYPE_ALARM);
    //设置host id
    pBlock->send_ctrl().setHostId(0);
    //设置发送过程的回调
    pBlock->send_ctrl().setMsgAttr(CwxMsgSendCtrl::FAIL_FINISH_NOTICE);
    //设置发送包的Task ID，当发送失败或完成时，可以获取。
    pBlock->event().setTaskId(header.getTaskId());
    if (0 != pApp->sendMsgByConn(pBlock))
    {
        CWX_ERROR(("Failure to report alarm msg, task-id=%u", header.getTaskId()));
        CwxMsgBlockAlloc::free(pBlock);
        return false;
    }
    CWX_DEBUG(("Alarm is reported,wid=%u", alarm.getWid()));
    return true;
}
