#include "CwxMgrClient.h"

CwxMgrClient::CwxMgrClient()
{
    m_unPort = 0;
    m_pDumpBuf = NULL;
    m_uiDumpBufLen = 0;
    m_szErr2K[0] = 0;
    m_recvMsg = NULL;
    m_sendMsg = NULL;
    m_bValid = false;

}

CwxMgrClient::~CwxMgrClient()
{
    this->clear();
}

int CwxMgrClient::init(char const* szAddr, CWX_UINT16 unPort)
{
    m_strAddr = szAddr;
    m_unPort = unPort;
    if (0 != initEnv()) return -1;
    m_bValid = true;
    printf("Success to connect to addr[%s] port[%u]\n", szAddr, unPort);
    printf("Supporting command list:\n");
    printCmds();
    return 0;
}

void CwxMgrClient::run()
{
    char szInput[1024];
    while(true){
        prompt();
        fgets(szInput, 1023, stdin);
        if (strlen(szInput) == 0) continue;
        szInput[strlen(szInput)-1] = 0x00;
        if (1 != doCommand(szInput)) break;
    }
}


void CwxMgrClient::clear()
{
    if (m_pDumpBuf) delete [] m_pDumpBuf;
    m_pDumpBuf = NULL;
    m_uiDumpBufLen = 0;
    m_szErr2K[0] = 0x00;
    m_conn.close();
    if (m_recvMsg)
    {
        CwxMsgBlockAlloc::free(m_recvMsg);
        m_recvMsg = NULL;
    }
    if (m_sendMsg)
    {
        CwxMsgBlockAlloc::free(m_sendMsg);
        m_sendMsg = NULL;
    }
    m_cmds.clear();
    m_bValid = false;
}

int  CwxMgrClient::initEnv()
{
    int ret = 0;
    this->clear();
    if (0 != connect())
    {
        return -1;
    }
    printf("Success connect to %s:%d\n", m_strAddr.c_str(), m_unPort);
    //get help
    CwxMsgHead header(0, 0, CwxAppMgrInterface::MSG_TYPE_HELP, 0, 0);
    if (m_sendMsg) CwxMsgBlockAlloc::free(m_sendMsg);
    m_sendMsg =  CwxMsgBlockAlloc::pack(header, NULL, 0);
    if (0 != send()) return -1;
    //split help
    int level = 0;
    int status = 0;
    string strErr;
    CwxKeyValueItem record;
    if (0 != recv(status, level, strErr, record))
    {
        return -1;
    }
    if (0 != status)
    {
        printf("Service[%s:%d] is not valid, err-msg:%s\n", m_strAddr.c_str(), m_unPort, m_strAddr.c_str());
        return -1;
    }
    CwxPackageReader reader;
    CwxPackageReader cmdReader;
    CwxKeyValueItem const* pRecord = NULL;

    if (!reader.unpack(record.m_szData, record.m_uiDataLen))
    {
        printf("Failure to unpack help info, error:%s\n", reader.getErrMsg());
        return -1;
    }
    CwxMgrCmd cmd;
    CwxMgrCmdArg arg;
    string strArgs;
    list<string> args;
    list<string>::iterator args_iter;
    list<string> argInfos;
    list<string>::iterator argInfos_iter;
    for (CWX_UINT16 i=0; i<reader.getKeyNum(); i++)
    {
        pRecord = reader.getKey(i);
        if (!cmdReader.unpack(pRecord->m_szData, pRecord->m_uiDataLen))
        {
            printf("Failure to unpack cmd's info, err:%s\n", pRecord->m_szKey, cmdReader.getErrMsg());
            return -1;
        }
        cmd.reset();
        //get cmd name
        if (!cmdReader.getKey(CWX_APP_MGR_CMD_MSG_NAME, cmd.m_strName))
        {
            printf("No cmd name key[%s] in package\n", CWX_APP_MGR_CMD_MSG_NAME);
            return -1;
        }
        //get cmd msg type
        if (!cmdReader.getKey(CWX_APP_MGR_CMD_MSG_TYPE, cmd.m_unMsgType))
        {
            printf("No cmd msg_type key[%s] in package\n", CWX_APP_MGR_CMD_MSG_TYPE);
            return -1;
        }
        //get cmd desc
        if (!cmdReader.getKey(CWX_APP_MGR_CMD_DESC, cmd.m_strDesc))
        {
            printf("No cmd desc key[%s] in package\n", CWX_APP_MGR_CMD_DESC);
            return -1;
        }
        //get cmd arg
        if (!cmdReader.getKey(CWX_APP_MGR_CMD_ARG, strArgs))
        {
            printf("No cmd arg key[%s] in package\n", CWX_APP_MGR_CMD_ARG);
            return -1;
        }
        CwxCommon::split(strArgs, args, '|');
        args_iter =args.begin();
        while(args_iter != args.end())
        {

            if (args_iter->length())
            {
                CwxCommon::split(*args_iter, argInfos, ':');
                if (argInfos.size() != 3)
                {
                    printf("Cmd[%s]'s format is not fit [arg:optional:type], it's value:%s",
                        cmd.m_strName.c_str(),
                        args_iter->c_str());
                    return -1;
                }
                arg.reset();
                argInfos_iter = argInfos.begin();
                arg.m_strArg = *argInfos_iter;
                argInfos_iter++;
                arg.m_bOptional = strcasecmp("n", argInfos_iter->c_str())==0?false:true;
                argInfos_iter++;
                arg.m_bNum = strcasecmp("n", argInfos_iter->c_str())==0?true:false;
                cmd.m_args.push_back(arg);
            }
            m_cmds.push_back(cmd);
            cmd.reset();
            args_iter++;
        }
    }
    return 0;
}

void CwxMgrClient::prompt() const
{
    if (m_bValid)
        printf("\n%s:%u>", m_strAddr.c_str(), m_unPort);
    else
        printf("\n-:->");
}

int CwxMgrClient::connect()
{
    CwxINetAddr addr(m_unPort, m_strAddr.c_str());
    CwxSockConnector connector;
    CwxTimeValue timeout(CONN_TIMEOUT);
    CwxTimeouter timer(&timeout);
    if (CWX_INVALID_HANDLE != m_conn.getHandle())
    {
        m_conn.close();
    }
    if (0 != connector.connect(m_conn, addr, CwxAddr::sap_any, &timer))
    {
        printf("Failure to connect addr[%s], port[%u], errno=%d\n",
            m_strAddr.c_str(),
            m_unPort,
            errno);
        return -1;
    }
    return 0;
}

int CwxMgrClient::recv(int& status, int& level, string& err, CwxKeyValueItem& record)
{
    CwxTimeValue timeout(RECV_TIMEOUT);
    CwxTimeouter timer;
    if (m_recvMsg) CwxMsgBlockAlloc::free(m_recvMsg);
    m_recvMsg = NULL;
    int ret = m_conn.read(m_recvHeader, m_recvMsg, &timer);
    if (-1 == ret)
    {
        printf("Failure to read data from %s:%d, errno=%d\n", m_strAddr.c_str(), m_unPort, errno);
        return -1;
    }
    else if (0 == ret)
    {
        printf("conn[%s:%d] closed.\n", m_strAddr.c_str(), m_unPort);
        return -1;
    }
    CwxPackageReader reader;
    if (!reader.unpack(m_recvMsg->rd_ptr(), m_recvMsg->length(), false))
    {
        printf("Failure to unpack the help package, err=%s\n", reader.getErrMsg());
        return -1;
    }
    //get CWX_APP_MGR_STATUS
    if (!reader.getKey(CWX_APP_MGR_STATUS, status))
    {
        printf("No key[%s] in recv package\n", CWX_APP_MGR_STATUS);
        return -1;
    }
    if (0 != status)
    {
        //get err msg
        if (!reader.getKey(CWX_APP_MGR_ERROR, err))
        {
            printf("No key[%s] in recv package\n", CWX_APP_MGR_ERROR);
            return -1;
        }
    }
    else
    {
        err = "";
    }
    //get level
    if (!reader.getKey(CWX_APP_MGR_LEVEL, level))
    {
        printf("No key[%s] in recv package\n", CWX_APP_MGR_LEVEL);
        return -1;
    }
    //get record
    CwxKeyValueItem const* pRecord = NULL;
    if (!(pRecord = reader.getKey(CWX_APP_MGR_RECORD)))
    {
        printf("No key[%s] in recv package\n", CWX_APP_MGR_RECORD);
        return -1;
    }
    record = *pRecord;
    return 0;
}

int CwxMgrClient::send()
{
    CwxTimeValue timeout(SEND_TIMEOUT);
    CwxTimeouter timer;
    int ret = m_conn.send_n(m_sendMsg->rd_ptr(),
        m_sendMsg->length(),
        &timer);
    if (ret != m_sendMsg->length())
    {
        printf("Failure to send msg to %s:%d, errno=%d\n", m_strAddr.c_str(), m_unPort, errno);
        return -1;
    }
    return 0;
}

void CwxMgrClient::printCmds()
{
    string args;
    list<CwxMgrCmd>::iterator iter;
    list<CwxMgrCmdArg>::iterator  arg_iter;
    printf("help:\t get the command description.\n");
    printf("connect  ip  port:\tconnect to the mgr port.\n");
    printf("quit:\texit.\n");
    if (m_bValid)
    {
        iter = m_cmds.begin();
        while(iter != m_cmds.end())
        {
            args.erase();
            arg_iter = iter->m_args.begin();
            while(arg_iter != iter->m_args.end())
            {
                if (args.length())
                {
                    args += "  ";
                    args += arg_iter->m_strArg;
                }
                else
                {
                    args = arg_iter->m_strArg;
                }
                arg_iter ++;
            }
            if (args.length())
                printf("%s %s:\t%s\n", iter->m_strName.c_str(), args.c_str(), iter->m_strDesc.c_str());
            else
                printf("%s:\t%s\n", iter->m_strName.c_str(), iter->m_strDesc.c_str());
            iter++;
        }
    }
}


//0£ºexit£¬1£ºcontinue
int CwxMgrClient::doCommand(char* szCmd)
{
    string strCmd;
    list<string> value;
    CwxCommon::trim(szCmd);
    strCmd = szCmd;
    CwxCommon::split(strCmd, value, ' ');
    list<string>::iterator iter = value.begin();
    //remove the empty value
    while(iter != value.end()){
        if ((*iter).length() == 0){
            value.erase(iter);
            iter = value.begin();
            continue;
        }
        iter++;
    }
    if (value.size() == 0) return 1;

    iter = value.begin();
    CWX_UINT32 uiItemNum = value.size();
    if(0 == strcasecmp((*iter).c_str(), "help")){
        doHelp();
    }
    else if(0 == strcasecmp((*iter).c_str(), "quit")){
        return 0;
    }
    else if (0 == strcasecmp((*iter).c_str(), "connect"))
    {
        if (3 != value.size())
        {
            printf("Invalid connect arg, using: connect ip  port\n");
            return 1;
        }
        iter++;
        m_strAddr = *iter;
        iter++;
        m_unPort = strtoul(iter->c_str(), NULL, 10);
        initEnv();
        m_bValid = true;
    }
    else if (m_bValid)
    {
        doMgrCmd(value);
    }
    else
    {
        printf("Unknow command.\n");
    }
    return 1;    
}

void CwxMgrClient::doHelp()
{
    printCmds();
    prompt();
}

void CwxMgrClient::doMgrCmd(list<string> const& args)
{
    list<string>::const_iterator iter=args.begin();
    list<CwxMgrCmdArg>::iterator arg_iter;
    list<CwxMgrCmd>::iterator cmd_iter = m_cmds.begin();
    int status = 0;
    int level = 0;
    string err;
    CwxKeyValueItem record;
    CWX_UINT32 uiBufLen;
    while(cmd_iter != m_cmds.end())
    {
        if ((strcasecmp(cmd_iter->m_strName.c_str(), iter->c_str()) == 0)&&
            (strcasecmp(cmd_iter->m_strName.c_str(), "help") != 0))
        {
            m_writer.beginPack();
            m_writer.addKeyValue(CWX_APP_MGR_CMD, iter->c_str(), iter->length());
            iter++;
            arg_iter = cmd_iter->m_args.begin();
            while((arg_iter != cmd_iter->m_args.end()) &&
                (iter != args.end()))
            {
                m_writer.addKeyValue(arg_iter->m_strArg.c_str(), iter->c_str(), iter->length());
                arg_iter++;
                iter++;
            }
            m_writer.pack();
            CwxMsgHead head(0, 0, cmd_iter->m_unMsgType, 0, m_writer.getMsgSize());
            m_sendMsg = CwxMsgBlockAlloc::pack(head, m_writer.getMsg(), m_writer.getMsgSize());
            if (0 != send())
            {
                m_bValid = false;
                m_conn.close();
                return ;
            }
            if (0 != recv(status, level, err, record))
            {
                m_bValid = false;
                m_conn.close();
                return;
            }
            if (!prepareDumpBuf(record.m_uiDataLen * 4))
            {
                printf("Failure to malloc buf.");
                return;
            }
            uiBufLen = m_uiDumpBufLen - 1;
            CwxPackage::dump(record.m_szData,
                record.m_uiDataLen,
                m_pDumpBuf,
                uiBufLen);
            m_pDumpBuf[uiBufLen] = 0x00;
            printf("output:\n");
            printf("status:%d\n", status);
            printf("level:%d\n", level);
            if (0 != status) printf("error:%s\n", err.c_str());
            printf("detail:\n");
            printf("%s\n", m_pDumpBuf);
            return;
        }
        cmd_iter++;
    }
    printf("Invalid command:%s\n", iter->c_str());
}

bool CwxMgrClient::prepareDumpBuf(CWX_UINT32 uiLen){
    if (uiLen + 1 > m_uiDumpBufLen){
        if (m_pDumpBuf) delete [] m_pDumpBuf;
        m_uiDumpBufLen = uiLen + 2048;
        m_pDumpBuf = new char[m_uiDumpBufLen];
        if (!m_pDumpBuf){
            printf("Failure to malloc memory, size:%u\n", m_uiDumpBufLen);
            m_uiDumpBufLen = 0;
            return false;
        }
    }
    return true;
}


