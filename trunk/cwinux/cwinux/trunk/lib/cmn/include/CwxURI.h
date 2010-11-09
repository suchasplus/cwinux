#ifndef __CWX_URI_H__
#define __CWX_URI_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxURI.h
@brief 定义了URI类，实现URI资源的解析与处理
@author cwinux@gmail.com
@version 0.1
@date 2009-10-15
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxURI
@brief Uniform Resource Identifier, 实现RFC 3986的specification。
       各种参数都是大小写敏感的。无论是scheme、host、path、arg、还是fragment，
       设置或获取都应该是经过编码的。URI对象不负责编码的处理。
*/

class CWX_API CwxURI
{
public:
    ///默认构造函数
	CwxURI();
    ///指定URI的构造函数
	explicit CwxURI(char const* uri);
    ///拷贝构造
	CwxURI(CwxURI const& uri);
    ///析构函数
	~CwxURI();
public:	
    ///赋值操作
	CwxURI& operator = (CwxURI const& uri);
    ///通过字符串的赋值操作
	CwxURI& operator = (char const* uri);
    ///相同比较操作
    bool operator == (CwxURI const& uri) const;
    ///相同比较操作
    bool operator == (char const* uri) const;
    ///不相同比较操作
    bool operator != (CwxURI const& uri) const;
    ///不相同比较操作
    bool operator != (char const* uri) const;
public:
    ///设置uri。返回值：true，成功；false：无效
    bool setUri(char const* uri);
    ///获取URI的scheme，如http，ftp等
    string const& getScheme() const;
    ///设置URI的scheme
    void setScheme(char const* scheme);
    ///获取URI中的用户info
    string const& getUserInfo() const;
    ///设置URI中的用户info
    void setUserInfo(char const* userInfo);
    ///获取URI的主机
    string const& getHost() const;
    ///设置URI的主机
    void setHost(char const* host);
    ///获取端口号
    CWX_UINT16 getPort() const;
    ///设置端口号
    void setPort(CWX_UINT16 port);
    ///返回uri中的权限部分，为userinfo\@host或当端口不为标准端口的时候，返回userinfo\@host:port
    string const& getAuthority(string& strAuth) const;
    ///设置URI的权限部分，格式为userinfo\@host或当端口不为标准端口的时候，userinfo\@host:port
    bool setAuthority(char const* authority);
    ///获取URI的路径
    string const& getPath() const;
    ///设置URI的路径
    void setPath(char const* path);
    ///获取URI的参数部分
    string const& getArg() const;
    ///设置url的参数部分
    void setArg(char const* arg);	
    ///获取URI的fragment部分
    string const& getFragment() const;
    ///设置URI的fragment部分
    void setFragment(char const* fragment);
    ///获取path和arg部分
    string & getPathArg(string & pathArg) const;
    ///判断对象是否为空
    bool empty() const;
    ///判断uri是否为相对目录，即scheme为空
    bool isRelative() const;
    ///返回出错时的错误消息
    string const& getErrMsg() const
    {
        return m_strErrMsg;
    }
public:
    ///输出URI
    string& toUri(string& uri) const;
    ///清空对象
	void clear();
		
protected:
    ///判断uri与本对象是否相等
	bool equals(CwxURI const& uri) const;
    ///判断设置的端口号是否是scheme的标准端口号
	bool isDefPort() const;
    ///获取scheme的标准端口号
	CWX_UINT16 getDefPort(char const* szScheme) const;
    ///解析URI，并用URI初始化对象，返回值：false，无效的URI；true：成功。
	bool parse(char const* uri);
    ///解析URI的userinfo\@host:port部分，返回值：false，无效的URI；true：成功。
    bool parseAuth(char const*& auth);
    ///解析URI的Path arg及fragment部分，返回值：false，无效的URI；true：成功。
    bool parsePathArgFragment(char const*& pathArgFrag);
    ///解析URI的host:port部分
    bool parseHostAndPort(char const* host_port);
    ///解析path
    bool parsePath(char const*& path);
    ///解析path
    bool parseArg(char const*& arg);
    ///解析fragment
    bool parseFragment(char const*& fragment);

private:
    string         m_strScheme; ///<uri's scheme
    string         m_strUserInfo;///<uri's user 
    string         m_strHost; ///<uri's host
    CWX_UINT16     m_unPort; ///<uri's host port
    string         m_strPath;///<uri's path
    string         m_strArg; ///<uri's argument
    string         m_strFragment; ///<uri's fragment
    string         m_strErrMsg; ///<failure message
};

CWINUX_END_NAMESPACE
#include "CwxURI.inl"
#include "CwxPost.h"

#endif
