#ifndef __CWX_HTML_TMPL_ESCAPE_H__
#define __CWX_HTML_TMPL_ESCAPE_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxHtmlTmplEscape.h
@brief 定义文本替换模板的Escape函数对象的接口，并实现一些常用的Escape函数对象。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxChrType.h"

CWINUX_BEGIN_NAMESPACE

class CwxHtmlTmpl;
class CwxHtmlTmplEnv;
/**
@class CwxHtmlTmplEscape
@brief 文本替换模板的Escape函数对象的接口类，为所有CwxHtmlTmplEscape函数对象的基类。<br>
此对象必须保证多线程安全
*/
class CWX_API CwxHtmlTmplEscape
{
public:
    ///构造函数
    CwxHtmlTmplEscape(string const& strName):m_strName(strName)
    {

    }
    ///析构函数
    virtual ~CwxHtmlTmplEscape()
    {

    }
public:
    /**
    @brief ESCACPE函数。
    @param [in] szSrc 变量的值
    @param [in] szOutBuf 变量escape的输出buffer
    @param [in,out] uiBufLen  传入szOutBuf的大小，输出形成的escape字符串的长度
    @param [in] len 变量定义时设置的、对输出内容的长度限制
    @param [in] szArgs 模板语法中设置的args，NULL表示没有设置
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：encode成功；false：szOutBuf太小
    */
    virtual bool escape(char const* szSrc,
        char* szOutBuf,
        CWX_UINT32& uiBufLen,
        int len,
        char const* szArgs,
        CwxHtmlTmpl* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv)=0;
public:
    ///获取escape的名字
    string const& getName() const
    {
        return m_strName;
    }
private:
    string     m_strName;
};
///定义escape对象的名字map
typedef map<string const, CwxHtmlTmplEscape*> CwxHtmlTmplEscapeMap;


CWINUX_END_NAMESPACE

#include "CwxPost.h"
#endif
