#ifndef __CWX_XML_FILE_PARSER_H__
#define __CWX_XML_FILE_PARSER_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxXmlFileParser.h
@brief 定义了CwxXmlFileParser类，此类基于expat实现XML文件的流解析。
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

#include "CwxXmlParser.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxXmlFileParser
@brief 重载CwxXmlParser对象，实现XML文件的流解析
*/
class  CWX_API CwxXmlFileParser : public CwxXmlParser
{
public:
    ///构造文件，strFileName为要解析的XML的文件名
    CwxXmlFileParser(string const& strFileName);
    ///析构函数
    virtual ~CwxXmlFileParser();
protected:
    ///XML文件解析的准备
    virtual bool prepare();
    ///从XML文件中读取下一个待解析的数据块，-1：表示文件尾或文件读错误，通过status来识别，>=0：读取的数据的长度
    virtual ssize_t readBlock(void);
private:
    FILE *  m_fd; ///<XML文件的句柄
    string  m_strFileName;///<XML文件的名字

};
#include "CwxPost.h"

CWINUX_END_NAMESPACE;

#endif /* _EXPATMM_EXPATXMLFILEPARSER_H */

