/**
*@file CwxPackageWriter.inl
*@brief CwxPackageWriter类的Inline实现
*@author cwinux@gmail.com
*@version 1.0
*@date  2009-06-05
*@warning  nothing
*@bug    
*/
CWINUX_BEGIN_NAMESPACE

///建立一个新package
inline void CwxPackageWriter::beginPack()
{
    reset();
}
inline bool CwxPackageWriter::addKeyValue(char const* szKey, string const& strData)
{
    return addKeyValue(szKey, strData.c_str(), strData.length(), false);
}

///Add a new key.true: success; false:failure, get err-msg by GetError().
inline bool CwxPackageWriter::addKeyValue(char const* szKey, CWX_UINT8 ucData)
{
    char buf[32];
    sprintf(buf, "%u", ucData);
    return addKeyValue(szKey, buf, strlen(buf), false);
}
///Add a new key.true: success; false:failure, get err-msg by GetError().
inline bool CwxPackageWriter::addKeyValue(char const* szKey, CWX_INT8 cData)
{
    char buf[32];
    sprintf(buf, "%d", cData);
    return addKeyValue(szKey, buf, strlen(buf), false);
}

///Add a new key.true: success; false:failure, get err-msg by GetError().
inline bool CwxPackageWriter::addKeyValue(char const* szKey, CWX_UINT16 unData)
{
    char buf[32];
    sprintf(buf, "%u", unData);
    return addKeyValue(szKey, buf, strlen(buf), false);
}

///Add a new key.true: success; false:failure, get err-msg by GetError().
inline bool CwxPackageWriter::addKeyValue(char const* szKey, CWX_INT16 nData)
{
    char buf[32];
    sprintf(buf, "%d", nData);
    return addKeyValue(szKey, buf, strlen(buf), false);
}


///Add a new key.true: success; false:failure, get err-msg by GetError().
inline bool CwxPackageWriter::addKeyValue(char const* szKey, CWX_UINT32 uiData)
{
    char buf[32];
    sprintf(buf, "%u", uiData);
    return addKeyValue(szKey, buf, strlen(buf), false);
}

///Add a new key.true: success; false:failure, get err-msg by GetError().
inline bool CwxPackageWriter::addKeyValue(char const* szKey, CWX_INT32 iData)
{
    char buf[32];
    sprintf(buf, "%d", iData);
    return addKeyValue(szKey, buf, strlen(buf), false);
}


///Add a new key.true: success; false:failure, get err-msg by GetError().
inline bool CwxPackageWriter::addKeyValue(char const* szKey, CWX_UINT64 ullData)
{
    char buf[32];
    CwxCommon::toString(ullData, buf);
    return addKeyValue(szKey, buf, strlen(buf), false);
}

///Add a new key.true: success; false:failure, get err-msg by GetError().
inline bool CwxPackageWriter::addKeyValue(char const* szKey, CWX_INT64 llData)
{
    char buf[32];
    CwxCommon::toString(llData, buf);
    return addKeyValue(szKey, buf, strlen(buf), false);
}


///Package msg . true: success; false:failure, get err-msg by GetError().
inline bool CwxPackageWriter::pack()
{
    return true;
}
///获取当前package的Key的数量。
inline CWX_UINT32 CwxPackageWriter::getKeyNum() const
{
    return m_uiKeyNum;
}
///
inline CWX_UINT32 CwxPackageWriter::getBufSize() const
{
    return m_uiPackBufLen;
}
///return msg's size
inline CWX_UINT32 CwxPackageWriter::getMsgSize() const 
{
    return m_uiCurPackPos;
}
///return msg
inline char const* CwxPackageWriter::getMsg() const 
{
    return m_szPackMsg;
}
///return the errmsg
inline char const* CwxPackageWriter::getErrMsg() const
{
    return m_szErr;
}


CWINUX_END_NAMESPACE
