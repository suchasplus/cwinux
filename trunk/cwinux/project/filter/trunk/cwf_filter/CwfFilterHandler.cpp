#include "CwfFilterHandler.h"
#include "CwfFilterApp.h"

///echo请求的处理函数
int CwfFilterHandler::onRecvMsg(CwxMsgBlock*& msg, CwxAppTss* pThrEnv)
{
    CwfFilterTss* pTss = (CwfFilterTss*)pThrEnv;
    do
    {
        if (!pTss->m_pReader->unpack(msg->rd_ptr(), msg->length(), false, true))
        {
            CwxCommon::snprintf(pTss->m_szBuf2K, 2047, "Failure to unpack the filter request package, err=%s", pTss->m_pReader->getErrMsg());
            CWX_ERROR((pTss->m_szBuf2K));
            reply(msg->event().getMsgHeader(), msg->event().getConnId(), -1, 0, NULL, 0, pTss->m_szBuf2K ,pThrEnv);
            break;
        }
        //get content
        CwxKeyValueItem const* pContent = pTss->m_pReader->getKey(CWF_CONTENT);
        if (!pContent)
        {
            CwxCommon::snprintf(pTss->m_szBuf2K, 2047, "No key[%s] in filter request package", CWF_CONTENT);
            CWX_ERROR((pTss->m_szBuf2K));
            reply(msg->event().getMsgHeader(), msg->event().getConnId(), -1, 0, NULL, 0, pTss->m_szBuf2K,pThrEnv);
            break;
        }
        //get id
        int ret = 0;
        CWX_UINT32 uiId=0;
        string  strName;
        CWX_UINT8  ucLevel=0;
        CWX_UINT32 uiWordPos=0;
        CWX_UINT32 uiWordLen=0;
        if (pTss->m_pReader->getKey(CWF_ID, uiId))
        {
           ret = m_pApp->getFilterMgr()->filter(uiId,
               pContent->m_szData,
               pContent->m_uiDataLen,
               ucLevel,
               uiWordPos,
               uiWordLen,
               pTss->m_szBuf2K);
        }
        else if(pTss->m_pReader->getKey(CWF_NAME, strName))
        {//get name
            ret = m_pApp->getFilterMgr()->filter(strName.c_str(),
                pContent->m_szData,
                pContent->m_uiDataLen,
                ucLevel,
                uiWordPos,
                uiWordLen,
                pTss->m_szBuf2K);
        }
        else
        {
            ret = -1;
            CwxCommon::snprintf(pTss->m_szBuf2K, 2047, "Filter request package hasn't filter id[%s] or filter name[%s] key", CWF_ID, CWF_NAME);
        }
        if (-1 == ret)
        {
           reply(msg->event().getMsgHeader(),
               msg->event().getConnId(),
               ret,
               0,
               NULL,
               0,
               pTss->m_szBuf2K,
               pThrEnv);
        }
        else
        {
            reply(msg->event().getMsgHeader(),
                msg->event().getConnId(),
                0,
                ucLevel,
                ucLevel?pContent->m_szData + uiWordPos:NULL,
                ucLevel?uiWordLen:0,
                NULL,
                pThrEnv);
        }
    }while(0);
    return 1;
}

void CwfFilterHandler::reply(CwxMsgHead const& header,
                             CWX_UINT32 uiConnId,
                             int ret,
                             CWX_UINT8 ucLevel,
                             char const* szWord,
                             CWX_UINT32 uiWordLen,
                             char const* szErrMsg,
                             CwxAppTss* pThrEnv)
{
    CwfFilterTss* pTss = (CwfFilterTss*)pThrEnv;
    CwxPackageWriter* pWriter = pTss->m_pSndPackage;
    pWriter->beginPack();
    ///add ret
    pWriter->addKeyValue(CWF_RET, ret);
    ///add level
    pWriter->addKeyValue(CWF_LEVEL, ucLevel);
    ///add word
    if (szWord)
    {
        pWriter->addKeyValue(CWF_WORD, szWord, uiWordLen, false);
    }
    else
    {
        pWriter->addKeyValue(CWF_WORD, "", 0, false);
    }
    ///add err
    if (szErrMsg)
    {
        pWriter->addKeyValue(CWF_ERR, szErrMsg, strlen(szErrMsg), false);
    }
    else
    {
        pWriter->addKeyValue(CWF_ERR, "", 0, false);
    }
    pWriter->pack();
    CwxMsgHead locHeader(0,
        0,
        header.getMsgType() + 1,
        header.getTaskId(),
        pWriter->getMsgSize());
    CwxMsgBlock* pBlock = CwxMsgBlockAlloc::pack(locHeader,
        pWriter->getMsg(),
        pWriter->getMsgSize());
    pBlock->send_ctrl().setConnId(uiConnId);
    pBlock->send_ctrl().setSvrId(CwfFilterApp::SVR_TYPE_FILTER);
    pBlock->send_ctrl().setHostId(0);
    pBlock->send_ctrl().setMsgAttr(CwxMsgSendCtrl::NONE);
    if (0 != m_pApp->sendMsgByConn(pBlock))
    {
        CWX_ERROR(("Failure to reply error msg"));
        CwxMsgBlockAlloc::free(pBlock);
    }
}
