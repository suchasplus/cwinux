#include "CwfFilterMgr.h"
#include "CwxFile.h"

///构造函数
CwfFilterMgr::CwfFilterMgr(CWX_UINT32 uiStopWord, CWX_UINT32 uiWord)
{
    m_uiStopWord = uiStopWord;
    m_uiKeyWord = uiWord;
    m_idFilterMap = NULL;
    m_nameFilterMap = NULL;
    m_bCaseSensive = true;
}
///析构函数
CwfFilterMgr::~CwfFilterMgr()
{
    reset();

}

///初始化filter管理器，0：成功；-1：失败
int CwfFilterMgr::init(CwfFilterConfig const& config, char* szErr2K)
{
    CWX_UINT32 uiFilterNum = config.m_fiters.size();
    //check filter num
    if (!uiFilterNum)
    {
        CwxCommon::snprintf(szErr2K, 2047, "No filter in config");
        return -1;
    }
    m_bCaseSensive = config.m_bCaseSensive;
    //create the filter index
    m_idFilterMap = new hash_map<CWX_UINT32/*id*/, CwxFilterBase*>(uiFilterNum * 2);
    m_nameFilterMap = new hash_map<string, CwxFilterBase*, CwxStringHash>(uiFilterNum * 2);
    //create all filters
    list<CwfFilterConfigFilter*>::const_iterator iter = config.m_fiters.begin();
    map<CWX_UINT8/*level*/, string/*file*/>::const_iterator iter_file;
    CwxFilterBase* pFilter = NULL;
    FILE* fd = NULL;
    string line;
    bool bRet = true;
    while(iter != config.m_fiters.end())
    {
        if (config.m_bUtf8)
        {
            pFilter = new CwxFilterMachine<CwxCharsetUtf8>(m_uiKeyWord, m_uiStopWord);
        }
        else
        {
            pFilter = new CwxFilterMachine<CwxCharsetGbk>(m_uiKeyWord, m_uiStopWord);
        }
        //检查指定id的过滤器是否存在
        if (m_idFilterMap->find((*iter)->m_uiId) != m_idFilterMap->end())
        {
            CwxCommon::snprintf(szErr2K, 2047, "Filter for id[%u] is duplicate.", (*iter)->m_uiId);
            return -1;
        }
        //检查指定名字的过滤器是否存在
        if (m_nameFilterMap->find((*iter)->m_strName) != m_nameFilterMap->end())
        {
            CwxCommon::snprintf(szErr2K, 2047, "Filter for name[%s] is duplicate.", (*iter)->m_strName.c_str());
            return -1;
        }
        (*m_idFilterMap)[(*iter)->m_uiId] =  pFilter;
        (*m_nameFilterMap)[(*iter)->m_strName] = pFilter;
        //load stopword
        fd = fopen((*iter)->m_strStopwordFile.c_str(), "rb");
        if (!fd)
        {
            CwxCommon::snprintf(szErr2K, 2047, "Failure to open stop-word-file:%s, errno=%d", (*iter)->m_strStopwordFile.c_str(), errno);
            return -1;
        }
        while(CwxFile::readTxtLine(fd, line))
        {
            if (line.empty()) break;
            if (m_bCaseSensive)
            {
                bRet = pFilter->addStopWord(line.c_str(), line.length());
            }
            else
            {
                bRet = pFilter->addCaseStopWord(line.c_str(), line.length());
            }
            if (!bRet)
            {
                CwxCommon::snprintf(szErr2K, 2047, "Failure to add stop-word[%s] in stop-word-file:%s", line.c_str(), (*iter)->m_strStopwordFile.c_str());
                fclose(fd);
                return -1;
            }
        }
        fclose(fd);
        //load filter word
        iter_file = (*iter)->m_levelFiles.begin();
        while(iter_file != (*iter)->m_levelFiles.end())
        {
            if ((iter_file->first>=1)&&(iter_file->first<=8))
            {
                //open file
                fd = fopen(iter_file->second.c_str(), "rb");
                if (!fd)
                {
                    CwxCommon::snprintf(szErr2K, 2047, "Failure to open filter-word file:%s", iter_file->second.c_str());
                    return -1;
                }
                while(CwxFile::readTxtLine(fd, line))
                {
                    if (line.empty()) break;
                    if (m_bCaseSensive)
                    {
                        bRet = pFilter->addFilterStr(line.c_str(), line.length(), 1<<(iter_file->first-1));
                    }
                    else
                    {
                        bRet = pFilter->addCaseFilterStr(line.c_str(), line.length(), 1<<(iter_file->first-1));
                    }
                    if (!bRet)
                    {
                        CwxCommon::snprintf(szErr2K, 2047, "Failure to add filter-word[%s] in word-file:%s", line.c_str(), iter_file->second.c_str());
                        fclose(fd);
                        return -1;
                    }
                }
                fclose(fd);
            }
            iter_file++;
        }
        iter++;
    }
    return 0;
}

///根据filter的id，filte content，-1：失败；0：成功
int CwfFilterMgr::filter(CWX_UINT32 uiFilterId,
                         char const* szContent,
                         CWX_UINT32 uiLen,
                         CWX_UINT8& ucLevel,
                         CWX_UINT32& uiWordPos,
                         CWX_UINT32& uiWordLen,
                         char* szErr2K)
{
    hash_map<CWX_UINT32/*id*/, CwxFilterBase*>::iterator iter = m_idFilterMap->find(uiFilterId);
    if (iter == m_idFilterMap->end())
    {
        CwxCommon::snprintf(szErr2K, 2047, "Filter id [%u] doesn't exist.", uiFilterId);
        return -1;
    }
    CwxFilterBase *pFilter  = iter->second;
    if (m_bCaseSensive)
    {
        ucLevel = pFilter->filterStr(szContent, uiLen, uiWordPos, uiWordLen);
    }
    else
    {
        ucLevel = pFilter->filterCaseStr(szContent, uiLen, uiWordPos, uiWordLen);
    }    
    ucLevel = level(ucLevel);
    return 0;
}
///根据filter的名字，filte content，-1：失败；0：成功
int CwfFilterMgr::filter(string strName,
                         char const* szContent, 
                         CWX_UINT32 uiLen,
                         CWX_UINT8& ucLevel,
                         CWX_UINT32& uiWordPos,
                         CWX_UINT32& uiWordLen,
                         char* szErr2K)
{
    hash_map<string, CwxFilterBase*, CwxStringHash>::iterator iter = m_nameFilterMap->find(strName);
    if (iter == m_nameFilterMap->end())
    {
        CwxCommon::snprintf(szErr2K, 2047, "Filter name [%u] doesn't exist.", strName.c_str());
        return -1;
    }
    CwxFilterBase *pFilter  = iter->second;
    if (m_bCaseSensive)
    {
        ucLevel = pFilter->filterStr(szContent, uiLen, uiWordPos, uiWordLen);
    }
    else
    {
        ucLevel = pFilter->filterCaseStr(szContent, uiLen, uiWordPos, uiWordLen);
    }
    ucLevel = level(ucLevel);
    return 0;
}

CWX_UINT8 CwfFilterMgr::level(CWX_UINT8 ucLevel)
{
    CWX_UINT8 ucBit = 0;
    if (ucLevel)
    {
        for (CWX_UINT8 i=0; i<8; i++)
        {
            if (CWX_CHECK_ATTR(ucLevel, 1<<i))
            {
                ucBit = i + 1;
            }
        }
    }
    return ucBit;
}

void CwfFilterMgr::reset()
{
    if (m_idFilterMap)
    {
        hash_map<CWX_UINT32/*id*/, CwxFilterBase*>::iterator iter = m_idFilterMap->begin();
        while(iter != m_idFilterMap->end())
        {
            delete iter->second;
            iter++;
        }
        delete m_idFilterMap;
        m_idFilterMap = NULL;
    }
    if (m_nameFilterMap)
    {
        delete m_nameFilterMap;
        m_nameFilterMap = NULL;
    }
    m_bCaseSensive = true;
}
