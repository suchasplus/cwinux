#include "CwmCenterAlarmTask.h"
#include "CwmCenterApp.h"

void CwmCenterAlarmTask::noticeTimeout(CwxAppTss* )
{
    CWX_DEBUG(("Alarm report task istimeout, task-id=%u", getTaskId()));
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_bSuccess = false;
}

void CwmCenterAlarmTask::noticeRecvMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
    CWX_DEBUG(("Alarm report task's msg is replied, task-id=%u", getTaskId()));
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_bSuccess = true;
}

void CwmCenterAlarmTask::noticeFailSendMsg(CwxMsgBlock*& , CwxAppTss* )
{
    CWX_DEBUG(("Alarm report task's msg is failure to send, task-id=%u", getTaskId()));
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_bSuccess = false;
}

void CwmCenterAlarmTask::noticeEndSendMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
    CWX_DEBUG(("Alarm report task's msg is finish to send, task-id=%u", getTaskId()));
}

void CwmCenterAlarmTask::noticeConnClosed(CWX_UINT32 , CWX_UINT32 , CWX_UINT32 , CwxAppTss* )
{
    CWX_DEBUG(("Alarm report task's connection is closed, task-id=%u", getTaskId()));
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_bSuccess = false;
}

int CwmCenterAlarmTask::noticeActive(CwxAppTss* pThrEnv)
{
    string strMails;
    //get alarm's email
    m_pApp->getConfig().getAlarmMail(m_alarm.getSystem(), m_alarm.getService())->getAlarmEmails(m_alarm.getPLevel(), m_alarm.getLevel(), strMails);
    if (!CwmCenterAlarmReportor::sendAlarm(m_pApp, pThrEnv, m_uiAlarmConnId, getTaskId(), strMails, m_alarm))
    {
        CWX_ERROR(("Failure to report alarm"));
        m_bSuccess = false;
        return -1;
    }
    return 0;
}
 
void CwmCenterAlarmTask::execute(CwxAppTss* pThrEnv)
{
    if (CwxAppTaskBoardTask::TASK_STATE_INIT == getTaskState())
    {
        CWX_DEBUG(("Execute a alarm-report task. task-id=%u", getTaskId()));
        m_bSuccess = false;
        //get emails
        getTaskBoard()->noticeActiveTask(this, pThrEnv);
    }
    if (isFinish())
    {
        CWX_DEBUG(("A alarm-report task is finished. task-id=%u", getTaskId()));
        if (m_bSuccess)
        {
            ///通知一个报警已经发送
            m_pApp->getDataMgr()->setAlarmReported(m_alarm.getHost(), m_alarm.getWid());
        }
        m_pApp->getAlarmReportor()->_setReporting(false);
        ///发送下一个
        CwmCenterAlarmReportor::reportNextAlarm(m_pApp, pThrEnv);
        ///删除自己
        delete this;
    }
}
