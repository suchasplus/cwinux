#ifndef __CWX_RW_LOCK_H__
#define __CWX_RW_LOCK_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxRwLock.h
@brief 实现了读写锁的接口。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>

CWINUX_BEGIN_NAMESPACE

/**
@class CwxRwLock
@brief 读写锁对象，支持线程读写锁与进程读写锁。
*/
class CWX_API CwxRwLock
{
public:
    ///构造函数，创建一个线程读写锁
    CwxRwLock(pthread_rwlockattr_t *arg = NULL);
    ///构造函数，创建一个进程级的读写锁
///    CwxRwLock(char* path, void *arg = NULL);
    ///析构函数
    ~CwxRwLock();
public:
    ///获取写锁，0：成功，-1：失败
    int acquire();
    ///检查是否可加写锁，0：可，-1：否
    int tryacquire();
    ///是否锁，0：成功，-1：失败
    int release();
    ///获取读锁，0：成功，-1：失败
    int acquire_read();
    ///获取写锁，0：成功，-1：失败
    int acquire_write();
    ///检查是否可加读锁，0：可，-1：否
    int tryacquire_read();
    ///检查是否可加写锁，0：可，-1：否
    int tryacquire_write();
    ///获取内部的锁句柄，此为OS的锁句柄
    pthread_rwlock_t const &lock() const;
private:
    pthread_rwlock_t*  m_rwLock;///<读写锁句柄
};


CWINUX_END_NAMESPACE
#include "CwxRwLock.inl"
#include "CwxPost.h"

#endif
