#ifndef __CWM_NERVE_SERVER_HANDLER_H__
#define __CWM_NERVE_SERVER_HANDLER_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
/**
@file CwmNerveServerHandler.h
@brief Nerve处理server的tcp通信的handle。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-26
@warning
@bug
*/
#include "CwxAppCommander.h"
#include "CwxMsgBlock.h"
#include "CwmNerveServiceStatus.h"
#include "CwmNerveActionStatus.h"
#include "CwmNerveTss.h"
class CwmNerveApp;

/**
@class CwmNerveServerHandler
@brief Server类型的ACTION的Handler
*/

class CwmNerveServerHandler : public CwxAppCmdOp
{
public:
    ///构造函数
    CwmNerveServerHandler(CwmNerveApp* pApp):m_pApp(pApp)
    {

    }
    ///析构函数
    ~CwmNerveServerHandler()
    {

    }
public:
    /**
    @brief 连接建立事件的处理函数。
    @param [in] msg 连接建立的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onConnCreated(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 处理Server连接关闭的事件。
    @param [in] msg 连接关闭的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onConnClosed(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 消息发送完毕事件的处理函数。
    @param [in] msg 消息发送完毕的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onEndSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 消息发送失败事件的处理函数。
    @param [in] msg 消息发送失败的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onFailSendMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
    /**
    @brief 收到数据查询。
    @param [in] msg 收到通信数据包的事件对象。
    @param [in] pThrEnv 线程的TSS对象。
    @return -1：处理失败，0：不处理此事件，1：处理此事件。
    */
    virtual int onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv);
public:
    ///根据名字查询server
    static bool queryServer(CwxAppTss* pThrEnv,
        CwmNerveApp* pApp,
        string const& strSystem,
        string const& strService,
        string const& strAction,
        string& strErr,
        CwxMsgBlock*& queryMsg);
    ///根据对象，轮询server
    static bool queryServer(CwxAppTss* pThrEnv,
        CwmNerveApp* pApp,
        CwmNerveServiceStatus* pService,
        CwmNerveActionStatus* pAction,
        string& strErr,
        CwxMsgBlock*& queryMsg);
    ///执行查询操作
    static bool execQuery(CwxAppTss* pThrEnv,
        CwmNerveApp* pApp,
        CwmNerveServiceStatus* pService,
        CwmNerveActionStatus* pAction,
        CWX_UINT32 uiTaskId);
    ///分析查询的结果
    static bool analyseReply(CwxAppTss* pThrEnv,
        int& status,
        string& strLevel,
        CwxKeyValueItem & record,
        string& strErr);
private:
    CwmNerveApp*      m_pApp;///<app对象
};


#endif
