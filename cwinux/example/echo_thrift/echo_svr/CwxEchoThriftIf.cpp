#include "CwxEchoThriftIf.h"
#include "CwxEchoApp.h"

void CwxEchoThriftIf::Echo(EchoData& _return, const std::string& echo_data) {
  _return.data = echo_data;
}
