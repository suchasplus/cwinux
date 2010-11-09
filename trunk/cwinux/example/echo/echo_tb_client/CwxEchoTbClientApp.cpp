#include "CwxEchoTbClientApp.h"

///构造函数，初始化发送的echo数据内容
CwxEchoTbClientApp::CwxEchoTbClientApp()
{
    CWX_UINT32 i=0;
    for (i=0; i<sizeof(m_szBuf100K) - 1; i++)
    {
        m_szBuf100K[i] = 'a' + i % 26;
    }
    m_szBuf100K[i] = 0x00;
    m_uiTaskId = 0;
    m_uiRecvNum = 0;
    m_threadPool = NULL;
    m_pEventHandle = NULL;

}
///析构函数
CwxEchoTbClientApp::~CwxEchoTbClientApp()
{

}

///初始化APP，加载配置文件
int CwxEchoTbClientApp::init(int argc, char** argv)
{
    string strErrMsg;
    ///首先调用架构的init
    if (CwxAppFramework::init(argc, argv) == -1) return -1;
    ///若没有通过-f指定配置文件，则采用默认的配置文件
    if ((NULL == this->getConfFile()) || (strlen(this->getConfFile()) == 0))
    {
        this->setConfFile("svr_conf.xml");
    }
    ///加载配置文件
    if (0 != m_config.loadConfig(getConfFile()))
    {
        CWX_ERROR((m_config.getError()));
        return -1;
    }
    if (m_config.m_uiDataSize > sizeof(m_szBuf100K) -1) m_config.m_uiDataSize = sizeof(m_szBuf100K) -1;
    ///设置输出运行日志的level
    setLogLevel(CwxAppLogger::LEVEL_ERROR|CwxAppLogger::LEVEL_INFO|CwxAppLogger::LEVEL_WARNING);
    return 0;
}

//init the Enviroment before run.0:success, -1:failure.
int CwxEchoTbClientApp::initRunEnv()
{
    CWX_UINT32 i=0;
    ///设置时钟的刻度，最小为1ms，此为1s。
    this->setClick(1000);//1s
    //set work dir
    this->setWorkDir(this->m_config.m_strWorkDir.c_str());
    //Set log file
    this->setLogFileNum(LOG_FILE_NUM);
    this->setLogFileSize(LOG_FILE_SIZE*1024*1024);
    ///调用架构的initRunEnv，使设置的参数生效
    if (CwxAppFramework::initRunEnv() == -1 ) return -1;
    //set version
    this->setAppVersion(ECHO_TB_CLIENT_APP_VERSION);
    //set last modify date
    this->setLastModifyDatetime(ECHO_TB_CLIENT_MODIFY_DATE);
    //set compile date
    this->setLastCompileDatetime(CWX_COMPILE_DATE(_BUILD_DATE));

    //output config
    string strConfOut;
    m_config.outputConfig(strConfOut);
    CWX_INFO((strConfOut.c_str()));
    m_unEchoGroupNum = m_config.m_svrGroup.size();
    if (0 == m_unEchoGroupNum)
    {
        CWX_ERROR(("No config echo svr"));
        return -1;
    }
    ///创建事件handle
    m_pEventHandle = new CwxEchoTbEventHandler(this);
    ///注册handle
    for (i=0; i<m_unEchoGroupNum; i++)
    {
        getCommander().regHandle(SVR_TYPE_ECHO_BASE + i, m_pEventHandle);
    }

    ///打开管理端口
    if (m_config.m_mgrListen.getHostName().length())
    {
        if (-1 == noticeMgrListen(m_config.m_mgrListen.getHostName().c_str(),
            m_config.m_mgrListen.getPort()))
        {
            CWX_ERROR(("Failure to register mgr listen, addr=%s, port=%u",
                m_config.m_mgrListen.getHostName().c_str(),
                m_config.m_mgrListen.getPort()));
            return -1;
        }
    }
    //连接echo server
    CwxEchoTbClientGroup* group;
    CWX_UINT32 uiHostId=0;
    CWX_UINT16 j=0;
    list<CwxHostInfo>::iterator iter_host;
    list<CwxEchoTbClientGroup*>::iterator iter= m_config.m_svrGroup.begin();
    CWX_UINT32 uiSvrId = SVR_TYPE_ECHO_BASE;
    while(iter != m_config.m_svrGroup.end())
    {
        group = *iter;
        iter_host= group->m_hosts.begin();
        uiHostId = 1;
        while(iter_host != group->m_hosts.end())
        {
            for (j=0; j<m_config.m_unConnNum; j++)
            {
                if (iter_host->getUnixDomain().length())
                {
                    if (0 > this->noticeLsockConnect(uiSvrId,
                        uiHostId,
                        iter_host->getUnixDomain().c_str(),
                        false,
                        0,
                        false))
                    {
                        CWX_ERROR(("Can't connect the echo service: unix=%s",
                            iter_host->getUnixDomain().c_str()));
                        return -1;
                    }
                }
                else
                {
                    if (0 > this->noticeTcpConnect(uiSvrId,
                        uiHostId,
                        iter_host->getHostName().c_str(),
                        iter_host->getPort(),
                        false,
                        0,
                        false))
                    {
                        CWX_ERROR(("Can't connect the echo service: addr=%s, port=%d",
                            iter_host->getHostName().c_str(),
                            iter_host->getPort()));
                        return -1;
                    }
                }
            }
            uiHostId++;
            iter_host++;
        }
        uiSvrId++;
        iter++;
    }

    ///创建线程池对象，此线程池中线程的group-id为2，线程池的线程数量为m_config.m_unThreadNum。
    m_threadPool = new CwxAppThreadPoolEx(this, 2, m_config.m_unThreadNum);
    ///启动线程，线程池中线程的线程栈大小为1M。
    if ( 0 != m_threadPool->start(NULL))
    {
        CWX_ERROR(("Failure to start thread pool"));
        return -1;
    }

    //发送消息
    CwxEchoTbClientTask* pTask;
    for (i=0; i<m_config.m_uiTaskNum; i++)
    {
        pTask = new CwxEchoTbClientTask(this, &getTaskBoard());
        pTask->execute(this->getAppTss());
    }

    return 0;
}

///时钟响应函数，什么也没有做
void CwxEchoTbClientApp::onTime(CwxTimeValue const& current)
{
    static CWX_UINT64 ullLastTime = CwxDate::getTimestamp();
    CwxAppFramework::onTime(current);
    if (current.to_usec() > ullLastTime + 1000000)
    {//1s
        ///形成超时检查事件，由CwmCenterUiQuery的onTimeoutCheck的响应
        ullLastTime = current.to_usec();
        CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(0);
        pBlock->event().setSvrId(SVR_TYPE_ECHO_BASE);
        pBlock->event().setHostId(0);
        pBlock->event().setConnId(0);
        pBlock->event().setEvent(CwxEventInfo::TIMEOUT_CHECK);
        m_threadPool->append(pBlock, 0);
    }

}

///信号处理函数
void CwxEchoTbClientApp::onSignal(int signum)
{
    switch(signum)
    {
    case SIGQUIT: 
        CWX_INFO(("Recv exit signal, exit right now."));
        this->stop();
        break;
    default:
        ///其他信号，忽略
        CWX_INFO(("Recv signal=%d, ignore it.", signum));
        break;
    }
}

//echo连接建立函数
int CwxEchoTbClientApp::onConnCreated(CwxAppHandler4Msg& conn, bool& , bool& )
{
    if (conn.getConnInfo().getSvrId() >= SVR_TYPE_ECHO_BASE)
    {
        conn.getConnInfo().setMaxWaitingMsgNum(m_config.m_uiWaitMsgNum);
    }
    return 0;
}

///echo回复的消息响应函数
int CwxEchoTbClientApp::onRecvMsg(CwxMsgBlock* msg, CwxAppHandler4Msg const& conn, CwxMsgHead const& header, bool& )
{

    msg->event().setSvrId(conn.getConnInfo().getSvrId());
    msg->event().setHostId(conn.getConnInfo().getHostId());
    msg->event().setConnId(conn.getConnInfo().getConnId());
    msg->event().setEvent(CwxEventInfo::RECV_MSG);
    msg->event().setMsgHeader(header);
    msg->event().setTimestamp(CwxDate::getTimestamp());
    m_threadPool->append(msg, conn.getConnInfo().getConnId());
    return 0;
}

int CwxEchoTbClientApp::onConnClosed(CwxAppHandler4Msg const& conn)
{
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(0);
    pBlock->event().setSvrId(conn.getConnInfo().getSvrId());
    pBlock->event().setHostId(conn.getConnInfo().getHostId());
    pBlock->event().setConnId(conn.getConnInfo().getConnId());
    pBlock->event().setEvent(CwxEventInfo::CONN_CLOSED);
    m_threadPool->append(pBlock, conn.getConnInfo().getConnId());
    return 0;
}

CWX_UINT32 CwxEchoTbClientApp::onEndSendMsg(CwxMsgBlock*& msg,
                                CwxAppHandler4Msg const& conn)
{
    msg->event().setSvrId(conn.getConnInfo().getSvrId());
    msg->event().setHostId(conn.getConnInfo().getHostId());
    msg->event().setConnId(conn.getConnInfo().getConnId());
    msg->event().setEvent(CwxEventInfo::END_SEND_MSG);
    m_threadPool->append(msg, conn.getConnInfo().getConnId());
    msg = NULL;
    return 0;
}
void CwxEchoTbClientApp::onFailSendMsg(CwxMsgBlock*& msg)
{
    msg->event().setSvrId(msg->send_ctrl().getSvrId());
    msg->event().setHostId(msg->send_ctrl().getHostId());
    msg->event().setConnId(msg->send_ctrl().getConnId());
    msg->event().setEvent(CwxEventInfo::FAIL_SEND_MSG);
    m_threadPool->append(msg, msg->send_ctrl().getConnId());
    msg = NULL;
}


void CwxEchoTbClientApp::destroy()
{
    if (m_threadPool){
        m_threadPool->stop();
        delete m_threadPool;
        m_threadPool = NULL;
    }
    if (m_pEventHandle)
    {
        delete m_pEventHandle;
        m_pEventHandle = NULL;
    }
    CwxAppFramework::destroy();
}


