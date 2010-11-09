#include "CwxAppMgrServer.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE
int CwxAppMgrServer::onRecvMsg(CwxMsgBlock*& msg, 
                               CwxAppHandler4Msg const& conn, 
                               CwxMsgHead const& header,
                               CwxAppTss* pThrEnv)
{
    bool bRet = true;
    CwxAppMgrReply reply;
    CWX_UINT16 unRecvMsgType = header.getMsgType();

    msg->event().setSvrId(conn.getConnInfo().getSvrId());
    msg->event().setHostId(conn.getConnInfo().getHostId());
    msg->event().setConnId(conn.getConnInfo().getConnId());
    msg->event().setIoHandle(CWX_INVALID_HANDLE);
    msg->event().setConnUserData(conn.getConnInfo().getUserData());
    msg->event().setMsgHeader(header);
    if(MSG_TYPE_HELP == unRecvMsgType)
    {
        bRet = onCmdHelp(msg, pThrEnv, reply);
    }
    else if (MSG_TYPE_STOP == unRecvMsgType)
    {
        bRet = onCmdStop(msg, pThrEnv, reply);
    }
    else if (MSG_TYPE_RESTART == unRecvMsgType)
    {
        bRet = onCmdRestart(msg, pThrEnv, reply);
    }
    else if (MSG_TYPE_SET_LOG_LEVEL == unRecvMsgType)
    {
        bRet = onCmdSetLogLevel(msg, pThrEnv, reply);
    }
    else if (MSG_TYPE_GET_LOG_LEVEL == unRecvMsgType)
    {
        bRet = onCmdGetLogLevel(msg, pThrEnv, reply);
    }
    else if (MSG_TYPE_NEXT_LOG_FILE == unRecvMsgType)
    {
        bRet = onCmdNextLogfile(msg, pThrEnv, reply);
    }
    else if (MSG_TYPE_RUN_CHECK == unRecvMsgType)
    {
        bRet = onCmdRunCheck(msg, pThrEnv, reply);
    }
    else if (MSG_TYPE_RUN_DETAIL == unRecvMsgType)
    {
        bRet = onCmdRunDetail(msg, pThrEnv, reply);
    }
    else
    {
        bRet = onCmdUser(msg, pThrEnv, reply);
    }
    if (bRet)
    {
        replyCmd(conn.getConnInfo().getSvrId(),
            conn.getConnInfo().getHostId(),
            conn.getConnInfo().getConnId(),
            header,
            reply);
    }
    else
    {
        getApp()->noticeCloseConn(conn.getConnInfo().getConnId());
    }
    if (msg) CwxMsgBlockAlloc::free(msg);
    msg = NULL;
    return 1;
}

bool CwxAppMgrServer::onCmdHelp(CwxMsgBlock*& , 
                                CwxAppTss* pThrEnv,
                                CwxAppMgrReply& reply)
{
    reply.reset();
    reply.m_pRecord = new CwxPackageWriter(32 * 1024);
    CwxPackageWriter cmdPackage(1024);
    reply.m_pRecord->beginPack();
    bool bRet = false;
    do
    {
        if (!packHelpCmd(reply,
            cmdPackage, MSG_TYPE_HELP,
            CWX_APP_MGR_CMD_NAME_HELP,
            "",
            "Get help command."))
            break;
        if (!packHelpCmd(reply,
            cmdPackage,
            MSG_TYPE_SET_LOG_LEVEL,
            CWX_APP_MGR_CMD_NAME_SET_LEVEL,
            "level:n:c",
            "Set log's level, info|debug|warning|error."))
            break;
        if (!packHelpCmd(reply,
            cmdPackage,
            MSG_TYPE_GET_LOG_LEVEL,
            CWX_APP_MGR_CMD_NAME_GET_LEVEL,
            "",
            "Get log's level."))
            break;
        CwxCommon::snprintf(pThrEnv->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "Set the log's file-no, rang[0,%u).", getApp()->getLogFileNum());
        if (!packHelpCmd(reply,
            cmdPackage,
            MSG_TYPE_NEXT_LOG_FILE,
            CWX_APP_MGR_CMD_NAME_FILE_NO,
            "no:n:n",
            pThrEnv->m_szBuf2K))
            break;
        if (!packHelpCmd(reply,
            cmdPackage,
            MSG_TYPE_RUN_CHECK,
            CWX_APP_MGR_CMD_NAME_CHECK,
            "",
            "get running state."))
            break;
        if (!packHelpCmd(reply,
            cmdPackage,
            MSG_TYPE_RUN_DETAIL,
            CWX_APP_MGR_CMD_NAME_DETAIL,
            "",
            "get running information."))
            break;
        if (!onCmdHelpUser(pThrEnv, reply, cmdPackage)) break;
        bRet = true;
    }while(0);
    return bRet;
}

bool CwxAppMgrServer::onCmdHelpUser(CwxAppTss* , CwxAppMgrReply& , CwxPackageWriter& )
{
    return true;
}

bool CwxAppMgrServer::onCmdStop(CwxMsgBlock*& , CwxAppTss* , CwxAppMgrReply& )
{
//    getApp()->stop();
    return false;
}

bool CwxAppMgrServer::onCmdRestart(CwxMsgBlock*& , CwxAppTss* , CwxAppMgrReply& )
{
//    getApp()->stop();
    return false;
}

bool CwxAppMgrServer::onCmdSetLogLevel(CwxMsgBlock*& msg, CwxAppTss* pThrEnv, CwxAppMgrReply& reply)
{
    CwxPackageReader package;
    reply.reset();
    if (!msg)
    {
        reply.m_status = -1;
        reply.m_strError = "Set Log  Level, but package is empty";
        return true;
    }
    if (!package.unpack(msg->rd_ptr(), msg->length(), false))
    {
        reply.m_status = -1;
        CwxCommon::snprintf(pThrEnv->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "Failure to unpack Set-Log-Level package, error:%s", package.getErrMsg());
        reply.m_strError = pThrEnv->m_szBuf2K;
        return true;
    }
    string strLevel;
    list<string> levels;
    list<string>::iterator iter;
    CWX_UINT32 uiLevel = 0;
    if (!package.getKey(CWX_APP_MGR_LOG_LEVEL, strLevel, false))
    {
        CwxCommon::snprintf(pThrEnv->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "key[%u] doesn't exist in Set-Log-Level package", CWX_APP_MGR_LOG_LEVEL);
        reply.m_status = -1;
        reply.m_strError = pThrEnv->m_szBuf2K;
        return true;
    }
    CwxCommon::split(strLevel, levels, '|');
    iter = levels.begin();
    while(iter != levels.end())
    {
        if (strcasecmp(iter->c_str(), "debug")==0)
        {
            uiLevel |= CwxAppLogger::LEVEL_DEBUG;
        }
        else if (strcasecmp(iter->c_str(), "info")==0)
        {
            uiLevel |= CwxAppLogger::LEVEL_INFO;
        }
        else if (strcasecmp(iter->c_str(), "warning")==0)
        {
            uiLevel |= CwxAppLogger::LEVEL_WARNING;
        }
        else if (strcasecmp(iter->c_str(), "error")==0)
        {
            uiLevel |= CwxAppLogger::LEVEL_ERROR;
        }
        iter++;
    }

    getApp()->setLogLevel(uiLevel);
    reply.m_strError = "Success to set the log level";
    return true;
}

bool CwxAppMgrServer::onCmdGetLogLevel(CwxMsgBlock*& , CwxAppTss* , CwxAppMgrReply& reply)
{
    CWX_UINT32 uiLevel = getApp()->getLogLevel();
    string strLevel;
    if (CWX_CHECK_ATTR(uiLevel, CwxAppLogger::LEVEL_DEBUG))
    {
        strLevel += strLevel.length()?"|debug":"debug";
    }
    if (CWX_CHECK_ATTR(uiLevel, CwxAppLogger::LEVEL_INFO))
    {
        strLevel += strLevel.length()?"|info":"info";
    }
    if (CWX_CHECK_ATTR(uiLevel, CwxAppLogger::LEVEL_WARNING))
    {
        strLevel += strLevel.length()?"|warning":"warning";
    }
    if (CWX_CHECK_ATTR(uiLevel, CwxAppLogger::LEVEL_ERROR))
    {
        strLevel += strLevel.length()?"|error":"error";
    }

    reply.reset();
    reply.m_pRecord = new CwxPackageWriter(1024);
    reply.m_pRecord->beginPack();
    reply.m_pRecord->addKeyValue(CWX_APP_MGR_LOG_LEVEL, strLevel.c_str(), strLevel.length());
    reply.m_pRecord->pack();
    return true;
}

bool CwxAppMgrServer::onCmdNextLogfile(CwxMsgBlock*& msg, CwxAppTss* pThrEnv, CwxAppMgrReply& reply)
{
    reply.reset();
    CwxPackageReader package;
    if (!msg)
    {
        reply.m_status = -1;
        strcpy(pThrEnv->m_szBuf2K, "Change Next Log  file, but package is empty");
        reply.m_strError = pThrEnv->m_szBuf2K;
        return true;
    }
    if (!package.unpack(msg->rd_ptr(), msg->length(), false))
    {
        CwxCommon::snprintf(pThrEnv->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "Failure to unpack Change-Next-Log  package, error:%s", package.getErrMsg());
        reply.m_status = -1;
        reply.m_strError = pThrEnv->m_szBuf2K;
        return true;
    }
    CWX_UINT16 unFileNo = 0;
    if (!package.getKey(CWX_APP_MGR_LOG_FILE_NO, unFileNo))
    {
        CwxCommon::snprintf(pThrEnv->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "key[%u] doesn't exist in Change-Next-Log package", CWX_APP_MGR_LOG_FILE_NO);
        reply.m_status = -1;
        reply.m_strError = pThrEnv->m_szBuf2K;
        return true;
    }
    getApp()->toLogFileNo(unFileNo);
    reply.m_strError = "Success to set log file number";
    return true;
}

bool CwxAppMgrServer::onCmdRunCheck(CwxMsgBlock*& , CwxAppTss* , CwxAppMgrReply& reply)
{
    reply.reset();
    reply.m_pRecord = new CwxPackageWriter(4 * 1024);
    reply.m_pRecord->beginPack();
    if (!reply.m_pRecord->addKeyValue(CWX_APP_MGR_APP_VERSION, getApp()->getAppVersion().c_str(), getApp()->getAppVersion().length(), false))
    {
        reply.m_status = -1;
        reply.m_strError = reply.m_pRecord->getErrMsg();
        return true;
    }
    if (!reply.m_pRecord->addKeyValue(CWX_APP_MGR_APP_MODIFY_TIME, getApp()->getLastModifyDatetime().c_str(), getApp()->getLastModifyDatetime().length(), false))
    {
        reply.m_status = -1;
        reply.m_strError = reply.m_pRecord->getErrMsg();
        return true;
    }
    if (!reply.m_pRecord->addKeyValue(CWX_APP_MGR_COMPILE_TIME, getApp()->getLastCompileDatetime().c_str(), getApp()->getLastCompileDatetime().length(), false))
    {
        reply.m_status = -1;
        reply.m_strError = reply.m_pRecord->getErrMsg();
        return true;
    }
    if (!getApp()->isAppRunValid())
    {
        reply.m_status = -1;
        reply.m_strError = getApp()->getAppRunFailReason();
    }
    else
    {
        reply.m_status = 0;
        reply.m_strError = "valid";
    }
    return true;
}

bool CwxAppMgrServer::onCmdRunDetail(CwxMsgBlock*& , CwxAppTss* pThrEnv, CwxAppMgrReply& reply)
{
    reply.reset();
    reply.m_pRecord = new CwxPackageWriter(64 * 1024);
    CwxPackageWriter tPoolsPackage(60 * 1024);
    CwxPackageWriter tPoolPackage(60 * 1024);
    CwxPackageWriter threadPackage(4 * 1024);
    reply.m_pRecord->beginPack();
    if (!reply.m_pRecord->addKeyValue(CWX_APP_MGR_APP_VERSION, getApp()->getAppVersion().c_str(), getApp()->getAppVersion().length(), false))
    {
        reply.m_status = -1;
        reply.m_strError = reply.m_pRecord->getErrMsg();
        return true;
    }
    if (!reply.m_pRecord->addKeyValue(CWX_APP_MGR_APP_MODIFY_TIME, getApp()->getLastModifyDatetime().c_str(), getApp()->getLastModifyDatetime().length(), false))
    {
        reply.m_status = -1;
        reply.m_strError = reply.m_pRecord->getErrMsg();
        return true;
    }
    if (!reply.m_pRecord->addKeyValue(CWX_APP_MGR_COMPILE_TIME, getApp()->getLastCompileDatetime().c_str(), getApp()->getLastCompileDatetime().length(), false))
    {
        reply.m_status = -1;
        reply.m_strError = reply.m_pRecord->getErrMsg();
        return true;
    }
    if (!getApp()->isAppRunValid())
    {
        reply.m_status = -1;
        reply.m_strError = getApp()->getAppRunFailReason();
    }
    else
    {
        reply.m_status = 0;
        reply.m_strError = "valid";
    }
    //patch thread info
    vector<vector<CwxAppTss*> > arrTss;
    CwxAppTss* pEnv = NULL;

    tPoolsPackage.beginPack();
    getApp()->getThreadPoolMgr()->getTss(arrTss);
    for (CWX_UINT32 i=0; i<arrTss.size(); i++)
    {
        tPoolPackage.beginPack();
        for (CWX_UINT32 j=0; j<arrTss[i].size(); j++)
        {
            pEnv = arrTss[i][j];
            if (!pEnv->packThreadInfo(threadPackage, pThrEnv->m_szBuf2K))return false;
            if (!tPoolPackage.addKeyValue(CWX_APP_MGR_THREAD, threadPackage.getMsg(), threadPackage.getMsgSize(), true))
            {
                reply.m_status = -1;
                reply.m_strError = tPoolPackage.getErrMsg();
                return true;
            }
        }
        if (!tPoolPackage.pack())
        {
            reply.m_status = -1;
            reply.m_strError = tPoolPackage.getErrMsg();
            return true;
        }
        if (!tPoolsPackage.addKeyValue(CWX_APP_MGR_THREAD_POOL, tPoolPackage.getMsg(), tPoolPackage.getMsgSize(), true))
        {
            reply.m_status = -1;
            reply.m_strError = tPoolsPackage.getErrMsg();
            return true;
        }
    }
    if (!reply.m_pRecord->addKeyValue(CWX_APP_MGR_THREAD_POOLS, tPoolsPackage.getMsg(), tPoolsPackage.getMsgSize(), true))
    {
        reply.m_status = -1;
        reply.m_strError = reply.m_pRecord->getErrMsg();
        return true;
    }
    if (!reply.m_pRecord->pack())
    {
        reply.m_status = -1;
        reply.m_strError = reply.m_pRecord->getErrMsg();
        return true;
    }
    return true;
}

bool CwxAppMgrServer::onCmdUser(CwxMsgBlock*& msg, CwxAppTss* pThrEnv, CwxAppMgrReply& reply)
{
    reply.reset();
    reply.m_status = -1;
    CwxCommon::snprintf(pThrEnv->m_szBuf2K, CwxAppTss::TSS_2K_BUF, "Unknown Msg type:%u", msg->event().getMsgHeader().getMsgType());
    reply.m_strError = pThrEnv->m_szBuf2K;
    return true;
}

CWINUX_END_NAMESPACE
