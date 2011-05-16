#ifndef __CWX_CRC32_H__
#define __CWX_CRC32_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.


#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxUtilMacro.h"
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <endian.h>
/**
*@file  CwxMd5.h
*@brief  本代码来源于google levelDB的crc32，由CWINUX从C翻译为了C++，并采用CWINUX的接口
         此文件定义了CwxCrc32的类，实现CRC32的签名
*@author cwinux@gmail.com
*@version 0.1
*@date    2011-05-16
*@warning 无
*@bug   
*/
CWINUX_BEGIN_NAMESPACE

class CWX_API CwxCrc32
{
public:
    enum
    {
        CRC32_MASK_DELTA = 0xa282ead8ul
    };

public:
    // Return the crc32c of concat(A, data[0,n-1]) where init_crc is the
    // crc32c of some string A.  Extend() is often used to maintain the
    // crc32c of a stream of data.
    static CWX_UINT32 extend(CWX_UINT32 init_crc, const char* data, size_t n);

    // Return the crc32c of data[0,n-1]
    static inline CWX_UINT32 value(const char* data, size_t n)
    {
        return extend(0, data, n);
    }

    // Return a masked representation of crc.
    //
    // Motivation: it is problematic to compute the CRC of a string that
    // contains embedded CRCs.  Therefore we recommend that CRCs stored
    // somewhere (e.g., in files) should be masked before being stored.
    static inline CWX_UINT32 mask(CWX_UINT32 crc)
    {
        // Rotate right by 15 bits and add a constant.
        return ((crc >> 15) | (crc << 17)) + CRC32_MASK_DELTA;
    }

    // Return the crc whose masked representation is masked_crc.
    static inline CWX_UINT32 unmask(CWX_UINT32 masked_crc)
    {
        CWX_UINT32 rot = masked_crc - CRC32_MASK_DELTA;
        return ((rot >> 17) | (rot << 15));
    }
private:
    CwxCrc32()
    {
    }
}

CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif