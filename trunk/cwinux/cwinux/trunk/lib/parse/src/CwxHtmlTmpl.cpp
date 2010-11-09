#include "CwxHtmlTmpl.h"

CWINUX_BEGIN_NAMESPACE
CwxHtmlTmplCmpMap CwxHtmlTmpl::m_cmpMap;
CwxHtmlTmplEscapeMap CwxHtmlTmpl::m_escapeMap;

void CwxHtmlTmpl::init()
{
    clearCmp();
    clearEscape();
    ///registr compare
    addCmp(new CwxHtmlTmplCmpEqual());
    addCmp(new CwxHtmlTmplCmpLess());
    addCmp(new CwxHtmlTmplCmpMore());
    addCmp(new CwxHtmlTmplCmpPrefix());
    addCmp(new CwxHtmlTmplCmpRPrefix());
    addCmp(new CwxHtmlTmplCmpSuffix());
    addCmp(new CwxHtmlTmplCmpRSuffix());
    addCmp(new CwxHtmlTmplCmpIn());
    addCmp(new CwxHtmlTmplCmpRIn());
}

void CwxHtmlTmpl::destroy()
{
    clearCmp();
    clearEscape();
}

bool CwxHtmlTmpl::addCmp(CwxHtmlTmplCmp* pCmpObj)
{
    if (m_cmpMap.find(pCmpObj->getName()) != m_cmpMap.end()) return false;
    m_cmpMap[pCmpObj->getName()] = pCmpObj;
    return true;
}

CwxHtmlTmplCmp* CwxHtmlTmpl::getCmp(string const& strName)
{
    CwxHtmlTmplCmpMap::iterator iter = m_cmpMap.find(strName);
    if (iter != m_cmpMap.end()) return iter->second;
    return NULL;
}

CwxHtmlTmplCmp* CwxHtmlTmpl::eraseCmp(string const& strName)
{
    CwxHtmlTmplCmp* pCmp = NULL;
    CwxHtmlTmplCmpMap::iterator iter = m_cmpMap.find(strName);
    if (iter != m_cmpMap.end())
    {
        pCmp = iter->second;
        m_cmpMap.erase(iter);
    }
    return pCmp;
}

void CwxHtmlTmpl::clearCmp()
{
    CwxHtmlTmplCmp* pCmp = NULL;
    CwxHtmlTmplCmpMap::iterator iter = m_cmpMap.begin();
    while (iter != m_cmpMap.end())
    {
        pCmp = iter->second;
        m_cmpMap.erase(iter);
        delete pCmp;
        iter = m_cmpMap.begin();
    }
}
bool CwxHtmlTmpl::addEscape(CwxHtmlTmplEscape* pEscapeObj)
{
    if (m_escapeMap.find(pEscapeObj->getName()) != m_escapeMap.end()) return false;
    m_escapeMap[pEscapeObj->getName()] = pEscapeObj;
    return true;
}

CwxHtmlTmplEscape* CwxHtmlTmpl::getEscape(string const& strName)
{
    CwxHtmlTmplEscapeMap::iterator iter = m_escapeMap.find(strName);
    if (iter != m_escapeMap.end()) return iter->second;
    return NULL;
}

CwxHtmlTmplEscape* CwxHtmlTmpl::eraseEscape(string const& strName)
{
    CwxHtmlTmplEscapeMap::iterator iter = m_escapeMap.find(strName);
    CwxHtmlTmplEscape* pEscape = NULL;
    if (iter != m_escapeMap.end())
    {
        pEscape = iter->second;
        m_escapeMap.erase(iter);
    }
    return pEscape;
}

void CwxHtmlTmpl::clearEscape()
{
    CwxHtmlTmplEscapeMap::iterator iter = m_escapeMap.begin();
    CwxHtmlTmplEscape* pEscape = NULL;
    while(iter != m_escapeMap.end())
    {
        pEscape = iter->second;
        m_escapeMap.erase(iter);
        delete pEscape;
        iter = m_escapeMap.begin();
    }
}

bool CwxHtmlTmpl::parse(char const* szTemplate, CWX_UINT32 uiTemplateLen)
{
    bool bSuccess = true;
    this->clear();
    m_uiTmpBufLen = 2048;
    m_szTmpBuf = new char[m_uiTmpBufLen + 1];
    m_tmplLineReader.setMem(szTemplate, uiTemplateLen, false);
    do
    {
        m_uiReplaceNum = 0;
        if (!(bSuccess = parseCharset())) break;
        if (m_uiReplaceNum)
        {
            if (isGbkTmpl())
            {
                m_repl = new CwxReplMachine<CwxCharsetGbk>(m_uiReplaceNum);
            }
            else
            {
                m_repl = new CwxReplMachine<CwxCharsetUtf8>(m_uiReplaceNum);
            }
        }
        m_tmplLineReader.seekHead();
        memset(&m_root, 0x00, sizeof(m_root));
        m_root.m_kind = CwxHtmlTmplTagKind::TAG_JOIN;
        CwxHtmlTmplTagNode* pPrev = &m_root;
        while(readLine())
        {
            if (isCharsetDef())
            {
                continue;
            }
            else if (isReplaceDef())
            {
                if (!(bSuccess=parseReplace())) break;
            }
            else if (isConstDef())
            {
                if (!(bSuccess=parseConst())) break;
            }
            else if (isValFormatDef())
            {
                if (!(bSuccess=parseValFormat())) break;
            }
            else if (isLoopBeginDef())
            {
                pPrev = parseLoopBegin(pPrev);
                if (!pPrev)
                {
                    bSuccess = false;
                    break;
                }
            }
            else if (isLoopEndDef())
            {
                CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [loop] for the [end-loop], [end-loop]:%s\n", m_uiLineBegin, m_strLine.c_str());
                m_strErr = m_szTmpBuf;
                bSuccess = false;
                break;
            }
            else if (isIfBeginDef())
            {
                pPrev = parseIfBegin(pPrev);
                if (!pPrev)
                {
                    bSuccess = false;
                    break;
                }
            }
            else if (isIfElseIfDef())
            {
                CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [if] for the [elif], [elif]:%s\n", m_uiLineBegin, m_strLine.c_str());
                m_strErr = m_szTmpBuf;
                bSuccess = false;
                break;
            }
            else if (isIfElseDef())
            {
                CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [if] for the [else], [else]:%s\n", m_uiLineBegin, m_strLine.c_str());
                m_strErr = m_szTmpBuf;
                bSuccess = false;
                break;
            }
            else if (isIfEndDef())
            {
                CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [if] for the [end-if], [end-if]:%s\n", m_uiLineBegin, m_strLine.c_str());
                m_strErr = m_szTmpBuf;
                bSuccess = false;
                break;
            }
            else
            {
                pPrev= parseLine(pPrev);
                if (!pPrev)
                {
                    bSuccess = false;
                    break;
                }
            }
        }
    }while(0);

    m_valFormatMap.clear();
    m_constMap.clear();

    delete [] m_szTmpBuf;
    m_szTmpBuf = NULL;
    m_uiTmpBufLen = 0;
    return bSuccess;
}
bool CwxHtmlTmpl::output(CwxHtmlTmplEnv& env) const
{
    bool bSuccess = true;
    memset(env.m_szOutText, 0x00, env.m_uiOutTextLen);
    env.m_uiOutTextPos = 0;
    bSuccess = output(m_root.m_next, env);
    return bSuccess;
}


bool CwxHtmlTmpl::output(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env) const
{
    while(pNode)
    {
        if (CwxHtmlTmplTagKind::TAG_TEXT == pNode->m_kind)
        {
            if (!checkBufSize(env, pNode->m_tag.m_text.m_iTextLen)) return false;
            memcpy(env.m_szOutText + env.m_uiOutTextPos, pNode->m_tag.m_text.m_szText, pNode->m_tag.m_text.m_iTextLen);
            env.m_uiOutTextPos += pNode->m_tag.m_text.m_iTextLen;
        }
        else if (CwxHtmlTmplTagKind::TAG_VAL == pNode->m_kind)
        {
            if (!outputVal(pNode, env)) return false;
        }
        else if (CwxHtmlTmplTagKind::TAG_IF == pNode->m_kind)
        {
            if (!outputIf(pNode, env)) return false;
        }
        else if (CwxHtmlTmplTagKind::TAG_LOOP == pNode->m_kind)
        {
            if (!outputLoop(pNode, env)) return false;
        }
        else if (CwxHtmlTmplTagKind::TAG_JOIN == pNode->m_kind)
        {    
        }
        else
        {
            CWX_ASSERT(0);
        }
        pNode = pNode->m_next;
    }
    env.m_szOutText[env.m_uiOutTextPos] = 0x00;
    return true;
}

bool CwxHtmlTmpl::outputVal(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env) const
{
    char const* szValue = getValValue(pNode, env, true);
    if (!szValue)
    {
        if (CWX_CHECK_ATTR(pNode->m_tag.m_var.m_ucSysAttr, CwxHtmlTmplValSysAttr::ATTR_EXPLICT))
        {
            CwxCommon::snprintf(env.m_szErr1K, env.m_uiErrBufLen, "No value for val:%s", pNode->m_tag.m_var.m_szName);
            return false;
        }
        else
        {
            return true;
        }
    }
    CWX_UINT32 uiSrcLen = strlen(szValue);

    if (!checkBufSize(env, uiSrcLen)) return false;
    memcpy(env.m_szOutText + env.m_uiOutTextPos, szValue, uiSrcLen);
    env.m_uiOutTextPos += uiSrcLen;
    return true;
}

bool CwxHtmlTmpl::outputIf(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env) const
{
    CwxHtmlTmplTagNode node;
    initValNode(node, pNode->m_tag.m_if.m_szName, pNode->m_tag.m_if.m_szObjName, 0);
    char const* szValue = getValValue(&node, env, false);
    if (checkIfCondition(szValue, pNode, env))
    {
        pNode = pNode->m_tag.m_if.m_pMain;
    }
    else
    {
        pNode = pNode->m_tag.m_if.m_pBranch;
        while(pNode)
        {
            pNode = pNode->m_next; //skip the join node
            if (CwxHtmlTmplTagKind::TAG_ELIF == pNode->m_kind)
            {
                initValNode(node, pNode->m_tag.m_elif.m_szName, pNode->m_tag.m_elif.m_szObjName, 0);
                szValue = getValValue(&node, env, false);
                if (checkElifCondition(szValue, pNode, env))
                {
                    pNode = pNode->m_tag.m_elif.m_pMain;
                    break;//find the if's branche
                }
                else
                {
                    pNode = pNode->m_tag.m_elif.m_pBranch;
                }
            }
            else
            {//it's must be else node
                CWX_ASSERT(CwxHtmlTmplTagKind::TAG_ELSE == pNode->m_kind);
                pNode = pNode->m_tag.m_else.m_pBody;
                break;
            }
        }
    }
    if (pNode) return output(pNode, env);
    return true;
}


bool CwxHtmlTmpl::outputLoop(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env) const
{
    CwxHtmlTmplLoop* pLoop = env.m_loopStackList.begin()->second->loop(pNode->m_tag.m_loop.m_szLoopName, *pNode->m_tag.m_loop.m_attrs);
    if (!pLoop)
    {
        CwxCommon::snprintf(env.m_szErr1K, env.m_uiErrBufLen, "can't get loop[%s] instance", pNode->m_tag.m_loop.m_szLoopName);
        return false;
    }
    string strLoopName;
    if (env.m_loopMap.size())
    {
        strLoopName = env.m_loopStackList.begin()->first + "." + pNode->m_tag.m_loop.m_szLoopName;
    }
    else
    {
        strLoopName = pNode->m_tag.m_loop.m_szLoopName;
    }
    env.m_loopStackList.push_front(pair<string,CwxHtmlTmplLoop*>(strLoopName, pLoop));
    env.m_loopMap[strLoopName] = pLoop;
    while(1 == pLoop->next())
    {
        if (!output(pNode->m_tag.m_loop.m_pBody, env))
        {
            env.m_loopStackList.pop_front();
            env.m_loopMap.erase(strLoopName);
            return false;
        }
    }
    env.m_loopStackList.pop_front();
    env.m_loopMap.erase(strLoopName);
    return true;
}
void CwxHtmlTmpl::initValNode(CwxHtmlTmplTagNode& node, char const* szName, char const* szObjectName, CWX_UINT8 ucSysAttr) const
{
    node.m_kind = CwxHtmlTmplTagKind::TAG_VAL;
    node.m_tag.m_var.m_szName = szName;
    node.m_tag.m_var.m_szObjName = szObjectName;
    node.m_tag.m_var.m_iDataLen = 0;
    node.m_tag.m_var.m_pEscapeObj = NULL;
    node.m_tag.m_var.m_szArgument = NULL;
    node.m_tag.m_var.m_szDefault = NULL;
    node.m_tag.m_var.m_szEscape = NULL;
    node.m_tag.m_var.m_szUserAttr = NULL;
    node.m_tag.m_var.m_ucSysAttr = ucSysAttr;
}

char const* CwxHtmlTmpl::getValValue(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env, bool bVal) const
{
    CwxHtmlTmplLoop* pLoop = NULL;
    char const* szValue = NULL;
    if (pNode->m_tag.m_var.m_szObjName)
    {
        pLoop = env.m_loopMap.find(pNode->m_tag.m_var.m_szObjName)->second;
        szValue = pLoop->val(pNode->m_tag.m_var.m_szName,
            true,
            pNode->m_tag.m_var.m_ucSysAttr,
            pNode->m_tag.m_var.m_szUserAttr,
            pNode->m_tag.m_var.m_iDataLen,
            pNode->m_tag.m_var.m_pEscapeObj,
            pNode->m_tag.m_var.m_szArgument,
            m_repl,
            bVal);
    }
    else
    {
        list<pair<string, CwxHtmlTmplLoop*> >::iterator iter = env.m_loopStackList.begin();
        while(iter != env.m_loopStackList.end())
        {
            pLoop = iter->second;
            szValue = pLoop->val(pNode->m_tag.m_var.m_szName,
                false,
                pNode->m_tag.m_var.m_ucSysAttr,
                pNode->m_tag.m_var.m_szUserAttr,
                pNode->m_tag.m_var.m_iDataLen,
                pNode->m_tag.m_var.m_pEscapeObj,
                pNode->m_tag.m_var.m_szArgument,
                m_repl,
                bVal);
            if (szValue) break;
            iter++;
        }
    }
    if (!szValue) szValue = pNode->m_tag.m_var.m_szDefault;
    return szValue;
}


void CwxHtmlTmpl::clear()
{
    m_attrTmplMap.clear();
    m_strTmplName.clear();
    m_strTmplFile.clear();
    m_strCharset.clear();
    m_bCaseInsensitive = false;
    if (m_repl) delete m_repl;
    m_repl = NULL;
    memset(&m_root, 0x00, sizeof(m_root));
    m_valFormatMap.clear();
    m_constMap.clear();
    m_tagPool.clear();
    m_memPool.free();
    m_strErr.clear();

    m_tmplLineReader.clear();
    m_uiLineBegin = 0;
    m_uiLineEnd = 0;
    if (m_szTmpBuf) delete [] m_szTmpBuf;
    m_szTmpBuf = NULL;
    m_uiTmpBufLen = 0;
    m_loopList.clear();

}

bool CwxHtmlTmpl::parseReplace()
{
    list<pair<char*, char*> > const* lsAttrs = NULL;
    bool bRet = true;

    if (isGbkTmpl())
    {
        string strLine = "<?xml version=\"1.0\" encoding=\"gbk\" ?>\n";
        strLine += m_strLine;
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    else
    {
        string strLine = "<?xml version=\"1.0\"  ?>\n";
        strLine += m_strLine;
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    if (!bRet)
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], failure to parse replace command, replace command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return false;
    }
    bool bHex = false;
    pair<char*, char*>  item;
    if (CwxCommon::findKey(*lsAttrs, "HEX", item))
    {
        if (strcasecmp(item.second, "yes") == 0) bHex = true;
    }
    if (!CwxCommon::findKey(*lsAttrs, "SRC", item))
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], replace command hasn't [SRC] key, replace command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return false;
    }
    char const* pSrc=item.second;
    if (!CwxCommon::findKey(*lsAttrs, "DEST", item))
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], replace command hasn't [DEST] key, replace command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return false;
    }
    if (bHex)
    {
        size_t uiSize = strlen(pSrc)/2 + 1;
        char* src = m_memPool.malloc(uiSize);
        if (!CwxCommon::hexToString(pSrc, src, uiSize))
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], replace command's src not hex, replace command:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return false;
        }
        src[uiSize] = 0x00;
        uiSize =strlen(item.second)/2 + 1;
        char* dest = m_memPool.malloc(uiSize);
        if (!CwxCommon::hexToString(item.second, dest, uiSize))
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], replace command's dest not hex, replace command:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return false;
        }
        dest[uiSize] = 0x00;
        if (m_bCaseInsensitive)
            m_repl->addCaseStr(src, strlen(src), dest, strlen(dest));
        else
            m_repl->addStr(src, strlen(src), dest, strlen(dest));
    }
    else
    {
        if (m_bCaseInsensitive)
            m_repl->addCaseStr(pSrc, strlen(pSrc), item.second, strlen(item.second));
        else
            m_repl->addStr(pSrc, strlen(pSrc), item.second, strlen(item.second));
    }
    return true;
}

bool CwxHtmlTmpl::parseConst()
{
    list<pair<char*, char*> > const* lsAttrs = NULL;
    bool bRet = true;

    if (isGbkTmpl())
    {
        string strLine = "<?xml version=\"1.0\" encoding=\"gbk\" ?>\n";
        strLine += m_strLine;
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    else
    {
        string strLine = "<?xml version=\"1.0\"  ?>\n";
        strLine += m_strLine;
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    if (!bRet)
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], failure to parse const command, const command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return false;
    }
    pair<char*, char*>  item;
    if (!CwxCommon::findKey(*lsAttrs, "NANE", item))
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], const command hasn't [NAME] key, const command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return false;
    }
    char const* pSrc=item.second;
    if (!CwxCommon::findKey(*lsAttrs, "VALUE", item))
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], const command hasn't [VALUE] key, const command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return false;
    }
    m_constMap[pSrc] = item.second;
    return true;
}

bool CwxHtmlTmpl::parseValFormat()
{
    list<pair<char*, char*> > const* lsAttrs = NULL;
    bool bRet = true;

    if (isGbkTmpl())
    {
        string strLine = "<?xml version=\"1.0\" encoding=\"gbk\" ?>\n";
        strLine += m_strLine;
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    else
    {
        string strLine = "<?xml version=\"1.0\"  ?>\n";
        strLine += m_strLine;
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    if (!bRet)
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], failure to parse val format command, val format command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return false;
    }
    CwxHtmlTmplTagNode* pTag = m_tagPool.malloc();
    pTag->m_kind = CwxHtmlTmplTagKind::TAG_VAL;
    pTag->m_next = NULL;
    char* szName = NULL;
    char* szAttr = NULL;
    char* szLength = NULL;
    char* szDefault = NULL;
    char* szEscape = NULL;
    char* szArgument = NULL;
    pair<char*, char*>  item;
    if (!CwxCommon::findKey(*lsAttrs, "NAME", item))
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], val format command hasn't [NAME] key, val format command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return false;
    }
    szName = item.second;
    if (CwxCommon::findKey(*lsAttrs, "ATTR", item))
    {
        szAttr = item.second;
    }
    if (CwxCommon::findKey(*lsAttrs, "LENGTH", item))
    {
        szLength = item.second;
    }
    if (CwxCommon::findKey(*lsAttrs, "ESCAPE", item))
    {
        szEscape = item.second;
    }
    if (CwxCommon::findKey(*lsAttrs, "DEFAULT", item))
    {
        szDefault = item.second;
    }
    if (CwxCommon::findKey(*lsAttrs, "ARGUMENT", item))
    {
        szArgument = item.second;
    }
    if (!parseVal(pTag, szName, szAttr, szLength, szDefault, szEscape, szArgument)) return false;
    m_valFormatMap[pTag->m_tag.m_var.m_szName] = pTag;
    return true;
}

CwxHtmlTmplTagNode* CwxHtmlTmpl::parseLoopBegin(CwxHtmlTmplTagNode* pPrev)
{
    list<pair<char*, char*> > const* lsAttrs = NULL;
    bool bRet = true;

    if (isGbkTmpl())
    {
        string strLine = "<?xml version=\"1.0\" encoding=\"gbk\" ?>\n";
        strLine += m_strLine;
        strLine += "</TMPL_LOOP>";
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    else
    {
        string strLine = "<?xml version=\"1.0\" ?>\n";
        strLine += m_strLine;
        strLine += "</TMPL_LOOP>";
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    if (!bRet)
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], failure to parse loop command, loop command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return NULL;
    }
    CwxHtmlTmplTagNode* pLoop = m_tagPool.malloc();
    pLoop->m_kind = CwxHtmlTmplTagKind::TAG_LOOP;
    pLoop->m_next = NULL;
    pair<char*, char*>  item;
    if (!CwxCommon::findKey(*lsAttrs, "NAME", item))
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], loop command hasn't [NAME] key, loop command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return NULL;
    }
    pLoop->m_tag.m_loop.m_szLoopName = m_memPool.malloc(strlen(item.second) + 1);
    strcpy((char*)pLoop->m_tag.m_loop.m_szLoopName, item.second);
    if (lsAttrs->size()>1)
    {
        pLoop->m_tag.m_loop.m_attrs = new list<pair<char*, char*> >();
        list<pair<char*, char*> >::const_iterator iter = lsAttrs->begin();
        while(iter != lsAttrs->end())
        {
            if (strcmp(iter->first, "NAME")!=0)
            {
                item.first = m_memPool.malloc(strlen(iter->first) + 1);
                strcpy(item.first, iter->first);
                item.second = m_memPool.malloc(strlen(iter->second) + 1);
                strcpy(item.second, iter->second);
                pLoop->m_tag.m_loop.m_attrs->push_back(item);
            }
            iter++;
        }
    }
    else
    {
        pLoop->m_tag.m_loop.m_attrs = NULL;
    }
    pPrev->m_next = pLoop;
    pLoop->m_tag.m_loop.m_pBody = m_tagPool.malloc();
    pPrev = pLoop->m_tag.m_loop.m_pBody;
    pPrev->m_next = NULL;
    pPrev->m_kind = CwxHtmlTmplTagKind::TAG_JOIN;

    {
        string strLoopName;
        if (m_loopList.empty())
        {
            strLoopName = pLoop->m_tag.m_loop.m_szLoopName;
            m_loopList.push_front(strLoopName);
        }
        else
        {
            strLoopName = *m_loopList.begin() + pLoop->m_tag.m_loop.m_szLoopName;
            m_loopList.push_front(strLoopName);
        }
    }

    bool bFindLoopEnd = false;
    string strLoop = m_strLine;
    while(readLine())
    {
        if (isCharsetDef())
        {
            continue;
        }
        else if (isReplaceDef())
        {
            if (!parseReplace()) return NULL;
        }
        else if (isConstDef())
        {
            if (!parseConst()) return NULL;
        }
        else if (isValFormatDef())
        {
            if (!parseValFormat()) return NULL;
        }
        else if (isLoopBeginDef())
        {
            pPrev = parseLoopBegin(pPrev);
            if (!pPrev) return NULL;
        }
        else if (isLoopEndDef())
        {
            bFindLoopEnd = true;
            break;
        }
        else if (isIfBeginDef())
        {
            pPrev = parseIfBegin(pPrev);
            if (!pPrev) return NULL;
        }
        else if (isIfElseIfDef())
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [if] for the [elif], [elif]:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        else if (isIfElseDef())
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [if] for the [else], [else]:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        else if (isIfEndDef())
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [if] for the [end-if], [end-if]:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        else
        {
            pPrev= parseLine(pPrev);
            if (!pPrev) return NULL;
        }
        pPrev->m_next = NULL;
    }
    if (!bFindLoopEnd)
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No find loop's end, loop:%s\n", m_uiLineBegin, strLoop.c_str());
        m_strErr = m_szTmpBuf;
        return NULL;
    }
    m_loopList.pop_front();
    return pLoop;
}

CwxHtmlTmplTagNode* CwxHtmlTmpl::parseIfBegin(CwxHtmlTmplTagNode* pPrev)
{
    list<pair<char*, char*> > const* lsAttrs = NULL;
    bool bRet = true;

    if (isGbkTmpl())
    {
        string strLine = "<?xml version=\"1.0\" encoding=\"gbk\" ?>\n";
        strLine += m_strLine;
        strLine += "</TMPL_IF>";
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    else
    {
        string strLine = "<?xml version=\"1.0\" ?>\n";
        strLine += m_strLine;
        strLine += "</TMPL_IF>";
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    if (!bRet)
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], failure to parse if command, if command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return NULL;
    }
    CwxHtmlTmplTagNode* pIf = m_tagPool.malloc();
    pIf->m_kind = CwxHtmlTmplTagKind::TAG_IF;
    pIf->m_next = NULL;
    pair<char*, char*>  item;

    if (!CwxCommon::findKey(*lsAttrs, "NAME", item))
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], if command hasn't [NAME] key, if command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return NULL;
    }
    pIf->m_tag.m_if.m_szName = m_memPool.malloc(strlen(item.second) + 1);
    strcpy((char*)pIf->m_tag.m_if.m_szName, item.second);

    parseValObject(pIf->m_tag.m_if.m_szName, pIf->m_tag.m_if.m_szObjName);
    if (pIf->m_tag.m_if.m_szObjName)
    {
        if (!isExistObject(pIf->m_tag.m_if.m_szObjName))
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], if command's name[%s]'s object doesn't exist, if command:%s\n", m_uiLineBegin, pIf->m_tag.m_if.m_szName, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        pIf->m_tag.m_if.m_szName = pIf->m_tag.m_if.m_szName + strlen(pIf->m_tag.m_if.m_szObjName) + 1;
    }

    if (!CwxCommon::findKey(*lsAttrs, "FUNC", item))
    {
        pIf->m_tag.m_if.m_pCompareObj = NULL;
        pIf->m_tag.m_if.m_szCompair = NULL;
        pIf->m_tag.m_if.m_bReverse = false;
    }
    else
    {
        CwxCommon::trim(item.second);
        pIf->m_tag.m_if.m_bReverse = false;
        if (strlen(item.second))
        {
            if ('!' == item.second[0])
            {
                pIf->m_tag.m_if.m_bReverse = true;
                item.second++;
            }
        }
        if (strlen(item.second))
        {
            pIf->m_tag.m_if.m_pCompareObj = CwxHtmlTmpl::getCmp(item.second);
            if (!pIf->m_tag.m_if.m_pCompareObj)
            {
                CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], if command's function[%s] doesn't exist, if command:%s\n", m_uiLineBegin, item.second, m_strLine.c_str());
                m_strErr = m_szTmpBuf;
                return NULL;
            }
            pIf->m_tag.m_if.m_szCompair = m_memPool.malloc(strlen(item.second) + 1);
            strcpy((char*)pIf->m_tag.m_if.m_szCompair, item.second);
        }
        else
        {
            pIf->m_tag.m_if.m_pCompareObj = NULL;
            pIf->m_tag.m_if.m_szCompair = NULL;
        }
    }

    if (!CwxCommon::findKey(*lsAttrs, "VALUE", item))
    {
        pIf->m_tag.m_if.m_szData = NULL;
    }
    else
    {
        pIf->m_tag.m_if.m_szData = m_memPool.malloc(strlen(item.second) + 1);
        strcpy((char*)pIf->m_tag.m_if.m_szData, item.second);
    }
    if (CwxCommon::findKey(*lsAttrs, "ARGUMENT", item))
    {
        pIf->m_tag.m_if.m_szArgument = m_memPool.malloc(strlen(item.second) + 1);
        strcpy((char*)pIf->m_tag.m_if.m_szArgument, item.second);
    }
    else
    {
        pIf->m_tag.m_if.m_szArgument = NULL;
    }

    pIf->m_tag.m_if.m_pMain = NULL;
    pIf->m_tag.m_if.m_pBranch = NULL;

    pPrev->m_next = pIf;
    pIf->m_tag.m_if.m_pMain = m_tagPool.malloc();
    pPrev = pIf->m_tag.m_if.m_pMain;
    pPrev->m_next = NULL;
    pPrev->m_kind = CwxHtmlTmplTagKind::TAG_JOIN;
    while(readLine())
    {
        if (isCharsetDef())
        {
            continue;
        }
        else if (isReplaceDef())
        {
            if (!parseReplace()) return NULL;
        }
        else if (isConstDef())
        {
            if (!parseConst()) return NULL;
        }
        else if (isValFormatDef())
        {
            if (!parseValFormat()) return NULL;
        }
        else if (isLoopBeginDef())
        {
            pPrev = parseLoopBegin(pPrev);
            if (!pPrev) return NULL;
        }
        else if (isLoopEndDef())
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [loop] for the [loop-end], [loop-end]:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        else if (isIfBeginDef())
        {
            pPrev = parseIfBegin(pPrev);
            if (!pPrev) return NULL;
        }
        else if (isIfElseIfDef())
        {
            pPrev->m_next = NULL; //end for the if-main-block
            pIf->m_tag.m_if.m_pBranch = m_tagPool.malloc();
            pPrev = pIf->m_tag.m_if.m_pBranch;
            pPrev->m_next = NULL;
            pPrev->m_kind = CwxHtmlTmplTagKind::TAG_JOIN;
            pPrev = parseIfElif(pPrev);
            if (!pPrev) return NULL;
            pPrev->m_next = NULL;
            break;
        }
        else if (isIfElseDef())
        {
            pPrev->m_next = NULL; //end for the if-main-block
            pIf->m_tag.m_if.m_pBranch = m_tagPool.malloc();
            pPrev = pIf->m_tag.m_if.m_pBranch;
            pPrev->m_next = NULL;
            pPrev->m_kind = CwxHtmlTmplTagKind::TAG_JOIN;
            pPrev = parseIfElse(pPrev);
            if (!pPrev) return NULL;
            pPrev->m_next = NULL;
            break;
        }
        else if (isIfEndDef())
        {
            pPrev->m_next = NULL;
            break;
        }
        else
        {
            pPrev= parseLine(pPrev);
            if (!pPrev) return NULL;
        }
        pPrev->m_next = NULL;
    }
    return pIf;
}

CwxHtmlTmplTagNode* CwxHtmlTmpl::parseIfElif(CwxHtmlTmplTagNode* pPrev)
{
    list<pair<char*, char*> > const* lsAttrs = NULL;
    bool bRet = true;

    if (isGbkTmpl())
    {
        string strLine = "<?xml version=\"1.0\" encoding=\"gbk\" ?>\n";
        strLine += m_strLine;
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    else
    {
        string strLine = "<?xml version=\"1.0\"  ?>\n";
        strLine += m_strLine;
        bRet = parseXml(strLine.c_str(), lsAttrs);
    }
    if (!bRet)
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], failure to parse elif command, elif command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return NULL;
    }
    CwxHtmlTmplTagNode* pElif = m_tagPool.malloc();
    pElif->m_kind = CwxHtmlTmplTagKind::TAG_ELIF;
    pElif->m_next = NULL;
    pair<char*, char*>  item;

    if (!CwxCommon::findKey(*lsAttrs, "NAME", item))
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], elif command hasn't [NAME] key, elif command:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return NULL;
    }
    pElif->m_tag.m_elif.m_szName = m_memPool.malloc(strlen(item.second) + 1);
    strcpy((char*)pElif->m_tag.m_elif.m_szName, item.second);

    parseValObject(pElif->m_tag.m_elif.m_szName, pElif->m_tag.m_elif.m_szObjName);
    if (pElif->m_tag.m_elif.m_szObjName)
    {
        if (!isExistObject(pElif->m_tag.m_elif.m_szObjName))
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], elif command's name[%s]'s object doesn't exist, elif command:%s\n", m_uiLineBegin, pElif->m_tag.m_elif.m_szObjName, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        pElif->m_tag.m_elif.m_szName = pElif->m_tag.m_elif.m_szName + strlen(pElif->m_tag.m_elif.m_szObjName) + 1;
    }

    if (!CwxCommon::findKey(*lsAttrs, "FUNC", item))
    {
        pElif->m_tag.m_elif.m_pCompareObj = NULL;
        pElif->m_tag.m_elif.m_szCompair = NULL;
        pElif->m_tag.m_elif.m_bReverse = false;
    }
    else
    {
        pElif->m_tag.m_elif.m_bReverse = false;
        if (strlen(item.second))
        {
            if ('!' == item.second[0])
            {
                pElif->m_tag.m_elif.m_bReverse = true;
                item.second++;
            }
        }
        if (strlen(item.second))
        {
            pElif->m_tag.m_elif.m_pCompareObj = CwxHtmlTmpl::getCmp(item.second);
            if (!pElif->m_tag.m_elif.m_pCompareObj)
            {
                CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], elif command's function[%s] doesn't exist, elif command:%s\n", m_uiLineBegin, item.second, m_strLine.c_str());
                m_strErr = m_szTmpBuf;
                return NULL;
            }
            pElif->m_tag.m_elif.m_szCompair = m_memPool.malloc(strlen(item.second) + 1);
            strcpy((char*)pElif->m_tag.m_elif.m_szCompair, item.second);
        }
        else
        {
            pElif->m_tag.m_elif.m_pCompareObj = NULL;
            pElif->m_tag.m_elif.m_szCompair = NULL;
        }
    }
    if (!CwxCommon::findKey(*lsAttrs, "VALUE", item))
    {
        pElif->m_tag.m_elif.m_szData = NULL;
    }
    else
    {
        pElif->m_tag.m_elif.m_szData = m_memPool.malloc(strlen(item.second) + 1);
        strcpy((char*)pElif->m_tag.m_elif.m_szData, item.second);
    }
    if (CwxCommon::findKey(*lsAttrs, "ARGUMENT", item))
    {
        pElif->m_tag.m_elif.m_szArgument = m_memPool.malloc(strlen(item.second) + 1);
        strcpy((char*)pElif->m_tag.m_elif.m_szArgument, item.second);
    }
    else
    {
        pElif->m_tag.m_elif.m_szArgument = NULL;
    }

    pElif->m_tag.m_elif.m_pMain = NULL;
    pElif->m_tag.m_elif.m_pBranch = NULL;

    pPrev->m_next = pElif;
    pElif->m_tag.m_elif.m_pMain = m_tagPool.malloc();
    pPrev = pElif->m_tag.m_elif.m_pMain;
    pPrev->m_next = NULL;
    pPrev->m_kind = CwxHtmlTmplTagKind::TAG_JOIN;
    while(readLine())
    {
        if (isCharsetDef())
        {
            continue;
        }
        else if (isReplaceDef())
        {
            if (!parseReplace()) return NULL;
        }
        else if (isConstDef())
        {
            if (!parseConst()) return NULL;
        }
        else if (isValFormatDef())
        {
            if (!parseValFormat()) return NULL;
        }
        else if (isLoopBeginDef())
        {
            pPrev = parseLoopBegin(pPrev);
            if (!pPrev) return NULL;
        }else if (isLoopEndDef()){
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [loop] for the [loop-end], [loop-end]:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        else if (isIfBeginDef())
        {
            pPrev = parseIfBegin(pPrev);
            if (!pPrev) return NULL;
        }
        else if (isIfElseIfDef())
        {
            pPrev->m_next = NULL; //end for the if-main-block
            pElif->m_tag.m_elif.m_pBranch = m_tagPool.malloc();
            pPrev = pElif->m_tag.m_elif.m_pBranch;
            pPrev->m_next = NULL;
            pPrev->m_kind = CwxHtmlTmplTagKind::TAG_JOIN;
            pPrev = parseIfElif(pPrev);
            if (!pPrev) return NULL;
            pPrev->m_next = NULL;
            break;
        }
        else if (isIfElseDef())
        {
            pPrev->m_next = NULL; //end for the if-main-block
            pElif->m_tag.m_elif.m_pBranch = m_tagPool.malloc();
            pPrev = pElif->m_tag.m_elif.m_pBranch;
            pPrev->m_next = NULL;
            pPrev->m_kind = CwxHtmlTmplTagKind::TAG_JOIN;
            pPrev = parseIfElse(pPrev);
            if (!pPrev) return NULL;
            pPrev->m_next = NULL;
            break;
        }
        else if (isIfEndDef())
        {
            pPrev->m_next = NULL;
            break;
        }
        else
        {
            pPrev= parseLine(pPrev);
            if (!pPrev) return NULL;
        }
        pPrev->m_next = NULL;
    }
    return pElif;
}

CwxHtmlTmplTagNode* CwxHtmlTmpl::parseIfElse(CwxHtmlTmplTagNode* pPrev)
{
    CwxHtmlTmplTagNode* pElse = m_tagPool.malloc();
    pElse->m_kind = CwxHtmlTmplTagKind::TAG_ELSE;
    pElse->m_next = NULL;
    pPrev->m_next = pElse;

    pElse->m_tag.m_else.m_pBody = m_tagPool.malloc();
    pPrev = pElse->m_tag.m_else.m_pBody;
    pPrev->m_next = NULL;
    pPrev->m_kind = CwxHtmlTmplTagKind::TAG_JOIN;

    while(readLine())
    {
        if (isCharsetDef())
        {
            continue;
        }
        else if (isReplaceDef())
        {
            if (!parseReplace()) return NULL;
        }
        else if (isConstDef())
        {
            if (!parseConst()) return NULL;
        }
        else if (isValFormatDef())
        {
            if (!parseValFormat()) return NULL;
        }
        else if (isLoopBeginDef())
        {
            pPrev = parseLoopBegin(pPrev);
            if (!pPrev) return NULL;
        }
        else if (isLoopEndDef())
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [loop] for the [loop-end], [loop-end]:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        else if (isIfBeginDef())
        {
            pPrev = parseIfBegin(pPrev);
            if (!pPrev) return NULL;
        }
        else if (isIfElseIfDef())
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [if] for the [elif], [elif]:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        else if (isIfElseDef())
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], No [if] for the [else], [else]:%s\n", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return NULL;
        }
        else if (isIfEndDef())
        {
            pPrev->m_next = NULL;
            break;
        }
        else
        {
            pPrev= parseLine(pPrev);
            if (!pPrev) return NULL;
        }
        pPrev->m_next = NULL;
    }
    return pElse;
}

CwxHtmlTmplTagNode* CwxHtmlTmpl::parseLine(CwxHtmlTmplTagNode* pPrev)
{
    list<string> arrText;
    list<string> arrValFields;

    if (CwxCommon::split(m_strLine, arrText, '$')%2==0)
    {
        CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], [%] is val's sign, [%]'s num is odd, line:%s\n", m_uiLineBegin, m_strLine.c_str());
        m_strErr = m_szTmpBuf;
        return NULL;
    }
    CWX_UINT32 uiBlockIndex=0;
    list<string>::iterator iter=arrText.begin();
    list<string>::iterator iter_field;
    CwxHtmlTmplTagNode* pTag = NULL;
    char const* szName = NULL;
    char const* szAttr = NULL;
    char const* szLength = NULL;
    char const* szDefault = NULL;
    char const* szEscape = NULL;
    char const* szArgument = NULL;
    while(iter != arrText.end())
    {
        if (uiBlockIndex%2==0)
        {//it's text
            if (iter->length())
            {
                pTag = m_tagPool.malloc();
                pTag->m_kind = CwxHtmlTmplTagKind::TAG_TEXT;
                pTag->m_next = NULL;
                pTag->m_tag.m_text.m_iTextLen = iter->length();
                pTag->m_tag.m_text.m_szText = m_memPool.malloc(pTag->m_tag.m_text.m_iTextLen + 1);
                strcpy((char*)pTag->m_tag.m_text.m_szText, iter->c_str());
                pPrev->m_next = pTag;
                pPrev = pTag;
            }
        }
        else
        {
            szName = NULL;
            szAttr = NULL;
            szLength = NULL;
            szDefault = NULL;
            szEscape = NULL;
            szArgument = NULL;
            if (iter->length())
            {
                CwxCommon::split(*iter, arrValFields, ':');
                iter_field = arrValFields.begin();
                if (m_constMap.find(*iter_field) != m_constMap.end())
                {//const
                    pTag = m_tagPool.malloc();
                    pTag->m_kind = CwxHtmlTmplTagKind::TAG_TEXT;
                    pTag->m_next = NULL;
                    pTag->m_tag.m_text.m_iTextLen = m_constMap.find(*iter_field)->second.length();
                    pTag->m_tag.m_text.m_szText = m_constMap.find(*iter_field)->second.c_str();
                    pPrev->m_next = pTag;
                    pPrev = pTag;
                }
                else if (m_valFormatMap.find(iter_field->c_str()) != m_valFormatMap.end())
                {
                    pTag = m_tagPool.malloc();
                    memcpy(pTag, m_valFormatMap.find(iter_field->c_str())->second, sizeof(*pTag));
                    pTag->m_kind = CwxHtmlTmplTagKind::TAG_TEXT;
                    pTag->m_next = NULL;
                    pPrev->m_next = pTag;
                    pPrev = pTag;
                }
                else
                {
                    //name
                    szName = m_memPool.malloc(iter_field->length() + 1);
                    strcpy((char*)szName, iter_field->c_str());
                    iter_field++;
                    //attr
                    if(iter_field != arrValFields.end())
                    {
                        szAttr = m_memPool.malloc(iter_field->length() + 1);
                        strcpy((char*)szAttr, iter_field->c_str());
                        iter_field++;
                    }
                    //length
                    if (iter_field != arrValFields.end())
                    {
                        szLength = m_memPool.malloc(iter_field->length() + 1);
                        strcpy((char*)szLength, iter_field->c_str());
                        iter_field++;
                    }
                    //default
                    if (iter_field != arrValFields.end())
                    {
                        szDefault = m_memPool.malloc(iter_field->length() + 1);
                        strcpy((char*)szDefault, iter_field->c_str());
                        iter_field++;
                    }
                    //Escape
                    if (iter_field != arrValFields.end())
                    {
                        szEscape = m_memPool.malloc(iter_field->length() + 1);
                        strcpy((char*)szEscape, iter_field->c_str());
                        iter_field++;
                    }
                    //argument
                    if (iter_field != arrValFields.end())
                    {
                        szArgument = m_memPool.malloc(iter_field->length() + 1);
                        strcpy((char*)szArgument, iter_field->c_str());
                        iter_field++;
                    }
                    pTag = m_tagPool.malloc();
                    if (!parseVal(pTag, szName, szAttr, szLength, szDefault, szEscape, szArgument)) return NULL; 
                    pTag->m_kind = CwxHtmlTmplTagKind::TAG_VAL;
                    pTag->m_next = NULL;
                    pPrev->m_next = pTag;
                    pPrev = pTag;
                }
            }
            else
            {
                pTag = m_tagPool.malloc();
                pTag->m_kind = CwxHtmlTmplTagKind::TAG_TEXT;
                pTag->m_next = NULL;
                pTag->m_tag.m_text.m_iTextLen = 1;
                pTag->m_tag.m_text.m_szText = m_memPool.malloc(pTag->m_tag.m_text.m_iTextLen + 1);
                pTag->m_tag.m_text.m_szText = "$";
                pPrev->m_next = pTag;
                pPrev = pTag;
            }
        }
        iter++;
        uiBlockIndex++;
    }
    return pPrev;
}



bool CwxHtmlTmpl::readLine()
{
    m_uiLineEnd = 0;
    m_strLine.clear();
    m_uiLineBegin = m_tmplLineReader.getCurLineNo();
    char const* szLineEnd = NULL;
    while(m_tmplLineReader.nextLine())
    {
        m_uiLineEnd = m_tmplLineReader.getCurLineNo();
        if (!isLineContinue(szLineEnd))
        {
            m_strLine += string(m_tmplLineReader.getCurLineBegin(), m_tmplLineReader.getCurLineEnd() - m_tmplLineReader.getCurLineBegin());
            return true;
        }
        m_strLine += string(m_tmplLineReader.getCurLineBegin(), szLineEnd - m_tmplLineReader.getCurLineBegin());

    }
    return false;
}

bool CwxHtmlTmpl::parseXml(char const* szXml, list<pair<char*, char*> > const*& lsAttrs)
{
    if (!m_cmdParse.parse(szXml)) return false;
    lsAttrs = &m_cmdParse.getRoot()->m_lsAttrs;
    return true;
}


bool CwxHtmlTmpl::parseCharset()
{
    list<pair<char*, char*> > const* lsAttrs = NULL;
    pair<char*, char*> item;
    bool bFindCharset = false;
    m_tmplLineReader.seekHead();
    m_uiReplaceNum = 0;
    while(readLine())
    {
        if (isReplaceDef())
        {
            m_uiReplaceNum++;
        }
        else if (!bFindCharset && isCharsetDef())
        {
            bFindCharset = true;
            if (!parseXml(m_strLine.c_str(), lsAttrs))
            {
                CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u]: Invalid XML-CHARSET, content:%s\n", m_uiLineBegin, m_strLine.c_str());
                m_strErr = m_szTmpBuf;
                return false;
            }
            if (CwxCommon::findKey(*lsAttrs, "ENCODING", item))
            {
                m_strCharset = item.second;
                if (!CwxCharsetGbk::charset(m_strCharset.c_str()) && !CwxCharsetUtf8::charset(m_strCharset.c_str()))
                {
                    CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u]: Invalid charset name:%s\n", m_uiLineBegin, m_strCharset.c_str());
                    m_strErr = m_szTmpBuf;
                    return false;
                }
            }
            if (CwxCommon::findKey(*lsAttrs, "CASE-INSENSITIVE", item))
            {
                if (strcasecmp(item.second, "yes")==0) m_bCaseInsensitive = true;
            }
        }
    }
    if (!m_strCharset.length() || !CwxCharsetGbk::charset(m_strCharset.c_str()))
    {
        m_strCharset = CwxCharsetUtf8::charset();
    }
    return true;
}

bool CwxHtmlTmpl::prepareCmd(list<pair<char*, char*> >& lsAttrs)
{
    list<pair<char*, char*> >const* pLocalAttrs=NULL;
    lsAttrs.clear();
    if (isGbkTmpl())
    {
        string strXml = "<?xml version='1.0' encoding='gbk' ?>";
        strXml += m_strLine; 
        if (!parseXml(strXml.c_str(), pLocalAttrs))
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u]: invalid xml command, cmd:%s", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return false;
        }

    }
    else
    {
        if (!parseXml(m_strLine.c_str(), pLocalAttrs))
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u]: invalid xml command, cmd:%s", m_uiLineBegin, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return false;
        }
    }
    list<pair<char*, char*> >::const_iterator iter = pLocalAttrs->begin();
    pair<char*, char*> item;
    while(iter != pLocalAttrs->end())
    {
        item.first = m_memPool.malloc(strlen(iter->first) + 1);
        strcpy(item.first, iter->first);
        item.second = m_memPool.malloc(strlen(iter->second) + 1);
        strcpy(item.second, iter->second);
        lsAttrs.push_back(item);
        iter++;
    }
    return true;
}

bool CwxHtmlTmpl::isCharsetDef()
{
    return m_strLine.find("<TMPL_CHARSET") != string::npos;
}

bool CwxHtmlTmpl::isReplaceDef()
{
    return m_strLine.find("<TMPL_REPLACE") != string::npos;
}

bool CwxHtmlTmpl::isConstDef()
{
    return m_strLine.find("<TMPL_CONST") != string::npos;
}

bool CwxHtmlTmpl::isValFormatDef()
{
    return m_strLine.find("<TMPL_VAR") != string::npos;
}

bool CwxHtmlTmpl::isLoopBeginDef()
{
    return m_strLine.find("<TMPL_LOOP") != string::npos;
}

bool CwxHtmlTmpl::isLoopEndDef()
{
    return m_strLine.find("</TMPL_LOOP>") != string::npos;
}

bool CwxHtmlTmpl::isIfBeginDef()
{
    return m_strLine.find("<TMPL_IF") != string::npos;
}

bool CwxHtmlTmpl::isIfElseIfDef()
{
    return m_strLine.find("<TMPL_ELIF") != string::npos;
}
bool CwxHtmlTmpl::isIfElseDef()
{
    return m_strLine.find("<TMPL_ELSE") != string::npos;
}
bool CwxHtmlTmpl::isIfEndDef()
{
    return m_strLine.find("</TMPL_IF>") != string::npos;
}

bool CwxHtmlTmpl::isLineContinue(char const*& szLineEnd)
{
    char const* szLineBegin = m_tmplLineReader.getCurLineBegin();
    szLineEnd = m_tmplLineReader.getCurLineEnd()-1;
    while(szLineBegin <= szLineEnd)
    {
        if (('\r' == szLineEnd[0]) || ('\n' == szLineEnd[0]))
        {
            szLineEnd--;
            continue;
        }
        if ('\\' == szLineEnd[0])
        {
            return true;
        }
        break;
    }
    return false;
}
bool CwxHtmlTmpl::isGbkTmpl()
{
    return CwxCharsetGbk::charset(m_strCharset.c_str());
}

void CwxHtmlTmpl::parseValObject(char const* szValName, char const*& szObjName)
{
    szObjName = NULL;
    char const* pPos = strrchr(szValName, '.');
    if (!pPos) return;
    szObjName = m_memPool.malloc(pPos - szValName + 1 );
    memcpy((char*)szObjName, szValName, pPos - szValName);
    ((char*)szObjName)[pPos - szValName] = 0x00;
}

bool CwxHtmlTmpl::isExistObject(char const* szObjName)
{
    list<string>::iterator iter = m_loopList.begin();
    while(iter != m_loopList.end())
    {
        if (*iter == szObjName) return true;
        iter++;
    }
    return false;
}

bool CwxHtmlTmpl::parseVal(CwxHtmlTmplTagNode* pTag, char const* szName, char const* szAttr, char const* szLength, char const* szDefault, char const* szEscape, char const* szArgument)
{
    pTag->m_kind = CwxHtmlTmplTagKind::TAG_VAL;
    pTag->m_tag.m_var.m_szName = m_memPool.malloc(strlen(szName) + 1);
    strcpy((char*)pTag->m_tag.m_var.m_szName, szName);

    parseValObject(pTag->m_tag.m_var.m_szName, pTag->m_tag.m_var.m_szObjName);
    if (pTag->m_tag.m_var.m_szObjName)
    {
        if (isExistObject(pTag->m_tag.m_var.m_szObjName))
        {
//            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], val's object name[%s] doesn't exist, val format command:%s\n", m_uiLineBegin, pTag->m_tag.m_var.m_szObjName, m_strLine.c_str());
//            m_strErr = m_szTmpBuf;
//            return false;
//        }
            pTag->m_tag.m_var.m_szName = pTag->m_tag.m_var.m_szName + strlen(pTag->m_tag.m_var.m_szObjName) + 1;
        }
        else
        {
            pTag->m_tag.m_var.m_szObjName = NULL;
        }
    }

    pTag->m_tag.m_var.m_ucSysAttr = 0;
    if (szAttr)
    {
        string strUsrAttr;
        for (CWX_UINT16 i=0; i<strlen(szAttr); i++)
        {
            if (('c'==szAttr[i]) || ('C'==szAttr[i]) )
            {
                pTag->m_tag.m_var.m_ucSysAttr |= CwxHtmlTmplValSysAttr::ATTR_CHARSET_CONVERT;
            }
            else if (('r'==szAttr[i]) || ('R'==szAttr[i]) )
            {
                pTag->m_tag.m_var.m_ucSysAttr |= CwxHtmlTmplValSysAttr::ATTR_REPLACE;
            }
            else if (('e'==szAttr[i]) || ('E'==szAttr[i]) )
            {
                pTag->m_tag.m_var.m_ucSysAttr |= CwxHtmlTmplValSysAttr::ATTR_EXPLICT;
            }
            else
            {
                strUsrAttr += szAttr[i];
            }
        }
        if (strUsrAttr.length())
        {
            pTag->m_tag.m_var.m_szUserAttr = m_memPool.malloc(strUsrAttr.length() + 1);
            strcpy((char*)pTag->m_tag.m_var.m_szUserAttr, strUsrAttr.c_str());
        }
    }
    if (szLength)
    {
        pTag->m_tag.m_var.m_iDataLen = atoi(szLength);
    }
    else
    {
        pTag->m_tag.m_var.m_iDataLen = 0;
    }
    if (szEscape)
    {
        CWX_UINT32 uiEscapeLen = strlen(szEscape);
        if (!uiEscapeLen || (strcasecmp(szEscape, "none")==0) || ((1==uiEscapeLen) && ('0'==szEscape[0])))
        {
            szEscape = NULL;
        }
    }
    if (szEscape)
    {
        pTag->m_tag.m_var.m_szEscape = m_memPool.malloc(strlen(szEscape) + 1);
        strcpy((char*)pTag->m_tag.m_var.m_szEscape, szEscape);
        pTag->m_tag.m_var.m_pEscapeObj = getEscape(pTag->m_tag.m_var.m_szEscape);
        if (!pTag->m_tag.m_var.m_pEscapeObj)
        {
            CwxCommon::snprintf(m_szTmpBuf, m_uiTmpBufLen, "Line[%u], val format command's escape[%s]  doesn't exist, val format command:%s\n", m_uiLineBegin, szEscape, m_strLine.c_str());
            m_strErr = m_szTmpBuf;
            return false;
        }
    }
    else
    {
        pTag->m_tag.m_var.m_szEscape = NULL;
        pTag->m_tag.m_var.m_pEscapeObj = NULL;
    }

    if (szDefault)
    {
        pTag->m_tag.m_var.m_szDefault = m_memPool.malloc(strlen(szDefault) + 1);
        strcpy((char*)pTag->m_tag.m_var.m_szDefault, szDefault);
    }
    else
    {
        pTag->m_tag.m_var.m_szDefault = NULL;
    }

    if (szArgument)
    {
        pTag->m_tag.m_var.m_szArgument = m_memPool.malloc(strlen(szArgument) + 1);
        strcpy((char*)pTag->m_tag.m_var.m_szArgument, szArgument);
    }
    else
    {
        pTag->m_tag.m_var.m_szArgument = NULL;
    }

    return true;
}


CWINUX_END_NAMESPACE
