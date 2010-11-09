#ifndef __CWX_APP_PAI_H__
#define __CWX_APP_PAI_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppPai.h
@brief 通信协议适配对象接口定义
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxMsgBlock.h"
#include "CwxAppConfig.h"
#include "CwxAppMacro.h"
#include "CwxAppHandler4Msg.h"


CWINUX_BEGIN_NAMESPACE
/**
@class CwxAppPai
@brief 通信数据包协议适配接口对象。Pai是protocol Adapter interface的缩写
*/
class CWX_API CwxAppPai
{
public:
    ///构造函数，参数为适配的连接对象
    CwxAppPai(CwxAppHandler4Msg const* conn);
    ///析构函数
    virtual ~CwxAppPai();
public:
    /**
    @brief 通知连接上收到了数据
    @param [in] msg 收到的数据包。此消息有协议适配对象负责释放
    @param [in] conn 收到消息的连接。
    @param [in] header 收到消息的消息头。
    @param [in] bSuspendConn 是否suspend连接上的数据接收
    @return -1：消息无效，关闭连接。 0：不连续接受消息； >0：连续从此连接上接受消息。
    */
    virtual int noticeRecvMsg(CwxMsgBlock* msg,
        CwxAppHandler4Msg const& conn,
        CwxMsgHead const& header,
        bool& bSuspendConn) = 0;
    ///检查发送KEEP-ALIVE的消息。返回值：-1，关闭连接；0,正常
    virtual int checkKeepAlive()
    {
        return 0;
    }
    ///检查是否连接需要KEEP-ALIVE
    bool isKeepAlive() const
    {
        return m_bKeepAlive;
    }
    ///设置是否连接需要KEEP-ALIVE
    void setKeepAlive(bool bKeepAlive)
    {
        m_bKeepAlive = bKeepAlive;
    }
private:
    CwxAppHandler4Msg const*   m_pConn;///<连接对象
    bool          m_bKeepAlive;///<是否需要KEEP-ALIVE的标志
};


CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif 
