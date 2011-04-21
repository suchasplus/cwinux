#include "CwxEchoChannelEventHandler.h"

/**
@brief 连接可读事件，返回-1，close()会被调用
@return -1：处理失败，会调用close()； 0：处理成功
*/
int CwxEchoChannelEventHandler::onInput()
{
    ssize_t recv_size = 0;
    ssize_t need_size = 0;
    need_size = CwxMsgHead::MSG_HEAD_LEN - this->m_uiRecvHeadLen;
    if (need_size > 0 )
    {//not get complete head
        recv_size = CwxSocket::recv(getHandle(), this->m_szHeadBuf + this->m_uiRecvHeadLen, need_size);
        if (recv_size <=0 )
        { //error or signal
            if ((0==recv_size) || ((errno != EWOULDBLOCK) && (errno != EINTR)))
            {
                return -1; //error
            }
            else
            {//signal or no data
                return 0;
            }
        }
        this->m_uiRecvHeadLen += recv_size;
        if (recv_size < need_size)
        {
            return 0;
        }
        this->m_szHeadBuf[this->m_uiRecvHeadLen] = 0x00;
        if (!m_header.fromNet(this->m_szHeadBuf))
        {
            CWX_ERROR(("Msg header is error."));
            return -1;
        }
        if (m_header.getDataLen() > 0) this->m_recvMsgData = CwxMsgBlockAlloc::malloc(m_header.getDataLen());
        CWX_ASSERT(this->m_uiRecvDataLen==0);
    }//end  if (need_size > 0)
    //recv data
    need_size = m_header.getDataLen() - this->m_uiRecvDataLen;
    if (need_size > 0)
    {//not get complete data
        recv_size = CwxSocket::recv(getHandle(), this->m_recvMsgData->wr_ptr(), need_size);
        if (recv_size <=0 )
        { //error or signal
            if ((errno != EWOULDBLOCK)&&(errno != EINTR))
            {
                return -1; //error
            }
            else
            {//signal or no data
                return 0;
            }
        }
        //move write pointer
        this->m_recvMsgData->wr_ptr(recv_size);
        this->m_uiRecvDataLen += recv_size;
        if (recv_size < need_size)
        {
            return 0;
        }
    }
    replyMessage();
    //notice recieving a msg.
    if (!this->m_recvMsgData) this->m_recvMsgData = CwxMsgBlockAlloc::malloc(0);
    
    CwxMsgBlockAlloc::free(m_recvMsgData);
    this->m_recvMsgData = NULL;
    this->m_uiRecvHeadLen = 0;
    this->m_uiRecvDataLen = 0;
    return 0;
}
/**
@brief 通知连接关闭。
@return 对于主动连接，1：不从engine中移除注册；0：不从engine中移除注册但不删除handler；-1：从engine中将handle移除并删除。
*/
int CwxEchoChannelEventHandler::onConnClosed()
{
    return -1;
}

void CwxEchoChannelEventHandler::replyMessage()
{
    ///设置echo回复的消息类型，为请求的消息类型+1
    m_recvMsgData->event().getMsgHeader().setMsgType(m_recvMsgData->event().getMsgHeader().getMsgType() + 1);
    ///设置echo回复的数据包长度
    m_recvMsgData->event().getMsgHeader().setDataLen(m_recvMsgData->length());
    ///创建回复的数据包
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::malloc(m_recvMsgData->length() + CwxMsgHead::MSG_HEAD_LEN);
    ///拷贝数据包的包头
    memcpy(pBlock->wr_ptr(), m_recvMsgData->event().getMsgHeader().toNet(), CwxMsgHead::MSG_HEAD_LEN);
    ///滑动block的写指针
    pBlock->wr_ptr(CwxMsgHead::MSG_HEAD_LEN);
    ///拷贝数据包的数据
    memcpy(pBlock->wr_ptr(), m_recvMsgData->rd_ptr(), m_recvMsgData->length());
    ///滑动block的写指针
    pBlock->wr_ptr(m_recvMsgData->length());
    if (!putMsg(pBlock))
    {
        CWX_ERROR(("Failure to put message"));
        CwxMsgBlockAlloc::free(pBlock);
    }
    m_ullMsgNum ++;
    if (m_ullMsgNum && !(m_ullMsgNum%10000))
    {
        char szBuf[64];
        CwxCommon::toString(m_ullMsgNum, szBuf, 10);
        CWX_INFO(("Recv echo message num:%s", szBuf));
    }


}
