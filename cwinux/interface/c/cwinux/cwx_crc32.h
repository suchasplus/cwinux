#ifndef __CWX_CRC32_H__
#define __CWX_CRC32_H__


#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <endian.h>

/**
*@file  cwx_dd5.h
*@brief  本代码来源于google levelDB的crc32
*@author cwinux@gmail.com
*@version 0.1
*@date    2011-05-16
*@warning 无
*@bug   
*/
#define  CWX_CRC32_MASK_DELTA  0xa282ead8ul

// Return the crc32c of concat(A, data[0,n-1]) where init_crc is the
// crc32c of some string A.  Extend() is often used to maintain the
// crc32c of a stream of data.
CWX_UINT32 cwx_crc32_extend(CWX_UINT32 init_crc, const char* data, size_t n);

// Return the crc32c of data[0,n-1]
inline CWX_UINT32 cwx_crc32_value(const char* data, size_t n)
{
    return cwx_crc32_extend(0, data, n);
}

// Return a masked representation of crc.
//
// Motivation: it is problematic to compute the CRC of a string that
// contains embedded CRCs.  Therefore we recommend that CRCs stored
// somewhere (e.g., in files) should be masked before being stored.
inline CWX_UINT32 cwx_crc32_mask(CWX_UINT32 crc)
{
    // Rotate right by 15 bits and add a constant.
    return ((crc >> 15) | (crc << 17)) + CWX_CRC32_MASK_DELTA;
}

// Return the crc whose masked representation is masked_crc.
inline CWX_UINT32 cwx_crc32_unmask(CWX_UINT32 masked_crc)
{
    CWX_UINT32 rot = masked_crc - CWX_CRC32_MASK_DELTA;
    return ((rot >> 17) | (rot << 15));
}


#endif
