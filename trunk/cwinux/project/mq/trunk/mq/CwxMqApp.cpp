#include "CwxMqApp.h"
#include "CwxDate.h"

///���캯��
CwxMqApp::CwxMqApp()
{
    m_uiCurSid = 1; ///<��С��sidΪ1
    m_pBinLogMgr = NULL;
    m_pAsyncHandler = NULL;
    m_pMasterHandler = NULL;
    m_pRecvHandler = NULL;
    m_pFetchHandler = NULL;
    m_pWriteThreadPool = NULL;
    m_sysFile = NULL;
    m_queueMgr = NULL;
}

///��������
CwxMqApp::~CwxMqApp()
{

}

///��ʼ��
int CwxMqApp::init(int argc, char** argv)
{
    string strErrMsg;
    ///���ȵ��üܹ���init api
    if (CwxAppFramework::init(argc, argv) == -1) return -1;
    ///����Ƿ�ͨ��-fָ���������ļ�����û�У������Ĭ�ϵ������ļ�
    if ((NULL == this->getConfFile()) || (strlen(this->getConfFile()) == 0))
    {
        this->setConfFile("svr_conf.xml");
    }
    ///���������ļ�����ʧ�����˳�
    if (0 != m_config.loadConfig(getConfFile()))
    {
        CWX_ERROR((m_config.getErrMsg()));
        return -1;
    }
    ///����������־�����level
    setLogLevel(CwxAppLogger::LEVEL_ERROR|CwxAppLogger::LEVEL_INFO|CwxAppLogger::LEVEL_WARNING);
    return 0;
}

///�������л�����Ϣ
int CwxMqApp::initRunEnv()
{
    ///����ϵͳ��ʱ�Ӽ������С�̶�Ϊ1ms����Ϊ1s��
    this->setClick(1000);//1s
    ///���ù���Ŀ¼
    this->setWorkDir(m_config.getCommon().m_strWorkDir.c_str());
    ///����ѭ��������־������
    this->setLogFileNum(LOG_FILE_NUM);
    ///����ÿ����־�ļ��Ĵ�С
    this->setLogFileSize(LOG_FILE_SIZE*1024*1024);
    ///���üܹ���initRunEnv��ʹ�������õĲ�����Ч
    if (CwxAppFramework::initRunEnv() == -1 ) return -1;
    ///�����ص������ļ���Ϣ�������־�ļ��У��Թ��鿴���
    m_config.outputConfig();
    ///block����signal
    this->blockSignal(SIGTERM);
    this->blockSignal(SIGUSR1);
    this->blockSignal(SIGUSR2);
    this->blockSignal(SIGCHLD);
    this->blockSignal(SIGCLD);
    this->blockSignal(SIGHUP);
    this->blockSignal(SIGPIPE);
    this->blockSignal(SIGALRM);
    this->blockSignal(SIGCONT);
    this->blockSignal(SIGSTOP);
    this->blockSignal(SIGTSTP);
    this->blockSignal(SIGTTOU);

    //set version
    this->setAppVersion(CWX_MQ_VERSION);
    //set last modify date
    this->setLastModifyDatetime(CWX_MQ_MODIFY_DATE);
    //set compile date
    this->setLastCompileDatetime(CWX_COMPILE_DATE(_BUILD_DATE));
    ///���÷���״̬
    this->setAppRunValid(false);
    ///���÷�����Ϣ
    this->setAppRunFailReason("Starting..............");

    //����binlog������
    if (0 != startBinLogMgr()) return -1;
    CwxMqPoco::init(NULL);

    //�򿪼����˿�
    if (m_config.getCommon().m_mgrListen.getHostName().length())
    {
        if (-1 == this->noticeMgrListen(m_config.getCommon().m_mgrListen.getHostName().c_str(),
            m_config.getCommon().m_mgrListen.getPort(),
            true,
            new CwxMqMgrServer(this)))
        {
            CWX_ERROR(("Failure to open mgr listen, ip=%s, port=%u",
                m_config.getCommon().m_mgrListen.getHostName().c_str(),
                m_config.getCommon().m_mgrListen.getPort()));
            return -1;
        }
    }
    
    if (m_config.getCommon().m_bMaster)
    {
        ///ע�����ݽ���handler
        m_pRecvHandler = new CwxMqRecvHandler(this);
        getCommander().regHandle(SVR_TYPE_RECV, m_pRecvHandler);
    }else{
        ///ע��slave��master���ݽ���handler
        m_pMasterHandler = new CwxMqMasterHandler(this);
        getCommander().regHandle(SVR_TYPE_MASTER, m_pMasterHandler);
    }
    ///�����첽�ַ���handler
    m_pAsyncHandler = new CwxMqAsyncHandler(this);
    getCommander().regHandle(SVR_TYPE_ASYNC, m_pAsyncHandler);
    ///����mq�Ļ�ȡhandler
    m_pFetchHandler = new CwxMqFetchHandler(this);
    getCommander().regHandle(SVR_TYPE_FETCH, m_pFetchHandler);

    ///�����������������
    if (0 != startNetwork()) return -1;

    ///����recv�̳߳ض��󣬴��̳߳����̵߳�group-idΪTHREAD_GROUP_USER_START��
    ///�̳߳ص��߳�����Ϊ1��
    m_pWriteThreadPool = new CwxAppThreadPool(this,
        CwxAppFramework::THREAD_GROUP_USER_START,
        1);
    ///�����̵߳�tss����
    CwxAppTss** pTss = new CwxAppTss*[1];
    pTss[0] = new CwxMqTss();
    ((CwxMqTss*)pTss[0])->init();
    ///�����߳�
    if ( 0 != m_pWriteThreadPool->start(pTss))
    {
        CWX_ERROR(("Failure to start recv thread pool"));
        return -1;
    }
    ///���·����״̬
    updateAppRunState();
    return 0;
}

///ʱ�Ӻ���
void CwxMqApp::onTime(CwxTimeValue const& current)
{
    ///���û����onTime����
    CwxAppFramework::onTime(current);
    ///��鳬ʱ
    static time_t ttLastTime = time(NULL);
    if (m_config.getCommon().m_bMaster && (time(NULL) >= ttLastTime + 1))
    {
        ttLastTime = time(NULL);
        CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(0);
        pBlock->event().setSvrId(SVR_TYPE_RECV);
        pBlock->event().setEvent(CwxEventInfo::TIMEOUT_CHECK);
        //����ʱ����¼��������¼�����
        m_pWriteThreadPool->append(pBlock);
    }
}

///�źŴ�������
void CwxMqApp::onSignal(int signum)
{
    switch(signum)
    {
    case SIGQUIT: 
        ///����ؽ���֪ͨ�˳������Ƴ�
        CWX_INFO(("Recv exit signal, exit right now."));
        this->stop();
        break;
    default:
        ///�����źţ�ȫ������
        CWX_INFO(("Recv signal=%d, ignore it.", signum));
        break;
    }
}

///���ӽ���
int CwxMqApp::onConnCreated(CwxAppHandler4Msg& conn, bool& , bool& )
{
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(0);
    pBlock->event().setSvrId(conn.getConnInfo().getSvrId());
    pBlock->event().setHostId(conn.getConnInfo().getHostId());
    pBlock->event().setConnId(conn.getConnInfo().getConnId());
    ///�����¼�����
    pBlock->event().setEvent(CwxEventInfo::CONN_CREATED);
    ///���¼����ӵ���Ϣ����
    m_pWriteThreadPool->append(pBlock);
    ///��Ϣ��������������
    if (SVR_TYPE_ASYNC == conn.getConnInfo().getSvrId())
    {
        int bufLen = 64 * 1024;
        while(setsockopt(conn.getHandle(), SOL_SOCKET, SO_RCVBUF, (void *)&bufLen, sizeof(bufLen)) < 0)
        {
            bufLen -= 1024;
            if (bufLen < 0) break;
        }
        bufLen = 1024 * 1024;
        while (setsockopt(conn.getHandle(), SOL_SOCKET, SO_SNDBUF, (void*)&bufLen, sizeof( bufLen)) < 0)
        {
            bufLen -= 1024;
            if (bufLen < 0) break;
        }
        conn.getConnInfo().setContinueSendNum(m_config.getCommon().m_uiDispatchWindowSize);
    }
    else if (SVR_TYPE_MASTER == conn.getConnInfo().getSvrId())
    {
        int bufLen = 1024 * 1024;
        while (setsockopt(conn.getHandle(), SOL_SOCKET, SO_RCVBUF, (void *)&bufLen, sizeof(bufLen)) < 0)
        {
            bufLen -= 1024;
            if (bufLen < 0) break;
        }
        bufLen = 64 * 1024;
        while (setsockopt(conn.getHandle(), SOL_SOCKET, SO_SNDBUF, (void*)&bufLen, sizeof( bufLen)) < 0)
        {
            bufLen -= 1024;
            if (bufLen < 0) break;
        }
        conn.getConnInfo().setContinueSendNum(m_config.getCommon().m_uiFromMasterWindowSize);
    }
    return 0;
}
///���ӹر�
int CwxMqApp::onConnClosed(CwxAppHandler4Msg const& conn)
{
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(0);
    pBlock->event().setSvrId(conn.getConnInfo().getSvrId());
    pBlock->event().setHostId(conn.getConnInfo().getHostId());
    pBlock->event().setConnId(conn.getConnInfo().getConnId());
    ///�����¼�����
    pBlock->event().setEvent(CwxEventInfo::CONN_CLOSED);
    m_pWriteThreadPool->append(pBlock);
    return 0;
}

///�յ���Ϣ
int CwxMqApp::onRecvMsg(CwxMsgBlock* msg,
                        CwxAppHandler4Msg const& conn,
                        CwxMsgHead const& header,
                        bool& )
{
    msg->event().setSvrId(conn.getConnInfo().getSvrId());
    msg->event().setHostId(conn.getConnInfo().getHostId());
    msg->event().setConnId(conn.getConnInfo().getConnId());
    ///������Ϣͷ
    msg->event().setMsgHeader(header);
    ///�����¼�����
    msg->event().setEvent(CwxEventInfo::RECV_MSG);
    ///����Ϣ�ŵ��̳߳ض����У����ڲ����̵߳����䴦��handle������
    m_pWriteThreadPool->append(msg);
    if (SVR_TYPE_ASYNC == conn.getConnInfo().getSvrId())
    {
//        if (conn.getConnInfo().getContinueRecvNum()) CWX_INFO(("Async continue %u", conn.getConnInfo().getContinueRecvNum()));
        return conn.getConnInfo().getContinueRecvNum() < m_config.getCommon().m_uiDispatchWindowSize?1:0;
    }
    else if (SVR_TYPE_MASTER == conn.getConnInfo().getSvrId())
    {
//        if (conn.getConnInfo().getContinueRecvNum()) CWX_INFO(("Master continue %u", conn.getConnInfo().getContinueRecvNum()));
        return conn.getConnInfo().getContinueRecvNum() < m_config.getCommon().m_uiFromMasterWindowSize?1:0;
    }
    return 0;
}

///��Ϣ�������
CWX_UINT32 CwxMqApp::onEndSendMsg(CwxMsgBlock*& msg,
                                  CwxAppHandler4Msg const& conn)
{
    if ((SVR_TYPE_ASYNC == conn.getConnInfo().getSvrId()) ||
        (SVR_TYPE_FETCH == conn.getConnInfo().getSvrId()))
    { ///<ֻ��ASYNC��SVR_TYPE_FETCH���ͣ��Ź���EndSendMsg���¼�
        msg->event().setSvrId(conn.getConnInfo().getSvrId());
        msg->event().setHostId(conn.getConnInfo().getHostId());
        msg->event().setConnId(conn.getConnInfo().getConnId());
        msg->event().setEvent(CwxEventInfo::END_SEND_MSG);
        m_pWriteThreadPool->append(msg);
        ///���msg����ֹ�ͷ�
        msg = NULL;
    }
    return 0;
}

///��Ϣ����ʧ��
void CwxMqApp::onFailSendMsg(CwxMsgBlock*& msg)
{
    if ((SVR_TYPE_ASYNC == msg->send_ctrl().getSvrId()) ||
        (SVR_TYPE_FETCH == msg->send_ctrl().getSvrId()))
    { ///<ֻ��ASYNC��SVR_TYPE_FETCH���ͣ��Ź���EndSendMsg���¼�
        msg->event().setSvrId(msg->send_ctrl().getSvrId());
        msg->event().setHostId(msg->send_ctrl().getHostId());
        msg->event().setConnId(msg->send_ctrl().getConnId());
        msg->event().setEvent(CwxEventInfo::FAIL_SEND_MSG);
        m_pWriteThreadPool->append(msg);
        ///���msg����ֹ�ͷ�
        msg = NULL;
    }
}


void CwxMqApp::destroy()
{
    if (m_pWriteThreadPool)
    {
        m_pWriteThreadPool->stop();
        delete m_pWriteThreadPool;
        m_pWriteThreadPool = NULL;
    }
    if (m_queueMgr)
    {
        delete m_queueMgr;
        m_queueMgr = NULL;
    }
    if (m_pAsyncHandler)
    {
        delete m_pAsyncHandler;
        m_pAsyncHandler = NULL;
    }
    if (m_pMasterHandler)
    {
        delete m_pMasterHandler;
        m_pMasterHandler = NULL;
    }
    if (m_pRecvHandler)
    {
        delete m_pRecvHandler;
        m_pRecvHandler = NULL;
    }
    if (m_pFetchHandler)
    {
        delete m_pFetchHandler;
        m_pFetchHandler = NULL;
    }

    if (m_pBinLogMgr)
    {
        m_pBinLogMgr->commit();
        delete m_pBinLogMgr;
        m_pBinLogMgr = NULL;
    }
    if (m_sysFile)
    {
        m_sysFile->commit();
        delete m_sysFile;
        m_sysFile = NULL;
    }
    CwxMqPoco::destory();

    CwxAppFramework::destroy();
}


///����binlog��������-1��ʧ�ܣ�0���ɹ�
int CwxMqApp::startBinLogMgr()
{
    ///��ʼ��binlog
    {
        CWX_UINT64 ullBinLogSize = m_config.getBinLog().m_uiBinLogMSize;
        ullBinLogSize *= 1024 * 1024;
        m_pBinLogMgr = new CwxBinLogMgr(m_config.getBinLog().m_strBinlogPath.c_str(),
            m_config.getBinLog().m_strBinlogPrex.c_str(),
            ullBinLogSize);
        if (0 != m_pBinLogMgr->init(m_config.getBinLog().m_uiMgrMaxDay,
            CWX_APP_TSS_2K_BUF))
        {///<���ʧ�ܣ��򷵻�-1
            CWX_ERROR(("Failure to init binlog manager, error:%s", CWX_APP_TSS_2K_BUF));
            return -1;
        }
        m_bFirstBinLog = true;
        m_ttLastCommitTime = time(NULL);
        m_uiUnCommitLogNum = 0;
        ///��ȡsid
        if (m_config.getBinLog().m_uiFlushNum > MAX_SKIP_SID_COUNT)
        {
            m_uiCurSid = m_pBinLogMgr->getMaxSid() + MAX_SKIP_SID_COUNT;
        }
        else if (m_config.getBinLog().m_uiFlushNum < MIN_SKIP_SID_COUNT)
        {
            m_uiCurSid = m_pBinLogMgr->getMaxSid() + MIN_SKIP_SID_COUNT;
        }
        else
        {
            m_uiCurSid = m_pBinLogMgr->getMaxSid() + m_config.getBinLog().m_uiFlushNum;
        }
    }
    //��ʼ��MQ
    if (m_config.getMq().m_queues.size())
    {
        ///��ʼ��mq�ַ���ϵͳ�ļ�
        {
            m_ttMqLastCommitTime = time(NULL);
            m_uiMqUnCommitLogNum = 0;
            if (m_sysFile) delete m_sysFile;
            set<string> queues;
            map<string, CwxMqConfigQueue>::const_iterator iter =  m_config.getMq().m_queues.begin();
            while(iter != m_config.getMq().m_queues.end())
            {
                queues.insert(iter->first);
                iter ++;
            }
            m_sysFile = new CwxMqSysFile(100, m_config.getBinLog().m_strBinlogPath + ".cwx_mq_fetch_pos.dat");
            if (0 != m_sysFile->init(queues))
            {
                CWX_ERROR(("Failure to init sys file, err:%s", m_sysFile->getErrMsg()));
                return -1;
            }
        }
        //��ʼ�����й�����
        if (m_queueMgr) delete m_queueMgr;
        m_queueMgr = new CwxMqQueueMgr();
        if (0 != m_queueMgr->init(m_pBinLogMgr,
            m_sysFile->getQueues(),
            m_config.getMq().m_queues))
        {
            CWX_ERROR(("Failure to init mq queue manager"));
            return -1;
        }
    }
    ///�ɹ�
    return 0;
}

int CwxMqApp::startNetwork()
{
    ///������
    if (m_config.getCommon().m_bMaster)
    {
        ///�򿪽��յ�listen
        if (m_config.getMaster().m_recv.getHostName().length())
        {
            if (0 > this->noticeTcpListen(SVR_TYPE_RECV, 
                m_config.getMaster().m_recv.getHostName().c_str(),
                m_config.getMaster().m_recv.getPort(),
                false,
                0,
                m_config.getMaster().m_recv.isKeepAlive()))
            {
                CWX_ERROR(("Can't register the recv tcp accept listen: addr=%s, port=%d",
                    m_config.getMaster().m_recv.getHostName().c_str(),
                    m_config.getMaster().m_recv.getPort()));
                return -1;
            }
        }
        if (m_config.getMaster().m_recv.getUnixDomain().length())
        {
            if (0 > this->noticeLsockListen(SVR_TYPE_RECV,
                m_config.getMaster().m_recv.getUnixDomain().c_str(),
                false,
                0,
                m_config.getMaster().m_recv.isKeepAlive()))
            {
                CWX_ERROR(("Can't register the recv unix-domain accept listen: path-file=%s",
                    m_config.getMaster().m_recv.getUnixDomain().c_str()));
                return -1;
            }
        }
        ///���첽�ַ�
        if (m_config.getMaster().m_async.getHostName().length())
        {
            if (0 > this->noticeTcpListen(SVR_TYPE_ASYNC, 
                m_config.getMaster().m_async.getHostName().c_str(),
                m_config.getMaster().m_async.getPort(),
                false,
                0,
                m_config.getMaster().m_async.isKeepAlive()))
            {
                CWX_ERROR(("Can't register the async-dispatch tcp accept listen: addr=%s, port=%d",
                    m_config.getMaster().m_async.getHostName().c_str(),
                    m_config.getMaster().m_async.getPort()));
                return -1;
            }
        }
        if (m_config.getMaster().m_async.getUnixDomain().length())
        {
            if (0 > this->noticeLsockListen(SVR_TYPE_ASYNC,
                m_config.getMaster().m_async.getUnixDomain().c_str(),
                false,
                0,
                m_config.getMaster().m_async.isKeepAlive()))
            {
                CWX_ERROR(("Can't register the async-dispatch unix-domain accept listen: path-file=%s",
                    m_config.getMaster().m_async.getUnixDomain().c_str()));
                return -1;
            }
        }
    }
    else
    {
        ///����master
        if (m_config.getSlave().m_master.getUnixDomain().length())
        {
            if (0 > this->noticeLsockConnect(SVR_TYPE_MASTER, 0, 
                m_config.getSlave().m_master.getUnixDomain().c_str(),
                false,
                0,
                false,
                1,
                2))
            {
                CWX_ERROR(("Can't register the master unix-domain connect: path-file=%s",
                    m_config.getSlave().m_master.getUnixDomain().c_str()));
                return -1;
            }
        }
        else if (m_config.getSlave().m_master.getHostName().length())
        {
            if (0 > this->noticeTcpConnect(SVR_TYPE_MASTER, 0, 
                m_config.getSlave().m_master.getHostName().c_str(),
                m_config.getSlave().m_master.getPort(),
                false,
                0,
                m_config.getSlave().m_master.isKeepAlive(),
                1,
                2))
            {
                CWX_ERROR(("Can't register the master tcp connect: addr=%s, port=%d",
                    m_config.getSlave().m_master.getHostName().c_str(),
                    m_config.getSlave().m_master.getPort()));
                return -1;
            }
        }
        else
        {
            CWX_ERROR(("Not config master dispatch"));
            return -1;
        }
        ///�첽�ַ�
        if (m_config.getSlave().m_async.getHostName().length())
        {
            if (0 > this->noticeTcpListen(SVR_TYPE_ASYNC, 
                m_config.getSlave().m_async.getHostName().c_str(),
                m_config.getSlave().m_async.getPort(),
                false,
                0,
                m_config.getSlave().m_async.isKeepAlive()))
            {
                CWX_ERROR(("Can't register the async-dispatch tcp accept listen: addr=%s, port=%d",
                    m_config.getSlave().m_async.getHostName().c_str(),
                    m_config.getSlave().m_async.getPort()));
                return -1;
            }
        }
        if (m_config.getSlave().m_async.getUnixDomain().length())
        {
            if (0 > this->noticeLsockListen(SVR_TYPE_ASYNC,
                m_config.getSlave().m_async.getUnixDomain().c_str(),
                false,
                0,
                m_config.getSlave().m_async.isKeepAlive()))
            {
                CWX_ERROR(("Can't register the async-dispatch unix-domain accept listen: path-file=%s",
                    m_config.getSlave().m_async.getUnixDomain().c_str()));
                return -1;
            }
        }
    }
    //��mq��ȡ�ļ����˿�
    ///�첽�ַ�
    if (m_config.getMq().m_listen.getHostName().length())
    {
        if (0 > this->noticeTcpListen(SVR_TYPE_FETCH, 
            m_config.getMq().m_listen.getHostName().c_str(),
            m_config.getMq().m_listen.getPort(),
            false,
            0,
            m_config.getMq().m_listen.isKeepAlive()))
        {
            CWX_ERROR(("Can't register the mq-fetch tcp accept listen: addr=%s, port=%d",
                m_config.getMq().m_listen.getHostName().c_str(),
                m_config.getMq().m_listen.getPort()));
            return -1;
        }
    }
    if (m_config.getMq().m_listen.getUnixDomain().length())
    {
        if (0 > this->noticeLsockListen(SVR_TYPE_FETCH,
            m_config.getMq().m_listen.getUnixDomain().c_str(),
            false,
            0,
            m_config.getMq().m_listen.isKeepAlive()))
        {
            CWX_ERROR(("Can't register the mq-fetch unix-domain accept listen: path-file=%s",
                m_config.getMq().m_listen.getUnixDomain().c_str()));
            return -1;
        }
    }

    return 0;
}

int CwxMqApp::commit_mq(char* szErr2K)
{
    int iRet = 0;
    CWX_INFO(("Begin flush mq sys file......."));
    if (getMqUncommitNum())
    {
        iRet = getSysFile()->commit();
        if ((0 != iRet) && szErr2K) strcpy(szErr2K, getSysFile()->getErrMsg());
        setMqUncommitNum(0);
        setMqLastCommitTime(time(NULL));
    }
    CWX_INFO(("End flush mq sys file......."));
    return iRet;
}

///�ַ��µ�binlog
void CwxMqApp::dispathWaitingBinlog(CwxMqTss* pTss)
{
    m_pAsyncHandler->dispatch(pTss);
    m_pFetchHandler->dispatch(pTss);
}