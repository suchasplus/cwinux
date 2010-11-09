#ifndef __CWX_ERR_GUARD_H__
#define __CWX_ERR_GUARD_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxErrGuard.h
@brief 定义了CwxErrGuard类，确保errno不被覆盖。
@author cwinux@gmail.com
@version 0.1
@date 2010-07-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxErrGuard
@brief 确保errno不被覆盖。
*/
class CWX_API CwxErrGuard
{
public:
    CwxErrGuard();
    ~CwxErrGuard();
private:
    int    m_errno;
};

CWINUX_END_NAMESPACE

#include "CwxErrGuard.inl"
#include "CwxPost.h"

#endif

