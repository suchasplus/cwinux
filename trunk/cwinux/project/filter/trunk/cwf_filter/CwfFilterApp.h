#ifndef __CWF_FILTER_APP_H__
#define __CWF_FILTER_APP_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
#include "CwxAppFramework.h"
#include "CwfFilterConfig.h"
#include "CwfFilterHandler.h"
#include "CwfFilterMgr.h"
#include "CwfFilterTss.h"

#define FILTER_APP_VERSION "1.0"
#define FILTER_MODIFY_DATE "2010-09-08 22:10:66 "

///多线程的echo服务，支持TCP与UNIX-DOMAIN两种协议
class CwfFilterApp : public CwxAppFramework{
public:
    enum{
        LOG_FILE_SIZE = 30, ///<每个可循环使用日志文件的MByte
        LOG_FILE_NUM = 7, ///<可循环使用日志文件的数量
        SVR_TYPE_FILTER = CwxAppFramework::SVR_TYPE_USER_START ///<echo服务的服务类型，及SVR-ID的数值
    };
    ///构造函数
	CwfFilterApp();
    ///析构函数
	virtual ~CwfFilterApp();
    ///重载初始化函数
    virtual int init(int argc, char** argv);
public:
    ///时钟响应函数
    virtual void onTime(CwxTimeValue const& current);
    ///signal响应函数
    virtual void onSignal(int signum);
    ///收到echo消息的响应函数
    virtual int onRecvMsg(CwxMsgBlock* msg,///<收到的echo请求数据包
                        CwxAppHandler4Msg const& conn,///<收到echo请求的连接对象
                        CwxMsgHead const& header, ///<收到echo请求的消息头
                        bool& bSuspendConn///<true：停止此连接继续接受稍息，false：此连接可以继续接受消息
                        );
public:
    CwfFilterMgr* getFilterMgr() { return m_pFilterMgr;}
protected:
    ///重载运行环境设置API
	virtual int initRunEnv();
    virtual void destroy();
private:
    CwfFilterHandler*            m_pFilterHandler;///<echo请求处理的commander handle
    CwxAppThreadPool*            m_pThreadPool;///<线程池对象
    CwfFilterMgr*                m_pFilterMgr; ///<过滤器管理器对象
    CwfFilterConfig              m_config;///<配置文件对象
};
#endif

