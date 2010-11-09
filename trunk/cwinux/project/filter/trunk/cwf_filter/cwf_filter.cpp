#include "CwxAppProcessMgr.h"
#include "CwfFilterApp.h"

int main(int argc, char** argv)
{
    //创建ECHO的app对象实例
    CwfFilterApp* pApp = new CwfFilterApp();
    //初始化双进程管理器
    if (0 != CwxAppProcessMgr::init(pApp)) return 1;
    //启动双进程，一个为监控echo进程的监控进程，一个为提供echo服务的工作进程。
    CwxAppProcessMgr::start(argc, argv, 100, 300);
    return 0;
}
