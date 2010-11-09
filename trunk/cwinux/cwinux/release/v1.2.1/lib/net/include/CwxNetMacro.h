#ifndef __CWX_NET_MACRO_H__
#define __CWX_NET_MACRO_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwxNetMacro.h
@brief Net模块的公共宏定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"

#include <netinet/in.h>
#include <arpa/inet.h>

# if !defined (INADDR_ANY)
#   define INADDR_ANY (CWX_UINT32)0x00000000
# endif /* INADDR_ANY */

#if !defined (INADDR_LOOPBACK)
#  define INADDR_LOOPBACK ((CWX_UINT32) 0x7f000001)
#endif /* INADDR_LOOPBACK */

#if !defined (AF_ANY)
#  define AF_ANY (-1)
#endif /* AF_ANY */

#define  CWX_HAS_IPV6  0
#define  CWX_IPV4_IPV6_MIGRATION 0

#define CWX_HAS_SOCKADDR_IN6_SIN6_LEN 0
#define CWX_HAS_SOCKADDR_IN_SIN_LEN 0


#define CWX_MAX_DEFAULT_PORT 65535

#define CWX_MAX_HOST_NAME_LEN  256

#include "CwxPost.h"

#endif
