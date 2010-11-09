#include "CwmNerveCgiTask.h"
#include "CwmNerveApp.h"

void CwmNerveCgiTask::noticeTimeout(CwxAppTss* )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_strErrMsg = "Cgi is Timeout";
    m_bSuccess = false;
}

void CwmNerveCgiTask::noticeRecvMsg(CwxMsgBlock*& msg, CwxAppTss* , bool& bConnAppendMsg)
{
    bConnAppendMsg = true;
    m_strResultXml += string(msg->rd_ptr(), msg->length());
}

void CwmNerveCgiTask::noticeFailSendMsg(CwxMsgBlock*& , CwxAppTss* )
{
    CWX_ASSERT(0);
}

void CwmNerveCgiTask::noticeEndSendMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
}

void CwmNerveCgiTask::noticeConnClosed(CWX_UINT32 , CWX_UINT32 , CWX_UINT32 , CwxAppTss* )
{
    this->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    m_bSuccess = true;
}

int CwmNerveCgiTask::noticeActive(CwxAppTss* )
{
    return 0;
}

void CwmNerveCgiTask::execute(CwxAppTss* pThrEnv)
{
    CWX_UINT32 uiBufLen = 0;
    CwmNerveTss* pTss = (CwmNerveTss*) pThrEnv;
    if (CwxAppTaskBoardTask::TASK_STATE_INIT == getTaskState())
    {
        if (m_pService) m_pServiceInfo = m_pService->getServiceInfo();
        if (m_pAction) m_pActionInfo = m_pAction->getActionInfo();
        m_bSuccess = false;
        m_strErrMsg = "No execute";
        m_strResultXml="";
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
        {//unpack
            uiBufLen = CwmNerveTss::MAX_PACKAGE_SIZE;
            if (!pTss->getXmlPackageConv()->xmlToPackage(m_strResultXml.c_str(), pTss->getBuf(), uiBufLen))
            {
                CwxCommon::snprintf(pTss->getBuf(), CwmNerveTss::MAX_PACKAGE_SIZE, "Failure to convert xml, service=%s, action=%s, cgi=%s, err=%s, xml=%s", 
                    m_pServiceInfo->getName().c_str(),
                    m_pActionInfo->getName().c_str(),
                    m_pActionInfo->getCgiCmd().c_str(),
                    pTss->getXmlPackageConv()->getErrMsg(),
                    m_strResultXml.c_str());
                CWX_ERROR((pTss->getBuf()));
                strErrMsg = pTss->getBuf();
                bRet = false;
            }
            else
            {
                CwxPackageReader reader;
                if (!reader.unpack(pTss->getBuf(), uiBufLen))
                {
                    CWX_ASSERT(0);
                }
                //get result
                CwxKeyValueItem const* pItem = reader.getKey(CWM_CMN_RESULT);
                if (!pItem)
                {
                    CwxCommon::snprintf(pTss->getBuf(), CwmNerveTss::MAX_PACKAGE_SIZE, "Failure to get key[result], service=%s, action=%s, cgi=%s, err=no key, xml=%s", 
                        m_pServiceInfo->getName().c_str(),
                        m_pActionInfo->getName().c_str(),
                        m_pActionInfo->getCgiCmd().c_str(),
                        m_strResultXml.c_str());
                    CWX_ERROR((pTss->getBuf()));
                    strErrMsg = pTss->getBuf();
                    bRet = false;
                }
                else
                {
                    CwxCommon::memMove(pTss->getBuf(), pItem->m_szData, pItem->m_uiDataLen);
                    pTss->setBufDataLen(pItem->m_uiDataLen);
                    bRet = CwmNerveServerHandler::analyseReply(pThrEnv, status, strLevel, kvRecord, strErrMsg);
                }
                if (!bRet)
                {
                    CwxCommon::snprintf(pTss->getBuf(), CwmNerveTss::MAX_PACKAGE_SIZE, "Failure to analyse reply, service=%s, action=%s, cgi=%s, err=%s, xml=%s", 
                        m_pServiceInfo->getName().c_str(),
                        m_pActionInfo->getName().c_str(),
                        m_pActionInfo->getCgiCmd().c_str(),
                        strErrMsg.c_str(),
                        m_strResultXml.c_str());
                    CWX_ERROR((pTss->getBuf()));
                }
            }
        }
        else
        {
            CwxCommon::snprintf(pTss->getBuf(), CwmNerveTss::MAX_PACKAGE_SIZE, "Failure to analyse reply, service=%s, action=%s, cgi=%s, err=%s, xml=%s", 
                m_pServiceInfo->getName().c_str(),
                m_pActionInfo->getName().c_str(),
                m_pActionInfo->getCgiCmd().c_str(),
                m_strErrMsg.c_str(),
                m_strResultXml.c_str());
            CWX_ERROR((pTss->getBuf()));
            strErrMsg = pTss->getBuf();
            bRet = false;
        }///end if (m_bSuccess)
        if (m_pMsg)
        {//reply
            if (!bRet)
            {
                CwmNerveQueryHandler::replyError(m_pApp, m_pMsg, pThrEnv, m_strErrMsg.c_str());
            }
            else
            {
                CWX_DEBUG(("Query result: service=%s, action=%s, cgi=%s, status=%d, level=%s, err=%s",
                    m_pServiceInfo->getName().c_str(),
                    m_pActionInfo->getName().c_str(),
                    m_pActionInfo->getCgiCmd().c_str(),
                    status,
                    strLevel.length()?strLevel.c_str():"",
                    strErrMsg.length()?strErrMsg.c_str():""));
                CwmNerveQueryHandler::replyQuery(m_pApp, m_pMsg, pThrEnv, status, strLevel, strErrMsg, kvRecord);
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
                        0, alarm);
                    CWX_ERROR(("New alarm, service=%s, action=%s, level=%s, error=%s",
                        alarm.getService().c_str(),
                        alarm.getAction().c_str(),
                        CwmCmnAlarm::getAlarmString(alarm.getLevel()).c_str(),
                        alarm.getError().length()?alarm.getError().c_str():""));
                    m_pAction->getAlarmStatus().setWid(alarm.getWid());
                    m_pApp->getDataMgr()->insert(alarm);
                    CwmNerveAlarmHandler::reportNextAlarm(m_pApp, pThrEnv);
                }

            }
            else
            {
                CWX_DEBUG(("Poll result: service=%s, action=%s, cgi=%s, status=%d, level=%s, err=%s",
                    m_pServiceInfo->getName().c_str(),
                    m_pActionInfo->getName().c_str(),
                    m_pActionInfo->getCgiCmd().c_str(),
                    status,
                    strLevel.length()?strLevel.c_str():"",
                    strErrMsg.length()?strErrMsg.c_str():""));
                bRet = m_pAction->getAlarmStatus().update(0==status?"0":"-1", strLevel, strErrMsg);
                m_pAction->getAlarmStatus().formActionRecord(m_pApp->getConfig().getHostName(),
                    0,
                    m_pApp->getDataMgr()->nextPid(), kvRecord, record);
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
