#include "CwxAppLogger.h"
#include "CwmCenterConfig.h"
#include "CwxFile.h"
list<string> const& CwmCenterAlarmMail::getAlarmEmails(CWX_UINT8 ucLevel) const
{
    switch (ucLevel)
    {
        case CwmCmnAlarm::ALARM_FATAL:
            return m_fatal;
        case CwmCmnAlarm::ALARM_CRITICAL:
            return m_critical;
        case CwmCmnAlarm::ALARM_MAJOR:
            return m_major;
        case CwmCmnAlarm::ALARM_MINOR:
            return m_minor;
    }
    return m_warning;
}

string& CwmCenterAlarmMail::getAlarmEmails(CWX_UINT8 ucLevel, string& strAlarms) const
{
    list<string> const* pAlarms = &m_warning;
    switch (ucLevel)
    {
        case CwmCmnAlarm::ALARM_FATAL:
            pAlarms = &m_fatal;
            break;
        case CwmCmnAlarm::ALARM_CRITICAL:
            pAlarms = &m_critical;
            break;
        case CwmCmnAlarm::ALARM_MAJOR:
            pAlarms = &m_major;
            break;
        case CwmCmnAlarm::ALARM_MINOR:
            pAlarms = &m_minor;
            break;
    }
    list<string>::const_iterator iter = pAlarms->begin();
    strAlarms = "";
    while(iter != pAlarms->end())
    {
        strAlarms += *iter;
        iter++;
        if (iter != pAlarms->end()) strAlarms +=";";
    }
    return strAlarms;
}

///根据当前告警level与前一级level，获取告警的mail
void CwmCenterAlarmMail::getAlarmEmails(CWX_UINT8 ucPLevel,
                                        CWX_UINT8 ucLevel,
                                        set<string>& emails) const
{
    CWX_UINT8 ucFetchLevel = 0;
    emails.clear();
    list<string>::const_iterator iter;
    if (CwmCmnAlarm::ALARM_CLEAR == ucLevel)
    {//clear
        ucFetchLevel = ucPLevel;
    }
    else
    {
        ucFetchLevel = ucLevel;
    }
    switch(ucFetchLevel)
    {
    case CwmCmnAlarm::ALARM_FATAL:
        iter = m_fatal.begin();
        while(iter != m_fatal.end())
        {
            emails.insert(*iter);
            iter++;
        }
    case CwmCmnAlarm::ALARM_CRITICAL:
        iter = m_critical.begin();
        while(iter != m_critical.end())
        {
            emails.insert(*iter);
            iter++;
        }
    case CwmCmnAlarm::ALARM_MAJOR:
         iter = m_major.begin();
         while(iter != m_major.end())
         {
             emails.insert(*iter);
             iter++;
         }
    case CwmCmnAlarm::ALARM_MINOR:
         iter = m_minor.begin();
         while(iter != m_minor.end())
         {
             emails.insert(*iter);
             iter++;
         }
    case CwmCmnAlarm::ALARM_WARNING:
         iter = m_warning.begin();
         while(iter != m_warning.end())
         {
             emails.insert(*iter);
             iter++;
         }
    }
    
}
///根据当前告警level与前一级level，获取告警的mail
string& CwmCenterAlarmMail::getAlarmEmails(CWX_UINT8 ucPLevel,
                                           CWX_UINT8 ucLevel,
                                           string& strEmails) const
{
    set<string> emails;
    getAlarmEmails(ucPLevel, ucLevel, emails);
    set<string>::iterator iter = emails.begin();
    strEmails = "";
    while(iter != emails.end())
    {
        strEmails += *iter;
        iter++;
    }
    return strEmails;
}


void CwmCenterAlarmMail::setAlarmEmails(CWX_UINT8 ucLevel,
                                        string const& strEmails)
{
    switch (ucLevel)
    {
    case CwmCmnAlarm::ALARM_FATAL:
        return setEmails(m_fatal, strEmails);
    case CwmCmnAlarm::ALARM_CRITICAL:
        return setEmails(m_critical, strEmails);
    case CwmCmnAlarm::ALARM_MAJOR:
        return setEmails(m_major, strEmails);
    case CwmCmnAlarm::ALARM_MINOR:
        return setEmails(m_minor, strEmails);
    }
    return setEmails(m_warning, strEmails);
}

void CwmCenterAlarmMail::appendAlarmEmails(CWX_UINT8 ucLevel, string const& strEmails)
{
    switch (ucLevel)
    {
    case CwmCmnAlarm::ALARM_FATAL:
        return appendEmails(m_fatal, strEmails);
    case CwmCmnAlarm::ALARM_CRITICAL:
        return appendEmails(m_critical, strEmails);
    case CwmCmnAlarm::ALARM_MAJOR:
        return appendEmails(m_major, strEmails);
    case CwmCmnAlarm::ALARM_MINOR:
        return appendEmails(m_minor, strEmails);
    }
    return appendEmails(m_warning, strEmails);
}



void CwmCenterAlarmMail::setEmails(list<string>& emails, string const& strEmails)
{
    emails.clear();
    set<string, CwxCaseStringLess> mailSet;
    CwxCommon::split(strEmails, emails, ';');
    list<string>::iterator iter=emails.begin();
    while(iter != emails.end())
    {
        CwxCommon::trim(*iter);
        if (iter->length() && (mailSet.find(*iter) == mailSet.end()))
        {//not empty and not exist
            mailSet.insert(*iter);
        }
        iter++;
    }
    emails.clear();
    set<string, CwxCaseStringLess>::iterator iter_set = mailSet.begin();
    while(iter_set != mailSet.end())
    {
        emails.push_back(*iter_set);
        iter_set++;
    }
}

void CwmCenterAlarmMail::appendEmails(list<string>& emails,
                                      string const& strEmails)
{
    set<string, CwxCaseStringLess> mailSet;
    list<string>::iterator iter=emails.begin();
    while(iter != emails.end())
    {
        mailSet.insert(*iter);
    }
    CwxCommon::split(strEmails, emails, ';');
    while(iter != emails.end())
    {
        CwxCommon::trim(*iter);
        if (iter->length() && (mailSet.find(*iter) == mailSet.end()))
        {//not empty and not exist
            mailSet.insert(*iter);
        }
        iter++;
    }
    emails.clear();
    set<string, CwxCaseStringLess>::iterator iter_set = mailSet.begin();
    while(iter_set != mailSet.end())
    {
        emails.push_back(*iter_set);
        iter_set++;
    }
}

///添加一个host
bool CwmCenterService::addHost(CwmCenterHost const* pHost)
{
    map<string/*ip*/, CwmCenterHost const*>::iterator iter = m_hosts.find(pHost->getIp());
    if (iter != m_hosts.end()) return false;
    m_hosts[pHost->getIp()] = pHost;
    return true;
}

bool CwmCenterConfig::loadConfig(string const & strConfFile)
{
    CwxXmlFileConfigParser parser;
    char const* value;
    string data;

    //解析配置文件
    if (false == parser.parse(strConfFile))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to Load conf file:%s.", strConfFile.c_str());
        return false;
    }
    //load workdir sm_center:common:workdir{value}
    if ((NULL == (value=parser.getElementAttr("sm_center:common:workdir", "value"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:workdir].");
        return false;
    }
    m_strWorkDir = value;
	if ('/' != m_strWorkDir[m_strWorkDir.length()-1]) m_strWorkDir +="/";

    //load data sm_center:common:data{path}
    if ((NULL == (value=parser.getElementAttr("sm_center:common:data", "path"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:data].");
        return false;
    }
    m_strDbFilePath = value;
    if ('/' != m_strDbFilePath[m_strDbFilePath.length() -1]) m_strDbFilePath +="/";
    //load query ip
    if ((NULL == (value=parser.getElementAttr("sm_center:common:query", "ip"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:query:ip].");
        return false;
    }
    m_queryListen.setHostName(value);
    if ((NULL == (value=parser.getElementAttr("sm_center:common:query", "port"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:query:port].");
        return false;
    }
    m_queryListen.setPort(strtoul(value, NULL, 0));
    //load mgr listen
    if ((NULL == (value=parser.getElementAttr("sm_center:common:mgr", "ip"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:mgr:ip].");
        return false;
    }
    m_mgrListen.setHostName(value);
    if ((NULL == (value=parser.getElementAttr("sm_center:common:mgr", "port"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:mgr:port].");
        return false;
    }
    m_mgrListen.setPort(strtoul(value, NULL, 0));
    //load alarm accept ip
    if ((NULL == (value=parser.getElementAttr("sm_center:common:alarm:accept", "ip"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:alarm:accept:ip].");
        return false;
    }
    m_alarmAccept.setHostName(value);
    if ((NULL == (value=parser.getElementAttr("sm_center:common:alarm:accept", "port"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:alarm:accept:port].");
        return false;
    }
    m_alarmAccept.setPort(strtoul(value, NULL, 0));
    //load alarm report ip
    if ((NULL == (value=parser.getElementAttr("sm_center:common:alarm:report", "ip"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:alarm:report:ip].");
        return false;
    }
    m_alarmReport.setHostName(value);
    if ((NULL == (value=parser.getElementAttr("sm_center:common:alarm:report", "port"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:alarm:report:port].");
        return false;
    }
    m_alarmReport.setPort(strtoul(value, NULL, 0));

    m_bAvgReport = false;
    if (NULL != (value=parser.getElementAttr("sm_center:common:alarm:report", "mode")))
    {
        if (strcasecmp(value, "avg") == 0)
        {
            m_bAvgReport = true;
        }
    }
    //load work thread num
    if ((NULL == (value=parser.getElementAttr("sm_center:common:work_thread", "num"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:work_thread:num].");
        return false;
    }
    m_uiThreadNum = atoi(value);
    //load query timeout
    if (!parser.getElementData("sm_center:common:timeout:query", data) || !data.length())
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:timeout:query].");
        return false;
    }
    m_uiQueryTimeout = strtoul(data.c_str(), NULL, 0);
    //load alarm timeout
    if (!parser.getElementData("sm_center:common:timeout:alarm", data) || !data.length())
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:timeout:alarm].");
        return false;
    }
    m_uiAlarmTimeout = strtoul(data.c_str(), NULL, 0);

    //load web 
    if ((NULL == (value=parser.getElementAttr("sm_center:common:web", "url"))) || !value[0])
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:common:web:url].");
        return false;
    }
    m_strWebUrl = value;
    CwxXmlTreeNode const* pNodeRoot = NULL;
    CwxXmlTreeNode const* node = NULL;
    //load template
    {
        CwmCenterTemplate* pTemplate = NULL;
        pNodeRoot = parser.getElementNode("sm_center:templates");
        if (!pNodeRoot || !pNodeRoot->m_pChildHead)
        {
            CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:templates].");
            return false;
        }
        node = pNodeRoot->m_pChildHead;
        while(node)
        {
            if (strcmp(node->m_szElement, "template") == 0)
            {
                pTemplate = new CwmCenterTemplate();
                if (!loadTemplate(node, *pTemplate))
                {
                    delete pTemplate;
                    return false;
                }
                m_templates[pTemplate->getName()] = pTemplate;
            }
            node = node->m_next;
        }
    }
    //get alarm rule
    {
        CwmCmnAlarmRule* pRule = NULL;
        pNodeRoot = parser.getElementNode("sm_center:rules:alarm");
        if (!pNodeRoot)
        {
            CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:rules:alarm].");
            return false;
        }
        node = pNodeRoot->m_pChildHead;
        while(node)
        {
            if (strcmp(node->m_szElement, "rule") == 0)
            {
                pRule = new CwmCmnAlarmRule();
                if (!loadAlarmRule(node, *pRule))
                {
                    delete pRule;
                    return false;
                }
                m_alarmRules[pRule->getName()] = pRule;
            }
            node = node->m_next;
        }
    }
    //get alarm mail
    {
        CwmCenterAlarmMail* pMailRule = NULL;
        pNodeRoot = parser.getElementNode("sm_center:rules:mail");
        if (!pNodeRoot)
        {
            CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:rules:mail].");
            return false;
        }
        node = pNodeRoot->m_pChildHead;
        while(node)
        {
            if (strcmp(node->m_szElement, "mail") == 0)
            {
                pMailRule = new CwmCenterAlarmMail();
                if (!loadAlarmMail(node, *pMailRule)){
                    delete pMailRule;
                    return false;
                }
                m_alarmMails[pMailRule->getName()] = pMailRule;
            }
            node = node->m_next;
        }
    }
    //get host
    {
        pair<char*, char*> key_item;
        CwmCenterHost* pHost = NULL;
        pNodeRoot = parser.getElementNode("sm_center:hosts");
        if (!pNodeRoot || !pNodeRoot->m_pChildHead)
        {
            CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:hosts].");
            return false;
        }
        node = pNodeRoot->m_pChildHead;
        while(node)
        {
            if (strcmp(node->m_szElement, "host") == 0)
            {
                pHost = new CwmCenterHost();
                //get ip;
                if (!CwxCommon::findKey(node->m_lsAttrs, "ip",  key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "host hasn't ip.");
                    delete pHost;
                    return false;
                }
                pHost->setIp(key_item.second);
                //get name
                if (!CwxCommon::findKey(node->m_lsAttrs, "name",  key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "host hasn't name.");
                    delete pHost;
                    return false;
                }
                pHost->setName(key_item.second);
                //get port
                if (!CwxCommon::findKey(node->m_lsAttrs, "port",  key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "host hasn't port.");
                    delete pHost;
                    return false;
                }
                pHost->setPort(atoi(key_item.second));
                //alarm
                if (!CwxCommon::findKey(node->m_lsAttrs, "alarm",  key_item))
                {
                    key_item.second = CWM_CMN_DEFAULT;
                }
                if (!getAlarmRule(key_item.second))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "Host[%s]'s alarm rule[%s] doesn't exist.", pHost->getName().c_str(), key_item.second);
                    delete pHost;
                    return false;
                }
                pHost->setAlarmRule(getAlarmRule(key_item.second));
                //mail rule
                if (!CwxCommon::findKey(node->m_lsAttrs, "mail",  key_item))
                {
                    key_item.second = CWM_CMN_DEFAULT;
                }
                if (!getAlarmMail(key_item.second))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "Host[%s]'s mail rule[%s] doesn't exist.", pHost->getName().c_str(), key_item.second);
                    delete pHost;
                    return false;
                }
                pHost->setMailRule(getAlarmMail(key_item.second));
                m_hosts[pHost->getIp()] = pHost;
            }
            node = node->m_next;
        }
    }

    //get system
    {
        pair<char*, char*> key_item;
        string strName;
        string strFile;
        CwmCenterSystem* pSystem = NULL;
        pNodeRoot = parser.getElementNode("sm_center:systems");
        if (!pNodeRoot || !pNodeRoot->m_pChildHead)
        {
            CwxCommon::snprintf(m_szErrMsg, 511, "Must set [sm_center:systems].");
            return false;
        }
        node = pNodeRoot->m_pChildHead;
        while(node)
        {
            if (strcmp(node->m_szElement, "system") == 0)
            {
                //get name;
                if (!CwxCommon::findKey(node->m_lsAttrs, "name",  key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "system hasn't name.");
                    return false;
                }
                strName = key_item.second;
                //get file
                if (!CwxCommon::findKey(node->m_lsAttrs, "config_file",  key_item))
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "system hasn't config_file.");
                    return false;
                }
                strFile = key_item.second;
                //get system
                pSystem = new CwmCenterSystem();
                if (!loadSystem(strName, strFile, *pSystem))
                {
                    delete pSystem;
                    return false;
                }
                m_systems[pSystem->getName()] = pSystem;
            }
            node = node->m_next;
        }
    }
    return true;
}

bool CwmCenterConfig::loadTemplate(CwxXmlTreeNode const* node, CwmCenterTemplate& tmpt)
{
    pair<char*, char*> key_item;
    CWX_ASSERT(node);
    //get name
    if (!CwxCommon::findKey(node->m_lsAttrs, "name",  key_item))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "tempalte hasn't name.");
        return false;
    }
    tmpt.setName(key_item.second);
    //get file
    if (!CwxCommon::findKey(node->m_lsAttrs, "file",  key_item))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "tempalte hasn't file.");
        return false;
    }
    tmpt.setFileName(key_item.second);
    //get type
    if (!CwxCommon::findKey(node->m_lsAttrs, "type",  key_item))
    {
        tmpt.setType("text/html");
    }
    else
    {
        tmpt.setType(key_item.second);
    }
    //load file
    string strFileData;
    if (!CwxFile::readTxtFile(tmpt.getFileName(), strFileData))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to load tempalte file:%s", tmpt.getFileName().c_str());
        return false;
    }
    CwxHtmlTmpl * pHtml = (CwxHtmlTmpl*)tmpt.getParser();
    if (!pHtml->parse(strFileData.c_str(), strFileData.length()))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to paser tempalte file:%s,err=%s", tmpt.getFileName().c_str(), tmpt.getParser()->getErr().c_str());
        return false;
    }
    return true;
}

bool CwmCenterConfig::loadAlarmRule(CwxXmlTreeNode const* node, CwmCmnAlarmRule& alarmRule)
{
    pair<char*, char*> key_item;
    CWX_ASSERT(node);
    alarmRule.reset();
    //get name
    if (!CwxCommon::findKey(node->m_lsAttrs, "name",  key_item))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Alarm rule's hasn't name.");
        return false;
    }
    alarmRule.setName(key_item.second);
    //get check interface
    if (CwxCommon::findKey(node->m_lsAttrs, "check_internal",  key_item))
    {
        alarmRule.setPollSecond(strtoul(key_item.second, NULL, 0));
    }
    //get clear_times
    if (CwxCommon::findKey(node->m_lsAttrs, "clear_times",  key_item))
    {
        alarmRule.setClearTime(strtoul(key_item.second, NULL, 0));
    }
    //get warning_times
    if (CwxCommon::findKey(node->m_lsAttrs, "warning_times",  key_item))
    {
        alarmRule.setWarningTime(strtoul(key_item.second, NULL, 0));
    }
    //get minor_times
    if (CwxCommon::findKey(node->m_lsAttrs, "minor_times",  key_item))
    {
        alarmRule.setMinorTime(strtoul(key_item.second, NULL, 0));
    }
    //get major_times
    if (CwxCommon::findKey(node->m_lsAttrs, "major_times",  key_item))
    {
        alarmRule.setMajorTime(strtoul(key_item.second, NULL, 0));
    }
    //get critical_times
    if (CwxCommon::findKey(node->m_lsAttrs, "critical_times",  key_item))
    {
        alarmRule.setCriticalTime(strtoul(key_item.second, NULL, 0));
    }
    //get fatal_times
    if (CwxCommon::findKey(node->m_lsAttrs, "fatal_times",  key_item))
    {
        alarmRule.setFatalTime(strtoul(key_item.second, NULL, 0));
    }
    return true;
}

bool CwmCenterConfig::loadAlarmMail(CwxXmlTreeNode const* node, CwmCenterAlarmMail& alarmMail)
{
    pair<char*, char*> key_item;
    CWX_ASSERT(node);
    alarmMail.reset();
    //get name
    if (!CwxCommon::findKey(node->m_lsAttrs, "name",  key_item))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Alarm mail's hasn't name.");
        return false;
    }
    alarmMail.setName(key_item.second);
    //get warning
    if (!CwxCommon::findKey(node->m_lsAttrs, "warning",  key_item))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Alarm mail's hasn't warning mail.");
        return false;
    }
    alarmMail.setAlarmEmails(CwmCmnAlarm::ALARM_WARNING, key_item.second);
    //get minor
    if (!CwxCommon::findKey(node->m_lsAttrs, "minor",  key_item))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Alarm mail's hasn't minor mail.");
        return false;
    }
    alarmMail.setAlarmEmails(CwmCmnAlarm::ALARM_MINOR, key_item.second);
    //get major
    if (!CwxCommon::findKey(node->m_lsAttrs, "major",  key_item))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Alarm mail's hasn't major mail.");
        return false;
    }
    alarmMail.setAlarmEmails(CwmCmnAlarm::ALARM_MAJOR, key_item.second);
    //get critical
    if (!CwxCommon::findKey(node->m_lsAttrs, "critical",  key_item))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Alarm mail's hasn't critical mail.");
        return false;
    }
    alarmMail.setAlarmEmails(CwmCmnAlarm::ALARM_CRITICAL, key_item.second);
    //get fatal
    if (!CwxCommon::findKey(node->m_lsAttrs, "fatal",  key_item))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Alarm mail's hasn't fatal mail.");
        return false;
    }
    alarmMail.setAlarmEmails(CwmCmnAlarm::ALARM_FATAL, key_item.second);
    return true;
}

bool CwmCenterConfig::loadSystem(string const& strName, string const& strFile, CwmCenterSystem& system)
{
    CwxXmlFileConfigParser parser;
    char const* value;
    string data;
    //解析配置文件
    if (false == parser.parse(strFile))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to Load system conf file, system=%s, file=%s.", strName.c_str(), strFile.c_str());
        return false;
    }
    system.setName(strName);
    system.setFileName(strFile);
    //get desc key
    if ((value=parser.getElementAttr("system", "desc")))
    {
        system.setDesc(value);
    }
    CwxXmlTreeNode const* pNodeRoot = NULL;
    CwxXmlTreeNode const* node = NULL;
    //get alarm rule
    {
        CwmCmnAlarmRule* pRule = NULL;
        pNodeRoot = parser.getElementNode("system:rules:alarm");
        if (!pNodeRoot)
        {
            CwxCommon::snprintf(m_szErrMsg, 511, "Must set [system:rules:alarm], system=%s, file=%s.", strName.c_str(), strFile.c_str());
            return false;
        }
        node = pNodeRoot->m_pChildHead;
        while(node)
        {
            if (strcmp(node->m_szElement, "rule") == 0)
            {
                pRule = new CwmCmnAlarmRule();
                if (!loadAlarmRule(node, *pRule))
                {
                    string strErr = m_szErrMsg;
                    CwxCommon::snprintf(m_szErrMsg, 511, "Failure to load alarm rule, system[%s], file[%s],err=%s", strName.c_str(), strFile.c_str(), strErr.c_str());
                    delete pRule;
                    return false;
                }
                system.addAlarmRule(pRule);
            }
            node = node->m_next;
        }
    }
    //get alarm mail
    {
        CwmCenterAlarmMail* pMailRule = NULL;
        pNodeRoot = parser.getElementNode("system:rules:mail");
        if (!pNodeRoot)
        {
            CwxCommon::snprintf(m_szErrMsg, 511, "Must set [system:rules:mail].system=%s, file=%s.", strName.c_str(),  strFile.c_str());
            return false;
        }
        node = pNodeRoot->m_pChildHead;
        while(node)
        {
            if (strcmp(node->m_szElement, "mail") == 0)
            {
                pMailRule = new CwmCenterAlarmMail();
                if (!loadAlarmMail(node, *pMailRule))
                {
                    string strErr = m_szErrMsg;
                    CwxCommon::snprintf(m_szErrMsg, 511, "Failure to load mail rule, system[%s], file[%s],err=%s", strName.c_str(), strFile.c_str(), strErr.c_str());
                    delete pMailRule;
                    return false;
                }
                system.addAlarmMail(pMailRule);
            }
            node = node->m_next;
        }
    }
    //load service
    pNodeRoot = parser.getElementNode("system:services");
    if (!pNodeRoot)
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Must set [system:services].system=%s, file=%s.", strName.c_str(), strFile.c_str());
        return false;
    }
    node = pNodeRoot->m_pChildHead;

    CwmCenterService* pService = NULL;
    pair<char*, char*> key_item;
    while(node)
    {
        if (strcmp("service", node->m_szElement) == 0)
        {
            pService = new CwmCenterService();
            //get name
            if (!CwxCommon::findKey(node->m_lsAttrs, "name",  key_item))
            {
                CwxCommon::snprintf(m_szErrMsg, 511, "System's service hasn't name, system[%s], file=%s", strName.c_str(), strFile.c_str());
                delete pService;
                return false;
            }
            pService->setName(key_item.second);
            //get mail
            if (!CwxCommon::findKey(node->m_lsAttrs, "mail",  key_item))
            {
                key_item.second = CWM_CMN_DEFAULT;
            }
            if (system.getAlarmMail(key_item.second))
            {
                pService->setMailRule(system.getAlarmMail(key_item.second));
            }
            else
            {
                if (getAlarmMail(key_item.second))
                {
                    pService->setMailRule(getAlarmMail(key_item.second));
                }
                else
                {
                    CwxCommon::snprintf(m_szErrMsg, 511, "Service's mail rule doesn't exist, mail[%s], system[%s], file=%s", key_item.second, strName.c_str(), strFile.c_str());
                    delete pService;
                    return false;
                }
            }
            //get action and host
            CwxXmlTreeNode const* pNode_service = NULL;
            CwxXmlTreeNode const* pNode_child = NULL;
            CwmCenterHost * pHost = NULL;
            pNode_service = node->m_pChildHead;
            while(pNode_service)
            {
                if (strcmp("actions", pNode_service->m_szElement)==0)
                {
                    pNode_child = pNode_service->m_pChildHead;
                    while(pNode_child)
                    {
                        if (strcmp("action", pNode_child->m_szElement) == 0)
                        {
                            //get name
                            if (!CwxCommon::findKey(pNode_child->m_lsAttrs, "name",  key_item))
                            {
                                CwxCommon::snprintf(m_szErrMsg, 511, "Service's action hasn't name, system[%s], service[%s] file=%s", strName.c_str(), pService->getName().c_str(), strFile.c_str());
                                delete pService;
                                return false;
                            }
                            pService->addAction(key_item.second);
                        }
                        pNode_child = pNode_child->m_next;
                    }
                }
                else if (strcmp("hosts", pNode_service->m_szElement)==0)
                {
                    pNode_child = pNode_service->m_pChildHead;
                    while(pNode_child)
                    {
                        if (strcmp("host", pNode_child->m_szElement) == 0)
                        {
                            //get ip
                            if (!CwxCommon::findKey(pNode_child->m_lsAttrs, "ip",  key_item))
                            {
                                CwxCommon::snprintf(m_szErrMsg, 511, "Service's host hasn't ip, system[%s], service[%s] file=%s", strName.c_str(), pService->getName().c_str(), strFile.c_str());
                                delete pService;
                                return false;
                            }
                            pHost = getHost(key_item.second);
                            if (!pHost)
                            {
                                CwxCommon::snprintf(m_szErrMsg, 511, "Service's host doesn't exist, system[%s], service[%s] ip[%s] file=%s", strName.c_str(), pService->getName().c_str(), key_item.second, strFile.c_str());
                                delete pService;
                                return false;
                            }
                            pHost->addService(strName, pService->getName(), pService);
                            pService->addHost(pHost);
                        }
                        pNode_child = pNode_child->m_next;
                    }

                }
                pNode_service = pNode_service->m_next;
            }//end while(pNode_service)
            system.addService(pService);
        }//end if (strcmp("service", node->m_szElement) == 0)
        node = node->m_next;
    }//end while(node)
    return true;
}

void CwmCenterConfig::outputConfig()
{
    CWX_INFO(("*****************BEGIN CONFIG *******************"));
    CWX_INFO(("workdir= %s", m_strWorkDir.c_str()));
    CWX_INFO(("data_path=%s",m_strDbFilePath.c_str()));
    CWX_INFO(("query: ip=%s, port=%d",m_queryListen.getHostName().c_str(), m_queryListen.getPort()));
    CWX_INFO(("mgr listen: ip=%s, port=%d",m_mgrListen.getHostName().c_str(), m_mgrListen.getPort()));
    CWX_INFO(("alarm_accept: ip=%s, port=%d", m_alarmAccept.getHostName().c_str(), m_alarmAccept.getPort()));
    CWX_INFO(("alarm_report: ip=%s, port=%d", m_alarmReport.getHostName().c_str(), m_alarmReport.getPort()));
    CWX_INFO(("work_thread=%u", m_uiThreadNum));
    CWX_INFO(("query_timeout=%u", m_uiQueryTimeout));
    CWX_INFO(("alarm_timeout=%u", m_uiAlarmTimeout));
    CWX_INFO(("web_url=%s",m_strWebUrl.c_str()));
    CWX_INFO(("*****************template*******************"));
    {
        map<string/*name*/, CwmCenterTemplate*>::iterator iter=m_templates.begin();
        while(iter != m_templates.end())
        {
            CWX_INFO(("Template: name=%s, type=%s, file=%s",
                iter->second->getName().c_str(),
                iter->second->getType().c_str(),
                iter->second->getFileName().c_str()));
            iter++;
        }
    }
    CWX_INFO(("*****************ALARM RULE*******************"));
    {
        map<string/*name*/, CwmCmnAlarmRule*>::iterator iter=m_alarmRules.begin();
        while(iter != m_alarmRules.end())
        {
            CWX_INFO(("Alarm rule: name=%s check_internal=%u clear_time=%u warning_time=%u minor_time=%u major_time=%u critical_time=%u fatal_time=%u",
                iter->second->getName().c_str(),
                iter->second->getPollSecond(),
                iter->second->getClearTime(),
                iter->second->getWarningTime(),
                iter->second->getMinorTime(),
                iter->second->getMajorTime(),
                iter->second->getCriticalTime(),
                iter->second->getFatalTime()));
            iter++;
        }
    }
    CWX_INFO(("*****************MAIL RULE*******************"));
    {
        string strEmails;
        map<string/*name*/, CwmCenterAlarmMail*>::iterator iter = m_alarmMails.begin();
        while(iter != m_alarmMails.end())
        {
            CWX_INFO(("Mail.rule: name=%s warning=%s minor=%s major=%s critical=%s fatal=%s",
                iter->second->getName().c_str(),
                iter->second->getAlarmEmails(CwmCmnAlarm::ALARM_WARNING, strEmails).c_str(),
                iter->second->getAlarmEmails(CwmCmnAlarm::ALARM_MINOR, strEmails).c_str(),
                iter->second->getAlarmEmails(CwmCmnAlarm::ALARM_MAJOR, strEmails).c_str(),
                iter->second->getAlarmEmails(CwmCmnAlarm::ALARM_CRITICAL, strEmails).c_str(),
                iter->second->getAlarmEmails(CwmCmnAlarm::ALARM_FATAL, strEmails).c_str()));
            iter++;
        }
    }
    CWX_INFO(("*****************HOST INFO*******************"));
    {
        map<string/*ip*/, CwmCenterHost*>::iterator iter=m_hosts.begin();
        while(iter != m_hosts.end())
        {
            CWX_INFO(("host: name=%s ip=%s port=%u alarm=%s email=%s",
                iter->second->getName().c_str(),
                iter->second->getIp().c_str(),
                iter->second->getPort(),
                iter->second->getAlarmRule()->getName().c_str(),
                iter->second->getMailRule()->getName().c_str()));
            iter++;
        }
    }
    CWX_INFO(("*****************SYSTEM INFO*******************"));
    {
        map<string/*system name*/, CwmCenterSystem*>::iterator iter = m_systems.begin(); ///<系统配置
        while(iter != m_systems.end())
        {
            CWX_INFO(("\t\t************system[%s], file=%s*****", iter->second->getName().c_str(), iter->second->getFileName().c_str()));
            CWX_INFO(("\t\t*****************Alarm rule*******************"));
            {
                map<string/*name*/, CwmCmnAlarmRule*>::const_iterator iter_alarm = iter->second->getAlarmRules().begin();
                while(iter_alarm != iter->second->getAlarmRules().end())
                {
                    CWX_INFO(("Alarm rule: name=%s check_internal=%u clear_time=%u warning_time=%u minor_time=%u major_time=%u critical_time=%u fatal_time=%u",
                        iter_alarm->second->getName().c_str(),
                        iter_alarm->second->getPollSecond(),
                        iter_alarm->second->getClearTime(),
                        iter_alarm->second->getWarningTime(),
                        iter_alarm->second->getMinorTime(),
                        iter_alarm->second->getMajorTime(),
                        iter_alarm->second->getCriticalTime(),
                        iter_alarm->second->getFatalTime()));
                    iter_alarm++;
                }
            }
            CWX_INFO(("\t\t*****************Mail rule*******************"));
            {
                string strEmails;
                map<string/*name*/, CwmCenterAlarmMail*>::const_iterator iter_mail = iter->second->getAlarmMails().begin();
                while(iter_mail != iter->second->getAlarmMails().end())
                {
                    CWX_INFO(("Mail.rule: name=%s warning=%s minor=%s major=%s critical=%s fatal=%s",
                        iter_mail->second->getName().c_str(),
                        iter_mail->second->getAlarmEmails(CwmCmnAlarm::ALARM_WARNING, strEmails).c_str(),
                        iter_mail->second->getAlarmEmails(CwmCmnAlarm::ALARM_MINOR, strEmails).c_str(),
                        iter_mail->second->getAlarmEmails(CwmCmnAlarm::ALARM_MAJOR, strEmails).c_str(),
                        iter_mail->second->getAlarmEmails(CwmCmnAlarm::ALARM_CRITICAL, strEmails).c_str(),
                        iter_mail->second->getAlarmEmails(CwmCmnAlarm::ALARM_FATAL, strEmails).c_str()));
                    iter_mail++;
                }

            }
            CWX_INFO(("\t\t*****************Servivces*******************"));
            {
                map<string/*service name*/, CwmCenterService*>::const_iterator iter_svr = iter->second->getServices().begin();
                while(iter_svr != iter->second->getServices().end())
                {
                    CWX_INFO(("\t\t*****************Service[%s], mail=%s*******************", iter_svr->second->getName().c_str(), iter_svr->second->getMailRule()->getName().c_str()));
                    CWX_INFO(("\t\t**********action**************"));
                    {
                        list<string>::const_iterator iter_action = iter_svr->second->getActions().begin();
                        while(iter_action != iter_svr->second->getActions().end())
                        {
                            CWX_INFO(("\t\t action:%s", iter_action->c_str()));
                            iter_action++;
                        }
                    }
                    CWX_INFO(("\t\t**********host**************"));
                    {
                        map<string/*ip*/, CwmCenterHost const*>::const_iterator iter_host = iter_svr->second->getHosts().begin();
                        while(iter_host != iter_svr->second->getHosts().end())
                        {
                            CWX_INFO(("\t\t host name=%s, ip=%s", iter_host->second->getName().c_str(), iter_host->second->getIp().c_str()));
                            iter_host++;
                        }
                    }
                    iter_svr++;
                }
            }

            iter++;
        }

    }
    CWX_INFO(("*****************END   CONFIG *******************"));

}


void CwmCenterConfig::reset()
{
    m_strWorkDir.erase();
    m_queryListen.reset();
    m_mgrListen.reset();
    m_alarmAccept.reset();
    m_alarmReport.reset();
    m_uiThreadNum = DEF_THREAD_NUM;
    m_uiQueryTimeout = DEF_TIMEOUT_SECOND;
    m_uiAlarmTimeout = DEF_TIMEOUT_SECOND;
    m_strDbFilePath.erase();
    {
        map<string, CwmCenterTemplate*>::iterator iter=m_templates.begin();
        while(iter != m_templates.end())
        {
            delete iter->second;
            iter++;
        }
        m_templates.clear();
    }
    {
        map<string, CwmCmnAlarmRule*>::iterator iter = m_alarmRules.begin();
        while(iter != m_alarmRules.end())
        {
            delete iter->second;
            iter++;
        }
        m_alarmRules.clear();
    }
    {
        map<string, CwmCenterSystem*>::iterator iter = m_systems.begin();
        while(iter != m_systems.end())
        {
            delete iter->second;
            iter++;
        }
        m_systems.clear();
    }
    {
        map<string/*ip*/, CwmCenterHost*>::iterator iter = m_hosts.begin();
        while(iter != m_hosts.end())
        {
            delete iter->second;
            iter++;
        }
        m_hosts.clear();
    }
    memset(m_szErrMsg, 0x00, 512);
}
