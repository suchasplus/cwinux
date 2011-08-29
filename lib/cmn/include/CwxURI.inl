CWINUX_BEGIN_NAMESPACE

inline CwxURI& CwxURI::operator = (CwxURI const& uri)
{
    if (&uri != this)
    {
        m_strScheme = uri.m_strScheme;
        m_strUserInfo = uri.m_strUserInfo;
        m_strHost = uri.m_strHost;
        m_unPort = uri.m_unPort;
        m_strPath = uri.m_strPath;
        m_strArg = uri.m_strArg;
        m_strFragment  = uri.m_strFragment;
    }
    return *this;
}

inline CwxURI& CwxURI::operator = (char const* uri)
{
    if (!setUri(uri))
    {
        clear();
    }
    return *this;
}

inline bool CwxURI::operator == (CwxURI const& uri) const
{
    return equals(uri);
}

inline bool CwxURI::operator == (char const* uri) const
{
    CwxURI obj(uri);
    return equals(obj);
}

inline bool CwxURI::operator != (CwxURI const& uri) const
{
    return !equals(uri);
}

inline bool CwxURI::operator != (char const* uri) const
{
    CwxURI obj(uri);
    return !equals(obj);
}

inline bool CwxURI::setUri(char const* uri)
{
    return parse(uri);
}

inline string const& CwxURI::getScheme() const
{
    return m_strScheme;
}

inline void CwxURI::setScheme(char const* scheme)
{
    if (scheme)
        m_strScheme = scheme;
    else
        m_strScheme.erase();
    if (!m_unPort) m_unPort = getDefPort(m_strScheme.c_str());
}

inline string const& CwxURI::getUserInfo() const
{
    return m_strUserInfo;
}

inline void CwxURI::setUserInfo(char const* userInfo)
{
    if (userInfo)
        m_strUserInfo = userInfo;
    else
        m_strUserInfo.erase();
}

inline string const& CwxURI::getHost() const
{
    return m_strHost;
}

inline void CwxURI::setHost(char const* host)
{
    if (host)
        m_strHost = host;
    else
        m_strHost.erase();
}

inline CWX_UINT16 CwxURI::getPort() const
{
    return m_unPort?m_unPort:getDefPort(m_strScheme.c_str());
}

inline void CwxURI::setPort(CWX_UINT16 port)
{
    m_unPort = port;
}

inline string const& CwxURI::getAuthority(string& strAuth) const
{
    strAuth.erase();
    if (m_strUserInfo.length())
    {
        strAuth.append(m_strUserInfo);
        strAuth += '@';
    }
    strAuth.append(m_strHost);
    if (m_unPort && !isDefPort())
    {
        strAuth += ':';
        char szBuf[16];
        sprintf(szBuf, "%u", m_unPort);
        strAuth += szBuf;
    }
    return strAuth;
}

inline bool CwxURI::setAuthority(char const* authority)
{
    return parseAuth(authority);
}

inline string const& CwxURI::getPath() const
{
    return m_strPath;
}

inline void CwxURI::setPath(char const* path)
{
    if (path)
        m_strPath = path;
    else
        m_strPath.erase();
}

inline string const& CwxURI::getArg() const
{
    return m_strArg;
}

inline void CwxURI::setArg(char const* arg)
{
    if (arg)
        m_strArg = arg;
    else
        m_strArg.erase();
}

inline string const& CwxURI::getFragment() const
{
    return m_strFragment;
}

inline void CwxURI::setFragment(char const* fragment)
{
    if (fragment)
        m_strFragment = fragment;
    else
        m_strFragment.erase();
}

inline string& CwxURI::getPathArg(string & pathArg) const
{
    pathArg = m_strPath;
    if (!m_strArg.empty())
    {
        pathArg += '?';
        pathArg += m_strArg;
    }
    return pathArg;
}


inline bool CwxURI::empty() const
{
    return m_strHost.empty() && m_strPath.empty();
}

inline bool CwxURI::isRelative() const
{
    return m_strHost.empty();
}

inline void CwxURI::clear()
{
    m_strScheme.erase();
    m_strUserInfo.erase();
    m_strHost.erase();
    m_unPort = 0;
    m_strPath.erase();
    m_strArg.erase();
    m_strFragment.erase();
}

inline bool CwxURI::equals(CwxURI const& uri) const
{
    return 	m_strScheme == uri.m_strScheme
        && m_strUserInfo == uri.m_strUserInfo
        && m_strHost     == uri.m_strHost
        && getPort() == uri.getPort()
        && m_strPath     == uri.m_strPath
        && m_strArg    == uri.m_strArg
        && m_strFragment == uri.m_strFragment;
}

inline bool CwxURI::isDefPort() const
{
    return getDefPort(m_strScheme.c_str()) == m_unPort;
}

inline CWX_UINT16 CwxURI::getDefPort(char const* szSchema) const
{
    if (szSchema)
    {
        if (strcasecmp(szSchema, "ftp") == 0)
        {
            return 21;
        }
        else if (strcasecmp(szSchema, "ssh") == 0)
        {
            return 22;
        }
        else if (strcasecmp(szSchema,"telnet") == 0)
        {
            return 23;
        }
        else if (strcasecmp(szSchema, "http") == 0)
        {
            return 80;
        }
        else if (strcasecmp(szSchema, "nntp") == 0)
        {
            return 119;
        }
        else if (strcasecmp(szSchema, "ldap") == 0)
        {
            return 389;
        }
        else if (strcasecmp(szSchema, "https") == 0)
        {
            return 443;
        }
    }
    return 0;
}

CWINUX_END_NAMESPACE
