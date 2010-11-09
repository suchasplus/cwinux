#ifndef __CWX_APP_HANDLER_4_TIME_H__
#define __CWX_APP_HANDLER_4_TIME_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppHandler4Time.h
@brief Time Handle
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxAppHandler4Base.h"
#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;
/**
@class CwxAppHandler4Time
@brief time click Handle
*/
class CWX_API CwxAppHandler4Time:public CwxAppHandler4Base
{
public:
    ///构造函数
    CwxAppHandler4Time(CwxAppFramework* pApp, CwxAppReactor* reactor, CwxTimeValue const& internal);
    ///析构函数
    ~CwxAppHandler4Time();
public:
    /**
    @brief handler open。往reactor的注册。
    @param [in] arg  无效参数
    @return -1：失败； 0：成功
    */
    virtual int open (void * arg= 0);
    /**
    @brief 接受Timeout的事件
    @param [in] event timeout 事件
    @param [in] handle 无效
    @return -1：处理失败； 0：处理成功
    */
    virtual int handle_event(int event, CWX_HANDLE handle=CWX_INVALID_HANDLE);
    ///handle close
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE);
private:
    CwxTimeValue    m_internal;
    CwxTimeValue    m_cur;
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
