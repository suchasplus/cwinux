#ifndef __CWX_XML_FILE_CONFIG_PARSER_H__
#define __CWX_XML_FILE_CONFIG_PARSER_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxXmlFileConfigParser.h
@brief 定义了CwxXmlFileConfigParser类，将XML的文件解析成CwxXmlTreeNode组织的节点树。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxStl.h"
#include "CwxCommon.h"
#include "CwxParseMacro.h"

#include "CwxXmlConfigParser.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxXmlFileConfigParser
@brief 将XML的文件解析成CwxXmlTreeNode组织的节点树。除了支持expat默认支持的字符集，还支持GBK与gb2312字符集
*/
class  CWX_API CwxXmlFileConfigParser
{
public:
    /**
    @brief 构造函数。
    @param [in] uiAvgTokenLen XML中的数据节点的平均长度
    @param [in] uiAvgXmlSize 要解析的XML的平均大小
    */
    CwxXmlFileConfigParser(CWX_UINT32 uiAvgTokenLen=1024, CWX_UINT32 uiAvgXmlSize=4096);
    ///析构函数
    virtual ~CwxXmlFileConfigParser(void);
public:
    /**
    @brief 将szXml定义的XML文本，解析成CwxXmlTreeNode的节点树。
    @param [in] strFileName XML文件名
    @return true：解析成功；false：解析失败
    */
    bool parse(string const& strFileName);
    /**
    @brief 获取一个XML节点的属性。
    @param [in] szPath XML的节点，采用key:key1:key2的格式，各个节点以【:】分割
    @param [in] szAttr 节点的属性名
    @return NULL：不存在；否则为节点属性的数值
    */
    char const* getElementAttr(char const* szPath, char const* szAttr) const 
    {
        return m_parser.getElementAttr(szPath, szAttr);
    }
    /**
    @brief 获取一个XML节点的所有属性。
    @param [in] szPath XML的节点，采用key:key1:key2的格式，各个节点以【:】分割
    @param [in] attrs 节点的所有属性名，pair的first为属性名，second为属性的值
    @return false：节点不存在；否则返回节点的属性列表
    */
    bool getElementAttrs(char const* szPath, list<pair<char*, char*> >& attrs) const
    {
        return m_parser.getElementAttrs(szPath, attrs);
    }
    /**
    @brief 获取[\<aa\>xxxx\</aa\>]的形式的节点的数据xxxx。
    @param [in] szPath XML的节点，采用key:key1:key2的格式，各个节点以【:】分割
    @param [in] strData 节点的数据
    @return false：节点不存在或不是\<aa\>xxxx\</aa\>的格式；节点存在而且为此格式
    */
    bool getElementData(char const* szPath, string& strData) const 
    {
        return m_parser.getElementData(szPath, strData);
    }
    /**
    @brief 获取节点的Tree Node。
    @param [in] szPath XML的节点，采用key:key1:key2的格式，各个节点以【:】分割
    @return NULL：节点不存在；路径的节点
    */
    CwxXmlTreeNode const* getElementNode(char const* szPath) const 
    {
        return m_parser.getElementNode(szPath);
    }
    ///获取节点的根
    CwxXmlTreeNode const * getRoot() const
    { 
        return m_parser.getRoot(); 
    }
    ///判断xml的编码是否为GBK
    bool isGbk() const
    { 
        return m_parser.isGbk();
    }
private:
    FILE *  m_fd;///<xml文件的FD
    string  m_strFileName;///<xml文件的名字
    char*   m_szBuf;///<XML文件读取BUF
    CwxXmlConfigParser  m_parser;///<CwxXmlConfigParser类，完成XML 内存的解析

};
#include "CwxPost.h"

CWINUX_END_NAMESPACE;

#endif /* _EXPATMM_EXPATXMLFILEPARSER_H */

