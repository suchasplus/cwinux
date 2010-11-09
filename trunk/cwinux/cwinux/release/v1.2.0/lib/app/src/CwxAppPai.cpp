#include "CwxAppPai.h"
#include "CwxAppFramework.h"

CWINUX_BEGIN_NAMESPACE

CwxAppPai::CwxAppPai(CwxAppHandler4Msg const* conn)
:m_pConn(conn)
{
    m_bKeepAlive = false;
}

CwxAppPai::~CwxAppPai()
{

}

CWINUX_END_NAMESPACE
