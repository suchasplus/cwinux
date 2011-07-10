CWINUX_BEGIN_NAMESPACE
/***********************************************************************
                    CwxBinLogHeader  class
***********************************************************************/
inline CwxBinLogHeader::CwxBinLogHeader()
{
    memset(this, 0x00, sizeof(this));
}

inline CwxBinLogHeader::CwxBinLogHeader(CWX_UINT64 ullSid,
										CWX_UINT32 uiLogNo,
                                     CWX_UINT32 uiDatetime,
                                     CWX_UINT32 uiOffset,
                                     CWX_UINT32 uiLogLen,
                                     CWX_UINT32 uiPrevOffset,
                                     CWX_UINT32 uiGroup,
                                     CWX_UINT32 uiType):
m_ullSid(ullSid), m_uiLogNo(uiLogNo), m_uiDatetime(uiDatetime), m_uiOffset(uiOffset),
m_uiLogLen(uiLogLen), m_uiPrevOffset(uiPrevOffset),
m_uiGroup(uiGroup), m_uiType(uiType)
{
}


inline CwxBinLogHeader::CwxBinLogHeader(CwxBinLogHeader const& header)
{
    memcpy(this, &header, sizeof(CwxBinLogHeader));
}

inline CwxBinLogHeader& CwxBinLogHeader::operator=(CwxBinLogHeader const& header)
{
    if (this != &header)
    {
        memcpy(this, &header, sizeof(CwxBinLogHeader));
    }
    return *this;
}

inline bool CwxBinLogHeader::operator<(CwxBinLogHeader const& header) const
{
    return this->m_ullSid<header.m_ullSid;
}

inline void CwxBinLogHeader::setSid(CWX_UINT64 ullSid)
{
    m_ullSid = ullSid;
}

inline CWX_UINT64 CwxBinLogHeader::getSid() const
{
    return m_ullSid;
}

///设置记录号
inline void CwxBinLogHeader::setLogNo(CWX_UINT32 uiLogNo)
{
	m_uiLogNo = uiLogNo;
}
///获取记录号
inline CWX_UINT32 CwxBinLogHeader::getLogNo() const
{
	return m_uiLogNo;
}

inline void CwxBinLogHeader::setDatetime(CWX_UINT32 uiDatetime)
{
    m_uiDatetime = uiDatetime;
}

inline CWX_UINT32 CwxBinLogHeader::getDatetime() const
{
    return m_uiDatetime;
}

inline void CwxBinLogHeader::setOffset(CWX_UINT32 uiOffset)
{
    m_uiOffset = uiOffset;
}

inline CWX_UINT32 CwxBinLogHeader::getOffset() const
{
    return m_uiOffset;
}

inline void CwxBinLogHeader::setLogLen(CWX_UINT32 uiLogLen)
{
    m_uiLogLen = uiLogLen;
}

inline CWX_UINT32 CwxBinLogHeader::getLogLen() const
{
    return m_uiLogLen;
}

inline void CwxBinLogHeader::setPrevOffset(CWX_UINT32 uiPrevOffset)
{
    m_uiPrevOffset = uiPrevOffset;
}

inline CWX_UINT32 CwxBinLogHeader::getPrevOffset() const
{
    return m_uiPrevOffset;
}

///设置binlog的分组
inline void CwxBinLogHeader::setGroup(CWX_UINT32 uiGroup)
{
    m_uiGroup = uiGroup;
}

///获取binlog的分组
inline CWX_UINT32 CwxBinLogHeader::getGroup() const
{
    return m_uiGroup;
}

inline void CwxBinLogHeader::setType(CWX_UINT32 uiType)
{
    m_uiType = uiType;
}

inline CWX_UINT32 CwxBinLogHeader::getType() const
{
    return m_uiType;
}


inline CWX_UINT32 CwxBinLogHeader::serialize(char* szBuf) const
{
    CWX_UINT32 byte4;
    CWX_UINT8  pos = 0;
    //sid
    byte4 = (CWX_UINT32)(m_ullSid>>32);
    byte4 = CWX_HTONL(byte4); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    byte4 = (CWX_UINT32)(m_ullSid&0xFFFFFFFF);
    byte4 = CWX_HTONL(byte4); memcpy(szBuf+pos, &byte4, 4); pos+=4;
	//logno
	byte4 = CWX_HTONL(m_uiLogNo); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    //datetime
    byte4 = CWX_HTONL(m_uiDatetime); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    //offset
    byte4 = CWX_HTONL(m_uiOffset); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    //log-length
    byte4 = CWX_HTONL(m_uiLogLen); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    //prev-offset
	byte4 = CWX_HTONL(m_uiPrevOffset); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    //group
    byte4 = CWX_HTONL(m_uiGroup); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    //type
    byte4 = CWX_HTONL(m_uiType); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    return pos;
}

inline CWX_UINT32 CwxBinLogHeader::unserialize(char const* szBuf)
{
    CWX_UINT32 byte4;
    CWX_UINT8  pos = 0;
    //sid
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_ullSid = CWX_NTOHL(byte4);
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_ullSid <<=32;
    m_ullSid += CWX_NTOHL(byte4);
	//log no
	memcpy(&byte4, szBuf+pos, 4); pos += 4;
	m_uiLogNo = CWX_NTOHL(byte4);
    //datetime
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_uiDatetime = CWX_NTOHL(byte4);
    //offset
	memcpy(&byte4, szBuf+pos, 4); pos += 4;
	m_uiOffset = CWX_NTOHL(byte4);
    //log-length
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_uiLogLen = CWX_NTOHL(byte4);
    //prev-offset
	memcpy(&byte4, szBuf+pos, 4); pos += 4;
	m_uiPrevOffset = CWX_NTOHL(byte4);
    //group
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_uiGroup = CWX_NTOHL(byte4);
    //type
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_uiType = CWX_NTOHL(byte4);
    return pos;
}

///清空对象
inline void CwxBinLogHeader::reset()
{
    memset(this, 0x00, sizeof(CwxBinLogHeader));
}

/***********************************************************************
                    CwxBinLogIndex  class
***********************************************************************/
inline CwxBinLogIndex::CwxBinLogIndex()
{
    memset(this, 0x00, sizeof(CwxBinLogIndex));
}

inline CwxBinLogIndex::CwxBinLogIndex(CWX_UINT64 ullSid, CWX_UINT32 uiDatetime, CWX_UINT32 uiOffset, CWX_UINT32 uiLogLen):
m_ullSid(ullSid), m_uiDatetime(uiDatetime), m_uiOffset(uiOffset), m_uiLogLen(uiLogLen)
{
}

inline CwxBinLogIndex::CwxBinLogIndex(CwxBinLogIndex const& index)
{
    memcpy(this, &index, sizeof(CwxBinLogIndex));
}

inline CwxBinLogIndex::CwxBinLogIndex(CwxBinLogHeader const& header)
{
    m_ullSid = header.getSid();
    m_uiDatetime = header.getDatetime();
    m_uiOffset = header.getOffset();
    m_uiLogLen = header.getLogLen();
}

inline CwxBinLogIndex& CwxBinLogIndex::operator=(CwxBinLogIndex const& index)
{
    if (this != &index)
    {
        memcpy(this, &index, sizeof(CwxBinLogIndex));
    }
    return *this;
}

inline CwxBinLogIndex& CwxBinLogIndex::operator=(CwxBinLogHeader const& header)
{
    m_ullSid = header.getSid();
    m_uiDatetime = header.getDatetime();
    m_uiOffset = header.getOffset();
    m_uiLogLen = header.getLogLen();
    return *this;
}

inline bool CwxBinLogIndex::operator<(CwxBinLogIndex const& index) const
{
    return m_ullSid < index.m_ullSid;
}

inline void CwxBinLogIndex::setSid(CWX_UINT64 ullSid)
{
    m_ullSid = ullSid;
}

inline CWX_UINT64 CwxBinLogIndex::getSid() const
{
    return m_ullSid;
}

inline void CwxBinLogIndex::setDatetime(CWX_UINT32 uiDatetime)
{
    m_uiDatetime = uiDatetime;
}

inline CWX_UINT32 CwxBinLogIndex::getDatetime() const
{
    return m_uiDatetime;
}


inline void CwxBinLogIndex::setOffset(CWX_UINT32 uiOffset)
{
    m_uiOffset = uiOffset;
}

inline CWX_UINT32 CwxBinLogIndex::getOffset() const
{
    return m_uiOffset;
}

inline void CwxBinLogIndex::setLogLen(CWX_UINT32 uiLogLen)
{
    m_uiLogLen = uiLogLen;
}

inline CWX_UINT32 CwxBinLogIndex::getLogLen() const
{
    return m_uiLogLen;
}

inline CWX_UINT32 CwxBinLogIndex::serialize(char* szBuf) const
{
    CWX_UINT32 byte4;
    CWX_UINT8  pos = 0;
    //sid
    byte4 = (CWX_UINT32)(m_ullSid>>32);
    byte4 = CWX_HTONL(byte4); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    byte4 = (CWX_UINT32)(m_ullSid&0xFFFFFFFF);
    byte4 = CWX_HTONL(byte4); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    //datetime
    byte4 = CWX_HTONL(m_uiDatetime); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    //offset
	byte4 = CWX_HTONL(m_uiOffset); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    //log-length
    byte4 = CWX_HTONL(m_uiLogLen); memcpy(szBuf+pos, &byte4, 4); pos+=4;
    return pos;
}

inline CWX_UINT32 CwxBinLogIndex::unserialize(char const* szBuf)
{
    CWX_UINT32 byte4;
    CWX_UINT8  pos = 0;
    //sid
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_ullSid = CWX_NTOHL(byte4);
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_ullSid <<=32;
    m_ullSid += CWX_NTOHL(byte4);
    //datetime
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_uiDatetime = CWX_NTOHL(byte4);
    //offset
	memcpy(&byte4, szBuf+pos, 4); pos += 4;
	m_uiOffset = CWX_NTOHL(byte4);
    //log-length
    memcpy(&byte4, szBuf+pos, 4); pos += 4;
    m_uiLogLen = CWX_NTOHL(byte4);
    return pos;
}

///清空对象
inline void CwxBinLogIndex::reset()
{
    memset(this, 0x00, sizeof(CwxBinLogIndex));
}



/***********************************************************************
                    CwxBinLogCursor  class
***********************************************************************/
inline int CwxBinLogCursor::next()
{
    if (-1 != m_fd)
    {
        if (m_bDangling)
        {
            int iRet = header(0);
            if (1 == iRet)
            {//m_curLogHeader有效
                m_bDangling = false;
            }
            return iRet;
        }
        return header(m_curLogHeader.getOffset() + CwxBinLogHeader::BIN_LOG_HEADER_SIZE + m_curLogHeader.getLogLen());
    }
    CwxCommon::snprintf(this->m_szErr2K, 2047, "Cursor's file handle is invalid");
    return -1;
}

inline int CwxBinLogCursor::prev()
{
    if (-1 != m_fd)
    {
        if (m_bDangling)
        {
            int iRet = header(0);
            if (1 == iRet)
            {//m_curLogHeader有效
                m_bDangling = false;
            }
            return iRet;
        }
        if (0 != m_curLogHeader.getOffset())
        {
           return header(m_curLogHeader.getPrevOffset());
        }
        return 0;
    }
    CwxCommon::snprintf(this->m_szErr2K, 2047, "Cursor's file handle is invalid");
    return -1;
}

inline int CwxBinLogCursor::seek(CWX_UINT32 uiOffset)
{
    int iRet = header(uiOffset);
    if (m_bDangling && (1 == iRet))
    {
        m_bDangling = false;
    }
    else if (0 == uiOffset)
    {
        if ((0 == iRet) || (-2 == iRet))
        {
           m_bDangling = true;
        }
    }
    return iRet;
}

inline int CwxBinLogCursor::seek(char const* header, CWX_UINT64 ullSid, bool bDangling)
{
	m_bDangling = bDangling;
	if (!m_bDangling)
	{
		m_curLogHeader.unserialize(header);
		if (ullSid != m_curLogHeader.getSid())
		{
			char szBuf1[64];
			char szBuf2[64];
			CwxCommon::snprintf(this->m_szErr2K, 2047, "Header buf's sid is:%s, but input sid is:%s. They are not same.",
				CwxCommon::toString(m_curLogHeader.getSid(), szBuf1),
				CwxCommon::toString(ullSid, szBuf2));
			return -1;
		}
		return 1;
	}
	return 0;
}


inline bool CwxBinLogCursor::isDangling() const
{
    return m_bDangling;
}

inline CwxBinLogHeader const& CwxBinLogCursor::getHeader() const
{
    return m_curLogHeader;
}

inline string const& CwxBinLogCursor::getFileName() const
{
    return m_strFileName;
}

///获取文件的日期
inline CWX_UINT32 CwxBinLogCursor::getFileDay() const
{
    return m_pBinLogFile?m_pBinLogFile->getFileDay():0;
}

///获取文件的序号
inline CWX_UINT32 CwxBinLogCursor::getFileNo() const
{
    return m_pBinLogFile?m_pBinLogFile->getFileNo():0;
}


inline char const* CwxBinLogCursor::getErrMsg() const
{
    return m_szErr2K;
}

inline int CwxBinLogCursor::getHandle() const
{
    return m_fd;
}
/***********************************************************************
                    CwxBinLogWriteCache  class
***********************************************************************/
inline int CwxBinLogWriteCache::flushData(char* szErr2K)
{
	if (m_uiDataLen)
	{
		if (m_uiDataLen != (CWX_UINT32)::pwrite(m_dataFd, m_dataBuf, m_uiDataLen, m_uiDataFileOffset))
		{
			if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to write data to binlog file, errno=%d", errno);
			return -1;
		}
		m_ullPrevDataSid = m_ullMaxSid;
		m_ullMinDataSid = 0;
		m_uiDataFileOffset += m_uiDataLen; ///新的偏移位置
		m_uiDataLen = 0;
		m_dataSidMap.clear();
	}
	return 0;
}


inline int CwxBinLogWriteCache::flushIndex(char* szErr2K)
{
	if (m_uiIndexLen)
	{
		if (m_uiIndexLen != (CWX_UINT32)::pwrite(m_indexFd, m_indexBuf, m_uiIndexLen, m_uiIndexFileOffset))
		{
			if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to write index data to binlog index, errno=%d", errno);
			return -1;
		}
		m_ullPrevIndexSid = m_ullMaxSid;
		m_ullMinIndexSid = 0;
		m_uiIndexFileOffset += m_uiIndexLen;
		m_uiIndexLen = 0;
		m_indexSidMap.clear();
	}
	return 0;
}





/***********************************************************************
                    CwxBinLogFile  class
***********************************************************************/
///小于比较
inline bool CwxBinLogFile::operator < (CwxBinLogFile const& obj) const
{
    return m_ullMinSid < obj.m_ullMaxSid;
}

///获取最小的sid
inline CWX_UINT64 CwxBinLogFile::getMinSid() const
{
    return m_ullMinSid;
}

///获取最大的sid
inline CWX_UINT64 CwxBinLogFile::getMaxSid() const
{
    return m_ullMaxSid;
}

///获取binlog的最小时间戳
inline CWX_UINT32 CwxBinLogFile::getMinTimestamp() const
{
    return m_ttMinTimestamp;
}

///获取binlog的最大时间戳
inline CWX_UINT32 CwxBinLogFile::getMaxTimestamp() const
{
    return m_ttMaxTimestamp;
}

inline CWX_UINT32 CwxBinLogFile::getFileDay() const
{
    return m_ttDay;
}

///获取binlog的文件序号
inline CWX_UINT32 CwxBinLogFile::getFileNo() const
{
    return m_uiFileNo;
}

///获取binlog文件的log记录数
inline CWX_UINT32 CwxBinLogFile::getLogNum() const
{
    return m_uiLogNum;
}

///获取binlog文件的大小
inline CWX_UINT32 CwxBinLogFile::getFileSize() const
{
    return m_uiFileSize;
}

///判断是否只读
inline bool CwxBinLogFile::readOnly() const
{
    return m_bReadOnly;
}

///设置只读
inline void CwxBinLogFile::setReadOnly()
{
    if (!m_bReadOnly)
    {
		flush_cache(true, NULL);
		fsync_data(true,NULL);
        m_bReadOnly = true;
        if (-1 != m_fd) ::close(m_fd);
        m_fd = -1;
        if (-1 != m_indexFd) ::close(m_indexFd);
        m_indexFd = -1;
		if (m_writeCache) delete m_writeCache;
		m_writeCache = NULL;
    }
}

///判断日志文件是否为空
inline bool CwxBinLogFile::empty() const
{
    return !m_uiLogNum;
}

///获取数据文件的名字
inline string const& CwxBinLogFile::getDataFileName() const
{
    return m_strPathFileName;
}
///获取索引文件的名字
inline string const& CwxBinLogFile::getIndexFileName() const
{
    return m_strIndexFileName;
}


//-1：失败；0：成功。
inline int CwxBinLogFile::readIndex(int fd, CwxBinLogIndex& index, CWX_UINT32 uiOffset, char* szErr2K) const
{
    char szBuf[CwxBinLogIndex::BIN_LOG_INDEX_SIZE];
	ssize_t ret = pread(fd, &szBuf, CwxBinLogIndex::BIN_LOG_INDEX_SIZE, uiOffset);
    if (CwxBinLogIndex::BIN_LOG_INDEX_SIZE != ret)
    {
		if (szErr2K)
		{
			if (-1 == ret)
			{
				CwxCommon::snprintf(szErr2K, 2047, "Failure to read binlog index, file:%s, errno=%d", this->m_strIndexFileName.c_str(), errno);
			}
			else
			{
				CwxCommon::snprintf(szErr2K, 2047, "No complete index record, offset:%u", uiOffset);
			}
		}
        return -1;
    }
    index.unserialize(szBuf);
    return 0;
}

// -1：失败；0：成功。
inline int CwxBinLogFile::writeIndex(int fd, CwxBinLogIndex const& index, CWX_UINT32 uiOffset, char* szErr2K) const
{
    char szBuf[CwxBinLogIndex::BIN_LOG_INDEX_SIZE];
    index.serialize(szBuf);
    if (CwxBinLogIndex::BIN_LOG_INDEX_SIZE != pwrite(fd, szBuf, CwxBinLogIndex::BIN_LOG_INDEX_SIZE, uiOffset))
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to write binlog index, file:%s, errno=%d", m_strIndexFileName.c_str(), errno);
        return -1;
    }
    return 0;
}

/***********************************************************************
                    CwxBinLogMgr  class
***********************************************************************/
///获取管理器是否有效
inline bool CwxBinLogMgr::isInvalid() const
{
    return !m_bValid;
}

inline bool CwxBinLogMgr::isOutRange(CwxBinLogCursor* pCursor)
{
    CwxReadLockGuard<CwxRwLock> lock(&m_rwLock);
    return _isOutRange(pCursor);
}

///cursor对应的文件，是否在管理的范围之外
inline bool CwxBinLogMgr::_isOutRange(CwxBinLogCursor*& pCursor)
{
    return pCursor->m_curLogHeader.getSid() < getMinSid();
}


///获取管理器无效的原因
inline char const* CwxBinLogMgr::getInvalidMsg() const
{
    return m_szErr2K;
}

///获取最小的sid
inline CWX_UINT64 CwxBinLogMgr::getMinSid()
{
    return m_ullMinSid;
}
///获取最大的sid
inline CWX_UINT64 CwxBinLogMgr::getMaxSid()
{
    return m_ullMaxSid;
}
///获取binlog的最小时间戳
inline CWX_UINT32 CwxBinLogMgr::getMinTimestamp()
{
    return m_ttMinTimestamp;
}

///获取binlog的最大时间戳
inline CWX_UINT32 CwxBinLogMgr::getMaxTimestamp()
{
    return m_ttMaxTimestamp;
}

///获取管理的binlog的最小文件序号
inline string& CwxBinLogMgr::getMinFile(string& strFile)
{
    ///读锁保护
    CwxReadLockGuard<CwxRwLock> lock(&m_rwLock);
    CwxBinLogFile* pFile = _getMinBinLogFile();
    strFile = pFile?pFile->getDataFileName():"";
    return strFile;
}
///获取管理的binlog的最大文件序号
inline string& CwxBinLogMgr::getMaxFile(string& strFile)
{
    ///读锁保护
    CwxReadLockGuard<CwxRwLock> lock(&m_rwLock);
    CwxBinLogFile* pFile = _getMaxBinLogFile();
    strFile = pFile?pFile->getDataFileName():"";
    return strFile;
}
///检查是否为空
inline bool CwxBinLogMgr::empty()
{
    return !getMaxSid();
}
///获取文件号对应的binlog文件名
inline string& CwxBinLogMgr::getFileNameByFileNo(CWX_UINT32 uiFileNo,
                                                 CWX_UINT32 ttDay,
                                                 string& strFileName)
{
    char szPathFile[512];
    string strDay;
    CwxDate::getDateY4MDHMS2(ttDay, strDay);
    snprintf(szPathFile, 511, "%s%s.%s.%4.4d.log",
        m_strPrexLogPath.c_str(),
        m_strFilePrex.c_str(),
        strDay.substr(0,8).c_str(),
        uiFileNo);
    strFileName = szPathFile;
    return strFileName;
}
///获取文件号对应的binlog文件的索引文件名
inline string& CwxBinLogMgr::getIndexFileNameByFileNo(CWX_UINT32 uiFileNo,
                                                      CWX_UINT32 ttDay,
                                                      string& strFileName)
{
    getFileNameByFileNo(uiFileNo, ttDay, strFileName);
    strFileName += ".idx";
    return strFileName;
}

inline CWX_UINT32 CwxBinLogMgr::getBinLogFileNo(string const& strFileName, CWX_UINT32& ttDay)
{
    
    if (strFileName.length() <= m_strPrexLogPath.length() + m_strFilePrex.length() + 13) return 0;
    string strDay = strFileName.substr(m_strPrexLogPath.length() + m_strFilePrex.length() + 1, 8);
    strDay += "000000";
    ttDay = CwxDate::getDateY4MDHMS2(strDay);
    return strtoul(strFileName.c_str() + m_strPrexLogPath.length() + m_strFilePrex.length() + 1 + 8 + 1, NULL, 10);
}

inline bool CwxBinLogMgr::isBinLogFile(string const& strFileName)
{
    string strTmp;
    CWX_UINT32  ttDay=0;
    CWX_UINT32 uiFileNo = getBinLogFileNo(strFileName, ttDay);
    return getFileNameByFileNo(uiFileNo, ttDay, strTmp) == strFileName;
}

///判断一个文件名是否是一个binlog的索引文件
inline bool CwxBinLogMgr::isBinLogIndexFile(string const& strFileName)
{
    string strTmp;
    CWX_UINT32 ttDay = 0;
    CWX_UINT32 uiFileNo = getBinLogFileNo(strFileName, ttDay);
    return getIndexFileNameByFileNo(uiFileNo, ttDay, strTmp) == strFileName;
}
///获取binlog的前缀名
inline  string const& CwxBinLogMgr::getBinlogPrexName() const
{
    return m_strFilePrex;
}
///是否有效的前缀名
inline bool CwxBinLogMgr::isValidPrexName(char const* szName)
{
    if (!szName) return false;
    if (!strlen(szName)) return false;
    size_t i=0;
    while(szName[i])
    {
        if (((szName[i] >= 'a') && (szName[i]<='z')) ||
            ((szName[i] >= 'A') && (szName[i]<='Z')) ||
            ((szName[i] >= '0') && (szName[i]<='9')) ||
            (szName[i] == '.') || (szName[i] == '_') || (szName[i] == '-'))
            continue;
        return false;
    }
    return true;
}


inline CwxBinLogFile* CwxBinLogMgr::_getMinBinLogFile() 
{
    if (m_arrBinlog.size()) return m_arrBinlog[0];
    return m_pCurBinlog;
}

inline CwxBinLogFile* CwxBinLogMgr::_getMaxBinLogFile()
{
    return m_pCurBinlog;
}

inline bool CwxBinLogMgr::_isManageBinLogFile(CwxBinLogFile* pBinLogFile)
{
    if (!m_pCurBinlog) return true;
    ///如果文件被cursor使用，则被管理
    return (m_ttMaxTimestamp - pBinLogFile->getMaxTimestamp()) < m_uiMaxDay * (3600 * 24);
}

///检查cursor是否有效
inline bool CwxBinLogMgr::isCursorValid(CwxBinLogCursor* pCursor)
{
    return  CURSOR_STATE_READY == pCursor->m_ucSeekState;
}

inline bool CwxBinLogMgr::isUnseek(CwxBinLogCursor* pCursor)
{
    return CURSOR_STATE_UNSEEK == pCursor->m_ucSeekState;
}


CWINUX_END_NAMESPACE
