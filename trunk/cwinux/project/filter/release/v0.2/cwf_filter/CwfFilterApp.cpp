#include "CwfFilterApp.h"
#include "CwxDate.h"

///构造函数
CwfFilterApp::CwfFilterApp(){
    m_pFilterHandler = NULL;
    m_pThreadPool = NULL;
    m_pFilterMgr = NULL;
}

///析构函数
CwfFilterApp::~CwfFilterApp(){
}

///初始化
int CwfFilterApp::init(int argc, char** argv){
    string strErrMsg;
    ///首先调用架构的init api
    if (CwxAppFramework::init(argc, argv) == -1) return -1;
    ///检查是否通过-f指定了配置文件，若没有，则采用默认的配置文件
    if ((NULL == this->getConfFile()) || (strlen(this->getConfFile()) == 0)){
        this->setConfFile("svr_conf.xml");
    }
    ///加载配置文件，若失败则退出
    if (0 != m_config.loadConfig(getConfFile())){
        CWX_ERROR((m_config.getError()));
        return -1;
    }
    ///设置运行日志的输出level
    setLogLevel(CwxAppLogger::LEVEL_ERROR|CwxAppLogger::LEVEL_INFO|CwxAppLogger::LEVEL_WARNING);
    return 0;
}

///配置运行环境信息
int CwfFilterApp::initRunEnv(){
    ///设置系统的时钟间隔，最小刻度为1ms，此为1s。
    this->setClick(1000);//1s
    ///设置工作目录
    this->setWorkDir(this->m_config.m_strWorkDir.c_str());
    ///设置循环运行日志的数量
    this->setLogFileNum(LOG_FILE_NUM);
    ///设置每个日志文件的大小
    this->setLogFileSize(LOG_FILE_SIZE*1024*1024);
    ///调用架构的initRunEnv，使以上设置的参数生效
    if (CwxAppFramework::initRunEnv() == -1 ) return -1;
    ///将加载的配置文件信息输出到日志文件中，以供查看检查
    m_config.outputConfig();
    //set version
    this->setAppVersion(FILTER_APP_VERSION);
    //set last modify date
    this->setLastModifyDatetime(FILTER_MODIFY_DATE);
    //set compile date
    this->setLastCompileDatetime(CWX_COMPILE_DATE(_BUILD_DATE));

    //打开监听端口
    if (-1 == this->noticeMgrListen(m_config.m_mgrListen.getHostName().c_str(),
        m_config.m_mgrListen.getPort(),
        true))
    {
        CWX_ERROR(("Failure to open mgr listen, ip=%s, port=%u", m_config.m_mgrListen.getHostName().c_str(), m_config.m_mgrListen.getPort()));
        return -1;
    }

    ///初始化过滤器管理器对象
    m_pFilterMgr = new CwfFilterMgr();
    if (0 != m_pFilterMgr->init(m_config, getAppTss()->m_szBuf2K))
    {
        CWX_ERROR(("Failure to init Filter mgr, err=%s", getAppTss()->m_szBuf2K));
        return -1;
    }
    ///创建filter handler
    m_pFilterHandler = new CwfFilterHandler(this);
    ///注册echo请求的处理handle，echo请求的svr-id为SVR_TYPE_ECHO
    this->getCommander().regHandle(SVR_TYPE_FILTER, m_pFilterHandler);
    ///监听TCP连接，其建立的连接的svr-id都为SVR_TYPE_FILTER，接收的消息的svr-id都为SVR_TYPE_FILTER。
    ///全部由m_pFilterHandler对象来处理, no-keep-alive
    if (m_config.m_filterListen.getHostName().length())
    {
        if (0 > this->noticeTcpListen(SVR_TYPE_FILTER, 
            this->m_config.m_filterListen.getHostName().c_str(),
            this->m_config.m_filterListen.getPort()))
        {
            CWX_ERROR(("Can't register the filter acceptor port: addr=%s, port=%d",
                this->m_config.m_filterListen.getHostName().c_str(),
                this->m_config.m_filterListen.getPort()));
            return -1;
        }
    }
    ///监听UNIX DOMAIN连接，其建立的连接的svr-id都为SVR_TYPE_FILTER，接收的消息的svr-id都为SVR_TYPE_FILTER。
    ///全部由m_pFilterHandler对象来处理, no-keep-alive
    if (m_config.m_strUnixPathFile.length())
    {
        if (0 > this->noticeLsockListen(SVR_TYPE_FILTER, 
            this->m_config.m_strUnixPathFile.c_str()))
        {
            CWX_ERROR(("Can't register the filter unix acceptor port: path=%s",
                m_config.m_strUnixPathFile.c_str()));
            return -1;
        }
    }
    ///创建线程池对象，此线程池中线程的group-id为THREAD_GROUP_USER_START，
    ///线程池的线程数量为m_config.m_unThreadNum。
    m_pThreadPool = new CwxAppThreadPool(this,
        CwxAppFramework::THREAD_GROUP_USER_START,
        m_config.m_unThreadNum);
    ///创建线程的tss对象
    CwxAppTss** pTss = new CwxAppTss*[m_config.m_unThreadNum];
    for (CWX_UINT32 i=0; i<m_config.m_unThreadNum; i++){
        pTss[i] = new CwfFilterTss();
        ((CwfFilterTss*)pTss[i])->init();
    }
    ///启动线程，线程池中线程的线程栈大小为64k。
    if ( 0 != m_pThreadPool->start(pTss, 64 * 1024))
    {
        CWX_ERROR(("Failure to start thread pool"));
        return -1;
    }
    return 0;

}

///时钟函数，什么也没有做
void CwfFilterApp::onTime(CwxTimeValue const& current)
{
    CwxAppFramework::onTime(current);
}

///信号处理函数
void CwfFilterApp::onSignal(int signum)
{
    switch(signum){
    case SIGQUIT: 
        CWX_INFO(("Recv exit signal , exit right now."));
        this->stop();
        break;
    default:
        ///其他信号，全部忽略
        CWX_INFO(("Recv signal=%d , ignore it.", signum));
        break;
    }

}

///echo请求的请求消息
int CwfFilterApp::onRecvMsg(CwxMsgBlock* msg,
                            CwxAppHandler4Msg const& conn,
                            CwxMsgHead const& header,
                            bool& )
{
    msg->event().setSvrId(conn.getConnInfo().getSvrId());
    msg->event().setHostId(conn.getConnInfo().getHostId());
    msg->event().setConnId(conn.getConnInfo().getConnId());
    msg->event().setMsgHeader(header);
    msg->event().setEvent(CwxEventInfo::RECV_MSG);
    msg->event().setTimestamp(CwxDate::getTimestamp());
    ///将消息放到线程池队列中，有内部的线程调用其处理handle来处理
    m_pThreadPool->append(msg);
    return 0;
}

void CwfFilterApp::destroy()
{
    if (m_pThreadPool)
    {
        m_pThreadPool->stop();
        delete m_pThreadPool;
        m_pThreadPool = NULL;
    }
    if (m_pFilterMgr)
    {
        delete m_pFilterMgr;
        m_pFilterMgr = NULL;
    }
    if (m_pFilterHandler)
    {
        delete m_pFilterHandler;
        m_pFilterHandler = NULL;
    }
    CwxAppFramework::destroy();
}


