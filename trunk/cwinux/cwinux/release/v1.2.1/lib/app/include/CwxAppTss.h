#ifndef __CWX_APP_TSS_H__
#define __CWX_APP_TSS_H__

/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/


/**
@file CwxAppTss.h
@brief 线程TSS定义
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxPackageWriter.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include <pthread.h>


CWINUX_BEGIN_NAMESPACE
/**
@class CwxAppTssInfo
@brief 线程的基本信息，若需要更多的信息，则需要重载这个对象，此对象只应包含基本数据成员。
*/
class CWX_API CwxAppTssInfo
{
public:
    ///构造函数
    CwxAppTssInfo()
    {
        m_unThreadGroup = 0;
        m_unThreadNo = 0;
        m_threadId = (pthread_t)0;
        m_bStopped = false;
        m_bBlocked = false;
        m_uiQueuedMsg = 0;
        m_ttStartTime = 0;
        m_ttUpdateTime = 0;
        m_llRecvMsgNum = 0;
    }
public:
    ///设置线程的group id
    inline void setThreadGroup(CWX_UINT16 unGroup)
    {
        m_unThreadGroup = unGroup;
    }
    ///获取线程的group id
    inline CWX_UINT16 getThreadGroup() const 
    {
        return m_unThreadGroup;
    }
    ///设置线程的thread id
    inline void setThreadNo(CWX_UINT16 unThreadNo)
    {
        m_unThreadNo = unThreadNo;
    }
    ///获取线程的thread id
    inline CWX_UINT16 getThreadNo() const 
    {
        return m_unThreadNo;
    }
    ///设置线程的OS thread id
    inline void setThreadId(pthread_t const& id)
    {
        m_threadId = id;
    }
    ///获取线程的OS thread id
    inline pthread_t const& getThreadId() const 
    {
        return m_threadId;
    }
    ///检查线程是否停止
    inline bool isStopped() const 
    {
        return m_bStopped;
    }
    ///设置线程停止
    inline void setStopped(bool bStopped) 
    {
        m_bStopped = bStopped;
    }
    ///获取线程是否在等待锁
    inline bool isBlocked() const 
    {
        return m_bBlocked;
    }
    ///设置线程在等待锁
    inline void setBlocked(bool bBlocked) 
    {
        m_bBlocked = bBlocked;
    }
    ///获取队列中消息数量
    inline CWX_UINT32 getQueuedMsgNum() const
    {
        return m_uiQueuedMsg;
    }
    ///设置队列中消息数量
    inline void setQueuedMsgNum(CWX_UINT32 uiNum)
    {
        m_uiQueuedMsg = uiNum;
    }
    ///设置线程的启动时间
    inline void setStartTime(time_t ttTime)
    {
        m_ttStartTime = ttTime;
    }
    ///获取线程的启动时间
    inline time_t getStartTime() const
    {
        return m_ttStartTime;
    }
    ///更新线程的罪行时间
    inline void setUpdateTime(time_t ttTime)
    {
        m_ttUpdateTime = ttTime;
    }
    ///获取线程的最新更新时间
    inline time_t getUpdateTime() const
    {
        return m_ttUpdateTime;
    }
    ///设置线程处理的消息任务的数量
    inline void setRecvMsgNum(CWX_UINT64 ullMsgNum)
    {
        m_llRecvMsgNum = ullMsgNum;
    }
    ///获取线程处理的消息任务的数量
    inline CWX_UINT64 getRecvMsgNum() const
    {
        return m_llRecvMsgNum;
    }
    ///递增线程处理的消息任务的数量
    inline void incRecvMsgNum()
    {
        m_llRecvMsgNum++;
    }
private:
    CWX_UINT16      m_unThreadGroup;///<线程的group
    CWX_UINT16      m_unThreadNo;///<线程的id
    pthread_t       m_threadId;///<线程的OS id
    bool           m_bStopped;///<线程是否停止
    bool           m_bBlocked;///<线程是否在等待锁
    CWX_UINT32      m_uiQueuedMsg; ///<队列中消息数量
    time_t          m_ttStartTime;///<线程的启动时间
    time_t          m_ttUpdateTime;///<线程的最新更新时间
    CWX_UINT64      m_llRecvMsgNum;///<线程处理的消息的数量
};

class CwxAppLogger;

/**
@class CwxAppTss
@brief 线程的TSS对象，用户存放在整个线程声明周期内一直有效的对象。。
*/
class CWX_API CwxAppTss
{
public:
    enum{
        TSS_2K_BUF = 2048///<error-buffer的大小
    };
public:
    ///构造函数
    CwxAppTss(CwxAppTssInfo* pThreadInfo)
        :m_pThreadInfo(pThreadInfo)
    {
        m_fileNo = 0;
        m_fileName = NULL;
    }
    ///析构函数
    virtual ~CwxAppTss()
    {
        if (m_pThreadInfo) delete m_pThreadInfo;
    }
public:
    ///获取Tss的thread info信息
    CwxAppTssInfo* getThreadInfo()
    {
        return m_pThreadInfo;
    }
    ///对线程的运行信息，形成package对象；返回值：false；失败；true：成功
    virtual bool packThreadInfo(CwxPackageWriter& threadInfoPack, char* szErr2K);
public:
    ///初始化对象的静态数据
    static int initTss();
    ///将pThrEnv注册为调用此API的tss
    static int regTss(CwxAppTss* pThrEnv);
    ///unreg线程的tss
    static int unRegTss();
    ///但实例化模式，获取此类的实例
    static CwxAppTss* instance();
private:
    ///默认构造函数
    CwxAppTss()
    {
        m_pThreadInfo = NULL;
    }
public:
    char                    m_szBuf2K[TSS_2K_BUF + 1];///错误消息描述
    int                     m_fileNo;
    char const*              m_fileName;
private:
    friend class CwxAppLogger;
    char                    m_szErrMsg[TSS_2K_BUF + 1];///错误消息描述
    CwxAppTssInfo*           m_pThreadInfo;///<当前TSS的线程info
    static bool              m_bInit;///<对象是否已经执行了init操作
    static pthread_key_t      m_tssKey;///<线程的tss对象
};

#define  CWX_APP_TSS(type) (type*)CwxAppTss::instance()
#define  CWX_APP_TSS_2K_BUF  CwxAppTss::instance()->m_szBuf2K
#define  CWX_APP_TSS_FILE_NO  CwxAppTss::instance()->m_fileNo
#define  CWX_APP_TSS_FILE_NAME  CwxAppTss::instance()->m_fileName


CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
