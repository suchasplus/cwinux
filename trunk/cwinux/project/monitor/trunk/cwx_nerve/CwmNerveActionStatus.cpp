#include "CwmNerveActionStatus.h"
#include "CwmNerveServiceStatus.h"
#include "CwxDate.h"

///构造函数
CwmNerveActionStatus::CwmNerveActionStatus()
{
}
///析构函数
CwmNerveActionStatus::~CwmNerveActionStatus()
{

}
///设置action的service对象
void CwmNerveActionStatus::setServiceStatus(CwmNerveServiceStatus const* pServiceStatus)
{
    if (pServiceStatus)
    {
        m_alarmStatus.setSystemName(pServiceStatus->getServiceInfo()->getSystem());
        m_alarmStatus.setServiceName(pServiceStatus->getServiceInfo()->getName());
    }
    m_pServiceStatus=pServiceStatus;
} 
