#include "CwxAppThreadPoolEx.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE
///构造函数
CwxAppThreadPoolEx::CwxAppThreadPoolEx(CwxAppFramework* pApp,///<app对象
                   CWX_UINT16 unGroupId,///<线程池的thread-group
                   CWX_UINT16 unThreadNum,///<线程池中线程的数量
                   CWX_UINT32 uiDeathCheckMsgWaterMask,///<线程的状态监测的排队消息门限
                   CWX_UINT32 uiDeathCheckUpdateWaterMask///<线程失效的无状态更新的时间门限
                   ): CwxAppTpi(unGroupId, unThreadNum)
{
    m_pApp = pApp;
    m_uiTheadDeathMsgWaterMask = uiDeathCheckMsgWaterMask;
    m_uiThreadDeathUpdateWaterMask = uiDeathCheckUpdateWaterMask;
    m_arrThreadPool = new CwxAppThread*[unThreadNum];
    memset(m_arrThreadPool, 0x00, sizeof(CwxAppThread*) * unThreadNum);
}

///析构函数
CwxAppThreadPoolEx::~CwxAppThreadPoolEx()
{
    if (m_arrThreadPool)
    {
        for (CWX_UINT16 i=0; i<getThreadNum(); i++)
        {
            if (m_arrThreadPool[i]) delete m_arrThreadPool[i];
        }
        delete [] m_arrThreadPool;
    }
}

int CwxAppThreadPoolEx::start(CwxAppTss** pThrEnv, size_t stack_size)
{
    CWX_UINT16 i = 0;
    if (m_pApp->getThreadPoolMgr()->isExist(getGroupId()))
    {
        CWX_ERROR(("Thread group[%u] exist.", getGroupId()));
        return -1;
    }
    m_pApp->getThreadPoolMgr()->add(getGroupId(), this);
    for (i=0; i<getThreadNum(); i++)
    {
        if (m_arrThreadPool[i]) delete m_arrThreadPool[i];
    }
    for (i=0; i<getThreadNum(); i++)
    {
        m_arrThreadPool[i] = onCreateThread(m_pApp, getGroupId(), i, m_uiTheadDeathMsgWaterMask, m_uiThreadDeathUpdateWaterMask);
        if (0 != m_arrThreadPool[i]->start(pThrEnv?pThrEnv[i]:NULL, stack_size))
        {
            return -1;
        }
    }
    if (pThrEnv) delete [] pThrEnv;
    return 0;
}

void CwxAppThreadPoolEx::stop()
{
    for (CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (m_arrThreadPool && m_arrThreadPool[i]) m_arrThreadPool[i]->stop();
    }
}

bool CwxAppThreadPoolEx::isDeath()
{
    for (CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (m_arrThreadPool && m_arrThreadPool[i]) 
        {
            if (m_arrThreadPool[i]->isDeath()) return true;
        }
    }
    return false;
}
bool CwxAppThreadPoolEx::isStop()
{
    for (CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (m_arrThreadPool && m_arrThreadPool[i]) 
        {
            if (m_arrThreadPool[i]->isStop()) return true;
        }
    }
    return false;
}

CwxAppTss* CwxAppThreadPoolEx::getTss(CWX_UINT16 unThreadIndex)
{
    if (m_arrThreadPool && (unThreadIndex<getThreadNum()) && m_arrThreadPool[unThreadIndex])
    {
        return m_arrThreadPool[unThreadIndex]->getTss();
    }
    return NULL;
}
///锁住整个线程池。返回值0：成功；-1：失败
int CwxAppThreadPoolEx::lock()
{
    for (CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (m_arrThreadPool && m_arrThreadPool[i]) 
        {
            if (0 != m_arrThreadPool[i]->lock())
            {
                for (CWX_UINT16 j=0; j<i; j++)
                {
                    if (m_arrThreadPool[j]) m_arrThreadPool[j]->unlock();
                }
                return -1;
            }
        }
    }
    return 0;
}
///解锁这个线程池。返回值0：成功；-1：失败
int CwxAppThreadPoolEx::unlock()
{
    for (CWX_UINT16 i=0; i<getThreadNum(); i++)
    {
        if (m_arrThreadPool && m_arrThreadPool[i]) 
        {
            m_arrThreadPool[i]->unlock();
        }
    }
    return 0;
}

CwxAppThread* CwxAppThreadPoolEx::onCreateThread(CwxAppFramework* pApp, CWX_UINT16 unGroup, CWX_UINT16 unThreadId, CWX_UINT32 uiMsgWaterMask, CWX_UINT32 uiUpdateWaterMask)
{
    return new CwxAppThread(pApp, unGroup, unThreadId, uiMsgWaterMask, uiUpdateWaterMask);
}


CWINUX_END_NAMESPACE
