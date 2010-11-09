#ifndef __CWX_APP_HANDLER_4_BASE_H__
#define __CWX_APP_HANDLER_4_BASE_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
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
#include <event.h>

CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;
class CwxAppReactor;


class CWX_API CwxAppHandler4Base{
public:
    ///定义事件类型
    enum{
        TIMEOUT_MASK = EV_TIMEOUT,
        READ_MASK = EV_READ,
        WRITE_MASK = EV_WRITE,
        SIGNAL_MASK = EV_SIGNAL,
        PERSIST_MASK = EV_PERSIST,
        PREAD_MASK = READ_MASK|PERSIST_MASK,
        RW_MASK = READ_MASK|WRITE_MASK,
        IO_MASK = RW_MASK|PERSIST_MASK,
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
    ///获取app
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
protected:
    /// Force CwxAppHandler4Base to be an abstract base class.
    CwxAppHandler4Base (CwxAppFramework* pApp, CwxAppReactor *reactor);
    friend class CwxAppReactor;
    ///设置reg mask
    void setRegMask(int mask);
    ///设置注册状态
    void setReg(bool bReg);
    ///设置注册类型
    void setRegType(int type);
private:
    CwxAppFramework*       m_pApp; ///<app对象的指针
    CwxAppReactor *        m_reactor; ///<reactor对象的指针
    bool                  m_bReg; ///<handler是否已经注册
    int                   m_regMask; ///<handler注册的掩码
    int                   m_regType; ///<handler的reactor注册类型
    CWX_HANDLE             m_handler; ///<事件的io handle
    int                   m_type; ///<event handle type;
    struct event           m_event; ///<handler的event对象
};

CWINUX_END_NAMESPACE

#include "CwxAppHandler4Base.inl"
#include "CwxPost.h"
#endif
