#ifndef __CWX_XML_CONFIG_PARSER_H__
#define __CWX_XML_CONFIG_PARSER_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxXmlConfigParser.h
@brief 定义了CwxXmlConfigParser类，将XML的BUF解析成CwxXmlTreeNode组织的节点树。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxStl.h"
#include "CwxGlobalMacro.h"
#include "CwxCommon.h"
#include "CwxGbkUnicodeMap.h"
#include "CwxCharPool.h"
#include "CwxXmlTreeNode.h"
#include "CwxParseMacro.h"
#include <expat.h>


CWINUX_BEGIN_NAMESPACE

/**
@class CwxXmlConfigParser
@brief 将XML的BUF解析成CwxXmlTreeNode组织的节点树。除了支持expat默认支持的字符集，还支持GBK与gb2312字符集
*/
class CWX_API CwxXmlConfigParser
{
public:
    /**
    @brief 构造函数。
    @param [in] uiAvgTokenLen XML中的数据节点的平均长度
    @param [in] uiAvgXmlSize 要解析的XML的平均大小
    */
    CwxXmlConfigParser(CWX_UINT32 uiAvgTokenLen=1024, CWX_UINT32 uiAvgXmlSize=4096);
    ///析构函数
    ~CwxXmlConfigParser();
public:
    /**
    @brief 将szXml定义的XML文本，解析成CwxXmlTreeNode的节点树。
    @param [in] szXml XML
    @return true：解析成功；false：解析失败
    */
    bool parse(char const* szXml);
    /**
    @brief 获取一个XML节点的属性。
    @param [in] szPath XML的节点，采用key:key1:key2的格式，各个节点以【:】分割
    @param [in] szAttr 节点的属性名
    @return NULL：不存在；否则为节点属性的数值
    */
    char const* getElementAttr(char const* szPath, char const* szAttr) const;
    /**
    @brief 获取一个XML节点的所有属性。
    @param [in] szPath XML的节点，采用key:key1:key2的格式，各个节点以【:】分割
    @param [in] attrs 节点的所有属性名，pair的first为属性名，second为属性的值
    @return false：节点不存在；否则返回节点的属性列表
    */
    bool getElementAttrs(char const* szPath, list<pair<char*, char*> >& attrs) const;
    /**
    @brief 获取[\<aa\>xxxx\</aa\>]的形式的节点的数据xxxx。
    @param [in] szPath XML的节点，采用key:key1:key2的格式，各个节点以【:】分割
    @param [in] strData 节点的数据
    @return false：节点不存在或不是\<aa\>xxxx\</aa\>的格式；节点存在而且为此格式
    */
    bool getElementData(char const* szPath, string& strData) const;
    /**
    @brief 获取节点的Tree Node。
    @param [in] szPath XML的节点，采用key:key1:key2的格式，各个节点以【:】分割
    @return NULL：节点不存在；路径的节点
    */
    CwxXmlTreeNode const* getElementNode(char const* szPath) const;
    ///获取节点的根
    CwxXmlTreeNode const * getRoot() const
    {
        return m_root; 
    }
    ///判断xml的编码是否为GBK
    bool isGbk() const 
    {
        return m_bGbk;
    }
private:
    /**
    @brief 通知进入一个XML的数据节点。
    @param [in] name XML节点的名字
    @param [in] atts XML节点的属性，atts[2n]为属性的名字，atts[2n+1]为属性的值，若atts[2n]为NULL，表示属性结束
    @return void
    */
    void startElement(const XML_Char *name, const XML_Char **atts);
    /**
    @brief 通知离开一个XML的数据节点。
    @param [in] name XML节点的名字
    @return void
    */
    void endElement(const XML_Char *name);
    /**
    @brief 通知一个节点内的数据。
    @param [in] s 数据的内容，其编码为UTF8的编码
    @param [in] len 数据的内容的长度。
    @return void
    */
    void characterData(const XML_Char *s, int len);
    /**
    @brief 若字符集为GBK或gb2312，则将expat的UTF-8输出变为GBK或gb2312的编码格式。
    @param [in] value expat输出的UTF-8的字符串
    @param [in] uiValueLen value的长度
    @return 返回GBK或gb2312编码的字符串
    */
    char const* charsetValue(XML_Char const* value, CWX_UINT32 uiValueLen);
private:
    ///注册所有的expat的事件处理函数
    void regDefHandlers(void);
    ///进入一个XML节点的事件处理函数
    static XMLCALL void elementStartHandler(void *userData,
        const XML_Char *name, const XML_Char **atts);
    ///离开一个XML节点的事件处理函数
    static XMLCALL void elementEndHandler(void *userData,
        const XML_Char *name);
    ///节点内部数据的接受函数
    static XMLCALL void characterDataHandler(void *userData,
        const XML_Char *s, int len);
    ///GBK、gb2312的字符集转换API
    static XMLCALL int convert(void* data, char const* s);
    ///GBK、gb2312字符编码转换的事件函数
    static XMLCALL int encodingHandler(void* userData, XML_Char const* name, XML_Encoding* info);
private:
    XML_Parser  m_expatParser;///<expat的引擎
    CwxCharPool m_memPool;///<字符内存池
    CwxXmlTreeNode* m_root;///<根节点
    CwxXmlTreeNode* m_pCur;///<解析过程中的当前节点
    bool      m_bGbk;///<是否为GBK编码
    XML_Char*  m_szGbkBuf;///<GBK编码转换的临时BUF
    CWX_UINT32 m_uiGbkBufLen;///<m_szGbkBuf的空间大小
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif

