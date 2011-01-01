#ifndef __CWF_FILTER_HANDLER_H__
#define __CWF_FILTER_HANDLER_H__
/*
版权声明：
    本软件遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html）
*/
#include "CwxAppCommander.h"
#include "CwfFilterMacro.h"

class CwfFilterApp;

///filter 请求的处理handle，为command的handle
class CwfFilterHandler : public CwxAppCmdOp
{
public:
    ///构造函数
    CwfFilterHandler(CwfFilterApp* pApp):m_pApp(pApp)
    {

    }
    ///析构函数
    virtual ~CwfFilterHandler()
    {

    }
public:
    ///收到echo请求的处理函数
    virtual int onRecvMsg(CwxMsgBlock*& msg,///<echo数据包及相关的请求连接信息
                         CwxAppTss* pThrEnv///<处理线程的thread-specific-store
                        );
private:
    void reply(CwxMsgHead const& header,
        CWX_UINT32 uiConnId,
        int ret,
        CWX_UINT8 ucLevel,
        char const* szWord,
        CWX_UINT32 uiWordLen,
        char const* szErrMsg,
        CwxAppTss* pThrEnv);
private:
    CwfFilterApp*     m_pApp;  ///<app对象
};

#endif 
