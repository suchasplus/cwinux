#include "CwxEchoEventHandler.h"
#include "CwxEchoApp.h"

///echo请求的处理函数
int CwxEchoEventHandler::onRecvMsg(CwxMsgBlock*& msg, CwxTss* )
{
    CwxMsgQueue* queue = (CwxMsgQueue*)msg->event().getConnUserData();
    string const* data;
    memcpy(&data, msg->rd_ptr(), sizeof(string*));
    CwxMsgBlock* block = CwxMsgBlockAlloc::malloc(data->length());
    memcpy(block->wr_ptr(), data->c_str(), data->length());
    block->wr_ptr(data->length());
    block->event().m_ullArg = msg->event().m_ullArg;
    queue->enqueue(block);
    return 1;
}

