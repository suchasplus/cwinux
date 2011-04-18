#include "CwxAppHandler4Channel.h"
#include "CwxAppChannel.h"

CWINUX_BEGIN_NAMESPACE


CwxAppHandler4Channel::CwxAppHandler4Channel(CwxAppChannel *channel)
:CwxAppHandler4Base(NULL)
{
    m_uiSendByte = 0;
    m_curSndingMsg = 0;
    m_waitSendMsgHead = NULL;
    m_waitSendMsgTail = NULL;
    m_uiRecvHeadLen = 0;
    m_uiRecvDataLen = 0;
    m_recvMsgData = NULL;
    m_channel = channel;
}

///析构函数
CwxAppHandler4Channel::~CwxAppHandler4Channel()
{
    clear();
    if (getHandle() != CWX_INVALID_HANDLE)
    {
        ::close(getHandle());
        setHandle(CWX_INVALID_HANDLE);
    }
}


int CwxAppHandler4Channel::open (void * )
{
    if (0 != channel()->registerHandler(getHandle(),
        this,
        CwxAppHandler4Base::READ_MASK))
    {
        CWX_ERROR(("Failure to register conn[%d] for read_mask",
            getHandle()));
        return -1;
    }
    return 0;
}

int CwxAppHandler4Channel::close(CWX_HANDLE)
{
    channel()->removeHandler(this);
    delete this;
    return 0;
}


/**
@brief 接受连接上的到达数据
@param [in] handle 连接的handle
@return -1：关闭的连接； 0：接收数据成功
*/
int CwxAppHandler4Channel::handle_event(int event, CWX_HANDLE)
{
    if (CWX_CHECK_ATTR(event, CwxAppHandler4Base::WRITE_MASK))
    {
        if (0 != handle_output()) return -1;
    }
    if (CWX_CHECK_ATTR(event, CwxAppHandler4Base::READ_MASK))
    {
        if (0 != handle_input()) return -1;
    }
    if (CWX_CHECK_ATTR(event, CwxAppHandler4Base::TIMEOUT_MASK))
    {
        handle_timeout();
    }
    return 0;
}


int CwxAppHandler4Channel::handle_output ()
{
    int result = 0;
    CWX_UINT32 uiNum = 0;
    bool bCloseConn = false;
    bool bReconn = false;
    CWX_UINT32 uiReconnDelay = 0;
    // The list had better not be empty, otherwise there's a bug!
    while(1)
    {
        if (NULL == this->m_curSndingMsg)
        {
            CWX_ASSERT(m_uiSendByte==0);
            while(1)
            {
                result = this->getNextMsg();
                if (0 == result) return this->cancelWakeup();
                if (CWX_CHECK_ATTR(this->m_curSndingMsg->send_ctrl().getMsgAttr(), CwxMsgSendCtrl::BEGIN_NOTICE))
                {
                    if (-1 == this->getApp()->onStartSendMsg(m_curSndingMsg, *this))
                    {
                        CwxMsgBlockAlloc::free(this->m_curSndingMsg);
                        this->m_curSndingMsg = NULL;
                        continue; //next msg;
                    }
                }//end if (this->m_curSndingMsg->IsBeginNotice())
                //it's a msg which need be sent.
                break; //break while
            }//end while
        }
        //has message
        result = this->nonBlockSend();
        if (0 == result)
        {// Partial send.
            CWX_ASSERT (errno == EWOULDBLOCK);
            break;
        }
        else if (1 == result)
        {//finish
            bCloseConn = CWX_CHECK_ATTR(this->m_curSndingMsg->send_ctrl().getMsgAttr(), CwxMsgSendCtrl::CLOSE_NOTICE);
            bReconn = CWX_CHECK_ATTR(this->m_curSndingMsg->send_ctrl().getMsgAttr(), CwxMsgSendCtrl::RECONN_NOTICE);
            if (bReconn) uiReconnDelay = m_curSndingMsg->send_ctrl().getReconnDelay();
            this->m_uiSendByte = 0;
            this->m_conn.setWaitingMsgNum(m_conn.getWaitingMsgNum()-1);
            this->m_conn.setLastSendMsgTime(time(NULL));
            if (CWX_CHECK_ATTR(this->m_curSndingMsg->send_ctrl().getMsgAttr(), CwxMsgSendCtrl::FINISH_NOTICE)){
                CWX_UINT32 uiResume = this->getApp()->onEndSendMsg(m_curSndingMsg, *this);
                if (CwxMsgSendCtrl::RESUME_CONN == uiResume)
                {
                    if (0 != reactor()->resumeHandler(this, CwxAppHandler4Base::READ_MASK))
                    {
                        CWX_ERROR(("Failure to resume handler with conn_id[%u]", m_conn.getConnId()));
                        return -1;
                    }
                }
                else if (CwxMsgSendCtrl::SUSPEND_CONN == uiResume)
                {
                    if (0 != reactor()->suspendHandler(this, CwxAppHandler4Base::READ_MASK))
                    {
                        CWX_ERROR(("Failure to suspend handler with conn_id[%u]", m_conn.getConnId()));
                        return -1;
                    }
                }
            }
            if (bCloseConn)
            {
                m_conn.setActiveClose(true);
                result = -1;
                break;
            }
            else if (bReconn)
            {
                m_conn.setReconn(true);
                m_conn.setReconnDelay(uiReconnDelay);
                result = -1;
                break;
            }
            if (m_curSndingMsg)
            {
                CwxMsgBlockAlloc::free(m_curSndingMsg);
                this->m_curSndingMsg = NULL;
            }
        }
        else
        {//failure
            if (m_curSndingMsg->send_ctrl().isFailNotice())
            {
                this->getApp()->onFailSendMsg(m_curSndingMsg);
            }
            if (m_curSndingMsg) CwxMsgBlockAlloc::free(m_curSndingMsg);
            this->m_curSndingMsg = NULL;
            break;
        }
        uiNum++;
        if (uiNum > m_conn.getContinueSendNum()) break;
    };
    return -1==result?-1:0;
}

/***
desc:
	recv data from socket.
param: 
	handle of the event.
return:
	-1 : failure, handle_close is invoked.
	0  : success.
***/
int CwxAppHandler4Channel::handle_input ()
{
    bool bSuspend = false;
    if (getApp()->isStopped()) return 0;
    ssize_t recv_size = 0;
    ssize_t need_size = 0;
    int result = 0;
    while(1)
    {
        if (this->m_conn.isRawData())
        {//recv raw data
            need_size = this->getApp()->getRawRecvBufSize();
            recv_size = CwxSocket::recv(getHandle(), this->getApp()->getRawRecvBuf(), need_size);
            if (recv_size <=0 )
            { //error or signal
                if ((0 == recv_size) || (errno != EWOULDBLOCK))
                {
                    return -1; //error
                }
                else
                {//signal
                    m_conn.setContinueRecvNum(0);
                    return 0;
                }
            }
            CWX_ASSERT(!m_recvMsgData);
            this->m_recvMsgData = CwxMsgBlockAlloc::malloc(recv_size);
            memcpy(m_recvMsgData->wr_ptr(), this->getApp()->getRawRecvBuf(), recv_size);
            //move write pointer
            this->m_recvMsgData->wr_ptr(recv_size);
            bSuspend = false;
            result = this->getApp()->recvMessage(m_header, this->m_recvMsgData, *this, bSuspend);
            if (bSuspend)
            {
                if (0 != reactor()->suspendHandler(this, CwxAppHandler4Base::READ_MASK))
                {
                    CWX_ERROR(("Failure to suspend handler with conn_id[%u]", m_conn.getConnId()));
                    return -1;
                }
            }
            this->m_recvMsgData = NULL;
        }
        else
        {//not recv raw data
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
                        m_conn.setContinueRecvNum(0);
                        return 0;
                    }
                }
                this->m_uiRecvHeadLen += recv_size;
                if (recv_size < need_size)
                {
                    m_conn.setContinueRecvNum(0);
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
                        m_conn.setContinueRecvNum(0);
                        return 0;
                    }
                }
                //move write pointer
                this->m_recvMsgData->wr_ptr(recv_size);
                this->m_uiRecvDataLen += recv_size;
                if (recv_size < need_size)
                {
                    m_conn.setContinueRecvNum(0);
                    return 0;
                }
            }
            //notice recieving a msg.
            if (!this->m_recvMsgData) this->m_recvMsgData = CwxMsgBlockAlloc::malloc(0);
            bSuspend = false;
            result = this->getApp()->recvMessage(m_header, this->m_recvMsgData, *this, bSuspend);
            if (bSuspend)
            {
                if (0 != reactor()->suspendHandler(this, CwxAppHandler4Base::READ_MASK))
                {
                    CWX_ERROR(("Failure to suspend handler with conn_id[%u]", m_conn.getConnId()));
                    return -1;
                }
            }

            this->m_recvMsgData = NULL;
            this->m_uiRecvHeadLen = 0;
            this->m_uiRecvDataLen = 0;
        }
        if (0 < result)
        {///recv next msg
            CWX_ASSERT(m_uiRecvDataLen==0);
            CWX_ASSERT(m_uiRecvHeadLen==0);
            m_conn.setContinueRecvNum(m_conn.getContinueRecvNum()+1);
            continue;
        }
        else
        {
            CWX_ASSERT(m_uiRecvDataLen==0);
            CWX_ASSERT(m_uiRecvHeadLen==0);
            m_conn.setContinueRecvNum(0);
            break;
        }
    }
    return result>=0?0:-1;
}

/***
desc:
	timeout evnet, it exec the noticeReconnect operation.
return:
	0  : success.
***/
void CwxAppHandler4Channel::handle_timeout()
{
}

///由于没有消息发送，使连接的发送监测休眠.返回值， -1: failure, 0: success
int CwxAppHandler4Channel::cancelWakeup()
{
    if(-1 == this->channel()->suspendHandler(this, CwxAppHandler4Base::WRITE_MASK)){
        CWX_ERROR(("Failure to cancel wakeup a connection. conn[%d]", getHandle()));
        return -1;
    }
    return 0;
}

///唤醒连接的可写监控，以发送未发送完毕的数据.返回值， -1:failure； 0:success。
int CwxAppHandler4Channel::wakeUp()
{
    if(-1 == this->CwxAppChannel()->resumeHandler(this, CwxAppHandler4Base::WRITE_MASK)){
        CWX_ERROR(("Failure to wakeup a connection. conn[%d]", getHandle()));
        return -1;
    }
    return 0;
}

CWINUX_END_NAMESPACE

