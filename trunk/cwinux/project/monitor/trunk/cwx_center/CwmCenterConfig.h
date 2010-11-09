#ifndef __CWM_CENTER_CONFIG_H__
#define __CWM_CENTER_CONFIG_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmCenterConfig.h
@brief Center的配置项加载文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-11-2
@warning
@bug
*/

#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwxXmlFileConfigParser.h"
#include "CwxHtmlTmpl.h"
#include "CwmCmnMacro.h"
#include "CwmCmnAlarm.h"
#include "CwmCmnAlarmRule.h"

/**
@class CwmCenterTemplate
@brief 配置中的模板项对象
*/

class CwmCenterTemplate
{
public:
    ///构造函数
    CwmCenterTemplate()
    {

    }
    ///构造函数
    CwmCenterTemplate(string const& strName, string const& strFile, string const& strType):
    m_strName(strName), m_strFile(strFile), m_strType(strType)
    {
    }
    ///析构函数
    ~CwmCenterTemplate()
    {

    }
public:
    ///获取名字
    string const& getName() const
    {
        return m_strName;
    }
    ///设置名字
    void setName(string const& strName)
    {
        m_strName = strName;
    }
    ///获取模板文件的名字
    string const& getFileName() const 
    {
        return m_strFile;
    }
    ///设置模板文件的名字
    void setFileName(string const& strFile)
    {
        m_strFile = strFile;
    }
    ///获取模板输出的文本类型，为MIME
    string const& getType() const
    {
        return m_strType;
    }
    ///设置模板输出的文本类型
    void setType(string const& strType)
    {
        m_strType = strType;
    }
    ///获取template
    CwxHtmlTmpl const* getParser() const
    {
        return &m_parser;
    }
private:
    ///拷贝构造
    CwmCenterTemplate(CwmCenterTemplate const& )
    {

    }
    ///赋值操作
    CwmCenterTemplate& operator=(CwmCenterTemplate const& )
    {
        return *this;
    }
private:
    string         m_strName;///<模板的名字
    string         m_strFile; ///<模板的文件名
    string         m_strType; ///<模板输出的类型，为MIME的类型定义，缺省为text/html
    CwxHtmlTmpl    m_parser; ///<template的解析器，实现文本的动态输出
};

/**
@class CwmCenterAlarmMail
@brief 定义不同类型告警的发送email,对于同一级别，可以设置多个email，不同email通过【;】分割
*/
class CwmCenterAlarmMail
{
public:
    ///构造函数
    CwmCenterAlarmMail()
    {

    }
    ///析构函数
    ~CwmCenterAlarmMail()
    {

    }
    ///拷贝构造
    CwmCenterAlarmMail(CwmCenterAlarmMail const& obj)
    {
        m_strName = obj.m_strName;
        m_warning = obj.m_warning;
        m_minor = obj.m_minor;
        m_major = obj.m_major;
        m_critical = obj.m_critical;
        m_fatal = obj.m_fatal;
    }
    ///赋值操作
    CwmCenterAlarmMail& operator=(CwmCenterAlarmMail const& obj)
    {
        if (this != &obj)
        {
            m_strName = obj.m_strName;
            m_warning = obj.m_warning;
            m_minor = obj.m_minor;
            m_major = obj.m_major;
            m_critical = obj.m_critical;
            m_fatal = obj.m_fatal;
        }
        return *this;
    }
public:
    ///获取规则的名字
    string const& getName() const 
    {
        return m_strName;
    }
    ///设置规则的名字
    void setName(string const& strName)
    {
        m_strName = strName;
    }
    ///获取告警的email, ucLevel为CwmCmnAlarm::ALARM_WARNING~CwmCmnAlarm::ALARM_FATAL
    list<string> const& getAlarmEmails(CWX_UINT8 ucLevel) const;
    ///获取告警的email, ucLevel为CwmCmnAlarm::ALARM_WARNING~CwmCmnAlarm::ALARM_FATAL
    string& getAlarmEmails(CWX_UINT8 ucLevel, string& strAlarms) const;
    ///根据当前告警level与前一级level，获取告警的mail
    void getAlarmEmails(CWX_UINT8 ucPLevel, CWX_UINT8 ucLevel, set<string>& emails) const;
    ///根据当前告警level与前一级level，获取告警的mail
    string& getAlarmEmails(CWX_UINT8 ucPLevel, CWX_UINT8 ucLevel, string& strEmails) const;
    ///设置告警的email，此为全替换模式
    void setAlarmEmails(CWX_UINT8 ucLevel, string const& strEmails);
    ///添加告警email，此为append模式
    void appendAlarmEmails(CWX_UINT8 ucLevel, string const& strEmails);
public:
    void reset()
    {
        m_strName.erase();
        m_warning.clear();
        m_minor.clear();
        m_major.clear();
        m_critical.clear();
        m_fatal.clear();

    }
private:
    void setEmails(list<string>& emails, string const& strEmails);
    void appendEmails(list<string>& emails, string const& strEmails);
private:
    string          m_strName; ///<规则的名字
    list<string>     m_warning; ///<warning级别告警的email发送列表
    list<string>     m_minor;  ///<minor级别告警的email发送列表
    list<string>     m_major;  ///<major级别告警的email发送列表
    list<string>     m_critical; ///<critical级别告警的email发送列表
    list<string>     m_fatal; ///<fatal级别告警的email发送列表
};

class CwmCenterHost;
/**
@class CwmCenterService
@brief Center管理的系统
*/
class CwmCenterService
{
public:
    ///构造函数
    CwmCenterService()
    {
        m_pAlarmMail = NULL;
    }
    ///析构函数
    ~CwmCenterService()
    {

    }
    ///拷贝构造
    CwmCenterService(CwmCenterService const& obj)
    {
        m_strName = obj.m_strName;
        m_lsActions = obj.m_lsActions;
        m_pAlarmMail = obj.m_pAlarmMail;
        m_actions = obj.m_actions;
    }
    ///赋值操作
    CwmCenterService& operator=(CwmCenterService const& obj)
    {
        if (this != &obj)
        {
            m_strName = obj.m_strName;
            m_lsActions = obj.m_lsActions;
            m_pAlarmMail = obj.m_pAlarmMail;
            m_actions = obj.m_actions;
        }
        return *this;
    }
public:
    ///获取service的名字
    string const& getName() const
    {
        return m_strName;
    }
    ///设置service的名字
    void setName(string const& strName)
    {
        m_strName = strName;
    }
    ///获取告警的规则
    inline CwmCenterAlarmMail const* getMailRule() const
    {
        return m_pAlarmMail;
    }
    ///设置告警的规则
    inline void setMailRule(CwmCenterAlarmMail const* pRule)
    {
        m_pAlarmMail = pRule;
    }
    ///获取service的action列表
    inline list<string> const& getActions() const
    {
        return m_lsActions;
    }
    inline bool isExistAction(string const& strAction)
    {
        return m_actions.find(strAction) != m_actions.end();
    }
    ///添加一个action的定义
    inline void addAction(string const& strAction)
    {
        m_lsActions.push_back(strAction);
        m_actions.insert(strAction);
    }
    ///获取所有的主机
    inline map<string/*ip*/, CwmCenterHost const*> const& getHosts() const
    {
        return m_hosts;
    }
    ///根据主机的Ip，获取指定的主机
    inline CwmCenterHost const* getHost(string const& ip) const
    {
        map<string/*ip*/, CwmCenterHost const*>::const_iterator iter = m_hosts.find(ip);
        if (iter == m_hosts.end()) return NULL;
        return iter->second;
    }
    ///添加一个host
    bool addHost(CwmCenterHost const* pHost);
public:
    ///清空对象
    void reset()
    {
        m_actions.clear();
        m_hosts.clear();
        m_pAlarmMail = NULL;
        m_strName.erase();
    }
private:
    string     m_strName; ///<service的名字
    CwmCenterAlarmMail const*  m_pAlarmMail; ///<服务的告警email规则
    set<string> m_actions; ///<service包含的action列表
    list<string> m_lsActions; ///<按照配置次序的action列表
    map<string/*ip*/, CwmCenterHost const*> m_hosts; ///<系统的机器
};

/**
@class CwmCenterServiceKey
@brief Center监控的system的service map的Key对象
*/
class CwmCenterServiceKey
{
public:
    ///构造函数
    CwmCenterServiceKey()
    {

    }
    ///构造函数
    CwmCenterServiceKey(string const& strSystem, string const& strService)
    {
        m_strSystem = strSystem;
        m_strService = strService;
    }
    ///析构函数
    ~CwmCenterServiceKey()
    {

    }
    ///拷贝构造
    CwmCenterServiceKey(CwmCenterServiceKey const& obj)
    {
        m_strSystem = obj.m_strSystem;
        m_strService = obj.m_strService;
    }
    ///赋值操作
    CwmCenterServiceKey& operator=(CwmCenterServiceKey const& obj)
    {
        if (this != &obj)
        {
            m_strSystem = obj.m_strSystem;
            m_strService = obj.m_strService;
        }
        return *this;
    }
    ///小于操作
    bool operator<(CwmCenterServiceKey const& obj) const
    {
        if (m_strSystem < obj.m_strSystem) return true;
        if (m_strSystem > obj.m_strSystem) return true;
        return m_strService < obj.m_strService;
    }
    ///等于操作
    bool operator==(CwmCenterServiceKey const& obj) const
    {
        return (m_strSystem == obj.m_strSystem)&& (m_strService == obj.m_strService);
    }
public:
    ///获取system名字
    inline string const& getSystem() const
    {
        return m_strSystem;
    }
    ///设置system名字
    inline void setSystem(string const& strSystem)
    {
        m_strSystem = strSystem;
    }
    ///获取service名字
    inline string const& getService() const
    {
        return m_strService;
    }
    ///设置service名字
    inline void setService(string const& strService)
    {
        m_strService = strService;
    }
private:
    string     m_strSystem; ///<service的system名字
    string     m_strService; ///<service的名字
};

/**
@class CwmCenterHost
@brief Center监控的host对象
*/
class CwmCenterHost
{
public:
    ///构造函数
    CwmCenterHost()
    {
        m_unPort = 0;
        m_pAlarmRule = NULL;
        m_pMailRule = NULL;
    }
    ///析构函数
    ~CwmCenterHost()
    {

    }
public:
    ///获取名字
    inline string const& getName() const
    {
        return m_strName;
    }
    ///设置名字
    inline void setName(string const& strName)
    {
        m_strName = strName;
    }
    ///获取IP
    inline string const& getIp() const
    {
        return m_strIp;
    }
    ///设置IP
    inline void setIp(string const& strIp)
    {
        m_strIp = strIp;
    }
    ///获取port
    inline CWX_UINT16 getPort() const
    {
        return m_unPort;
    }
    ///设置port
    inline void setPort(CWX_UINT16 unPort)
    {
        m_unPort = unPort;
    }
    ///获取告警的规则
    inline CwmCmnAlarmRule const* getAlarmRule() const
    {
        return m_pAlarmRule;
    }
    ///设置告警的规则
    inline void setAlarmRule(CwmCmnAlarmRule const* pRule)
    {
        m_pAlarmRule = pRule;
    }
    ///获取告警mail的规则
    inline CwmCenterAlarmMail const* getMailRule() const
    {
        return m_pMailRule;
    }
    ///设置告警mail的规则
    inline void setMailRule(CwmCenterAlarmMail const* pMail)
    {
        m_pMailRule = pMail;
    }
    ///获取主机上的所有system的所有service
    inline map<CwmCenterServiceKey, CwmCenterService*> const& getServices() const
    {
        return m_serviceMap;
    }
    ///添加一个system的service
    inline bool addService(string const& strSystem,
        string const& strService,
        CwmCenterService* pService)
    {
        CwmCenterServiceKey key(strSystem, strService);
        map<CwmCenterServiceKey, CwmCenterService*>::iterator iter = m_serviceMap.find(key);
        if (iter != m_serviceMap.end()) return false;
        m_serviceMap[key] = pService;
        return true;
    }
    ///获取指定的service
    inline CwmCenterService const* getService(string const& strSystem,
        string const& strService) const
    {
        CwmCenterServiceKey key(strSystem, strService);
        map<CwmCenterServiceKey, CwmCenterService*>::const_iterator iter = m_serviceMap.find(key);
        if (iter != m_serviceMap.end())
        {
            return iter->second;
        }
        return NULL;
    }
private:
    ///拷贝构造
    CwmCenterHost(CwmCenterHost const& )
    {

    }
    ///赋值操作
    CwmCenterHost& operator=(CwmCenterHost const& )
    {
        return *this;
    }
private:
    string       m_strName;///<主机的名字
    string       m_strIp; ///<主机的ip
    CWX_UINT16    m_unPort; ///<主机的PORT
    CwmCmnAlarmRule const*   m_pAlarmRule; ///<告警规则
    CwmCenterAlarmMail const*  m_pMailRule; ///<告警的email规则
    map<CwmCenterServiceKey, CwmCenterService*> m_serviceMap; ///<system的service的map
};


/**
@class CwmCenterSystem
@brief Center管理的系统
*/
class CwmCenterSystem
{
public:
    ///构造函数
    CwmCenterSystem()
    {

    }
    ///构造函数
    CwmCenterSystem(string const& strName, string const& strDesc)
    {
        m_strName = strName;
        m_strDesc = strDesc;
    }
    ///析构函数
    ~CwmCenterSystem()
    {
        reset();
    }
public:
    ///获取系统的名字
    inline string const& getName() const
    {
        return m_strName;
    }
    ///设置系统的名字
    inline void setName(string const& strName)
    {
        m_strName = strName;
    }
    ///获取系统的描述
    inline string const& getDesc() const
    {
        return m_strDesc;
    } 
    ///设置系统的面搜狐
    inline void setDesc(string const& strDesc)
    {
        m_strDesc = strDesc;
    }
    ///获取系统的配置文件名
    inline string const& getFileName() const
    {
        return m_strFile;
    }
    ///设置系统的配置文件名
    inline void setFileName(string const& strFile)
    {
        m_strFile = strFile;
    }
    ///获取系统的所有告警规则
    inline map<string/*alarm name*/, CwmCmnAlarmRule*> const& getAlarmRules() const
    {
        return m_alarmRules;
    }
    ///获取指定的告警规则
    inline CwmCmnAlarmRule const* getAlarmRule(string const& strName) const
    {
        map<string/*alarm name*/, CwmCmnAlarmRule*>::const_iterator iter = m_alarmRules.find(strName);
        if (iter == m_alarmRules.end()) return NULL;
        return iter->second;
    }
    ///添加告警规则
    inline bool addAlarmRule(CwmCmnAlarmRule* pAlarmRule)
    {
        map<string/*alarm name*/, CwmCmnAlarmRule*>::iterator iter = m_alarmRules.find(pAlarmRule->getName());
        if (iter != m_alarmRules.end()) return false;
        m_alarmRules[pAlarmRule->getName()] = pAlarmRule;
        return true;
    }
    ///获取所有email的规则
    inline map<string/*mail name*/, CwmCenterAlarmMail*> const& getAlarmMails() const
    {
        return m_alarmMails;
    }
    ///获取指定的告警email规则
    inline CwmCenterAlarmMail const* getAlarmMail(string const& strMailName) const 
    {
        map<string/*mail name*/, CwmCenterAlarmMail*>::const_iterator iter = m_alarmMails.find(strMailName);
        if (iter == m_alarmMails.end()) return NULL;
        return iter->second;
    }
    ///添加mail规则
    inline bool addAlarmMail(CwmCenterAlarmMail* pAlarmMail)
    {
        map<string/*mail name*/, CwmCenterAlarmMail*>::iterator iter = m_alarmMails.find(pAlarmMail->getName());
        if (iter != m_alarmMails.end()) return false;
        m_alarmMails[pAlarmMail->getName()] = pAlarmMail;
        return true;
    }
    ///获取所有的service
    inline map<string/*service name*/, CwmCenterService*> const& getServices() const
    {
        return m_services;
    }
    ///获取指定的service
    inline CwmCenterService const* getService(string const& strName) const
    {
        map<string/*service name*/, CwmCenterService*>::const_iterator iter = m_services.find(strName);
        if (iter == m_services.end()) return NULL;
        return iter->second;
    }
    ///添加service
    inline bool addService(CwmCenterService* pService)
    {
        map<string/*service name*/, CwmCenterService*>::const_iterator iter = m_services.find(pService->getName());
        if (iter != m_services.end()) return false;
        m_services[pService->getName()] = pService;
        return true;
    }
public:
    void reset()
    {
        {
            map<string/*alarm name*/, CwmCmnAlarmRule*>::iterator iter = m_alarmRules.begin();
            while(iter != m_alarmRules.end())
            {
                delete iter->second;
                iter++;
            }
            m_alarmRules.clear();
        }
        {
            map<string/*mail name*/, CwmCenterAlarmMail*>::iterator iter = m_alarmMails.begin();
            while(iter != m_alarmMails.begin())
            {
                delete iter->second;
                iter++;
            }
            m_alarmMails.clear();
        }
        {
            map<string/*service name*/, CwmCenterService*>::iterator iter=m_services.begin();
            while(iter != m_services.end())
            {
                delete iter->second;
                iter++;
            }
            m_services.clear();
        }
    }
private:
    ///拷贝构造
    CwmCenterSystem(CwmCenterSystem const& )
    {

    }
    ///赋值操作
    CwmCenterSystem & operator=(CwmCenterSystem const&)
    {
        return *this;
    }
private:
    string      m_strName;///<系统的名字
    string      m_strDesc; ///<系统的描述
    string      m_strFile; ///<系统的配置文件
    map<string/*alarm name*/, CwmCmnAlarmRule*>  m_alarmRules; ///<系统的告警规则
    map<string/*mail name*/, CwmCenterAlarmMail*> m_alarmMails; ///<系统的email规则
    map<string/*service name*/, CwmCenterService*>  m_services; ///<系统包含的服务
};

/**
@class CwmCenterConfig
@brief Center配置项加载、管理对象
*/
class CwmCenterConfig
{
public:
    enum
    {
        DEF_TIMEOUT_SECOND = 10,///<缺省的超时时间
        DEF_THREAD_NUM = 2 ///<缺省的thread的数量
    };
public:
    CwmCenterConfig()
    {
        m_uiThreadNum = DEF_THREAD_NUM;
        m_uiQueryTimeout = DEF_TIMEOUT_SECOND;
        m_uiAlarmTimeout = DEF_TIMEOUT_SECOND;
        m_bAvgReport = true;
        memset(m_szErrMsg, 0x00, 512);
    }    
    ~CwmCenterConfig()
    {
        this->reset();
    }
public:
    //加载配置文件.false:failure, true:success
    bool loadConfig(string const & strConfFile);
    //输出配置文件
    void outputConfig();
    //获取加载配置文件的失败错误信息
    char const* getErrMsg()
    {
        return m_szErrMsg;
    }
public:
    ///获取工作目录
    inline string const& getWorkDir() const
    {
        return m_strWorkDir;
    }
    ///获取query的listen
    inline CwxHostInfo const& getQueryListen() const
    {
        return  m_queryListen;
    }
    ///获取管理端口的监听
    inline CwxHostInfo const& getMgrListen() const
    {
        return m_mgrListen;
    }
    ///获取告警接收listen
    inline CwxHostInfo const& getAlarmAccept() const
    {
        return m_alarmAccept;
    }
    ///获取告警分发listen
    inline CwxHostInfo const& getAlarmReport() const
    {
        return m_alarmReport;
    }
    ///是否均匀report告警
    inline bool isAvgReport() const
    {
        return m_bAvgReport;
    }
    ///获取工作线程的数量
    inline CWX_UINT32 getThreadNum() const
    {
        return m_uiThreadNum;
    }
    ///获取query的超时时间
    inline CWX_UINT32 getQueryTimeout() const
    {
        return m_uiQueryTimeout;
    }
    ///设置query的超时时间
    inline void setQueryTimeout(CWX_UINT32 uiTimeout)
    {
        m_uiQueryTimeout = uiTimeout;
    }
    ///获取alarm timeout时间
    inline CWX_UINT32  getAlarmTimeout() const
    {
        return m_uiAlarmTimeout;
    }
    ///获取数据文件的目录
    inline string const& getDbFilePath() const
    {
        return m_strDbFilePath;
    }
    ///获取页面的WEB url
    inline string const& getWebUrl() const
    {
        return m_strWebUrl;
    }  
    ///获取所有模板文件的map
    inline map<string/*name*/, CwmCenterTemplate*> const& getTemplates() const
    {
        return m_templates;
    }
    ///根据模板名，获取模板对象的指针
    inline CwmCenterTemplate const* getTemplate(string const& strName) const
    {
        map<string/*name*/, CwmCenterTemplate*>::const_iterator iter=m_templates.find(strName);
        if (iter != m_templates.end()) return iter->second;
        return NULL;
    }
    ///获取所有的告警规则
    inline map<string/*name*/, CwmCmnAlarmRule*> const& getAlarmRules() const
    {
        return m_alarmRules;
    }
    ///获取指定的告警规则，NULL表示不存在
    inline CwmCmnAlarmRule const* getAlarmRule(string const& strRuleName) const
    {
        map<string/*name*/, CwmCmnAlarmRule*>::const_iterator iter = m_alarmRules.find(strRuleName);
        if (iter == m_alarmRules.end()) return NULL;
        return iter->second;
    }
    ///获取所有的告警email规则
    inline map<string/*name*/, CwmCenterAlarmMail*> const& getAlarmMails() const
    {
        return m_alarmMails;
    }
    ///获取指定的告警email嘴个，NULL表示不存在
    inline CwmCenterAlarmMail const* getAlarmMail(string const& strMailName) const
    {
        map<string/*name*/, CwmCenterAlarmMail*>::const_iterator iter = m_alarmMails.find(strMailName);
        if (iter == m_alarmMails.end()) return NULL;
        return iter->second;
    }
    ///获取指定system、service的email对象
    inline CwmCenterAlarmMail const* getAlarmMail(string const& strSystem, string const& strService) const
    {
        CwmCenterAlarmMail const* pEmail = NULL;
        CwmCenterSystem const* pSystem = getSystem(strSystem);
        if (!pSystem)
        {
            return getAlarmMail(CWM_CMN_DEFAULT);
        }
        CwmCenterService const* pService = pSystem->getService(strService);
        if (!pService)
        {
            pEmail = pSystem->getAlarmMail(CWM_CMN_DEFAULT);
            if (pEmail) return pEmail;
            return getAlarmMail(CWM_CMN_DEFAULT);
        }
        return pService->getMailRule();
    }
    ///获取所管理的所有系统
    inline map<string/*system name*/, CwmCenterSystem*> const& getSystems() const
    {
        return m_systems;
    }
    ///获取指定的系统，NULL表示不存在
    inline CwmCenterSystem const* getSystem(string const& strName) const
    {
        map<string/*system name*/, CwmCenterSystem*>::const_iterator iter = m_systems.find(strName);
        if (iter == m_systems.end()) return NULL;
        return iter->second;
    }
    ///获取所有的主机
    inline map<string/*ip*/, CwmCenterHost*> const& getHosts() const
    {
        return m_hosts;
    }
    ///根据主机的Ip，获取指定的主机
    inline CwmCenterHost * getHost(string const& ip)
    {
        map<string/*ip*/, CwmCenterHost*>::iterator iter = m_hosts.find(ip);
        if (iter == m_hosts.end()) return NULL;
        return iter->second;
    }
public:
    void reset();
private:
    bool loadTemplate(CwxXmlTreeNode const* pNode, CwmCenterTemplate& tmpt);
    bool loadAlarmRule(CwxXmlTreeNode const* pNode, CwmCmnAlarmRule& alarmRule);
    bool loadAlarmMail(CwxXmlTreeNode const* pNode, CwmCenterAlarmMail& alarmMail);
    bool loadSystem(string const& strName, string const& strFile, CwmCenterSystem& system);
private:
    ///设置工作目录
    inline void setWorkDir(string const& strWorkDir)
    {
        m_strWorkDir = strWorkDir;
    }
    ///设置query的listen
    inline void setQueryListen(CwxHostInfo const& listen)
    {
        m_queryListen = listen;
    }
    ///设置管理端口的监听
    inline void setMgrListen(CwxHostInfo const& host)
    {
        m_mgrListen = host;
    }
    ///设置告警接收listen
    inline void setAlarmAccept(CwxHostInfo const& listen)
    {
        m_alarmAccept = listen;
    }
    ///设置告警分发listen
    inline void setAlarmReport(CwxHostInfo const& listen)
    {
        m_alarmReport = listen;
    }
    ///设置工作线程的数量
    inline void setThreadNum(CWX_UINT32 uiNum)
    {
        m_uiThreadNum = uiNum;
    }
    ///设置alarm timeout时间
    inline void setAlarmTimeout(CWX_UINT32 uiTimeout)
    {
        m_uiAlarmTimeout = uiTimeout;
    }
    ///设置数据文件的目录
    inline void setDbFilePath(string const& strPath)
    {
        m_strDbFilePath = strPath;
    }
    ///设置WEB页面的URL
    inline void setWebUrl(string const& strUrl)
    {
        m_strWebUrl = strUrl;
    }
    ///添加模板，返回值，true：成功；false：失败
    inline bool addTemplate(CwmCenterTemplate* pTemplate)
    {
        if (m_templates.find(pTemplate->getName()) != m_templates.end()) return false;
        m_templates[pTemplate->getName()] = pTemplate;
        return true;
    }
    ///添加告警规则
    inline bool addAlarmRule(CwmCmnAlarmRule* pRule)
    {
        map<string/*name*/, CwmCmnAlarmRule*>::iterator iter = m_alarmRules.find(pRule->getName());
        if (iter != m_alarmRules.end()) return false;
        m_alarmRules[pRule->getName()] = pRule;
        return true;
    }
    ///添加告警email规则
    inline bool addAlarmMail(CwmCenterAlarmMail* pMail)
    {
        map<string/*name*/, CwmCenterAlarmMail*>::iterator iter = m_alarmMails.find(pMail->getName());
        if (iter != m_alarmMails.end()) return false;
        m_alarmMails[pMail->getName()] = pMail;
        return true;
    }
    ///添加一个系统
    inline bool addSystem(CwmCenterSystem* pSystem)
    {
        map<string/*system name*/, CwmCenterSystem*>::iterator iter = m_systems.find(pSystem->getName());
        if (iter != m_systems.end()) return false;
        m_systems[pSystem->getName()] = pSystem;
        return true;
    }
    ///添加一个host
    inline bool addHost(CwmCenterHost* pHost)
    {
        map<string/*ip*/, CwmCenterHost*>::iterator iter = m_hosts.find(pHost->getIp());
        if (iter != m_hosts.end()) return false;
        m_hosts[pHost->getIp()] = pHost;
        return true;
    }

private:
    CwmCenterConfig(CwmCenterConfig const&)
    {

    }
    CwmCenterConfig& operator=(CwmCenterConfig const& )
    {
        return *this;
    }
private:
    string              m_strWorkDir; ///<工作目录
    string               m_strDbFilePath; ///<db file
    CwxHostInfo        m_mgrListen; ///<管理监听端口
    CwxHostInfo       m_queryListen; ///<查询监听端口
    CwxHostInfo       m_alarmAccept; ///<告警接收的监听端口
    CwxHostInfo       m_alarmReport; ///<告警分发的监听端口
    bool                m_bAvgReport; ///<是否对连接上来的告警接收端均匀发送
    CWX_UINT32           m_uiThreadNum; ///<工作线程的数量
    CWX_UINT32           m_uiQueryTimeout; ///<查询的timeout值
    CWX_UINT32           m_uiAlarmTimeout; ///<告警发布的timeout值
    string               m_strWebUrl; ///<页面的域名URL
    map<string/*name*/, CwmCenterTemplate*> m_templates; ///<模板文件的
    map<string/*name*/, CwmCmnAlarmRule*>  m_alarmRules; ///<告警规则的map
    map<string/*name*/, CwmCenterAlarmMail*> m_alarmMails; ///<告警email的规则
    map<string/*system name*/, CwmCenterSystem*> m_systems; ///<系统配置
    map<string/*ip*/, CwmCenterHost*> m_hosts; ///<管理的机器
    char                m_szErrMsg[512];///<错误消息buf
};

#endif
