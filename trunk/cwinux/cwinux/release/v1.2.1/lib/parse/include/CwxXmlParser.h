#ifndef __CWX_XML_PARSER_H__
#define __CWX_XML_PARSER_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxXmlParser.h
@brief 定义了CwxXmlParser类，此类基于expat实现XML的流解析。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxStl.h"
#include "CwxCommon.h"
#include "CwxGbkUnicodeMap.h"
#include "CwxParseMacro.h"
#include <expat.h>

CWINUX_BEGIN_NAMESPACE
/**
@class CwxXmlParser
@brief 基于expat实现的XML流解析对象。除了支持expat默认支持的字符集，还支持GBK与gb2312字符集
*/
class CWX_API CwxXmlParser
{
public:
    enum{
        DEF_TRUCK_BUF_SIZE = 16 * 1024, ///<缺省的数据块的大小
        PATH_BUF_ALIGN = 1024 ///数据块的边界对齐大小
    };
public:
    /**
    @brief 构造函数。
    @param [in] uiBufSize 数据块的大小
    */
    CwxXmlParser(CWX_UINT32 uiBufSize=DEF_TRUCK_BUF_SIZE);
    ///析构函数
    virtual ~CwxXmlParser(void);
public:
    ///解析XML实体，用户可以对此API进行重载。
    virtual bool parse();
    ///expat的XML解析引擎是否初始化
    bool isReady(void) const
    {
        return m_isReady;
    }
    ///获取XML的解析错误消息
    XML_Error getLastError(void) const
    {
        return m_lastError;
    }
    ///获取XML解析的状态代码
    XML_Status getStatus(void) const
    {
        return m_status; 
    }
    ///获取XML解析的数据块
    XML_Char *getBuf(void) const
    {
        return m_szXmlBuf; 
    }
    ///获取数据块的大小
    CWX_UINT32 getBufSize(void) const
    {
        return m_uiBufSize; 
    }
    ///获取当前解析到的XML路径
    XML_Char const* getXmlPath() const
    {
        return m_szXmlPath;
    }
    ///判断XML是否为GBK的编码
    bool isGbk() const
    {
        return m_bGbk;
    }
    /**
    @brief 若字符集为GBK或gb2312，则将expat的UTF-8输出变为GBK或gb2312的编码格式。
    @param [in] value expat输出的UTF-8的字符串
    @param [in] uiValueLen value的长度
    @return 返回GBK或gb2312编码的字符串
    */
    char const* charsetValue(XML_Char const* value, CWX_UINT32 uiValueLen);
protected:
    ///设置expat的引擎的ready状态
    void setReady(bool isReady)
    {
        m_isReady = isReady;
    }
    ///设置xml解析的状态码
    void setStatus(XML_Status status)
    {
        m_status = status; 
    }
    ///设置XML解析的错误信息
    void setLastError(XML_Error lastError)
    {
        m_lastError = lastError;
    }
    ///准备xml解析的环境，继承类可以重载此API
    virtual bool prepare();
    ///获取XML解析的下一个数据块，重载类需要重载此API，为expat引擎提供数据流
    virtual ssize_t readBlock(void);
    /**
    @brief 通知进入一个XML的数据节点。
    @param [in] name XML节点的名字
    @param [in] atts XML节点的属性，atts[2n]为属性的名字，atts[2n+1]为属性的值，若atts[2n]为NULL，表示属性结束
    @return void
    */
    virtual void startElement(const XML_Char *name, const XML_Char **atts);
    /**
    @brief 通知离开一个XML的数据节点。
    @param [in] name XML节点的名字
    @return void
    */
    virtual void endElement(const XML_Char *name);
    /**
    @brief 通知一个节点内的数据。
    @param [in] s 数据的内容，其编码为UTF8的编码
    @param [in] len 数据的内容的长度。
    @return void
    */
    virtual void characterData(const XML_Char *s, int len);
    /**
    @brief 通知XML的instructions.
    @param [in] target instruction的第一个word.
    @param [in] data 第一个word后，去掉所有空格的字符串。
    @return void
    */
    virtual void processingInstruction(const XML_Char *target, const XML_Char *data);
    ///xml中的注释
    virtual void commentData(const XML_Char *data);
    ///xml的缺省数据处理句柄
    virtual void defaultHandler(const XML_Char *s, int len);
    ///通知进入XML的CDATA语法
    virtual void startCData(void);
    ///通知离开XML的CDATA语法
    virtual void endCData(void);

private:
    ///注册所有的expat的事件处理函数
    void regDefHandlers();
    ///进入一个XML节点的事件处理函数
    static XMLCALL void elementStartHandler(void *userData,
        const XML_Char *name, const XML_Char **atts);
    ///离开一个XML节点的事件处理函数
    static XMLCALL void elementEndHandler(void *userData,
        const XML_Char *name);
    ///节点内部数据的接受函数
    static XMLCALL void characterDataHandler(void *userData,
        const XML_Char *s, int len);
    ///XML instruction的接受函数
    static XMLCALL void processingInstrHandler(void *userData,
        const XML_Char *target, const XML_Char *data);
    ///注释的接受函数
    static XMLCALL void commentHandler(void *userData,
        const XML_Char *data);
    ///缺省事件的处理函数
    static XMLCALL void defaultHandler(void *userData,
        const XML_Char *s, int len);
    ///进入CDATA的事件函数
    static XMLCALL void startCDatahandler(void *userData);
    ///离开CDATA的事件函数
    static XMLCALL void endCDatahandler(void *userData);
    ///GBK、gb2312的字符集转换API
    static XMLCALL int convert(void* data, char const* s);
    ///GBK、gb2312字符编码转换的事件函数
    static XMLCALL int encodingHandler(void* userData, XML_Char const* name, XML_Encoding* info);
private:
    XML_Parser  m_expatParser;///<expat的引擎
    XML_Char *  m_szXmlBuf; ///<内部临时BUF
    CWX_UINT32  m_uiBufSize;///<临时BUF的大小
    bool    m_isReady;///<引擎初始化状态标记
    XML_Status m_status;///<XML解析的状态码
    XML_Error m_lastError;///<XML解析的错误信息
    XML_Char*  m_szXmlPath;///<当前XML节点的全路径
    CWX_UINT32 m_uiPathBufLen;///<XML节点全路径的BUF的长度
    CWX_UINT32 m_uiPathLen;///<m_szXmlPath中的节点路径长度
    bool      m_bGbk;///<是否是中文编码
    XML_Char*  m_szGbkBuf;///<进行GBK编码转换的内存
    CWX_UINT32 m_uiGbkBufLen;///<m_szGbkBuf的内存长度
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif

