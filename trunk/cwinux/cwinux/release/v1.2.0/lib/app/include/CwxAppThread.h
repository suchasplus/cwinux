#ifndef __CWX_APP_THREAD_H__
#define __CWX_APP_THREAD_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppThread.h
@brief 线程对象定义
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxMsgQueue.h"
#include "CwxMsgBlock.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxAppLogger.h"
#include "CwxAppTss.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;

typedef void* (*CWX_THR_FUNC)(void *);

#define THREAD_BOUND               0x00000001
#define THREAD_NEW_LWP             0x00000002
#define THREAD_DETACHED            0x00000040
#define THREAD_SUSPENDED           0x00000080
#define THREAD_DAEMON              0x00000100
#define THREAD_SCHED_FIFO          0x00020000
#define THREAD_SCHED_RR            0x00040000
#define THREAD_SCHED_DEFAULT       0x00080000
#define THREAD_JOINABLE            0x00010000
#define THREAD_SCOPE_SYSTEM        THREAD_BOUND
#define THREAD_SCOPE_PROCESS       0x00200000
#define THREAD_INHERIT_SCHED       0x00400000
#define THREAD_EXPLICIT_SCHED      0x00800000
#define THREAD_SCHED_IO            0x01000000
#define CWX_DEFAULT_THREAD_PRIORITY (-0x7fffffffL - 1L)

class CWX_API CwxAppThread
{
public:
    ///定义线程常量
    enum{
        DEATH_CHECK_MSG_WATER_MASK = 5, ///<线程的状态监测的排队消息门限
        DEATH_CHECK_UPDATE_WATER_MASK = 30 ///<线程失效的无状态更新的时间门限
    };
public :
    ///构造函数
    CwxAppThread(CwxAppFramework* pApp,///<app对象
        CWX_UINT16 unGroupId,///<线程的group id
        CWX_UINT16 unThreadId=0,///<线程在线程group中的序号
        CWX_UINT32 uiDeathCheckMsgWaterMask=DEATH_CHECK_MSG_WATER_MASK,///<线程的状态监测的排队消息门限
        CWX_UINT32 uiDeathCheckUpdateWaterMask=DEATH_CHECK_UPDATE_WATER_MASK///<线程失效的无状态更新的时间门限
        );
    ///析构函数
    virtual ~CwxAppThread();
public:
    /**
    @brief 启动线程
    @param [in] pThrEnv 线程的Tss，若不指定，可通过onThreadCreated创建。
    @param [in] stack_size 线程堆栈的大小，若为0，则采用系统默认大小。
    @return -1：失败； 0：成功
    */
    virtual int start(CwxAppTss* pThrEnv=NULL, size_t stack_size = 0);
    ///停止线程
    virtual void stop();
    /**
    @brief 通知线程创建，若要创建自己的Thread-Env，则重载此API
    @param [in] unGroup 线程的线程组。
    @param [in] unThreadId 线程在线程组中的序号。
    @param [out] pThrEnv 创建的Thread的Thread Env。
    @return -1：失败； 0：成功
    */
    virtual int onThreadCreated(CWX_UINT16 unGroup, CWX_UINT16 unThreadId, CwxAppTss*& pThrEnv);
    /**
    @brief 通知线程退出
    @param [out] pThrEnv Thread的Thread Env。
    @return void
    */
    virtual void onThreadClosed(CwxAppTss*& pThrEnv);
    /**
    @brief 线程的body，若需要改变线程的运行行为，则重载此API
    @param [out] pThrEnv Thread的Thread Env。
    @return void
    */
    virtual void threadMain(CwxAppTss* pThrEnv);
    ///check thread 是否死锁。若需要改变检查的规则，则重载此API
    virtual bool isDeath();
    ///check thread 是否停止。若需要改变检查的规则，则重载此API
    virtual bool isStop();
    ///获取线程的TSS，及Thread env
    virtual CwxAppTss* getTss();
    ///锁住线程。返回值0：成功；-1：失败
    virtual int lock();
    ///解锁线程。返回值0：成功；-1：失败
    virtual int unlock();
public:
    ///获取线程的消息队列排队消息数
    size_t getQueuedMsgNum();
    /**
    @brief 往线程的消息队列添加一个新消息。
    @param [in] pMsg append的消息
    @return -1：失败；>=0队列中排队的消息数量
    */
    int  append(CwxMsgBlock* pMsg);
    /**
    @brief 从线程的消息队列获取一个排队消息，若队列为空，则阻塞。
    @param [out] pMsg pop的消息。
    @return -1：失败；>=0队列中排队的消息数量
    */
    int  pop(CwxMsgBlock*& pMsg);
    ///获取线程的group id
    CWX_UINT16 getThreadId() const;
    ///获取现在在线程组中的序号
    CWX_UINT16 getGroupId() const;
public:
    /**
    @brief 创建一个线程。
    @param [in] func 线程的main函数。
    @param [in] args 线程的main函数的参数。
    @param [in] flags 线程的属性标志。
    @param [out] thr_id 线程的id。
    @param [in] priority 线程的优先级。
    @param [in] stack 线程的堆栈。
    @param [in] stacksize 线程的堆栈的大小。
    @return -1：失败，错误原因在errno；0成功。
    */
    static int spawn(CWX_THR_FUNC func,
        void *args=NULL,
        long flags=THREAD_NEW_LWP | THREAD_JOINABLE,
        pthread_t *thr_id=NULL,
        long priority = CWX_DEFAULT_THREAD_PRIORITY,
        void *stack = NULL,
        size_t stacksize = 0);
    ///对os的pthread_join的封装，0：成功；-1：失败，errno放失败的代码
    static int join(pthread_t thread, void **value_ptr);
    ///对os的pthread_kill的封装，0：成功；-1：失败，errno放失败的代码
    static int kill(pthread_t thread, int sig);
    ///对os的pthread_self的封装，返回thread id。
    static pthread_t self();
    ///对os的pthread_exit的封装
    static void exit(void *value_ptr);
    ///对os的pthread_equal的封装
    static bool equal(pthread_t t1, pthread_t t2);
    ///对os的pthread_cancel的封装，0：成功；-1：失败，errno放失败的代码
    static int cancel(pthread_t thread);
private:
    static void* threadFunc(void *);
private:
    CwxAppFramework*        m_pApp;///<架构的APP
    CwxAppTss*             m_pTssEnv;///<线程的tss
    CWX_UINT16             m_unGroupId;///<线程的线程组id
    CWX_UINT16             m_unThreadId;///<线程在线程组中的序号
    CWX_UINT32             m_uiTheadDeathMsgWaterMask;///<线程death检查的队列消息排队阀值
    CWX_UINT32             m_uiThreadDeathUpdateWaterMask;///<线程death检查的线程无状态更新的时间阀值
    CwxMsgQueue*         m_msgQueue;
    pthread_t              m_tid;
};

CWINUX_END_NAMESPACE

#include "CwxAppThread.inl"
#include "CwxPost.h"
#endif
