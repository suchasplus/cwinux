#ifndef __CWX_XML_PACKAGE_CONV_H__
#define __CWX_XML_PACKAGE_CONV_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxXmlPackageConv.h
@brief 定义XML与Package相互转换的对象CwxXmlPackageConv。
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
#include "CwxReplMachine.h"
#include "CwxPackageReader.h"
#include "CwxPackageWriter.h"
#include "CwxEncodeXml.h"
#include "CwxXmlConfigParser.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxXmlPackageConv
@brief 实现XML与Package的相互转换。
*/
class CWX_API CwxXmlPackageConv
{
public:
    ///构造函数，参数为XML的encode替换器，有外部进行管理
    CwxXmlPackageConv(CwxEncodeXml const* encode)
        :m_xmlEncode(encode)
    {
        memset(m_szErrMsg, 0x00, 512);
    }
    ///析构函数
    ~CwxXmlPackageConv()
    {

    }
public:
    /**
    *@brief  将XML转换为Package.
    *@param [in] szSrc XML。
    *@param [out] szOut 输出的PACKAGE。
    *@param [in,out] uiOutLen 输入szDesc的容量，输出形成的Package的大小。
    *@return false：失败；true：成功.
    */ 
    bool xmlToPackage(char const * szSrc,
        char* szOut,
        CWX_UINT32& uiOutLen);
    /**
    *@brief  将Package转换为完整的XML，默认采用UTF8编码
    *@param [in] szRootName 形成的XML根节点的名字,如果为空，则表示没有根节点。
    *@param [in] szSrc package的buf。
    *@param [in] uiSrcLen Package的大小。
    *@param [out] szOut 输出的XML。
    *@param [in,out] uiOutLen 输入XML的BUF大小，输出形成的XML的长度。
    *@param [in] szXmlTitile XML的标题，若不指定，则为【<?xml version='1.0' encoding=\"utf-8\" ?>】
    *@return false：失败；true：成功.
    */ 
    bool packageToXml(char const* szRootName,
        char const * szSrc,
        CWX_UINT32 uiSrcLen,
        char* szOut,
        CWX_UINT32& uiOutLen,
        char const* szXmlTitile=NULL);
    /**
    *@brief  将Package转换为完整的XML，默认采用UTF8编码
    *@param [in] szRootName 形成的XML根节点的名字,如果为空，则表示没有根节点。
    *@param [in] package package。
    *@param [out] szOut 输出的XML。
    *@param [in,out] uiOutLen 输入XML的BUF大小，输出形成的XML的长度。
    *@param [in] szXmlTitile XML的标题，若不指定，则为【<?xml version='1.0' encoding=\"utf-8\" ?>】
    *@return false：失败；true：成功.
    */ 
    bool packageToXml(char const* szRootName,
        CwxPackageReader& package,
        char* szOut,
        CWX_UINT32& uiOutLen,
        char const* szXmlTitile=NULL);
    /**
    *@brief  将package转换为以szNodeName为节点的XML片段.
    *@param [in] szNodeName 形成的XML的节点的名字,如果为空，则表示没有根节点。
    *@param [in] szSrc package的buf。
    *@param [in] uiSrcLen PACKAGE的长度。
    *@param [out] szOut 输出的XML。
    *@param [in,out] uiOutLen 输入XML的BUF大小，输出形成的XML的长度。
    *@return false：失败；true：成功.
    */ 
    bool packageToXmlNode(char const* szNodeName,
        char const * szSrc,
        CWX_UINT32 uiSrcLen,
        char* szOut,
        CWX_UINT32& uiOutLen);
    /**
    *@brief  将package转换为以szNodeName为节点的XML片段.
    *@param [in] szNodeName 形成的XML的节点的名字,如果为空，则表示没有根节点。
    *@param [in] package package。
    *@param [out] szOut 输出的XML。
    *@param [in,out] uiOutLen 输入XML的BUF大小，输出形成的XML的长度。
    *@return false：失败；true：成功.
    */ 
    bool packageToXmlNode(char const* szNodeName,
        CwxPackageReader& package,
        char* szOut,
        CWX_UINT32& uiOutLen);
    ///返回错误信息
    char const* getErrMsg() const{ return m_szErrMsg;}
private:
    ///xml转换为package
    bool xmlToPackage(CwxXmlTreeNode const * treeNode, char* szOut, CWX_UINT32& uiOutLen);
    ///package转换为xml
    bool packageToXml(CwxKeyValueItem const& item, char* szOut, CWX_UINT32& uiOutLen);
    ///append <key>
    bool appendXmlKeyBegin(char const* szKey, CWX_UINT16 unKeyLen, char* szOut, CWX_UINT32& uiOutLen);
    ///append </key>
    bool appendXmlKeyEnd(char const* szKey, CWX_UINT16 unKeyLen, char* szOut, CWX_UINT32& uiOutLen);
private:
    CwxEncodeXml const*  m_xmlEncode;///<xml的特殊字符替换器
    char m_szErrMsg[512];///<错误buf
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif
