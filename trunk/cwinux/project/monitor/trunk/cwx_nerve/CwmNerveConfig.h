#ifndef __CWM_NERVE_CONFIG_H__
#define __CWM_NERVE_CONFIG_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmNerveConfig.h
@brief Nerve的配置项加载文件。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-25
@warning
@bug
*/

#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwxXmlFileConfigParser.h"
#include "CwmCmnMacro.h"
#include "CwmCmnAlarm.h"
#include "CwmCmnAlarmRule.h"

/**
@class CwmNerveActionInfo
@brief Nerve的Action对象定义
*/
class CwmNerveActionInfo
{
public:
    ///默认构造函数
    CwmNerveActionInfo();
    ///CGI类型ACTION的构造函数
    CwmNerveActionInfo(string const& strName,
        CwmCmnAlarmRule const& alarmRule,
        string const& strAlarmType,
        string const& strCgi,
        bool bPoll = true);
    ///SERVER类型ACTION的构造函数
    CwmNerveActionInfo(string const& strName,
        CwmCmnAlarmRule const& alarmRule,
        string const& strAlarmType,
        CWX_UINT16 unMsgType,
        bool bPoll = true);
    ///拷贝构造
    CwmNerveActionInfo(CwmNerveActionInfo const& obj);
    ///赋值操作
    CwmNerveActionInfo &operator=(CwmNerveActionInfo const& obj);
    ///相同比较
    bool operator ==(CwmNerveActionInfo const& obj) const
    {
        return m_strName == obj.m_strName;
    }
public:
    ///设置名字
    inline void setName(string const& strName)
    {
        m_strName = strName;
    }
    ///获取名字
    inline string const& getName() const
    {
        return m_strName;
    }
    ///设置告警规则名字
    inline void setAlarmRule(CwmCmnAlarmRule const& alarmRule)
    {
        m_alarmRule = alarmRule;
    }
    ///获取告警规则名字
    inline CwmCmnAlarmRule const& getAlarmRule() const
    {
        return m_alarmRule;
    }
    ///设置是否执行poll
    inline void setPoll(bool bPoll)
    {
        m_bPoll = bPoll;
    }
    ///获取是否执行poll
    inline bool isPoll() const
    {
        return m_bPoll;
    }
    ///设置是否为CGI ACTION
    inline void setCgi(bool bCgi)
    {
        m_bCgiAction = bCgi;
    }
    ///获取是否为CGI ACTION
    inline bool isCgi() const
    {
        return m_bCgiAction;
    }
    ///设置告警类型
    inline void setAlarmType(string const strType)
    {
        m_strAlarmType = strType;
    }
    ///获取告警leixing
    inline string const& getAlarmType() const
    {
        return m_strAlarmType;
    }
    ///设置CGI的命令
    inline void setCgiCmd(string const& strCmd)
    {
        m_strCgi = strCmd;
    }
    ///获取CGI的命令
    inline string const& getCgiCmd() const
    {
        return m_strCgi;
    }
    ///设置消息类型
    inline void setMsgType(CWX_UINT16 unMsgType)
    {
        m_unMsgType = unMsgType;
    }
    ///获取消息类型
    inline CWX_UINT16 getMsgType() const
    {
        return m_unMsgType;
    }
    ///添加消息参数
    inline void addArg(string const& strName, string const& strData)
    {
        m_args.push_back(pair<string, string>(strName, strData));
    }
    ///获取消息参数
    inline list<pair<string, string> >const& getArg() const
    {
        return m_args;
    }
    ///清空消息参数
    inline void clearArg()
    {
        m_args.clear();
    }
public:
    inline void reset()
    {
        m_strName.erase();
        m_alarmRule.reset();
        m_bPoll = true;
        m_bCgiAction = false;
        m_strAlarmType.erase();
        m_strCgi.erase();
        m_unMsgType = 0;
        m_args.clear();
    }
private:
    string          m_strName; ///<action的名字
    CwmCmnAlarmRule  m_alarmRule; ///<action的告警规则
    bool            m_bPoll; ///<是否需要进行poll轮询
    bool            m_bCgiAction; ///<是否为CGI类型的action
    string          m_strAlarmType; ///<告警类型
    string          m_strCgi; ///<cgi类型action的cgi脚本
    CWX_UINT16      m_unMsgType; ///<非cgi类型action的消息类型
    list<pair<string, string> > m_args; ///<非cgi类型action的参数
};

/**
@class CwmNerveServiceInfo
@brief Nerve的Service对象定义
*/

class CwmNerveServiceInfo
{
public:
    ///缺省构造函数
    CwmNerveServiceInfo();
    ///cgi模式的构造函数
    CwmNerveServiceInfo(string const& strName,
        string const& strSystem,
        CwmCmnAlarmRule const& alarmRule);
    ///server模式的构造函数
    CwmNerveServiceInfo(string const& strName,
        string const& strSystem,
        CwmCmnAlarmRule const& alarmRule,
        string const& strServerIp,
        CWX_UINT16 unPort);
    ///拷贝构造
    CwmNerveServiceInfo(CwmNerveServiceInfo const& obj);
    ///赋值操作
    CwmNerveServiceInfo& operator=(CwmNerveServiceInfo const& obj);
    ///相等比较操作
    bool operator==(CwmNerveServiceInfo const& obj) const
    {
        return (m_strName == obj.m_strName) && (m_strSystem == obj.m_strSystem);
    }
public:
    ///设置名字
    inline void setName(string const& strName)
    {
        m_strName = strName;
    }
    ///获取名字
    inline string const& getName() const
    {
        return m_strName;
    }
    ///设置系统名
    inline void setSystem(string const& strSystem)
    {
        m_strSystem = strSystem;
    }
    ///获取系统名
    inline string const& getSystem() const
    {
        return m_strSystem;
    }
    ///设置是否为CGI
    inline void setCgi(bool bCgi)
    {
        m_bCgi = bCgi;
    }
    ///获取是否为CGI
    inline bool isCgi() const
    {
        return m_bCgi;
    }
    ///设置告警规则
    inline void setAlarmRule(CwmCmnAlarmRule const& alarmRule)
    {
        m_alarmRule = alarmRule;
    }
    ///获取告警规则
    inline CwmCmnAlarmRule const& getAlarmRule() const
    {
        return m_alarmRule;
    }
    ///设置server ip
    inline void setServerIp(string const& strIp)
    {
        m_strServeIp = strIp;
    }
    ///获取server ip
    inline string const& getServerIp() const
    {
        return m_strServeIp;
    }
    ///设置server port
    inline void setPort(CWX_UINT16 unPort)
    {
        m_unPort = unPort;
    }
    ///获取server port
    inline CWX_UINT16 getPort() const
    {
        return m_unPort;
    }
    ///添加action
    inline void addAction(CwmNerveActionInfo const& action)
    {
        m_actions.push_back(action);
    }
    ///获取一个action
    inline CwmNerveActionInfo const* getAction(string const& strName) const
    {
        list<CwmNerveActionInfo>::const_iterator iter = m_actions.begin();
        while(iter != m_actions.end())
        {
            if (iter->getName() == strName) return &(*iter);
            iter++;
        }
        return NULL;
    }
    ///获取action 列表
    inline list<CwmNerveActionInfo> const& getActions() const
    {
        return m_actions;
    }
    ///清空action
    inline void clearAction()
    {
        m_actions.clear();
    }
public:
    inline void reset()
    {
        m_strName.erase();
        m_strSystem.erase();
        m_bCgi = false;
        m_alarmRule.reset();
        m_strServeIp.erase();
        m_unPort = 0;
        m_actions.clear();
    }

private:
    string   m_strName; ///<service的名字
    string   m_strSystem; ///<service的系统名字
    bool     m_bCgi; ///<是否为cgi
    CwmCmnAlarmRule  m_alarmRule; ///<service的告警规则
    string   m_strServeIp; ///<对于服务类型的服务IP
    CWX_UINT16 m_unPort; ///<对于服务类型的port
    list<CwmNerveActionInfo> m_actions; ///<服务的action列表
};

/**
@class CwmNerveConfig
@brief Nerve配置项加载、管理对象
*/
class CwmNerveConfig
{
public:
    enum
    {
        DEF_TIMEOUT_SECOND = 10///<缺省的超时时间
    };
public:
    CwmNerveConfig()
    {
        m_uiTimeout = DEF_TIMEOUT_SECOND;
        memset(m_szErrMsg, 0x00, sizeof(m_szErrMsg));
    }    
    ~CwmNerveConfig()
    {

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
    ///设置数据目录
    inline void setDataPath(string const& strDataPath)
    {
        m_strDbFilePath = strDataPath;
    }
    ///获取数据目录
    inline string const& getDataPath() const
    {
        return m_strDbFilePath;
    }
    ///设置工作目录
    inline void setWorkDir(string const& strWorkDir)
    {
        m_strWorkDir = strWorkDir;
    }
    ///获取工作目录
    inline string const& getWorkDir() const
    {
        return m_strWorkDir;
    }
    ///设置查询监听
    inline void setQueryListen(CwxHostInfo const& host)
    {
        m_queryListen = host;
    }
    ///获取查询监听
    inline CwxHostInfo const& getQueryListen() const
    {
        return m_queryListen;
    }
    ///设置管理端口的监听
    inline void setMgrListen(CwxHostInfo const& host)
    {
        m_mgrListen = host;
    }
    ///获取管理端口的监听
    inline CwxHostInfo const& getMgrListen() const
    {
        return m_mgrListen;
    }
    ///设置告警host
    inline void setAlarmListen(CwxHostInfo const& host)
    {
        m_alarmListen = host;
    }
    ///获取告警host
    inline CwxHostInfo const& getAlarmListen() const
    {
        return m_alarmListen;
    }
    ///设置超时
    inline void setTimeout(CWX_UINT32 uiTimeout)
    {
        m_uiTimeout = uiTimeout;
    }
    ///获取超时
    inline CWX_UINT32 getTimeout() const
    {
        return m_uiTimeout;
    }
    ///设置节点的host的名字
    inline void setHostName(string const& strHostName)
    {
        m_strHostName = strHostName;
    }
    ///获取节点的host的名字
    inline string const& getHostName() const
    {
        return m_strHostName;
    }
    ///添加一个告警规则
    inline void addAlarmRule(CwmCmnAlarmRule const& rule)
    {
        m_alarmRules[rule.getName()] = rule;
    }
    ///删除一个告警规则
    inline bool eraseAlarmRule(string const& strName)
    {
        map<string, CwmCmnAlarmRule>::iterator iter=m_alarmRules.find(strName);
        if (iter != m_alarmRules.end())
        {
            m_alarmRules.erase(iter);
            return true;
        }
        return false;
    }
    ///获取一个告警规则
    inline CwmCmnAlarmRule const* getAlarmRule(string const& strName) const
    {
        map<string, CwmCmnAlarmRule>::const_iterator iter=m_alarmRules.find(strName);
        if (iter != m_alarmRules.end())
        {
            return &iter->second;
        }
        return NULL;
    }
    ///修改一个告警规则
    inline bool modifyAlarmRule(CwmCmnAlarmRule const& rule)
    {
        map<string, CwmCmnAlarmRule>::iterator iter=m_alarmRules.find(rule.getName());
        if (iter != m_alarmRules.end())
        {
            iter->second = rule;
            return true;
        }
        return false;
    }
    ///获取所有的规则
    map<string, CwmCmnAlarmRule> const& getAlarmRules() const
    {
        return m_alarmRules;
    }
    ///清空所有告警规则
    inline void clearAlarmRule()
    {
        m_alarmRules.clear();
    }
    ///添加一个service
    inline void addService(CwmNerveServiceInfo const& service)
    {
        pair<string, string> key(service.getSystem(), service.getName());
        m_services[key] = service;
    }
    ///删除一个service
    inline bool eraseService(string const& strSystem, string const& strName)
    {
        pair<string, string> key(strSystem, strName);
        map<pair<string, string> , CwmNerveServiceInfo>::iterator iter=m_services.find(key);
        if (iter != m_services.end())
        {
            m_services.erase(iter);
            return true;
        }
        return false;
    }
    ///获取一个service
    inline CwmNerveServiceInfo const* getService(string const& strSystem, string const& strName) const
    {
        pair<string, string> key(strSystem, strName);
        map<pair<string,string>, CwmNerveServiceInfo>::const_iterator iter=m_services.find(key);
        if (iter != m_services.end())
        {
            return &iter->second;
        }
        return NULL;
    }
    ///修改一个service
    inline bool modifyService(CwmNerveServiceInfo const& service)
    {
        pair<string, string> key(service.getSystem(), service.getName());
        map<pair<string,string>, CwmNerveServiceInfo>::iterator iter=m_services.find(key);
        if (iter != m_services.end())
        {
            iter->second = service;
            return true;
        }
        return false;
    }
    ///获取所有的service
    map<pair<string,string>, CwmNerveServiceInfo> const& getServices() const
    {
        return m_services;
    }
    ///清空所有service
    inline void clearService()
    {
        m_services.clear();
    }
public:
    void reset()
    {
        m_strWorkDir.erase();
        m_queryListen.reset();
        m_mgrListen.reset();
        m_alarmListen.reset();
        m_uiTimeout = DEF_TIMEOUT_SECOND;
        m_strHostName.erase();
        m_alarmRules.clear();
        m_services.clear();
        m_szErrMsg[0] = 0x00;
    }
private:
    CwmNerveConfig& operator=(CwmNerveConfig const& )
    {
        return *this;
    }

private:
    string              m_strWorkDir; ///<工作目录
    string              m_strDbFilePath; ///<db file
    CwxHostInfo       m_queryListen; ///<查询监听端口
    CwxHostInfo       m_mgrListen; ///<管理端口的监听
    CwxHostInfo       m_alarmListen; ///<CWX_CENTER的告警接收端口
    CWX_UINT32           m_uiTimeout; ///<timeout值
    string               m_strHostName; ///<机器名
    map<string, CwmCmnAlarmRule> m_alarmRules; ///<告警规则定义的map
    map<pair<string,string>, CwmNerveServiceInfo> m_services; ///<监控的service
    char                m_szErrMsg[512];///<错误消息buf
};

#endif
