#include "CwxEchoTbEventHandler.h"
#include "CwxEchoTbClientApp.h"

///echo请求的处理函数
int CwxEchoTbEventHandler::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    m_pApp->getTaskBoard().noticeRecvMsg(msg->event().getMsgHeader().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}


int CwxEchoTbEventHandler::onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
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
    return 1;
}

int CwxEchoTbEventHandler::onEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    m_pApp->getTaskBoard().noticeEndSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

int CwxEchoTbEventHandler::onFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwxAppTaskBoardTask* pTask = NULL;
    m_pApp->getTaskBoard().noticeFailSendMsg(msg->event().getTaskId(), msg, pThrEnv, pTask);
    if (pTask) pTask->execute(pThrEnv);
    return 1;
}

int CwxEchoTbEventHandler::onTimeoutCheck(CwxMsgBlock*& , CwxAppTss* pThrEnv)
{
    list<CwxAppTaskBoardTask*> tasks;
    m_pApp->getTaskBoard().noticeCheckTimeout(pThrEnv, tasks);
    if (!tasks.empty())
    {
        list<CwxAppTaskBoardTask*>::iterator iter=tasks.begin();
        (*iter)->execute(pThrEnv);
        iter++;
    }
    return 1;
}
