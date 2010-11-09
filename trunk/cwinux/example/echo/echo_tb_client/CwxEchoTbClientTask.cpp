#include "CwxEchoTbClientTask.h"
#include "CwxEchoTbClientApp.h"
///构造函数
CwxEchoTbClientTask::CwxEchoTbClientTask(CwxEchoTbClientApp* pApp, CwxAppTaskBoard* pTaskBoard)
:CwxAppTaskBoardTask(pTaskBoard),m_pApp(pApp)
{
    m_unRecvMsgNum = 0;
    m_unFailMsgNum = 0;
    m_unSendMsgNum = 0;
    m_arrSendConn = new CWX_UINT32[pApp->getEchoGroupNum()];
}

void CwxEchoTbClientTask::noticeTimeout(CwxAppTss* )
{
    setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    CWX_DEBUG(("Task is timeout , task_id=%u", getTaskId()));
}

void CwxEchoTbClientTask::noticeRecvMsg(CwxMsgBlock*& , CwxAppTss* , bool& )
{
    m_unRecvMsgNum++;
    if (m_unFailMsgNum + m_unRecvMsgNum == m_unSendMsgNum)
    {
        setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    }
}

void CwxEchoTbClientTask::noticeFailSendMsg(CwxMsgBlock*& , CwxAppTss* )
{
    m_unFailMsgNum++;
    if (m_unFailMsgNum + m_unRecvMsgNum == m_unSendMsgNum)
    {
        setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    }
}

void CwxEchoTbClientTask::noticeEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* , bool& )
{
    m_arrSendConn[m_unSentMsgNum++] = msg->event().getConnId();
}

void CwxEchoTbClientTask::noticeConnClosed(CWX_UINT32 , CWX_UINT32 , CWX_UINT32 uiConnId, CwxAppTss* )
{
    for (CWX_UINT16 i=0; i<m_unSentMsgNum; i++)
    {
        if (m_arrSendConn[i] == uiConnId)
        {
            m_unFailMsgNum++;
            break;
        }
    }
    if (m_unFailMsgNum + m_unRecvMsgNum == m_unSendMsgNum)
    {
        setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
    }
}

int CwxEchoTbClientTask::noticeActive(CwxAppTss* )
{
    CwxMsgBlock* msg;
    setTaskState(TASK_STATE_WAITING);
    //往所有的svr类型发送消息
    for (CWX_UINT16 i=0; i<m_pApp->getEchoGroupNum(); i++)
    {
        msg = m_pApp->packEchoMsg(m_pApp->getEchoGroupSvrId(i), getTaskId());
        if (0 != m_pApp->sendMsgBySvr(msg))
        {
            CWX_ERROR(("Failure to send echo msg, svr_id=%u", m_pApp->getEchoGroupSvrId(i)));
            CwxMsgBlockAlloc::free(msg);
        }
        else
        {
            m_unSendMsgNum ++;
        }
    }
    if (0 == m_unSendMsgNum)
    {
        CWX_ERROR(("No msg is sent"));
        return -1;
    }
    return 0;
}

void CwxEchoTbClientTask::execute(CwxAppTss* pThrEnv)
{
    do{
        if (CwxAppTaskBoardTask::TASK_STATE_INIT != getTaskState())
        {
            m_pApp->incRecvMsg();
            setTaskState(CwxAppTaskBoardTask::TASK_STATE_INIT);
            if (m_pApp->getEchoGroupNum() != m_unRecvMsgNum)
            {
                CWX_INFO(("Group:%u, recv-msg:%u, fail-msg:%u, send-msg:%u, sent-msg:%u\n", m_pApp->getEchoGroupNum(), m_unRecvMsgNum, m_unFailMsgNum, m_unSendMsgNum, m_unSentMsgNum));
            }
        }
        setTaskId(m_pApp->getNextTaskId());
        m_unRecvMsgNum = 0;
        m_unFailMsgNum = 0;
        m_unSendMsgNum = 0;
        m_unSentMsgNum = 0;
        setTimeoutValue(CwxDate::getTimestamp() + m_pApp->getConfig().m_uiTimeout * 1000);
        for (CWX_UINT16 i=0; i<m_pApp->getEchoGroupNum(); i++)
        {
            m_arrSendConn[i] = 0;
        }
        getTaskBoard()->noticeActiveTask(this, pThrEnv);
        if (CwxAppTaskBoardTask::TASK_STATE_FINISH == getTaskState())
        {
            CWX_INFO(("Task is finished during invoking noticeActiveTask."));
            continue;
        }
        break;
    }while(1);
}


