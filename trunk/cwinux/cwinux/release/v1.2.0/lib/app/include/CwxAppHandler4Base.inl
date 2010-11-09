CWINUX_BEGIN_NAMESPACE

inline void CwxAppHandler4Base::setApp(CwxAppFramework* app)
{
    m_pApp = app;
}
///获取app
inline CwxAppFramework* CwxAppHandler4Base::getApp()
{
    return m_pApp;
}
///获取app
///设置handle的reactor
inline void CwxAppHandler4Base::reactor (CwxAppReactor *reactor)
{
    m_reactor = reactor;
}
///获取handle的reactor.
inline CwxAppReactor *CwxAppHandler4Base::reactor(void)
{
    return m_reactor;
}
inline bool CwxAppHandler4Base::isReg() const
{
    return m_bReg;
}
///设置注册状态
inline void CwxAppHandler4Base::setReg(bool bReg)
{
    m_bReg = bReg;
}

///获取reg mask
inline int CwxAppHandler4Base::getRegMask() const
{
    return m_regMask;
}
///设置reg mask
inline void CwxAppHandler4Base::setRegMask(int mask)
{
    m_regMask = mask;
}

///获取Io handle
inline CWX_HANDLE CwxAppHandler4Base::getHandle(void) const
{
    return m_handler;
}
///设置IO handle
inline void CwxAppHandler4Base::setHandle (CWX_HANDLE handle)
{
    m_handler = handle;
}

///设置handle type
inline void CwxAppHandler4Base::setType(int type)
{
    m_type = type;
}
///获取handle type
inline int CwxAppHandler4Base::getType() const
{
    return m_type;
}

///获取注册类型
inline int CwxAppHandler4Base::getRegType() const
{
    return m_regType;
}
///设置注册类型
inline void CwxAppHandler4Base::setRegType(int type)
{
    m_regType = type;
}

///获取是否为persist事件
inline bool CwxAppHandler4Base::isPersistMask() const
{
    return (m_regMask&PERSIST_MASK) !=0;
}

///是否设置了read mask
inline bool CwxAppHandler4Base::isReadMask() const
{
    return (m_regMask&READ_MASK) !=0;
}
///是否设置了write mask
inline bool CwxAppHandler4Base::isWriteMask() const
{
    return (m_regMask&WRITE_MASK) !=0;
}
///是否设置了timeout mask
inline bool CwxAppHandler4Base::isTimeoutMask() const
{
    return (m_regMask&TIMEOUT_MASK) !=0;
}
///是否设置了signal mask
inline bool CwxAppHandler4Base::isSignalMask() const
{
    return (m_regMask&SIGNAL_MASK) !=0;
}



CWINUX_END_NAMESPACE

