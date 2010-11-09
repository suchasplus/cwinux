#include "CwxAppProcessMgr.h"
#include "CwmNerveApp.h"

int main(int argc, char** argv)
{
    ///创建Nerve App 对象
    CwmNerveApp* pApp = new CwmNerveApp();
    ///初始化进程管理器
    if (0 != CwxAppProcessMgr::init(pApp)) return 1;
    ///启动Nerve App，10为心跳的最大间隔为10s，
    ///300为进程启动的时候，前300秒可以不发送告警
    CwxAppProcessMgr::start(argc, argv, 10, 300);
    return 0;
}
