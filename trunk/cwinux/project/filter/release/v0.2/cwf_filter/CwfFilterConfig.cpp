#include "CwfFilterConfig.h"
#include "CwfFilterApp.h"

int CwfFilterConfig::loadConfig(string const & strConfFile)
{
    CwxXmlFileConfigParser parser;
    char const* pValue;
    string value;
    //½âÎöÅäÖÃÎÄ¼þ
    if (false == parser.parse(strConfFile))
    {
        snprintf(m_szError, 2047, "Failure to Load conf file:%s.", strConfFile.c_str());
        return -1;
    }
    //load workdir filter_svr:workdir:path
    if ((NULL == (pValue=parser.getElementAttr("filter_svr:workdir", "path"))) || !pValue[0])
    {
        snprintf(m_szError, 2047, "Must set [filter_svr:workdir:path].");
        return -1;
    }
    value = pValue;
	if ('/' != value[value.length()-1]) value +="/";
    m_strWorkDir = value;
    //load charset filter_svr:charset:encode
    if ((NULL == (pValue=parser.getElementAttr("filter_svr:charset", "encode"))) || !pValue[0])
    {
        m_bUtf8 = true;
    }
    else
    {
        if (strncasecmp("utf-8", pValue, strlen("utf-8")) == 0){
            m_bUtf8 = true;
        }
        else
        {
            m_bUtf8 = false;
        }
    }
    //load charset case-sensive filter_svr:charset:case_sensive
    if ((NULL == (pValue=parser.getElementAttr("filter_svr:charset", "case_sensive"))) || !pValue[0])
    {
        m_bCaseSensive = true;
    }
    else
    {
        if (strncasecmp("true", pValue, strlen("true")) == 0)
        {
            m_bCaseSensive = true;
        }
        else
        {
            m_bCaseSensive = false;
        }
    }
    //load filter_svr:listen:unix{file}
    if ((NULL == (pValue=parser.getElementAttr("filter_svr:listen:unix", "file"))) || !pValue[0])
    {
        m_strUnixPathFile.erase();
    }
    else
    {
        m_strUnixPathFile = pValue;
    }
    //load listen
    pValue = parser.getElementAttr("filter_svr:listen:tcp", "ip");
    if (pValue && pValue[0])
    {
        m_filterListen.setHostName(pValue);
        if ((NULL == (pValue=parser.getElementAttr("filter_svr:listen:tcp", "port"))) || !pValue[0])
        {
            snprintf(m_szError, 2047, "Must set [filter_svr:listen:tcp:port].");
            return -1;
        }
        m_filterListen.setPort(strtoul(pValue, NULL, 0));
    }
    else
    {
        if (m_strUnixPathFile.empty())
        {
            snprintf(m_szError, 2047, "Must set either tcp or unix domain filter-listen.");
            return -1;
        }
    }
    //load mgr listen
    pValue=parser.getElementAttr("filter_svr:mgr", "ip");
    if (pValue  && pValue[0])
    {
        m_mgrListen.setHostName(pValue);
        if ((NULL == (pValue=parser.getElementAttr("filter_svr:mgr", "port"))) || !pValue[0])
        {
            snprintf(m_szError, 2047, "Must set [filter_svr:mgr:port].");
            return -1;
        }
        m_mgrListen.setPort(strtoul(pValue, NULL, 0));
    }

    // load query thread num
    if ((NULL == (pValue=parser.getElementAttr("filter_svr:thread", "num"))) || !pValue[0])
    {
        snprintf(m_szError, 2047, "Must set [filter_svr:thread].");
        return -1;
    }
    m_unThreadNum = strtoul(pValue, NULL, 0);

    // load filter
    //get alarm rule
    CwxXmlTreeNode const* pNodeRoot = parser.getElementNode("filter_svr:filters");
    if (!pNodeRoot || !pNodeRoot->m_pChildHead)
    {
        CwxCommon::snprintf(m_szError, 2047, "Must set [filter_svr:filters].");
        return -1;
    }
    CwxXmlTreeNode const* node = pNodeRoot->m_pChildHead;
    CwxXmlTreeNode const* node_level = NULL;
    CwfFilterConfigFilter* pFilter = NULL;
    pair<char*, char*> key_item;
    CWX_UINT8 ucLevel=0;
    while(node)
    {
        if ((strcmp(node->m_szElement, "filter") == 0) && node->m_pChildHead)
        {
            pFilter = new CwfFilterConfigFilter();
            m_fiters.push_back(pFilter);
            //fetch id
            if (!CwxCommon::findKey(node->m_lsAttrs, "id",  key_item))
            {
                CwxCommon::snprintf(m_szError, 2047, "Must set filter's id");
                return -1;
            }
            pFilter->m_uiId = strtoul(key_item.second, NULL, 10);
            //fetch name
            if (!CwxCommon::findKey(node->m_lsAttrs, "name",  key_item))
            {
                CwxCommon::snprintf(m_szError, 2047, "Must set filter's name");
                return -1;
            }
            pFilter->m_strName = key_item.second;
            //fetch stopword
            if (!CwxCommon::findKey(node->m_lsAttrs, "stop_word_file",  key_item))
            {
                CwxCommon::snprintf(m_szError, 2047, "Must set filter's stop-word-file");
                return -1;
            }
            pFilter->m_strStopwordFile = key_item.second;
            //fetch filter-level;
            node_level = node->m_pChildHead;
            while(node_level)
            {
                if (strcmp(node_level->m_szElement, "filter_level") == 0)
                {
                    //fetch level
                    if (!CwxCommon::findKey(node_level->m_lsAttrs, "level",  key_item))
                    {
                        CwxCommon::snprintf(m_szError, 2047, "Must set the filter file's filter-level");
                        return -1;
                    }
                    ucLevel = strtoul(key_item.second, NULL, 10);
                    //fetch file
                    if (!CwxCommon::findKey(node_level->m_lsAttrs, "file", key_item))
                    {
                        CwxCommon::snprintf(m_szError, 2047, "Must set the filter file");
                        return -1;
                    }
                    pFilter->m_levelFiles[ucLevel] = key_item.second;
                }
                node_level = node_level->m_next;
            }//end while (node_level)
        }//end if ((strcmp(node->m_szElement, "filter") == 0) && node->m_pChildHead)
        node = node->m_next;
    }//end while(node)
    return 0;
}

void CwfFilterConfig::outputConfig()
{
    CWX_INFO(("*****************BEGIN CONFIG *******************"));
    CWX_INFO(("workdir=%s", m_strWorkDir.c_str()));
    CWX_INFO(("query listen unix-domain file=%s", m_strUnixPathFile.c_str()));
    CWX_INFO(("query listen tcp ip=%s, port=%u", m_filterListen.getHostName().c_str(), m_filterListen.getPort()));
    CWX_INFO(("charset encode=%s, case-sensive=%s", m_bUtf8?"utf-8":"gbk", m_bCaseSensive?"true":"false"));
    CWX_INFO(("thread_num=%u", m_unThreadNum));
    CWX_INFO(("mgr listen tcp ip=%s, port=%u", m_mgrListen.getHostName().c_str(), m_mgrListen.getPort()));
    list<CwfFilterConfigFilter*>::iterator iter = m_fiters.begin();
    while(iter != m_fiters.end())
    {
        CWX_INFO(("**************Filter, name=%s, id=%u***************", (*iter)->m_strName.c_str(), (*iter)->m_uiId));
        CWX_INFO(("\tstop-word-file=%s", (*iter)->m_strStopwordFile.c_str()));
        map<CWX_UINT8/*level*/, string/*file*/>::iterator level_iter = (*iter)->m_levelFiles.begin();
        while(level_iter != (*iter)->m_levelFiles.end())
        {
            CWX_INFO(("\tlevel=%u, file=%s", level_iter->first, level_iter->second.c_str()));
            level_iter++;
        }
        CWX_INFO(("*******************************************************"));
        iter++;
    }

    CWX_INFO(("*****************END   CONFIG *******************"));
}
