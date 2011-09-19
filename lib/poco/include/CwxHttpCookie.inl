CWINUX_BEGIN_NAMESPACE
///设置cookie的版本号，缺省为0
inline void CwxHttpCookie::setVersion(int version)
{
    m_iVersion = version;
}
///获取cookie的版本号
inline int CwxHttpCookie::getVersion() const
{
    return m_iVersion;
}
///设置cookie的名字		
inline void CwxHttpCookie::setName(char const* name)
{
    if (name)
    {
        m_strName = name;
    }
    else
    {
        m_strName.erase();
    }
}
///获取cookie的名字		
inline string const& CwxHttpCookie::getName() const
{
    return m_strName;
}
///设置cookie的数值，其长度不能超过4K
inline void CwxHttpCookie::setValue(char const* value)
{
    if (value)
    {
        m_strValue = value;
    }
    else
    {
        m_strValue.erase();
    }
}
///获取cookie的数值		
inline string const& CwxHttpCookie::getValue() const
{
    return m_strValue;
}
///设置cookie的comment,只有v1支持comment
inline void CwxHttpCookie::setComment(char const* comment)
{
    if (comment)
    {
        m_strComment = comment;
    }
    else
    {
        m_strComment.erase();
    }
}
///获取cookie的comment
inline string const& CwxHttpCookie::getComment() const
{
    return m_strComment;
}
///设置cookie的domain
inline void CwxHttpCookie::setDomain(char const* domain)
{
    if (domain)
    {
        m_strDomain = domain;
    }
    else
    {
        m_strDomain.erase();
    }
}
///获取cookie的domain		
inline string const& CwxHttpCookie::getDomain() const
{
    return m_strDomain;
}
///设置cookie的路径
inline void CwxHttpCookie::setPath(char const* path)
{
    if (path)
    {
        m_strPath = path;
    }
    else
    {
        m_strPath.erase();
    }
}
///获取cookie的路径		
inline string const& CwxHttpCookie::getPath() const
{
    return m_strPath;
}
///设置cookie的安全flag
inline void CwxHttpCookie::setSecure(bool secure)
{
    m_bSecure =secure;
}
///获取是否设置了安全flag
inline bool CwxHttpCookie::getSecure() const
{
    return m_bSecure;
}
///设置失效时间。ttTime为失效的时间点。
inline void CwxHttpCookie::setExpire(time_t ttTime)
{
    m_ttExpire = ttTime;
}
///获取多少秒后失效。
inline time_t CwxHttpCookie::getExpire() const
{
    return m_ttExpire;
}
///设置失效时间，为yyyy-mm-dd hh:mm:ss格式。
inline void CwxHttpCookie::setExpire(char const* daytime)
{
    if (daytime)
    {
        m_ttExpire = CwxDate::getDate(daytime);
    }
    else
    {
        m_ttExpire = -1;
    }
}
///获取失效的时间。返回的格式为yyyy-mm-dd hh:mm:ss格式。
inline string& CwxHttpCookie::getExpire(string& daytime) const
{
    CwxDate::getDate(m_ttExpire, daytime);
    return daytime;
}
///设置cookie的HttpOnly标记
inline void CwxHttpCookie::setHttpOnly(bool flag)
{
    m_bHttpOnly = flag;
}
///获取cookie的HttpOnly标记		
inline bool CwxHttpCookie::getHttpOnly() const
{
    return m_bHttpOnly;
}

CWINUX_END_NAMESPACE
