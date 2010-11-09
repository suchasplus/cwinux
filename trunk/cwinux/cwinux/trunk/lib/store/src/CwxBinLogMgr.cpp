#include "CwxBinLogMgr.h"

CWINUX_BEGIN_NAMESPACE
/***********************************************************************
                    CwxBinLogCursor  class
***********************************************************************/
CwxBinLogCursor::CwxBinLogCursor()
{
    m_fd = -1;
    m_bDangling = true;
    m_szHeadBuf[0] = 0x00;
    m_szErr2K[0] = 0x00;
    m_ullSid = 0; ///<seek的sid
    m_ucSeekState = 0; ///<seek的状态
    m_pBinLogFile = NULL;

}

CwxBinLogCursor::~CwxBinLogCursor()
{
    if (-1 != m_fd) ::close(m_fd);
}

int CwxBinLogCursor::open(char const* szFileName)
{
    if (-1 != this->m_fd) ::close(m_fd);
    m_fd = ::open(szFileName, O_RDONLY);
    if (-1 == m_fd)
    {
        CwxCommon::snprintf(this->m_szErr2K, 2047, "Can't open file:%s.", szFileName);
        return -1;
    }
    m_strFileName = szFileName;
    m_bDangling = true;
    m_curLogHeader.reset();
    m_szErr2K[0] = 0x00;
/*    m_ullSid = 0;
    m_ucSeekState = 0; ///<seek的状态
    m_pBinLogFile = NULL;*/
    return 0;
}

/**
@brief 获取当前log的data
@return -1：失败；>=0：获取数据的长度
*/
int CwxBinLogCursor::data(char * szBuf, CWX_UINT32& uiBufLen)
{
    int iRet;
    if (-1 == m_fd)
    {
        CwxCommon::snprintf(this->m_szErr2K, 2047, "Cursor's file handle is invalid");
        return -1;
    }
    if (m_bDangling)
    {
        CwxCommon::snprintf(this->m_szErr2K, 2047, "Cursor is dangling.");
        return -1;
    }
    if (m_curLogHeader.getLogLen())
    {
        if (uiBufLen < m_curLogHeader.getLogLen())
        {
            CwxCommon::snprintf(this->m_szErr2K, 2047, "Buf is too small, buf-size[%u], data-size[%u]",
                uiBufLen, m_curLogHeader.getLogLen());
            return -1;
        }
        iRet = pread(this->m_fd,
            szBuf,
            m_curLogHeader.getLogLen(),
            m_curLogHeader.getOffset() + CwxBinLogHeader::BIN_LOG_HEADER_SIZE);
        if (iRet != (int)m_curLogHeader.getLogLen())
        {
            if (-1 == iRet)
            {
                CwxCommon::snprintf(this->m_szErr2K, 2047, "Failure to read file, errno=%d", errno);
                return -1;
            }
            uiBufLen = iRet;
            CwxCommon::snprintf(this->m_szErr2K, 2047, "Log's dat is less, log-data-size[%u], read-size[%u]",
                m_curLogHeader.getLogLen(), iRet);
            return -2;
        }
    }
    uiBufLen = m_curLogHeader.getLogLen();
    return uiBufLen;
}

void CwxBinLogCursor::close()
{
    if (m_fd) ::close(m_fd);
    m_fd = -1;
    m_bDangling = true;
}


///-2：不存在完成的记录头；-1：失败；0：结束；1：读取一个
int CwxBinLogCursor::header(CWX_UINT64 ullOffset)
{
    int iRet;
    iRet = pread(this->m_fd, m_szHeadBuf, CwxBinLogHeader::BIN_LOG_HEADER_SIZE, ullOffset);
    if (iRet != CwxBinLogHeader::BIN_LOG_HEADER_SIZE)
    {
        if (0 == iRet)
        {
            return 0;
        }
        else if(-1 == iRet)
        {
            CwxCommon::snprintf(this->m_szErr2K, 2047, "Failure to read bin-log, errno=%d.", errno);
            return -1;
        }
        CwxCommon::snprintf(this->m_szErr2K, 2047, "Log is incomplete");
        return -2;
    }
    m_curLogHeader.unserialize(m_szHeadBuf);
    if (ullOffset != m_curLogHeader.getOffset())
    {
        char szBuf1[64];
        char szBuf2[64];
        CwxCommon::snprintf(this->m_szErr2K, 2047, "Invalid binlog, offset of header[%s] is different with it's file-offset[%s].",
            CwxCommon::toString(ullOffset, szBuf1),
            CwxCommon::toString(m_curLogHeader.getOffset(), szBuf2));
        return -1;
    }
    return 1;
}


/***********************************************************************
                    CwxBinLogFile  class
***********************************************************************/
CwxBinLogFile::CwxBinLogFile(CWX_UINT32 ttDay, CWX_UINT32 uiFileNo, CWX_UINT64 ullMaxFileSize)
{
    m_bValid = false;
    if (ullMaxFileSize<MIN_BINLOG_FILE_SIZE)
    {
        m_ullMaxFileSize = MIN_BINLOG_FILE_SIZE;
    }
    else
    {
        m_ullMaxFileSize = ullMaxFileSize;
    }
    m_ullMinSid = 0;
    m_ullMaxSid = 0;
    m_ttMinTimestamp = 0;
    m_ttMaxTimestamp = 0;
    m_uiLogNum = 0;
    m_bReadOnly = true;
    m_fd = -1;
    m_indexFd = -1;
    m_ullFileSize = 0;
    m_ullIndexFileSize = 0;
    m_ullPrevLogOffset = 0;
    m_ttDay = ttDay;
    m_uiFileNo = uiFileNo;
    m_prevBinLogFile = NULL;
    m_nextBinLogFile = NULL;
}

CwxBinLogFile::~CwxBinLogFile()
{
    close();
}


//0: success, 
//-1:failure,
int CwxBinLogFile::open(char const* szPathFile,
                        bool bReadOnly,
                        bool bCreate,
                        char* szErr2K)
{
    string strIndexPathFileName;
    //关闭对象
    close();
    m_bReadOnly = bCreate?false:bReadOnly;
    m_strPathFileName = szPathFile;
    m_strIndexFileName = m_strPathFileName + ".idx";

    //获取binlog文件的大小
    if (CwxFile::isFile(m_strPathFileName.c_str()))
    {
        m_ullFileSize = CwxFile::getFileSize(m_strPathFileName.c_str());
        if (-1 == (CWX_INT64)m_ullFileSize)
        {
            if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to get binlog file's size, file:%s, errno=%d", m_strPathFileName.c_str(), errno);
            return -1;
        }
    }
    else
    {
        m_ullFileSize = -1;
    }
    //获取binlog索引文件的大小
    if (CwxFile::isFile(m_strIndexFileName.c_str()))
    {
        m_ullIndexFileSize = CwxFile::getFileSize(m_strIndexFileName.c_str());
        if (-1 == (CWX_INT64)m_ullIndexFileSize)
        {
            if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to get binlog index file's size, file:%s, errno=%d", m_strIndexFileName.c_str(), errno);
            return -1;
        }
    }
    else
    {
        m_ullIndexFileSize = -1;
    }

    //创建binlog文件及其索引文件
    if (bCreate)
    {
        if (-1 == mkBinlog(szErr2K)) return -1;
    }
    else
    {
        if (-1 == (CWX_INT64)m_ullFileSize)
        {//binlog 文件不存在
            if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Binlog file doesn't exist, file:%s", m_strPathFileName.c_str());
            return -1;
        }
    }
    //打开binlog文件
    m_fd = ::open(m_strPathFileName.c_str(),  O_RDWR);
    if (-1 == m_fd)
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Can't open binlog file:%s", m_strPathFileName.c_str());
        return -1;
    }
    //打开索引文件
    if (-1 != (CWX_INT64)m_ullIndexFileSize)
    {//索引文件存在
        m_indexFd = ::open(m_strIndexFileName.c_str(),  O_RDWR);
    }
    else
    {//索引文件不存在
        m_indexFd = ::open(m_strIndexFileName.c_str(),  O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
        //设置index文件的大小为0.
        m_ullIndexFileSize = 0;
    }
    if (-1 == m_indexFd)
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Can't open binlog's index file:%s", m_strIndexFileName.c_str());
        return -1;
    }
    //保证binlog文件与索引文件一致
    if (-1 == prepareFile(szErr2K)) return -1;
    //如果是只读，则关闭binlog与索引的io handle
    if (m_bReadOnly)
    {
        if (-1 != m_fd) ::close(m_fd);
        m_fd = -1;
        if (-1 != m_indexFd) ::close(m_indexFd);
        m_indexFd = -1;
    }

    m_bValid = true;
    return 0;
}

//-1：失败；0：日志文件满了；1：成功。
int CwxBinLogFile::append(CWX_UINT64 ullSid,
                          CWX_UINT32 ttTimestamp,
                          CWX_UINT32 uiGroup,
                          CWX_UINT32 uiType,
                          CWX_UINT32 uiAttr,
                          char const* szData,
                          CWX_UINT32 uiDataLen,
                          char* szErr2K)
{
    char szBuf[CwxBinLogHeader::BIN_LOG_HEADER_SIZE];
    if (m_ullFileSize + FREE_BINLOG_FILE_SIZE + CwxBinLogHeader::BIN_LOG_HEADER_SIZE + uiDataLen >= m_ullMaxFileSize) return 0; //it's full.

    if (this->m_bReadOnly)
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "The log is opened in read mode, can't append record.file:%s", m_strPathFileName.c_str());
        return -1;
    }
    if (!m_bValid)
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "The log is not valid, can't append record. file:%s", m_strPathFileName.c_str());
        return -1;
    }
    if (ttTimestamp < m_ttMaxTimestamp) ttTimestamp = m_ttMaxTimestamp;
    //sid必须升序
    if (!m_uiLogNum)
    {
        if (ullSid <= m_ullMaxSid)
        {
            char szBuf1[64];
            char szBuf2[64];
            if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "binlog[%s]'s sid [%s] is more than the appending binlog's sid[%s]",
                m_strPathFileName.c_str(),
                CwxCommon::toString(m_ullMaxSid, szBuf1),
                CwxCommon::toString(ullSid, szBuf2));
            return -1;
        }
    }
    CwxBinLogHeader header(ullSid,
        (CWX_UINT32)ttTimestamp,
        m_ullFileSize,
        uiDataLen,
        m_uiLogNum,
        m_ullPrevLogOffset,
        uiGroup,
        uiType,
        uiAttr);
    //保存数据文件
    header.serialize(szBuf);
    if (CwxBinLogHeader::BIN_LOG_HEADER_SIZE != pwrite(m_fd, szBuf, CwxBinLogHeader::BIN_LOG_HEADER_SIZE, m_ullFileSize))
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to write log header to binlog:%s, errno=%d", m_strPathFileName.c_str(), errno);
        return -1;
    }
    if (uiDataLen)
    {
        if ((int)uiDataLen != pwrite(m_fd, szData, uiDataLen, m_ullFileSize + CwxBinLogHeader::BIN_LOG_HEADER_SIZE))
        {
            if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to write log data to binlog:%s, errno=%d", m_strPathFileName.c_str(), errno);
            return -1;
        }
    }
    //保存索引文件
    CwxBinLogIndex index(header);
    if (0 != writeIndex(m_indexFd, index, m_ullIndexFileSize, szErr2K)) return -1;
    ///更新前一个binlog的文件offset
    m_ullPrevLogOffset = m_ullFileSize;
    ///修改文件的大小
    m_ullFileSize += CwxBinLogHeader::BIN_LOG_HEADER_SIZE + uiDataLen;
    ///修改索引文件的大小
    m_ullIndexFileSize += CwxBinLogIndex::BIN_LOG_INDEX_SIZE;
    //设置sid、时间戳
    m_ullMaxSid = ullSid;
    m_ttMaxTimestamp = ttTimestamp;
    if (!m_uiLogNum)
    {
        m_ullMinSid = ullSid;
        m_ttMinTimestamp = ttTimestamp;
    }
    //记录数加1
    m_uiLogNum ++;
    return 1;
}

// -1：失败；0：成功。
int CwxBinLogFile::commit(char* szErr2K)
{
    if (this->m_bReadOnly)
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "The log is opened in read mode, can't commit.file:%s", m_strPathFileName.c_str());
        return -1;
    }
    if (!m_bValid)
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "The log is not valid, can't commit. file:%s", m_strPathFileName.c_str());
        return -1;
    }
    fsync(m_fd);
    fsync(m_indexFd);
    return 0;
}

//-2：不存在完成的记录头；-1：失败；0：不存在；1：定位到指定的位置
int CwxBinLogFile::seek(CwxBinLogCursor& cursor, CWX_UINT8 ucMode)
{
    CWX_ASSERT(m_bValid);

    int iRet = cursor.open(m_strPathFileName.c_str());
    if (-1 == iRet) return -1;
    if (SEEK_START == ucMode)  return cursor.seek(0);
    if (SEEK_TAIL == ucMode) return cursor.seek(m_ullPrevLogOffset);
    
    //根据指定的SID定位
    int fd = -1;
    CWX_UINT64 ullOffset = 0;
    if (cursor.m_ullSid >= m_ullMaxSid) return 0;///不存在
    if (cursor.m_ullSid < m_ullMinSid )
    {///从第一个开始
        return cursor.seek(0);
    }
    //折半查找
    fd = ::open(m_strIndexFileName.c_str(), O_RDONLY);
    if (-1 == fd)
    {
        CwxCommon::snprintf(cursor.m_szErr2K, 2047, "Failure to open index file:%s, errno=%d", m_strIndexFileName.c_str(), errno);
        return -1;
    }
    CWX_UINT32 uiStart = 0;
    CWX_UINT32 uiEnd = m_uiLogNum - 1;
    CWX_UINT32 uiMid = 0;
    CwxBinLogIndex item;
    while(uiEnd >= uiStart)
    {
        uiMid = (uiStart + uiEnd)/2;
        ullOffset = uiMid;
        ullOffset *= CwxBinLogIndex::BIN_LOG_INDEX_SIZE;
        if (0 != readIndex(fd, item, ullOffset, cursor.m_szErr2K))
        {
            ::close(fd);
            return -1;
        }
        if (cursor.m_ullSid == item.getSid())
        {
            break;
        }
        else if (cursor.m_ullSid < item.getSid())
        {
            uiEnd = uiMid-1;
        }
        else
        {
            uiStart = uiMid+1;
        }
    }
    ::close(fd);
    if (cursor.m_ullSid >= item.getSid())
    {//next item
        ullOffset = item.getOffset() + item.getLogLen() + CwxBinLogHeader::BIN_LOG_HEADER_SIZE;
    }
    else if (cursor.m_ullSid < item.getSid())
    {
        ullOffset = item.getOffset();
    }
    return cursor.seek(ullOffset);
}


void CwxBinLogFile::reset()
{
    m_bValid = false;
    m_strPathFileName.erase();
    m_strIndexFileName.erase();
    m_ullMinSid = 0;
    m_ullMaxSid = 0;
    m_ttMinTimestamp = 0;
    m_ttMaxTimestamp = 0;
    m_uiLogNum = 0;
    m_bReadOnly = true;
    m_fd = -1;
    m_indexFd = -1;
    m_ullFileSize = 0;
    m_ullIndexFileSize = 0;
    m_ullPrevLogOffset = 0;
    m_prevBinLogFile = NULL;
    m_nextBinLogFile = NULL;
}

void CwxBinLogFile::remove(char const* szPathFileName)
{
    //删除binlog文件
    CwxFile::rmFile(szPathFileName);
    //删除索引文件
    string strIndexFile=szPathFileName;
    strIndexFile += ".idx";
    CwxFile::rmFile(strIndexFile.c_str());
}

//关闭
void CwxBinLogFile::close()
{
    if (-1 != m_fd) ::close(m_fd);
    m_fd = -1;
    if (-1 != m_indexFd) ::close(m_indexFd);
    m_indexFd = -1;
    reset();
}


int CwxBinLogFile::mkBinlog(char* szErr2K)
{
    //pretect the sync-log file
    if ((-1 != (CWX_INT64)m_ullFileSize) && (m_ullFileSize > CwxBinLogHeader::BIN_LOG_HEADER_SIZE * 2))
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "binlog file [%s] exists, can't create.", m_strPathFileName.c_str());
        return -1;
    }
    int fd=-1;
    //以清空文件内容的方式打开文件
    fd = ::open(m_strPathFileName.c_str(),  O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    if (-1 == fd)
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to create binlog file [%s] .", m_strPathFileName.c_str());
        return -1;
    }
    //设置数据文件大小为0
    m_ullFileSize = 0;
    ::close(fd);

    //以清空文件内容的方式打开文件
    fd = ::open(m_strIndexFileName.c_str(),  O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    if (-1 == fd)
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to create binlog's index file [%s] .", m_strIndexFileName.c_str());
        return -1;
    }
    //设置索引文件大小为0
    m_ullIndexFileSize = 0; 
    ::close(fd);
    return 0;
}

//-1：失败；0：成功。
int CwxBinLogFile::prepareFile(char* szErr2K)
{
    int iRet = isRebuildIndex(szErr2K);
    if (-1 == iRet) return -1;
    if (1 == iRet)
    {//rebuilt index
        if (0 != createIndex(szErr2K)) return -1;
    }
    //获取binlog数量
    CWX_ASSERT(!(m_ullIndexFileSize%CwxBinLogIndex::BIN_LOG_INDEX_SIZE));
    m_uiLogNum = m_ullIndexFileSize /CwxBinLogIndex::BIN_LOG_INDEX_SIZE;

    //设置最后一个记录的开始位置
    m_ullPrevLogOffset = 0;

    if (m_uiLogNum)
    {//记录存在
        //获取最小的sid，timestamp
        CwxBinLogIndex index;
        if (0 != readIndex(m_indexFd, index, 0, szErr2K)) return -1;
        m_ullMinSid = index.getSid();
        m_ttMinTimestamp = index.getDatetime();
        //获取最大的sid, timestamp
        if (0 != readIndex(m_indexFd, index, m_ullIndexFileSize - CwxBinLogIndex::BIN_LOG_INDEX_SIZE)) return -1;
        m_ullMaxSid = index.getSid();
        m_ttMaxTimestamp = index.getDatetime();
        m_ullPrevLogOffset = index.getOffset();
        if (m_ullMinSid > m_ullMaxSid)
        {
            char szBuf1[64];
            char szBuf2[64];
            if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Binlog file's min-sid[%s] more than max sid[%s], file:%s",
                CwxCommon::toString(m_ullMinSid, szBuf1),
                CwxCommon::toString(m_ullMaxSid, szBuf2),
                m_strPathFileName.c_str());
            return -1;
        }
    }
    else
    {
        m_ullMinSid = 0;
        m_ullMaxSid = 0;
        m_ttMinTimestamp = 0;
        m_ttMaxTimestamp = 0;
    }
    return 0;
}

//-1：失败；0：不需要；1：需要。
int CwxBinLogFile::isRebuildIndex(char* szErr2K)
{
    //索引自身不完整
    if (m_ullIndexFileSize%CwxBinLogIndex::BIN_LOG_INDEX_SIZE) return 1;

    //索引为空，但数据不为空
    if (!m_ullIndexFileSize) return 1;

    //检查索引记录的binlog文件大小与binlog文件自身的大小关系
    char szBuf[CwxBinLogIndex::BIN_LOG_INDEX_SIZE];
    CwxBinLogIndex index;
    //获取最大的sid, timestamp
    if (CwxBinLogIndex::BIN_LOG_INDEX_SIZE != pread(m_indexFd,
        &szBuf,
        CwxBinLogIndex::BIN_LOG_INDEX_SIZE,
        m_ullIndexFileSize - CwxBinLogIndex::BIN_LOG_INDEX_SIZE))
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Failure to read binlog index, file:%s, errno=%d", this->m_strIndexFileName.c_str(), errno);
        return -1;
    }
    
    index.unserialize(szBuf);
    //不相等需要重建
    if (index.getOffset() + index.getLogLen() + CwxBinLogHeader::BIN_LOG_HEADER_SIZE != m_ullFileSize) return 1;
    //不需要重建
    return 0;
}

//-1：失败；0：成功。
int CwxBinLogFile::createIndex(char* szErr2K)
{
    CwxBinLogIndex index;
    CwxBinLogCursor cursor;
    int iRet = cursor.open(m_strPathFileName.c_str());
    m_ullIndexFileSize = 0;
    if (-1 == iRet)
    {
        if (szErr2K) strcpy(szErr2K, cursor.getErrMsg());
        return -1;
    }

    while(1 == (iRet = cursor.next()))
    {
        index = cursor.getHeader();
        if (0 != writeIndex(m_indexFd, index, m_ullIndexFileSize, szErr2K)) return -1;
        m_ullIndexFileSize += CwxBinLogIndex::BIN_LOG_INDEX_SIZE;
    }
    if (-1 == iRet)
    {
        if (szErr2K) strcpy(szErr2K, cursor.getErrMsg());
        return -1;
    }
    if (m_ullIndexFileSize)
    {//存在有效的binlog
        m_ullFileSize = index.getOffset() + index.getLogLen() + CwxBinLogHeader::BIN_LOG_HEADER_SIZE;
    }
    else
    {//不存在有效的binlog
        m_ullFileSize = 0;
    }
    //truncate binlog 文件
    ftruncate(m_fd, m_ullFileSize);
    //truncate index 文件
    ftruncate(m_indexFd, m_ullIndexFileSize);
    return 0;
}


/***********************************************************************
                    CwxBinLogMgr  class
***********************************************************************/

CwxBinLogMgr::CwxBinLogMgr(char const* szLogPath, char const* szFilePrex, CWX_UINT64 ullMaxFileSize)
{
    m_bValid = false;
    strcpy(m_szErr2K, "Not init.");
    m_pCurBinlog = NULL;
    m_strLogPath = szLogPath;
    if ('/' !=m_strLogPath[m_strLogPath.length() - 1]) m_strLogPath += "/";
    m_strPrexLogPath = m_strLogPath + szFilePrex + "/";
    m_strFilePrex = szFilePrex;
    m_ullMaxFileSize = ullMaxFileSize;
    m_uiMaxDay = DEF_MANAGE_MAX_DAY;
    m_ttDayStart = 0;
    m_fdLock = -1;
    m_ullMinSid = 0; ///<binlog文件的最小sid
    m_ullMaxSid = 0; ///<binlog文件的最大sid
    m_ttMinTimestamp = 0; ///<binlog文件的log开始时间
    m_ttMaxTimestamp = 0; ///<binlog文件的log结束时间

}

CwxBinLogMgr::~CwxBinLogMgr()
{
    if (m_pCurBinlog) delete m_pCurBinlog;
    CWX_UINT32 i = 0;
    for (i=0; i<m_arrBinlog.size(); i++)
    {
        delete m_arrBinlog[i];
    }
    m_arrBinlog.clear();
}

// -1：失败；0：成功。
int CwxBinLogMgr::init(CWX_UINT32 uiMaxDay, char* szErr2K)
{
    ///写锁保护
    CwxWriteLockGuard<CwxRwLock> lock(&m_rwLock);
    this->_clear();
    m_bValid = false;
    strcpy(m_szErr2K, "Not init.");
    if (uiMaxDay < MIN_MANAGE_DAY) m_uiMaxDay = MIN_MANAGE_DAY;
    if (uiMaxDay > MAX_MANAGE_DAY) m_uiMaxDay = MAX_MANAGE_DAY;
    m_uiMaxDay = uiMaxDay;

    //如果binlog的目录不存在，则创建此目录
    if (!CwxFile::isDir(m_strLogPath.c_str()))
    {
        if (!CwxFile::createDir(m_strLogPath.c_str()))
        {
            CwxCommon::snprintf(m_szErr2K, 2047, "Failure to create binlog path:%s, errno=%d", m_strLogPath.c_str(), errno);
            if (szErr2K) strcpy(szErr2K, m_szErr2K);
            return -1;
        }
    }
    if (!CwxFile::isDir(m_strPrexLogPath.c_str()))
    {
        if (!CwxFile::createDir(m_strPrexLogPath.c_str()))
        {
            CwxCommon::snprintf(m_szErr2K, 2047, "Failure to create binlog path:%s, errno=%d", m_strPrexLogPath.c_str(), errno);
            if (szErr2K) strcpy(szErr2K, m_szErr2K);
            return -1;
        }
    }
    //获取系统所文件
    string strLockFile=m_strLogPath + m_strFilePrex + ".lock";
    if (!CwxFile::isFile(strLockFile.c_str()))
    {///创建锁文件
        m_fdLock = ::open(strLockFile.c_str(),  O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    }
    else
    {
        m_fdLock = ::open(strLockFile.c_str(),  O_RDWR);
    }
    if (-1 == m_fdLock)
    {
        CwxCommon::snprintf(m_szErr2K, 2047, "Failure to open  binlog lock file:%s, errno=%d", strLockFile.c_str(), errno);
        if (szErr2K) strcpy(szErr2K, m_szErr2K);
        return -1;
    }
    if (!CwxFile::lock(m_fdLock))
    {
        CwxCommon::snprintf(m_szErr2K, 2047, "Failure to lock  binlog lock file:%s, errno=%d", strLockFile.c_str(), errno);
        if (szErr2K) strcpy(szErr2K, m_szErr2K);
        return -1;
    }

    //获取目录下的所有文件
    list<string> files;
    if (!CwxFile::getDirFile(m_strPrexLogPath, files))
    {
        CwxCommon::snprintf(m_szErr2K, 2047, "Failure to create binlog path:%s, errno=%d", m_strPrexLogPath.c_str(), errno);
        if (szErr2K) strcpy(szErr2K, m_szErr2K);
        return -1;
    }
    //提取目录下的所有binlog文件，并放到map中，利用map的排序，逆序打开文件
    string strPathFile;
    list<string>::iterator iter=files.begin();
    map<CwxBinLogFileItem, string> fileMap;
    CWX_UINT32 ttDay = 0;
    CWX_UINT32 uiFileNo = 0;
    while(iter != files.end())
    {
        strPathFile = m_strPrexLogPath + *iter;
        if (isBinLogFile(strPathFile))
        {
            uiFileNo = getBinLogFileNo(strPathFile, ttDay);
            CwxBinLogFileItem item(ttDay, uiFileNo);
            fileMap[item] = strPathFile;
        }
        iter++;
    }
    map<CwxBinLogFileItem, string>::reverse_iterator map_iter = fileMap.rbegin();

    CwxBinLogFile* pBinLogFile = NULL;
    while(map_iter != fileMap.rend())
    {
        pBinLogFile = new CwxBinLogFile(map_iter->first.getDay(), map_iter->first.getFileNo(), m_ullMaxFileSize);
        if (0 != pBinLogFile->open(map_iter->second.c_str(),
            m_pCurBinlog?true:false,
            false,
            m_szErr2K))
        {
            delete pBinLogFile;
            if (szErr2K) strcpy(szErr2K, m_szErr2K);
            return -1;
        }
        //如果binlog超出管理的范围，则break
        if (!_isManageBinLogFile(pBinLogFile))
        {
            delete pBinLogFile;
            break;
        }
        pBinLogFile->m_nextBinLogFile = NULL;
        pBinLogFile->m_prevBinLogFile = NULL;
        if (!m_pCurBinlog)
        {
            m_pCurBinlog = pBinLogFile;
            m_ullMaxSid = pBinLogFile->getMaxSid();
            m_ullMinSid = pBinLogFile->getMinSid();
            m_ttMaxTimestamp = pBinLogFile->getMaxTimestamp();
        }
        else
        {
            ///按照降序提取文件，因为，最后加入的binlog文件的sid应该小于已有的sid
            if (pBinLogFile->getMaxSid() >= getMinSid())
            {
                char szBuf1[64];
                char szBuf2[64];
                CwxCommon::snprintf(m_szErr2K, 2047, "BinLog file[%s]'s max sid[%s] is more than the existing min sid[%s]",
                    pBinLogFile->getDataFileName().c_str(),
                    CwxCommon::toString(pBinLogFile->getMaxSid(), szBuf1),
                    CwxCommon::toString(getMinSid(), szBuf2)
                    );
                delete pBinLogFile;
                if (szErr2K) strcpy(szErr2K, m_szErr2K);
                return -1;
            }
            if (0 == m_arrBinlog.size())
            {
                m_pCurBinlog->m_prevBinLogFile = pBinLogFile;
                pBinLogFile->m_nextBinLogFile = m_pCurBinlog;
            }
            else
            {
                m_arrBinlog[0]->m_prevBinLogFile = pBinLogFile;
                pBinLogFile->m_nextBinLogFile = m_arrBinlog[0];
            }
            m_arrBinlog.insert(m_arrBinlog.begin(), pBinLogFile);
            m_ullMinSid = pBinLogFile->getMinSid();
            m_ttMinTimestamp = pBinLogFile->getMinTimestamp();
        }
        map_iter++;
    }
    
    _outputManageBinLog();
    m_bValid = true;
    m_szErr2K[0] = 0x00;

    return 0;
}

//-1：失败；0：成功。
int CwxBinLogMgr::append(CWX_UINT64 ullSid,
                         CWX_UINT32 ttTimestamp,
                         CWX_UINT32 uiGroup,
                         CWX_UINT32 uiType,
                         CWX_UINT32 uiAttr,
                         char const* szData,
                         CWX_UINT32 uiDataLen,
                         char* szErr2K)
{
    ///写锁保护
    CwxWriteLockGuard<CwxRwLock> lock(&m_rwLock);
    //如果没有binlog文件，则创建初始binlog文件，初始文件序号为0
    if (ttTimestamp < m_ttMaxTimestamp) ttTimestamp = m_ttMaxTimestamp;
    if (!m_pCurBinlog)
    {
        string strPathFile;
        m_pCurBinlog = new CwxBinLogFile(CwxDate::trimToDay(ttTimestamp), 0, m_ullMaxFileSize);
        getFileNameByFileNo(0, m_pCurBinlog->getFileDay(), strPathFile);
        if (0 != m_pCurBinlog->open(strPathFile.c_str(),
            false,
            true,
            m_szErr2K))
        {
            delete m_pCurBinlog;
            m_pCurBinlog = NULL;
            m_bValid = false;
            if (szErr2K) strcpy(szErr2K, m_szErr2K);
            return -1;
        }
    }

    if (!m_bValid)
    {
        if (szErr2K) CwxCommon::snprintf(szErr2K, 2047, "Binlog manage is invalid, error:%s", m_szErr2K);
        return -1;
    }
    if (ullSid <= m_ullMaxSid)
    {
        char szBuf1[64];
        char szBuf2[64];
        CwxCommon::snprintf(m_szErr2K, 2047, "Log's sid[%s] is no more than max sid[%s]", CwxCommon::toString(ullSid, szBuf1), CwxCommon::toString(m_ullMaxSid, szBuf2));
        if (szErr2K) strcpy(szErr2K, m_szErr2K);
        return -1;
    }
    if (MIN_SID_NO > ullSid)
    {
        char szBuf1[64];
        char szBuf2[64];
        CwxCommon::snprintf(m_szErr2K, 2047, "Log's sid[%s] is less than min sid[%s]", CwxCommon::toString(ullSid, szBuf1), CwxCommon::toString((CWX_UINT64)MIN_SID_NO, szBuf2));
        if (szErr2K) strcpy(szErr2K, m_szErr2K);
        return -1;
    }

    if ((CWX_UINT32)CwxDate::trimToDay(ttTimestamp) > m_pCurBinlog->getFileDay())///new day
    {
        string strPathFile;
        getFileNameByFileNo(0, CwxDate::trimToDay(ttTimestamp), strPathFile);
        CwxBinLogFile* pBinLogFile = new CwxBinLogFile(CwxDate::trimToDay(ttTimestamp), 0, m_ullMaxFileSize);
        if (0 != pBinLogFile->open(strPathFile.c_str(),
            false,
            true,
            m_szErr2K))
        {
            delete pBinLogFile;
            m_bValid = false;
            if (szErr2K) strcpy(szErr2K, m_szErr2K);
            return -1;
        }
        ///设置当前的binlog为只读
        m_pCurBinlog->setReadOnly();
        ///将当前的binlog，放到m_arrBinlog数组中
        m_arrBinlog.push_back(m_pCurBinlog);
        pBinLogFile->m_nextBinLogFile = NULL;
        pBinLogFile->m_prevBinLogFile = m_pCurBinlog;
        m_pCurBinlog->m_nextBinLogFile = pBinLogFile;
        m_pCurBinlog = pBinLogFile;
        //检查是否有超出管理范围的binlog文件
        while(m_arrBinlog.size())
        {
            if (_isManageBinLogFile(m_arrBinlog[0])) break;
            delete m_arrBinlog[0];
            m_arrBinlog.erase(m_arrBinlog.begin());
        }
        if (m_arrBinlog.size()) m_arrBinlog[0]->m_prevBinLogFile = NULL;
        ///重新输出管理的binlog文件信息
        _outputManageBinLog();
    }

    int iRet = m_pCurBinlog->append(ullSid,
        ttTimestamp,
        uiGroup,
        uiType,
        uiAttr,
        szData,
        uiDataLen,
        m_szErr2K);
    if (-1 == iRet)
    {
        m_bValid = false;
        if (szErr2K) strcpy(szErr2K, m_szErr2K);
        return -1;
    }
    if (1 == iRet)
    {
        m_ullMaxSid = ullSid;
        m_ttMaxTimestamp = ttTimestamp;
        if ( 0 == m_ullMinSid)
        {
            m_ullMinSid = ullSid;
            m_ttMinTimestamp = ttTimestamp;
        }
        return 0;
    }
    if (0 == iRet)
    {
        string strPathFile;
        getFileNameByFileNo(m_pCurBinlog->getFileNo() + 1,
            m_pCurBinlog->getFileDay(),
            strPathFile);
        CwxBinLogFile* pBinLogFile = new CwxBinLogFile(m_pCurBinlog->getFileDay(),
            m_pCurBinlog->getFileNo() + 1, 
            m_ullMaxFileSize);
        if (0 != pBinLogFile->open(strPathFile.c_str(),
            false,
            true,
            m_szErr2K))
        {
            delete pBinLogFile;
            m_bValid = false;
            if (szErr2K) strcpy(szErr2K, m_szErr2K);
            return -1;
        }
        ///设置当前的binlog为只读
        m_pCurBinlog->setReadOnly();
        ///将当前的binlog，放到m_arrBinlog数组中
        m_arrBinlog.push_back(m_pCurBinlog);
        pBinLogFile->m_nextBinLogFile = NULL;
        pBinLogFile->m_prevBinLogFile = m_pCurBinlog;
        m_pCurBinlog->m_nextBinLogFile = pBinLogFile;
        m_pCurBinlog = pBinLogFile;
        //检查是否有超出管理范围的binlog文件
        while(m_arrBinlog.size())
        {
            if (_isManageBinLogFile(m_arrBinlog[0])) break;
            delete m_arrBinlog[0];
            m_arrBinlog.erase(m_arrBinlog.begin());
        }
        if (m_arrBinlog.size()) m_arrBinlog[0]->m_prevBinLogFile = NULL;
        ///重新输出管理的binlog文件信息
        _outputManageBinLog();
    }
    ///将记录重新添加到新binlog文件中
    iRet = m_pCurBinlog->append(ullSid,
        ttTimestamp,
        uiGroup,
        uiType,
        uiAttr,
        szData,
        uiDataLen,
        m_szErr2K);
    if (-1 == iRet)
    {
        m_bValid = false;
        if (szErr2K) strcpy(szErr2K, m_szErr2K);
        return -1;
    }
    if (1 == iRet)
    {
        m_ullMaxSid = ullSid;
        m_ttMaxTimestamp = ttTimestamp;
        if ( 0 == m_ullMinSid)
        {
            m_ullMinSid = ullSid;
            m_ttMinTimestamp = ttTimestamp;
        }
        return 0;
    }
    ///新文件无法放下一个记录
    CwxCommon::snprintf(m_szErr2K, 2047, "Binlog's length[%d] is too large, can't be put binlog file", uiDataLen);
    if (szErr2K) strcpy(szErr2K, m_szErr2K);
    m_bValid = false;
    return -1;
}


//-1：失败；0：成功。
int CwxBinLogMgr::commit(char* szErr2K)
{
    int iRet = 0;
    if (m_pCurBinlog) iRet = m_pCurBinlog->commit(m_szErr2K);
    if (0 != iRet)
    {
        if (szErr2K) strcpy(szErr2K, m_szErr2K);
    }
    return iRet;
}


///清空binlog管理器
void CwxBinLogMgr::clear()
{
    ///写锁保护
    CwxWriteLockGuard<CwxRwLock> lock(&m_rwLock);
    _clear();
}

void CwxBinLogMgr::_clear()
{
    if (m_pCurBinlog) delete m_pCurBinlog;
    m_pCurBinlog = NULL;
    CWX_UINT32 i = 0;
    for (i=0; i<m_arrBinlog.size(); i++)
    {
        delete m_arrBinlog[i];
    }
    if (-1 != m_fdLock)
    {
        CwxFile::unlock(m_fdLock);
        ::close(m_fdLock);
        m_fdLock = -1;
    }
    m_arrBinlog.clear();
    m_uiMaxDay = DEF_MANAGE_MAX_DAY;
    m_ullMinSid = 0; ///<binlog文件的最小sid
    m_ullMaxSid = 0; ///<binlog文件的最大sid
    m_ttMinTimestamp = 0; ///<binlog文件的log开始时间
    m_ttMaxTimestamp = 0; ///<binlog文件的log结束时间
    m_bValid = false;
    strcpy(m_szErr2K, "Not init.");
}


//NULL 失败；否则返回游标对象的指针。
CwxBinLogCursor* CwxBinLogMgr::createCurser()
{
    CwxBinLogCursor* pCursor =  new CwxBinLogCursor();
    pCursor->m_ullSid = 0;
    pCursor->m_ucSeekState = CURSOR_STATE_UNSEEK;
    pCursor->m_pBinLogFile = NULL;
    return pCursor;
}

/// -1：失败；0：无法定位到ullSid下一个binlog；1：定位到ullSid下一个的binlog上。
int CwxBinLogMgr::seek(CwxBinLogCursor* pCursor, CWX_UINT64 ullSid)
{
    ///读锁保护
    CwxReadLockGuard<CwxRwLock> lock(&m_rwLock);
    return _seek(pCursor, ullSid);
}

/// -1：失败；0：无法定位到ullSid下一个binlog；1：定位到ullSid下一个的binlog上。
int CwxBinLogMgr::_seek(CwxBinLogCursor* pCursor, CWX_UINT64 ullSid)
{
    CwxBinLogFile* pBinLogFile = NULL;
    int iRet = 0;
    pCursor->m_ullSid = ullSid;
    pCursor->m_pBinLogFile = NULL;
    if (!m_pCurBinlog || 
         (ullSid >= m_pCurBinlog->getMaxSid()))
    {///超过最大值
        pCursor->m_ucSeekState = CURSOR_STATE_UNSEEK;
        return 0;
    }

    //定位sid所在的binlog文件
    if (ullSid>=m_pCurBinlog->getMinSid())
    {///在当前binlog文件内
        pBinLogFile = m_pCurBinlog;
    }
    else
    {///查找历史数据
        for (CWX_UINT32 i=0; i<m_arrBinlog.size(); i++)
        {
            if (ullSid < m_arrBinlog[i]->getMaxSid())
            {
                pBinLogFile = m_arrBinlog[i];
                break;
            }
        }
    }
    if (!pBinLogFile) pBinLogFile = m_pCurBinlog;

    //定位cursor
    iRet = pBinLogFile->seek(*pCursor, CwxBinLogFile::SEEK_SID);
    if (1 == iRet)
    {
        pCursor->m_ucSeekState = CURSOR_STATE_READY;
        pCursor->m_pBinLogFile = pBinLogFile;
        return 1;
    }

    pCursor->m_ucSeekState = CURSOR_STATE_ERROR;
   
    if ((-1 == iRet) || (-2 == iRet)) return -1; 

    //iRet == 0
    char szBuf1[64], szBuf2[64];
    CwxCommon::snprintf(pCursor->m_szErr2K, 2047, "Sid[%s] should be found, but not. binlog-file's max-sid[%s], binlog file:%s",
        CwxCommon::toString(ullSid, szBuf1),
        CwxCommon::toString(pBinLogFile->getMaxSid(), szBuf2),
        pBinLogFile->getDataFileName().c_str());
    return -1;
}

//-1：失败；0：移到最后；1：成功移到下一个binlog。
int CwxBinLogMgr::next(CwxBinLogCursor* pCursor)
{
    ///读锁保护
    CwxReadLockGuard<CwxRwLock> lock(&m_rwLock);
    if (!isCursorValid(pCursor))
    {
        if (isUnseek(pCursor)) strcpy(pCursor->m_szErr2K, "Cursor is unseek.");
        return -1;
    }
    if (CURSOR_STATE_UNSEEK == pCursor->m_ucSeekState)
    {
        if (!m_pCurBinlog ||
            (pCursor->m_ullSid >= m_pCurBinlog->getMaxSid())) return 0;
        if (0 != _seek(pCursor, pCursor->m_ullSid)) return -1;
        CWX_ASSERT(CURSOR_STATE_READY == pCursor->m_ucSeekState);
    }
    int iRet = 0;
    if (_isOutRange(pCursor))
    {
        pCursor->m_pBinLogFile = _getMinBinLogFile();
        CWX_ASSERT(pCursor->m_pBinLogFile->getLogNum());
        iRet = pCursor->m_pBinLogFile->seek(*pCursor, CwxBinLogFile::SEEK_START);
        //iRet -2：不存在完成的记录头；-1：失败；0：不存在；1：定位到指定的位置
        if (1 == iRet) return 1;
        pCursor->m_pBinLogFile = NULL;
        pCursor->m_ucSeekState = CURSOR_STATE_ERROR;
        if (0 == iRet)
        {
            CwxCommon::snprintf(pCursor->m_szErr2K, 2047, "Seek to binlog file's start, but return 0, LogNum=%d, file=%s",
                pCursor->m_pBinLogFile->getLogNum(),
                pCursor->m_pBinLogFile->getDataFileName().c_str());
        }
        return -1;
    }
    iRet = pCursor->next();//-2：log的header不完整；-1：读取失败；0：当前log为最后一个log；1：移到下一个log
    if (1 == iRet) return 1;
    if ((-1==iRet) || (-2==iRet))
    {
        pCursor->m_pBinLogFile = NULL;
        pCursor->m_ucSeekState = CURSOR_STATE_ERROR;
        return -1;
    }
    //Now, iRet=0
    CWX_ASSERT(pCursor->getHeader().getSid() == pCursor->m_pBinLogFile->getMaxSid());
    if (pCursor->m_pBinLogFile->m_nextBinLogFile)
    {
        pCursor->m_pBinLogFile = pCursor->m_pBinLogFile->m_nextBinLogFile;
        CWX_ASSERT(pCursor->m_pBinLogFile->getLogNum());
        iRet = pCursor->m_pBinLogFile->seek(*pCursor, CwxBinLogFile::SEEK_START);
        //iRet -2：不存在完成的记录头；-1：失败；0：不存在；1：定位到指定的位置
        if (1 == iRet) return 1;
        pCursor->m_pBinLogFile = NULL;
        pCursor->m_ucSeekState = CURSOR_STATE_ERROR;
        if (0 == iRet)
        {
            CwxCommon::snprintf(pCursor->m_szErr2K, 2047, "Seek to binlog file's start, but return 0, LogNum=%d, file=%s",
                pCursor->m_pBinLogFile->getLogNum(),
                pCursor->m_pBinLogFile->getDataFileName().c_str());
        }
        return -1;
    }
    //the end
    return 0;
}

// -1：失败；0：移到最开始；1：成功移到前一个binlog。
int CwxBinLogMgr::prev(CwxBinLogCursor* pCursor)
{
    ///读锁保护
    CwxReadLockGuard<CwxRwLock> lock(&m_rwLock);
    if (!isCursorValid(pCursor))
    {
        if (isUnseek(pCursor)) strcpy(pCursor->m_szErr2K, "Cursor is unseek.");
        return -1;
    }
    if (CURSOR_STATE_UNSEEK == pCursor->m_ucSeekState)
    {
        if (!m_pCurBinlog ||
            (pCursor->m_ullSid <= m_pCurBinlog->getMinSid())) return 0;
        if (pCursor->m_ullSid >= m_pCurBinlog->getMaxSid())
        {
            if (m_pCurBinlog->getMaxSid())
            {
                pCursor->m_ullSid = m_pCurBinlog->getMaxSid() - 1;
                if (0 != _seek(pCursor, pCursor->m_ullSid)) return -1;
            }
            else
            {
                return 0;
            }
        }
        CWX_ASSERT(CURSOR_STATE_READY == pCursor->m_ucSeekState);
    }

    int iRet = 0;
    if (_isOutRange(pCursor))
    {
        pCursor->m_pBinLogFile = _getMinBinLogFile();
        CWX_ASSERT(pCursor->m_pBinLogFile->getLogNum());
        iRet = pCursor->m_pBinLogFile->seek(*pCursor, CwxBinLogFile::SEEK_START);
        //iRet -2：不存在完成的记录头；-1：失败；0：不存在；1：定位到指定的位置
        if (1 == iRet) return 0;
        pCursor->m_pBinLogFile = NULL;
        pCursor->m_ucSeekState = CURSOR_STATE_ERROR;
        if (0 == iRet)
        {
            CwxCommon::snprintf(pCursor->m_szErr2K, 2047, "Seek to binlog file's start, but return 0, LogNum=%d, file=%s",
                pCursor->m_pBinLogFile->getLogNum(),
                pCursor->m_pBinLogFile->getDataFileName().c_str());
        }
        return -1;
    }

    iRet = pCursor->prev();//-2：log的header不完整；-1：读取失败；0：当前log为最后一个log；1：移到下一个log

    if (1 == iRet) return 1;
    if ((-1==iRet) || (-2==iRet))
    {
        pCursor->m_pBinLogFile = NULL;
        pCursor->m_ucSeekState = CURSOR_STATE_ERROR;
        return -1;
    }
    //Now, iRet=0
    CWX_ASSERT(pCursor->getHeader().getSid() == pCursor->m_pBinLogFile->getMinSid());
    if (pCursor->m_pBinLogFile->m_prevBinLogFile)
    {
        pCursor->m_pBinLogFile = pCursor->m_pBinLogFile->m_prevBinLogFile;
        CWX_ASSERT(pCursor->m_pBinLogFile->getLogNum());
        iRet = pCursor->m_pBinLogFile->seek(*pCursor, CwxBinLogFile::SEEK_TAIL);
        //iRet -2：不存在完成的记录头；-1：失败；0：不存在；1：定位到指定的位置
        if (1 == iRet) return 1;
        pCursor->m_pBinLogFile = NULL;
        pCursor->m_ucSeekState = CURSOR_STATE_ERROR;
        if (0 == iRet)
        {
            CwxCommon::snprintf(pCursor->m_szErr2K, 2047, "Seek to binlog file's end, but return 0, LogNum=%d, file=%s",
                pCursor->m_pBinLogFile->getLogNum(),
                pCursor->m_pBinLogFile->getDataFileName().c_str());
        }
        return -1;
    }
    //the end
    return 0;
}

//-1：失败；0：成功获取下一条binlog。
int CwxBinLogMgr::fetch(CwxBinLogCursor* pCursor, char* szData, CWX_UINT32& uiDataLen)
{
    if (!isCursorValid(pCursor))
    {
        if (isUnseek(pCursor)) strcpy(pCursor->m_szErr2K, "Cursor is unseek.");
        return -1;
    }
    if (CURSOR_STATE_UNSEEK == pCursor->m_ucSeekState)
    {
        strcpy(pCursor->m_szErr2K, "the cursor doesn't seek.");
        return -1;
    }
    int iRet = pCursor->data(szData, uiDataLen);
    //iRet: -2：数据不完成；-1：失败；>=0：获取数据的长度
    if (iRet >= 0) return 0;
    pCursor->m_pBinLogFile = NULL;
    pCursor->m_ucSeekState = CURSOR_STATE_ERROR;
    return -1;
}

// -1：失败；0：移到最后；1：成功获取下一条binlog。
int CwxBinLogMgr::next(CwxBinLogCursor* pCursor, char* szData, CWX_UINT32& uiDataLen)
{
    int iRet = next(pCursor);
    //iRet -1：失败；0：移到最后；1：成功移到下一个binlog。
    if (1 == iRet)
    {
        iRet = fetch(pCursor, szData, uiDataLen);
        //iRet -1：失败；0：成功获取下一条binlog。
        if (0 == iRet) return 1;
        return -1;
    }
    if (0 == iRet) return 0;
    return -1;
}

// -1：失败；0：移到最开始；1：成功获取前一个binlog。
int CwxBinLogMgr::prev(CwxBinLogCursor* pCursor, char* szData, CWX_UINT32& uiDataLen)
{
    int iRet = prev(pCursor);
    //iRet -1：失败；0：移到开始；1：成功移到下一个binlog。
    if (1 == iRet)
    {
        iRet = fetch(pCursor, szData, uiDataLen);
        //iRet -1：失败；0：成功获取下一条binlog。
        if (0 == iRet) return 1;
        return -1;
    }
    if (0 == iRet) return 0;
    return -1;
}

//-1：失败；0：成功。
int CwxBinLogMgr::destoryCurser(CwxBinLogCursor*& pCursor)
{
    if (pCursor) delete pCursor;
    pCursor = NULL;
    return 0;
}

CWX_INT64 CwxBinLogMgr::leftLogNum(CwxBinLogCursor const* pCursor)
{
    ///读锁保护
    CwxReadLockGuard<CwxRwLock> lock(&m_rwLock);
    if (!pCursor) return -1;
    if (CURSOR_STATE_READY != pCursor->m_ucSeekState) return -1;
    CwxBinLogFile* pFile = pCursor->m_pBinLogFile;
    CWX_INT64 num = pFile->getLogNum() - pCursor->m_curLogHeader.getLogNo();
    pFile = pFile->m_nextBinLogFile;
    while(pFile)
    {
        num += pFile->getLogNum();
        pFile = pFile->m_nextBinLogFile;
    }
    return num;
}

//void
void CwxBinLogMgr::_outputManageBinLog()
{
    char szBuf[2048];
    char szBuf1[64];
    char szBuf2[64];
    string strTimeStamp1;
    string strTimeStamp2;
    string strFileName = m_strPrexLogPath + m_strFilePrex + "_mgr.inf";
    FILE* fd = fopen(strFileName.c_str(), "wb");
    if (fd)
    {
        CwxCommon::snprintf(szBuf, 2047, "FileNo\t\tMinSid\t\tMaxSid\t\tMinTimestamp\t\tMaxTimestamp\t\tLogNo\t\tFile\n");
        fwrite(szBuf, 1, strlen(szBuf), fd);
        if (m_pCurBinlog)
        {
            CwxCommon::snprintf(szBuf,
                2047,
                "%d\t\t%s\t\t%s\t\t%s\t\t%s\t\t%u\t\t%s\n",
                m_pCurBinlog->getFileNo(),
                CwxCommon::toString(m_pCurBinlog->getMinSid(), szBuf1),
                CwxCommon::toString(m_pCurBinlog->getMaxSid(), szBuf2),
                CwxDate::getDate(m_pCurBinlog->getMinTimestamp(), strTimeStamp1).c_str(),
                CwxDate::getDate(m_pCurBinlog->getMaxTimestamp(), strTimeStamp2).c_str(),
                m_pCurBinlog->getLogNum(),
                m_pCurBinlog->getDataFileName().c_str()
                );
            fwrite(szBuf, 1, strlen(szBuf), fd);
        }
        for (CWX_UINT32 i=m_arrBinlog.size(); i>0; i--)
        {
            CwxCommon::snprintf(szBuf,
                2047,
                "%d\t\t%s\t\t%s\t\t%s\t\t%s\t\t%u\t\t%s\n",
                m_arrBinlog[i-1]->getFileNo(),
                CwxCommon::toString(m_arrBinlog[i-1]->getMinSid(), szBuf1),
                CwxCommon::toString(m_arrBinlog[i-1]->getMaxSid(), szBuf2),
                CwxDate::getDate(m_arrBinlog[i-1]->getMinTimestamp(), strTimeStamp1).c_str(),
                CwxDate::getDate(m_arrBinlog[i-1]->getMaxTimestamp(), strTimeStamp2).c_str(),
                m_arrBinlog[i-1]->getLogNum(),
                m_arrBinlog[i-1]->getDataFileName().c_str()
                );
            fwrite(szBuf, 1, strlen(szBuf), fd);
        }
    }
    fclose(fd);
}



CWINUX_END_NAMESPACE
