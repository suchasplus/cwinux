
CWINUX_BEGIN_NAMESPACE

inline bool CwxAppFramework::isAppRunValid() const
{
    return m_bAppRunValid;
}

inline void CwxAppFramework::setAppRunValid(bool bValid)
{
    m_bAppRunValid = bValid;
}

inline string const& CwxAppFramework::getAppRunFailReason() const
{
    return m_strAppRunFailReason;
}

inline void CwxAppFramework::setAppRunFailReason(string const& strReason)
{
    m_strAppRunFailReason = strReason;
}

inline void CwxAppFramework::setAppVersion(string const& strVersion)
{
    m_strVersion = strVersion;
}

inline string const& CwxAppFramework::getAppVersion() const
{
    return m_strVersion;
}

inline void CwxAppFramework::setLastModifyDatetime(string const& strDatetime)
{
    m_strLastModifyDatetime = strDatetime;
}

inline string const& CwxAppFramework::getLastModifyDatetime() const
{
    return m_strLastModifyDatetime;
}

inline void CwxAppFramework::setLastCompileDatetime(string const& strDatetime)
{
    m_strLastCompileDatetime = strDatetime;
}

inline string const& CwxAppFramework::getLastCompileDatetime() const
{
    return m_strLastCompileDatetime;
}

inline bool CwxAppFramework::isCmdStop() const
{
    return m_bCmdStop;
}

inline bool CwxAppFramework::isCmdRestart() const
{
    return m_bCmdRestart;
}

inline bool CwxAppFramework::isStopped() const
{
    return m_bStopped;
}


inline char const*  CwxAppFramework::getWordDir() const 
{
    return m_strWorkDir.c_str();
}

inline void CwxAppFramework::setWorkDir(char const* szWorkDir)
{
    m_strWorkDir = szWorkDir;
}

inline char const* CwxAppFramework::getConfFile() const
{
    return m_strConfFile.c_str();
}

inline void CwxAppFramework::setConfFile(char const* szConfFile)
{
    m_strConfFile = szConfFile;
}

inline CWX_UINT32 CwxAppFramework::getClick() const
{
    return m_uiTimeClick;
}

inline void CwxAppFramework::setClick(CWX_UINT32 uiInternal)
{
    m_uiTimeClick = uiInternal;
}

inline CWX_UINT32 CwxAppFramework::getLogFileSize() const
{
    return m_uiLogSize;
}

inline void CwxAppFramework::setLogFileSize(CWX_UINT32 uiSize)
{
    m_uiLogSize =  uiSize;
}

inline CWX_UINT32 CwxAppFramework::getLogLevel() const
{
    return CwxAppLogger::instance()->getLevel();
}

inline void CwxAppFramework::setLogLevel(CWX_UINT32 uiLevel)
{
    CwxAppLogger::instance()->setLevel(uiLevel);
}

inline CWX_UINT16 CwxAppFramework::getLogFileNum() const
{
    return CwxAppLogger::instance()->getLogFileNum();
}

inline void CwxAppFramework::setLogFileNum(CWX_UINT16 unLogFileNum)
{
    m_unLogFileNum = unLogFileNum;
}

inline void CwxAppFramework::toLogFileNo(CWX_UINT16 unLogFileNo)
{
    CwxAppLogger::instance()->nextLog(false, unLogFileNo);
}

inline CWX_UINT16 CwxAppFramework::getKeepAliveSecond() const
{
    return  m_unDefKeepAliveSecond;
}

inline void CwxAppFramework::setKeepAliveSecond(CWX_UINT16 unSecond)
{
    m_unDefKeepAliveSecond = unSecond;
    if (m_unDefKeepAliveSecond>MAX_KEEPALIVE_SECOND)
        m_unDefKeepAliveSecond=MAX_KEEPALIVE_SECOND;
    if (m_unDefKeepAliveSecond<MIN_KEEPALIVE_SECOND)
        m_unDefKeepAliveSecond=MIN_KEEPALIVE_SECOND;
}

inline CWX_UINT16 CwxAppFramework::getLogCheckSecond() const
{
    return m_unLogCheckSecond;
}

inline void CwxAppFramework::setLogCheckSecond(CWX_UINT16 unSecond)
{
    m_unLogCheckSecond = unSecond;
    if (m_unLogCheckSecond > MAX_LOG_CHECK_SECOND)
        m_unLogCheckSecond = MAX_LOG_CHECK_SECOND;
    if (m_unLogCheckSecond < MIN_LOG_CHECK_SECOND)
        m_unLogCheckSecond = MIN_LOG_CHECK_SECOND;
}

inline bool CwxAppFramework::isDebug() const
{
    return m_bDebug;
}

inline void CwxAppFramework::setDebug(bool bDebug)
{
    m_bDebug = bDebug;
}

inline bool CwxAppFramework::isEnableHook()
{
    return m_bEnableHook;
}

inline void CwxAppFramework::enableHook(bool bEnable)
{
    m_bEnableHook = bEnable; 
}

inline CWX_UINT16 CwxAppFramework::getProcId() const
{
    return m_uiProcId;
}

inline void CwxAppFramework::setProcId(CWX_UINT16 uiProcId)
{
    m_uiProcId = uiProcId;
}

inline CwxAppCommander& CwxAppFramework::getCommander()
{
    return m_commander;
}

inline CwxAppTaskBoard& CwxAppFramework::getTaskBoard()
{
    return  m_taskBoard;
}

inline CwxAppThreadPoolMgr* CwxAppFramework::getThreadPoolMgr()
{
    return m_pThreadPoolMgr;
}

inline CwxAppTss* CwxAppFramework::getAppTss()
{
    return m_pTss;
}

inline CwxAppHandlerCache* CwxAppFramework::getHandlerCache()
{
    return m_pHandleCache;
}

///获取TCP connector
inline CwxAppTcpConnector* CwxAppFramework::getTcpConnector()
{
    return m_pTcpConnector;
}
///获取UNIX connector
inline CwxAppUnixConnector* CwxAppFramework::getUnixConnector()
{
    return m_pUnixConnector;
}

inline CwxAppReactor* CwxAppFramework::reactor()
{
    return m_pReactor;
}

inline int CwxAppFramework::openConn(CwxAppHandler4Msg& conn, bool& bStopListen)
{
    bool bSuspendConn = false;
    bStopListen = false;
    if (conn.getConnInfo().getPai())
    {
        delete conn.getConnInfo().getPai();
        conn.getConnInfo().setPai(NULL);
    }
    int iRet = 0;
    if (conn.getConnInfo().getSvrId() == SVR_TYPE_SYS)
    {
        CWX_ASSERT(m_pMgrServer);
        bSuspendConn = false;
        bStopListen = false;
        iRet = 0;
    }
    else
    {
        if (!isStopped() && conn.getConnInfo().isInvokeCreate()) iRet = this->onConnCreated(conn, bSuspendConn, bStopListen);
    }
    if (0 > iRet) return -1;

    if (this->reactor()->registerHandler(conn.getHandle(),
        &conn,
        CwxAppHandler4Base::PREAD_MASK,
        conn.getConnInfo().getConnId()) == -1)
    {
        CWX_ERROR(("Failure to register_handler."));
        return -1;
    }
    if (bSuspendConn)
    {
        this->reactor()->suspendHandler(&conn, CwxAppHandler4Base::READ_MASK);
    }
    if (conn.getConnInfo().isKeepalive() ||
       (conn.getConnInfo().isRawData() &&
       conn.getConnInfo().getPai() &&
       conn.getConnInfo().getPai()->isKeepAlive()))
    {
        conn.getConnInfo().setKeepAliveReply(true);
        conn.getConnInfo().setLastRecvMsgTime(time(NULL));
        (*m_pKeepAliveMap)[conn.getConnInfo().getConnId()] = &conn;
    }
    if (conn.getConnInfo().getHostId())
    {
        HostMapKey hostKey(conn.getConnInfo().getSvrId(), conn.getConnInfo().getHostId());
        CwxAppHostInfo* pHostInfo = m_pHostMap->find(hostKey)->second;
        pHostInfo->addConn(&conn.getConnInfo());
        conn.getConnInfo().setParent(pHostInfo);
    }else{
        conn.getConnInfo().setParent(NULL);
    }

    return 0;
}

//return -1：消息无效，关闭连接。 0：不连续接受消息； >0：连续从此连接上接受消息。
inline int CwxAppFramework::recvMessage(CwxMsgHead& header,
                                        CwxMsgBlock* msg,
                                        CwxAppHandler4Msg& conn,
                                        bool& bSuspendConn)
{
    if (isStopped()) return 0;
    getAppTss()->getThreadInfo()->incRecvMsgNum();
    bSuspendConn = false;
    conn.getConnInfo().setLastRecvMsgTime(time(NULL));
    if (!conn.getConnInfo().isRawData() && header.isSysMsg())
    {
        return recvSysMessage(msg, conn, header);
    }
    else if (SVR_TYPE_SYS == conn.getConnInfo().getSvrId())
    {
        m_pMgrServer->onRecvMsg(msg, conn, header, m_pTss);
        if (msg) CwxMsgBlockAlloc::free(msg);
        return 0;
    }
    else if (conn.getConnInfo().isRawData() && conn.getConnInfo().getPai())
    {
        return conn.getConnInfo().getPai()->noticeRecvMsg(msg, conn, header, bSuspendConn);
    }
    return this->onRecvMsg(msg, conn, header, bSuspendConn);
}

inline int CwxAppFramework::recvSysMessage(CwxMsgBlock* msg, CwxAppHandler4Msg& conn, CwxMsgHead const& header)
{
    int iRet = 0;
    CwxMsgHead local_header;
    do
    {
        CwxMsgBlock* reply = local_header.packSysMsgReply(header, msg);
        {
            CWX_DEBUG(("Recieve sys msg for svr_id:%u, host_id:%u, conn_id:%u",
                conn.getConnInfo().getSvrId(),
                conn.getConnInfo().getHostId(),
                conn.getConnInfo().getConnId()));
        }
        if (reply)
        {
            reply->send_ctrl().setSvrId(conn.getConnInfo().getSvrId());
            reply->send_ctrl().setHostId(conn.getConnInfo().getHostId());
            reply->send_ctrl().setConnId(conn.getConnInfo().getConnId());
            reply->send_ctrl().setUserData(NULL);
            reply->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
            sendMsgByConn(reply);
        }
        if (header.isKeepAlive(true))
        {//keepalive-reply
            conn.getConnInfo().setKeepAliveReply(true);
            CWX_DEBUG(("sys-msg is keep-alive-reply for svr_id:%u, host_id:%u, conn_id:%u",
                conn.getConnInfo().getSvrId(),
                conn.getConnInfo().getHostId(),
                conn.getConnInfo().getConnId()));
        }
        else if (header.isKeepAlive(false))
        {//keepalive
            CWX_DEBUG(("sys-msg is keep-alive for svr_id:%u, host_id:%u, conn_id:%u",
                conn.getConnInfo().getSvrId(),
                conn.getConnInfo().getHostId(),
                conn.getConnInfo().getConnId()));
        }
    }while(0);
    CwxMsgBlockAlloc::free(msg);
    return iRet<0?-1:0;
}

inline int CwxAppFramework::connClosed(CwxAppHandler4Msg& conn)
{
    if (isStopped()) return 0;
    if (conn.getConnInfo().isKeepalive())
    {
        m_pKeepAliveMap->erase(conn.getConnInfo().getConnId());
    }
    if (SVR_TYPE_SYS == conn.getConnInfo().getSvrId())
    {
        CWX_ASSERT(m_pMgrServer);
        return 0;
    }
    int iRet = this->onConnClosed(conn);
    if (conn.getConnInfo().getPai())
    {
        delete conn.getConnInfo().getPai();
        conn.getConnInfo().setPai(NULL);
    }
    return iRet;
}

///根据SVR-ID,HOST-ID获取主机信息，NULL表示不存在。
inline CwxAppHostInfo* CwxAppFramework::getHostById(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId)
{
    HostMapKey key(uiSvrId, uiHostId);
    IdHostHash::iterator iter = m_pHostMap->find(key);
    return m_pHostMap->end()==iter?NULL:iter->second;
}
///根据SVR-ID获取SVR信息，NULL表示不存在
inline CwxAppSvrInfo* CwxAppFramework::getSvrById(CWX_UINT32 uiSvrId)
{
    IdSvrHash::iterator iter = m_pSvrMap->find(uiSvrId);
    return m_pSvrMap->end()==iter?NULL:iter->second;
}
///往系统中注册服务类型为uiSvrId, 主机ID为uiHostId的主机
inline void CwxAppFramework::addSvrHostInfo(CWX_UINT32 uiSvrId, CWX_UINT32 uiHostId)
{
    CwxAppSvrInfo* pSvrInfo = NULL;
    IdSvrHash::iterator iter_svr = m_pSvrMap->find(uiSvrId);
    if ( iter_svr == m_pSvrMap->end())
    {
        pSvrInfo = new CwxAppSvrInfo(uiSvrId);
        (*m_pSvrMap)[uiSvrId] = pSvrInfo;
    }
    else
    {
        pSvrInfo = iter_svr->second;
    }
    if (uiHostId)
    {
        CwxAppHostInfo* pHostInfo = NULL;
        HostMapKey hostKey(uiSvrId, uiHostId);
        IdHostHash::iterator iter_host = m_pHostMap->find(hostKey);
        if (iter_host == m_pHostMap->end())
        {
            pHostInfo = new CwxAppHostInfo(uiSvrId, uiHostId, pSvrInfo);
            (*m_pHostMap)[hostKey] = pHostInfo;
            pSvrInfo->addHost(pHostInfo);
        }
    }
}

///往msg中设置的主机发送消息，返回false表示发送失败
inline bool CwxAppFramework::innerSendMsgByHost(CwxMsgBlock*& msg)
{
    CwxAppHostInfo* pHost = getHostById(msg->send_ctrl().getSvrId(), msg->send_ctrl().getHostId());
    if (!pHost || !pHost->isEnableSendMsg())
    {
        if (msg->send_ctrl().isFailNotice())
        {
            failSendMsg(msg);
        }
        return false;
    }
    if (!innerNoticeSendMsgByHost(msg, pHost))
    {
        if (msg->send_ctrl().isFailNotice())
        {
            failSendMsg(msg);
        }
        return false;
    }
    return true;

}
///往msg中设置的服务下的某个主机发送消息，返回false表示发送失败
inline bool CwxAppFramework::innerSendMsgBySvr(CwxMsgBlock*& msg)
{
    CwxAppSvrInfo* pSvr = getSvrById(msg->send_ctrl().getSvrId());
    CwxAppHostInfo* pHost = NULL;
    if (msg->send_ctrl().getDefHostId())
    {
        pHost = getHostById(msg->send_ctrl().getSvrId(), msg->send_ctrl().getDefHostId());
        if (pHost && !pHost->isEnableSendMsg()) pHost = NULL;
    }
    if (!pHost) pHost = pSvr?pSvr->selectHost():NULL;
    if (!pSvr || !pHost || !pHost->isEnableSendMsg())
    {
        if (msg->send_ctrl().isFailNotice())
        {
            failSendMsg(msg);
        }
        return false;
    }
    innerNoticeSendMsgByHost(msg , pHost);
    return true;
}
///notice send msg
inline bool CwxAppFramework::innerNoticeSendMsgByHost(CwxMsgBlock* msg, CwxAppHostInfo* pHost)
{
    CwxAppConnInfo* pConn = pHost->selectConn();
    if(pConn) return pConn->getHandler()->putMsg(msg);
    return false;
}

///通知一条消息发送失败，与onFailSendMsg()对应。
inline void CwxAppFramework::failSendMsg(CwxMsgBlock*& msg)
{
    if (SVR_TYPE_SYS != msg->send_ctrl().getSvrId()){
        onFailSendMsg(msg);
    }else{
        CWX_ASSERT(m_pMgrServer);
    }
}

///消息发送失败的再发送处理。
inline void CwxAppFramework::dipatchFailureSendMsg(CwxMsgBlock* msg)
{
    if (!msg->send_ctrl().isFixConnId() && msg->send_ctrl().isFailRetry())
    {
        if (msg->send_ctrl().isFixHostId()){
            if (!innerSendMsgByHost(msg))
            {
                if(msg) CwxMsgBlockAlloc::free(msg);
            }
        }else if (msg->send_ctrl().isFixSvrId()){
            if (!innerSendMsgBySvr(msg))
            {
                if(msg) CwxMsgBlockAlloc::free(msg);
            }
        }
        return;
    }
    if (msg->send_ctrl().isFailNotice()){
        failSendMsg(msg);
    }
    if (msg) CwxMsgBlockAlloc::free(msg);
}





CWINUX_END_NAMESPACE

