#ifndef __CWX_MGR_CLIENT_H__
#define __CWX_MGR_CLIENT_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwxMgrClient.h
@brief 服务管理端口处理对象
@author cwinux@gmail.com
@version 0.1
@date 2010-08-29
@warning
@bug
*/
#include "CwxPackageReader.h"
#include "CwxXmlPackageConv.h"
#include "CwxINetAddr.h"
#include "CwxSockStream.h"
#include "CwxSockConnector.h"
#include "CwxAppMgrInterface.h"
#include "CwxMsgHead.h"



CWINUX_USING_NAMESPACE
class CwxMgrCmdArg
{
public:
    CwxMgrCmdArg()
    {
        m_bOptional = false;
        m_bNum = false;
    }
    CwxMgrCmdArg(CwxMgrCmdArg const& item)
    {
        m_strArg = item.m_strArg;
        m_bNum = item.m_bNum;
        m_bOptional = item.m_bOptional;
    }
    CwxMgrCmdArg& operator=(CwxMgrCmdArg const& item)
    {
        if (this != &item)
        {
            m_strArg = item.m_strArg;
            m_bNum = item.m_bNum;
            m_bOptional = item.m_bOptional;
        }
        return *this;
    }
    void reset()
    {
        m_strArg.erase();
        m_bNum = false;
        m_bOptional = false;
    }
public:
    string    m_strArg;
    bool      m_bOptional;
    bool      m_bNum;
};
class CwxMgrCmd
{
public:
    CwxMgrCmd()
    {
        m_unMsgType = 0;
    }
    CwxMgrCmd(CwxMgrCmd const& item)
    {
        m_strName = item.m_strName;
        m_unMsgType = item.m_unMsgType;
        m_args = item.m_args;
        m_strDesc = item.m_strDesc;
    }
    CwxMgrCmd& operator=(CwxMgrCmd const& item)
    {
        if (this != &item)
        {
            m_strName = item.m_strName;
            m_unMsgType = item.m_unMsgType;
            m_args = item.m_args;
            m_strDesc = item.m_strDesc;
        }
        return *this;
    }
    void reset()
    {
        m_strName.erase();
        m_unMsgType = 0;
        m_args.clear();
        m_strDesc.erase();
    }
public:
    string      m_strName;
    CWX_UINT16  m_unMsgType;
    list<CwxMgrCmdArg> m_args;
    string      m_strDesc;
};

class CwxMgrClient{
public:
    enum
    {
        CONN_TIMEOUT = 10, ///<连接超时为10s
        SEND_TIMEOUT = 10, ///<发送命令的超时为10s
        RECV_TIMEOUT = 10  ///<接收命令的超时为10s
    };
public:
    CwxMgrClient();
    ~CwxMgrClient();
    int init(char const* szAddr, CWX_UINT16 unPort);
    void run();
private:
    void clear();
    int  initEnv();
    void prompt() const;
    int  connect();
    int  recv(int& status, int& level, string& err, CwxKeyValueItem& record);
    int  send();
    void printCmds();
    //0：exit，1：continue
    int doCommand(char* szCmd);
    void doHelp();
    void doMgrCmd(list<string> const& args);

    bool prepareDumpBuf(CWX_UINT32 uiLen);

private:
    string              m_strAddr; ///<连接的地址
    CWX_UINT16          m_unPort; ///<连接的Port
    CwxSockStream        m_conn; ///<连接
    char*               m_pDumpBuf; ///<data dump buf
    CWX_UINT32           m_uiDumpBufLen; ///<data dump buf的空间大小
    char                m_szErr2K[2048]; ///<错误时的错误消息
    CwxMsgBlock*        m_recvMsg; ///<接受到的消息包
    CwxMsgBlock*        m_sendMsg; ///<发送的消息包
    CwxMsgHead          m_recvHeader; ///<收到的消息包头
    CwxPackageWriter    m_writer;
    list<CwxMgrCmd>     m_cmds;
    bool                m_bValid; ///<是否成功的获取了各种命令信息
};

#endif
