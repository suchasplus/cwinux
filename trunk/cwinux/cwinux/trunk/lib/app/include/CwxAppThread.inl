
CWINUX_BEGIN_NAMESPACE

///获取线程的消息队列排队消息数
inline size_t CwxAppThread::getQueuedMsgNum()
{
    return m_msgQueue->getMsgCount();
}
/**
@brief 往线程的消息队列添加一个新消息。
@param [in] pMsg append的消息
@return -1：失败；>=0队列中排队的消息数量
*/
inline int  CwxAppThread::append(CwxMsgBlock* pMsg)
{
    return this->m_msgQueue->enqueue_tail(pMsg);
}
/**
@brief 从线程的消息队列获取一个排队消息，若队列为空，则阻塞。
@param [out] pMsg pop的消息。
@return -1：失败；>=0队列中排队的消息数量
*/
inline int CwxAppThread::pop(CwxMsgBlock*& pMsg)
{
    return this->m_msgQueue->dequeue(pMsg);
}


///获取线程的group id
inline CWX_UINT16 CwxAppThread::getThreadId() const
{
    return m_unThreadId;
}
///获取现在在线程组中的序号
inline CWX_UINT16 CwxAppThread::getGroupId() const 
{ 
    return m_unGroupId;
}


inline int CwxAppThread::join(pthread_t thread, void **value_ptr)
{
    int result = ::pthread_join(thread, value_ptr);
    if (0 != result)
    {
        errno = result;
        result = -1;
        CWX_ERROR(("Failure to invoke pthread_join(), err-code=%d", errno));
    }
    return result;
}

inline int CwxAppThread::kill(pthread_t thread, int sig)
{
    int result = ::pthread_kill(thread, sig);
    if (0 != result)
    {
        errno = result;
        result = -1;
        CWX_ERROR(("Failure to invoke pthread_kill(), err-code=%d", errno));
    }
    return result;
}

inline pthread_t CwxAppThread::self()
{
    return pthread_self();
}

inline void CwxAppThread::exit(void *value_ptr)
{
    return pthread_exit(value_ptr);
}

inline bool CwxAppThread::equal(pthread_t t1, pthread_t t2)
{
    return pthread_equal(t1, t2) != 0;
}


inline int CwxAppThread::cancel(pthread_t thread)
{
    int result = ::pthread_cancel(thread);
    if (0 != result)
    {
        errno = result;
        result = -1;
        CWX_ERROR(("Failure to invoke pthread_cancel(), err-code=%d", errno));
    }
    return result;
}


CWINUX_END_NAMESPACE

