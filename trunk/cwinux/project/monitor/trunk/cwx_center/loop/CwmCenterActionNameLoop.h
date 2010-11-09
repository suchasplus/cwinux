#ifndef __CWM_CENTER_ACTION_NAME_LOOP_H__
#define __CWM_CENTER_ACTION_NAME_LOOP_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCenterActioNnameLoop.h
@brief 模板的loop对象，实现Center所管理的一个service的action名字的输出。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-06
@warning
@bug
*/

#include "CwxHtmlTmplLoop.h"
#include "CwxHtmlTmpl.h"
#include "CwmCenterTss.h"
#include "CwmCenterConfig.h"
class CwmCenterApp;
/**
@class CwmCenterActionNameLoop
@brief 查询、显示Center所管理的一个service的action名字的输出
*/
class CwmCenterActionNameLoop : public  CwxHtmlTmplLoop
{
public:
    ///构造函数
    CwmCenterActionNameLoop()
    {
        m_bFirst = false;
        m_pActions = NULL;
        m_pTss = NULL;
    }
    ///析构函数
    ~CwmCenterActionNameLoop()
    {
    }
public:
    ///初始化loop对象
    void init(CwmCenterTss* pTss, list<string> const* pActions);
public:
    /**
    @brief 此为global的loop对象，没有下一个
    @return 0：没有下一条数据记录；1：移到了下一条数据记录
    */
    virtual int next();
    /**
    @brief 获取global 对象的一个变量。
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
        bool bVal);
    /**
    @brief 返回system的loop对象。
    @param [in] szLoop loop对象的名字
    @param [in] attrs loop对象定义的时候设置的LOOP的属性对
    @return NULL：loop对象不存在；否则为名字所对应的LOOP对象
    */
    virtual CwxHtmlTmplLoop* loop(char const* szLoop,
        list<pair<char*, char*> >const& attrs);

private:
    bool           m_bFirst; ///<是否是第一次loop
    list<string> const* m_pActions; ///<action名字的列表
    list<string>::const_iterator m_action_iter; ///<当前action的名字
    CwmCenterTss*    m_pTss; ///<tss对象
};


#endif
