#ifndef __CWF_FILTER_TSS_H__
#define __CWF_FILTER_TSS_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
/**
@file CwmCenterTss.h
@brief 监控系统中的Center服务的TSS定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-05
@warning
@bug
*/

#include "CwfFilterMacro.h"
#include "CwxAppLogger.h"
#include "CwxAppTss.h"
#include "CwxPackageReader.h"
#include "CwxPackageWriter.h"

//center的tss
class CwfFilterTss:public CwxAppTss
{
public:
    enum
    {
        MAX_PACKAGE_SIZE = 256 * 1024 ///<UI查询返回数据包的最大长度
    };
public:
    ///构造函数
    CwfFilterTss():CwxAppTss(new CwxAppTssInfo)
    {
        m_pSndPackage = NULL;
        m_pReader = NULL;
    }
    ///析构函数
    ~CwfFilterTss();
public:
    ///tss的初始化
    int init();
public:
    CwxPackageWriter*   m_pSndPackage; ///<reply query's package writer
    CwxPackageReader*   m_pReader;
};





#endif
