#ifndef __CWM_CENTER_TSS_H__
#define __CWM_CENTER_TSS_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
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

#include "CwxGlobalMacro.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxCommon.h"
#include "CwxPackageWriter.h"
#include "CwxAppLogger.h"
#include "CwxAppTss.h"
#include "CwxXmlPackageConv.h"
#include "CwmCmnMacro.h"

class CwmCenterActionLoop;
class CwmCenterServiceLoop;
class CwmCenterSystemLoop;
class CwmCenterActionNameLoop;
//center的tss
class CwmCenterTss:public CwxAppTss
{
public:
    enum
    {
        MAX_PACKAGE_SIZE = 2 * 1024 * 1024 ///<UI查询返回数据包的最大长度
    };
public:
    ///构造函数
    CwmCenterTss():CwxAppTss(new CwxAppTssInfo)
    {
        m_pSndPackage = NULL;
        m_szOutputBuf = NULL;
        m_uiOutputDataLen = 0;
        m_pActionNameLoop = NULL;
        m_pActionLoop = NULL;
        m_pServiceLoop = NULL;
        m_pSystemLoop = NULL;
    }
    ///析构函数
    ~CwmCenterTss();
public:
    ///tss的初始化
    int init();
public:
    CwxPackageWriter*   m_pSndPackage; ///<reply query's package writer
    char*              m_szOutputBuf; ///<template's output buf
    CWX_UINT32         m_uiOutputDataLen; ///<模板输出的数据长度
    char               m_szKeyValue64K[60 * 1024]; ///<模板变量key的data的临时空间
    CwmCenterActionNameLoop* m_pActionNameLoop; ///<service的action名字的loop
    CwmCenterActionLoop*  m_pActionLoop; ///<service的action的loop对象
    CwmCenterServiceLoop*  m_pServiceLoop;  ///<system的service的loop对象
    CwmCenterSystemLoop* m_pSystemLoop; ///<system的loop对象
};





#endif
