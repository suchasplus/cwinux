#ifndef __CWX_ECHO_TB_CLIENT_CONFIG_H__
#define __CWX_ECHO_TB_CLIENT_CONFIG_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
#include "CwxHostInfo.h"
#include "CwxCommon.h"
#include "CwxXmlFileConfigParser.h"

CWINUX_USING_NAMESPACE

class CwxEchoTbClientGroup
{
public:
    CwxEchoTbClientGroup()
    {
    }
    CwxEchoTbClientGroup(CwxEchoTbClientGroup const& group)
    {
        m_hosts = group.m_hosts;
    }
    CwxEchoTbClientGroup& operator=(CwxEchoTbClientGroup const& group)
    {
        if (this != & group)
        {
            m_hosts = group.m_hosts;
        }
        return *this;
    }
public:
    list<CwxHostInfo> m_hosts;
};
///echo压力测试的配置文件加载对象
class CwxEchoTbClientConfig
{
public:
    CwxEchoTbClientConfig()
    {
        m_uiDataSize = 0;
        m_unConnNum = 1;
        m_unThreadNum = 4;
        m_uiTimeout = 2000;
        m_uiTaskNum = 10;
        m_uiWaitMsgNum = 10;
        m_szError[0] = 0x00;
    }
    
    ~CwxEchoTbClientConfig()
    {
        list<CwxEchoTbClientGroup*>::iterator iter=m_svrGroup.begin();
        while(iter != m_svrGroup.end())
        {
            delete *iter;
            iter++;
        }
        m_svrGroup.clear();
    }
public:
    //加载配置文件.-1:failure, 0:success
    int loadConfig(string const & strConfFile);
    //输出配置文件
    void outputConfig(string & strConfig);
    //获取加载配置文件的失败错误信息
    char const* getError() { return m_szError; };
    
public:
    string              m_strWorkDir;///<工作目录
    CwxHostInfo          m_mgrListen; ///<管理的监听端口
    CWX_UINT32           m_uiDataSize;///<echo的数据包的大小
    CWX_UINT16           m_unConnNum; ///<每个server的连接数量
    CWX_UINT16           m_unThreadNum; ///<线程的数量
    CWX_UINT32           m_uiTaskNum; ///<任务的数量
    CWX_UINT32           m_uiTimeout; ///<查询超时时间，单位为ms
    CWX_UINT32           m_uiWaitMsgNum; ///<连接上等待发送消息的数量
    list<CwxEchoTbClientGroup*> m_svrGroup; ///<服务分组的map
    char                m_szError[2048];///<错误消息buf
};

#endif
