#include "CwxAppMgrInterface.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE
bool CwxAppMgrInterface::packHelpCmd(CwxAppMgrReply& reply,
                                     CwxPackageWriter& cmdPackage,
                                     CWX_UINT16 unMsgType,
                                     char const* szCmdName,
                                     char const* args,
                                     char const* desc)
{
    bool bRet = false;
    cmdPackage.beginPack();
    do
    {
        if (!cmdPackage.addKeyValue(CWX_APP_MGR_CMD_MSG_TYPE, unMsgType)) break;
        if (!cmdPackage.addKeyValue(CWX_APP_MGR_CMD_MSG_NAME, szCmdName)) break;
        if (!cmdPackage.addKeyValue(CWX_APP_MGR_CMD_ARG, args?args:"", args?strlen(args):0, false)) break;
        if (!cmdPackage.addKeyValue(CWX_APP_MGR_CMD_DESC, desc?desc:"", desc?strlen(desc):0, false)) break;
        if (!cmdPackage.pack()) break;
        bRet = true;
    }while(0);
    if (!bRet)
    {
        reply.m_status = -1;
        reply.m_strError = cmdPackage.getErrMsg();
    }
    else
    {
        if (!reply.m_pRecord->addKeyValue(CWX_APP_MGR_CMD, cmdPackage.getMsg(), cmdPackage.getMsgSize(), true))
        {
            bRet = false;
            reply.m_status = -1;
            reply.m_strError = reply.m_pRecord->getErrMsg();
        }
    }
    return bRet;
}

void CwxAppMgrInterface::replyCmd(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CwxMsgHead const& header, CwxAppMgrReply& reply)
{
    CwxPackageWriter* writer = NULL;
    CWX_UINT32 uiExtraLen = reply.m_strError.length() + reply.m_strLevel.length() + 1024;
    if (reply.m_pRecord)
    {
        writer = new CwxPackageWriter(reply.m_pRecord->getMsgSize() + uiExtraLen);
    }
    else
    {
        writer = new CwxPackageWriter(uiExtraLen);
    }
    writer->beginPack();
    writer->addKeyValue(CWX_APP_MGR_STATUS, reply.m_status);
    writer->addKeyValue(CWX_APP_MGR_LEVEL, reply.m_strLevel);
    writer->addKeyValue(CWX_APP_MGR_ERROR, reply.m_strError);
    if (reply.m_pRecord)
    {
        writer->addKeyValue(CWX_APP_MGR_RECORD, reply.m_pRecord->getMsg(), reply.m_pRecord->getMsgSize(), true);
    }
    else
    {
        writer->addKeyValue(CWX_APP_MGR_RECORD, "", 0, true);
    }
    writer->pack();
    CwxMsgHead localHead(header);
    localHead.setDataLen(writer->getMsgSize());
    localHead.setMsgType(header.getMsgType() + 1);    
    CwxMsgBlock* block = CwxMsgBlockAlloc::pack(localHead, writer->getMsg(), writer->getMsgSize());
    block->send_ctrl().setSvrId(uiSvrId);
    block->send_ctrl().setHostId(uiHostId);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setUserData(NULL);
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != m_pApp->sendMsgByConn(block))
    {
        CwxMsgBlockAlloc::free(block);
    }
    delete writer;
}

bool CwxAppMgrInterface::sendCmdHelp(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId)
{
    CwxMsgHead header(0, 0, MSG_TYPE_HELP, uiTaskId, 0);
    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(CwxMsgHead::MSG_HEAD_LEN);
    memcpy(block->wr_ptr(), header.toNet(), CwxMsgHead::MSG_HEAD_LEN);
    block->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    block->send_ctrl().setSvrId(uiSvrId);
    block->send_ctrl().setHostId(uiHostId);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setUserData((void*)((size_t)uiTaskId));
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != m_pApp->sendMsgByConn(block))
    {
        CwxMsgBlockAlloc::free(block);
        return false;
    }
    return true;
}
bool CwxAppMgrInterface::sendCmdStop(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId)
{
    CwxMsgHead header(0, 0, MSG_TYPE_STOP, uiTaskId, 0);
    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(CwxMsgHead::MSG_HEAD_LEN);
    memcpy(block->wr_ptr(), header.toNet(), CwxMsgHead::MSG_HEAD_LEN);
    block->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    block->send_ctrl().setSvrId(uiSvrId);
    block->send_ctrl().setHostId(uiHostId);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setUserData((void*)((size_t)uiTaskId));
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != m_pApp->sendMsgByConn(block))
    {
        CwxMsgBlockAlloc::free(block);
        return false;
    }
    return true;
}

bool CwxAppMgrInterface::sendCmdRestart(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId)
{
    CwxMsgHead header(0, 0, MSG_TYPE_RESTART, uiTaskId, 0);
    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(CwxMsgHead::MSG_HEAD_LEN);
    memcpy(block->wr_ptr(), header.toNet(), CwxMsgHead::MSG_HEAD_LEN);
    block->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    block->send_ctrl().setSvrId(uiSvrId);
    block->send_ctrl().setHostId(uiHostId);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setUserData((void*)((size_t)uiTaskId));
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != m_pApp->sendMsgByConn(block))
    {
        CwxMsgBlockAlloc::free(block);
        return false;
    }
    return true;
}

bool CwxAppMgrInterface::sendCmdSetLogLevel(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId, CWX_UINT32 uiLevel)
{
    CwxMsgHead header(0, 0, MSG_TYPE_SET_LOG_LEVEL, uiTaskId, 0);
    CwxPackageWriter package(1024);
    package.beginPack();
    package.addKeyValue(CWX_APP_MGR_LOG_LEVEL, uiLevel);
    package.pack();

    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(CwxMsgHead::MSG_HEAD_LEN + package.getMsgSize());
    memcpy(block->wr_ptr(), header.toNet(), CwxMsgHead::MSG_HEAD_LEN);
    block->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    memcpy(block->wr_ptr(), package.getMsg(), package.getMsgSize());
    block->wr_ptr(package.getMsgSize());
    block->send_ctrl().setSvrId(uiSvrId);
    block->send_ctrl().setHostId(uiHostId);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setUserData((void*)((size_t)uiTaskId));
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);

    if (0 != m_pApp->sendMsgByConn(block))
    {
        CwxMsgBlockAlloc::free(block);
        return false;
    }
    return true;
}

bool CwxAppMgrInterface::sendCmdQueryLogLevel(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId)
{
    CwxMsgHead header(0, 0, MSG_TYPE_GET_LOG_LEVEL, uiTaskId, 0);
    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(CwxMsgHead::MSG_HEAD_LEN);
    memcpy(block->wr_ptr(), header.toNet(), CwxMsgHead::MSG_HEAD_LEN);
    block->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    block->send_ctrl().setSvrId(uiSvrId);
    block->send_ctrl().setHostId(uiHostId);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setUserData((void*)((size_t)uiTaskId));
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != m_pApp->sendMsgByConn(block))
    {
        CwxMsgBlockAlloc::free(block);
        return false;
    }
    return true;

}

bool CwxAppMgrInterface::sendCmdNextLogFile(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId, CWX_UINT16 unFileNo)
{
    CwxMsgHead header(0, 0, MSG_TYPE_NEXT_LOG_FILE, uiTaskId, 0);
    CwxPackageWriter package(1024);
    package.beginPack();
    package.addKeyValue(CWX_APP_MGR_LOG_FILE_NO, unFileNo);
    package.pack();

    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(CwxMsgHead::MSG_HEAD_LEN + package.getMsgSize());
    memcpy(block->wr_ptr(), header.toNet(), CwxMsgHead::MSG_HEAD_LEN);
    block->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    memcpy(block->wr_ptr(), package.getMsg(), package.getMsgSize());
    block->wr_ptr(package.getMsgSize());
    block->send_ctrl().setSvrId(uiSvrId);
    block->send_ctrl().setHostId(uiHostId);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setUserData((void*)((size_t)uiTaskId));
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);

    if (0 != m_pApp->sendMsgByConn(block))
    {
        CwxMsgBlockAlloc::free(block);
        return false;
    }
    return true;
}

bool CwxAppMgrInterface::sendCmdRunCheck(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId)
{
    CwxMsgHead header(0, 0, MSG_TYPE_RUN_CHECK, uiTaskId, 0);
    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(CwxMsgHead::MSG_HEAD_LEN);
    memcpy(block->wr_ptr(), header.toNet(), CwxMsgHead::MSG_HEAD_LEN);
    block->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    block->send_ctrl().setSvrId(uiSvrId);
    block->send_ctrl().setHostId(uiHostId);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setUserData((void*)((size_t)uiTaskId));
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != m_pApp->sendMsgByConn(block))
    {
        CwxMsgBlockAlloc::free(block);
        return false;
    }
    return true;

}

bool CwxAppMgrInterface::sendCmdRunDetail(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId, CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId)
{
    CwxMsgHead header(0, 0, MSG_TYPE_RUN_DETAIL, uiTaskId, 0);
    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(CwxMsgHead::MSG_HEAD_LEN);
    memcpy(block->wr_ptr(), header.toNet(), CwxMsgHead::MSG_HEAD_LEN);
    block->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    block->send_ctrl().setSvrId(uiSvrId);
    block->send_ctrl().setHostId(uiHostId);
    block->send_ctrl().setConnId(uiConnId);
    block->send_ctrl().setUserData((void*)((size_t)uiTaskId));
    block->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != m_pApp->sendMsgByConn(block))
    {
        CwxMsgBlockAlloc::free(block);
        return false;
    }
    return true;
}


CWINUX_END_NAMESPACE
