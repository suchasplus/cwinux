#include "CwfFilterTss.h"


CwfFilterTss::~CwfFilterTss()
{
    if (m_pSndPackage) delete m_pSndPackage;
    if (m_pReader) delete m_pReader;
}

int CwfFilterTss::init()
{
    m_pSndPackage = new CwxPackageWriter(MAX_PACKAGE_SIZE);
    m_pReader = new CwxPackageReader(true);
    return 0;
}
