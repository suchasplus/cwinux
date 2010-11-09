#ifndef __CWX_HTML_TMPL_LOOP_H__
#define __CWX_HTML_TMPL_LOOP_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxHtmlTmplLoop.h
@brief 定义文本替换模板的loop对象接口，模板有一个默认的根LOOP对象。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxReplMachine.h"
#include "CwxHtmlTmplEscape.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxHtmlTmplLoop
@brief 文本替换模板的LOOP对象的接口类，为所有LOOP对象的基类。
*/
class CWX_API CwxHtmlTmplLoop
{
public:
    ///构造函数
    CwxHtmlTmplLoop()
    {

    }
    ///析构函数
    virtual ~CwxHtmlTmplLoop()
    {

    }
public:
    /**
    @brief 获取LOOP对象的下一条数据记录。成的环境对象
    @return 0：没有下一条数据记录；1：移到了下一条数据记录
    */
    virtual int next() =0;
    /**
    @brief 从LOOP对象中，获取一个变量的数值。
    @param [in] szName 变量的名字
    @param [in] bAttrName 变量名是否是当前对象的属性名
    @param [in] ucSysAttr 变量的系统属性，在CwxHtmlTmplValSysAttr中做了定义
    @param [in] szUserAttr  用户自己的变量属性
    @param [in] iLengthLimit 变量内容输出的长度限制，<0：输出的字符个数不能超过此绝对值
                             0：没有限制；>0：输出的字节数不能超过此数值
    @param [in] pEscape 变量的escape变换对象
    @param [in] szArgs 变量定义时设置的参数
    @param [in] pRepl 模板的字符替换对象
    @param [in] bVal true：此为变量取值；false：此为比较取值
    @return NULL：变量不存在；否则，为变量的数值
    */
    virtual char const* val(char const* szName,
        bool bAttrName,
        CWX_UINT8 ucSysAttr,
        char const* szUserAttr,
        int iLengthLimit,
        CwxHtmlTmplEscape* pEscape,
        char const* szArgs,
        CwxReplBase* pRepl,
        bool bVal)=0;
    /**
    @brief 根据LOOP的名字，获取LOOP对象实例。
    @param [in] szLoop loop对象的名字
    @param [in] attrs loop对象定义的时候设置的LOOP的属性对
    @return NULL：loop对象不存在；否则为名字所对应的LOOP对象
    */
    virtual CwxHtmlTmplLoop* loop(char const* szLoop,
        list<pair<char*, char*> >const& attrs) = 0;
};



CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif
