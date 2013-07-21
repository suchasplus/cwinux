#include "CwxEchoThriftIf.h"
#include "CwxEchoApp.h"

void CwxEchoThriftIf::Echo(echo_thrift::EchoData& _return, const std::string& echo_data) {
  _return.data = echo_data;
}
