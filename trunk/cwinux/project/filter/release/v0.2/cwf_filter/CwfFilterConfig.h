#ifndef __CWF_FILTER_CONFIG_H__
#define __CWF_FILTER_CONFIG_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

#include "CwxGlobalMacro.h"
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwxXmlFileConfigParser.h"

CWINUX_USING_NAMESPACE

///一个过滤器对象的各个level的filter 文件定义
class CwfFilterConfigFilter
{
public:
    ///构造函数
    CwfFilterConfigFilter()
    {
        m_uiId = 0;
    }

    ///析构函数
    ~CwfFilterConfigFilter()
    {
    }
    ///赋值操作
    CwfFilterConfigFilter& operator=(CwfFilterConfigFilter const& obj)
    {
        if (this != &obj)
        {
            m_strName = obj.m_strName;
            m_uiId = obj.m_uiId;
            m_strStopwordFile = obj.m_strStopwordFile;
            m_levelFiles = obj.m_levelFiles;
        }
        return *this;
    }
    ///拷贝构造
    CwfFilterConfigFilter(CwfFilterConfigFilter const& obj)
    {
        m_strName = obj.m_strName;
        m_uiId = obj.m_uiId;
        m_strStopwordFile = obj.m_strStopwordFile;
        m_levelFiles = obj.m_levelFiles;
    }
public:
    string     m_strName; ///<过滤器的名字
    CWX_UINT32  m_uiId; ///<过滤器的id
    string     m_strStopwordFile;///<过滤器的stopword文件
    map<CWX_UINT8/*level*/, string/*file*/> m_levelFiles;  ///<过滤器1~7的7级过滤文件
};

///配置文件加载对象
class CwfFilterConfig
{
public:
    ///构造函数
    CwfFilterConfig()
    {
        m_unThreadNum = 4;
        m_bUtf8 = true;
        m_bCaseSensive = true;

    }
    ///析构函数
    ~CwfFilterConfig()
    {
        list<CwfFilterConfigFilter*>::iterator iter = m_fiters.begin();
        while(iter != m_fiters.end())
        {
            delete *iter;
            iter++;
        }
        m_fiters.clear();
    }
public:
    //加载配置文件.-1:failure, 0:success
    int loadConfig(string const & strConfFile);
    //输出加载的配置文件信息
    void outputConfig();
    //获取配置文件加载的失败原因
    char const* getError() { return m_szError; };
public:
    string              m_strWorkDir;///<工作目录
    string              m_strUnixPathFile;///<unix domain的监听 path file
    CwxHostInfo          m_filterListen;///<过滤的tcp的监听ip/port
    bool                m_bUtf8; ///<配置的过滤词及过滤的内容是否utf-8编码。缺省为UTF-8
    bool                m_bCaseSensive; ///<配置的过滤词及过滤的内容是否大小写敏感，缺省为大小写敏感
    CWX_UINT16           m_unThreadNum;///<echo服务的echo线程数量
    CwxHostInfo          m_mgrListen;///<管理的tcp的监听ip/port
    list<CwfFilterConfigFilter*> m_fiters; ///<过滤组定义列表
    char                m_szError[2048];///<错误消息的buf
};

#endif
