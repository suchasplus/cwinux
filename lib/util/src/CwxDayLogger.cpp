#include "CwxDayLogger.h"

CWINUX_BEGIN_NAMESPACE

int CwxDayLogger::init(string const& strPath, string const& strFilePrex, CWX_UINT32 uiSwitchInternal)
{
    this->m_strPath = strPath;
    m_switchInternal = uiSwitchInternal * 60;
    if (!m_switchInternal) m_switchInternal = DEF_LOG_INTERNAL * 60;
    if (this->m_strPath[this->m_strPath.length()-1] != '/') this->m_strPath+="/";
    this->m_strFilePrex = strFilePrex;
    return switchLog();
}
int CwxDayLogger::log(char const* format, ...)
{
    FILE* fd = NULL;
    const int LEN=4095;
    char buf[LEN+1];
    struct tm tmNow;
    time_t tt = time(NULL);
    int pos = 0;

    if (tt >= m_ttCurLogTimeEnd)
    {
        if (0 != switchLog()) return -1;
    }
    localtime_r(&tt, &tmNow);
    pos += CwxCommon::snprintf(buf + pos, LEN - pos, "[%.4d-%.2d-%.2d %.2d:%.2d:%.2d]",
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec);
    va_list args;
    va_start(args, format);
    pos +=vsnprintf(buf + pos, LEN - pos, format, args);
    va_end(args);
    buf[pos++]='\n';
    //write log
    fd = m_curFd;
    fwrite(buf, 1, pos, fd);
    fflush(fd);
    return 0;
}

int CwxDayLogger::log(char* buf, CWX_UINT32 uiBufLen, char const* format, ...)
{
    FILE* fd = NULL;
    int LEN= uiBufLen - 1;
    struct tm tmNow;
    time_t tt = time(NULL);
    int pos = 0;

    if (tt >= m_ttCurLogTimeEnd)
    {
        if (0 != switchLog()) return -1;
    }
    localtime_r(&tt, &tmNow);
    pos += CwxCommon::snprintf(buf + pos, LEN - pos, "[%.4d-%.2d-%.2d %.2d:%.2d:%.2d]",
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec);
    va_list args;
    va_start(args, format);
    pos +=vsnprintf(buf + pos, LEN - pos, format, args);
    va_end(args);
    buf[pos++]='\n';
    //write log
    fd = m_curFd;
    fwrite(buf, 1, pos, fd);
    fflush(fd);
    return 0;
}

int CwxDayLogger::rawLog(char const* format, ...)
{
    FILE* fd = NULL;
    const int LEN=4095;
    char buf[LEN+1];
    time_t tt = time(NULL);
    int pos = 0;
    if (tt >= m_ttCurLogTimeEnd)
    {
        if (0 != switchLog()) return -1;
    }
    va_list args;
    va_start(args, format);
    pos +=vsnprintf(buf + pos, LEN - pos, format, args);
    va_end(args);
    buf[pos++]='\n';
    //write log
    fd = m_curFd;
    fwrite(buf, 1, pos, fd);
    fflush(fd);
    return 0;
}

int CwxDayLogger::rawLog(char* buf, CWX_UINT32 uiBufLen, char const* format, ...)
{
    FILE* fd = NULL;
    const int LEN=uiBufLen-1;
    int pos = 0;
    time_t tt = time(NULL);
    if (tt >= m_ttCurLogTimeEnd)
    {
        if (0 != switchLog()) return -1;
    }
    va_list args;
    va_start(args, format);
    pos +=vsnprintf(buf + pos, LEN - pos, format, args);
    va_end(args);
    buf[pos++]='\n';
    //write log
    fd = m_curFd;
    fwrite(buf, 1, pos, fd);
    fflush(fd);
    return 0;

}

int CwxDayLogger::switchLog()
{
    string strDate;
    char szPathFile[512];
    time_t ttNow;
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    time(&ttNow);
    CwxDate::getDateY4MDHMS2(ttNow, strDate);
    if (ttNow >= this->m_ttCurLogTimeEnd)
    {//switch
        m_ttCurLogTimeBegin = (ttNow - ttNow%m_switchInternal);
        m_ttCurLogTimeEnd = m_ttCurLogTimeBegin + m_switchInternal;
        if (m_prevFd) fclose(m_prevFd);
        m_prevFd = m_curFd;
        sprintf(szPathFile, "%s%s_%s.log", this->m_strPath.c_str(), this->m_strFilePrex.c_str(), strDate.c_str());
        this->m_curFd = fopen(szPathFile, "a+");
        if (NULL == this->m_curFd)
        {
            sprintf(this->m_szErrMsg, "Can't open log file:%s", szPathFile);
            return -1;
        }
    }
    return 0;
}

CWINUX_END_NAMESPACE
