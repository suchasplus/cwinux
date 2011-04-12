
CWINUX_BEGIN_NAMESPACE

inline size_t CwxAppThreadPoolEx::getQueuedMsgNum()
{
    size_t num = 0;
    for (CWX_UINT16 i=0; i<getThreadNum(); i++){
        if (m_arrThreadPool && m_arrThreadPool[i]) {
            num += m_arrThreadPool[i]->getQueuedMsgNum();
        }
    }
    return num;
}
///获取线程的消息队列排队消息数
inline size_t CwxAppThreadPoolEx::getQueuedMsgNum(CWX_UINT16 unThreadIndex)
{
    if (!m_arrThreadPool || (unThreadIndex >= getThreadNum())) return 0;
    return m_arrThreadPool[unThreadIndex]->getQueuedMsgNum();
}

/**
@brief 往线程的消息队列添加一个新消息。
@param [in] pMsg append的消息
@param [in] uiThread 消息的线程队列，内部会根据Thread的数量，计算出所属的线程。
@return -1：失败；>=0为对应线程的队列中排队的消息数量
*/
inline int CwxAppThreadPoolEx::append(CwxMsgBlock* pMsg, CWX_UINT32 uiThread)
{
    CWX_UINT16 uiIndex = uiThread%getThreadNum();
    return m_arrThreadPool[uiIndex]->append(pMsg);
}


CWINUX_END_NAMESPACE

