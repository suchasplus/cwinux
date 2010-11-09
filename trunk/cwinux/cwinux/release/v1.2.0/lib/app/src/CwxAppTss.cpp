#include "CwxAppTss.h"
#include "CwxDate.h"
CWINUX_BEGIN_NAMESPACE


pthread_key_t CwxAppTss::m_tssKey;
bool CwxAppTss::m_bInit = false;


bool CwxAppTss::packThreadInfo(CwxPackageWriter& threadInfoPack, char* szErr2K)
{
    bool bRet = false;
    string strTmp;
    threadInfoPack.beginPack();
    do
    {
        if (!threadInfoPack.addKeyValue("thread_group", m_pThreadInfo->getThreadGroup())) break;
        if (!threadInfoPack.addKeyValue("thread_no", m_pThreadInfo->getThreadNo())) break;
        if (!threadInfoPack.addKeyValue("thread_id", (CWX_UINT64)m_pThreadInfo->getThreadId())) break;
        strTmp = m_pThreadInfo->isStopped()?"true":"false";
        if (!threadInfoPack.addKeyValue("thread_stop", strTmp.c_str(), strTmp.length(), false)) break;
        strTmp = m_pThreadInfo->isBlocked()?"false":"true";
        if (!threadInfoPack.addKeyValue("thread_wait_lock", strTmp.c_str(), strTmp.length(), false)) break;
        if (!threadInfoPack.addKeyValue("queue msg num", m_pThreadInfo->getQueuedMsgNum())) break;
        CwxDate::getDate(m_pThreadInfo->getStartTime(), strTmp);
        if (!threadInfoPack.addKeyValue("thread_start_time", strTmp.c_str(), strTmp.length(), false)) break;
        CwxDate::getDate(m_pThreadInfo->getUpdateTime(), strTmp);
        if (!threadInfoPack.addKeyValue("thread_update_time", strTmp.c_str(), strTmp.length(), false)) break;
        if (!threadInfoPack.addKeyValue("thread_recv_msg_num", m_pThreadInfo->getRecvMsgNum())) break;
        if (!threadInfoPack.pack()) break;
        bRet = true;
    }while(0);
    if (!bRet) strcpy(szErr2K, threadInfoPack.getErrMsg());
    return bRet;
}

int CwxAppTss::initTss()
{
    if (!m_bInit){
        m_bInit = true;
        return pthread_key_create(&m_tssKey, NULL);
    }
    return 0;

}

int CwxAppTss::regTss(CwxAppTss* pThrEnv)
{
    if (!m_bInit)
    {
        if (0 != initTss()) return -1;
    }
    void* pObj = pthread_getspecific(m_tssKey);
    if (pObj) delete ((CwxAppTss*)pObj);
    return pthread_setspecific(m_tssKey, pThrEnv);
}

int CwxAppTss::unRegTss()
{
    if (m_bInit)  pthread_key_delete(m_tssKey);
    return 0;
}


CwxAppTss* CwxAppTss::instance()
{
    if (!m_bInit)
    {
        if (0 != initTss()) return NULL;
    }
    void * pObj = NULL;
    pObj = pthread_getspecific(m_tssKey);
    if (!pObj)
    {
        pObj = new CwxAppTss();
        regTss((CwxAppTss*)pObj);
    }
    return (CwxAppTss*)pObj;
}


CWINUX_END_NAMESPACE

