#ifndef __CWM_NERVE_CGI_HANDLER_H__
#define __CWM_NERVE_CGI_HANDLER_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmNerveCgiHandler.h
@brief Nerve处理CGI通信的handle。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-26
@warning
@bug
*/
#include "CwxAppCommander.h"
#include "CwxAppFramework.h"
#include "CwxMsgBlock.h"

#include "CwmCmnMacro.h"
#include "CwmNerveTss.h"
#include "CwmNerveServiceStatus.h"
#include "CwmNerveActionStatus.h"
class CwmNerveApp;

/**
@class CwmNerveCgiHandler
@brief  Nerve处理CGI通信的handle。
*/
class CwmNerveCgiHandler : public CwxAppCmdOp
{
    ///fork的handle对象，用于将pipe的io handle及script传递到child process
    class ForkHandler
    {
    public:
        ForkHandler()
        {
            m_filedes[0] = -1;
            m_filedes[1] = -1;
        }
        ~ForkHandler()
        {
            if (-1 != m_filedes[0]) close(m_filedes[0]);
            if (-1 != m_filedes[1]) close(m_filedes[1]);
        }
    public:
        int        m_filedes[2]; ///<pipe的read/write io handle
        string     m_strCgiScript; ///<child's execute script
    };
public:
    ///构造函数
    CwmNerveCgiHandler(CwmNerveApp* pApp):m_pApp(pApp)
    {

    }
    ///析构函数
    ~CwmNerveCgiHandler()
    {

    }
public:
    /**
    @brief 处理CGI连接关闭的事件,此时意味着child process退出。
    @param [in] msg 连接关闭的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 通过pipe，收到CGI输出的数据。
    @param [in] msg 收到通信数据包的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
public:
    ///framework的fork所需要的forkPrepare API
    static void forkPrepare(CwxAppFramework* pParentApp, void* handle);
    ///framework的fork所需要的forkChild API
    static void forkChild(void* handle);
public:
    ///静态函数，根据service与action的名字，执行cgi脚本的执行，此用于nerve的cgi类型的query
    static bool queryCgi(CwmNerveApp* pApp,
        CwxAppTss* pThrEnv,
        string const& strSystem,
        string const& strService,
        string const& strAction,
        string& strErr,
        CwxMsgBlock*& queryMsg);
    ///静态函数，根据action对象，执行cgi脚本的执行，此用于nerve的cgi类型的poll
    static bool queryCgi(CwmNerveApp* pApp, CwxAppTss* pThrEnv, CwmNerveServiceStatus* pService, CwmNerveActionStatus* pAction, string& strErr);
private:
    CwmNerveApp*      m_pApp; ///app对象
};


#endif
