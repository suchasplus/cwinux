#ifndef __CWX_ECHO_TB_EVENT_HANDLER_H__
#define __CWX_ECHO_TB_EVENT_HANDLER_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
#include "CwxAppCommander.h"
#include "CwxAppTss.h"
#include "CwxGlobalMacro.h"

CWINUX_USING_NAMESPACE

class CwxEchoTbClientApp;
///echo请求的处理handle，为command的handle
class CwxEchoTbEventHandler : public CwxAppCmdOp 
{
public:
    ///构造函数
    CwxEchoTbEventHandler(CwxEchoTbClientApp* pApp):m_pApp(pApp)
    {
    }
    ///析构函数
    virtual ~CwxEchoTbEventHandler()
    {

    }
public:
    ///收到echo请求的处理函数
    virtual int onRecvMsg(CwxMsgBlock*& msg,///<echo数据包及相关的请求连接信息
                            CwxAppTss* pThrEnv///<处理线程的thread-specific-store
                            );
    virtual int onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    virtual int onEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    virtual int onFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    virtual int onTimeoutCheck(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
private:
    CwxEchoTbClientApp*     m_pApp;  ///<app对象
};

#endif 
