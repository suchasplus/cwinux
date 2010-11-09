#ifndef __CWX_ECHO_EVENT_HANDLER_H__
#define __CWX_ECHO_EVENT_HANDLER_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
#include "CwxAppCommander.h"

CWINUX_USING_NAMESPACE

class CwxEchoApp;
///echo请求的处理handle，为command的handle
class CwxEchoEventHandler : public CwxAppCmdOp 
{
public:
    ///构造函数
    CwxEchoEventHandler(CwxEchoApp* pApp):m_pApp(pApp)
    {
        m_ullMsgNum = 0;
    }
    ///析构函数
    virtual ~CwxEchoEventHandler()
    {

    }
public:
    ///收到echo请求的处理函数
    virtual int onRecvMsg(CwxMsgBlock*& msg,///<echo数据包及相关的请求连接信息
                            CwxAppTss* pThrEnv///<处理线程的thread-specific-store
                            );
private:
    CwxEchoApp*     m_pApp;  ///<app对象
    CWX_UINT64      m_ullMsgNum;
};

#endif 
