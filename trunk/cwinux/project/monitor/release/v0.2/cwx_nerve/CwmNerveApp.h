#ifndef __CWM_NERVE_APP_H__
#define __CWM_NERVE_APP_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmNerveApp.h
@brief 监控系统中的Nerve服务的App定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-25
@warning
@bug
*/

#include "CwxAppFramework.h"
#include "CwxAppHandler4Msg.h"
#include "CwmCmnMacro.h"
#include "CwmNerveConfig.h"
#include "CwmNerveAlarmHandler.h"
#include "CwmNerveCgiHandler.h"
#include "CwmNerveQueryHandler.h"
#include "CwmNerveServerHandler.h"
#include "CwmNerveServiceStatus.h"
#include "CwmCmnDataMgr.h"

/**
@class CwmNerveApp
@brief Nerve的App类
*/
class CwmNerveApp : public CwxAppFramework
{
public:
    enum
    {
        LOG_FILE_SIZE = 30, ///<每个循环运行日志文件的MBTYE
        LOG_FILE_NUM = 7,///<循环日志文件的数量
        SVR_TYPE_QUERY = CwxAppFramework::SVR_TYPE_USER_START, ///<Monitor查询的svr-id类型
        SVR_TYPE_ALARM = CwxAppFramework::SVR_TYPE_USER_START + 1, ///<告警报告的svr-id
        SVR_TYPE_SERVER_ACTION = CwxAppFramework::SVR_TYPE_USER_START + 2, ///<server类型action的svr-id
        SVR_TYPE_CGI_ACTION = CwxAppFramework::SVR_TYPE_USER_START + 3 ///<CGI类型action的svr-id
    };
    ///构造函数
	CwmNerveApp();
    ///析构函数
	virtual ~CwmNerveApp();
    //初始化app, -1:failure, 0 success;
    virtual int init(int argc, char** argv);
public:
    ///创建通信线程的tss
    virtual CwxAppTss* onTssEnv();
    ///时钟响应函数
    virtual void onTime(CwxTimeValue const& current);
    ///信号响应函数
    virtual void onSignal(int signum);
    ///连接建立函数
    virtual int onConnCreated(CwxAppHandler4Msg& conn, bool& bSuspendConn, bool& bSuspendListen);
    ///连接关闭
    virtual int onConnClosed(CwxAppHandler4Msg const& conn);
    ///收到数据消息
    virtual int onRecvMsg(CwxMsgBlock* msg, CwxAppHandler4Msg const& conn, CwxMsgHead const& header, bool& bSuspendConn);
    ///消息发送完毕
    virtual CWX_UINT32 onEndSendMsg(CwxMsgBlock*& msg, CwxAppHandler4Msg const& conn);
    ///发送数据失败
    virtual void onFailSendMsg(CwxMsgBlock*& msg);
public:
    ///获取下一个TASK ID，由于是单线程，因此不需要锁保护
    CWX_UINT32 nextTaskId()
    {
        m_uiTaskId++;
        if (!m_uiTaskId) m_uiTaskId = 1;
        return m_uiTaskId;
    }
    ///获取处理cgi的消息响应的handler
    CwmNerveCgiHandler*   getCgiHandler()
    {
        return m_pCgiHandler;
    }
    ///获取处理server通信的消息响应的handler
    CwmNerveServerHandler*  getServerHandler()
    {
        return m_pServerHandler;
    }
    ///获取处理告警报告的消息响应的handler
    CwmNerveAlarmHandler* getAlarmHandler()
    {
        return m_pAlarmHandler;
    }
    ///获取处理数据查询的消息响应的handler
    CwmNerveQueryHandler* getQueryHandler()
    {
        return m_pQueryHandler;
    }
    ///获取告警、性能数据管理的manager
    CwmCmnDataMgr* getDataMgr()
    {
        return   m_pDataMgr;
    }
    ///获取nerve的config对象
    CwmNerveConfig const& getConfig() const
    {
        return m_config;
    }
    ///获取指定system与service名字对应的service对象，若不存在，则返回NULL。
    CwmNerveServiceStatus* getService(string const& strSystem, string const& strName)
    {
        list<CwmNerveServiceStatus*>::iterator iter = m_lsService.begin();
        while(iter != m_lsService.end())
        {
            if (((*iter)->getServiceInfo()->getName() == strName) &&
                ((*iter)->getServiceInfo()->getSystem() == strSystem))
            {
                return *iter;
            }
            iter++;
        }
        return NULL;
    }
    ///获取全部的service的列表。
    list<CwmNerveServiceStatus*>& getServices()
    {
        return m_lsService;
    }
protected:
    ///init the Enviroment before run.0:success, -1:failure.
	virtual int initRunEnv();
    ///释放nerve app中的对象
    virtual void destroy();
private:
    ///实现消息的分发
    void dispatch(CwxMsgBlock* msg);
private:
    CwmNerveConfig               m_config; ///<配置文件对象
    CWX_UINT32                  m_uiAlarmConnId; ///<告警的连接ID
    CWX_UINT32                   m_uiTaskId; ///<task id的计数器
    list<CwmNerveServiceStatus*>    m_lsService; ///<Service的列表
    CwmNerveAlarmHandler*         m_pAlarmHandler; ///<处理alarm报告的通信管理handler
    CwmNerveCgiHandler*           m_pCgiHandler;///<处理cgi管道通信的handler
    CwmNerveServerHandler*        m_pServerHandler;///<处理与server通信的handler
    CwmNerveQueryHandler*         m_pQueryHandler;///<处理查询消息的handler
    CwmCmnDataMgr*               m_pDataMgr;///<数据管理器
};

#endif

