#include "CwxAppProcessMgr.h"
#include "CwmCenterApp.h"

int main(int argc, char** argv)
{
    ///创建app对象
    CwmCenterApp* pApp = new CwmCenterApp();
    ///初始化进程管理器
    if (0 != CwxAppProcessMgr::init(pApp)) return 1;
    ///启动双进程，10为最长的心跳秒数，300表示启动时，前300秒可以没有心跳。
    CwxAppProcessMgr::start(argc, argv, 10, 300);
    return 0;
}
