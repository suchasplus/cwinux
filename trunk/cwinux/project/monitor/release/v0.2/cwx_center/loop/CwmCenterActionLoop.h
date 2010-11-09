#ifndef __CWM_CENTER_ACTION_LOOP_H__
#define __CWM_CENTER_ACTION_LOOP_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCenterActionLoop.h
@brief 模板的loop对象，实现Center所管理的一个service下的action实时信息的输出。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-06
@warning
@bug
*/

#include "CwxHtmlTmplLoop.h"
#include "CwxHtmlTmpl.h"
#include "CwxPackageReader.h"
#include "CwxMsgBlock.h"
#include "CwmCenterTss.h"
#include "CwmCenterConfig.h"
#include "CwmCenterNerveStatus.h"

class CwmCenterApp;
/**
@class CwmCenterActionLoopData
@brief Never的查询结果对象
*/
class CwmCenterActionLoopData
{
public:
    ///构造函数
    CwmCenterActionLoopData()
    {
        m_pRecvMsg = NULL;
        m_pNerve = NULL;
    }
    ///析构函数
    ~CwmCenterActionLoopData()
    {
        if (m_pRecvMsg) CwxMsgBlockAlloc::free(m_pRecvMsg);
        m_pRecvMsg = NULL;
    }
public:
    CwxMsgBlock*   m_pRecvMsg;///<从nerve收到的查询消息，NULL表示查询失败
    string           m_strErrMsg; ///<查询失败时的失败描述
    CwmCenterNerveStatus*    m_pNerve; ///<查询的nerve对象
};
/**
@class CwmCenterActionLoop
@brief 模板的loop对象，实现Center所管理的一个service下的action实时信息的输出。
*/
class CwmCenterActionLoop : public  CwxHtmlTmplLoop
{
public:
    CwmCenterActionLoop()
    {
        m_pActions = NULL;
        m_uiActioNum = 0;
        m_uiIndex = 0;
        m_pTss = NULL;
        m_bValid = false;
        m_pNerve = NULL;
    }
    ~CwmCenterActionLoop()
    {
    }
public:
    ///初始化loop对象
    void init(CwmCenterTss* pTss,
        string const& strName,
        CwmCenterActionLoopData* pActions,
        CWX_UINT32 uiActioNum);
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
    ///获取host对象的属性的值
    static char const* getNerveAttr(CwmCenterNerveStatus const* pNever,
        char const* szAttr,
        char* szBuf64k);
    ///获取action对象的属性值
    static char const* getActionAttr(string const& strName,
        CwxPackageReader const& actionReader,
        char const* szAttr,
        char* szBuf64k);
private:
    string      m_strName; ///<action的名字
    CwmCenterActionLoopData* m_pActions; ///<nerve返回的数组
    CwxPackageReader        m_packReader; ///<解析package的reader
    bool                   m_bValid; ///<当前的action的返回值是否有效
    string                 m_strErrMsg;///<当前action无效的原因
    CwmCenterNerveStatus*   m_pNerve; ///<当前记录的nerve对象
    CWX_UINT32  m_uiActioNum; ///<loop的nerve数量
    CWX_UINT32   m_uiIndex; ///<loop的当前index
    CwmCenterTss*    m_pTss;///<tss对象
};


#endif
