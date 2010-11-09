#ifndef __CWM_NERVE_TSS_H__
#define __CWM_NERVE_TSS_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmNerveTss.h
@brief 监控系统中的Nerve服务的TSS定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-25
@warning
@bug
*/

#include "CwxGlobalMacro.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxCommon.h"
#include "CwxPackageWriter.h"
#include "CwxAppLogger.h"
#include "CwxAppTss.h"
#include "CwxXmlPackageConv.h"
#include "CwmCmnMacro.h"

//nerve的tss
class CwmNerveTss:public CwxAppTss
{
public:
    enum
    {
        MAX_PACKAGE_SIZE = 128 * 1024 ///<查询数据包的最大长度
    };
public:
    ///构造函数
    CwmNerveTss():CwxAppTss(new CwxAppTssInfo)
    {
        m_pSndPackage = NULL;
        m_szBuf = NULL;
        m_uiBufDataLen = 0;
        m_pXmlEncode = NULL;
        m_pXmlConv = NULL;
    }
    ///析构函数
    ~CwmNerveTss()
    {
        if (m_pSndPackage) delete m_pSndPackage;
        if (m_szBuf) delete [] m_szBuf;
        if (m_pXmlEncode) delete m_pXmlEncode;
        if (m_pXmlConv) delete m_pXmlConv;
    }
public:
    ///初始化tss的成员，-1：失败；0：成功
    int init()
    {
        m_pSndPackage = new CwxPackageWriter(MAX_PACKAGE_SIZE);
        m_szBuf = new char[MAX_PACKAGE_SIZE];
        m_pXmlEncode = new CwxEncodeXml(false, true);
        if (!m_pXmlEncode->init())
        {
            CWX_ERROR(("Failure to init xml-encode"));
            return -1;
        }
        m_pXmlConv = new CwxXmlPackageConv(m_pXmlEncode);
        return 0;
    }
public:
    ///获取发送的package writer
    CwxPackageWriter* getSndPackage()
    {
        return m_pSndPackage;
    }
    ///package的buf，与package writer大小相同，用于XML与package的转换
    char* getBuf()
    {
        return m_szBuf;
    }
    ///获取package的XML转换器
    CwxXmlPackageConv*   getXmlPackageConv()
    {
        return m_pXmlConv;
    }
    ///设置buf中数据的长度
    void setBufDataLen(CWX_UINT32 uiLen)
    {
        m_uiBufDataLen = uiLen;
    }
    ///获取buf中数据的长度
    CWX_UINT32 getBufDataLen() const
    {
        return m_uiBufDataLen;
    }
private:
    CwxPackageWriter*   m_pSndPackage; ///<reply query's package writer
    char*              m_szBuf;     ///<XML与PACKAGE转换的buf
    CWX_UINT32         m_uiBufDataLen; ///<buf中数据的长度
    CwxEncodeXml*       m_pXmlEncode;   ///<xml特殊字符转换
    CwxXmlPackageConv*   m_pXmlConv;  ///<xml与package的转换器
};





#endif
