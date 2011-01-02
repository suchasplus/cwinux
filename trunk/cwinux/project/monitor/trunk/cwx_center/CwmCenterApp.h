#ifndef __CWM_CENTER_APP_H__
#define __CWM_CENTER_APP_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmCenterApp.h
@brief 监控系统中的Center服务的App定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-02
@warning
@bug
*/

#include "CwxAppFramework.h"
#include "CwxAppHandler4Msg.h"
#include "CwxAppThreadPoolEx.h"
#include "CwxHtmlTmpl.h"
#include "CwmCmnMacro.h"
#include "CwmCenterConfig.h"
#include "CwmCenterAlarmAcceptor.h"
#include "CwmCenterAlarmReportor.h"
#include "CwmCenterNerveQuery.h"
#include "CwmCenterUiQuery.h"
#include "CwmCmnDataMgr.h"
#include "CwmCenterNerveStatus.h"

/**
@class CwmCenterApp
@brief Center的App类
*/
class CwmCenterApp : public CwxAppFramework
{
public:
    enum
    {
        LOG_FILE_SIZE = 30, ///<每个循环运行日志文件的MBTYE
        LOG_FILE_NUM = 7,///<循环日志文件的数量
        SVR_TYPE_UI_QUERY = CwxAppFramework::SVR_TYPE_USER_START, ///<Monitor查询的svr-id类型
        SVR_TYPE_ALARM_ACCEPT = CwxAppFramework::SVR_TYPE_USER_START + 1, ///<告警接收的svr-id
        SVR_TYPE_ALARM_REPORT = CwxAppFramework::SVR_TYPE_USER_START + 2, ///<告警报告的svr-id
        SVR_TYPE_NERVE_QUERY = CwxAppFramework::SVR_TYPE_USER_START + 3 ///<Nerve查询的svr-id
    };
    ///构造函数
	CwmCenterApp();
    ///析构函数
	virtual ~CwmCenterApp();
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
    ///获取下一个task id，用于查询与alarm report
    CWX_UINT32 nextTaskId()
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        m_uiTaskId++;
        if (!m_uiTaskId) m_uiTaskId = 1;
        return m_uiTaskId;
    }
    ///获取数据管理器
    CwmCmnDataMgr* getDataMgr()
    {
        return m_pDataMgr;
    }
    ///获取配置信息
    CwmCenterConfig const& getConfig() const
    {
        return  m_config;
    }
    ///获取告警接收handler
    CwmCenterAlarmAcceptor* getAlarmAccptor()
    {
        return m_pAlarmAcceptor;
    }
    ///获取告警report的handler
    CwmCenterAlarmReportor* getAlarmReportor()
    {
        return m_pAlarmReportor;
    }
    ///获取查询nerve的handler
    CwmCenterNerveQuery*  getNerveQuery()
    {
        return m_pNerveQuery;
    }
    ///获取处理UI查询的handler
    CwmCenterUiQuery*  getUiQuery()
    {
        return m_pUiQuery;
    }
    ///获取center所管理的所有nerve的map
    map<CWX_UINT32, CwmCenterNerveStatus*>&  getNerves()
    {
        return m_nerves;
    }
    ///获取指定IP的nerve对象
    CwmCenterNerveStatus* getNerve(string const& strIp)
    {
        map<string, CwmCenterNerveStatus*>::iterator iter = m_nervesIndex.find(strIp);
        if (iter != m_nervesIndex.end()) return iter->second;
        return NULL;
    }
protected:
    //init the Enviroment before run.0:success, -1:failure.
	virtual int initRunEnv();
    ///释放center app的资源
    virtual void destroy();
private:
   CWX_UINT32      m_uiTaskId; ///<taskid
   CwxMutexLock    m_lock; ///<锁
   CwmCenterConfig  m_config; ///<配置信息
   CwmCmnDataMgr*   m_pDataMgr; ///<数据管理器
   CwmCenterAlarmAcceptor* m_pAlarmAcceptor; ///<告警接收的handler
   CwmCenterAlarmReportor* m_pAlarmReportor; ///<告警报告的handler
   CwmCenterNerveQuery*    m_pNerveQuery; ///<nerve查询的handler
   CwmCenterUiQuery*       m_pUiQuery; ///<ui 查询的handler
   map<CWX_UINT32, CwmCenterNerveStatus*> m_nerves; ///<nerve状态map，key为nerve的连接ID
   map<string, CwmCenterNerveStatus*> m_nervesIndex; ///<以nerve的ip为KEY，建立的nerve对象的索引
   CwxAppThreadPoolEx*      m_pThreadPool; ///<可以按线程进行任务调度的线程池对象
};

#endif

