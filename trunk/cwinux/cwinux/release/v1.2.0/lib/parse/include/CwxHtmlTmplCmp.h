#ifndef __CWX_HTML_TMPL_CMP_H__
#define __CWX_HTML_TMPL_CMP_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxHtmlTmplCmp.h
@brief 定义文本替换模板的比较函数对象的接口，并实现一些常用的比较函数对象。
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

CWINUX_BEGIN_NAMESPACE

class CwxHtmlTmpl;
class CwxHtmlTmplEnv;

/**
@class CwxHtmlTmplCmp
@brief 文本替换模板的比较函数对象的接口类，为所有比较函数的基类。<br>
       此对象必须保证多线程安全
*/
class CWX_API CwxHtmlTmplCmp
{
public:
    ///构造函数
    CwxHtmlTmplCmp(string const& strName):m_strName(strName)
    {

    }
    ///析构函数
    virtual ~CwxHtmlTmplCmp()
    {

    }
public:
    /**
    @brief 比较函数。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量，NULL表示不存在
    @param [in] args  模板语法中设置的args，NULL表示没有设置
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szSrc小于szDest；false：szSrc不小于szDest
    */
    virtual bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv)=0;
public:
    ///获取比较函数的对象名
    string const& getName() const
    {
        return m_strName;
    }
private:
    string     m_strName;
};

///定义模板比较函数对象的模板
typedef map<string const, CwxHtmlTmplCmp*, CwxCaseStringLess> CwxHtmlTmplCmpMap;

/**
@class CwxHtmlTmplCmpEqual
@brief 相等比较，可以是char、int、length类型。
*/
class CWX_API CwxHtmlTmplCmpEqual:public CwxHtmlTmplCmp
{
public:
    CwxHtmlTmplCmpEqual():CwxHtmlTmplCmp("=")
    {

    }
public:
    /**
    @brief 相等函数。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量, NULL表示不存在
    @param [in] args  比较的类型，分别为char,casechar, int, uint, length,缺省为char
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szSrc小于szDest；false：szSrc不小于szDest
    */
    bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv);
};


/**
@class CwxHtmlTmplCmpLess
@brief 小于比较，可以是char、int、length类型。
*/
class CWX_API CwxHtmlTmplCmpLess:public CwxHtmlTmplCmp
{
public:
    CwxHtmlTmplCmpLess():CwxHtmlTmplCmp("<")
    {

    }
public:
    /**
    @brief 小于函数。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量, NULL表示不存在
    @param [in] args  比较的类型，分别为char,casechar, int, uint, length,缺省为char
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szSrc小于szDest；false：szSrc不小于szDest
    */
    bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv);
};


/**
@class CwxHtmlTmplCmpMore
@brief 大于比较，可以是char、int、length类型。
*/
class CWX_API CwxHtmlTmplCmpMore:public CwxHtmlTmplCmp
{
public:
    CwxHtmlTmplCmpMore():CwxHtmlTmplCmp(">")
    {

    }
public:
    /**
    @brief 小于函数。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量, NULL表示不存在
    @param [in] args  比较的类型，分别为char,casechar, int, uint, length,缺省为char
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szSrc小于szDest；false：szSrc不小于szDest
    */
    bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv);
};

/**
@class CwxHtmlTmplCmpPrefix
@brief 常量是变量前缀的校验。
*/
class CWX_API CwxHtmlTmplCmpPrefix:public CwxHtmlTmplCmp
{
public:
    CwxHtmlTmplCmpPrefix():CwxHtmlTmplCmp("prefix")
    {

    }
public:
    /**
    @brief 常量是变量前缀的校验。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量, NULL表示不存在
    @param [in] args  case：大小写不敏感，否则为大小写敏感
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szDest是szSrc的前缀；false：szDest不是szSrc的前缀
    */
    bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv);
};


/**
@class CwxHtmlTmplCmpRPrefix
@brief 变量是常量前缀的校验。
*/
class CWX_API CwxHtmlTmplCmpRPrefix:public CwxHtmlTmplCmp
{
public:
    CwxHtmlTmplCmpRPrefix():CwxHtmlTmplCmp("r-prefix")
    {

    }
public:
    /**
    @brief 变量是常量前缀的校验。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量, NULL表示不存在
    @param [in] args  case：大小写不敏感，否则为大小写敏感
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szSrc是szDest的前缀；false：szSrc不是szDest的前缀
    */
    bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv);
};

/**
@class CwxHtmlTmplCmpSuffix
@brief 常量是变量后缀的校验。
*/
class CWX_API CwxHtmlTmplCmpSuffix:public CwxHtmlTmplCmp
{
public:
    CwxHtmlTmplCmpSuffix():CwxHtmlTmplCmp("suffix")
    {

    }
public:
    /**
    @brief 常量是变量后缀的校验。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量, NULL表示不存在
    @param [in] args  case：大小写不敏感，否则为大小写敏感
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szDest是szSrc的后缀；false：szDest不是szSrc的后缀
    */
    bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv);
};

/**
@class CwxHtmlTmplCmpRSuffix
@brief 变量是常量的后缀的校验。
*/
class CWX_API CwxHtmlTmplCmpRSuffix:public CwxHtmlTmplCmp
{
public:
    CwxHtmlTmplCmpRSuffix():CwxHtmlTmplCmp("suffix")
    {

    }
public:
    /**
    @brief 变量是常量的后缀的校验。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量, NULL表示不存在
    @param [in] args  case：大小写不敏感，否则为大小写敏感
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szSrc是szDest的后缀；false：szSrc不是szDest的后缀
    */
    bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv);
};

/**
@class CwxHtmlTmplCmpIn
@brief 变量包含常量的校验。
*/
class CWX_API CwxHtmlTmplCmpIn:public CwxHtmlTmplCmp
{
public:
    CwxHtmlTmplCmpIn():CwxHtmlTmplCmp("suffix")
    {

    }
public:
    /**
    @brief 变量包含常量的校验。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量, NULL表示不存在
    @param [in] args  case：大小写不敏感，否则为大小写敏感
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szSrc中包含szDest；false：szSrc中不包含szDest
    */
    bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv);
};

/**
@class CwxHtmlTmplCmpRIn
@brief 常量包含变量的校验。
*/
class CWX_API CwxHtmlTmplCmpRIn:public CwxHtmlTmplCmp
{
public:
    CwxHtmlTmplCmpRIn():CwxHtmlTmplCmp("suffix")
    {

    }
public:
    /**
    @brief 常量包含变量的校验。
    @param [in] szSrc 参与比较的第一个参数，也就是模板语法的变量的值, NULL表示不存在
    @param [in] szDest 参与比较的第二个参数，也就是模板语法的比较常量, NULL表示不存在
    @param [in] args  case：大小写不敏感，否则为大小写敏感
    @param [in] pHtmlTmpl 函数对象所属的模板对象
    @param [in] pHtmlTmplEnv 模板形成的环境对象
    @return true：szDest中包含szSrc；false：szDest中不包含szSrc
    */
    bool compare(char const* szSrc,
        char const* szDest,
        char const* args,
        CwxHtmlTmpl const* pHtmlTmpl,
        CwxHtmlTmplEnv* pHtmlTmplEnv);
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"
#endif
