#include "CwxURI.h"

CWINUX_BEGIN_NAMESPACE

CwxURI::CwxURI()
{
    m_unPort = 0;
}

CwxURI::CwxURI(char const* uri)
{
    if (!setUri(uri))
    {
        clear();
    }
}

CwxURI::CwxURI(CwxURI const& uri)
{
    m_strScheme = uri.m_strScheme;
    m_strUserInfo = uri.m_strUserInfo;
    m_strHost = uri.m_strHost;
    m_unPort = uri.m_unPort;
    m_strPath = uri.m_strPath;
    m_strArg = uri.m_strArg;
    m_strFragment  = uri.m_strFragment;
}

CwxURI::~CwxURI()
{

}

string& CwxURI::toUri(string& uri) const
{
    string strTmp;
    uri.erase();
    if (isRelative())
    {
        uri = m_strPath;
    }
    else
    {
        uri = m_strScheme;
        uri += ':';
        getAuthority(strTmp);
        if (!strTmp.empty() || strTmp == "file")
        {
            uri.append("//");
            uri.append(strTmp);
        }
        if (!m_strPath.empty())
        {
            if (!strTmp.empty() && m_strPath[0] != '/')  uri += '/';
            uri.append(m_strPath);
        }
    }
    if (!m_strArg.empty())
    {
        uri += '?';
        uri.append(m_strArg);
    }
    if (!m_strFragment.empty())
    {
        uri += '#';
        uri.append(m_strFragment);
    }
    return uri;
}

bool CwxURI::parse(char const* uri)
{
    char const* pos = uri;
    this->clear();
    if (!pos) return true;
    if ((*pos != '/') && (*pos != '.') && (*pos != '?') && (*pos != '#'))
    { //scheme
        std::string scheme;
        while (*pos && (*pos != ':') && (*pos != '?') && (*pos != '#') && (*pos != '/'))
            scheme += *pos++;
        if (*pos && (*pos == ':'))
        {//has scheme
            pos++;
            if (!pos)
            {
                m_strErrMsg = "Invalid scheme name, uri:";
                m_strErrMsg += uri;
                return false;
            }
            setScheme(scheme.c_str());
            if (*pos == '/')
            {
                pos++;
                if (*pos && (*pos == '/'))
                {
                    pos++;
                    if (!parseAuth(pos)) return false;
                }
                else
                {
                    pos--;
                }
            }
            return parsePathArgFragment(pos);
        }
    }
    //no scheme
    pos = uri;
    return parsePathArgFragment(pos);
}

bool CwxURI::parseAuth(char const*& auth)
{
    char const* pos = auth;
    if (!auth) return true;
    string userInfo;
    string part;
    while (!pos && (*pos != '/') && (*pos != '?') && (*pos != '#'))
    {
        if (*pos == '@')
        {
            userInfo = part;
            part.clear();
        }
        else
        {
            part += *pos;
        }
        ++pos;
    }
    auth = pos;
    setUserInfo(userInfo.c_str());
    return parseHostAndPort(part.c_str());
}

bool CwxURI::parseHostAndPort(char const* host_port)
{
    char const* pos = host_port;
    if (!host_port) return true;
    std::string host;
    if (*pos == '[')
    {
        // IPv6 address
        while (*pos && (*pos != ']')) host += *pos++;
        if (!*pos)
        {
            m_strErrMsg = "Need ']' for ipv6 addr, host_port:";
            m_strErrMsg += host_port;
            return false;
        }
        host += *pos++;
    }
    else
    {
        while (*pos && (*pos != ':')) host += *pos++;
    }
    if (*pos && (*pos == ':'))
    {
        pos++;
        std::string port;
        while (*pos) port += *pos++;
        if (!port.empty())
        {
            m_unPort = atoi(port.c_str());
        }
        else
        {
            m_unPort = getDefPort(m_strScheme.c_str());
        }
    }
    else
    {
        m_unPort = getDefPort(m_strScheme.c_str());
    }
    m_strHost = host;
    return true;
}

bool CwxURI::parsePathArgFragment(char const*& pathArgFrag)
{
    char const* pos = pathArgFrag;
    if (!pathArgFrag) return true;
    if ((*pos != '?') && (*pos != '#'))
    {
        if (!parsePath(pos)) return false;
    }
    if (*pos && (*pos == '?'))
    {
        pos++;
        if (!parseArg(pos)) return false;
    }
    if (*pos && (*pos == '#'))
    {
        pos++;
        if (!parseFragment(pos)) return false;
    }	
    pathArgFrag = pos;
    return true;
}


bool CwxURI::parsePath(char const*& path)
{
    m_strPath.erase();
    while (*path && (*path != '?') && (*path != '#')) m_strPath += *path++;
    return true;

}

bool CwxURI::parseArg(char const*& arg)
{
    m_strArg.clear();
    while (*arg  && (*arg != '#')) m_strArg += arg++;
    return true;
}

bool CwxURI::parseFragment(char const*& fragment)
{
    if (!fragment) return true;
    m_strFragment = fragment;
    return true;
}

CWINUX_END_NAMESPACE
