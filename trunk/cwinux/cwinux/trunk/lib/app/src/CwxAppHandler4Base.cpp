#include "CwxAppHandler4Base.h"
#include "CwxAppReactor.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE

/// Force CwxAppHandler4Base to be an abstract base class.
CwxAppHandler4Base::CwxAppHandler4Base (CwxAppFramework* pApp, CwxAppReactor * reactor)
{
    m_pApp = pApp;
    m_reactor = reactor;
    m_handler = CWX_INVALID_HANDLE;
    memset(&m_event, 0x00, sizeof m_event);
    m_bReg = false;
    m_regMask = 0;
    m_type = CWX_APP_HANDLE_UNKNOW;
    m_regType = CwxAppReactor::REG_TYPE_UNKNOWN;
}

CwxAppHandler4Base::~CwxAppHandler4Base (void)
{

}


CWINUX_END_NAMESPACE
