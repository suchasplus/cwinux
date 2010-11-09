#ifndef __CWX_APP_CONN_INFO_H__
#define __CWX_APP_CONN_INFO_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
*@file  CwxAppConnInfo.h
*@brief 定义了CwxAppConnInfo
*@author cwinux@gmail.com
*@version 0.1
*@date  2010-07-30
*@warning  无.
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxMsgHead.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxAppLogger.h"

CWINUX_BEGIN_NAMESPACE
class CwxAppHandler4Msg;
class CwxAppPai;
class CwxAppHostInfo;
class CwxAppSvrInfo;
/**
@class CwxAppConnInfo
@brief 连接对象，保存连接的状态、类型及数据收发等信息。
*/
class CWX_API CwxAppConnInfo{
public:
    ///连接状态定义
    enum{
        IDLE = 0,      ///<初始化的状态
        CONNECTING,    ///<连接中的状态，此有connector设置，可转换为ESTABLISHING、FAILED
        TIMEOUT,       ///<等待超时状态，也就是等待下一次重新连接。可转换为ESTABLISHING、FAILED
        ESTABLISHING,  ///<等待建立，可转换为ESTABLISHED、FAILED。
        ESTABLISHED,   ///<连接已经建立的状态，可转换为FAILED
        FAILED         ///<连接失败的状态，可转换为CONNECTING、TIMEOUT
    };
public:
    /**
    @brief 构造函数
    */
    CwxAppConnInfo();
    ///析构函数
    ~CwxAppConnInfo();
public:
    ///获取连接的SVR-ID
    CWX_UINT32 getSvrId() const; 
    ///设置连接的SVR-ID
    void setSvrId(CWX_UINT32 uiSvrId);

    ///获取连接的HOST-ID
    CWX_UINT32 getHostId() const;
    ///设置连接的HOST-ID
    void setHostId(CWX_UINT32 uiHostId);

    ///获取连接的连接ID
    CWX_UINT32 getConnId() const; 
    ///设置连接的连接ID
    void setConnId(CWX_UINT32 uiConnId);

    ///获取被动连接的Listen ID
    CWX_UINT32 getListenId() const;
    ///设置被动连接的Listen ID
    void setListenId(CWX_UINT32 uiListenId);

    ///获取连接的状态
    CWX_UINT16 getState() const;
    ///设置连接的状态
    void setState(CWX_UINT16 unState);

    ///获取连接的创建时间
    time_t getCreateTime() const;
    ///设置连接的创建时间
    void setCreateTime(time_t ttTime);
    ///获取连续失败连接的次数
    CWX_UINT32 getFailConnNum() const;
    ///设置连接失败连接次数
    void setFailConnNum(CWX_UINT32 uiNum);
    ///增加连接失败连接次数
    CWX_UINT32 incFailConnNum();
    ///获取失效主动连接最小重连接时间间隔
    CWX_UINT16 getMinRetryInternal() const;
    ///设置失效主动连接最小重连接时间间隔
    void setMinRetryInternal(CWX_UINT16 unInternal);
    ///获取失效主动连接最大重连接时间间隔
    CWX_UINT16 getMaxRetryInternal() const;
    ///设置失效主动连接最大重连接时间间隔
    void setMaxRetryInternal(CWX_UINT16 unInternal);
    ///获取连接是否为主动连接
    bool isActiveConn() const;
    ///设置为主动连接
    void setActiveConn(bool bActive);
    ///获取连接是否主动关闭
    bool isActiveClose() const;
    ///设置连接时主动关闭
    void setActiveClose(bool bActive);
    ///获取连接的数据包是否有包头
    bool isRawData() const ;
    ///设置连接的数据包是raw格式
    void setRawData(bool bRaw);
    ///获取无包头连接的数据接收BUF
    CWX_UINT32  getRawRecvLen() const;
    ///获取无包头连接的数据接收buf
    void setRawRecvLen(CWX_UINT32 uiLen);
    ///获取连接是否需要主动监测KEEP-ALIVE
    bool isKeepalive() const;
    ///设置连接是否需要主动监测KEEP-ALIVE
    void setKeepalive(bool bKeepAlive);
    ///获取连接最新收到消息的时间
    time_t  getLastRecvMsgTime() const;
    ///设置连接最新收到消息的时间
    void setLastRecvMsgTime(time_t ttTime);
    ///获取连接最新发送消息的时间
    time_t  getLastSendMsgTime() const;
    ///设置连接最新发送消息的时间
    void setLastSendMsgTime(time_t ttTime);
    ///获取KEEP-ALIVE连接最新发送KEEP-ALIVE的时间
    time_t  getKeepAliveSendTime() const;
    ///设置KEEP-ALIVE连接最新发送KEEP-ALIVE的时间
    void setKeepAliveSendTime(time_t ttTime);
    ///获取是否收到了连接的KEEP-ALIVE回复
    bool isKeepAliveReply() const;
    ///设置是否收到了连接的KEEP-ALIVE回复
    void setKeepAliveReply(bool bReply);
    ///获取连接的用户数据
    void*  getUserData() const;
    ///设置连接的用户数据
    void setUserData(void* userData);
    ///获取连接的协议适配对象
    CwxAppPai* getPai();
    ///设置连接的协议适配对象
    void setPai(CwxAppPai* pAdapter);
    ///获取连接等待发送的最大消息的数量，0表示没有限制
    CWX_UINT32 getMaxWaitingMsgNum() const;
    ///设置连接最大的等待发送的消息数量，默认0表示没有限制
    void setMaxWaitingMsgNum(CWX_UINT32 uiNum=0);
    ///判断是否连接待发送队列已满
    bool isWaitingMsgQueueFull() const;
    ///获取连接等待发送的消息的数量
    CWX_UINT32 getWaitingMsgNum() const;
    ///设置连接等待发送的消息的数量
    void setWaitingMsgNum(CWX_UINT32 uiNum);
    ///增加连接等待发送的消息的数量
    CWX_UINT32 incWaitingMsgNum();
    ///减少连接等待发送的消息的数量
    CWX_UINT32 decWaitingMsgNum();
    ///获取连接已经连续接收到的消息包的数量
    CWX_UINT32 getContinueRecvNum() const;
    ///设置连接已经连续收到的消息包的数量
    void setContinueRecvNum(CWX_UINT32 uiNum);
    ///获取连续发送的消息数量
    CWX_UINT32 getContinueSendNum() const;
    ///设置连续发送的消息数量
    void setContinueSendNum(CWX_UINT32 uiNum);
    ///判断断开的连接是否需要重连
    bool isNeedReconnect() const;
    ///是否调用CwxAppFramework::onCreate
    bool isInvokeCreate() const;
    ///设置是否调用CwxAppFramework::onCreate
    void setInvokeCreate(bool bInvoke);
    ///是否重新连接
    bool isReconn() const;
    ///设置是否重连
    void setReconn(bool bReconnect);
    ///获取重新连接延时的毫秒数
    CWX_UINT32 getReconnDelay() const;
    ///设置重新连接延时的毫秒数
    void setReconnDelay(CWX_UINT32 uiDelay);
    ///获取连接对应的handler
    CwxAppHandler4Msg* getHandler();
    ///设置连接对应的handler
    void setHandler(CwxAppHandler4Msg*  pHandler);
    ///获取parent
    CwxAppHostInfo* getParent();
    ///设置parent
    void setParent(CwxAppHostInfo* parent);

public:
    ///将连接对象的状态信息复位
    void reset();
private:
    CWX_UINT32         m_uiSvrId;  ///<svr id
    CWX_UINT32         m_uiHostId; ///<host id
    CWX_UINT32         m_uiConnId;  ///<connection id
    CWX_UINT32         m_uiListenId; ///<accept connection's  acceptor ID. for passive conn.
    CWX_UINT16         m_unState; ///<connection state.
    time_t             m_ttCreateTime;///<connection's create time
    CWX_UINT16          m_unMinRetryInternal; ///<min re-connect internal
    CWX_UINT16         m_unMaxRetryInternal; ///<max re-connect internal
    CWX_UINT32          m_uiFailConnNum; ///<count for failure connect
    bool               m_bActiveConn; ///< sign for active connection.
    bool               m_bActiveClose; ///< sign for close connection by user.
    bool               m_bRawData; ///< sign for raw data connection
    CWX_UINT32          m_uiRawRecvLen; ///<max len for per-recieve
    bool               m_bKeepAlive; ///<sign for keep alive
    time_t             m_ttLastRecvMsgTime;///<last recv msg time
    time_t             m_ttLastSendMsgTime;///<last send msg time
    time_t             m_ttKeepAliveSendTime;///<keep-alive send time
    bool               m_bKeepAliveReply; ///<sign for waiting keep-alive reply
    CwxAppPai*         m_pPai; ///<协议适配对象
    void*              m_pUserData; ///<user dat for connection
    CWX_UINT32         m_uiContinueRecvNum; ///<conintue recv msg num
    CWX_UINT32         m_uiContinueSendNum; ///<连续发送的最大数量
    CWX_UINT32         m_uiMaxWaitingMsgNum;
    CWX_UINT32         m_uiWaitingMsgNum;///<waiting msg num
    bool               m_bInvokeCreate; ///<是否在open的时候，调用CwxAppFramework::onCreate，默认调用
    bool               m_bReconn; ///<是否是重连
    CWX_UINT32         m_uiReconnDelay; ///<重连延时的毫秒数
    CwxAppHandler4Msg*  m_pHandler; ///<连接对应的Handler
    CwxAppHostInfo*    m_parent; ///<连接的parent
};


/**
@class CwxAppHostInfo
@brief Framework的主机对象
*/
class CWX_API CwxAppHostInfo{
public:
    /**
    @brief 构造函数
    @param [in] uiSvrId 主机的svr id
    @param [in] uiHostId 主机的host id
    @param [in] parent 主机的SVR 对象
    */
    CwxAppHostInfo(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CwxAppSvrInfo* parent);
public:
    ///Get Svr id
    inline CWX_UINT32 getSvrId() const;
    ///Get host id
    inline CWX_UINT32 getHostId() const;
    ///Get conn nun
    inline CWX_UINT32 getConnNum() const;
    ///获取是否能够发送消息
    inline bool isEnableSendMsg() const;
    ///Get WaitMsgNum() 
    inline CWX_UINT32 getWaitingMsgNum() const;
    ///Get SndMsgNum
    inline CWX_UINT64 getSndMsgNum() const;
    ///Get RecvMsgNum
    inline CWX_UINT64 getRecvMsgNum() const;
    ///get RecentSndMsgNum
    inline CWX_UINT32 getRecentSndMsgNum() const;
    ///get RecentRecvMsgNum
    inline CWX_UINT32 getRecentRecvMsgNum() const;
    ///get RecentClosedCount
    inline CWX_UINT32 getRecentClosedCount() const;
    ///select connection for send msg
    inline CwxAppConnInfo* selectConn();
    ///Get parent
    inline CwxAppSvrInfo * getParent();
    ///Get host's valid connection
    inline vector<CwxAppConnInfo*>* GetHostConn();
public:
    ///添加一个有效的主机连接
    inline void addConn(CwxAppConnInfo* conn);
    ///移除一个主机连接
    inline void removeConn(CwxAppConnInfo* conn);
    ///通知主机发送了一个消息
    inline void sendOneMsg();
    ///通知主机接收到一个要发送的消息
    inline void recvOneMsg();
    ///通知主机多了一个排队发送的消息
    inline void waitingOneMsg();
    ///计算主机的负载
    inline void calcHostLoad();
private:
    vector<CwxAppConnInfo*>   m_hostConn;///<host's connection
    CWX_UINT32                m_uiSvrId; ///<service id;
    CWX_UINT32                m_uiHostId; ///<host id
    CWX_UINT64                m_ullSndMsgNum; ///<send msg number
    CWX_UINT64                m_ullRecvMsgNum; ///<recv msg number
    CWX_UINT32                m_uiWaitingMsgNum; ///<msg num for waiting sending
    CWX_UINT32                m_uiRecentSndMsgNum;//recent one minute's sent msg num
    CWX_UINT32                m_uiRecentRecvMsgNum;//recent one minute's recv msg num
    CWX_UINT32                m_uiRecentClosedCount;//recent one minute's closed connection
    CWX_UINT32                m_uiRand;///<连接选择的随机数
    CwxAppSvrInfo*            m_parent; //parent
};

/**
@class CwxAppSvrInfo
@brief Framework的SVR对象
*/
class CWX_API CwxAppSvrInfo
{
public:
    ///构造函数
    CwxAppSvrInfo(CWX_UINT32 uiSvrId);
public:
    ///Get svr id
    inline CWX_UINT32 getSvrId() const;
    ///Get HostNum
    inline CWX_UINT32 getHostNum() const;
    ///选择一台主机，进行消息的发送
    inline CwxAppHostInfo* selectHost();
    ///获取svr下的主机列表
    inline vector<CwxAppHostInfo*>& getHosts();
public:
    ///增加一台新主机
    inline void addHost(CwxAppHostInfo* host);
    ///重新计算svr下不同主机的负载
    inline void calcHostLoad();
private:
    vector<CwxAppHostInfo*>     m_svrHost;///<SVR下的主机信息
    CWX_UINT32                 m_uiSvrId; ///<svr id
    CWX_UINT32                 m_uiRand;///<主机选择的随机基数
};

CWINUX_END_NAMESPACE
#include "CwxAppConnInfo.inl"
#include "CwxPost.h"
#endif

