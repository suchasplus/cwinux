#include "CwxAppEpoll.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

CWINUX_BEGIN_NAMESPACE
CwxAppEpoll::CwxAppEpoll()
{
    m_epfd = CWX_INVALID_HANDLE;
    memset(m_events, 0x00, sizeof(m_events));
    m_signalFd[0] = CWX_INVALID_HANDLE;
    m_signalFd[1] = CWX_INVALID_HANDLE;
    memset(m_arrSignals, 0x00, sizeof(m_arrSignals));
    memset(m_sHandler, 0x00, sizeof(m_sHandler));
    m_bSignal = 0;
    m_sigHandler = new SignalHanlder();
}

CwxAppEpoll::~CwxAppEpoll()
{
    if (m_epfd != CWX_INVALID_HANDLE)
    {
        ::close(m_epfd);
    }
    if (m_signalFd[0] != CWX_INVALID_HANDLE)
    {
        ::close(m_signalFd[0]);
    }
    if (m_signalFd[1] != CWX_INVALID_HANDLE)
    {
        ::close(m_signalFd[1]);
    }
    if (m_sigHandler) delete m_sigHandler;
}

///初始化epoll
int CwxAppEpoll::init()
{
    if (CWX_INVALID_HANDLE != m_epfd)
    {
        CWX_ERROR(("epoll handle is created, file_id=%d", m_epfd));
        return -1;
    }
    m_epfd = epoll_create(CWX_EPOLL_INIT_HANDLE); 
    if (m_epfd == -1)
    {
        CWX_ERROR(("Failure to invoke epoll_create, errno=%d", errno));
        return -1;
    }
    if (0 != CwxIpcSap::setCloexec(m_epfd, true))
    {
        CWX_ERROR(("Failure to set handle's cloexec sign, errno=%d", errno));
        return -1;
    }
    //创建signal的handle
    if (0 != pipe(m_signalFd))
    {
        CWX_ERROR(("Failure to invokde socketpair to create signal fd, errno=%d", errno));
        return -1;
    }
    if (0 != CwxIpcSap::setCloexec(m_signalFd[0], true))
    {
        CWX_ERROR(("Failure to set signal handle[0]'s cloexec sign, errno=%d", errno));
        return -1;
    }
    if (0 != CwxIpcSap::setCloexec(m_signalFd[1], true))
    {
        CWX_ERROR(("Failure to set signal handle[1]'s cloexec sign, errno=%d", errno));
        return -1;
    }
    if (0 != CwxIpcSap::setNonblock(m_signalFd[0], true))
    {
        CWX_ERROR(("Failure to set signal handle[0]'s noblock sign, errno=%d", errno));
        return -1;
    }
    if (0 != CwxIpcSap::setNonblock(m_signalFd[1], true))
    {
        CWX_ERROR(("Failure to set signal handle[1]'s noblock sign, errno=%d", errno));
        return -1;
    }
    //注册信号fd的读
    if (0 != registerHandler(m_signalFd[0], m_sigHandler, CwxAppHandler4Base::PREAD_MASK))
    {
        CWX_ERROR(("Failure to register signal handle to engine"));
        return -1;
    }
    return 0;
}

int CwxAppEpoll::registerHandler(CWX_HANDLE handle,
                    CwxAppHandler4Base *event_handler,
                    int mask,
                    CWX_UINT32 uiMillSecond)
{
    if ((handle < 0) || (handle >= CWX_APP_MAX_IO_NUM))
    {
        CWX_ERROR(("Invalid io handle id[%d], range[0,%d]", handle, CWX_APP_MAX_IO_NUM));
        return -1;
    }
    if (m_eHandler[handle].m_handler)
    {
        CWX_ERROR(("handler is registered, handle[%d]", (int)handle));
        return -1;
    }

    mask &=CwxAppHandler4Base::IO_MASK; ///只支持READ、WRITE、PERSIST、TIMEOUT四种掩码
    if (uiMillSecond) mask |= CwxAppHandler4Base::TIMEOUT_MASK;

    if (uiMillSecond)
    {
        CWX_ASSERT(-1 == event_handler->index());
        event_handler->m_ullTimeout = CwxDate::getTimestamp();
        event_handler->m_ullTimeout += uiMillSecond * 1000;
        if (!m_timeHeap.push(event_handler))
        {
            CWX_ASSERT(("Failure to add handler to time-heap, io[%d]", handle));
            return -1;
        }
    }

    if (mask&CwxAppHandler4Base::RW_MASK) ///如果存在READ、WRITE的掩码，则注册
    {
        if (0 != addEvent(handle, mask))
        {
            if (uiMillSecond) 
                m_timeHeap.erase(event_handler); ///删除timeout
            return -1;
        }
    }
    m_eHandler[handle].m_handler = event_handler;
    m_eHandler[handle].m_mask = mask;
    return 0;
}


CwxAppHandler4Base* CwxAppEpoll::removeHandler (CWX_HANDLE handle)
{
    if ((handle < 0) || (handle >= CWX_APP_MAX_IO_NUM))
    {
        CWX_ERROR(("Invalid io handle id[%d], range[0,%d]", io_handle, CWX_APP_MAX_IO_NUM));
        return NULL;
    }
    CwxAppHandler4Base* event_handler = m_eHandler[handle];
    if (!event_handler) return NULL;
    if (event_handler->index() >=0)
    {//timeout
        m_timeHeap.erase(event_handler);
        event_handler->setTimeout(0);
    }
    delEvent(handle, m_eHandler[handle].m_mask);
    m_eHandler[handle].m_mask = 0;
    m_eHandler[handle].m_handler = NULL;
    return event_handler;
}

int CwxAppEpoll::suspendHandler (CWX_HANDLE handle,
                    int suspend_mask)
{
    if ((handle < 0) || (handle >= CWX_APP_MAX_IO_NUM))
    {
        CWX_ERROR(("Invalid io handle id[%d], range[0,%d]", io_handle, CWX_APP_MAX_IO_NUM));
        return NULL;
    }
    CwxAppHandler4Base* event_handler = m_eHandler[handle];
    if (!event_handler)
    {
        CWX_ERROR(("Handler[%d] doesn't exist.", handle));
        return 0;
    }
    suspend_mask &=(CwxAppHandler4Base::RW_MASK&m_eHandler[handle].m_mask);
    if(!suspend_mask) return 0;
    if (0 != delEvent(handle, suspend_mask))
    {
        CWX_ERROR(("Failure to suspend handler[%d]", handle));
        return -1;
    }
    m_eHandler[handle] &=~suspend_mask;
    return 0;
}

int CwxAppEpoll::resumeHandler (CWX_HANDLE handle,
                   int resume_mask)
{
    if ((handle < 0) || (handle >= CWX_APP_MAX_IO_NUM))
    {
        CWX_ERROR(("Invalid io handle id[%d], range[0,%d]", io_handle, CWX_APP_MAX_IO_NUM));
        return NULL;
    }
    CwxAppHandler4Base* event_handler = m_eHandler[handle];
    if (!event_handler)
    {
        CWX_ERROR(("Handler[%d] doesn't exist.", handle));
        return 0;
    }
    resume_mask &=CwxAppHandler4Base::RW_MASK;
    resume_mask &=~m_eHandler[handle].m_mask;
    if(!resume_mask) return 0;
    if (0 != addEvent(handle, resume_mask))
    {
        CWX_ERROR(("Failure to resume handler[%d]", handle));
        return -1;
    }
    m_eHandler[handle] |=resume_mask;
    return 0;

}

int CwxAppEpoll::registerSignal(int signum,
                   CwxAppHandler4Base *event_handler)
{

}

int CwxAppEpoll::removeSignal(CwxAppHandler4Base *event_handler)
{

}

CwxAppHandler4Base* CwxAppEpoll::removeSignal(int sig)
{

}


int CwxAppEpoll::scheduleTimer (CwxAppHandler4Base *event_handler,
                   CwxTimeValue const &interval)
{

}

int CwxAppEpoll::cancelTimer (CwxAppHandler4Base *event_handler)
{

}

int CwxAppEpoll::forkReinit()
{

}


int CwxAppEpoll::poll()
{
    int i = 0;
    int num = 0;
    int ret = 0;
    CWX_UINT64 ullNow = CwxDate::getTimestamp();
    CWX_UINT64 ullTimeout = 0;
    int tv=0;
    struct epoll_event *event=NULL;
    ///计算超时时间
    timeout(ullTimeout);
    if (!ullTimeout)
    {
        tv = -1;
    }else
    {
        tv = (int)(ullTimeout - ullNow);
        if (tv < 1) tv = 1;
    }

    num = epoll_wait(m_epfd, m_events, CWX_APP_MAX_IO_NUM, tv);
    if (num > 0) {
        for (i = 0; i < num; i++) {
            int mask = 0;
            event = m_events[i];
            if (m_signalFd[0] == event->data.fd)
            {
                CWX_ASSERT(event->events == EPOLLIN);
                continue;
            }
            CWX_ASSERT(m_eHandler[event->data.fd]);
            if (event->events & EPOLLIN) mask |= CwxAppHandler4Base::READ_MASK;
            if (event->events & EPOLLOUT) mask |= CwxAppHandler4Base::WRITE_MASK;
            //如果不是持久，则移除handle
            if (!m_eHandler[event->data.fd]->isPersistMask())
            {
                
            }
            //检测timeout
            if (m_eHandler[event->data.fd]->index()>=0)
            {//从heap中删除
                m_timeHeap.erase(m_eHandler[event->data.fd]);
            }
            ret = m_eHandler[event->data.fd]->handle_event(mask, event->data.fd);
            if (-1 == ret)
            {
                handler->close(event->data.fd);
            }
        }
    }
    return numevents;

}



CWINUX_END_NAMESPACE
