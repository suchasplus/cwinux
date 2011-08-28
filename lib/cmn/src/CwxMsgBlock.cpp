#include "CwxMsgBlock.h"

CWINUX_BEGIN_NAMESPACE
CwxMutexLock CwxMsgBlockAlloc::m_lock;
CwxMsgBlockAlloc* CwxMsgBlockAlloc::m_pInstance = NULL;

///禁止创建实例
CwxMsgBlockAlloc::CwxMsgBlockAlloc():CwxSingleton("CwxMsgBlockAlloc")
{
    CWX_UINT16 i=0;
    for (i=0; i<CACHE_BLOCK_SIZE_SCALE; i++)
    {
        m_arrCacheNum[i] = 0;
        m_arrCacheBlock[i] = NULL;
    }
}
///禁止释放实例
CwxMsgBlockAlloc::~CwxMsgBlockAlloc()
{
    CWX_UINT16 i;
    CwxMsgBlock* block;
    for (i=0; i<CACHE_BLOCK_SIZE_SCALE; i++)
    {
        while(m_arrCacheBlock[i])
        {
            block = m_arrCacheBlock[i];
            m_arrCacheBlock[i] = block->m_next;
            delete block;
        }
        m_arrCacheNum[i] = 0;
        m_arrCacheBlock[i] = NULL;
    }
}

CwxMsgBlock* CwxMsgBlockAlloc::malloc(size_t size)
{
    CwxMsgBlock* pMsg = NULL;
    CwxMsgBlockAlloc* pAlloc = CwxMsgBlockAlloc::instance();
    int index = -1;
    if (size<=CACHE_MIN_BLOCK_SIZE)
    {
        size = CACHE_MIN_BLOCK_SIZE;
        index = 0;
    }
    else if (size>CACHE_MAX_BLOCK_SIZE)
    {
        size = ((size>>BLOCK_ALIGN_BIT) + 1)<<BLOCK_ALIGN_BIT;
    }
    else
    {
        size_t newSize = size;
        newSize>>=CACHE_MIN_BLOCK_BITS;
        CWX_ASSERT(newSize > 0);
        while(newSize)
        {
            index++;
            newSize>>=1;
        }
        newSize = (CACHE_MIN_BLOCK_SIZE<<index);
        if (newSize < size)
        {
            newSize <<=1;
            index++;
        }
        CWX_ASSERT(newSize>=size);
        CWX_ASSERT(index<CACHE_BLOCK_SIZE_SCALE);
        size = newSize;
    }
    if (-1 != index)
    {
        CwxMutexGuard<CwxMutexLock> lock(&m_lock);
        if (!pAlloc->m_arrCacheNum[index])
        {
            pMsg = new CwxMsgBlock(size);
            pMsg->m_bFree = false;
            return pMsg;
        }
        pAlloc->m_arrCacheNum[index]--;
        CwxMsgBlock* pMsg = pAlloc->m_arrCacheBlock[index];
        pAlloc->m_arrCacheBlock[index] = pAlloc->m_arrCacheBlock[index]->m_next;
        pMsg->m_next = NULL;
        pMsg->reset();
        pMsg->m_bFree = false;
        return pMsg;
    }
    pMsg = new CwxMsgBlock(size);
    pMsg->m_bFree = false;
    return pMsg;
}

void CwxMsgBlockAlloc::free(CwxMsgBlock* block)
{
    CWX_ASSERT(!block->m_bFree);
    CwxMsgBlockAlloc* pAlloc = CwxMsgBlockAlloc::instance();
    size_t size = block->capacity();
    if (size <CACHE_MIN_BLOCK_SIZE)
    {
        delete block;
        return;
    }
    else if (size <=CACHE_MAX_BLOCK_SIZE)
    {
        int index = -1;
        CWX_UINT32 uiBlockSize = 1;
        size >>= CACHE_MIN_BLOCK_BITS;
        while(size)
        {
            index++;
            size>>=1;
        }
        uiBlockSize <<= (CACHE_MIN_BLOCK_BITS + index);
        {
            CwxMutexGuard<CwxMutexLock> lock(&m_lock);
            block->m_bFree = true;
            if ((pAlloc->m_arrCacheNum[index]>MAX_SCALE_FREE_BLOCK_NUM) ||
                (((uiBlockSize * pAlloc->m_arrCacheNum[index])>>20) > MAX_SCALE_FREE_MEM_MSIZE))
            {
                delete block;
            }
            else
            {
                pAlloc->m_arrCacheNum[index]++;
/*                {
                    //debug
                    CwxMsgBlock* pItem = m_arrCacheBlock[index];
                    while(pItem){
                        CWX_ASSERT(pItem != block);
                        pItem = pItem->m_next;
                    }
                    
                }*/
                block->m_next = pAlloc->m_arrCacheBlock[index];
                pAlloc->m_arrCacheBlock[index] = block;
            }
        }
        return;
    }
    delete block;
}


CwxMsgBlock* CwxMsgBlockAlloc::pack(CwxMsgHead& header,
                            char const* szData,
                            CWX_UINT32 uiDateLen)
{
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(CwxMsgHead::MSG_HEAD_LEN + uiDateLen);
    memcpy(pBlock->wr_ptr(), header.toNet(), CwxMsgHead::MSG_HEAD_LEN);
    pBlock->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    if (szData)
    {
        memcpy(pBlock->wr_ptr(), szData, uiDateLen);
        pBlock->wr_ptr(uiDateLen);
    }
    return pBlock;
}


CWINUX_END_NAMESPACE
