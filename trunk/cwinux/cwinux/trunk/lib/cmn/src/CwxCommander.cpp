#include "CwxCommander.h"

CWINUX_BEGIN_NAMESPACE

int CwxCommander::onConnCreated(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onConnCreated(msg, pThrEnv);
}

int CwxCommander::onConnClosed(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onConnClosed(msg, pThrEnv);
}

int CwxCommander::onRecvMsg(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onRecvMsg(msg, pThrEnv);
}

int CwxCommander::onEndSendMsg(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onEndSendMsg(msg, pThrEnv);
}

int CwxCommander::onFailSendMsg(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onFailSendMsg(msg, pThrEnv);
}

int CwxCommander::onTimeoutCheck(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onTimeoutCheck(msg, pThrEnv);
}

int CwxCommander::onEvent4Handle(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onEvent4Handle(msg, pThrEnv);
}

int CwxCommander::onUserEvent(CwxAppCmdOp* pEventOp, CwxMsgBlock*& msg, CwxTss* pThrEnv)
{
    return pEventOp->onUserEvent(msg, pThrEnv);
}

CWINUX_END_NAMESPACE

