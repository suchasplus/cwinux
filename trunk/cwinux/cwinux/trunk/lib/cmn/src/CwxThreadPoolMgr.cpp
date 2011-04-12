#include "CwxAppThreadPoolMgr.h"

CWINUX_BEGIN_NAMESPACE
CwxAppThreadPoolMgr::CwxAppThreadPoolMgr()
{
}

CwxAppThreadPoolMgr::~CwxAppThreadPoolMgr()
{
    m_threadPoolMap.clear();
    map<CWX_UINT16, map<CWX_UINT16, CwxAppTss*> >::iterator iter =m_threadPoolTss.begin();
    while(iter != m_threadPoolTss.end())
    {
        map<CWX_UINT16, CwxAppTss*>::iterator iter_tss = iter->second.begin();
        while(iter_tss != iter->second.end())
        {
            delete iter_tss->second;
            iter_tss++;
        }
        iter++;
    }
    m_threadPoolTss.clear();
}

bool CwxAppThreadPoolMgr::add(CWX_UINT16 unGroupId, CwxAppTpi* pThreadPool)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    if (m_threadPoolMap.find(unGroupId) != m_threadPoolMap.end()) return false;
    m_threadPoolMap[unGroupId] = pThreadPool;
    return true;
}

bool CwxAppThreadPoolMgr::remove(CWX_UINT16 unGroupId)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    return m_threadPoolMap.erase(unGroupId)>0?true:false;
}

bool CwxAppThreadPoolMgr::isValid()
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    map<CWX_UINT16, CwxAppTpi*>::iterator iter=m_threadPoolMap.begin();
    while(iter != m_threadPoolMap.end())
    {
        if (iter->second->isDeath()) return false;
        iter++;
    }
    return true;
}

bool CwxAppThreadPoolMgr::isExist(CWX_UINT16 unGroupId)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    return m_threadPoolMap.find(unGroupId) != m_threadPoolMap.end();
}


CWX_UINT16 CwxAppThreadPoolMgr::getNum()
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    return m_threadPoolMap.size();
}

bool CwxAppThreadPoolMgr::addTss(CwxAppTss* pTss)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    CWX_UINT16 unGroup = pTss->getThreadInfo()->getThreadGroup();
    CWX_UINT16 unThreadNo = pTss->getThreadInfo()->getThreadNo();
    map<CWX_UINT16, map<CWX_UINT16, CwxAppTss*> >::iterator iter = m_threadPoolTss.find(unGroup);
    if (iter == m_threadPoolTss.end())
    {
        m_threadPoolTss[unGroup] = map<CWX_UINT16, CwxAppTss*>();
        iter = m_threadPoolTss.find(unGroup);
    }
    if (iter->second.find(unThreadNo) != iter->second.end())
    {
        return false;
    }
    iter->second[unThreadNo] = pTss;
    return true;
}

void CwxAppThreadPoolMgr::getTss(vector<vector<CwxAppTss*> >& arrTss)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    vector<CwxAppTss*> poolTss;
    arrTss.clear();
    map<CWX_UINT16, map<CWX_UINT16, CwxAppTss*> >::iterator iter = m_threadPoolTss.begin();
    while (iter != m_threadPoolTss.end())
    {
        poolTss.clear();
        map<CWX_UINT16, CwxAppTss*>::iterator iter_tss = iter->second.begin();
        while(iter_tss != iter->second.end())
        {
            poolTss.push_back(iter_tss->second);
            iter_tss++;
        }
        arrTss.push_back(poolTss);
        iter++;
    }
}


void CwxAppThreadPoolMgr::getTss(CWX_UINT16 unGroup, vector<CwxAppTss*>& arrTss)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    arrTss.clear();
    map<CWX_UINT16, map<CWX_UINT16, CwxAppTss*> >::iterator iter = m_threadPoolTss.find(unGroup);
    if (iter != m_threadPoolTss.end())
    {
        map<CWX_UINT16, CwxAppTss*>::iterator iter_tss = iter->second.begin();
        while(iter_tss != iter->second.end())
        {
            arrTss.push_back(iter_tss->second);
            iter_tss++;
        }
    }
}

CwxAppTss* CwxAppThreadPoolMgr::getTss(CWX_UINT16 unGroup, CWX_UINT16 unThreadId)
{
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    map<CWX_UINT16, map<CWX_UINT16, CwxAppTss*> >::iterator iter = m_threadPoolTss.find(unGroup);
    if (iter != m_threadPoolTss.end())
    {
        map<CWX_UINT16, CwxAppTss*>::iterator iter_tss = iter->second.find(unThreadId);
        if (iter_tss != iter->second.end()) return iter_tss->second;
    }
    return NULL;
}


CWINUX_END_NAMESPACE
