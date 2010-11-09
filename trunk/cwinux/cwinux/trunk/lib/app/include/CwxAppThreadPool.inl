CWINUX_BEGIN_NAMESPACE

inline size_t CwxAppThreadPool::getQueuedMsgNum()
{ 
    return m_msgQueue->getMsgCount();
}

inline int CwxAppThreadPool::append(CwxMsgBlock* pMsg)
{
    return m_msgQueue->enqueue_tail(pMsg);
}

inline int CwxAppThreadPool::pop(CwxMsgBlock*& pMsg)
{
    return m_msgQueue->dequeue(pMsg);
}

CWINUX_END_NAMESPACE

