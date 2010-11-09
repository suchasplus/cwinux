#ifndef  __CWX_STL_H__
#define  __CWX_STL_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
*@file  CwxStl.h
*@brief sgi header
*@author cwinux@gmail.com
*@version 0.1
*@date  2009-06-28
*@warning  无.
*/
#include "CwxPre.h"

#include <utility>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <set>
#include <map>
#ifdef __GNUC__
#include <ext/hash_set>
#include <ext/hash_map>
using namespace __gnu_cxx;
#define HAVE_STL_HASH  1
#else
#undef HAVE_STL_HASH
#endif

using namespace std;

#include "CwxPost.h"
#endif

