#ifndef __CWX_APP_HANDLER_4_BASE_H__
#define __CWX_APP_HANDLER_4_BASE_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppHandler4Base.h
@brief Event handler的基类
@author cwinux@gmail.com
@version 0.1
@date 2010-08-01
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxLockGuard.h"
#include "CwxMutexLock.h"
#include "CwxMsgBlock.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppReactor;
class CwxAppEpoll;
class CwxAppFramework;
class CWX_API CwxAppHandler4Base{
public:
    ///定义事件类型
    enum{
        TIMEOUT_MASK = 0x01,
        READ_MASK = 0x02,
        WRITE_MASK = 0x04,
        SIGNAL_MASK = 0x08,
        PERSIST_MASK = 0x10,
        PREAD_MASK = READ_MASK|PERSIST_MASK,
        RW_MASK = READ_MASK|WRITE_MASK,
        IO_MASK = RW_MASK|TIMEOUT_MASK|PERSIST_MASK,
        ALL_EVENTS_MASK =  TIMEOUT_MASK|
                          READ_MASK|
                          WRITE_MASK|
                          SIGNAL_MASK|
                          PERSIST_MASK
    };
public :
    ///析构函数.
    virtual ~CwxAppHandler4Base (void);
    /**
    @brief handler open。做handler的初始化及reactor的注册操作。
    @param [in] arg  Handler的初始化参数，各个子类自己定义
    @return -1：失败； 0：成功
    */
    virtual int open (void * arg= 0)=0;
    /**
    @brief Handler的事件通知回调。
    @param [in] event  发生的事件类型，为TIMEOUT_MASK、READ_MASK、WRITE_MASK、<br>
            SIGNAL_MASK的组合。
    @param [in] handle  发生的事件的handle。
    @return -1：失败，reactor会主动调用close；
            0：成功；
    */
    virtual int handle_event(int event, CWX_HANDLE handle=CWX_INVALID_HANDLE)=0;
    ///handle close
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE)=0;
public:
    ///设置app
    void setApp(CwxAppFramework* app);
    ///获取app 
    CwxAppFramework* getApp();
    ///设置handle的reactor
    void reactor (CwxAppReactor *reactor);
    ///获取handle的reactor.
    CwxAppReactor *reactor(void);
    ///是否注册
    bool isReg() const;
    ///获取reg mask
    int  getRegMask() const;
    ///获取Io handle
    CWX_HANDLE getHandle(void) const;
    ///设置IO handle
    void setHandle (CWX_HANDLE);
    ///设置handle type
    void setType(int type);
    ///获取handle type
    int getType() const;
    ///获取注册类型
    int getRegType() const;
    ///获取超时时间
    CWX_UINT64 getTimeout() const;
    ///获取heap中的index
    int index() const;
    ///获取是否为persist事件
    bool isPersistMask() const;
    ///是否设置了read mask
    bool isReadMask() const;
    ///是否设置了write mask
    bool isWriteMask() const;
    ///是否设置了timeout mask
    bool isTimeoutMask() const;
    ///是否设置了signal mask
    bool isSignalMask() const;
    ///超时比较函数
    bool operator<(CwxAppHandler4Base const& base) const;
protected:
    /// Force CwxAppHandler4Base to be an abstract base class.
    CwxAppHandler4Base (CwxAppFramework* app, CwxAppReactor *reactor);
    friend class CwxAppReactor;
    friend class CwxAppEpoll;
    friend class CwxAppFramework;
    ///设置reg mask
    void setRegMask(int mask);
    ///设置注册状态
    void setReg(bool bReg);
    ///设置注册类型
    void setRegType(int type);
    ///设置heap中的index
    void index(int index);
    ///设置超时时间
    void setTimeout(CWX_UINT64 ullTimeout);
private:
    CwxAppFramework*       m_pApp;
    CwxAppReactor *        m_reactor; ///<reactor对象的指针
    bool                   m_bReg; ///<handler是否已经注册
    int                    m_regMask; ///<handler注册的掩码
    int                    m_regType; ///<handler的reactor注册类型
    CWX_HANDLE             m_handler; ///<事件的io handle
    int                    m_type; ///<event handle type;
    CWX_UINT64             m_ullTimeout; ///<超时的时刻
    int                    m_index;
};

CWINUX_END_NAMESPACE

#include "CwxAppHandler4Base.inl"
#include "CwxPost.h"
#endif
