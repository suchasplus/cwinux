#include "CwmNerveConfig.h"
#include "CwxAppLogger.h"

///默认构造函数
CwmNerveActionInfo::CwmNerveActionInfo()
{
    m_bPoll = true;
    m_bCgiAction = false;
    m_unMsgType = 0;
};
///CGI类型ACTION的构造函数
CwmNerveActionInfo::CwmNerveActionInfo(string const& strName,
                                       CwmCmnAlarmRule const& alarmRule,
                                       string const& strAlarmType,
                                       string const& strCgi,
                                       bool bPoll)
{
    m_strName = strName;
    m_alarmRule = alarmRule;
    m_bPoll = bPoll;
    m_bCgiAction = true;
    m_strAlarmType = strAlarmType;
    m_strCgi = strCgi;
    m_unMsgType = 0;
}
///SERVER类型ACTION的构造函数
CwmNerveActionInfo::CwmNerveActionInfo(string const& strName,
                                       CwmCmnAlarmRule const& alarmRule,
                                       string const& strAlarmType,
                                       CWX_UINT16 unMsgType,
                                       bool bPoll)
{
    m_strName = strName;
    m_alarmRule = alarmRule;
    m_bPoll = bPoll;
    m_bCgiAction = false;
    m_strAlarmType = strAlarmType;
    m_unMsgType = unMsgType;
}
///拷贝构造
CwmNerveActionInfo::CwmNerveActionInfo(CwmNerveActionInfo const& obj)
{
    m_strName = obj.m_strName;
    m_alarmRule = obj.m_alarmRule;
    m_bPoll = obj.m_bPoll;
    m_bCgiAction = obj.m_bCgiAction;
    m_strAlarmType = obj.m_strAlarmType;
    m_strCgi = obj.m_strCgi;
    m_unMsgType = obj.m_unMsgType;
    m_args = obj.m_args;
}
///赋值操作
CwmNerveActionInfo &CwmNerveActionInfo::operator=(CwmNerveActionInfo const& obj)
{
    if (this != &obj)
    {
        m_strName = obj.m_strName;
        m_alarmRule = obj.m_alarmRule;
        m_bPoll = obj.m_bPoll;
        m_bCgiAction = obj.m_bCgiAction;
        m_strAlarmType = obj.m_strAlarmType;
        m_strCgi = obj.m_strCgi;
        m_unMsgType = obj.m_unMsgType;
        m_args = obj.m_args;
    }
    return *this;
}

///缺省构造函数
CwmNerveServiceInfo::CwmNerveServiceInfo()
{
    m_bCgi = false;
    m_unPort = 0;
}
///cgi模式的构造函数
CwmNerveServiceInfo::CwmNerveServiceInfo(string const& strName,
                                         string const& strSystem,
                                         CwmCmnAlarmRule const& alarmRule)
{
    m_strName = strName;
    m_strSystem = strSystem;
    m_bCgi = true;
    m_alarmRule = alarmRule;
    m_unPort = 0;
}
///server模式的构造函数
CwmNerveServiceInfo::CwmNerveServiceInfo(string const& strName,
                                         string const& strSystem,
                                         CwmCmnAlarmRule const& alarmRule,
                                         string const& strServerIp,
                                         CWX_UINT16 unPort)
{
    m_strName = strName;
    m_strSystem = strSystem;
    m_bCgi = false;
    m_alarmRule = alarmRule;
    m_strServeIp = strServerIp;
    m_unPort = unPort;
}
///拷贝构造
CwmNerveServiceInfo::CwmNerveServiceInfo(CwmNerveServiceInfo const& obj)
{
    m_strName = obj.m_strName;
    m_strSystem = obj.m_strSystem;
    m_bCgi = obj.m_bCgi;
    m_alarmRule = obj.m_alarmRule;
    m_strServeIp = obj.m_strServeIp;
    m_unPort = obj.m_unPort;
    m_actions = obj.m_actions;
}
///赋值操作
CwmNerveServiceInfo& CwmNerveServiceInfo::operator=(CwmNerveServiceInfo const& obj)
{
    if (this != &obj)
    {
        m_strName = obj.m_strName;
        m_strSystem = obj.m_strSystem;
        m_bCgi = obj.m_bCgi;
        m_alarmRule = obj.m_alarmRule;
        m_strServeIp = obj.m_strServeIp;
        m_unPort = obj.m_unPort;
        m_actions = obj.m_actions;
    }
    return *this;
}

bool CwmNerveConfig::loadConfig(string const & strConfFile)
{
    CwxXmlFileConfigParser parser;
    char const* value;
    //解析配置文件
    if (false == parser.parse(strConfFile))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to Load conf file:%s.", strConfFile.c_str());
        return false;
    }
    //load workdir sm_nerve:common:workdir{value}
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:workdir", "value"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:workdir].");
        return false;
    }
    m_strWorkDir = value;
	if ('/' != m_strWorkDir[m_strWorkDir.length()-1]) m_strWorkDir +="/";

    //load data sm_nerve:common:data{path}
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:data", "path"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:data].");
        return false;
    }
    m_strDbFilePath = value;
    if ('/' != m_strDbFilePath[m_strDbFilePath.length() -1]) m_strDbFilePath +="/";
    //load query ip
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:query", "ip"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:query:ip].");
        return false;
    }
    m_queryListen.setHostName(value);
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:query", "port"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:query:port].");
        return false;
    }
    m_queryListen.setPort(strtoul(value, NULL, 0));
    //load mgr listen
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:mgr", "ip"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:mgr:ip].");
        return false;
    }
    m_mgrListen.setHostName(value);
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:mgr", "port"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:mgr:port].");
        return false;
    }
    m_mgrListen.setPort(strtoul(value, NULL, 0));
    //load alarm ip
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:alarm", "ip"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:alarm:ip].");
        return false;
    }
    m_alarmListen.setHostName(value);
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:alarm", "port"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:alarm:port].");
        return false;
    }
    m_alarmListen.setPort(strtoul(value, NULL, 0));
    //load timeout
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:timeout", "second"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:timeout:second].");
        return false;
    }
    m_uiTimeout = strtoul(value, NULL, 0);
    //load host name
    if ((NULL == (value=parser.getElementAttr("sm_nerve:common:host", "name"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:common:host:name].");
        return false;
    }
    m_strHostName = value;
    //get alarm rule
    CwxXmlTreeNode const* pNodeRoot = parser.getElementNode("sm_nerve:alarm_rules");
    if (!pNodeRoot || !pNodeRoot->m_pChildHead)
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:alarm_rules].");
        return false;
    }
    CwxXmlTreeNode const* node = pNodeRoot->m_pChildHead;
    CwmCmnAlarmRule rule;
    pair<char*, char*> key_item;
    while(node)
    {
        if (strcmp(node->m_szElement, "rule") == 0)
        {
           rule.reset();
           //get name
           if (CwxCommon::findKey(node->m_lsAttrs, "name",  key_item))
           {
               rule.setName(key_item.second);
               //get check interface
               if (CwxCommon::findKey(node->m_lsAttrs, "check_internal",  key_item))
               {
                   rule.setPollSecond(strtoul(key_item.second, NULL, 0));
               }
               //get clear_times
               if (CwxCommon::findKey(node->m_lsAttrs, "clear_times",  key_item))
               {
                   rule.setClearTime(strtoul(key_item.second, NULL, 0));
               }
               //get warning_times
               if (CwxCommon::findKey(node->m_lsAttrs, "warning_times",  key_item))
               {
                   rule.setWarningTime(strtoul(key_item.second, NULL, 0));
               }
               //get minor_times
               if (CwxCommon::findKey(node->m_lsAttrs, "minor_times",  key_item))
               {
                   rule.setMinorTime(strtoul(key_item.second, NULL, 0));
               }
               //get major_times
               if (CwxCommon::findKey(node->m_lsAttrs, "major_times",  key_item))
               {
                   rule.setMajorTime(strtoul(key_item.second, NULL, 0));
               }
               //get critical_times
               if (CwxCommon::findKey(node->m_lsAttrs, "critical_times",  key_item))
               {
                   rule.setCriticalTime(strtoul(key_item.second, NULL, 0));
               }
               //get fatal_times
               if (CwxCommon::findKey(node->m_lsAttrs, "fatal_times",  key_item))
               {
                   rule.setFatalTime(strtoul(key_item.second, NULL, 0));
               }
               this->addAlarmRule(rule);
           }
        }
        node = node->m_next;
    }
    
    //get service
    pNodeRoot = parser.getElementNode("sm_nerve:services");
    if (!pNodeRoot || !pNodeRoot->m_pChildHead)
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_nerve:services].");
        return false;
    }
    node = pNodeRoot->m_pChildHead;
    CwmNerveServiceInfo service;
    CwxXmlTreeNode const* action_node=NULL;
    CwxXmlTreeNode const* arg_node=NULL;
    CwmNerveActionInfo action;
    string strName;
    string strValue;
    while(node)
    {
        if (strcmp(node->m_szElement, "service") == 0)
        {
            service.reset();
            //get name
            if (!CwxCommon::findKey(node->m_lsAttrs, "name",  key_item))
            {
                node = node->m_next;
                continue;
            }
            service.setName(key_item.second);
            //get system
            if (!CwxCommon::findKey(node->m_lsAttrs, "system",  key_item))
            {
                node = node->m_next;
                continue;
            }
            service.setSystem(key_item.second);
            //get type
            if (!CwxCommon::findKey(node->m_lsAttrs, "type",  key_item))
            {
                node = node->m_next;
                continue;
            }
            service.setCgi(strcmp("cgi", key_item.second)==0?true:false);
            //get rule
            if (!CwxCommon::findKey(node->m_lsAttrs, "rule",  key_item))
            {
                key_item.second=CWM_CMN_DEFAULT;
            }
            //check rule
            if (m_alarmRules.find(key_item.second) == m_alarmRules.end())
            {
                CwxCommon::snprintf(m_szErrMsg, 511, "Can't find alarm rule:%s, service=%s", key_item.second, service.getName().c_str());
                return false;
            }
            service.setAlarmRule(m_alarmRules.find(key_item.second)->second);
            if (!service.isCgi())
            {
                //get ip
                if (!CwxCommon::findKey(node->m_lsAttrs, "ip", key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "Must set service's ip for server type, service=%s", service.getName().c_str());
                    return false;
                }
                service.setServerIp(key_item.second);
                //get port 
                if (!CwxCommon::findKey(node->m_lsAttrs, "port", key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "Must set service's port for server type, service=%s", service.getName().c_str());
                    return false;
                }
                service.setPort(strtoul(key_item.second, NULL, 0));
            }
            action_node = node->m_pChildHead;
            while(action_node)
            {
                action.reset();
                //set cgi
                action.setCgi(service.isCgi());
                //get action name
                if (!CwxCommon::findKey(action_node->m_lsAttrs, "name", key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "Must set service's action-name , service=%s", service.getName().c_str());
                    return false;
                }
                action.setName(key_item.second);
                //get action rule
                if (!CwxCommon::findKey(action_node->m_lsAttrs, "rule", key_item))
                {
                    key_item.second = (char*)service.getAlarmRule().getName().c_str();
                }
                //check rule
                if (m_alarmRules.find(key_item.second) == m_alarmRules.end())
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "Can't find alarm rule:%s, service=%s, action=%s", key_item.second, service.getName().c_str(), action.getName().c_str());
                    return false;
                }
                action.setAlarmRule(m_alarmRules.find(key_item.second)->second);
                //get action poll
                if (!CwxCommon::findKey(action_node->m_lsAttrs, "poll", key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "Must set service's action-poll , service=%s, action=%s", service.getName().c_str(), action.getName().c_str());
                    return false;
                }
                action.setPoll(strcmp("yes", key_item.second)==0?true:false);
                //get alarm type
                if (!CwxCommon::findKey(action_node->m_lsAttrs, "alarm_type", key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "Must set service's action-alarm_type , service=%s, action=%s", service.getName().c_str(), action.getName().c_str());
                    return false;
                }
                action.setAlarmType(key_item.second);
                if (service.isCgi())
                {
                    //get cmd
                    if (!CwxCommon::findKey(action_node->m_lsAttrs, "cmd", key_item))
                    {
                        CwxCommon::snprintf(m_szErrMsg, 511, "Must set service's action-cmd , service=%s, action=%s", service.getName().c_str(), action.getName().c_str());
                        return false;
                    }
                    action.setCgiCmd(key_item.second);
                }
                else
                {
                    //get msg_type
                    if (!CwxCommon::findKey(action_node->m_lsAttrs, "msg_type", key_item))
                    {
                        CwxCommon::snprintf(m_szErrMsg, 511, "Must set service's action-msg_type , service=%s, action=%s", service.getName().c_str(), action.getName().c_str());
                        return false;
                    }
                    action.setMsgType(strtoul(key_item.second, NULL, 0));
                    //fetch arg
                    arg_node = action_node->m_pChildHead;
                    while(arg_node)
                    {
                        if (strcmp("arg", arg_node->m_szElement) == 0)
                        {
                            //get name
                            if (!CwxCommon::findKey(arg_node->m_lsAttrs, "name", key_item))
                            {
                                CwxCommon::snprintf(m_szErrMsg, 511, "Must set service's action-arg's name , service=%s, action=%s", service.getName().c_str(), action.getName().c_str());
                                return false;
                            }
                            strName = key_item.second;
                            //get value
                            if (!CwxCommon::findKey(arg_node->m_lsAttrs, "value", key_item))
                            {
                                CwxCommon::snprintf(m_szErrMsg, 511, "Must set service's action-arg's value , service=%s, action=%s", service.getName().c_str(), action.getName().c_str());
                                return false;
                            }
                            strValue = key_item.second;
                            action.addArg(strName, strValue);
                        }
                        arg_node = arg_node->m_next;
                    }//end while(arg_node)
                }//end else{
                service.addAction(action);
                action_node = action_node->m_next;
            }//end while(action_node)
            this->addService(service);
        }  
        node = node->m_next;
    }
    return true;
}

void CwmNerveConfig::outputConfig()
{
    CWX_INFO(("*****************BEGIN CONFIG *******************"));
    CWX_INFO(("workdir= %s", m_strWorkDir.c_str()));
    CWX_INFO(("query: ip=%s, port=%d", m_queryListen.getHostName().c_str(), m_queryListen.getPort()));
    CWX_INFO(("mgr listen: ip=%s, port=%d",m_mgrListen.getHostName().c_str(), m_mgrListen.getPort()));
    CWX_INFO(("alarm: ip=%s, port=%d", m_alarmListen.getHostName().c_str(), m_alarmListen.getPort()));
    CWX_INFO(("timeout=%u", m_uiTimeout));
    CWX_INFO(("host=%s", m_strHostName.c_str()));
    CWX_INFO(("*****************ALARM RULE*******************"));
    {
        map<string, CwmCmnAlarmRule>::iterator iter=m_alarmRules.begin();
        while(iter != m_alarmRules.end())
        {
            CWX_INFO(("Alarm rule: name=%s check_internal=%u clear_time=%u warning_time=%u minor_time=%u major_time=%u critical_time=%u fatal_time=%u",
                iter->second.getName().c_str(),
                iter->second.getPollSecond(),
                iter->second.getClearTime(),
                iter->second.getWarningTime(),
                iter->second.getMinorTime(),
                iter->second.getMajorTime(),
                iter->second.getCriticalTime(),
                iter->second.getFatalTime()));
            iter++;
        }
    }
    CWX_INFO(("*****************SERVICE RULE*******************"));
    {
        map<pair<string,string>, CwmNerveServiceInfo>::iterator iter = m_services.begin();
        while(iter != m_services.end())
        {
            if (iter->second.isCgi())
            {
                CWX_INFO(("service: name=%s system=%s type=cgi rule=%s",
                        iter->second.getName().c_str(),
                        iter->second.getSystem().c_str(),
                        iter->second.getAlarmRule().getName().c_str()));
            }
            else
            {
                CWX_INFO(("service: name=%s system=%s type=server rule=%s ip=%s port=%u",
                    iter->second.getName().c_str(),
                    iter->second.getSystem().c_str(),
                    iter->second.getAlarmRule().getName().c_str(),
                    iter->second.getServerIp().c_str(),
                    iter->second.getPort()));
            }
            //get action
            list<pair<string, string> >::const_iterator arg_iter;
            list<CwmNerveActionInfo>::const_iterator act_iter = iter->second.getActions().begin();
            while(act_iter != iter->second.getActions().end())
            {
                if (iter->second.isCgi())
                {
                    CWX_INFO(("action: name=%s rule=%s poll=%s alarm_type=%s cmd=%s",
                        act_iter->getName().c_str(),
                        act_iter->getAlarmRule().getName().c_str(),
                        act_iter->isPoll()?"yes":"no",
                        act_iter->getAlarmType().c_str(),
                        act_iter->getCgiCmd().c_str()));
                }
                else
                {
                    CWX_INFO(("action: name=%s rule=%s poll=%s alarm_type=%s msg_type=%u",
                        act_iter->getName().c_str(),
                        act_iter->getAlarmRule().getName().c_str(),
                        act_iter->isPoll()?"yes":"no",
                        act_iter->getAlarmType().c_str(),
                        act_iter->getMsgType()));
                }
                if (!iter->second.isCgi())
                {//out arg
                    arg_iter = act_iter->getArg().begin();
                    while(arg_iter != act_iter->getArg().end())
                    {
                        CWX_INFO(("\t\t arg: name=%s value=%s",
                            arg_iter->first.c_str(), arg_iter->second.c_str()));
                        arg_iter++;
                    }
                   
                }
                act_iter++;
            }
            iter++;
        }
    }
    CWX_INFO(("*****************END   CONFIG *******************"));
}
