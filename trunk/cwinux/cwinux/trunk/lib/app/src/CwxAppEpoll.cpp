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
    m_eHandler[m_signalFd[0]]->m_mask = ;
    if (-1 == addEvent(m_signalFd[0], CwxAppHandler4Base::READ_MASK))
    {
        CWX_ERROR(("Failure to register signal read pipe"));
        return -1;
    }
    m_eHandler[m_signalFd[0]]->m_mask = CwxAppHandler4Base::READ_MASK;
    return 0;
}


int CwxAppEpoll::poll()
{
    int i = 0;
    int num = 0;
    int ret = 0;
    CWX_UINT64 ullNow = CwxDate::getTimestamp();
    CWX_UINT64 ullTimeout = 0;
    int tv=0;
    char sigBuf[64];
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
                recv(m_signalFd[0], sigBuf, sizeof(sigBuf), 0);
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
