#include "CwmCenterAlarmAcceptor.h"
#include "CwmCenterApp.h"

int CwmCenterAlarmAcceptor::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxPackageReader reader(false);
    if (!reader.unpack(msg->rd_ptr(), msg->length(), true, false))
    {
        CWX_ERROR(("Failure to unpack the recieved alarm, err=%s", reader.getErrMsg()));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    //fetch alarm
    string strValue;
    CwmCmnAlarm alarm;
    //fetch host
    if (!reader.getKey(CWM_CMN_HOST, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_HOST));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setHost(strValue);
    //fetch wid
    if (!reader.getKey(CWM_CMN_WID, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_WID));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setWid(strtoul(strValue.c_str(), NULL, 0));
    //fetch pid 
    if (!reader.getKey(CWM_CMN_PID, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_PID));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setPid(strtoul(strValue.c_str(), NULL, 0));
    //fetch datetime 
    if (!reader.getKey(CWM_CMN_DATETIME, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_DATETIME));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setDatetime(strValue);
    //fetch level
    if (!reader.getKey(CWM_CMN_LEVEL, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_LEVEL));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setLevel(strtoul(strValue.c_str(), NULL, 0));

    //fetch system
    if (!reader.getKey(CWM_CMN_SYSTEM, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_SYSTEM));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setSystem(strValue);
    //fetch service
    if (!reader.getKey(CWM_CMN_SERVICE, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_SERVICE));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setService(strValue);
    //fetch action
    if (!reader.getKey(CWM_CMN_ACTION, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_ACTION));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setAction(strValue);
    //fetch type
    if (!reader.getKey(CWM_CMN_TYPE, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_TYPE));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setType(strValue);
    //fetch pwid
    if (!reader.getKey(CWM_CMN_PWID, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_PWID));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setPWid(strtoul(strValue.c_str(), NULL, 0));
    //fetch plevel
    if (!reader.getKey(CWM_CMN_PLEVEL, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_PLEVEL));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setPLevel(strtoul(strValue.c_str(), NULL, 0));
    //fetch error 
    if (!reader.getKey(CWM_CMN_ERROR, strValue))
    {
        CWX_ERROR(("Reported alarm no key:%s, close connection", CWM_CMN_ERROR));
        m_pApp->noticeCloseConn(msg->event().getConnId());
        return 1;
    }
    alarm.setError(strValue);
    CWX_DEBUG(("Recieve a alarm, host=%s, wid=%u, pid=%u, system=%s, service=%s, action=%s, type=%s, level=%s, pwid=%u, plevel=%s, err=%s",
        alarm.getHost().c_str(),
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
    //reply nerve
    replyAlarm(pThrEnv,
        msg->event().getConnId(),
        msg->event().getMsgHeader(),
        alarm.getWid());
    //添加告警
    m_pApp->getDataMgr()->insert(alarm);
    //报告告警
    CwmCenterAlarmReportor::reportNextAlarm(m_pApp, pThrEnv);
    return 1;
}

void CwmCenterAlarmAcceptor::replyAlarm(CwxAppTss* pThrEnv,
                                        CWX_UINT32 uiConnId,
                                        CwxMsgHead const& recvHeader,
                                        CWX_UINT32 uiWid)
{
    CwmCenterTss* pTss = (CwmCenterTss*)pThrEnv;
    pTss->m_pSndPackage->beginPack();
    pTss->m_pSndPackage->addKeyValue(CWM_CMN_WID, uiWid);
    pTss->m_pSndPackage->pack();
    CwxMsgHead local(recvHeader);
    local.setMsgType(recvHeader.getMsgType() + 1);
    local.setDataLen(pTss->m_pSndPackage->getMsgSize());
    CwxMsgBlock* block = CwxMsgBlockAlloc::pack(local, pTss->m_pSndPackage->getMsg(), pTss->m_pSndPackage->getMsgSize());
    block->send_ctrl().setSvrId(CwmCenterApp::SVR_TYPE_ALARM_ACCEPT);
    block->send_ctrl().setHostId(0);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != m_pApp->sendMsgByConn(block))
    {
        CWX_ERROR(("Failure to reply alarm report"));
        CwxMsgBlockAlloc::free(block);
    }
}
