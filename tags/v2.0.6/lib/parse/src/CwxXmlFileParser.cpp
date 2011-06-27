#include <string.h>
#include <stdio.h>
#include <expat.h>
#include "CwxXmlFileParser.h"

CWINUX_BEGIN_NAMESPACE

CwxXmlFileParser::CwxXmlFileParser(string const& strFileName)
{
    m_fd = NULL;
    m_strFileName = strFileName;
}

CwxXmlFileParser::~CwxXmlFileParser() 
{
    if (m_fd) fclose(m_fd);
    m_fd = NULL;
}

bool CwxXmlFileParser::prepare()
{
    if (!CwxXmlParser::prepare()) return false;
    if (m_fd) fclose(m_fd);
    m_fd = fopen(m_strFileName.c_str(), "r");
    if (!m_fd)
    {
        setReady(false);
        return false;
    }
    return true;
}

ssize_t CwxXmlFileParser::readBlock(void)
{
    if (!isReady()) return -1;
    size_t size = fread(getBuf(), sizeof(XML_Char), getBufSize(), m_fd);
    ssize_t code = (ssize_t)size;
    if(size < getBufSize())
    {
        if(feof(m_fd))
        {
            setLastError(XML_ERROR_FINISHED);
            return size;
        }
        if(ferror(m_fd))
        {
            setStatus(XML_STATUS_ERROR);
            setLastError(XML_ERROR_NO_ELEMENTS);
        }
    }
    if(size == 0)
    {
        setStatus(XML_STATUS_OK);
        setLastError(XML_ERROR_FINISHED);
        code = -1;
    }
    return code;
}

CWINUX_END_NAMESPACE

