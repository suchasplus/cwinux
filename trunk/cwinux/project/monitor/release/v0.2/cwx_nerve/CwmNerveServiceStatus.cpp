#include "CwmNerveServiceStatus.h"

CwmNerveServiceStatus::CwmNerveServiceStatus()
{
    m_uiServerConnId = 0;
    m_pServiceInfo = NULL;
    m_bConnected = false;
}

CwmNerveServiceStatus::~CwmNerveServiceStatus()
{
    list<CwmNerveActionStatus*>::iterator iter = m_lsActStatus.begin();
    while(iter != m_lsActStatus.end())
    {
        delete *iter;
        iter++;
    }
}

