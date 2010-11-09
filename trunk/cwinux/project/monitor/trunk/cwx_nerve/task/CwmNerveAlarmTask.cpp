
#include "CwmNerveAlarmTask.h"
#include "CwmNerveApp.h"

void CwmNerveAlarmTask::noticeTimeout(CwxAppTss* )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_bSuccess = false;
}

void CwmNerveAlarmTask::noticeRecvMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_bSuccess = true;
}

void CwmNerveAlarmTask::noticeFailSendMsg(CwxMsgBlock*& , CwxAppTss* )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_bSuccess = false;
}

void CwmNerveAlarmTask::noticeEndSendMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
}

void CwmNerveAlarmTask::noticeConnClosed(CWX_UINT32 , CWX_UINT32 , CWX_UINT32 , CwxAppTss* )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_bSuccess = false;
}

int CwmNerveAlarmTask::noticeActive(CwxAppTss* pThrEnv)
{
    if (!CwmNerveAlarmHandler::sendAlarm(m_pApp, pThrEnv, m_alarm))
    {
        CWX_ERROR(("Failure to report alarm"));
        m_bSuccess = false;
        return -1;
    }
    return 0;
}
 
void CwmNerveAlarmTask::execute(CwxAppTss* pThrEnv)
{
    if (CwxAppTaskBoardTask::TASK_STATE_INIT == getTaskState())
    {
        CWX_DEBUG(("Start report alarm, wid=%u", m_alarm.getWid()));
        m_bSuccess = false;
        getTaskBoard()->noticeActiveTask(this, pThrEnv);
    }
    if (isFinish())
    {
        CWX_DEBUG(("finish report alarm, wid=%u", m_alarm.getWid()));
        m_pApp->getAlarmHandler()->setReportReply(true);
        if (m_bSuccess)
        {
            ///通知一个报警已经发送
            m_pApp->getDataMgr()->setAlarmReported(m_alarm.getHost(), m_alarm.getWid());
           ///发送下一个
            CwmNerveAlarmHandler::reportNextAlarm(m_pApp, pThrEnv);
        }
        delete this;
    }
    else
    {
        m_pApp->getAlarmHandler()->setReportReply(false);
    }
}
