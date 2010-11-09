#ifndef __CWX_APP_THREAD_POOL_EX_H__
#define __CWX_APP_THREAD_POOL_EX_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppThreadPoolEx.h
@brief 可以控制单个线程的线程池对象实现
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxMsgBlock.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxAppTpi.h"
#include "CwxAppThread.h"
#include "CwxAppTss.h"


CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;
/**
@class CwxAppThreadPoolEx
@brief 可以控制线程池每个线程的线程池。
       此线程池本身就是一个生产、消费者队列。
*/
class CWX_API CwxAppThreadPoolEx:public CwxAppTpi
{
public:
    enum{
        DEATH_CHECK_MSG_WATER_MASK = 5, ///<线程的状态监测的排队消息门限
        DEATH_CHECK_UPDATE_WATER_MASK = 30 ///<线程失效的无状态更新的时间门限
    };
public :
    ///构造函数
    CwxAppThreadPoolEx(CwxAppFramework* pApp,///<app对象
        CWX_UINT16 unGroupId,///<线程池的thread-group
        CWX_UINT16 unThreadNum,///<线程池中线程的数量
        CWX_UINT32 uiDeathCheckMsgWaterMask=DEATH_CHECK_MSG_WATER_MASK,///<线程的状态监测的排队消息门限
        CWX_UINT32 uiDeathCheckUpdateWaterMask=DEATH_CHECK_UPDATE_WATER_MASK///<线程失效的无状态更新的时间门限
        );
    ///析构函数
    ~CwxAppThreadPoolEx();
public:
    /**
    @brief 启动线程池
    @param [in] pThrEnv 线程池的线程Tss的数组，若不指定，可通过线程的onThreadCreated创建。
    @param [in] stack_size 线程堆栈的大小，若为0，则采用系统默认大小。
    @return -1：失败； 0：成功
    */
    virtual int start(CwxAppTss** pThrEnv=NULL, size_t stack_size = 0);
    ///停止线程池
    virtual void stop();
    ///check thread 是否死锁。若需要改变检查的规则，则重载此API
    virtual bool isDeath();
    ///check thread 是否停止。若需要改变检查的规则，则重载此API
    virtual bool isStop();
    ///获取线程的TSS，及Thread env
    virtual CwxAppTss* getTss(CWX_UINT16 unThreadIndex);
    ///锁住整个线程池。返回值0：成功；-1：失败
    virtual int lock();
    ///解锁这个线程池。返回值0：成功；-1：失败
    virtual int unlock();
public:
    /**
    @brief 通知线程创建，此需要创建线程对象
    @param [in] pApp 线程的APP。
    @param [in] unGroup 线程的线程组。
    @param [in] unThreadId 线程在线程组中的序号。
    @param [in] uiMsgWaterMask 线程死锁的监测的排队消息数量的阀值。
    @param [in] uiUpdateWaterMask 线程死锁的监测的线程状态没更新时间的阀值。
    @return NULL：失败； 否则，为创建的线程对象
    */
    virtual CwxAppThread* onCreateThread(CwxAppFramework* pApp, CWX_UINT16 unGroup, CWX_UINT16 unThreadId, CWX_UINT32 uiMsgWaterMask, CWX_UINT32 uiUpdateWaterMask);
public:
    ///获取线程的消息队列排队消息数
    inline size_t getQueuedMsgNum();
    ///获取线程的消息队列排队消息数
    inline size_t getQueuedMsgNum(CWX_UINT16 unThreadIndex);
    /**
    @brief 往线程的消息队列添加一个新消息。
    @param [in] pMsg append的消息
    @param [in] uiThread 消息的线程队列，内部会根据Thread的数量，计算出所属的线程。
    @return -1：失败；>=0为对应线程的队列中排队的消息数量
    */
    inline int append(CwxMsgBlock* pMsg, CWX_UINT32 uiThread);
private:
    CwxAppFramework*        m_pApp;///<架构的APP
    CwxAppThread**          m_arrThreadPool;///<线程的数组
    CWX_UINT32              m_uiTheadDeathMsgWaterMask;///<线程death检查的队列消息排队阀值
    CWX_UINT32              m_uiThreadDeathUpdateWaterMask;///<线程death检查的线程无状态更新的时间阀值
};


CWINUX_END_NAMESPACE

#include "CwxAppThreadPoolEx.inl"
#include "CwxPost.h"

#endif

