#ifndef __CWX_ECHO_TB_CLIENT_APP_H__
#define __CWX_ECHO_TB_CLIENT_APP_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
#include "CwxAppFramework.h"
#include "CwxEchoTbClientConfig.h"
#include "CwxEchoTbEventHandler.h"
#include "CwxEchoTbClientTask.h"
#include "CwxAppThreadPoolEx.h"

#define ECHO_TB_CLIENT_APP_VERSION "1.0"
#define ECHO_TB_CLIENT_MODIFY_DATE "2010-08-29"

CWINUX_USING_NAMESPACE;

///echo的压力测试app
class CwxEchoTbClientApp : public CwxAppFramework
{
public:
    enum
    {
        LOG_FILE_SIZE = 30, ///<每个循环运行日志文件的MBTYE
        LOG_FILE_NUM = 7,///<循环日志文件的数量
        SVR_TYPE_ECHO_BASE = CwxAppFramework::SVR_TYPE_USER_START ///<echo查询的svr-id类型
    };
    enum
    {
        SEND_MSG_TYPE = 1, ///<echo发送的消息类型
        RECV_MSG_TYPE =2 ///<echo回复的消息类型
    };

    ///构造函数
	CwxEchoTbClientApp();
    ///析构函数
	virtual ~CwxEchoTbClientApp();
    //初始化app, -1:failure, 0 success;
    virtual int init(int argc, char** argv);
public:
    //时钟响应函数
    virtual void onTime(CwxTimeValue const& current);
    //信号响应函数
    virtual void onSignal(int signum);
    //echo连接建立函数
    virtual int onConnCreated(CwxAppHandler4Msg& conn, bool& bSuspendConn, bool& bSuspendListen);
    //echo返回的响应函数
    virtual int onRecvMsg(CwxMsgBlock* msg, CwxAppHandler4Msg const& conn, CwxMsgHead const& header, bool& bSuspendConn);
    //连接关闭
    virtual int onConnClosed(CwxAppHandler4Msg const& conn);
    //消息发送完毕
    virtual CWX_UINT32 onEndSendMsg(CwxMsgBlock*& msg,
        CwxAppHandler4Msg const& conn);
    //消息发送失败
    virtual void onFailSendMsg(CwxMsgBlock*& msg);
public:
    CwxEchoTbClientConfig const& getConfig() const { return m_config;}
    CWX_UINT16 getEchoGroupNum() const { return m_unEchoGroupNum;}
    CWX_UINT32 getEchoGroupSvrId(CWX_UINT16 unGroup) { return SVR_TYPE_ECHO_BASE + unGroup;}
    CwxMsgBlock* packEchoMsg(CWX_UINT32 uiSvrId, CWX_UINT32 uiTaskId)
    {
        CwxMsgHead header;
        ///设置echo的消息类型
        header.setMsgType(SEND_MSG_TYPE);
        ///设置echo的数据包长度
        header.setDataLen(m_config.m_uiDataSize);
        ///设置echo数据包的taskid，此使用发送的数据序列号，当前没用
        header.setTaskId(uiTaskId);
        ///分配发送消息包的block
        CwxMsgBlock* pBlock = CwxMsgBlockAlloc::pack(header, m_szBuf100K, m_config.m_uiDataSize);
        ///设置消息的发送方式
        ///设置消息的svr-id
        pBlock->send_ctrl().setSvrId(uiSvrId);
        ///设置消息的host-id
        pBlock->send_ctrl().setHostId(0);
        ///设置消息发送的连接id
        pBlock->send_ctrl().setConnId(0);
        ///设置消息发送的user-data
        pBlock->send_ctrl().setUserData(NULL);
        ///设置消息发送阶段的行为，包括开始发送是否通知、发送完成是否通知、发送失败是否通知
        pBlock->send_ctrl().setMsgAttr(CwxMsgSendCtrl::FAIL_FINISH_RETRY);
        ///设置taskid
        pBlock->event().setTaskId(uiTaskId);
        return pBlock;
    }
    CWX_UINT32 getNextTaskId()
    {
        CwxMutexGuard<CwxMutexLock>  lock(&m_lock);
        m_uiTaskId++;
        if (!m_uiTaskId) m_uiTaskId = 1;
        return m_uiTaskId;
    }
    void incRecvMsg()
    {
        CwxMutexGuard<CwxMutexLock>  lock(&m_lock);
        m_uiRecvNum++;
        if (!(m_uiRecvNum % 10000))
        {
            CWX_INFO(("Send query msg num:%u", m_uiRecvNum));
        }
    }

protected:
    //init the Enviroment before run.0:success, -1:failure.
	virtual int initRunEnv();
    virtual void destroy();
private:
    CwxEchoTbClientConfig          m_config; ///<配置文件对象
    CwxEchoTbEventHandler*         m_pEventHandle; ///事件处理handler
    char                           m_szBuf100K[64 * 1024*1024+1]; ///<发送的echo数据buf及内容
    CWX_UINT32                     m_uiTaskId;
    CWX_UINT32                     m_uiRecvNum;///<接收到echo回复的数量
    CwxMutexLock                   m_lock; ///<m_uiRecvNum的保护锁
    CWX_UINT16                     m_unEchoGroupNum; ///echo服务的分组数量
    CwxAppThreadPoolEx*            m_threadPool;///<线程池对象
};

#endif

