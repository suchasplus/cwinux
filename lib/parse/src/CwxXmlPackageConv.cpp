#include "CwxXmlPackageConv.h"

CWINUX_BEGIN_NAMESPACE
bool CwxXmlPackageConv::xmlToPackage(char const * szSrc,
                                     char* szOut,
                                     CWX_UINT32& uiOutLen)
{
    CwxXmlConfigParser parser;
    if (!parser.parse(szSrc))
    {
        CwxCommon::snprintf(this->m_szErrMsg, 511, "%s", "Failure to parse xml");
        return false;
    }
    CwxXmlTreeNode const* pRoot = parser.getRoot();
    if (pRoot)
    {
        return xmlToPackage(pRoot, szOut, uiOutLen);
    }
    else
    {
        strcpy(this->m_szErrMsg, "Failure to parse xml.");
        return false;
    }
    return true;
}

bool CwxXmlPackageConv::packageToXml(char const* szRootName,
                  char const * szSrc,
                  CWX_UINT32 uiSrcLen,
                  char* szOut,
                  CWX_UINT32& uiOutLen,
                  char const* szXmlTitile)
{
    CwxPackageReader package;
    if (!package.unpack(szSrc, uiSrcLen, false, true))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to unpack msg, err=%s", package.getErrMsg());
        return false;
    }
    return packageToXml(szRootName, package, szOut, uiOutLen, szXmlTitile);
}

bool CwxXmlPackageConv::packageToXml(char const* szRootName,
                  CwxPackageReader& package,
                  char* szOut,
                  CWX_UINT32& uiOutLen,
                  char const* szXmlTitile)
{
    CWX_UINT32 pos = 0;
    CWX_UINT32 uiTmp = 0;

    memset(szOut, 0x00, uiOutLen);

    if (!szXmlTitile)
    {
        if (m_xmlEncode->isGbk())
        {
            szXmlTitile = "<?xml version='1.0' encoding=\"gbk\" ?>";
        }
        else
        {
            szXmlTitile = "<?xml version='1.0' encoding=\"utf-8\" ?>";
        }
    }
    uiTmp = strlen(szXmlTitile);
    if (pos + uiTmp >= uiOutLen)
    {
        strcpy(this->m_szErrMsg, "Buf is too less.");
        return false;
    }
    memcpy(szOut + pos, szXmlTitile, uiTmp);
    pos += uiTmp;
    uiTmp = uiOutLen - pos;
    if (!packageToXmlNode(szRootName, package, szOut + pos, uiTmp)) return false;
    pos += uiTmp;
    uiOutLen = pos;
    szOut[uiOutLen] = 0x00;
    return true;
}

bool CwxXmlPackageConv::packageToXmlNode(char const* szNodeName,
                      char const * szSrc,
                      CWX_UINT32 uiSrcLen,
                      char* szOut,
                      CWX_UINT32& uiOutLen)
{
    CwxPackageReader package;
    if (!package.unpack(szSrc, uiSrcLen, false, true))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to unpack msg, err=%s", package.getErrMsg());
        return false;
    }
    return packageToXmlNode(szNodeName, package, szOut, uiOutLen);
}

bool CwxXmlPackageConv::packageToXmlNode(char const* szNodeName,
                      CwxPackageReader& package,
                      char* szOut,
                      CWX_UINT32& uiOutLen)
{
    CwxKeyValueItem const* pItem;
    CWX_UINT32 pos = 0;
    CWX_UINT32 uiTmp = 0;
    memset(szOut, 0x00, uiOutLen);
    uiTmp = uiOutLen - pos;
    if (szNodeName)
    {
        if (!appendXmlKeyBegin(szNodeName, strlen(szNodeName), szOut + pos, uiTmp)) return false;
        pos += uiTmp;
    }
    for (CWX_UINT32 i=0; i<package.getKeyNum(); i++)
    {
        pItem = package.getKey(i);
        uiTmp = uiOutLen - pos;
        if (!packageToXml(*pItem, szOut + pos, uiTmp)) return false;
        pos += uiTmp;
    }
    if (szNodeName)
    {
        uiTmp = uiOutLen - pos;
        if (!appendXmlKeyEnd(szNodeName, strlen(szNodeName), szOut + pos, uiTmp)) return false;
        pos += uiTmp;
    }
    uiOutLen = pos;
    szOut[uiOutLen] = 0x00;
    return true;
}


///xml转换为package
bool CwxXmlPackageConv::xmlToPackage(CwxXmlTreeNode const * treeNode, char* szOut, CWX_UINT32& uiOutLen)
{
    CWX_UINT32 pos = 4;
    CWX_UINT16 key_len=0;
    CWX_UINT32 data_len=0;
    CWX_UINT32 byte4;
    CWX_UINT16 byte2;
    int       ret;
    list<char*>::const_iterator text_iter;
    list<pair<char*, char*> >::const_iterator attr_iter;
    CwxXmlTreeNode const* child;
    bool bKeyValue = false;
    if (!treeNode)
    {
        uiOutLen = 0;
        return true;
    }
    //add key
    key_len = strlen(treeNode->m_szElement);
    if (uiOutLen < pos + CwxPackage::getKvLen(key_len, 0))
    {
        strcpy(this->m_szErrMsg, "Package's buf is too small.");
        return false;
    }
    //add key len
    byte2 = CWX_HTONS(key_len);
    memcpy(szOut + pos, &byte2, sizeof(byte2));
    pos +=2;
    //append key with null
    memcpy(szOut + pos, treeNode->m_szElement, key_len + 1);
    pos += key_len + 1;

    if ((treeNode->m_listData.begin() != treeNode->m_listData.end()) &&
        (treeNode->m_lsAttrs.begin() == treeNode->m_lsAttrs.end()) &&
        !treeNode->m_pChildHead)
    {//it's [<name>xxxxx</name>]'s item
        bKeyValue = false;
        text_iter = treeNode->m_listData.begin();
        while(text_iter != treeNode->m_listData.end())
        {
            data_len = strlen(*text_iter);
            if (uiOutLen < pos + data_len + 1)
            {
                strcpy(this->m_szErrMsg, "Buf is too small.");
                return false;
            }
            memcpy(szOut + pos, *text_iter, data_len);
            pos += data_len;
            text_iter++;
        }
        szOut[pos++]=0x00;//append the \0
    }
    else
    {
        bKeyValue = true;
        //add attr
        attr_iter = treeNode->m_lsAttrs.begin();
        while(attr_iter != treeNode->m_lsAttrs.end())
        {
            ret = CwxPackage::appendKey(szOut + pos, uiOutLen-pos, attr_iter->first, attr_iter->second, strlen(attr_iter->second), false);
            if (-1 == ret)
            {
                strcpy(this->m_szErrMsg, "Buf is too small.");
                return false;
            }
            pos += ret;
            attr_iter ++;
        }
        //add child
        child = treeNode->m_pChildHead;
        if (child)
        {
            while(child)
            {
                data_len = uiOutLen - pos;
                if (!xmlToPackage(child, szOut + pos, data_len)) return false;
                pos += data_len;
                child = child->m_next;
            }
        }
        szOut[pos++] = 0x00;
    }
    if (bKeyValue)
    {
        byte4 = CWX_HTONL(pos + 0x80000000);
    }
    else
    {
        byte4 = CWX_HTONL(pos);
    }
    memcpy(szOut, &byte4, 4);
    uiOutLen = pos;
    return true;
}

///package转换为xml
bool CwxXmlPackageConv::packageToXml(CwxKeyValueItem const& item, char* szOut, CWX_UINT32& uiOutLen)
{
    CWX_UINT32 pos = 0;
    CWX_UINT32 left = uiOutLen;
    CWX_UINT32 in_len = 0;
    CWX_UINT32 out_len = 0;
    CwxKeyValueItem const* pItem;
    CwxPackageReader package;
    if (NULL == item.m_szData)
    {
        CwxCommon::snprintf(this->m_szErrMsg, 511, "key[%s]'s value is null.", item.m_szKey);
        return false;
    }
    if (!item.m_bKeyValue)
    {
        //add [<]
        out_len = left;
        if (!appendXmlKeyBegin(item.m_szKey, item.m_unKeyLen, szOut + pos, out_len)) return false;
        pos += out_len;
        left -= out_len;
        //append data
        in_len = item.m_uiDataLen;
        out_len = left;
        if (!m_xmlEncode->encode(item.m_szData, in_len, szOut + pos, out_len))
        {
            CwxCommon::snprintf(m_szErrMsg, 511, "Failure to xml-encode data:%s", item.m_szData);
            return false;
        }
        if (in_len != item.m_uiDataLen)
        {
            strcpy(this->m_szErrMsg, "Buff is to small.");
            return false;
        }
        pos+= out_len;
        left-=out_len;

        //add </key>\n
        out_len = left;
        if (!appendXmlKeyEnd(item.m_szKey, item.m_unKeyLen, szOut + pos, out_len)) return false;
        pos += out_len;
        left -= out_len;
        uiOutLen = pos;
        szOut[pos]=0x00;
        return true;
    }
    //it's key value
    if (!package.unpack(item.m_szData, item.m_uiDataLen, false, true))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to unpack key-value, key=%s, err=%s", item.m_szKey, package.getErrMsg());
        return false;
    }
    //add <item.key>
    out_len = left;
    if (!appendXmlKeyBegin(item.m_szKey, item.m_unKeyLen, szOut + pos, out_len)) return false;
    pos += out_len;
    left -= out_len;

    for (CWX_UINT32 i=0; i<package.getKeyNum(); i++)
    {
        pItem = package.getKey(i);
        out_len = left;
        if (!packageToXml(*pItem, szOut + pos, out_len)) return false;
        pos+=out_len;
        left-=out_len;
    }

    //add </item.key>
    out_len = left;
    if (!appendXmlKeyEnd(item.m_szKey, item.m_unKeyLen, szOut + pos, out_len)) return false;
    pos += out_len;
    left -= out_len;
    uiOutLen = pos;
    szOut[pos]=0x00;
    return true;
}

///append <key>
bool CwxXmlPackageConv::appendXmlKeyBegin(char const* szKey, CWX_UINT16 unKeyLen, char* szOut, CWX_UINT32& uiOutLen)
{
    CWX_UINT32 left = uiOutLen;
    CWX_UINT32 pos = 0;
    CWX_UINT32 in_len = 0;
    CWX_UINT32 out_len = 0;
    //add [<]
    if (left < 2)
    {
        strcpy(this->m_szErrMsg, "Buff is to small.");
        return false;
    }
    szOut[pos++]='<';
    left--;
    //add key
    in_len = unKeyLen;
    out_len = left;
    if (!m_xmlEncode->encode(szKey, in_len, szOut + pos, out_len))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to xml-encode key:%s", szKey);
        return false;
    }
    if (in_len != unKeyLen)
    {
        strcpy(this->m_szErrMsg, "Buff is to small.");
        return false;
    }
    pos+= out_len;
    left-=out_len;
    //append other '>'
    if (left < 2)
    {
        strcpy(this->m_szErrMsg, "Buff is to small.");
        return false;
    }
    szOut[pos++]='>';
    uiOutLen = pos;
    return true;
}
///append </key>
bool CwxXmlPackageConv::appendXmlKeyEnd(char const* szKey, CWX_UINT16 unKeyLen, char* szOut, CWX_UINT32& uiOutLen)
{
    CWX_UINT32 left = uiOutLen;
    CWX_UINT32 pos = 0;
    CWX_UINT32 in_len = 0;
    CWX_UINT32 out_len = 0;
    //add [</]
    if (left < 3)
    {
        strcpy(this->m_szErrMsg, "Buff is to small.");
        return false;
    }
    szOut[pos++]='<';
    left--;
    szOut[pos++]='/';
    left--;
    //add key
    in_len = unKeyLen;
    out_len = left;
    if (!m_xmlEncode->encode(szKey, in_len, szOut + pos, out_len))
    {
        CwxCommon::snprintf(m_szErrMsg, 511, "Failure to xml-encode key:%s", szKey);
        return false;
    }
    if (in_len != unKeyLen)
    {
        strcpy(this->m_szErrMsg, "Buff is to small.");
        return false;
    }
    pos+= out_len;
    left-=out_len;
    //append other '>\n'
    if (left < 3)
    {
        strcpy(this->m_szErrMsg, "Buff is to small.");
        return false;
    }
    szOut[pos++]='>';
    left--;
    szOut[pos++]='\n';
    left--;
    uiOutLen = pos;
    return true;
}



CWINUX_END_NAMESPACE
