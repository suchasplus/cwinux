#include "CwxAppThreadPool.h"
#include "CwxAppLogger.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE
///构造函数
CwxAppThreadPool::CwxAppThreadPool(CwxAppFramework* pApp,///<app对象
                 CWX_UINT16 unGroupId,///<线程池的thread-group
                 CWX_UINT16 unThreadNum,///<线程池中线程的数量
                 CWX_UINT32 uiDeathCheckMsgWaterMask,///<线程的状态监测的排队消息门限
                 CWX_UINT32 uiDeathCheckUpdateWaterMask///<线程失效的无状态更新的时间门限
                 ):CwxAppTpi(unGroupId, unThreadNum)
{
    m_pApp = pApp;
    m_arrTssEnv = NULL;
    m_uiTheadDeathMsgWaterMask = uiDeathCheckMsgWaterMask;
    m_uiThreadDeathUpdateWaterMask = uiDeathCheckUpdateWaterMask;
    m_msgQueue = new CwxMsgQueue(1024*1024*200, 1024*1024*200);
    m_tidArr = new pthread_t[unThreadNum];
    memset(m_tidArr, 0x00, sizeof(pthread_t) * unThreadNum);
}

///析构函数
CwxAppThreadPool::~CwxAppThreadPool()
{
    if (m_arrTssEnv) delete []m_arrTssEnv;
    if (m_tidArr) delete [] m_tidArr;
    if (m_msgQueue) delete m_msgQueue;
}

int CwxAppThreadPool::start(CwxAppTss** pThrEnv, size_t stack_size)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    if (m_pApp->getThreadPoolMgr()->isExist(getGroupId()))
    {
        CWX_ERROR(("Thread group[%u] exist.", getGroupId()));
        return -1;
    }
    m_pApp->getThreadPoolMgr()->add(getGroupId(), this);
    if (pThrEnv)
    {
        m_arrTssEnv = pThrEnv;
    }
    else
    {
        m_arrTssEnv = new CwxAppTss*[getThreadNum()];
        memset(m_arrTssEnv, 0x00, sizeof(CwxAppTss*) * getThreadNum());
    }
    memset(m_tidArr, 0x00, sizeof(pthread_t) * getThreadNum());
    for (CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (0 != CwxAppThread::spawn(
            threadFunc,
            this,
            THREAD_NEW_LWP | THREAD_JOINABLE | THREAD_INHERIT_SCHED,
            &m_tidArr[i],
            CWX_DEFAULT_THREAD_PRIORITY,
            NULL,
            stack_size        
            ))
        {
           _stop();
           return -1;
        }
    }
    return 0;
}

void CwxAppThreadPool::stop()
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    _stop();
}

bool CwxAppThreadPool::isDeath() 
{
    time_t ttNow = time(NULL);
    if (getQueuedMsgNum() > m_uiTheadDeathMsgWaterMask)
    {
        for (CWX_UINT16 i=0; i<getThreadNum(); i++)
        {
            if ( m_arrTssEnv[i] && (ttNow - m_arrTssEnv[i]->getThreadInfo()->getUpdateTime() > (int)m_uiThreadDeathUpdateWaterMask))
            {
                return true;
            }
        }
    }
    return false;
}

bool CwxAppThreadPool::isStop() 
{
    return m_msgQueue->getState() == CwxMsgQueue::DEACTIVATED;
}

CwxAppTss* CwxAppThreadPool::getTss(CWX_UINT16 unThreadIndex)
{
    if (unThreadIndex >= getThreadNum()) return NULL;
    return m_arrTssEnv[unThreadIndex];
}

///锁住整个线程池。返回值0：成功；-1：失败
int CwxAppThreadPool::lock()
{
    return m_msgQueue->lock().acquire();
}
///解锁这个线程池。返回值0：成功；-1：失败
int CwxAppThreadPool::unlock()
{
    return m_msgQueue->lock().release();
}


int CwxAppThreadPool::onThreadCreated(CWX_UINT16 unGroup, CWX_UINT16 unThreadId, CwxAppTss*& pThrEnv)
{
    if (!pThrEnv)
    {
        pThrEnv = new CwxAppTss(new CwxAppTssInfo());
    }
    pThrEnv->getThreadInfo()->setThreadGroup(unGroup);
    pThrEnv->getThreadInfo()->setThreadNo(unThreadId);
    if (!m_pApp->getThreadPoolMgr()->addTss(pThrEnv))
    {
        delete pThrEnv;
        pThrEnv = m_pApp->getThreadPoolMgr()->getTss(unGroup, unThreadId);
    }
    return 0;
}

void CwxAppThreadPool::onThreadClosed(CwxAppTss*& )
{
    //CwxAppTss::unRegTss();
}

void CwxAppThreadPool::threadMain(CwxAppTss* pThrEnv)
{
    CWX_UINT16 unThreadId = 0;
    CWX_UINT16 i;
    for (i=0; i<getThreadNum(); i++)
    {
        if (pThrEnv == m_arrTssEnv[i])
        {
           unThreadId = i;
           break;
        }
    }
    CWX_ASSERT(i == unThreadId);
    time_t ttTime = time(NULL);
    if (0 != onThreadCreated(getGroupId(), unThreadId, pThrEnv)) return ;
    m_arrTssEnv[unThreadId] = pThrEnv;
    pThrEnv->getThreadInfo()->setStopped(false);
    CwxAppTss::regTss(pThrEnv);
    pThrEnv->getThreadInfo()->setThreadId(CwxAppThread::self());
    pThrEnv->getThreadInfo()->setStartTime(ttTime);
    pThrEnv->getThreadInfo()->setUpdateTime(ttTime);
    pThrEnv->getThreadInfo()->setQueuedMsgNum(0);
    pThrEnv->getThreadInfo()->setRecvMsgNum(0);
    {
        int iRet;
        time_t ttTime = time(NULL);
        CwxMsgBlock* block = NULL;
        CWX_UINT32 uiEventType = 0;
        CWX_UINT32 uiSvrId = 0;
        pThrEnv->getThreadInfo()->setBlocked(true);
        while((iRet = this->pop(block)) != -1)
        {//block until has query message
            pThrEnv->getThreadInfo()->setBlocked(false);
            ttTime = time(NULL);
            pThrEnv->getThreadInfo()->setUpdateTime(ttTime);
            pThrEnv->getThreadInfo()->setQueuedMsgNum(iRet);
            pThrEnv->getThreadInfo()->incRecvMsgNum();
            iRet = 0;
            uiEventType = block->event().getEvent();
            uiSvrId = block->event().getSvrId();
            if (!this->m_pApp->getCommander().dispatch(block, pThrEnv, iRet))
            {
                CWX_DEBUG(("No handle to deal with event: event_type=%u, svr_id=%u", uiEventType, uiSvrId));
            }
            if (1 != iRet)
            {
                if (0 == iRet)
                {
                    CWX_DEBUG(("No care the event for  event_type=%u, svr_id=%u", uiEventType, uiSvrId));
                }
                else
                {
                    CWX_DEBUG(("Failure to act the event for  event_type=%u, svr_id=%u", uiEventType, uiSvrId));
                }
            }
            if (block) CwxMsgBlockAlloc::free(block);
            pThrEnv->getThreadInfo()->setBlocked(true);
        }
    }

    onThreadClosed(pThrEnv);
    pThrEnv->getThreadInfo()->setStopped(true);

}

void CwxAppThreadPool::_stop()
{
    m_msgQueue->deactivate();
    for(CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (0 == (int)m_tidArr[0]) break;
        CwxAppThread::join(m_tidArr[i], NULL);
    }
    m_msgQueue->flush();
    m_pApp->getThreadPoolMgr()->remove(getGroupId());
}

void* CwxAppThreadPool::threadFunc(void *thread)
{
    CwxAppThreadPool* pThread = (CwxAppThreadPool*)thread;
    int index = -1;
    {
        CwxMutexGuard<CwxMutexLock> lock(&pThread->m_lock);
        for (CWX_UINT16 i=0; i<pThread->getThreadNum(); i++)
        {
            if(CwxAppThread::equal(CwxAppThread::self(), pThread->m_tidArr[i]))
            {
                index = i;
                break;
            }
        }

    }
    CWX_ASSERT(-1 != index);
    pThread->threadMain(pThread->m_arrTssEnv[index]);
    return pThread;
}


CWINUX_END_NAMESPACE
