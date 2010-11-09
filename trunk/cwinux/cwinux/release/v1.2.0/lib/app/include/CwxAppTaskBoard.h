#ifndef __CWX_APP_TASK_BOARD_H__
#define __CWX_APP_TASK_BOARD_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppTaskBoard.h
@brief 异步任务消息管理对象定义
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxMutexLock.h"
#include "CwxLockGuard.h"
#include "CwxTypePoolEx.h"
#include "CwxMsgBlock.h"
#include "CwxAppTss.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppTaskBoard;
/**
@class CwxAppTaskBoardConnInfo
@brief Task的连接关闭及消息发送完毕事件的封装对象，实现以上两个消息的缓存。
*/
class CWX_API CwxAppTaskBoardConnInfo
{
public:
    ///构造函数
    CwxAppTaskBoardConnInfo(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId,
        CwxMsgBlock* msg);
    ///析构函数
    ~CwxAppTaskBoardConnInfo();
public:
    ///获取封装事件的svr-id
    inline CWX_UINT32 getSvrId() const;
    ///获取封装事件的host-id
    inline CWX_UINT32 getHostId() const;
    ///获取封装事件的conn-id
    inline CWX_UINT32 getConnId() const;
    ///获取封装事件的msg
    inline CwxMsgBlock* getMsg();
    ///设置封装事件的msg
    inline void setMsg(CwxMsgBlock* msg);
private:
    CWX_UINT32     m_uiSvrId;///<事件的svr-id
    CWX_UINT32     m_uiHostId;///<事件的host-id
    CWX_UINT32     m_uiConnId;///<事件的conn-id
    CwxMsgBlock*   m_msg;///<事件的msg

};

/**
@class CwxAppTaskBoardTask
@brief 异步任务的Task对象的基类，实现Task的数据及事件的处理,控制Task的状态转移
*/
class CWX_API CwxAppTaskBoardTask
{
public:
    ///任务的状态，其他的状态用户自己定义
    enum{
        TASK_STATE_INIT = 0,///<初始化状态
        TASK_STATE_FINISH = 1,///<完成状态
        TASK_STATE_USER = 2 ///<用户的Task状态的开始值
    };
public:
    ///构造函数
    CwxAppTaskBoardTask(CwxAppTaskBoard* pTaskBoard);
    ///析构函数
    virtual ~CwxAppTaskBoardTask();
public:
    /**
    @brief 通知Task已经超时
    @param [in] pThrEnv 调用线程的Thread-env
    @return void
    */
    virtual void noticeTimeout(CwxAppTss* pThrEnv) = 0;
    /**
    @brief 通知Task的收到一个数据包。
    @param [in] msg 收到的消息
    @param [in] pThrEnv 调用线程的Thread-env
    @param [out] bConnAppendMsg 收到消息的连接上，是否还有待接收的其他消息。true：是；false：没有,默认为false。
    @return void
    */
    virtual void noticeRecvMsg(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        bool& bConnAppendMsg) = 0;
    /**
    @brief 通知Task往外发送的一个数据包发送失败。
    @param [in] msg 收到的消息
    @param [in] pThrEnv 调用线程的Thread-env
    @return void
    */
    virtual void noticeFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv) = 0;
    /**
    @brief 通知Task通过某条连接，发送了一个数据包。
    @param [in] msg 发送的数据包的信息
    @param [in] pThrEnv 调用线程的Thread-env
    @param [out] bConnAppendMsg 发送消息的连接上，是否有等待回复的消息。true：是；false：没有，默认为true。
    @return void
    */
    virtual void noticeEndSendMsg(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        bool& bConnAppendMsg) = 0;
    /**
    @brief 通知Task等待回复消息的一条连接关闭。
    @param [in] uiSvrId 关闭连接的SVR-ID
    @param [in] uiHostId 关闭连接的HOST-ID
    @param [in] uiConnId 关闭连接的CONN-ID
    @param [in] pThrEnv 调用线程的Thread-env
    @return void
    */
    virtual void noticeConnClosed(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId,
        CwxAppTss* pThrEnv) = 0;
    /**
    @brief 激活Task。在Task启动前，Task有Task的创建线程所拥有。
           在启动前，Task可以接受自己的异步消息，但不能处理。
           此时有Taskboard的noticeActiveTask()接口调用的。
    @param [in] pThrEnv 调用线程的Thread-env
    @return 0:成功；-1：失败，在失败的时候，Task从Taskboard中移除并设置为完成状态。
    */
    virtual int noticeActive(CwxAppTss* pThrEnv)=0;
    /**
    @brief 执行Task。在调用此API前，Task在Taskboard中不存在，也就是说对别的线程不可见。
           Task要么是刚创建状态，要么是完成了前一个阶段的处理，处于完成状态。
           通过此接口，由Task自己控制自己的step的跳转而外界无需关系Task的类型及处理过程。
    @param [in] pThrEnv 调用线程的Thread-env
    @return void
    */
    virtual void execute(CwxAppTss* pThrEnv)
    {
        pThrEnv = NULL;
    }
public:
    ///获取Task的Taskboard
    inline CwxAppTaskBoard* getTaskBoard()
    {
        return m_pTaskBoard;
    }
    ///获取Task的task id
    inline CWX_UINT32 getTaskId() const;
    ///设置Task的task id
    inline void setTaskId(CWX_UINT32 id);
    ///获取Task超时的时间点
    inline CWX_UINT64 const& getTimeoutValue() const;
    ///设置Task超时的时间点
    inline void setTimeoutValue(CWX_UINT64 ullTimestamp);
    ///获取Task的状态
    inline CWX_UINT8 getTaskState() const;
    ///设置Task的状态
    inline void setTaskState(CWX_UINT8 state);
    ///Check Task是否完成，若完成，则需要脱离Taskboard的管理
    inline bool isFinish() const;
    ///Check Task是否超时，超时也意味着完成。若超时，则需要脱离Taskboard的管理。
    inline bool isTimeout() const;
    ///检查 Task是否在移除状态，此是有Taskboard的remove API设置的。若处于移除状态，则需要脱离Taskboard的管理。
    inline bool isWaitingRemove() const;
private:
    ///根据ullNow的时间点，检查任务是否超时。如超时，则设置超时状态。若超时状态已经设置，则无论ullNow,都是超时。
    inline bool checkTimeout(CWX_UINT64 const& ullNow);
    ///Task是否被lock。若被lock的话，说明有别的线程正在处理此Task。
    inline bool isLocked() const;
    ///清空Task状态、缓存的事件等信息
    inline void clearBase();
    ///获取Task缓存的事件
    inline void fetchWaitingMsg(bool& bTimeout,
        list<CwxMsgBlock*>& failSendMsgs,
        list<CwxMsgBlock*>& recvMsgs,
        list<CwxAppTaskBoardConnInfo*>& endclosedConnList);
    ///缓存一个发送的数据包通过连接发送完毕的事件
    inline void addEndSendMsgEvent(CwxMsgBlock* msg);
    ///缓存一个数据包发送失败的事件
    inline void addFailSendMsgEvent(CwxMsgBlock* msg);
    ///缓存收到一个数据包的事件
    inline void addRecvMsgEvent(CwxMsgBlock* msg);
    ///缓存等待接受数据的一条连接关闭的的事件
    inline void addClosedConnEvent(CWX_UINT32 uiSvrId,
        CWX_UINT32 uiHostId,
        CWX_UINT32 uiConnId);
    friend class CwxAppTaskBoard;
private:
    CWX_UINT32            m_uiTaskId; ///<Task的ID
    CWX_UINT64            m_ullTimeoutStamp;///<Task超时的时间点
    CWX_UINT8             m_ucTaskState;///<Task的状态
    CwxAppTaskBoardTask*   m_next;///<Task的下一个Task
    CwxAppTaskBoardTask*   m_prev;///<Task的前一个Task
    bool                  m_bLooked;///<Task是否处于锁状态，禁止操作
    bool                  m_bWaitingRemove;///<Task是否处于从Taskboard移除状态
    bool                  m_bTimeout;///<Task是否已经超时
    list<CwxMsgBlock*>      m_failSendMsgList;///<消息通过连接发送、发送失败的事件缓存队列
    list<CwxMsgBlock*>      m_recvMsgList;///<接收到的数据的缓存队列
    list<CwxAppTaskBoardConnInfo*> m_sendCloseConnList;///<连接关闭的缓存队列
    CwxAppTaskBoard*       m_pTaskBoard;
};


/**
@class CwxAppTaskBoardConnTasks
@brief 一条连接与Task关系的对象，用于实现管理一条连接与那些Task相关。
       当连接关闭的时候，会影响这些Task
*/
class CwxAppTaskBoardConnTasks
{
public:
    ///构造函数
    CwxAppTaskBoardConnTasks(CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId);
    ///析构函数
    ~CwxAppTaskBoardConnTasks();
    ///拷贝构造
    CwxAppTaskBoardConnTasks(CwxAppTaskBoardConnTasks const& item);
    ///赋值操作
    CwxAppTaskBoardConnTasks& operator=(CwxAppTaskBoardConnTasks const& item);
    ///对象的小于比较操作符
    inline bool operator<(CwxAppTaskBoardConnTasks const& item) const;
    ///对象的等于操作符
    inline bool operator==(CwxAppTaskBoardConnTasks const& item) const;
private:
    friend class CwxAppTaskBoard;
private:
    CWX_UINT32      m_uiConnId;///<连接ID
    CWX_UINT32      m_uiTaskId;///<与连接相关的Task ID
};

/**
@class CwxAppTaskBoardTaskConns
@brief 一个Task与连接关系的对象，用于实现管理一个Task与哪些连接相关。
当一个Task完成的时候，会清理那些连接上的Task
*/
class CwxAppTaskBoardTaskConns
{
public:
    ///构造函数
    CwxAppTaskBoardTaskConns(CWX_UINT32 uiTaskId, CWX_UINT32 uiConnId);
    ///析构函数
    ~CwxAppTaskBoardTaskConns();
    ///拷贝构造
    CwxAppTaskBoardTaskConns(CwxAppTaskBoardTaskConns const& item);
    ///赋值操作
    CwxAppTaskBoardTaskConns& operator=(CwxAppTaskBoardTaskConns const& item);
    ///小于操作符
    inline bool operator<(CwxAppTaskBoardTaskConns const& item) const;
    ///等于操作符
    inline bool operator==(CwxAppTaskBoardTaskConns const& item) const;
private:
    friend class CwxAppTaskBoard;
private:
    CWX_UINT32      m_uiTaskId; ///<Task id
    CWX_UINT32      m_uiConnId; ///<与Task相关的连接ID
};

/**
@class CwxAppTaskBoard
@brief Task的管理对象，实现Task任务与其等待的消息的关联。帮助Task完成状态转义。
       Task是通过noticeActiveTask()接口加入到Taskboard中的。当Task完成时，
       会自动脱离Taskboard。
*/
class CWX_API CwxAppTaskBoard
{
public:
    ///Task的hash-map类型定义
    typedef hash_map<CWX_UINT32/*taskid*/, CwxAppTaskBoardTask*, CwxNumHash<CWX_UINT32> > CWX_APP_TASK_MAP;
public:
    /**
    @brief 构造函数。
    @param [in] uiMaxTaskNum Taskboard管理的最大Task数，此用户控制Task hash的大小
    */
    CwxAppTaskBoard(CWX_UINT32 uiMaxTaskNum=1024);
    ///析构函数
    ~CwxAppTaskBoard();

public:
    /**
    @brief Taskboard的初始化。
    @return -1：失败；0：成功
    */
    int  init();
    /**
    @brief 检查指定Taskid的task是否存在。
    @param [in] uiTaskId Task id
    @return true：存在；false：不存在
    */
    bool isExist(CWX_UINT32 uiTaskId);
    /**
    @brief 将一个Task从Taskoard移除。
    @param [in] uiTaskId 要移除的Task的Task id
    @param [out] pFinishTask 移除Task的返回指针。
    @return -1：Task不存在，pFinishTask返回NULL；
            0：Task由于正被别的线程操作，处于lock状态，只设置了移除标志，无法移除，pFinishTask返回NULL。
            1：成功移除，移除的Task通过pFinishTask返回。
    */
    int remove(CWX_UINT32 uiTaskId, CwxAppTaskBoardTask*& pFinishTask);
    /**
    @brief 往Taskboard中添加一个Task并将Task激活。
    @param [in] pTask 要添加并激活的Task对象。
    @param [in] pThrEnv 调用线程的Thread-Env。
    @return -1：Taskboard中，存在与添加Task id相同的task；
    0：Task已经由Taskboard接管处理，调用线程不能再对Task进行任何处理。
    1：Task已经完成，不再有Taskboard管理，处于调用线程的控制之下。
    */
    int noticeActiveTask(CwxAppTaskBoardTask* pTask, CwxAppTss* pThrEnv);
    /**
    @brief 检查Taskboard中的超时Task。
    @param [in] pThrEnv 调用线程的Thread-Env。
    @param [out] finishTasks 已经超时的Task的列表。
    @return void。
    */
    void noticeCheckTimeout(CwxAppTss* pThrEnv, list<CwxAppTaskBoardTask*>& finishTasks);
    /**
    @brief 通知Taskboard，uiTaskId的任务收到一个通信数据包，此可能将是此Task完成。
    @param [in] uiTaskId 收到消息的Task的Task Id。
    @param [in] msg 收到的消息。
    @param [in] pThrEnv 调用线程的Thread-Env。
    @param [out] pFinishTask 如果收到消息的Task因为此消息而完成，则返回。
    @return -1：Task不存在；
            0：Task已经接受此消息，但Task没有完成，还在等待别的消息。
            1：Task已经完成，不再有Taskboard管理，处于调用线程的控制之下。
    */
    int noticeRecvMsg(CWX_UINT32 uiTaskId,
        CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        CwxAppTaskBoardTask*& pFinishTask);
    /**
    @brief 通知Taskboard，uiTaskId的Task的一个消息发送失败。此可能将是此Task完成。
    @param [in] uiTaskId 发送消息失败的Task的Task Id。
    @param [in] msg 发送失败的消息的信息。
    @param [in] pThrEnv 调用线程的Thread-Env。
    @param [out] pFinishTask 如果Task因为发送失败而完成，则返回。
    @return -1：Task不存在；
            0：Task已经接受此消息，但Task没有完成，还在等待别的消息。
            1：Task已经完成，不再有Taskboard管理，处于调用线程的控制之下。
    */
    int noticeFailSendMsg(CWX_UINT32 uiTaskId,
        CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv,
        CwxAppTaskBoardTask*& pFinishTask);
    /**
    @brief 通知Taskboard，uiTaskId的Task的一个消息有一条连接发送完毕。
    @param [in] uiTaskId Task的Task Id。
    @param [in] msg 发送连接相关的信息。
    @param [in] pThrEnv 调用线程的Thread-Env。
    @param [out] pFinishTask 如果Task完成，则返回。
    @return -1：Task不存在；
            0：Task已经接受此消息，但Task没有完成，还在等待别的消息。
            1：Task已经完成，不再有Taskboard管理，处于调用线程的控制之下。
    */
    int noticeEndSendMsg(CWX_UINT32 uiTaskId,
        CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv, 
        CwxAppTaskBoardTask*& pFinishTask);
    /**
    @brief 通知Taskboard，一条连接关闭。
    @param [in] msg 关闭连接相关的信息。
    @param [in] pThrEnv 调用线程的Thread-Env。
    @param [out] finishTasks 因为连接关闭而完成的任务。
    @return void
    */
    void noticeConnClosed(CwxMsgBlock*& msg,
        CwxAppTss* pThrEnv, 
        list<CwxAppTaskBoardTask*>& finishTasks);
    ///获取Taskboard管理的任务数量
    inline CWX_UINT32 getTaskNum() const ;
    ///获取taskboard中conn-tasks的map中元素数目，为了调试
    inline int getConnTasksMapSize() const;
    ///获取taskboard中task-conns的set中元素数目，为了调试
    inline int getTaskConnsMapSize() const;
    ///清空Taskboard
    void reset();
private:
    ///不带锁的判断一个任务是否存在
    inline bool _isExist(CWX_UINT32 uiTaskId);
    ///不带锁往Taskboard添加一个Task
    bool _addTask(CwxAppTaskBoardTask* pTask);
    ///不带锁根据TaskId获取对应的Task
    inline CwxAppTaskBoardTask* _getTask(CWX_UINT32 uiTaskId);
    ///不带锁从Taskboard删除一个task;
    CwxAppTaskBoardTask* _remove(CWX_UINT32 uiTaskId);
    ///分发Task上缓存的消息
    CWX_UINT8 dispatchEvent(CwxAppTaskBoardTask* pTask, CwxAppTss* pThrEnv);
    ///不带锁，添加连接的一个Task
    inline void _addConnTask(CWX_UINT32 uiConnId, CwxAppTaskBoardTask* pTask);
    ///不带锁，删除连接的一个Task
    inline void _removeConnTask(CWX_UINT32 uiConnId, CWX_UINT32 uiTaskId);
    ///不带锁，删除一条连接上的所有Task
    inline void _removeConnTask(CWX_UINT32 uiConnId);
private:
    CwxAppTaskBoardTask*      m_pTaskHead;///<管理的Task的链表的头
    CwxAppTaskBoardTask*      m_pTaskTail;///<管理的Task的链表的尾
    CWX_APP_TASK_MAP*         m_pTaskMap;///<Task的Map索引
    map<CwxAppTaskBoardConnTasks, CwxAppTaskBoardTask*> m_connTaskMap;///<连接与Task的对应Map
    set<CwxAppTaskBoardTaskConns> m_taskConnSet;///<Task与Conn的对应Map
	CwxMutexLock        m_lock;///<Taskboard的同步锁
    CWX_UINT32         m_uiMaxTaskNum;///<管理Task的最大数量
};

CWINUX_END_NAMESPACE
#include "CwxAppTaskBoard.inl"

#include "CwxPost.h"


#endif
