CWINUX_BEGIN_NAMESPACE

inline CwxAppTaskBoardConnInfo::CwxAppTaskBoardConnInfo(CWX_UINT32 uiSvrId,
                                                        CWX_UINT32 uiHostId,
                                                        CWX_UINT32 uiConnId,
                                                        CwxMsgBlock* msg)
                                                        :m_uiSvrId(uiSvrId),
                                                        m_uiHostId(uiHostId),
                                                        m_uiConnId(uiConnId),
                                                        m_msg(msg)
{

}

inline CWX_UINT32 CwxAppTaskBoardConnInfo::getSvrId() const
{
    return m_uiSvrId;
}

inline CWX_UINT32 CwxAppTaskBoardConnInfo::getHostId() const
{
    return m_uiHostId;
}

inline CWX_UINT32 CwxAppTaskBoardConnInfo::getConnId() const 
{
    return m_uiConnId;
}

inline CwxMsgBlock* CwxAppTaskBoardConnInfo::getMsg() 
{
    return m_msg;
}

inline void CwxAppTaskBoardConnInfo::setMsg(CwxMsgBlock* msg)
{
    m_msg = msg;
}

///构造函数
inline CwxAppTaskBoardTask::CwxAppTaskBoardTask(CwxAppTaskBoard* pTaskBoard)
:m_pTaskBoard(pTaskBoard)
{
    m_uiTaskId = 0;
    m_ullTimeoutStamp = 0;
    m_ucTaskState = TASK_STATE_INIT;
    m_next = NULL;
    m_prev = NULL;
    m_bLooked = false;
    m_bWaitingRemove = false;
    m_bTimeout = false;
}

inline CWX_UINT32 CwxAppTaskBoardTask::getTaskId() const
{
    return m_uiTaskId;
}

inline void CwxAppTaskBoardTask::setTaskId(CWX_UINT32 id) 
{
    m_uiTaskId = id;
}

inline CWX_UINT64 const& CwxAppTaskBoardTask::getTimeoutValue() const
{
    return m_ullTimeoutStamp;
}

inline void CwxAppTaskBoardTask::setTimeoutValue(CWX_UINT64 ullTimestamp)
{
    m_ullTimeoutStamp = ullTimestamp;
}

inline CWX_UINT8 CwxAppTaskBoardTask::getTaskState() const 
{
    return m_ucTaskState;
}

inline void CwxAppTaskBoardTask::setTaskState(CWX_UINT8 state)
{
    m_ucTaskState = state;
}

inline bool CwxAppTaskBoardTask::isFinish() const
{
    return m_bWaitingRemove || m_bTimeout || (TASK_STATE_FINISH == m_ucTaskState);
}

inline bool CwxAppTaskBoardTask::isTimeout() const
{
    return m_bTimeout; 
}

inline bool CwxAppTaskBoardTask::isWaitingRemove() const 
{
    return m_bWaitingRemove;
}

inline bool CwxAppTaskBoardTask::checkTimeout(CWX_UINT64 const& ullNow)
{
    if (ullNow > m_ullTimeoutStamp) m_bTimeout = true;
    if (m_bTimeout) m_ucTaskState = TASK_STATE_FINISH;
    return m_bTimeout;
}

inline bool CwxAppTaskBoardTask::isLocked() const
{
    return m_bLooked;
}

inline void CwxAppTaskBoardTask::clearBase()
{
    m_next = NULL;
    m_prev = NULL;
    m_bWaitingRemove = false;
    m_bLooked = false;
    m_bTimeout = false;
    list<CwxMsgBlock*>::iterator iter;

    iter = m_failSendMsgList.begin();
    while(iter != m_failSendMsgList.end())
    {
        CwxMsgBlockAlloc::free(*iter);
        iter++;
    }
    m_failSendMsgList.clear();

    iter = m_recvMsgList.begin();
    while(iter != m_recvMsgList.end())
    {
        CwxMsgBlockAlloc::free(*iter);
        iter++;
    }
    m_recvMsgList.clear();

    list<CwxAppTaskBoardConnInfo*>::iterator iter_conn = m_sendCloseConnList.begin();
    while(iter_conn != m_sendCloseConnList.end())
    {
        delete *iter_conn;
        iter_conn++;
    }
    m_sendCloseConnList.clear();
}
///获取Task缓存的事件
inline void CwxAppTaskBoardTask::fetchWaitingMsg(bool& bTimeout,
                                                 list<CwxMsgBlock*>& failSendMsgs, 
                                                 list<CwxMsgBlock*>& recvMsgs,
                                                 list<CwxAppTaskBoardConnInfo*>& endclosedConnList)
{
    bTimeout = m_bTimeout;

    failSendMsgs.clear();
    failSendMsgs = m_failSendMsgList;
    m_failSendMsgList.clear();

    recvMsgs.clear();
    recvMsgs = m_recvMsgList;
    m_recvMsgList.clear();

    endclosedConnList.clear();
    endclosedConnList = m_sendCloseConnList;
    m_sendCloseConnList.clear();
}
///缓存一个发送的数据包通过连接发送完毕的事件
inline void CwxAppTaskBoardTask::addEndSendMsgEvent(CwxMsgBlock* msg)
{
    m_sendCloseConnList.push_back(new CwxAppTaskBoardConnInfo(0, 0, 0, msg));
}
///缓存一个数据包发送失败的事件
inline void CwxAppTaskBoardTask::addFailSendMsgEvent(CwxMsgBlock* msg)
{
    m_failSendMsgList.push_back(msg);
}
///缓存收到一个数据包的事件
inline void CwxAppTaskBoardTask::addRecvMsgEvent(CwxMsgBlock* msg)
{
    m_recvMsgList.push_back(msg);
}
///缓存等待接受数据的一条连接关闭的的事件
inline void CwxAppTaskBoardTask::addClosedConnEvent(CWX_UINT32 uiSvrId,
                                                    CWX_UINT32 uiHostId,
                                                    CWX_UINT32 uiConnId)
{
    m_sendCloseConnList.push_back(new CwxAppTaskBoardConnInfo(uiSvrId, uiHostId, uiConnId, NULL));
}



inline CwxAppTaskBoardConnTasks::CwxAppTaskBoardConnTasks(CWX_UINT32 uiConnId,
                                                          CWX_UINT32 uiTaskId)
:m_uiConnId(uiConnId), m_uiTaskId(uiTaskId)
{

}

inline CwxAppTaskBoardConnTasks::~CwxAppTaskBoardConnTasks()
{

}

inline CwxAppTaskBoardConnTasks::CwxAppTaskBoardConnTasks(CwxAppTaskBoardConnTasks const& item)
{
    m_uiConnId = item.m_uiConnId;
    m_uiTaskId = item.m_uiTaskId;
}

inline CwxAppTaskBoardConnTasks& CwxAppTaskBoardConnTasks::operator=(CwxAppTaskBoardConnTasks const& item)
{
    m_uiConnId = item.m_uiConnId;
    m_uiTaskId = item.m_uiTaskId;
    return *this;
}

inline bool CwxAppTaskBoardConnTasks::operator<(CwxAppTaskBoardConnTasks const& item) const
{
    if (m_uiConnId < item.m_uiConnId) return true;
    if (m_uiConnId > item.m_uiConnId) return false;
    return m_uiTaskId<item.m_uiTaskId;
}

inline bool CwxAppTaskBoardConnTasks::operator==(CwxAppTaskBoardConnTasks const& item) const
{
    return (m_uiConnId == item.m_uiConnId) && (m_uiTaskId == item.m_uiTaskId);
}




///构造函数
inline CwxAppTaskBoardTaskConns::CwxAppTaskBoardTaskConns(CWX_UINT32 uiTaskId, CWX_UINT32 uiConnId)
:m_uiTaskId(uiTaskId), m_uiConnId(uiConnId)
{

}

inline CwxAppTaskBoardTaskConns::~CwxAppTaskBoardTaskConns()
{

}

inline CwxAppTaskBoardTaskConns::CwxAppTaskBoardTaskConns(CwxAppTaskBoardTaskConns const& item)
{
    m_uiTaskId = item.m_uiTaskId;
    m_uiConnId = item.m_uiConnId;
}

inline CwxAppTaskBoardTaskConns& CwxAppTaskBoardTaskConns::operator=(CwxAppTaskBoardTaskConns const& item)
{
    m_uiTaskId = item.m_uiTaskId;
    m_uiConnId = item.m_uiConnId;
    return *this;
}

inline bool CwxAppTaskBoardTaskConns::operator<(CwxAppTaskBoardTaskConns const& item) const
{
    if (m_uiTaskId < item.m_uiTaskId) return true;
    if (m_uiTaskId > item.m_uiTaskId) return false;
    return m_uiConnId<item.m_uiConnId;
}
///等于操作符
inline bool CwxAppTaskBoardTaskConns::operator==(CwxAppTaskBoardTaskConns const& item) const
{
    return (m_uiTaskId == item.m_uiTaskId)&&(m_uiConnId == item.m_uiConnId);
}

inline CwxAppTaskBoard::CwxAppTaskBoard(CWX_UINT32 uiMaxTaskNum)
{
    m_pTaskHead = NULL;
    m_pTaskTail = NULL;
    m_pTaskMap = NULL;
    m_uiMaxTaskNum = uiMaxTaskNum;
    if (m_uiMaxTaskNum < 256) m_uiMaxTaskNum = 256;

}
///析构函数
inline CwxAppTaskBoard::~CwxAppTaskBoard()
{
    reset();
}

///获取Taskboard管理的任务数量
inline CWX_UINT32 CwxAppTaskBoard::getTaskNum() const
{
    return m_pTaskMap->size();
}
///获取taskboard中conn-tasks的map中元素数目，为了调试
inline int CwxAppTaskBoard::getConnTasksMapSize() const 
{
    return m_connTaskMap.size();
}
///获取taskboard中task-conns的set中元素数目，为了调试
inline int CwxAppTaskBoard::getTaskConnsMapSize() const 
{
    return m_taskConnSet.size();
}
///清空Taskboard
inline void CwxAppTaskBoard::reset()
{
    CwxAppTaskBoardTask* pTmp = m_pTaskHead;
    while(m_pTaskHead)
    {
        pTmp = m_pTaskHead->m_next;
        delete m_pTaskHead;
        m_pTaskHead = pTmp;
    }
    m_pTaskHead = NULL;
    m_pTaskTail = NULL;
    if (m_pTaskMap) delete m_pTaskMap;
    m_pTaskMap = NULL;
    m_connTaskMap.clear();
    m_taskConnSet.clear();
}

///不带锁的判断一个任务是否存在
inline bool CwxAppTaskBoard::_isExist(CWX_UINT32 uiTaskId)
{
    return m_pTaskMap->find(uiTaskId) != m_pTaskMap->end();
}
///不带锁往Taskboard添加一个Task
inline bool CwxAppTaskBoard::_addTask(CwxAppTaskBoardTask* pTask)
{
    if (_isExist(pTask->getTaskId())) return false;
    pTask->clearBase();
    (*m_pTaskMap)[pTask->getTaskId()] = pTask;
    pTask->m_bLooked = true;
    pTask->m_next = m_pTaskHead;
    pTask->m_prev = NULL;
    if (m_pTaskHead) m_pTaskHead->m_prev = pTask;
    m_pTaskHead = pTask;
    if (!m_pTaskTail) m_pTaskTail = m_pTaskHead;
    return true;
}
///不带锁根据TaskId获取对应的Task
inline CwxAppTaskBoardTask* CwxAppTaskBoard::_getTask(CWX_UINT32 uiTaskId)
{
    CWX_APP_TASK_MAP::iterator iter = m_pTaskMap->find(uiTaskId);
    if (iter != m_pTaskMap->end()) return iter->second;
    return NULL;
}
///不带锁，添加连接的一个Task
inline void CwxAppTaskBoard::_addConnTask(CWX_UINT32 uiConnId, CwxAppTaskBoardTask* pTask)
{
    m_connTaskMap[CwxAppTaskBoardConnTasks(uiConnId, pTask->getTaskId())] = pTask;
    m_taskConnSet.insert(CwxAppTaskBoardTaskConns(pTask->getTaskId(), uiConnId));
}
///不带锁，删除连接的一个Task
inline void CwxAppTaskBoard::_removeConnTask(CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId)
{
    m_connTaskMap.erase(CwxAppTaskBoardConnTasks(uiConnId, uiTaskId));
    m_taskConnSet.erase(CwxAppTaskBoardTaskConns(uiTaskId, uiConnId));
}
///不带锁，删除一条连接上的所有Task
inline void CwxAppTaskBoard::_removeConnTask(CWX_UINT32 uiConnId)
{
    CwxAppTaskBoardConnTasks connTask(uiConnId, 0);
    map<CwxAppTaskBoardConnTasks, CwxAppTaskBoardTask*>::iterator iter_begin=m_connTaskMap.lower_bound(connTask);
    map<CwxAppTaskBoardConnTasks, CwxAppTaskBoardTask*>::iterator iter_end = iter_begin;
    while(iter_end != m_connTaskMap.end())
    {
        if (iter_end->first.m_uiConnId == uiConnId)
        {
            m_taskConnSet.erase(CwxAppTaskBoardTaskConns(iter_end->first.m_uiTaskId, iter_end->first.m_uiConnId));
            iter_end++;
        }
        break;
    }
    m_connTaskMap.erase(iter_begin, iter_end);
}
CWINUX_END_NAMESPACE
