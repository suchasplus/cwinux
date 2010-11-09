#ifndef __CWF_FILTER_MGR_H__
#define __CWF_FILTER_MGR_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

#include "CwxFilterMachine.h"
#include "CwfFilterMacro.h"
#include "CwfFilterConfig.h"

///Filter的管理器
class CwfFilterMgr
{
public:
    enum
    {
        DEF_PER_FILTER_STOPWORD_NUM = 1000,
        DEF_PER_FILTER_KEYWORD_NUM = 10000
    };
public:
    ///构造函数
    CwfFilterMgr(CWX_UINT32 uiStopWord=DEF_PER_FILTER_STOPWORD_NUM,
        CWX_UINT32 uiWord=DEF_PER_FILTER_KEYWORD_NUM);
    ///析构函数
    ~CwfFilterMgr();
public:
    ///初始化filter管理器，0：成功；-1：失败
    int init(CwfFilterConfig const& config,
        char* szErr2K);
    ///根据filter的id，filte content，-1：失败；0：成功
    int filter(CWX_UINT32 uiFilterId,
        char const* szContent,
        CWX_UINT32 uiLen,
        CWX_UINT8& ucLevel,
        CWX_UINT32& uiWordPos,
        CWX_UINT32& uiWordLen,
        char* szErr2K);
    ///根据filter的名字，filte content，-1：失败；0：成功
    int filter(string strName,
        char const* szContent,
        CWX_UINT32 uiLen, 
        CWX_UINT8& ucLevel,
        CWX_UINT32& uiWordPos,
        CWX_UINT32& uiWordLen,
        char* szErr2K);
private:
    ///清空对象
    void reset();
    CWX_UINT8 level(CWX_UINT8 ucLevel);
private:
    CWX_UINT32   m_uiStopWord;
    CWX_UINT32   m_uiKeyWord;
    hash_map<CWX_UINT32/*id*/, CwxFilterBase*>* m_idFilterMap;
    hash_map<string, CwxFilterBase*, CwxStringHash>*  m_nameFilterMap;
    bool  m_bCaseSensive;
};

#endif 
