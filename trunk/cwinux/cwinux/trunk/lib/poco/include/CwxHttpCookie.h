#ifndef __CWX_HTTP_COOKIE_H__
#define __CWX_HTTP_COOKIE_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxHttpCookie.h
@brief 定义了HTTP的cookie对象，支持V0及V1，缺省为V0
@author cwinux@gmail.com
@version 0.1
@date 2009-10-16
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxCommon.h"
#include "CwxDate.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxHttpCookie
@brief HTTP协议的Cookie对象的定义，支持V0与V1。
*/
class CWX_API CwxHttpCookie
{
public:
    ///缺省构造函数
	CwxHttpCookie();
    ///通过指定的名字，创建一个cookie，有效期为永远。
	explicit CwxHttpCookie(string const& name);
    ///通过指定的名字及其值，创建一个cookie，有效期为永远。
	CwxHttpCookie(string const& name, string const& value);
    ///拷贝构造函数
    CwxHttpCookie(CwxHttpCookie const& cookie);
    ///析构函数
    ~CwxHttpCookie();
    ///赋值操作	
    CwxHttpCookie& operator = (const CwxHttpCookie& cookie);
public:
    ///设置cookie的版本号，缺省为0
	void setVersion(int version=0);
    ///获取cookie的版本号
	int getVersion() const;
    ///设置cookie的名字		
	void setName(char const* name);
    ///获取cookie的名字		
	string const& getName() const;
    ///设置cookie的数值，其长度不能超过4K
	void setValue(char const* value);
    ///获取cookie的数值		
	string const& getValue() const;
    ///设置cookie的comment,只有v1支持comment
	void setComment(char const* comment);
    ///获取cookie的comment
	string const& getComment() const;
    ///设置cookie的domain
	void setDomain(char const* domain);
    ///获取cookie的domain		
	string const& getDomain() const;
    ///设置cookie的路径
	void setPath(char const* path);
    ///获取cookie的路径		
	string const& getPath() const;
    ///设置cookie的安全flag
	void setSecure(bool secure);
    ///获取是否设置了安全flag
	bool getSecure() const;
    ///设置失效时间。ttTime为失效的时间点。
    void setExpire(time_t ttTime);
    ///获取多少秒后失效。
	time_t getExpire() const;
    ///设置失效时间，为yyyy-mm-dd hh:mm:ss格式。
    void setExpire(char const* daytime);
    ///获取失效的时间。返回的格式为yyyy-mm-dd hh:mm:ss格式。
    string& getExpire(string& daytime) const;
    ///设置cookie的HttpOnly标记
    void setHttpOnly(bool flag);
    ///获取cookie的HttpOnly标记		
	bool getHttpOnly() const;
    ///返回cookie的字符串		
	string& toCookie(string& cookie) const;
    ///在出错的时候，返回错误消息
    string const& getErrMsg() const
    {
        return m_strErr;
    }
private:
	int         m_iVersion; ///<version
	string      m_strName; ///<cookie's name
	string      m_strValue; ///<cookie's value
	string      m_strComment; ///<cookie's comment, only for v1
	string      m_strDomain; ///<cookie's domain
	string      m_strPath; ///<cookie's path
	bool        m_bSecure; ///<cookie's secure sign
    time_t      m_ttExpire; ///<cookie's expire time
	bool        m_bHttpOnly; ///<http-only sign
    string      m_strErr; ///<error msg for failure
};

CWINUX_END_NAMESPACE
#include "CwxHttpCookie.inl"
#include "CwxPost.h"

#endif
