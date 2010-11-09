#ifndef __CWM_CMN_RECORD_H__
#define __CWM_CMN_RECORD_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCmnRecord.h
@brief 采集的数据对象的定义文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-27
@warning
@bug
*/
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwmCmnMacro.h"


/**
@class CwmCmnRecord
@brief Nerve的告警对象
*/

class CwmCmnRecord{
public:
    enum{
        DATA_ALIGN = 128 ///<数据buf的对齐边界
    };
    
public:
    ///构造函数
    CwmCmnRecord(){
        m_uiPid = 0;
        m_uiWid = 0;
        m_uiDataLen = 0;
        m_uiBufLen = 0;
        m_pData = NULL; ///<采集的性能数据。
    }
    ///析构函数
    ~CwmCmnRecord(){
        if (m_pData) delete [] m_pData;
    }
    ///构造函数
    CwmCmnRecord(string const& strHost,
                 CWX_UINT32 uiPid,
                 CWX_UINT32 uiWid,
                 string const& strDatetime,
                 string const& strSystem,
                 string const& strService,
                 string const& strAction,
                 char const* pData,
                 CWX_UINT32 uiDataLen,
                 string const& strErr)
    {
        m_strHost = strHost;
        m_uiPid = uiPid;
        m_uiWid = uiWid;
        m_strDatetime = strDatetime;
        m_strSystem = strSystem;
        m_strService = strService;
        m_strAction = strAction;
        m_uiDataLen = 0;
        m_uiBufLen = 0;
        m_pData = NULL;
        setData(pData, uiDataLen);
        m_strError = strErr;
    }
    ///拷贝构造
    CwmCmnRecord(CwmCmnRecord const& obj){
        m_strHost = obj.m_strHost;
        m_uiPid = obj.m_uiPid;
        m_uiWid = obj.m_uiWid;
        m_strDatetime = obj.m_strDatetime;
        m_strSystem = obj.m_strSystem;
        m_strService = obj.m_strService;
        m_strAction = obj.m_strAction;
        m_uiDataLen = 0;
        m_uiBufLen = 0;
        m_pData = NULL;
        setData(obj.m_pData, obj.m_uiDataLen);
        m_strError = obj.m_strError;
    }
    ///赋值操作
    CwmCmnRecord& operator=(CwmCmnRecord const& obj){
        if (this != &obj){
            m_strHost = obj.m_strHost;
            m_uiPid = obj.m_uiPid;
            m_uiWid = obj.m_uiWid;
            m_strDatetime = obj.m_strDatetime;
            m_strSystem = obj.m_strSystem;
            m_strService = obj.m_strService;
            m_strAction = obj.m_strAction;
            setData(obj.m_pData, obj.m_uiDataLen);
            m_strError = obj.m_strError;
        }
        return *this;
    }
public:
    ///获取数据的host
    string const& getHost() const { return m_strHost;}
    ///设置数据的host
    void setHost(string const& strHost){ m_strHost = strHost;}
    ///获取数据ID
    CWX_UINT32 getPid() const { return m_uiPid;}
    ///设置数据ID
    void setPid(CWX_UINT32 uiId) { m_uiPid = uiId;}
    ///获取数据的告警id
    CWX_UINT32 getWid() const { return m_uiWid;}
    ///设置数据的告警的id
    void setWid(CWX_UINT32 uiId) { m_uiWid = uiId;}
    ///获取数据的时间
    string const& getDatetime() const { return m_strDatetime;}
    ///设置设置的时间
    void setDatetime(string const& strDatetime) { m_strDatetime = strDatetime;}
    ///获取数据所属的system
    string const& getSystem() const { return m_strSystem;}
    ///设置数据所属的system
    void setSystem(string const& strSystem) { m_strSystem = strSystem;}
    ///获取数据所属的service
    string const& getService() const { return m_strService;}
    ///设置数据所属的service
    void setService(string const& strService) { m_strService = strService;}
    ///获取数据所属的action
    string const& getAction() const { return m_strAction;}
    ///设置数据所属的action
    void setAction(string const& strAction) { m_strAction = strAction;}
    ///获取数据错误时的错误描述
    string const& getError() const { return m_strError;}
    ///设置数据错误时的错误描述
    void setError(string const& strErr) { m_strError = strErr;}
    ///获取数据的data
    char const* getData() const { return m_pData;}
    ///获取数据的长度
    CWX_UINT32  getDataLen() const { return m_uiDataLen;}
    ///获取数据的buf长度
    CWX_UINT32 getDataBufLen() const { return m_uiBufLen;}
    ///设置数据
    void setData(char const* pData, CWX_UINT32 uiDataLen){
        if (uiDataLen > m_uiBufLen){
           if (m_pData) delete [] m_pData;
           m_uiBufLen = uiDataLen - (uiDataLen%DATA_ALIGN) + DATA_ALIGN;
           m_pData = new char[m_uiBufLen];
        }
        if (uiDataLen) memcpy(m_pData, pData, uiDataLen);
        m_uiDataLen = uiDataLen;
    }

private:
    string      m_strHost; ///<数据的主机
    CWX_UINT32  m_uiPid;///<数据的id
    CWX_UINT32  m_uiWid; ///<若有告警，则对应告警id
    string      m_strDatetime; ///<数据采集的时间
    string      m_strSystem; ///<数据所属的系统
    string      m_strService; ///<数据所属的服务
    string      m_strAction; ///<数据所属的action
    string      m_strError; ///<若数据采集有错误，则为错误的描述
    CWX_UINT32  m_uiBufLen; ///data的buf大小
    CWX_UINT32  m_uiDataLen; ///<采集的数据的长度
    char*        m_pData; ///<采集的性能数据。
};

#endif
