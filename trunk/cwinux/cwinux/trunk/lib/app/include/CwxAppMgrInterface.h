#ifndef __CWX_APP_MGR_INTERFACE_H__
#define __CWX_APP_MGR_INTERFACE_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppMgrInterface.h
@brief 服务管理、监控的接口定义
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxPackageReader.h"
#include "CwxMsgBlock.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxAppCommander.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;
#define CWX_APP_MGR_CMD_NAME_HELP "help"
#define CWX_APP_MGR_CMD_NAME_SET_LEVEL "set_level"
#define CWX_APP_MGR_CMD_NAME_GET_LEVEL "get_level"
#define CWX_APP_MGR_CMD_NAME_FILE_NO "log_file"
#define CWX_APP_MGR_CMD_NAME_CHECK "check"
#define CWX_APP_MGR_CMD_NAME_DETAIL "detail"

///在命令查询结果数据包中，每条系统支持的命令的数据的KEY
#define CWX_APP_MGR_CMD  "cmd"
///在命令查询结果数据包中，命令的消息类型的KEY
#define CWX_APP_MGR_CMD_MSG_TYPE  "type"
///在命令查询结果数据包中，命令的名字的KEY
#define CWX_APP_MGR_CMD_MSG_NAME  "name"
///在命令查询结果数据包中，命令参数定义的KEY
#define CWX_APP_MGR_CMD_ARG   "arg"
///在命令查询结果数据包中，命令描述定义的KEY
#define CWX_APP_MGR_CMD_DESC  "desc"
///设置或查询进程日志级别的key
#define CWX_APP_MGR_LOG_LEVEL "level"
///设置进程日志文件序号的key
#define CWX_APP_MGR_LOG_FILE_NO "no"
///服务版本号的KEY
#define CWX_APP_MGR_APP_VERSION "version"
///服务版本最新修改时间的KEY
#define CWX_APP_MGR_APP_MODIFY_TIME "modify_time"
///服务版本最新编译时间的KEY
#define CWX_APP_MGR_COMPILE_TIME "compile_time"
///线程池组
#define CWX_APP_MGR_THREAD_POOLS "thread_pools"
///线程池
#define CWX_APP_MGR_THREAD_POOL "thread_pool"
///服务线程信息的KEY
#define CWX_APP_MGR_THREAD "thread"
///level的缺省值
#define CWX_APP_MGR_DEFAULT "default"
///status的宏定义
#define CWX_APP_MGR_STATUS  "status"
///level的宏定义
#define CWX_APP_MGR_LEVEL   "level"
///error的宏定义
#define CWX_APP_MGR_ERROR   "error"
///record的宏定义
#define CWX_APP_MGR_RECORD  "record"

/**
@class CwxAppMgrInterface
@brief 服务监控、控制的通信接口定义，其具有如下的功能：
1、获取服务支持的监控、控制指令。
2、停止进程
3、重启进程
4、设置服务的运行日志级别
5、查询服务的运行日志级别
6、控制服务切换运行日志文件
7、查询服务运行状态是否正常
8、查询服务运行的详细信息
9、支持用户扩展自己的特殊命令
*/
class CWX_API CwxAppMgrInterface : public CwxAppCmdOp 
{
public:
    class CwxAppMgrReply
    {
    public:
        CwxAppMgrReply()
        {
           m_status = 0;
           m_strLevel = CWX_APP_MGR_DEFAULT;
           m_strError = "";
           m_pRecord = NULL;
        }
        ~CwxAppMgrReply()
        {
            if (m_pRecord) delete m_pRecord;
            m_pRecord = NULL;
        }
    public:
        void reset()
        {
            m_status = 0;
            m_strLevel = CWX_APP_MGR_DEFAULT;
            m_strError = "";
            if (m_pRecord) delete m_pRecord;
            m_pRecord = NULL;
        }
    public:
        CWX_INT32        m_status; ///<返回的状态，0：success；-1：with error
        string           m_strLevel; ///<错误时的alarm的level
        string           m_strError; ///<错误时的错误信息
        CwxPackageWriter* m_pRecord;  ///<查询内容
    };
    ///监控、管理命令的消息类型，回复的消息类型为对应的消息类型 + 1
    enum{
        MSG_TYPE_HELP = 1,///<服务帮助命令的消息类型
        MSG_TYPE_STOP = 3,///<停止服务命令的消息类型
        MSG_TYPE_RESTART = 5,///<重新启动服务命令的消息类型
        MSG_TYPE_SET_LOG_LEVEL = 7,///<设置服务运行日志级别的消息类型
        MSG_TYPE_GET_LOG_LEVEL = 9,///<获取服务运行日志级别的消息类型
        MSG_TYPE_NEXT_LOG_FILE = 11,///<通知服务切换运行日志文件的消息类型
        MSG_TYPE_RUN_CHECK = 13,///<检查服务运行状态的消息类型
        MSG_TYPE_RUN_DETAIL = 15,///<查询服务运行详细状况的消息类型
        MSG_TYPE_USER_BEGIN = 101,///<用户指令的开始消息类型
        MSG_TYPE_UNKNOWN = 65533///<不支持命令类型的回复的消息类型
    };
public:
    ///构造函数，并指定对象的APP对象类
    CwxAppMgrInterface(CwxAppFramework* pApp)
        :m_pApp(pApp)
    {

    }
    ///析构函数
    virtual ~CwxAppMgrInterface()
    {

    }
public:
    /**
    @brief Pack一个命令的帮助信息
    @param [in] reply reply的对象，保存接口的成员信息。
    @param [in] cmdPackage 形成命令package的package-writer
    @param [in] unMsgType 需要pack的命令的消息类型
    @param [in] szCmdName 需要pack的命令的名字
    @param [in] args 需要pack的命令的参数，格式为 arg1:是否可选(y/n):type(n/c)|arg2...。n表示数字，c表示为字符
    @param [in] desc 需要pack的命令的描述信息
    @return false：失败； true：成功
    */
    bool packHelpCmd(CwxAppMgrReply& reply,
        CwxPackageWriter& cmdPackage,
        CWX_UINT16 unMsgType, 
        char const* szCmdName,
        char const* args,
        char const* desc);
    /**
    @brief 回复一个的管理命令
    @param [in] uiSvrId 回复的连接svr-id
    @param [in] uiHostId 回复的连接host-id
    @param [in] uiConnId 回复的连接conn-id
    @param [in] header 回复的消息包头
    @param [in,out] reply 回复的package等相关的信息。
    @return void
    */
    void replyCmd(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId, 
        CWX_UINT32 uiConnId, 
        CwxMsgHead const & header, 
        CwxAppMgrReply& reply);
public:
    /**
    @brief 发送一个的help命令，以获取对端服务支持的操作命令
    @param [in] uiSvrId 消息发送的连接svr-id
    @param [in] uiHostId 消息发送的连接host-id
    @param [in] uiConnId 消息发送的连接conn-id
    @param [in] uiTaskId 消息发送的Task-id
    @return false：失败； true：成功
    */
    bool sendCmdHelp(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiTaskId);
    /**
    @brief 发送一个的stop命令，以停止对端服务
    @param [in] uiSvrId 消息发送的连接svr-id
    @param [in] uiHostId 消息发送的连接host-id
    @param [in] uiConnId 消息发送的连接conn-id
    @param [in] uiTaskId 消息发送的Task-id
    @return false：失败； true：成功
    */
    bool sendCmdStop(CWX_UINT32 uiSvrId, 
        CWX_UINT32 uiHostId, 
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiTaskId);
    /**
    @brief 发送一个的restart命令，以重启对端服务
    @param [in] uiSvrId 消息发送的连接svr-id
    @param [in] uiHostId 消息发送的连接host-id
    @param [in] uiConnId 消息发送的连接conn-id
    @param [in] uiTaskId 消息发送的Task-id
    @return false：失败； true：成功
    */
    bool sendCmdRestart(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiTaskId);
    /**
    @brief 发送一个的设置对端服务Log 级别的命令
    @param [in] uiSvrId 消息发送的连接svr-id
    @param [in] uiHostId 消息发送的连接host-id
    @param [in] uiConnId 消息发送的连接conn-id
    @param [in] uiTaskId 消息发送的Task-id
    @param [in] uiLevel 设置的Log 级别
    @return false：失败； true：成功
    */
    bool sendCmdSetLogLevel(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiTaskId,
        CWX_UINT32 uiLevel);
    /**
    @brief 发送一个的获取对端服务Log 级别的命令
    @param [in] uiSvrId 消息发送的连接svr-id
    @param [in] uiHostId 消息发送的连接host-id
    @param [in] uiConnId 消息发送的连接conn-id
    @param [in] uiTaskId 消息发送的Task-id
    @return false：失败； true：成功
    */
    bool sendCmdQueryLogLevel(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiTaskId);
    /**
    @brief 发送一个的切换对端服务日志文件的命令
    @param [in] uiSvrId 消息发送的连接svr-id
    @param [in] uiHostId 消息发送的连接host-id
    @param [in] uiConnId 消息发送的连接conn-id
    @param [in] uiTaskId 消息发送的Task-id
    @param [in] unFileNo 日志文件的序号
    @return false：失败； true：成功
    */
    bool sendCmdNextLogFile(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId, 
        CWX_UINT32 uiConnId, 
        CWX_UINT32 uiTaskId,
        CWX_UINT16 unFileNo);
    /**
    @brief 发送一个的查询对端服务运行状况的命令
    @param [in] uiSvrId 消息发送的连接svr-id
    @param [in] uiHostId 消息发送的连接host-id
    @param [in] uiConnId 消息发送的连接conn-id
    @param [in] uiTaskId 消息发送的Task-id
    @return false：失败； true：成功
    */
    bool sendCmdRunCheck(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId, 
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiTaskId);
    /**
    @brief 发送一个的查询对端服务运行详细信息的命令
    @param [in] uiSvrId 消息发送的连接svr-id
    @param [in] uiHostId 消息发送的连接host-id
    @param [in] uiConnId 消息发送的连接conn-id
    @param [in] uiTaskId 消息发送的Task-id
    @return false：失败； true：成功
    */
    bool sendCmdRunDetail(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId,
        CWX_UINT32 uiTaskId);

public:
    ///获取app对象
    CwxAppFramework* getApp()
    {
        return m_pApp;
    }
private:
    CwxAppFramework*     m_pApp;  ///<app对象
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif 
