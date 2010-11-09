#ifndef __CWX_DAY_LOGGER_H__
#define __CWX_DAY_LOGGER_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
*@file  CwxDayLogger.h
*@brief  定义了CwxDayLogger类，实现日志按照最大范围为1天，最小为1分钟的分文件记录
*@author cwinux@gmail.com
*@version 0.1
*@date    2009-10-19
*@warning 无
*@bug   
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxCommon.h"
#include "CwxMutexLock.h"
#include "CwxLockGuard.h"
#include "CwxDate.h"
#include "CwxUtilMacro.h"

CWINUX_BEGIN_NAMESPACE
/**
*@class  CwxDayLogger
*@brief  管理日志按照最大范围为1天，最小为1分钟的分文件记录。
*        每到指定的时刻，系统会自动切换日志文件，日志文件的命名为【文件名_yyyymmddH24MMSS.suffix】
*        此对象是多线程安全的。
*/
class CwxDayLogger
{
public:
    enum{
        DEF_LOG_INTERNAL = 1440///<缺省一天一个日志文件
    };
    ///构造函数
    CwxDayLogger()
    {
        m_switchInternal = DEF_LOG_INTERNAL * 60;
        m_ttCurLogTimeBegin = 0;
        m_ttCurLogTimeEnd = 0;
        m_curFd = NULL;
        m_prevFd = NULL;
        memset(m_szErrMsg, 0x00, 511);
    }
    ///析构函数
    ~CwxDayLogger()
    {
        if (m_curFd) fclose(m_curFd);
        if (m_prevFd) fclose(m_prevFd);
    }
public:
    /**
    *@brief  对象初始化.
    *@param [in] strPath 日志文件的路径+主文件名.
    *@param [in] strFilePrex 日志文件的后缀，形成的文件名为[strPath_yyyymmddH24MMSS.strFilePrex].
    *@param [in] uiSwitchInternal 日志文件的切换时间间隔，单位为分钟.
    *@return 0:success; -1:failure.
    */
    int init(string const& strPath, string const& strFilePrex, CWX_UINT32 uiSwitchInternal=DEF_LOG_INTERNAL);
    /**
    *@brief  将日志写到日志文件中，若当前时间为当前log的范围，则使用当前日志文件，否则切换下一个.
             支持的日志记录最长为4K。
             其支持变参，其格式与参数同printf()。此API会为每条记录自动加回车。
             同时，API会为每条记录增加一个[yyyy-mm-dd h24:mm:ss]的时间戳
    *@param [in] format 日志文件的格式后字符串.
    *@param [in] ... 变参参数.
    *@return 0:success; -1:failure.
    */
    int log(char const* format, ...);
    /**
    *@brief  同log(char const* format, ...)，其差别是buf有外部设定
    *@param [in] buf 日志的buf.
    *@param [in] uiBufLen buf的空间.
    *@param [in] format 日志文件的格式后字符串.
    *@param [in] ... 变参参数.
    *@return 0:success; -1:failure.
    */
    int log(char* buf, CWX_UINT32 uiBufLen, char const* format, ...);

    /**
    *@brief  将日志写到日志文件中，若当前时间为当前log的范围，则使用当前日志文件，否则切换下一个.
    支持的日志记录最长为4K。
    其支持变参，其格式与参数同printf()。此API会为每条记录自动加回车。
    与log的差别是不会自动增加时间戳。
    *@param [in] format 日志文件的格式后字符串.
    *@param [in] ... 变参参数.
    *@return 0:success; -1:failure.
    */
    int rawLog(char const* format, ...);
    /**
    *@brief  同rawLog(char const* format, ...)，其差别是buf有外部设定
    *@param [in] buf 日志的buf.
    *@param [in] uiBufLen buf的空间.
    *@param [in] format 日志文件的格式后字符串.
    *@param [in] ... 变参参数.
    *@return 0:success; -1:failure.
    */
    int rawLog(char* buf, CWX_UINT32 uiBufLen, char const* format, ...);

    ///初始化错误的时候，获取错误的原因。
    char const* getErrMsg() const
    {
        return m_szErrMsg;
    }
private:
    ///切换日志文件，ttDay为新日志文件所属的日期
    int switchLog();
private:
    string              m_strPath;  ///<日志文件的路径+主文件名
    string              m_strFilePrex;///<日志文件的后缀
    CWX_UINT32          m_switchInternal; ///<日志文件的切换时间间隔，单位为s
    time_t              m_ttCurLogTimeBegin; ///<当前日志文件的日期
    time_t              m_ttCurLogTimeEnd; ///<当前日志文件的日期
    CwxMutexLock         m_lock; ///<用于文件切换时候的保护锁
    FILE *              m_curFd;    ///<当前的日志文件fd
    FILE *              m_prevFd; ///<前一个的日志文件fd
    char                m_szErrMsg[512];///<保存init的错误日志
};


CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif
