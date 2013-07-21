#ifndef __CWX_ECHO_CLIENT_APP_H__
#define __CWX_ECHO_CLIENT_APP_H__
/*
版权声明：
    本软件遵循GNU GPL V3（http://www.gnu.org/licenses/gpl.html），
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/
#include "CwxAppFramework.h"
#include "CwxEchoClientConfig.h"

#define ECHO_CLIENT_APP_VERSION "1.0"
#define ECHO_CLIENT_MODIFY_DATE "2010-08-29"

CWINUX_USING_NAMESPACE;

///echo的压力测试app
class CwxEchoClientApp : public CwxAppFramework{
public:
    enum{
        LOG_FILE_SIZE = 30, ///<每个循环运行日志文件的MBTYE
        LOG_FILE_NUM = 7,///<循环日志文件的数量
        MAX_MSG_SIZE = 64 * 1024*1024,
        SVR_TYPE_ECHO = CwxAppFramework::SVR_TYPE_USER_START ///<echo查询的svr-id类型
    };
    enum{
        SEND_MSG_TYPE = 1, ///<echo发送的消息类型
        RECV_MSG_TYPE =2 ///<echo回复的消息类型
    };

    ///构造函数
	CwxEchoClientApp();
    ///析构函数
	virtual ~CwxEchoClientApp();
  //初始化app, -1:failure, 0 success;
  virtual int init(int argc, char** argv);
public:
  //时钟响应函数
  virtual void onTime(CwxTimeValue const& current);
  //信号响应函数
  virtual void onSignal(int signum);
  char const* buf() const { return m_szBuf;}
  void IncSendNum() {
    CwxMutexGuard<CwxMutexLock> lock(&m_lock);
    m_uiSendNum ++;
    if (m_uiSendNum%10000 == 0) {
      CWX_INFO(("Finish num=%u\n", m_uiSendNum));
    }
  }

  CwxEchoClientConfig const& conf() const { return m_config;}

protected:
    //init the Enviroment before run.0:success, -1:failure.
	virtual int initRunEnv();
private:
    CwxEchoClientConfig            m_config; ///<配置文件对象
    char                           m_szBuf[MAX_MSG_SIZE+1]; ///<发送的echo数据buf及内容
    CwxThreadPool**                m_threadPools;
    CwxMutexLock                   m_lock;
    CWX_UINT32                     m_uiSendNum;///<发送echo请求的数量
};

#endif

