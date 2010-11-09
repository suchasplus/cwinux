#include "CwxAppCommander.h"

CWINUX_BEGIN_NAMESPACE

int CwxAppCommander::onConnCreated(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    return pEventOp->onConnCreated(msg, pThrEnv);
}

int CwxAppCommander::onConnClosed(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    return pEventOp->onConnClosed(msg, pThrEnv);
}

int CwxAppCommander::onRecvMsg(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    return pEventOp->onRecvMsg(msg, pThrEnv);
}

int CwxAppCommander::onEndSendMsg(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    return pEventOp->onEndSendMsg(msg, pThrEnv);
}

int CwxAppCommander::onFailSendMsg(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    return pEventOp->onFailSendMsg(msg, pThrEnv);
}

int CwxAppCommander::onTimeoutCheck(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    return pEventOp->onTimeoutCheck(msg, pThrEnv);
}

int CwxAppCommander::onEvent4Handle(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    return pEventOp->onEvent4Handle(msg, pThrEnv);
}

int CwxAppCommander::onUserEvent(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    return pEventOp->onUserEvent(msg, pThrEnv);
}

CWINUX_END_NAMESPACE

