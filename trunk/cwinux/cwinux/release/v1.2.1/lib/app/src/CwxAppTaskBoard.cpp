#include "CwxDate.h"
#include "CwxAppTaskBoard.h"

CWINUX_BEGIN_NAMESPACE
///析构函数
CwxAppTaskBoardConnInfo::~CwxAppTaskBoardConnInfo()
{
    if (m_msg) CwxMsgBlockAlloc::free(m_msg);
}


///析构函数
CwxAppTaskBoardTask::~CwxAppTaskBoardTask()
{
    clearBase();
}


int CwxAppTaskBoard::init()
{
    this->reset();
    m_pTaskMap = new CWX_APP_TASK_MAP((int)(m_uiMaxTaskNum * 1.5));
    return 0;
}

bool CwxAppTaskBoard::isExist(CWX_UINT32 uiTaskId)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    return _isExist(uiTaskId);
}

///-1:task doesn't exist, 0:task is managed by task-board, 1:task is finised
int CwxAppTaskBoard::remove(CWX_UINT32 uiTaskId, CwxAppTaskBoardTask*& pFinishTask)
{
    CwxAppTaskBoardTask* pTask = NULL;
    pFinishTask = NULL;
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        pTask = _getTask(uiTaskId);
        if (!pTask) return -1;
        pTask->m_bWaitingRemove = true;
        if (pTask->isLocked()) return 0;
        _remove(pTask->getTaskId());
        pTask->m_bLooked = true;
    }
    pFinishTask = pTask;
    return 1;
}

///-1:task exist, 0:task is managed by task-board, 1:task is finised
int CwxAppTaskBoard::noticeActiveTask(CwxAppTaskBoardTask* pTask, CwxAppTss* pThrEnv)
{
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        if (!_addTask(pTask)) return -1;
    }
    if (-1 == pTask->noticeActive(pThrEnv))
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        pTask->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
        this->_remove(pTask->getTaskId());
        return 1;
    }
    CWX_UINT8 ucTaskState = dispatchEvent(pTask, pThrEnv);
    if (CwxAppTaskBoardTask::TASK_STATE_FINISH == ucTaskState)
    {
        return 1;
    }
    return 0;
}


void CwxAppTaskBoard::noticeCheckTimeout(CwxAppTss* pThrEnv, list<CwxAppTaskBoardTask*>& finishTasks)
{
    CWX_UINT64 ullTime = CwxDate::getTimestamp();
    finishTasks.clear();
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        CwxAppTaskBoardTask* pLoopTask = m_pTaskTail;
        while(pLoopTask)
        {
            if (pLoopTask->checkTimeout(ullTime) && !pLoopTask->isLocked())
            {
                finishTasks.push_back(pLoopTask);
                pLoopTask->m_bLooked = true;
            }
            pLoopTask = pLoopTask->m_prev;
        }
    }
    list<CwxAppTaskBoardTask*>::iterator iter = finishTasks.begin();
    CWX_UINT8 ucTaskState = 0;
    while(iter != finishTasks.end())
    {
        ucTaskState = dispatchEvent(*iter, pThrEnv);
        CWX_ASSERT(CwxAppTaskBoardTask::TASK_STATE_FINISH == ucTaskState);
        iter++;
    }
}

///-1:task doesn't exist, 0:task is managed by task-board, 1:task is finised
int CwxAppTaskBoard::noticeRecvMsg(CWX_UINT32 uiTaskId,
                                   CwxMsgBlock*& msg,
                                   CwxAppTss* pThrEnv,
                                   CwxAppTaskBoardTask*& pFinishTask)
{
    CwxAppTaskBoardTask* pTask = NULL;
    pFinishTask = NULL;
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        pTask = _getTask(uiTaskId);
        if (!pTask) return -1;
        pTask->addRecvMsgEvent(msg);
        msg = NULL;
        if (pTask->isLocked()) return 0;
        pTask->m_bLooked = true;
    }
    CWX_UINT8 ucTaskState = dispatchEvent(pTask, pThrEnv);
    if (CwxAppTaskBoardTask::TASK_STATE_FINISH == ucTaskState)
    {
        pFinishTask = pTask;
        return 1;
    }
    return 0;
}

///-1:task doesn't exist, 0:task is managed by task-board, 1:task is finised
int CwxAppTaskBoard::noticeFailSendMsg(CWX_UINT32 uiTaskId,
                                    CwxMsgBlock*& msg,
                                    CwxAppTss* pThrEnv,
                                    CwxAppTaskBoardTask*& pFinishTask)
{
    CwxAppTaskBoardTask* pTask = NULL;
    pFinishTask = NULL;
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        pTask = _getTask(uiTaskId);
        if (!pTask) return -1;
        pTask->addFailSendMsgEvent(msg);
        msg = NULL;
        if (pTask->isLocked()) return 0;
        pTask->m_bLooked = true;
    }
    CWX_UINT8 ucTaskState = dispatchEvent(pTask, pThrEnv);
    if (CwxAppTaskBoardTask::TASK_STATE_FINISH == ucTaskState)
    {
        pFinishTask = pTask;
        return 1;
    }
    return 0;
}

///-1:task doesn't exist, 0:task is managed by task-board, 1:task is finised
int CwxAppTaskBoard::noticeEndSendMsg(CWX_UINT32 uiTaskId, CwxMsgBlock*& msg, CwxAppTss* pThrEnv, CwxAppTaskBoardTask*& pFinishTask)
{
    CwxAppTaskBoardTask* pTask = NULL;
    pFinishTask = NULL;
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        pTask = _getTask(uiTaskId);
        if (!pTask) return -1;
        pTask->addEndSendMsgEvent(msg);
        msg = NULL;
        if (pTask->isLocked()) return 0;
        pTask->m_bLooked = true;
    }
    CWX_UINT8 ucTaskState = dispatchEvent(pTask, pThrEnv);
    if (CwxAppTaskBoardTask::TASK_STATE_FINISH == ucTaskState)
    {
        pFinishTask = pTask;
        return 1;
    }
    return 0;
}

void CwxAppTaskBoard::noticeConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv, list<CwxAppTaskBoardTask*>& finishTasks)
{
    CwxAppTaskBoardTask* pTask = NULL;
    list<CwxAppTaskBoardTask*> locFinishTasks;
    finishTasks.clear();
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        CwxAppTaskBoardConnTasks connTasks(msg->event().getConnId(), 0);
        map<CwxAppTaskBoardConnTasks, CwxAppTaskBoardTask*>::iterator iter=m_connTaskMap.lower_bound(connTasks);
        while(iter != m_connTaskMap.end())
        {
            if (iter->first.m_uiConnId != msg->event().getConnId()) break;
            pTask = iter->second;
            pTask->addClosedConnEvent(msg->event().getSvrId(), msg->event().getHostId(), msg->event().getConnId());
            if (!pTask->isLocked())
            {
                pTask->m_bLooked = true;
                locFinishTasks.push_back(pTask);
            }
            iter++;
        }
    }
    CwxMsgBlockAlloc::free(msg);
    msg = NULL;
    list<CwxAppTaskBoardTask*>::iterator iter = locFinishTasks.begin();
    CWX_UINT8 ucTaskState = 0;
    while(iter != locFinishTasks.end())
    {
        ucTaskState = dispatchEvent(*iter, pThrEnv);
        if(CwxAppTaskBoardTask::TASK_STATE_FINISH == ucTaskState)
        {
            finishTasks.push_back(*iter);
        }
        iter++;
    }
}


///remove task;
CwxAppTaskBoardTask* CwxAppTaskBoard::_remove(CWX_UINT32 uiTaskId)
{
    CWX_APP_TASK_MAP::iterator iter;
    CwxAppTaskBoardTask* pTask;
    iter = this->m_pTaskMap->find(uiTaskId);
    if (iter == this->m_pTaskMap->end()) return NULL;
    pTask = iter->second;
    //remove task from map
    this->m_pTaskMap->erase(iter);
    //remove task from timeout chain
    if ((NULL == pTask->m_prev) && (NULL == pTask->m_next))
    {
        m_pTaskHead = NULL;
        m_pTaskTail = NULL;
    }
    else if (NULL == pTask->m_prev)
    {
        m_pTaskHead = pTask->m_next;
        m_pTaskHead->m_prev = NULL;
    }
    else if (NULL == pTask->m_next)
    {
        m_pTaskTail = pTask->m_prev;
        m_pTaskTail->m_next = NULL;
    }
    else
    {
        pTask->m_prev->m_next = pTask->m_next;
        pTask->m_next->m_prev = pTask->m_prev;
    }

    CwxAppTaskBoardTaskConns taskConns(pTask->getTaskId(), 0);
    set<CwxAppTaskBoardTaskConns>::iterator taskConnBeginIter = m_taskConnSet.lower_bound(taskConns);
    set<CwxAppTaskBoardTaskConns>::iterator taskConnEndIter = taskConnBeginIter;
    //remove from conn tasks
    while(taskConnEndIter != m_taskConnSet.end())
    {
        if (taskConnEndIter->m_uiTaskId == pTask->getTaskId())
        {
            m_connTaskMap.erase(CwxAppTaskBoardConnTasks(taskConnEndIter->m_uiConnId, taskConnEndIter->m_uiTaskId));
            taskConnEndIter++;
            continue;
        }
        break;
    }
    //remove from task conns
    if (taskConnBeginIter != taskConnEndIter)
    {
        m_taskConnSet.erase(taskConnBeginIter, taskConnEndIter);
    }
    return pTask;
}


CWX_UINT8 CwxAppTaskBoard::dispatchEvent(CwxAppTaskBoardTask* pTask, CwxAppTss* pThrEnv)
{
    bool                  bTimeout;
    bool                  bAppendMsg;
    CWX_UINT32             uiConnId;
    list<CwxMsgBlock*>     failSendMsgList;
    list<CwxMsgBlock*>     recvMsgList;
    list<CwxAppTaskBoardConnInfo*> endclosedConns;
    list<CwxMsgBlock*>::iterator iter_event;
    list<CwxAppTaskBoardConnInfo*>::iterator iter_conn;
    CwxMsgBlock* msg = NULL;
    map<CWX_UINT32/*conn_id*/, CWX_UINT8/*state:0:used; 1:finish, 2:closed*/> connStateMap;
    map<CWX_UINT32/*conn_id*/, CWX_UINT8/*state:0:used; 1:finish, 2:closed*/>::iterator iter_connState;

    while(1)
    {
        //fetch dispacth event
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            iter_connState = connStateMap.begin();
            while(iter_connState != connStateMap.end())
            {
                if (0 == iter_connState->second)
                {//conn used
                    _addConnTask(iter_connState->first, pTask);
                }
                else
                {//conn finish or conn closed
                    _removeConnTask(iter_connState->first, pTask->getTaskId());
                }
                iter_connState++;
            }
            pTask->fetchWaitingMsg(bTimeout, failSendMsgList, recvMsgList, endclosedConns);
            //if not finish and no event, return.
            if (!pTask->isFinish() && !bTimeout && failSendMsgList.empty() && recvMsgList.empty() && endclosedConns.empty())
            {//no new event
                CWX_UINT8 ucTaskState = pTask->getTaskState();
                pTask->m_bLooked = false;
                return ucTaskState;
            }
            //if finish, remove task
            if (pTask->isFinish()) _remove(pTask->getTaskId());
        }
        connStateMap.clear();

        //dispatch end send event
        if (!endclosedConns.empty())
        {
            iter_conn = endclosedConns.begin();
            while(iter_conn != endclosedConns.end())
            {
                msg = (*iter_conn)->getMsg();
                if (msg)
                {
                    uiConnId = msg->event().getConnId();
                    bAppendMsg = true;
                    pTask->noticeEndSendMsg(msg, pThrEnv, bAppendMsg);
                    connStateMap[uiConnId] = bAppendMsg?0:1;
                    if (!msg) (*iter_conn)->setMsg(NULL);
                    delete *iter_conn;
                    iter_conn = endclosedConns.erase(iter_conn);
                }
                else
                {
                    iter_conn ++;
                }
            }
        }
        //dispatch recv msg event
        if (!recvMsgList.empty())
        {
            iter_event = recvMsgList.begin();
            while(iter_event != recvMsgList.end())
            {
                msg = *iter_event;
                uiConnId = msg->event().getConnId();
                bAppendMsg = false;
                pTask->noticeRecvMsg(msg, pThrEnv, bAppendMsg);
                connStateMap[uiConnId] = bAppendMsg?0:1;
                if (msg) CwxMsgBlockAlloc::free(msg);
                iter_event ++;
            }
            recvMsgList.clear();
        }
        //dispatch failure msg event
        if (!failSendMsgList.empty())
        {
            iter_event = failSendMsgList.begin();
            while(iter_event != failSendMsgList.end())
            {
                msg = *iter_event;
                pTask->noticeFailSendMsg(msg, pThrEnv);
                if (msg) CwxMsgBlockAlloc::free(msg);
                iter_event ++;
            }
            failSendMsgList.clear();
        }
        //dispatch closed msg event
        if (!endclosedConns.empty())
        {
            iter_conn = endclosedConns.begin();
            while(iter_conn != endclosedConns.end())
            {
                pTask->noticeConnClosed((*iter_conn)->getSvrId(), (*iter_conn)->getHostId(), (*iter_conn)->getConnId(), pThrEnv);
                connStateMap[(*iter_conn)->getConnId()] = 2;
                delete *iter_conn;
                iter_conn ++;
            }
            endclosedConns.clear();
        }

        if (bTimeout)
        {
            pTask->noticeTimeout(pThrEnv);
        }

        if (pTask->isFinish())
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            _remove(pTask->getTaskId());
            return CwxAppTaskBoardTask::TASK_STATE_FINISH;
        }
    }
}

CWINUX_END_NAMESPACE

