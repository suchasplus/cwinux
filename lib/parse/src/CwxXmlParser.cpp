
#include <string.h>
#include <stdio.h>
#include <expat.h>
#include "CwxXmlParser.h"

CWINUX_BEGIN_NAMESPACE

CwxXmlParser::CwxXmlParser(CWX_UINT32 uiBufSize) 
{
    /* Initialize the validity of this parser to OFF */
    m_isReady = false;
    m_expatParser = NULL;
    /* Allocate a buffer for streaming in data */
    m_uiBufSize = uiBufSize;
    m_szGbkBuf = NULL;
    m_uiGbkBufLen = 0;

    /* Allocate a buffer for streaming in data */
    m_szXmlBuf = new XML_Char[m_uiBufSize];
    memset(m_szXmlBuf, 0, m_uiBufSize);

    m_szXmlPath = new XML_Char[PATH_BUF_ALIGN];
    m_uiPathBufLen = PATH_BUF_ALIGN;
    m_uiPathLen = 0;

}

CwxXmlParser::~CwxXmlParser(void) 
{
    m_isReady = false;
    if(m_expatParser)  XML_ParserFree(m_expatParser);
    m_expatParser = NULL;

    if(m_szXmlBuf) delete [] m_szXmlBuf;
    m_szXmlBuf = NULL;

    if (m_szXmlPath) delete [] m_szXmlPath;
    m_szXmlPath = NULL;

    if (m_szGbkBuf) delete [] m_szGbkBuf;
    m_szGbkBuf = NULL;
}


bool CwxXmlParser::parse()
{
    ssize_t bytes_read;
    /* Ensure that the parser is ready */
    if(!prepare()) return false;
    m_bGbk = false;
    /* Loop, reading the XML source block by block */
    while((bytes_read = readBlock()) >= 0)
    {
        if(bytes_read > 0)
        {
            XML_Status local_status =  XML_Parse(m_expatParser, getBuf(), bytes_read, XML_FALSE);
            if(local_status != XML_STATUS_OK)
            {
                m_status = local_status;
                m_lastError = XML_GetErrorCode(m_expatParser);
                break;
            }
            /* Break on successful "short read", in event of EOF */
            if(getLastError() == XML_ERROR_FINISHED) break;
        }
    }
    /* Finalize the parser */
    if((getStatus() == XML_STATUS_OK) || (getLastError() == XML_ERROR_FINISHED)) 
    {
        XML_Parse(m_expatParser, NULL, 0, XML_TRUE);
        return true;
    }
    /* Return false in the event of an error. The parser is not finalized on error. */
    return false;
}

bool CwxXmlParser::prepare()
{
    /* Initialize the validity of this parser to OFF */
    m_isReady = false;
    if (m_expatParser)
    {
        XML_ParserFree(m_expatParser);
        m_expatParser = NULL;
    }

    /* Allocate a new parser state-object */
    m_expatParser = XML_ParserCreate(NULL);
    /* Set the "ready" flag on this parser */
    m_isReady = true;
    XML_SetUserData(m_expatParser, (void*)this);
    regDefHandlers();

    return true;
}

char const* CwxXmlParser::charsetValue(XML_Char const* value, CWX_UINT32 uiValueLen)
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


void CwxXmlParser::startElement(const XML_Char *, const XML_Char **)
{
}

void CwxXmlParser::endElement(const XML_Char *) 
{
}

void CwxXmlParser::characterData(const XML_Char *, int ) 
{
}

void CwxXmlParser::processingInstruction(const XML_Char *, const XML_Char *)
{
}

void CwxXmlParser::commentData(const XML_Char *) 
{
}

void CwxXmlParser::defaultHandler(const XML_Char *, int )
{
}

void CwxXmlParser::startCData(void)
{
}

void CwxXmlParser::endCData(void)
{
}

/* 
This function causes Expat to register this's default static handlers
with the Expat events.
*/
void CwxXmlParser::regDefHandlers(void)
{
    XML_SetElementHandler(m_expatParser,
        &CwxXmlParser::elementStartHandler,
        &CwxXmlParser::elementEndHandler);
    XML_SetCharacterDataHandler(m_expatParser,
        &CwxXmlParser::characterDataHandler);
    XML_SetProcessingInstructionHandler(m_expatParser,
        &CwxXmlParser::processingInstrHandler);
    XML_SetCommentHandler(m_expatParser, &CwxXmlParser::commentHandler);
    XML_SetCdataSectionHandler(m_expatParser,
        &CwxXmlParser::startCDatahandler,
        &CwxXmlParser::endCDatahandler);
    XML_SetDefaultHandler(m_expatParser, &CwxXmlParser::defaultHandler);
    XML_SetUnknownEncodingHandler(m_expatParser, &CwxXmlParser::encodingHandler, this);
}

ssize_t CwxXmlParser::readBlock(void)
{
    m_lastError = XML_ERROR_NO_ELEMENTS;
    m_status = XML_STATUS_ERROR;
    return -1;
}

/* 
**** INTERNAL HANDLER FUNCTIONS *****
The expatmm protocol is to pass (this) as the userData argument
in the XML_Parser structure. These static methods will convert 
handlers into upcalls to the instantiated class's virtual members
to do the actual handling work.
*/
void XMLCALL CwxXmlParser::elementStartHandler(void *userData, const XML_Char *name,
                                               const XML_Char **atts)
{
    CwxXmlParser *me = (CwxXmlParser*)userData;
    CWX_UINT32 uiNameLen = strlen(name);
    if(me != NULL)
    {
        if (me->m_uiPathLen + uiNameLen + 1 >= me->m_uiPathBufLen)
        {
            me->m_uiPathBufLen = ((me->m_uiPathBufLen + uiNameLen + PATH_BUF_ALIGN -1)/PATH_BUF_ALIGN)*PATH_BUF_ALIGN * 2;
            XML_Char* buf = new XML_Char[me->m_uiPathBufLen];
            memcpy(buf, me->m_szXmlPath, me->m_uiPathLen);
            delete [] me->m_szXmlPath;
            me->m_szXmlPath = buf;
        }
        if (me->m_uiPathLen)
        {
            me->m_szXmlPath[me->m_uiPathLen]=':';
            me->m_uiPathLen++;
        }
        memcpy(me->m_szXmlPath + me->m_uiPathLen, name, uiNameLen);
        me->m_uiPathLen += uiNameLen;
        me->m_szXmlPath[me->m_uiPathLen] = 0x00;
        me->startElement(name, atts);
    }
}

void XMLCALL CwxXmlParser::elementEndHandler(void *userData, const XML_Char *name)
{
    CwxXmlParser *me = (CwxXmlParser*)userData;
    if(me != NULL)
    {
        CWX_UINT32 i =0;
        for (i=me->m_uiPathLen; i>0; i--)
        {
            if (me->m_szXmlPath[i-1] == ':')
            {
                i--;
                break;
            }
        }
        me->m_szXmlPath[i] = 0x00;
        me->m_uiPathLen = i;
        me->endElement(name);
    }
}

void XMLCALL CwxXmlParser::characterDataHandler(void *userData, const XML_Char *s, int len)
{
    CwxXmlParser *me = (CwxXmlParser*)userData;
    if(me != NULL) me->characterData(s, len);
}

void XMLCALL CwxXmlParser::processingInstrHandler(void *userData, const XML_Char *target, const XML_Char *data)
{
    CwxXmlParser *me = (CwxXmlParser*)userData;
    if(me != NULL) me->processingInstruction(target, data);
}

void XMLCALL CwxXmlParser::commentHandler(void *userData, const XML_Char *data)
{
    CwxXmlParser *me = (CwxXmlParser*)userData;
    if(me != NULL) me->commentData(data);
}

void XMLCALL CwxXmlParser::defaultHandler(void *userData, const XML_Char *s, int len)
{
    CwxXmlParser *me = (CwxXmlParser*)userData;
    if(me != NULL) me->defaultHandler(s, len);
}

void XMLCALL CwxXmlParser::startCDatahandler(void *userData)
{
    CwxXmlParser *me = (CwxXmlParser*)userData;
    if(me != NULL) me->startCData();
}

void XMLCALL CwxXmlParser::endCDatahandler(void *userData)
{
    CwxXmlParser *me = (CwxXmlParser*)userData;
    if(me != NULL) me->endCData();
}

int XMLCALL CwxXmlParser::convert(void* , char const* s)
{
    CWX_UINT16 unGbk = s[0];
    unGbk <<= 8;
    unGbk += (CWX_UINT8)s[1];
    return CwxGbkUnicodeMap::gbkToUtf16(unGbk);
}
int  XMLCALL CwxXmlParser::encodingHandler(void* userData, XML_Char const* name, XML_Encoding* info)
{
    int i;
    if (!name) return XML_STATUS_ERROR;
    if((0==strcasecmp(name,"gb2312")) || (0 == strcasecmp(name,"gbk")))
    {
        CwxXmlParser *me = (CwxXmlParser*)userData;
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


