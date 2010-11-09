#include "CwxAppProcessMgr.h"
#include "CwxEchoTbClientApp.h"

int main(int argc, char** argv)
{
    CwxEchoTbClientApp* pApp = new CwxEchoTbClientApp();
    if (0 != CwxAppProcessMgr::init(pApp)) return 1;
    CwxAppProcessMgr::start(argc, argv, 200, 300);
    return 0;
}
