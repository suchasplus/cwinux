#include "CwxHtmlTmplCmp.h"
#include <stdlib.h>
#include <strings.h>
#include <string.h>
CWINUX_BEGIN_NAMESPACE

bool CwxHtmlTmplCmpEqual::compare(char const* szSrc,
             char const* szDest,
             char const* args,
             CwxHtmlTmpl const* ,
             CwxHtmlTmplEnv* )
{
    if (!szSrc) szSrc = "";
    if (!szDest) szDest="";
    if (!args) args="";
    if (strcasecmp(args, "casechar") == 0)
    {
        return strcasecmp(szSrc, szDest)==0?true:false;
    }
    else if (strcasecmp(args, "int") == 0)
    {
        return strtoll(szSrc, NULL, 0) == strtoll(szDest, NULL,0)?true:false;
    }
    else if (strcasecmp(args, "uint") == 0)
    {
        return strtoull(szSrc, NULL, 0) == strtoull(szDest, NULL,0)?true:false;
    }
    else if (strcasecmp(args, "length") == 0)
    {
        return strlen(szSrc)==strtoul(szDest, NULL, 0)?true:false;
    }
    //char
    return strcmp(szSrc, szDest)==0?true:false;
}


bool CwxHtmlTmplCmpLess::compare(char const* szSrc,
                                  char const* szDest,
                                  char const* args,
                                  CwxHtmlTmpl const* ,
                                  CwxHtmlTmplEnv* )
{
    if (!szSrc) szSrc = "";
    if (!szDest) szDest="";
    if (!args) args="";
    if (strcasecmp(args, "casechar") == 0)
    {
        return strcasecmp(szSrc, szDest)<0?true:false;
    }
    else if (strcasecmp(args, "int") == 0)
    {
        return strtoll(szSrc, NULL, 0) < strtoll(szDest, NULL,0)?true:false;
    }
    else if (strcasecmp(args, "uint") == 0)
    {
        return strtoull(szSrc, NULL, 0) < strtoull(szDest, NULL,0)?true:false;
    }
    else if (strcasecmp(args, "length") == 0)
    {
        return strlen(szSrc)<strtoul(szDest, NULL, 0)?true:false;
    }
    //char
    return strcmp(szSrc, szDest)<0?true:false;
}


bool CwxHtmlTmplCmpMore::compare(char const* szSrc,
                                 char const* szDest,
                                 char const* args,
                                 CwxHtmlTmpl const* ,
                                 CwxHtmlTmplEnv* )
{
    if (!szSrc) szSrc = "";
    if (!szDest) szDest="";
    if (!args) args="";
    if (strcasecmp(args, "casechar") == 0)
    {
        return strcasecmp(szSrc, szDest)>0?true:false;
    }
    else if (strcasecmp(args, "int") == 0)
    {
        return strtoll(szSrc, NULL, 0) > strtoll(szDest, NULL,0)?true:false;
    }
    else if (strcasecmp(args, "uint") == 0)
    {
        return strtoull(szSrc, NULL, 0) > strtoull(szDest, NULL,0)?true:false;
    }
    else if (strcasecmp(args, "length") == 0)
    {
        return strlen(szSrc) > strtoul(szDest, NULL, 0)?true:false;
    }
    //char
    return strcmp(szSrc, szDest)>0?true:false;
}


bool CwxHtmlTmplCmpPrefix::compare(char const* szSrc,
             char const* szDest,
             char const* args,
             CwxHtmlTmpl const* ,
             CwxHtmlTmplEnv* )
{
    if (!szSrc) szSrc = "";
    if (!szDest) szDest="";
    if (!args) args="";
    if (strcasecmp(args, "case") == 0)
    {
        return strncasecmp(szSrc, szDest, strlen(szDest))==0?true:false;
    }
    return strncmp(szSrc, szDest, strlen(szDest))==0?true:false;
}

bool CwxHtmlTmplCmpRPrefix::compare(char const* szSrc,
                                   char const* szDest,
                                   char const* args,
                                   CwxHtmlTmpl const* ,
                                   CwxHtmlTmplEnv* )
{
    if (!szSrc) szSrc = "";
    if (!szDest) szDest="";
    if (!args) args="";
    if (strcasecmp(args, "case") == 0)
    {
        return strncasecmp(szDest, szSrc, strlen(szSrc))==0?true:false;
    }
    return strncmp(szDest, szSrc, strlen(szSrc))==0?true:false;
}

bool CwxHtmlTmplCmpSuffix::compare(char const* szSrc,
                                    char const* szDest,
                                    char const* args,
                                    CwxHtmlTmpl const* ,
                                    CwxHtmlTmplEnv* )
{
    if (!szSrc) szSrc = "";
    if (!szDest) szDest="";
    if (!args) args="";
    CWX_UINT32 uiSrcLen = strlen(szSrc);
    CWX_UINT32 uiDestLen = strlen(szDest);
    if ( uiSrcLen < uiDestLen) return false;
    if (strcasecmp(args, "case") == 0)
    {
        return strncasecmp(szSrc + (uiSrcLen - uiDestLen), szDest, uiDestLen)==0?true:false;
    }
    return strncmp(szSrc + (uiSrcLen - uiDestLen), szDest, uiDestLen)==0?true:false;
}

bool CwxHtmlTmplCmpRSuffix::compare(char const* szSrc,
                                   char const* szDest,
                                   char const* args,
                                   CwxHtmlTmpl const* ,
                                   CwxHtmlTmplEnv* )
{
    if (!szSrc) szSrc = "";
    if (!szDest) szDest="";
    if (!args) args="";
    CWX_UINT32 uiSrcLen = strlen(szSrc);
    CWX_UINT32 uiDestLen = strlen(szDest);
    if ( uiDestLen < uiSrcLen ) return false;
    if (strcasecmp(args, "case") == 0)
    {
        return strncasecmp(szDest + (uiDestLen - uiSrcLen), szSrc, uiSrcLen)==0?true:false;
    }
    return strncmp(szDest + (uiDestLen - uiSrcLen), szSrc, uiSrcLen)==0?true:false;
}


bool CwxHtmlTmplCmpIn::compare(char const* szSrc,
                                    char const* szDest,
                                    char const* args,
                                    CwxHtmlTmpl const* ,
                                    CwxHtmlTmplEnv* )
{
    if (!szSrc) szSrc = "";
    if (!szDest) szDest="";
    if (!args) args="";
    if (strcasecmp(args, "case") == 0)
    {
        return strcasestr(szSrc, szDest)?true:false;
    }
    return strstr(szSrc, szDest)?true:false;
}

bool CwxHtmlTmplCmpRIn::compare(char const* szSrc,
                               char const* szDest,
                               char const* args,
                               CwxHtmlTmpl const* ,
                               CwxHtmlTmplEnv* )
{
    if (!szSrc) szSrc = "";
    if (!szDest) szDest="";
    if (!args) args="";
    if (strcasecmp(args, "case") == 0)
    {
        return strcasestr(szDest, szSrc)?true:false;
    }
    return strstr(szDest, szSrc)?true:false;
}

CWINUX_END_NAMESPACE
