#include "CwmCenterTss.h"
#include "CwmCenterActionLoop.h"
#include "CwmCenterActionNameLoop.h"
#include "CwmCenterServiceLoop.h"
#include "CwmCenterSystemLoop.h"


CwmCenterTss::~CwmCenterTss()
{
    if (m_pSndPackage) delete m_pSndPackage;
    if (m_szOutputBuf) delete [] m_szOutputBuf;
    if (m_pActionLoop) delete m_pActionLoop;
    if (m_pServiceLoop) delete m_pServiceLoop;
    if (m_pSystemLoop) delete m_pSystemLoop;
    if (m_pActionNameLoop) delete m_pActionNameLoop;
}

int CwmCenterTss::init()
{
    m_pSndPackage = new CwxPackageWriter(MAX_PACKAGE_SIZE);
    m_szOutputBuf = new char[MAX_PACKAGE_SIZE];
    m_pActionLoop = new CwmCenterActionLoop();
    m_pServiceLoop = new CwmCenterServiceLoop();
    m_pSystemLoop = new CwmCenterSystemLoop();
    m_pActionNameLoop = new CwmCenterActionNameLoop();
    return 0;
}
