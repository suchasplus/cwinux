#ifndef __CWM_CENTER_SYSTEM_LOOP_H__
#define __CWM_CENTER_SYSTEM_LOOP_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmCenterSystemLoop.h
@brief 模板的loop对象，实现Center所管理的system的信息输出。
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
#include "CwmCenterTss.h"
#include "CwmCenterConfig.h"
class CwmCenterApp;
/**
@class CwmCenterSystemLoop
@brief 模板的loop对象，实现Center所管理的system的信息输出
*/
class CwmCenterSystemLoop : public  CwxHtmlTmplLoop
{
public:
    ///构造函数
    CwmCenterSystemLoop()
    {
        m_pSystems = NULL;
        m_pTss = NULL;
        m_bFirst = true;
    }
    ///析构函数
    ~CwmCenterSystemLoop()
    {

    }
public:
    ///初始化loop对象
    void init(CwmCenterTss* pTss, map<string/*system name*/, CwmCenterSystem*> const* pSystems);
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
public:
    ///获取system的属性值
    static char const* getSystemAttr(CwmCenterSystem const* pSystem, char const* szAttr, char* szBuf64k);
private:
    bool           m_bFirst; ///<是不是第一次执行loop
    map<string/*system name*/, CwmCenterSystem*> const* m_pSystems; ///<system的列表
    map<string/*system name*/, CwmCenterSystem*>::const_iterator m_system_iter; ///<当前loop的system
    CwmCenterTss*    m_pTss; ///<tss对象
};


#endif
