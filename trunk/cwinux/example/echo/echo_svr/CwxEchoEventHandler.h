#ifndef __CWX_ECHO_EVENT_HANDLER_H__
#define __CWX_ECHO_EVENT_HANDLER_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
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
