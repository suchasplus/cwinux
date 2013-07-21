#include "CwxEchoClientApp.h"

///构造函数，初始化发送的echo数据内容
CwxEchoClientApp::CwxEchoClientApp():m_uiSendNum(0){
    CWX_UINT32 i=0;
    for (i=0; i<MAX_MSG_SIZE; i++){
        m_szBuf[i] = 'a' + i % 26;
    }
    m_szBuf[i] = 0x00;
}
///析构函数
CwxEchoClientApp::~CwxEchoClientApp(){

}

///初始化APP，加载配置文件
int CwxEchoClientApp::init(int argc, char** argv){
    string strErrMsg;
    ///首先调用架构的init
    if (CwxAppFramework::init(argc, argv) == -1) return -1;
    ///若没有通过-f指定配置文件，则采用默认的配置文件
    if ((NULL == this->getConfFile()) || (strlen(this->getConfFile()) == 0)){
        this->setConfFile("svr_conf.cnf");
    }
    ///加载配置文件
    if (0 != m_config.loadConfig(getConfFile())){
        CWX_ERROR((m_config.getError()));
        return -1;
    }
    if (m_config.m_uiDataSize > MAX_MSG_SIZE) m_config.m_uiDataSize = MAX_MSG_SIZE;
    ///设置输出运行日志的level
    setLogLevel(CwxLogger::LEVEL_ERROR|CwxLogger::LEVEL_INFO|CwxLogger::LEVEL_WARNING);
    return 0;
}

//init the Enviroment before run.0:success, -1:failure.
int CwxEchoClientApp::initRunEnv(){
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
    this->setAppVersion(ECHO_CLIENT_APP_VERSION);
    //set last modify date
    this->setLastModifyDatetime(ECHO_CLIENT_MODIFY_DATE);
    //set compile date
    this->setLastCompileDatetime(CWX_COMPILE_DATE(_BUILD_DATE));

    //output config
    string strConfOut;
    m_config.outputConfig(strConfOut);
    CWX_INFO((strConfOut.c_str()));


    CWX_UINT16 i=0;
    //建立配置文件中设置的、与echo服务的连接
    for (i=0; i<m_config.m_unConnNum; i++){
        if (m_config.m_bTcp){
            //create  conn
            if (0 > this->noticeTcpConnect(SVR_TYPE_ECHO,
                0,
                this->m_config.m_listen.getHostName().c_str(),
                this->m_config.m_listen.getPort(),
                false,
                1,
                60,
                CwxEchoClientApp::setSockAttr,
                this))
            {
                CWX_ERROR(("Can't connect the echo service: addr=%s, port=%d",
                    this->m_config.m_listen.getHostName().c_str(),
                    this->m_config.m_listen.getPort()));
                return -1;
            }
        }else{
            //create  conn
            if (0 > this->noticeLsockConnect(SVR_TYPE_ECHO,
                0,
                this->m_config.m_strUnixPathFile.c_str()))
            {
                CWX_ERROR(("Can't connect the echo service: addr=%s, port=%d",
                    this->m_config.m_listen.getHostName().c_str(),
                    this->m_config.m_listen.getPort()));
                return -1;
            }
        }
    }
    return 0;
}

///时钟响应函数，什么也没有做
void CwxEchoClientApp::onTime(CwxTimeValue const& current){
    CwxAppFramework::onTime(current);
}

///信号处理函数
void CwxEchoClientApp::onSignal(int signum){
    switch(signum){
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

