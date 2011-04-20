CWINUX_BEGIN_NAMESPACE

inline int CwxUnixConnector::connect(CwxUnixStream& stream,
                                     CwxAddr const& remoteAddr,
                                     CwxAddr const& localAddr,
                                     CwxTimeouter* timeout,
                                     int protocol,
                                     bool reuse_addr)
{
    return CwxSockConnector::connect(stream, remoteAddr, localAddr, timeout, protocol, reuse_addr);
}

CWINUX_END_NAMESPACE
