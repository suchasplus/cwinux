#include "CwmNerveApp.h"
#include <sys/wait.h>
///构造函数
CwmNerveApp::CwmNerveApp()
{
    m_uiAlarmConnId = 0;
    m_uiTaskId = 0;
    m_pAlarmHandler = NULL;
    m_pCgiHandler = NULL;
    m_pServerHandler = NULL;
    m_pQueryHandler = NULL;
    m_pDataMgr = NULL;

}

///析构函数
CwmNerveApp::~CwmNerveApp()
{

}

///初始化APP，加载配置文件
int CwmNerveApp::init(int argc, char** argv)
{
    ///首先调用架构的init
    if (CwxAppFramework::init(argc, argv) == -1) return -1;
    ///若没有通过-f指定配置文件，则采用默认的配置文件
    if ((NULL == this->getConfFile()) || (strlen(this->getConfFile()) == 0))
    {
        this->setConfFile("svr_conf.xml");
    }
    ///加载配置文件
    if (!m_config.loadConfig(getConfFile()))
    {
        CWX_ERROR((m_config.getErrMsg()));
        return -1;
    }
    ///设置输出运行日志的level
    setLogLevel(CwxAppLogger::LEVEL_ALL);
    return 0;
}

//init the Enviroment before run.0:success, -1:failure.
int CwmNerveApp::initRunEnv()
{
    ///设置时钟的刻度，最小为1ms，此为0.1s。
    this->setClick(100);//0.1s
    //set work dir
    this->setWorkDir(this->m_config.getWorkDir().c_str());
    //Set log file
    this->setLogFileNum(LOG_FILE_NUM);
    this->setLogFileSize(LOG_FILE_SIZE*1024*1024);
    ///调用架构的initRunEnv，使设置的参数生效
    if (CwxAppFramework::initRunEnv() == -1 ) return -1;
    //set version
    this->setAppVersion(CWM_APP_VERSION);
    //set last modify date
    this->setLastModifyDatetime(CWM_NERVE_MODIFY_DATE);
    //set compile date
    this->setLastCompileDatetime(CWX_COMPILE_DATE(_BUILD_DATE));
    //output config
    m_config.outputConfig();
    //打开监听端口
    if (-1 == this->noticeMgrListen(m_config.getMgrListen().getHostName().c_str(), m_config.getMgrListen().getPort(), true))
    {
       CWX_ERROR(("Failure to open mgr listen, ip=*, port=9010"));
       return -1;
    }
    //init data driver
    m_pDataMgr = new CwmCmnDataMgr();
    if (!m_pDataMgr->init(m_config.getDataPath() + CWM_NERVE_DB_FILE))
    {
        CWX_ERROR(("Failure to init Data file, err=%s, file=%s/%s", m_pDataMgr->getErrMsg(), m_config.getDataPath().c_str(),CWM_NERVE_DB_FILE));
        return -1;
    }
    ///创建消息的处理handler并进行注册，每个SVR-TYPE一个处理handle
    m_pQueryHandler = new CwmNerveQueryHandler(this);
    getCommander().regHandle(SVR_TYPE_QUERY, m_pQueryHandler);
    m_pAlarmHandler = new CwmNerveAlarmHandler(this);
    getCommander().regHandle(SVR_TYPE_ALARM, m_pAlarmHandler);
    m_pCgiHandler = new CwmNerveCgiHandler(this);
    getCommander().regHandle(SVR_TYPE_CGI_ACTION, m_pCgiHandler);
    m_pServerHandler = new CwmNerveServerHandler(this);
    getCommander().regHandle(SVR_TYPE_SERVER_ACTION, m_pServerHandler);

    //监听query
    int ret = 0;
    if (0 > (ret=this->noticeTcpListen(SVR_TYPE_QUERY,
        m_config.getQueryListen().getHostName().c_str(),
        m_config.getQueryListen().getPort(),
        false,
        0,
        true)))
    {
        CWX_ERROR(("Failure to listen query tcp, host=%s, port=%u", m_config.getQueryListen().getHostName().c_str(), m_config.getQueryListen().getPort()));
        return -1;
    }
    ///创建所管理的service的状态对象
    list<CwmNerveActionInfo>::const_iterator act_iter ;
    map<pair<string,string>, CwmNerveServiceInfo>::const_iterator iter = m_config.getServices().begin();
    CwmNerveServiceStatus* pServerStatus;
    CwmNerveActionStatus* pActStatus;
    while(iter != m_config.getServices().end())
    {
        pServerStatus = new CwmNerveServiceStatus();
        pServerStatus->setServiceInfo(&iter->second);
        ///若是server类型的service，则建立所管理的server的TCP连接
        if (!pServerStatus->getServiceInfo()->isCgi())
        {///connect server
            if (0 > (ret = noticeTcpConnect(SVR_TYPE_SERVER_ACTION, 0, iter->second.getServerIp().c_str(), iter->second.getPort(), false, 0, true, 1, 4)))
            {
                CWX_ERROR(("Failure to connect monitor server, host=%s, port=%u", iter->second.getServerIp().c_str(), iter->second.getPort()));
                return -1;
            }
            pServerStatus->getSelfStatus().setAlarmRule(&iter->second.getAlarmRule());
            pServerStatus->setConnId(ret);
            pServerStatus->setConnected(false);
        }
        ///add action
        act_iter = pServerStatus->getServiceInfo()->getActions().begin();
        while(act_iter != pServerStatus->getServiceInfo()->getActions().end())
        {
            pActStatus = new CwmNerveActionStatus();
            pActStatus->setActionInfo(&(*act_iter));
            pActStatus->setServiceStatus( pServerStatus);
            pServerStatus->addActionStatus(pActStatus);
            act_iter++;
        }
        m_lsService.push_back(pServerStatus);
        iter++;
    }

    //连接center的告警接收端口
    if (0 > (ret=this->noticeTcpConnect(SVR_TYPE_ALARM,
        0,
        m_config.getAlarmListen().getHostName().c_str(),
        m_config.getAlarmListen().getPort(),
        false,
        0,
        true,
        1,
        4
        )))
    {
        CWX_ERROR(("Failure to connect alarm listen, host=%s, port=%u", m_config.getAlarmListen().getHostName().c_str(), m_config.getAlarmListen().getPort())); 
        return -1;
    }
    m_uiAlarmConnId = ret;


    return 0;
}

///创建tss的对象
CwxAppTss* CwmNerveApp::onTssEnv()
{
    CwmNerveTss* pTss = new CwmNerveTss();
    if (0 != pTss->init())
    {
        CWX_ERROR(("Failure to init CwmNerveTss"));
        exit(0);
    }
    return pTss;
}

void CwmNerveApp::onTime(CwxTimeValue const& current)
{
    static CWX_UINT64 ullLastTime = CwxDate::getTimestamp();
    CwxAppFramework::onTime(current);
    if (current.to_usec() > ullLastTime + 200000)
    {//0.2s
        ///形成timeout check的事件，有CwmNerverQueryHandler的onTimeoutCheck来处理
        ullLastTime = current.to_usec();
        CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(0);
        pBlock->event().setSvrId(SVR_TYPE_QUERY);
        pBlock->event().setHostId(0);
        pBlock->event().setConnId(0);
        pBlock->event().setEvent(CwxEventInfo::TIMEOUT_CHECK);
        dispatch(pBlock);
    }
}

///信号处理函数
void CwmNerveApp::onSignal(int signum)
{
    int statues=0;
    switch(signum)
    {
    case SIGQUIT: 
        CWX_INFO(("Recv exit signal, exit right now."));
        this->stop();
        break;
    case SIGCHLD:
        ///cgi的进程退出，进行进程状态信息的回收，否则将形成僵尸进程
        wait(&statues);
        break;
    default:
        ///其他信号，忽略
        CWX_INFO(("Recv signal=%d, ignore it.", signum));
        break;
    }
}

///连接建立响应函数
int CwmNerveApp::onConnCreated(CwxAppHandler4Msg& conn, bool& , bool& )
{
    if ((SVR_TYPE_ALARM == conn.getConnInfo().getSvrId()) ||  ///若告警报告的连接建立，需要出发滞留告警的提交
        (SVR_TYPE_SERVER_ACTION == conn.getConnInfo().getSvrId())) ///若是负责管理的server的连接建立，则需要修改其连接状态
    {
        CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(0);
        ///将连接的信息设置到events对象中
        pBlock->event().setSvrId(conn.getConnInfo().getSvrId());
        pBlock->event().setHostId(conn.getConnInfo().getHostId());
        pBlock->event().setConnId(conn.getConnInfo().getConnId());
        ///设置事件的类型
        pBlock->event().setEvent(CwxEventInfo::CONN_CREATED);
        ///分发事件
        dispatch(pBlock);
    }
    return 0;
}

///连接关闭
int CwmNerveApp::onConnClosed(CwxAppHandler4Msg const& conn)
{
    ///若非query的连接关闭，都需要产生连接关闭的事件并进行分发
    if (SVR_TYPE_QUERY != conn.getConnInfo().getSvrId())
    {
        CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(0);
        pBlock->event().setSvrId(conn.getConnInfo().getSvrId());
        pBlock->event().setHostId(conn.getConnInfo().getHostId());
        pBlock->event().setConnId(conn.getConnInfo().getConnId());
        pBlock->event().setConnUserData(conn.getConnInfo().getUserData());
        pBlock->event().setEvent(CwxEventInfo::CONN_CLOSED);
        dispatch(pBlock);
    }
    return 0;
}


///响应函数
int CwmNerveApp::onRecvMsg(CwxMsgBlock* msg, CwxAppHandler4Msg const& conn, CwxMsgHead const& header, bool& )
{

    msg->event().setSvrId(conn.getConnInfo().getSvrId());
    msg->event().setHostId(conn.getConnInfo().getHostId());
    msg->event().setConnId(conn.getConnInfo().getConnId());
    msg->event().setConnUserData(conn.getConnInfo().getUserData());
    msg->event().setMsgHeader(header);
    msg->event().setEvent(CwxEventInfo::RECV_MSG);
    msg->event().setTimestamp(CwxDate::getTimestamp());
    msg->event().setTaskId(msg->event().getMsgHeader().getTaskId());
    this->dispatch(msg);
    return 0;
}

CWX_UINT32 CwmNerveApp::onEndSendMsg(CwxMsgBlock*& msg, CwxAppHandler4Msg const& conn)
{
    if (SVR_TYPE_QUERY != conn.getConnInfo().getSvrId())
    {
        msg->event().setSvrId(conn.getConnInfo().getSvrId());
        msg->event().setHostId(conn.getConnInfo().getHostId());
        msg->event().setConnId(conn.getConnInfo().getConnId());
        msg->event().setConnUserData(conn.getConnInfo().getUserData());
        msg->event().setEvent(CwxEventInfo::END_SEND_MSG);
        dispatch(msg);
        ///将msg置为空，防止底层释放
        msg = NULL;
    }
    return CwxMsgSendCtrl::UNDO_CONN;
}

///发送数据失败
void CwmNerveApp::onFailSendMsg(CwxMsgBlock*& msg)
{
    if (SVR_TYPE_QUERY != msg->send_ctrl().getSvrId())
    {
        msg->event().setSvrId(msg->send_ctrl().getSvrId());
        msg->event().setHostId(msg->send_ctrl().getHostId());
        msg->event().setConnId(msg->send_ctrl().getConnId());
        msg->event().setEvent(CwxEventInfo::FAIL_SEND_MSG);
        dispatch(msg);
        ///将msg置为空，防止底层释放
        msg = NULL;
    }
}

///是否各种资源
void CwmNerveApp::destroy()
{
    m_config.reset();
    m_uiAlarmConnId = 0;
    m_uiTaskId = 0;
    list<CwmNerveServiceStatus*>::iterator iter = m_lsService.begin();
    while(iter != m_lsService.end())
    {
        delete *iter;
        iter++;
    }
    m_lsService.clear();
    if (m_pAlarmHandler)
    {
        delete m_pAlarmHandler;
        m_pAlarmHandler = NULL;
    }
    if (m_pCgiHandler)
    {
        delete m_pCgiHandler;
        m_pCgiHandler = NULL;
    }
    if (m_pServerHandler)
    {
        delete m_pServerHandler;
        m_pServerHandler = NULL;
    }
    if (m_pQueryHandler)
    {
        delete m_pQueryHandler;
        m_pQueryHandler = NULL;
    }
    if (m_pDataMgr) delete m_pDataMgr;
    m_pDataMgr = NULL;
    ///最后必须调用父类的destroy api，以释放父类的资源
    CwxAppFramework::destroy();
}

///实现消息的分发
void CwmNerveApp::dispatch(CwxMsgBlock* msg)
{
    int iRet = 0;
    ///获取事件的类型
    CWX_UINT32 uiEventType = msg->event().getEvent();
    CWX_UINT32 uiSvrId = msg->event().getSvrId();
    ///基于msg中的SVR-ID选择处理handler，基于event-type确定调用的handler的响应方法
    if (!this->getCommander().dispatch(msg, getAppTss(), iRet))
    {
        CWX_DEBUG(("No handle to deal with event: event_type=%u, svr_id=%u", uiEventType, uiSvrId));
    }
    if (1 != iRet)
    {
        if (0 == iRet)
        {///此消息对应的handler不处理
            CWX_DEBUG(("No care the event for  event_type=%u, svr_id=%u", uiEventType, uiSvrId));
        }
        else
        {///消息处理失败
            CWX_DEBUG(("Failure to act the event for  event_type=%u, svr_id=%u", uiEventType, uiSvrId));
        }
    }
    ///如果事件处理对象没有释放或保留msg，则在此处统一释放
    if (msg) CwxMsgBlockAlloc::free(msg);
}


