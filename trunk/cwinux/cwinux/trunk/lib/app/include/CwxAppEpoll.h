#ifndef __CWX_APP_EPOLL_H__
#define __CWX_APP_EPOLL_H__

/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppEpoll.h
@brief 架构的epoll事件引擎对象
@author cwinux@gmail.com
@version 0.1
@date 2011-04-13
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxTimeValue.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxAppHandler4Base.h"
#include "CwxThread.h"
#include "CwxLogger.h"
#include "CwxMinHeap.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxAppEpoll
@brief 架构的epoll事件引擎
*/

class CWX_API CwxAppEpoll
{
public:
    enum
    {
        CWX_EPOLL_INIT_HANDLE = 81920
    };
public:
    ///构造函数
    CwxAppEpoll();
    ///析构函数
    ~CwxAppEpoll();
public:
    /**
    @brief 初始化epoll引擎。
    @return -1：失败；0：成功
    */
    int init();
    /**
    @brief 注册IO事件处理handle。
    @param [in] io_handle 监测的IO handle
    @param [in] event_handler io handle对应的event handler。
    @param [in] mask 注册的事件掩码，为READ_MASK、WRITE_MASK、PERSIST_MASK、TIMEOUT_MASK组合
    @return -1：失败；0：成功；
    */
    int registerHandler(CWX_HANDLE io_handle,
        CwxAppHandler4Base *event_handler,
        int mask);
    /**
    @brief 删除io事件处理handle。
    @param [in] event_handler 移除的event-handler
    @return -1：失败；
    0：成功；
    */
    int removeHandler (CwxAppHandler4Base *event_handler);
    /**
    @brief suspend io事件处理handle。
    @param [in] event_handler suspend的event-handler
    @param [in] suspend_mask suspend的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；
    0：成功；
    */
    int suspendHandler (CwxAppHandler4Base *event_handler,
        int suspend_mask);
    /**
    @brief resume io事件处理handle。
    @param [in] event_handler resume的event-handler
    @param [in] resume_mask resume的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；
    0：成功；
    */
    int resumeHandler (CwxAppHandler4Base *event_handler,
        int resume_mask);

    /**
    @brief 删除io事件处理handle。
    @param [in] handle 移除的 io handle
    @return NULL：不存在；否则：成功；
    */
    CwxAppHandler4Base* removeHandler (CWX_HANDLE handle);
    /**
    @brief suspend io事件处理handle。
    @param [in] handle suspend io handle
    @param [in] suspend_mask suspend的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；0：成功
    */
    int suspendHandler (CWX_HANDLE handle,
        int suspend_mask);
    /**
    @brief resume io事件处理handle。
    @param [in] handle resume io handle
    @param [in] resume_mask resume的事件,为READ_MASK、WRITE_MASK组合
    @return -1：失败；0：成功；
    */
    int resumeHandler (CWX_HANDLE handle,
        int resume_mask);
    /**
    @brief 注册signal事件处理handle，信号具有PERSIST属性。
    @param [in] signum 信号
    @param [in] event_handler signal的event handler
    @return -1：失败； 0：成功；
    */
    int registerSignal(int signum,
        CwxAppHandler4Base *event_handler);
    /**
    @brief 删除signal事件处理handle。
    @param [in] event_handler signal的event handler
    @return -1：失败；0：成功；
    */
    int removeSignal(CwxAppHandler4Base *event_handler);
    /**
    @brief 删除signal事件处理handle。多线程安全，任意线程都可以调用。
    @param [in] sig signal
    @return NULL：不存在；否则返回signal的handler
    */
    CwxAppHandler4Base* removeSignal(int sig);

    /**
    @brief 设置定时处理handle，timeout不具有persist特性。
    @param [in] event_handler timer的event handler
    @param [in] interval 定时的间隔
    @return -1：失败；0：成功；
    */
    int scheduleTimer (CwxAppHandler4Base *event_handler,
        CwxTimeValue const &interval);
    ///取消定时处理handle。
    int cancelTimer (CwxAppHandler4Base *event_handler);
    ///fork的re-init方法，返回值，0：成功；-1：失败
    int forkReinit();
    /**
    @brief 检测事件。
    @return -1：失败；0：成功
    */
    int poll();
private:
    ///获取下一个的epoll的超时时间
    void timeout(CWX_UINT64& ullTime);
    ///添加 epoll检测,mask为READ_MASK、WRITE_MASK的组合。
    int addEvent(int fd, int mask);
    ///删除存在的mask，mask为READ_MASK、WRITE_MASK的组合。
    int delEvent(int fd, int mask);
private:
    class EventHandle
    {
    public:
        EventHandle()
        {
            m_mask = 0;
            m_handler = NULL;
        }
        inline bool isReg() { return (m_mask&CwxAppHandler4Base::RW_MASK) != 0;}
    public:
        int         m_mask;
        CwxAppHandler4Base* m_handler;
    };
private:
    int                             m_epfd;     ///<epoll的fd
    struct epoll_event              m_events[CWX_APP_MAX_IO_NUM]; ///<epoll的event 数组
    EventHandle                     m_eHandler[CWX_APP_MAX_IO_NUM]; ///<epoll的event handler
    int                             m_signalFd[2]; ///<信号的读写handle
    sig_atomic_t                    m_arrSignals[CWX_APP_MAX_SIGNAL_ID + 1];///<signal的数组
    CwxAppHandler4Base*             m_sHandler[CWX_APP_MAX_SIGNAL_ID + 1];///<signal handler的数组
    volatile sig_atomic_t           m_bSignal; ///<是否有信号
    CwxMinHeap<CwxAppHandler4Base>  m_timeHeap; ///<时间堆
};


CWINUX_END_NAMESPACE
#include "CwxAppReactor.inl"
#include "CwxPost.h"

#endif
