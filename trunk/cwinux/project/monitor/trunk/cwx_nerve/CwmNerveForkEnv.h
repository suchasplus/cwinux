#ifndef __CWM_NERVE_FORK_ENV_H__
#define __CWM_NERVE_FORK_ENV_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwmNerveForkEnv.h
@brief cgi fork 对象的定义
@author cwinux@gmail.com
@version 0.1
@date 2009-11-25
@warning
@bug
*/

#include "CwxAppForkMgr.h"
#include "CwmCmnMacro.h"
#include "CwmNerveCgiTask.h"

class CwmNerveApp;
/**
@class CwmNerveForkEnv
@brief Cgi fork的异步事件，执行fork操作。
*/
class CWX_API CwmNerveForkEnv : public CwxAppForkEnv
{
public:
    ///构造函数
    CwmNerveForkEnv(CwmNerveApp* pApp);
    ///析构函数
    virtual ~CwmNerveForkEnv();
public:
    /**
    @brief 执行CGI的fork。
    @return   对于父进程，成功则返回子进程的pid，失败的时候返回-1，失败时子进程没有创建。
    对于child进程，则返回0。
    */
    virtual int onFork();
    /**
    @brief child进程的主体，完成child的工作。
    @return void
    */
    virtual void onChildMain();
public:
    ///释放对象的资源
    void reset();
public:
    int        m_cgiPipe[2]; ///<主进程与child进程的通信管道
    string     m_strCgiScript; ///<child进程的cgi脚本名字
    CwmNerveCgiTask*   m_pCgiTask; ///<cgi脚本对应的主进程的task对象
private:
    CwmNerveApp*    m_pApp;
};


#endif
