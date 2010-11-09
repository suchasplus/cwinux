#ifndef __CWX_BIN_LOG_MGR_H__
#define __CWX_BIN_LOG_MGR_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxBinLogMgr.h
@brief binlog 管理器的定义文件
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/

#include "CwxStoreMacro.h"
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxCommon.h"
#include "CwxFile.h"
#include "CwxDate.h"
#include "CwxRwLock.h"
#include "CwxLockGuard.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxBinLogHeader
@brief binlog的header对象定义
*/
class CwxBinLogHeader
{
public:
    enum{
        BIN_LOG_HEADER_SIZE = 48 ///<serialize的空间字节数
    };
public:
    ///缺省构造函数
    inline CwxBinLogHeader();
    ///构造函数
    inline CwxBinLogHeader(CWX_UINT64 ullSid, CWX_UINT32 uiDatetime, CWX_UINT64 ullOffset,
                   CWX_UINT32 uiLogLen, CWX_UINT32 uiLogNo, CWX_UINT64 ullPrevOffset,
                   CWX_UINT32 uiGroup, CWX_UINT32 uiType, CWX_UINT32 uiAttr);
    ///拷贝构造
    inline CwxBinLogHeader(CwxBinLogHeader const& header);
    ///赋值操作
    inline CwxBinLogHeader& operator=(CwxBinLogHeader const& header);
    ///比较操作,true：小于；false：不小于
    inline bool operator<(CwxBinLogHeader const& header) const;
public:
    ///设置sid
    inline void setSid(CWX_UINT64 ullSid);
    ///获取sid
    inline CWX_UINT64 getSid() const;
    ///设置log的时间戳
    inline void setDatetime(CWX_UINT32 uiDatetime);
    ///获取log的时间戳
    inline CWX_UINT32 getDatetime() const;
    ///设置log的文件偏移
    inline void setOffset(CWX_UINT64 ullOffset);
    ///获取log的文件偏移
    inline CWX_UINT64 getOffset() const;
    ///设置log的长度，不包括log header的长度
    inline void setLogLen(CWX_UINT32 uiLogLen);
    ///获取log的长度，不包括log header的长度
    inline CWX_UINT32 getLogLen() const;
    ///设置log在文件中的记录号
    inline void setLogNo(CWX_UINT32 uiLogNo);
    ///获取log在文件中的记录号
    inline CWX_UINT32 getLogNo() const;
    ///设置前一个log的offset
    inline void setPrevOffset(CWX_UINT64 ullPrevOffset);
    ///获取前一个log的offset
    inline CWX_UINT64 getPrevOffset() const;
    ///设置binlog的分组
    inline void setGroup(CWX_UINT32 uiGroup);
    ///获取binlog的分组
    inline CWX_UINT32 getGroup() const;
    ///设置binlog的类型
    inline void setType(CWX_UINT32 uiType);
    ///获取binlog的类型
    inline CWX_UINT32 getType() const;
    ///设置binlog的属性
    inline void setAttr(CWX_UINT32 uiAttr);
    ///获取binlog的属性
    inline CWX_UINT32 getAttr() const;
public:
    ///将log header对象序列化，返回序列化占用的空间字节数
    inline CWX_UINT32 serialize(char* szBuf) const;
    ///将log header的buf反序列化，返回序列化占用的空间字节数
    inline CWX_UINT32 unserialize(char const* szBuf);
    ///清空对象
    inline void reset();
private:
    CWX_UINT64    m_ullSid; ///<同步序列号
    CWX_UINT32    m_uiDatetime; ///<记录的时间戳
    CWX_UINT64    m_ullOffset; ///<记录的文件偏移
    CWX_UINT32    m_uiLogLen; ///<记录的长度，不包括log header的长度
    CWX_UINT32    m_uiLogNo; ///<记录的记录号
    CWX_UINT64    m_ullPrevOffset; ///<前一个记录的文件偏移
    CWX_UINT32    m_uiGroup; ///<binlog的group
    CWX_UINT32    m_uiType; ///<binlog的类型
    CWX_UINT32    m_uiAttr; ///<binlog的属性
};


/**
@class CwxBinLogIndex
@brief binlog的index对象定义
*/
class CwxBinLogIndex
{
public:
    enum{
        BIN_LOG_INDEX_SIZE = 24 ///<serialize的空间字节数
    };
public:
    ///缺省构造
    inline CwxBinLogIndex();
    ///构造函数
    inline CwxBinLogIndex(CWX_UINT64 ullSid, CWX_UINT32 uiDatetime, CWX_UINT64 ullOffset, CWX_UINT32 uiLogLen);
    ///拷贝构造
    inline CwxBinLogIndex(CwxBinLogIndex const& index);
    ///拷贝构造
    inline CwxBinLogIndex(CwxBinLogHeader const& header);
    ///赋值操作
    inline CwxBinLogIndex& operator=(CwxBinLogIndex const& index);
    ///赋值操作
    inline CwxBinLogIndex& operator=(CwxBinLogHeader const& header);
    ///比较操作。true：小于；false：不小于
    inline bool operator<(CwxBinLogIndex const& index) const;
public:
    ///设置sid
    inline void setSid(CWX_UINT64 ullSid);
    ///获取sid
    inline CWX_UINT64 getSid() const;
    ///设置log的时间戳
    inline void setDatetime(CWX_UINT32 uiDatetime);
    ///获取log的时间戳
    inline CWX_UINT32 getDatetime() const;
    ///设置log的文件偏移
    inline void setOffset(CWX_UINT64 ullOffset);
    ///获取log的文件偏移
    inline CWX_UINT64 getOffset() const;
    ///设置log的长度，不包括log header的长度
    inline void setLogLen(CWX_UINT32 uiLogLen);
    ///获取log的长度，不包括log header的长度
    inline CWX_UINT32 getLogLen() const;
public:
    ///将log header对象序列化，返回序列化占用的空间字节数
    inline CWX_UINT32 serialize(char* szBuf) const;
    ///将log header的buf反序列化，返回序列化占用的空间字节数
    inline CWX_UINT32 unserialize(char const* szBuf);
    ///清空对象
    inline void reset();
private:
    CWX_UINT64    m_ullSid; ///<同步序列号
    CWX_UINT32    m_uiDatetime; ///<binlog的timestamp
    CWX_UINT64    m_ullOffset; ///<记录的文件偏移
    CWX_UINT32    m_uiLogLen; ///<记录的长度，不包括log header的长度
};

class CwxBinLogFile;
class CwxBinLogMgr;


/**
@class CwxBinLogCursor
@brief BinLog 文件的读取游标
*/
class CwxBinLogCursor
{
public:
    ///构造函数
    CwxBinLogCursor();
    ///析构函数
    ~CwxBinLogCursor();
public:
    /**
    @brief 打开bin-log文件，此时cursor没有定位到任何记录，此时若调用next或prev，则会定位到第一个记录上。
           调用seek会定位到指定的offset上。
    @param [in] szFileName bin-log的文件名。
    @return -1：失败；0：成功
    */
    int open(char const* szFileName);
    /**
    @brief 移到下一个log
    @return -2：log的header不完整；-1：读取失败；0：当前log为最后一个log；1：移到下一个log
    */
    inline int next();
    /**
    @brief 移到前一个log
    @return -2：log的header不完整；-1：读取失败；0：当前log为第一个log；1：移到前一个log
    */
    inline int prev();
    /**
    @brief 文件偏移到指定的offset，offset的位置必须为一个log的开始位置
    @param [in] ullOffset binlog在文件中的offset。
    @return -2：log的header不完整；-1：读取失败；0：超出范围；1：移到指定的offset
    */
    inline int seek(CWX_UINT64 ullOffset);
    /**
    @brief 获取当前log的data
    @param [in] szBuf binlog的buf。
    @param [in,out] uiBufLen 传入szBuf的大小，返回读取数据的长度。
    @return  -2：数据不完成；-1：失败；>=0：获取数据的长度
    */
    int data(char * szBuf, CWX_UINT32& uiBufLen);
    ///关闭cursor
    void close();
    ///获取cursor是否处于悬空状态
    inline bool isDangling() const;
    ///获取当期log的记录头
    inline CwxBinLogHeader const& getHeader() const;
    ///获取文件的名字
    inline string const& getFileName() const;
    ///获取文件的日期
    inline CWX_UINT32 getFileDay() const;
    ///获取文件的序号
    inline CWX_UINT32 getFileNo() const;
    ///获取当前的错误信息
    inline char const* getErrMsg() const;
private:
    friend class CwxBinLogFile;
    friend class CwxBinLogMgr;
    /**
    @brief 从指定位置，读取log的header。
    @param [in] uiOffset binlog在文件中的offset。
    @return -2：不存在完成的记录头；-1：失败；0：结束；1：读取一个
    */
    int header(CWX_UINT64 ullOffset);
    //获取cursor的文件 io handle
    inline int getHandle() const;
private:
    string             m_strFileName; ///<文件的名字
    int                m_fd;///<文件的handle
    bool               m_bDangling; ///<cursor是否处于悬空状态
    CwxBinLogHeader     m_curLogHeader; ///<当前log的header
    char               m_szHeadBuf[CwxBinLogHeader::BIN_LOG_HEADER_SIZE]; ///<log header的buf空间
    char               m_szErr2K[2048];///<错误信息buf
    //由CwxBinLogMgr使用的状态值
    CWX_UINT64          m_ullSid; ///<seek的sid
    CWX_UINT8           m_ucSeekState; ///<seek的状态
    CwxBinLogFile*      m_pBinLogFile;

};


/**
@class CwxBinLogFile
@brief BinLog文件关系对象，此对象负责一个binlog的数据、索引文件的管理，负责binlog文件的读操作。
*/
class CwxBinLogFile
{
public:
    enum{
        SEEK_START = 0, ///<将光标移到文件的开头
        SEEK_TAIL = 1, ///<将光标移到文件的最后位置
        SEEK_SID = 2 ///<将光标移到文件的指定的SID
    };
    enum{
        MIN_BINLOG_FILE_SIZE = 256 * 1024 * 1024, ///<满的binlog文件最小为256M
        FREE_BINLOG_FILE_SIZE = 4 * 1024 * 1024 ///<binlog预留的空间为4M
    };
public:
    /**
    @brief 构造函数
    @param [in] ttDay binlog文件的日期。
    @param [in] uiFileNo binlog文件的序号。
    @param [in] ullMaxFileSize binlog文件的最大大小。
    @return 0：成功；-1：失败。
    */
    CwxBinLogFile(CWX_UINT32 ttDay, CWX_UINT32 uiFileNo=0, CWX_UINT64 ullMaxFileSize=2*1024*1024);
    ///析构函数
    ~CwxBinLogFile();
    ///小于比较
    inline bool operator < (CwxBinLogFile const& obj) const;

public:
    /**
    @brief 打开或创建新日志文件
    @param [in] szPathFile 日志文件名。
    @param [in] bReadOnly 是否以只读方式打开，此时，文件必须存在。
    @param [in] bCreate 是否创建新日志文件，此时，日志文件必须不存在。
    @param [in] szErr2K 错误信息buf，若为NULL则不返回错误消息。
    @return 0：成功；-1：失败。
    */
    int open(char const* szPathFile, bool bReadOnly = true, bool bCreate=false, char* szErr2K=NULL);
    /**
    @brief 往日志文件添加一个新日志
    @param [in] ullSid 日志的sid号。
    @param [in] ttTimestamp 日志的日期
    @param [in] uiGroup binlog的分组
    @param [in] uiType 日志的类型
    @param [in] uiAttr 日志的属性
    @param [in] szData 日志内容
    @param [in] uiDataLen 日志长度。
    @param [in] szErr2K 错误信息buf，若为NULL则不返回错误消息。
    @return -1：失败；0：日志文件满了；1：成功。
    */
    int append(CWX_UINT64 ullSid,
        CWX_UINT32 ttTimestamp,
        CWX_UINT32 uiGroup,
        CWX_UINT32 uiType,
        CWX_UINT32 uiAttr,
        char const* szData,
        CWX_UINT32 uiDataLen,
        char* szErr2K=NULL);
    /**
    @brief 确保写入的日志保存到硬盘
    @param [in] szErr2K 错误信息buf，若为NULL则不返回错误消息。
    @return -1：失败；0：成功。
    */
    int commit(char* szErr2K=NULL);
    /**
    @brief 定位Cursor的位置
    @param [in] cursor 日志读handle。
    @param [in] ucMode 定位的模式，SEEK_START：定位到文件的开头；SEEK_TAIL：定位到文件的最后；SEEK_SID：定位到第一个大于cursor.getSid()的日志处。
    @return -2：不存在完成的记录头；-1：失败；0：不存在；1：定位到指定的位置
    */
    int seek(CwxBinLogCursor& cursor, CWX_UINT8 ucMode=SEEK_SID);
    /**
    @brief 删除指定的binlog文件及其索引文件
    @param [in] szPathFileName binlog文件名。
    @return void。
    */
    static void remove(char const* szPathFileName);
    //关闭
    void close();
public:
    ///获取最小的sid
    inline CWX_UINT64 getMinSid() const;
    ///获取最大的sid
    inline CWX_UINT64 getMaxSid() const;
    ///获取binlog的最小时间戳
    inline CWX_UINT32 getMinTimestamp() const;
    ///获取binlog的最大时间戳
    inline CWX_UINT32 getMaxTimestamp() const;
    ///获取binlog文件的日期
    inline CWX_UINT32 getFileDay() const;
    ///获取binlog的文件序号
    inline CWX_UINT32 getFileNo() const;
    ///获取binlog文件的log记录数
    inline CWX_UINT32 getLogNum() const;
    ///获取binlog文件的大小
    inline CWX_UINT64 getFileSize() const;
    ///判断是否只读
    inline bool readOnly() const;
    ///设置只读
    inline void setReadOnly();
    ///判断日志文件是否为空
    inline bool empty() const;
    ///获取数据文件的名字
    inline string const& getDataFileName() const;
    ///获取索引文件的名字
    inline string const& getIndexFileName() const;
private:
    ///清空对象
    void reset();
    /**
    @brief 读取指定位置的索引记录
    @param [in] fd 索引文件的fd。
    @param [out] index 返回的索引。
    @param [in] ullOffset 索引的位置。
    @param [in] szErr2K 错误信息buf，若为NULL则不返回错误消息。
    @return -1：失败；0：成功。
    */
    inline int readIndex(int fd, CwxBinLogIndex& index, CWX_UINT64 ullOffset, char* szErr2K=NULL) const;
    /**
    @brief 往指定的位置写入索引
    @param [in] fd 索引文件的fd。
    @param [in] index 写入的索引。
    @param [in] ullOffset 索引的位置。
    @param [in] szErr2K 错误信息buf，若为NULL则不返回错误消息。
    @return -1：失败；0：成功。
    */
    inline int writeIndex(int fd, CwxBinLogIndex const& index, CWX_UINT64 ullOffset, char* szErr2K=NULL) const;
    /**
    @brief 创建指定的binlog文件
    @param [in] szErr2K 错误信息buf，若为NULL则不返回错误消息。
    @return -1：失败；0：成功。
    */
    int mkBinlog(char* szErr2K=NULL);
    /**
    @brief 检查binlog文件及其索引文件是否一致，若不一致则进行处理
    @param [in] szErr2K 错误信息buf，若为NULL则不返回错误消息。
    @return -1：失败；0：成功。
    */
    int prepareFile(char* szErr2K=NULL);
    /**
    @brief 检查是否需要创建索引
    @param [in] szErr2K 错误信息buf，若为NULL则不返回错误消息。
    @return -1：失败；0：不需要；1：需要。
    */
    int isRebuildIndex(char* szErr2K=NULL);
    /**
    @brief 创建索引
    @param [in] szErr2K 错误信息buf，若为NULL则不返回错误消息。
    @return -1：失败；0：成功。
    */
    int createIndex(char* szErr2K=NULL);
    friend class CwxBinLogMgr;
private:
    bool           m_bValid;       ///<是否有效
    string         m_strPathFileName; ///<binlog文件的名字
    string         m_strIndexFileName; ///<index文件的名字
    CWX_UINT64     m_ullMaxFileSize; ///<新建立的binlog文件的最大大小。
    CWX_UINT64     m_ullMinSid; ///<binlog文件的最小sid
    volatile CWX_UINT64     m_ullMaxSid; ///<binlog文件的最大sid
    CWX_UINT32              m_ttMinTimestamp; ///<binlog文件的log开始时间
    volatile CWX_UINT32     m_ttMaxTimestamp; ///<binlog文件的log结束时间
    volatile CWX_UINT32		m_uiLogNum; ///<binlog文件的log数量
    bool           m_bReadOnly; ///<是否为只读
    int            m_fd; ///<log文件的io handle
    int            m_indexFd; ///<索引文件的io handle
    volatile CWX_UINT64      m_ullFileSize; ///<binlog数据文件大小，-1表示不存在
    volatile CWX_UINT64      m_ullIndexFileSize; ///<索引文件的大小，-1表示不存在
    volatile CWX_UINT64      m_ullPrevLogOffset; ///<前一个binlog的偏移
    CWX_UINT32          m_ttDay; ///日志文件的日期
    CWX_UINT32      m_uiFileNo; ///<日志所在日期的编号。
    //下面的变量由CwxBinLogMgr使用
    CwxBinLogFile*  m_prevBinLogFile; ///<前一个binlog文件,由CwxBinLogMgr管理
    CwxBinLogFile*  m_nextBinLogFile; ///<下一个binlog文件，由CwxBinLogMgr管理
};


/**
@class CwxBinLogMgr
@brief BinLog的管理对象，管理binlog的读、写。binlog的命名规则为prex_xxxxxxxxxx.log。<br>
       其对应的索引文件的名字为prex_xxxxxxxxxx.log.idx。
*/
class CwxBinLogMgr
{
private:
    class CwxBinLogFileItem
    {
    public:
        CwxBinLogFileItem(CWX_UINT32 day, CWX_UINT32 no):m_uiDay(day), m_uiNo(no){}
        bool operator == (CwxBinLogFileItem const& item) const
        {
            return (m_uiDay == item.m_uiDay) && (m_uiNo == item.m_uiNo);
        }
        bool operator < (CwxBinLogFileItem const& item) const
        {
            if (m_uiDay < item.m_uiDay) return true;
            if (m_uiDay > item.m_uiDay) return false;
            return m_uiNo < item.m_uiNo;
        }
    public:
        CWX_UINT32 getDay() const { return m_uiDay;}
        CWX_UINT32 getFileNo() const { return m_uiNo;}

    private:
        CWX_UINT32   m_uiDay;
        CWX_UINT32   m_uiNo;
    };
public:
    enum{
        DEF_MANAGE_MAX_DAY=30, ///<缺省管理binlog的最多天数
        MIN_MANAGE_DAY = 1, ///<管理binlog的最小天数
        MAX_MANAGE_DAY = 180, ///<管理binlog的最大天数
        MIN_SID_NO = 1 ///<最小的sid序号
    };
    enum{
        CURSOR_STATE_UNSEEK = 0, ///<cursor处于未定位的状态
        CURSOR_STATE_READY = 1, ///<cursor处于定位的状态
        CURSOR_STATE_ERROR = 2 ///<cursor处于出错的状态
    };
public:
    /**
    @brief 构造函数。
    @param [in] szLogPath binlog文件所在的目录。
    @param [in] szFilePrex binlog文件的前缀，形成的文件名为szFilePrex_xxxxxxxxxx，xxxxxxxxxx为文件序号。
    @param [in] ullMaxFileSize binlog文件的最大大小。
    @return 无。
    */
    CwxBinLogMgr(char const* szLogPath, char const* szFilePrex, CWX_UINT64 ullMaxFileSize);
    ///析构函数
    ~CwxBinLogMgr();
public:
    /**
    @brief 初始化binlog管理器对象。
    @param [in] uiMaxDay 管理的binlog的最多天数。
    @param [out] szErr2K 若初始化失败，返回失败的错误信息；若为NULL，即便失败也不返回错误的原因。
    @return -1：失败；0：成功。
    */
    int init(CWX_UINT32 uiMaxDay, char* szErr2K=NULL);
    /**
    @brief 添加一条binlog。
    @param [in] ullSid binlog的sid，其值必须大于当前已有的最大值。
    @param [in] ttTimestamp binlog的时间戳，通过此时间戳，控制binlog同步的天数。
    @param [in] uiType 日志的分组
    @param [in] uiType 日志的类型
    @param [in] uiAttr 日志的属性
    @param [in] szData binlog的数据。
    @param [in] uiDataLen binlog的数据的长度。
    @param [in] szErr2K 若添加失败，则为失败的原因信息。
    @return -1：失败；0：成功。
    */
    int append(CWX_UINT64 ullSid,
        CWX_UINT32 ttTimestamp,
        CWX_UINT32 uiGroup,
        CWX_UINT32 uiType,
        CWX_UINT32 uiAttr,
        char const* szData,
        CWX_UINT32 uiDataLen,
        char* szErr2K=NULL);
    /**
    @brief 刷新已经写入的binlog，确保保存到硬盘。
    @param [in] szErr2K 若刷新失败，则返回失败的原因。
    @return -1：失败；0：成功。
    */
    int commit(char* szErr2K=NULL);
    ///清空binlog管理器
    void clear();
public:
    /**
    @brief 创建binlog读取的游标
    @return NULL：失败；否则返回游标对象的指针。
    */
    CwxBinLogCursor* createCurser();
    
    /**
    @brief 将binlog读取的游标移到>ullSid的binlog处。
    @param [in] pCursor binlog的读取游标。
    @param [in] ullSid 定位游标的sid，游标将定位到>ullSid的binlog处。
    @return -1：失败；0：无法定位到ullSid下一个binlog；1：定位到ullSid下一个的binlog上。
    */
    int seek(CwxBinLogCursor* pCursor, CWX_UINT64 ullSid);

    /**
    @brief 将游标移到下一个binlog记录处。若有错误，则通过pCursor的getErrMsg()获取。
    @param [in] pCursor 游标的对象指针。
    @return -1：失败；0：移到最后；1：成功移到下一个binlog。
    */
    int next(CwxBinLogCursor* pCursor);
    /**
    @brief 将游标移到前一个binlog记录处。若有错误，则通过pCursor的getErrMsg()获取。
    @param [in] pCursor 游标的对象指针。
    @return -1：失败；0：移到最开始；1：成功移到前一个binlog。
    */
    int prev(CwxBinLogCursor* pCursor);

    /**
    @brief 读取游标的当前binlog。若有错误，则通过pCursor的getErrMsg()获取。
    @param [in] pCursor 游标的对象指针。
    @param [out] szData binlog的data。
    @param [in,out] uiDataLen 传入szData的buf大小，传出szData的大小。
    @return -1：失败；0：成功获取下一条binlog。
    */
    int fetch(CwxBinLogCursor* pCursor, char* szData, CWX_UINT32& uiDataLen);
    /**
    @brief 获取下一条记录。若有错误，则通过pCursor的getErrMsg()获取。
    @param [in] pCursor 游标的对象指针。
    @param [out] szData binlog的data。
    @param [in,out] uiDataLen 传入szData的buf大小，传出szData的大小。
    @return -1：失败；0：移到最后；1：成功获取下一条binlog。
    */
    int next(CwxBinLogCursor* pCursor, char* szData, CWX_UINT32& uiDataLen);
    /**
    @brief 获取前一个binlog记录。若有错误，则通过pCursor的getErrMsg()获取。
    @param [in] pCursor 游标的对象指针。
    @param [out] szData binlog的data。
    @param [in,out] uiDataLen 传入szData的buf大小，传出szData的大小。
    @return -1：失败；0：移到最开始；1：成功获取前一个binlog。
    */
    int prev(CwxBinLogCursor* pCursor, char* szData, CWX_UINT32& uiDataLen);
    /**
    @brief 释放cursor。
    @param [in] pCursor 要释放的游标。
    @return -1：失败；0：成功。
    */
    int destoryCurser(CwxBinLogCursor*& pCursor);
    /**
    @brief 获取还没有读取的日志数量。
    @param [in] pCursor 游标。
    @return -1：游标无效；否则为记录的数量。
    */
    CWX_INT64 leftLogNum(CwxBinLogCursor const* pCursor);

public:
    ///获取管理器是否有效
    inline bool isInvalid() const;
    ///cursor对应的文件，是否在管理的范围之外
    inline bool isOutRange(CwxBinLogCursor* pCursor);
    ///获取cursor是否unseek
    inline bool isUnseek(CwxBinLogCursor* pCursor);
    ///检查cursor是否有效
    inline bool isCursorValid(CwxBinLogCursor* pCursor) ;
    ///获取管理器无效的原因
    char const* getInvalidMsg() const;
    ///获取最小的sid
    inline CWX_UINT64 getMinSid() ;
    ///获取最大的sid
    inline CWX_UINT64 getMaxSid() ;
    ///获取binlog的最小时间戳
    inline CWX_UINT32 getMinTimestamp() ;
    ///获取binlog的最大时间戳
    inline CWX_UINT32 getMaxTimestamp();
    ///获取管理的binlog的最小文件序号
    inline string& getMinFile(string& strFile) ;
    ///获取管理的binlog的最大文件序号
    inline string& getMaxFile(string& strFile);
    ///检查是否为空
    inline bool empty() ;
    ///获取文件号对应的binlog文件名
    inline string& getFileNameByFileNo(CWX_UINT32 uiFileNo, CWX_UINT32 ttDay, string& strFileName);
    ///获取文件号对应的binlog文件的索引文件名
    inline string& getIndexFileNameByFileNo(CWX_UINT32 uiFileNo, CWX_UINT32 ttDay, string& strFileName);
    ///根据binlog文件名，获取文件号
    inline CWX_UINT32 getBinLogFileNo(string const& strFileName, CWX_UINT32& ttDay);
    ///判断一个文件名是否是一个binlog文件
    inline bool isBinLogFile(string const& strFileName);
    ///判断一个文件名是否是一个binlog的索引文件
    inline bool isBinLogIndexFile(string const& strFileName);
    ///设置binlog的起始日期
    inline void setBinlogDayStart(CWX_UINT32 ttDay);
    ///获取binlog的结束日期
    inline CWX_UINT32 getBinlogDayStart() const;
    ///获取binlog的前缀名
    inline  string const& getBinlogPrexName() const;
    ///是否有效的前缀名
    static inline bool isValidPrexName(char const* szName);
private:
    ///清空binlog管理器
    void _clear();
    /**
    @brief 将binlog读取的游标移到>ullSid的binlog处。
    @param [in] pCursor binlog的读取游标。
    @param [in] ullSid 定位游标的sid，游标将定位到>ullSid的binlog处。
    @return -1：失败；0：无法定位到ullSid下一个binlog；1：定位到ullSid下一个的binlog上。
    */
    int _seek(CwxBinLogCursor* pCursor, CWX_UINT64 ullSid);
    ///检查一个binlog文件是否应该被管理
    inline bool _isManageBinLogFile(CwxBinLogFile* pBinLogFile) ;
    ///cursor对应的文件，是否在管理的范围之外
    inline bool _isOutRange(CwxBinLogCursor*& pCursor);
    ///获取最小的binlog文件
    inline CwxBinLogFile* _getMinBinLogFile() ;
    ///获取最大的binlog文件
    inline CwxBinLogFile* _getMaxBinLogFile() ;
    ///输出管理的binlog文件信息
    void  _outputManageBinLog() ;

private:
    string					  m_strLogPath; ///<binlog文件的根目录
    string                    m_strPrexLogPath; ///<指定前缀的binlog文件的目录
    string                   m_strFilePrex; ///<binlog文件的前缀名
    CWX_UINT64               m_ullMaxFileSize; ///<binlog文件的最大大小
    CWX_UINT32               m_uiMaxDay; ///<管理的binlog的最小天数
    CWX_UINT32               m_ttDayStart; ///<binlog的起始日期
    char                     m_szErr2K[2048]; ///<binlog 管理器无效的原因
    int                      m_fdLock; ///<系统锁文件句柄

    CwxRwLock                m_rwLock; ///<binlog的读写锁
    ///一下变量都在读写锁保护之中
    bool                     m_bValid; ///<binlog 管理器是否有效。
    vector<CwxBinLogFile*>   m_arrBinlog; ///<不包含当前binlog文件的binlog文件数组，按照FileNo升序，也是sid的升序
    CwxBinLogFile*           m_pCurBinlog;///<当前写的binlog文件
    CWX_UINT64               m_ullMinSid; ///<binlog文件的最小sid
    CWX_UINT64               m_ullMaxSid; ///<binlog文件的最大sid
    CWX_UINT32               m_ttMinTimestamp; ///<binlog文件的log开始时间
    CWX_UINT32               m_ttMaxTimestamp; ///<binlog文件的log结束时间
};

CWINUX_END_NAMESPACE
#include "CwxBinLogMgr.inl"
#include "CwxPost.h"
#endif
