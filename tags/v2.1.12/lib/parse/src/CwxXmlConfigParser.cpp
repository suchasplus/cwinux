#include "CwxXmlConfigParser.h"
CWINUX_BEGIN_NAMESPACE


CwxXmlConfigParser::CwxXmlConfigParser(CWX_UINT32 uiAvgTokenLen, CWX_UINT32 uiAvgXmlSize)
:m_memPool(uiAvgTokenLen*4, uiAvgXmlSize) 
{
    m_root = NULL;
    m_pCur = NULL;
    m_szGbkBuf = NULL;
    m_uiGbkBufLen = 0;
    m_expatParser = NULL;
}

CwxXmlConfigParser::~CwxXmlConfigParser(void)
{
    if(m_expatParser)  XML_ParserFree(m_expatParser);
    m_expatParser = NULL;
    if (m_root) delete m_root;
    m_root = NULL;
    if (m_szGbkBuf) delete [] m_szGbkBuf;
    m_szGbkBuf = NULL;
}

bool CwxXmlConfigParser::parse(char const* szXml)
{
    /* Allocate a new parser state-object */
    if (m_expatParser) XML_ParserFree(m_expatParser);
    m_expatParser = XML_ParserCreate(NULL);
    XML_SetUserData(m_expatParser, (void*)this);
    regDefHandlers();

    if (m_root) delete m_root;
    m_root = NULL;
    m_pCur = NULL;
    m_memPool.reset();
    m_bGbk = false;
    //parse
    if (szXml)
    {
        XML_Status local_status =  XML_Parse(m_expatParser, szXml, strlen(szXml), XML_TRUE);
        if(local_status != XML_STATUS_OK)
        {
            return false;
        }
    }
    return true;
}

///get a attribute value for a element specified by szPath which's format is e1:e2:...
char const* CwxXmlConfigParser::getElementAttr(char const* szPath, char const* szAttr) const
{
    CwxXmlTreeNode const * pNode = getElementNode(szPath);
    if (pNode && szAttr)
    {
        list<pair<char*, char*> >::const_iterator iter = pNode->m_lsAttrs.begin();
        while(iter != pNode->m_lsAttrs.end())
        {
            if (strcmp(szAttr, iter->first) == 0)
            {
                return iter->second;
            }
            iter++;
        }
    }
    return NULL;
    
}
bool CwxXmlConfigParser::getElementAttrs(char const* szPath, list<pair<char*, char*> >& attrs) const
{
    CwxXmlTreeNode const * pNode = getElementNode(szPath);
    attrs.clear();
    if (pNode)
    {
        list<pair<char*, char*> >::const_iterator iter = pNode->m_lsAttrs.begin();
        while(iter != pNode->m_lsAttrs.end())
        {
            attrs.push_front(*iter);
            iter++;
        }
        return true;
    }
    return false;
}

bool CwxXmlConfigParser::getElementData(char const* szPath, string& strData) const
{
    CwxXmlTreeNode const * pNode = getElementNode(szPath);
    if (pNode && !pNode->m_listData.empty() && pNode->m_lsAttrs.empty() && !pNode->m_pChildHead)
    {
        list<char*> ::const_iterator iter = pNode->m_listData.begin();
        strData.clear();
        while(iter != pNode->m_listData.end())
        {
            strData += *iter;
            iter++;
        }
        return true;
    }
    return false;
}
///get a path's element node, path's format is e1:e2:...
CwxXmlTreeNode const* CwxXmlConfigParser::getElementNode(char const* szPath) const
{
    char const *elem_name_start = szPath;
    char const *elem_name_end = NULL;
    CWX_UINT32 uiLen = 0;
    CwxXmlTreeNode* pNode = m_root;
    while (elem_name_start && pNode)
    {
        elem_name_end = strchr(elem_name_start, ':');
        if ( elem_name_end == NULL)
        {
            uiLen = strlen(elem_name_start);
        }
        else
        {
            uiLen = elem_name_end - elem_name_start;
            elem_name_end++;
        }
        while(pNode)
        {
            if ((strlen(pNode->m_szElement) != uiLen)||
                (0 != memcmp(elem_name_start, pNode->m_szElement, uiLen)))
            {
                pNode = pNode->m_next;
            }
            else
            {
                break;
            }
        }
        if (!pNode) return NULL;
        elem_name_start = elem_name_end;
        if (elem_name_start) pNode = pNode->m_pChildHead;
    }
    return pNode;

}


void CwxXmlConfigParser::startElement(const XML_Char *name, const XML_Char **atts) 
{
    CWX_UINT32 uiAttrNum = 0;
    CWX_UINT32 uiLen = 0;
    pair<char*, char*> item;
    CwxXmlTreeNode* pNode = new CwxXmlTreeNode();
    char const* pName = NULL;
    if (m_bGbk)
    {
        pName = charsetValue(name, strlen(name));
    }
    else
    {
        pName = name;
    }
    uiLen = strlen(pName);
    pNode->m_szElement = m_memPool.malloc(uiLen + 1);
    memcpy(pNode->m_szElement, pName, uiLen);
    pNode->m_szElement[uiLen] = 0x00;
    while(atts[uiAttrNum*2])
    {
        if (m_bGbk)
        {
            pName = charsetValue(atts[uiAttrNum*2], strlen(atts[uiAttrNum*2]));
        }
        else
        {
            pName = atts[uiAttrNum*2];
        }
        uiLen = strlen(pName);
        item.first = m_memPool.malloc(uiLen + 1);
        memcpy(item.first, pName, uiLen);
        item.first[uiLen] = 0x00;
        if (m_bGbk)
        {
            pName = charsetValue(atts[uiAttrNum*2 + 1], strlen(atts[uiAttrNum*2 + 1]));
        }
        else
        {
            pName = atts[uiAttrNum*2 + 1];
        }
        uiLen = strlen(pName);
        item.second = m_memPool.malloc(uiLen + 1);
        memcpy(item.second, pName, uiLen);
        item.second[uiLen] = 0x00;
        pNode->m_lsAttrs.push_back(item);
        uiAttrNum++;
    }
    if (!m_pCur)
    {
        m_root = pNode;
        m_pCur = pNode;
    }
    else
    {
        pNode->m_pParent = m_pCur;
        if (!m_pCur->m_pChildTail)
        {
            m_pCur->m_pChildTail = m_pCur->m_pChildHead = pNode;
        }
        else
        {//add to tail for child
            m_pCur->m_pChildTail->m_next = pNode;
            pNode->m_prev = m_pCur->m_pChildTail;
            m_pCur->m_pChildTail = pNode;
        }
    }
    m_pCur = pNode;
}

void CwxXmlConfigParser::endElement(const XML_Char* )
{
    m_pCur = m_pCur->m_pParent;
}

void CwxXmlConfigParser::characterData(const XML_Char *s, int len)
{
    char const* pName = NULL;
    CWX_UINT32 uiLen = len;
    if (isGbk())
    {
        pName = charsetValue(s, len);
        uiLen = strlen(pName);
    }
    else
    {
        pName = s;
    }
    char* pData = m_memPool.malloc(uiLen + 1);
    memcpy(pData, pName, uiLen);
    pData[uiLen] = 0x00;
    m_pCur->m_listData.push_back(pData);
}

char const* CwxXmlConfigParser::charsetValue(XML_Char const* value, CWX_UINT32 uiValueLen)
{
    if (uiValueLen >= m_uiGbkBufLen)
    {
        if (m_szGbkBuf) delete [] m_szGbkBuf;
        m_uiGbkBufLen = (uiValueLen + 1024)/1024;
        m_uiGbkBufLen *= 1024;
        m_szGbkBuf = new char[m_uiGbkBufLen];
    }
    if (!m_bGbk)
    {
        memcpy(m_szGbkBuf, value, uiValueLen);
        m_szGbkBuf[uiValueLen] = 0x00;
    }
    else
    {
        CWX_UINT32 uiGbkLen = m_uiGbkBufLen;
        CwxGbkUnicodeMap::utf8ToGbk(value, uiValueLen, m_szGbkBuf, uiGbkLen);
        m_szGbkBuf[uiGbkLen] = 0x00;
    }
    return m_szGbkBuf;
}

/* 
This function causes Expat to register this's default static handlers
with the Expat events.
*/
void CwxXmlConfigParser::regDefHandlers(void)
{
    XML_SetElementHandler(m_expatParser,
        &CwxXmlConfigParser::elementStartHandler,
        &CwxXmlConfigParser::elementEndHandler);
    XML_SetCharacterDataHandler(m_expatParser,
        &CwxXmlConfigParser::characterDataHandler);
    XML_SetUnknownEncodingHandler(m_expatParser, &CwxXmlConfigParser::encodingHandler, this);
}


void XMLCALL CwxXmlConfigParser::elementStartHandler(void *userData, const XML_Char *name,
                                               const XML_Char **atts)
{
    CwxXmlConfigParser *me = (CwxXmlConfigParser*)userData;
    if(me != NULL)
    {
        me->startElement(name, atts);
    }
}

void XMLCALL CwxXmlConfigParser::elementEndHandler(void *userData, const XML_Char *name)
{
    CwxXmlConfigParser *me = (CwxXmlConfigParser*)userData;
    if(me != NULL)
    {
        me->endElement(name);
    }
}

void XMLCALL CwxXmlConfigParser::characterDataHandler(void *userData, const XML_Char *s, int len)
{
    CwxXmlConfigParser *me = (CwxXmlConfigParser*)userData;
    if(me != NULL) me->characterData(s, len);
}

int XMLCALL CwxXmlConfigParser::convert(void* , char const* s)
{
    CWX_UINT16 unGbk = s[0];
    unGbk <<= 8;
    unGbk += (CWX_UINT8)s[1];
    return CwxGbkUnicodeMap::gbkToUtf16(unGbk);
}
int  XMLCALL CwxXmlConfigParser::encodingHandler(void* userData, XML_Char const* name, XML_Encoding* info)
{
    int i;
    if (!name) return XML_STATUS_ERROR;
    if((0==strcasecmp(name,"gb2312")) || (0 == strcasecmp(name,"gbk")))
    {
        CwxXmlConfigParser *me = (CwxXmlConfigParser*)userData;
        me->m_bGbk = true;
        for(i=0;i<128;i++) info->map[i]	= i;
        for(;i<256;i++) info->map[i]= -2;
        info->convert = convert;
        info->release = NULL;
        return XML_STATUS_OK;
    }
    return XML_STATUS_ERROR;
}

CWINUX_END_NAMESPACE

