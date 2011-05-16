#include <string.h>
#include <stdio.h>
#include <expat.h>
#include "CwxXmlFileConfigParser.h"

CWINUX_BEGIN_NAMESPACE

CwxXmlFileConfigParser::CwxXmlFileConfigParser(CWX_UINT32 uiAvgTokenLen, CWX_UINT32 uiAvgXmlSize)
:m_parser(uiAvgTokenLen, uiAvgXmlSize)
{
    m_fd = NULL;
    m_szBuf = NULL;
}

CwxXmlFileConfigParser::~CwxXmlFileConfigParser(void) 
{
    if (m_fd) fclose(m_fd);
    if (m_szBuf) delete []m_szBuf;
}

///Attempts to parse an entire XML source
bool CwxXmlFileConfigParser::parse(string const& strFileName)
{
    m_strFileName = strFileName;
    if (m_fd) fclose(m_fd);
    m_fd = fopen(m_strFileName.c_str(), "r");
    if (!m_fd) return false;
    fseek(m_fd, 0, SEEK_END);
    CWX_UINT32 uiFileSize = ftell(m_fd);
    if (m_szBuf) delete []m_szBuf;
    m_szBuf = new char[uiFileSize + 1];
    fseek(m_fd, 0, SEEK_SET);
    if (uiFileSize != fread(m_szBuf, 1, uiFileSize, m_fd))
    {
        delete []m_szBuf;
        return false;
    }
    m_szBuf[uiFileSize] = 0x00;
    bool bResult = m_parser.parse(m_szBuf);
    delete [] m_szBuf;
    m_szBuf = NULL;
    return bResult;
}

CWINUX_END_NAMESPACE

