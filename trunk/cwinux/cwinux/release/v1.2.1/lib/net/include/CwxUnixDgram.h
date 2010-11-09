#ifndef __CWX_UNIX_DGRAM_H__
#define __CWX_UNIX_DGRAM_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxUnixDgram.h
@brief unix domain 协议族的UDP包数据通信对象的定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-12
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxNetMacro.h"
#include "CwxUnixAddr.h"
#include "CwxSockDgram.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxUnixDgram
@brief Unix domain协议族的面向包的UDP通信对象。
*/
class CWX_API CwxUnixDgram:public CwxSockDgram
{
public:
    ///默认构造函数
    CwxUnixDgram();
    ///构造函数，创建对象并设定连接handle
    CwxUnixDgram(CWX_HANDLE handle);
    ///析构函数.
    ~CwxUnixDgram(void);
public:
private:
    /**
    * Return the address of the remotely connected peer (if there is
    * one), in the referenced CwxAddr. Returns 0 if successful, else
    * -1.
    */
    int getRemoteAddr (CwxAddr &) const;

};

CWINUX_END_NAMESPACE

#include "CwxUnixDgram.inl"
#include "CwxPost.h"

#endif
