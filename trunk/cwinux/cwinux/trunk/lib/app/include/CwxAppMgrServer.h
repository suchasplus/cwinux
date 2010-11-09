#ifndef __CWX_APP_MGR_SERVER_H__
#define __CWX_APP_MGR_SERVER_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppMgrServer.h
@brief 处理监控管理命令的CwxAppMgrServer类的定义
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
#include "CwxAppMgrInterface.h"
#include "CwxAppHandler4Msg.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxAppMgrServer
@brief 处理来自管理端口的监控控制命令。用户可以重载此对象，以改变默认的处理规则，并增加服务特殊的控制命令。
msg-type=1：获取服务支持的监控、控制指令。
msg-type=3：停止进程
msg-type=5：重启进程
msg-type=7：设置服务的运行日志级别
msg-type=9：查询服务的运行日志级别
msg-type=11：控制服务切换运行日志文件
msg-type=13：查询服务运行状态是否正常
msg-type=15：查询服务运行的详细信息
msg-type=其他：支持用户扩展自己的特殊命令
*/
class CWX_API CwxAppMgrServer : public CwxAppMgrInterface 
{
public:
    ///构造函数
    CwxAppMgrServer(CwxAppFramework* pApp)
        :CwxAppMgrInterface(pApp)
    {

    }
    ///析构函数
    virtual ~CwxAppMgrServer()
    {

    }
public:
    /**
    @brief help命令的响应函数
    @param [in] msg msg命令数据包
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @return false：不回复，关闭连接； true：回复
    */
    virtual bool onCmdHelp(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        CwxAppMgrReply& reply);
    /**
    @brief 用户自定义的help命令的响应函数
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @param [in] cmdPackage cmd的临时package，用户对一个command打包
    @return false：失败； true：成功
    */
    virtual bool onCmdHelpUser(CwxAppTss* pThrEnv,
        CwxAppMgrReply& reply,
        CwxPackageWriter& cmdPackage);
    /**
    @brief stop命令的响应函数
    @param [in] msg msg命令数据包
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @return false：不回复，关闭连接； true：回复
    */
    virtual bool onCmdStop(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        CwxAppMgrReply& reply);
    /**
    @brief restart命令的响应函数
    @param [in] msg msg命令数据包
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @return false：不回复，关闭连接； true：回复
    */
    virtual bool onCmdRestart(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        CwxAppMgrReply& reply);
    /**
    @brief 设置运行日志Level命令的响应函数
    @param [in] msg msg命令数据包
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @return false：不回复，关闭连接； true：回复
    */
    virtual bool onCmdSetLogLevel(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        CwxAppMgrReply& reply);
    /**
    @brief 获取运行日志Level命令的响应函数
    @param [in] msg msg命令数据包
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @return false：不回复，关闭连接； true：回复
    */
    virtual bool onCmdGetLogLevel(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        CwxAppMgrReply& reply);
    /**
    @brief 切换运行日志文件命令的响应函数
    @param [in] msg msg命令数据包
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @return false：不回复，关闭连接； true：回复
    */
    virtual bool onCmdNextLogfile(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv, 
        CwxAppMgrReply& reply);
    /**
    @brief 获取服务运行状态命令的响应函数
    @param [in] msg msg命令数据包
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @return false：不回复，关闭连接； true：回复
    */
    virtual bool onCmdRunCheck(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv, 
        CwxAppMgrReply& reply);
    /**
    @brief 获取服务详细运行信息命令的响应函数
    @param [in] msg msg命令数据包
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @return false：不回复，关闭连接； true：回复
    */
    virtual bool onCmdRunDetail(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        CwxAppMgrReply& reply);
    /**
    @brief 用户自定义命令的响应函数
    @param [in] msg msg命令数据包
    @param [in] pThrEnv 线程的Thread-env。
    @param [in] reply 回复的package等相关的信息。
    @return false：不回复，关闭连接； true：回复
    */
    virtual bool onCmdUser(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv, 
        CwxAppMgrReply& reply);
public:
    /**
    @brief 收到管理命令的响应函数，此重载了CwxAppCmdOp::noticeRecvMsg()
    @param [in] msg msg命令数据包
    @param [in] conn 收到消息的连接。
    @param [in] header 收到消息的消息头。
    @param [in] pThrEnv 线程的Thread-env。
    @return false：失败； true：成功
    */
    virtual int onRecvMsg(CwxMsgBlock*& msg,
        CwxAppHandler4Msg const& conn,
        CwxMsgHead const& header, 
        CwxAppTss* pThrEnv);
    ///此重载了CwxAppCmdOp::onConnCreated()，管理对象不处理
    virtual int onConnCreated(CwxMsgBlock*& ,
        CwxAppTss* )
    {
        return 1;
    }
    ///此重载了CwxAppCmdOp::onConnClosed()，管理对象不处理
    virtual int onConnClosed(CwxMsgBlock*& , CwxAppTss* )
    {
        return 1;
    }
    ///此重载了CwxAppCmdOp::onEndSendMsg()，管理对象不处理
    virtual int onEndSendMsg(CwxMsgBlock*& , CwxAppTss* )
    {
        return 1;
    }
    ///此重载了CwxAppCmdOp::onFailSendMsg()，管理对象不处理
    virtual int onFailSendMsg(CwxMsgBlock*& , CwxAppTss* )
    {
        return 1;
    }
    ///此重载了CwxAppCmdOp::onTimeoutCheck()，管理对象不处理
    virtual int onTimeoutCheck(CwxMsgBlock*& , CwxAppTss* )
    {
        return 1;
    }
    ///此重载了CwxAppCmdOp::onEvent4Handle()，管理对象不处理
    virtual int onEvent4Handle(CwxMsgBlock*& , CwxAppTss* )
    {
        return 1;
    }
    ///此重载了CwxAppCmdOp::onUserEvent()，管理对象不处理
    virtual int onUserEvent(CwxMsgBlock*& , CwxAppTss* )
    {
        return 1;
    }
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"


#endif 
