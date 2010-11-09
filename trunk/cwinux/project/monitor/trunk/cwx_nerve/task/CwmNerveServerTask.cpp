#include "CwmNerveServerTask.h"
#include "CwmNerveApp.h"

void CwmNerveServerTask::noticeTimeout(CwxAppTss* )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_strErrMsg = "Query server is Timeout";
    m_bSuccess = false;
}

void CwmNerveServerTask::noticeRecvMsg(CwxMsgBlock*& msg, CwxAppTss* , bool& )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_replyMsg = msg;
    msg = NULL;
    m_bSuccess = true;
}

void CwmNerveServerTask::noticeFailSendMsg(CwxMsgBlock*& , CwxAppTss* )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_strErrMsg = "Failure to send query request";
    m_bSuccess = false;
}

void CwmNerveServerTask::noticeEndSendMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
}

void CwmNerveServerTask::noticeConnClosed(CWX_UINT32 , CWX_UINT32 , CWX_UINT32 , CwxAppTss* )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_strErrMsg = "Server's connection is closed";
    m_bSuccess = false;
}

int CwmNerveServerTask::noticeActive(CwxAppTss* pThrEnv)
{
    if (!CwmNerveServerHandler::execQuery(pThrEnv, m_pApp, m_pService, m_pAction, getTaskId()))
    {
        m_strErrMsg = " Failure to query server";
        m_bSuccess = false;
        return -1;
    }
    return 0;
}

void CwmNerveServerTask::execute(CwxAppTss* pThrEnv)
{
    CwmNerveTss* pTss = (CwmNerveTss*) pThrEnv;
    if (CwxAppTaskBoardTask::TASK_STATE_INIT == getTaskState())
    {
        m_bSuccess = false;
        m_strErrMsg = "No execute";
        m_replyMsg = NULL;
        getTaskBoard()->noticeActiveTask(this, pThrEnv);
    }

    if (isFinish())
    {
        CwxKeyValueItem kvRecord;
        bool bRet = false;
        int status = -1;
        string strLevel=CWM_CMN_DEFAULT;
        string strErrMsg;
        CwmCmnAlarm alarm;
        CwmCmnRecord record;
        if (m_bSuccess)
        {
            memcpy(pTss->getBuf(), m_replyMsg->rd_ptr(), m_replyMsg->length());
            pTss->setBufDataLen(m_replyMsg->length());
            bRet = CwmNerveServerHandler::analyseReply(pThrEnv, status, strLevel, kvRecord, strErrMsg);
            if (!bRet)
            {
                CwxCommon::snprintf(pTss->getBuf(), CwmNerveTss::MAX_PACKAGE_SIZE, "Failure to analyse reply, service=%s, action=%s, err=%s", 
                    m_pService->getServiceInfo()->getName().c_str(),
                    m_pAction->getActionInfo()->getName().c_str(),
                    strErrMsg.c_str());
                CWX_ERROR((pTss->getBuf()));
            }
        }
        else
        {
            CwxCommon::snprintf(pTss->getBuf(), CwmNerveTss::MAX_PACKAGE_SIZE, "Failure to analyse reply, service=%s, action=%s, err=%s", 
                m_pService->getServiceInfo()->getName().c_str(),
                m_pAction->getActionInfo()->getName().c_str(),
                m_strErrMsg.c_str());
            CWX_ERROR((pTss->getBuf()));
            strErrMsg = pTss->getBuf();
            bRet = false;
        }///end if (m_bSuccess)
        if (m_queryMsg)
        {//reply
            if (!bRet)
            {
                CwmNerveQueryHandler::replyError(m_pApp, m_queryMsg, pThrEnv, m_strErrMsg.c_str());
            }
            else
            {
                CWX_DEBUG(("Query result: service=%s, action=%s, status=%d, level=%s, err=%s",
                    m_pService->getServiceInfo()->getName().c_str(),
                    m_pAction->getActionInfo()->getName().c_str(),
                    status,
                    strLevel.length()?strLevel.c_str():"",
                    strErrMsg.length()?strErrMsg.c_str():""));
                CwmNerveQueryHandler::replyQuery(m_pApp, m_queryMsg, pThrEnv, status, strLevel, strErrMsg, kvRecord);
            }
        }
        else
        {//poll
            m_pAction->getAlarmStatus().setPolling(false);
            if (!bRet)
            {
                //alarm
                if (m_pAction->getAlarmStatus().update(0==status?"0":"-1", strLevel, strErrMsg))
                {
                    m_pAction->getAlarmStatus().formActionAlarm(m_pApp->getConfig().getHostName(),
                        m_pApp->getDataMgr()->nextWid(),
                        0,
                        alarm);
                    m_pAction->getAlarmStatus().setWid(alarm.getWid());
                    CWX_ERROR(("New alarm, service=%s, action=%s, level=%s, error=%s",
                        alarm.getService().c_str(),
                        alarm.getAction().c_str(),
                        CwmCmnAlarm::getAlarmString(alarm.getLevel()).c_str(),
                        alarm.getError().length()?alarm.getError().c_str():""));
                    m_pApp->getDataMgr()->insert(alarm);
                    CwmNerveAlarmHandler::reportNextAlarm(m_pApp, pThrEnv);
                }

            }
            else
            {
                CWX_DEBUG(("Poll result: service=%s, action=%s, status=%d, level=%s, err=%s",
                    m_pService->getServiceInfo()->getName().c_str(),
                    m_pAction->getActionInfo()->getName().c_str(),
                    status,
                    strLevel.length()?strLevel.c_str():"",
                    strErrMsg.length()?strErrMsg.c_str():""));
                bRet = m_pAction->getAlarmStatus().update(0==status?"0":"-1", strLevel, strErrMsg);
                m_pAction->getAlarmStatus().formActionRecord(m_pApp->getConfig().getHostName(),
                    0,
                    m_pApp->getDataMgr()->nextPid(),
                    kvRecord,
                    record);
                if (bRet)
                {//alarm
                    m_pAction->getAlarmStatus().formActionAlarm(m_pApp->getConfig().getHostName(),
                        m_pApp->getDataMgr()->nextWid(),
                        record.getPid(),
                        alarm);
                    m_pAction->getAlarmStatus().setWid(alarm.getWid());
                    CWX_ERROR(("New alarm, service=%s, action=%s, level=%s, error=%s",
                        alarm.getService().c_str(),
                        alarm.getAction().c_str(),
                        CwmCmnAlarm::getAlarmString(alarm.getLevel()).c_str(),
                        alarm.getError().length()?alarm.getError().c_str():""));
                    record.setWid(alarm.getWid());
                    m_pApp->getDataMgr()->insert(alarm);
                    CwmNerveAlarmHandler::reportNextAlarm(m_pApp, pThrEnv);
                }
                m_pApp->getDataMgr()->insert(record);
            }
        }
        delete this;
    }

}
