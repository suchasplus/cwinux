#ifndef __CWX_SINGLETON_H__
#define __CWX_SINGLETON_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxSingleton.h
@brief 单实例对象接口的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-02
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"

CWINUX_BEGIN_NAMESPACE
class CwxSingletonMgr;
/**
@class CwxSingleton
@brief 单实例对象接口。
*/
class CWX_API CwxSingleton
{
public:
    inline string const& getName() const
    {
        return m_strName;
    }
protected:
    ///构造函数，将往CwxSingletonMgr进行注册, strName为对象的名字，以便调试
    CwxSingleton(string const& strName);
    ///析构函数，对象的实例将有CwxSingletonMgr释放
    virtual ~CwxSingleton();
    friend class CwxSingletonMgr;
private:
    CwxSingleton*      m_next;
    string             m_strName;

};

CWINUX_END_NAMESPACE


#include "CwxPost.h"

#endif
