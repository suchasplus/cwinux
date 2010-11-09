#include "CwxEchoTbClientConfig.h"

int CwxEchoTbClientConfig::loadConfig(string const & strConfFile)
{
    CwxXmlFileConfigParser parser;
    char const* pValue;
    string value;
    //Ω‚Œˆ≈‰÷√Œƒº˛
    if (false == parser.parse(strConfFile)){
        snprintf(m_szError, 2047, "Failure to Load conf file.");
        return -1;
    }
    //load workdir svr_def:workdir{path}
    if ((NULL == (pValue=parser.getElementAttr("svr_def:workdir", "path"))) || !pValue[0]){
        snprintf(m_szError, 2047, "Must set [svr_def:workdir].");
        return -1;
    }
    value = pValue;
	if ('/' != value[value.length()-1]) value +="/";
    m_strWorkDir = value;
    // conn num
    if ((NULL == (pValue=parser.getElementAttr("svr_def:conn", "num"))) || !pValue[0]){
        snprintf(m_szError, 2047, "Must set [svr_def:conn:num].");
        return -1;
    }
    m_unConnNum =strtoul(pValue, NULL, 0);

    // data size
    if ((NULL == (pValue=parser.getElementAttr("svr_def:data", "size"))) || !pValue[0]){
        snprintf(m_szError, 2047, "Must set [svr_def:data].");
        return -1;
    }
    m_uiDataSize =strtoul(pValue, NULL, 0);
    // thread num
    if ((NULL == (pValue=parser.getElementAttr("svr_def:thread", "num"))) || !pValue[0]){
        snprintf(m_szError, 2047, "Must set [svr_def:thread:num].");
        return -1;
    }
    m_unThreadNum =strtoul(pValue, NULL, 0);
    //timeout
    if ((NULL == (pValue=parser.getElementAttr("svr_def:timeout", "mili_second"))) || !pValue[0]){
        snprintf(m_szError, 2047, "Must set [svr_def:timeout:mili_second].");
        return -1;
    }
    m_uiTimeout = strtoul(pValue, NULL, 0);
    //task
    if ((NULL == (pValue=parser.getElementAttr("svr_def:task", "num"))) || !pValue[0]){
        snprintf(m_szError, 2047, "Must set [svr_def:task:num].");
        return -1;
    }
    m_uiTaskNum = strtoul(pValue, NULL, 0);
    if(!m_uiTaskNum) m_uiTaskNum = 1;
    //conn_wait_send_msg
    if ((NULL == (pValue=parser.getElementAttr("svr_def:conn_wait_send_msg", "num"))) || !pValue[0]){
        snprintf(m_szError, 2047, "Must set [svr_def:conn_wait_send_msg:num].");
        return -1;
    }
    m_uiWaitMsgNum = strtoul(pValue, NULL, 0);
    //load mgr listen
    if ((pValue=parser.getElementAttr("svr_def:mgr", "ip")) && pValue[0])
    {
        m_mgrListen.setHostName(pValue);
        if ((NULL == (pValue=parser.getElementAttr("svr_def:mgr", "port"))) || !pValue[0])
        {
            snprintf(m_szError, 2047, "Must set [svr_def:mgr:port].");
            return -1;
        }
        m_mgrListen.setPort(strtoul(pValue, NULL, 0));
    }

    //load svr group
    CwxXmlTreeNode const* pNodeRoot = NULL;
    CwxXmlTreeNode const* node = NULL;
    CwxXmlTreeNode const* host_node = NULL;
    CwxHostInfo host;

    pair<char*, char*> key;

    //load echos group
    {
        CwxEchoTbClientGroup* group = NULL;
        pNodeRoot = parser.getElementNode("svr_def:echos");
        if (!pNodeRoot || !pNodeRoot->m_pChildHead)
        {
            CwxCommon::snprintf(m_szError, 511, "Must set [svr_def:echos].");
            return -1;
        }
        node = pNodeRoot->m_pChildHead;
        while(node)
        {
            if ((strcmp(node->m_szElement, "echo") == 0) && node->m_pChildHead)
            {
                group = new CwxEchoTbClientGroup();
                host_node = node->m_pChildHead;
                while(host_node)
                {
                    if (strcmp(host_node->m_szElement, "host") == 0)
                    {
                        host.reset();
                        //find unix
                        if (CwxCommon::findKey(host_node->m_lsAttrs, "unix",  key) && strlen(key.second))
                        {
                            host.setUnixDomain(string(key.second));
                        }
                        else if (CwxCommon::findKey(host_node->m_lsAttrs, "ip",  key) && strlen(key.second))
                        {
                            host.setHostName(string(key.second));
                            if (!CwxCommon::findKey(host_node->m_lsAttrs, "port",  key))
                            {
                                CwxCommon::snprintf(m_szError, 511, "Must set [svr_def:echos:echo:port].");
                                return -1;
                            }
                            host.setPort(strtoul(key.second, NULL, 0));
                        }
                        else
                        {
                            CwxCommon::snprintf(m_szError, 511, "Must set [svr_def:echos:echo:ip/unix].");
                            return -1;
                        }
                        group->m_hosts.push_back(host);
                    }
                    host_node = host_node->m_next;
                }
                m_svrGroup.push_back(group);
            }
            node = node->m_next;
        }
    }
    return 0;
}

void CwxEchoTbClientConfig::outputConfig(string & strConfig)
{
    char szBuf[512];
	strConfig.clear();	
	strConfig += "*****************BEGIN CONFIG *******************";
    strConfig += "\nworkdir= " ;
    strConfig += m_strWorkDir;
    if (m_mgrListen.getHostName().length())
    {
        strConfig += "\nmgr listen ip=";
        strConfig += m_mgrListen.getHostName();
        strConfig += "  port=";
        sprintf(szBuf, "%u", m_mgrListen.getPort());;
        strConfig += szBuf;
    }
    strConfig += "\nconn_num=";
    sprintf(szBuf, "%u", m_unConnNum);
    strConfig += szBuf;
    strConfig += "\ndata_size=";
    sprintf(szBuf, "%u", m_uiDataSize);
    strConfig += szBuf;
    sprintf(szBuf, "\ntimeout=%u", m_uiTimeout);
    strConfig += szBuf;
    sprintf(szBuf, "\ntask=%u\n", m_uiTaskNum);
    strConfig += szBuf;
    //echo groups
    list<CwxHostInfo>::iterator iter_host;
    list<CwxEchoTbClientGroup*>::iterator iter = m_svrGroup.begin();
    while(iter != m_svrGroup.end())
    {
        strConfig += "****************************echo group****************************";
        iter_host = (*iter)->m_hosts.begin();
        while(iter_host != (*iter)->m_hosts.end())
        {
            sprintf(szBuf, "ip=%s port=%u unix=%s\n", iter_host->getHostName().c_str(), iter_host->getPort(), iter_host->getUnixDomain().c_str());
            strConfig += szBuf;
            iter_host++;
        }
        iter++;
    }
    strConfig += "\n*****************END   CONFIG *******************\n";   
}
