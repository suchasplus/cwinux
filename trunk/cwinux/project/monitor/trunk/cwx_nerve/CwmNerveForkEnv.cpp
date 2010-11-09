#include "CwmNerveForkEnv.h"
#include "CwmNerveApp.h"

CwmNerveForkEnv::CwmNerveForkEnv(CwmNerveApp* pApp):m_pApp(pApp)
{
    m_cgiPipe[0] = -1;
    m_cgiPipe[1] = -1;
    m_pCgiTask = NULL;
}

CwmNerveForkEnv::~CwmNerveForkEnv()
{
    reset();
}

int CwmNerveForkEnv::onFork()
{
    //创建管道
    pipe(m_cgiPipe);
    CWX_DEBUG(("Begin fork cgi process."));
    pid_t pid = fork();
    if (-1 == pid)
    {
        CWX_ERROR(("Failure to fork cgi process."));
        m_pCgiTask->setSuccess(false);
        m_pCgiTask->setErrMsg("Failure to fork cgi process.");
        m_pCgiTask->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
        m_pCgiTask->execute(m_pApp->getAppTss());
        return -1;
    }
    if (pid)
    {//父进程
        CWX_UINT32 uiTaskId = m_pCgiTask->getTaskId();
        //register reader
        int ret = m_pApp->noticeHandle4Msg(CwmNerveApp::SVR_TYPE_CGI_ACTION, 0, m_cgiPipe[0], true, 1024, false, (void*)m_pCgiTask->getTaskId());
        if (0>ret)
        {
            CWX_ERROR(("Failure to register cgi read handler"));
            m_pCgiTask->setSuccess(false);
            m_pCgiTask->setErrMsg("Failure to register cgi read handler");
            m_pCgiTask->setTaskState(CwxAppTaskBoardTask::TASK_STATE_FINISH);
            m_pCgiTask->execute(m_pApp->getAppTss());
            return pid;
        }
        //not close the read 
        m_cgiPipe[0] = -1;
        m_pCgiTask->execute(m_pApp->getAppTss());
        //通知taskboard，ret对应的连接有消息需要接收
        CwxMsgBlock* pDummyMsg = CwxMsgBlockAlloc::malloc(0);
        pDummyMsg->event().setSvrId(CwmNerveApp::SVR_TYPE_CGI_ACTION);
        pDummyMsg->event().setHostId(0);
        pDummyMsg->event().setConnId((CWX_UINT32)ret);
        CwxAppTaskBoardTask* pBaseTask = NULL;
        m_pApp->getTaskBoard().noticeEndSendMsg(uiTaskId, pDummyMsg, m_pApp->getAppTss(), pBaseTask);
        if (pDummyMsg)
        {
            CwxMsgBlockAlloc::free(pDummyMsg);
        }
        if (pBaseTask) pBaseTask->execute(m_pApp->getAppTss());
        //not release task
        this->m_pCgiTask = NULL;
        return pid;
    }
    //child
    //do nothong for child
    return 0;
}

void CwmNerveForkEnv::onChildMain()
{
    if( STDOUT_FILENO != m_cgiPipe[1] )
    {
        //将stdout重定向到pipe[1]
        close(STDOUT_FILENO);
        dup2(m_cgiPipe[1], STDOUT_FILENO);
    }
    else
    {
        m_cgiPipe[1] = -1;
    }
    reset();
    m_pApp->blockSignal(SIGCHLD);
    ///执行cgi脚本
    if( 0 > system(m_strCgiScript.c_str()) )
    {
        fprintf(stderr, "Failure to execute cgi script:%s", m_strCgiScript.c_str());
    }
}

void CwmNerveForkEnv::reset()
{
    if (-1 != m_cgiPipe[0])
    {
        close(m_cgiPipe[0]);
        m_cgiPipe[0] = -1;
    }
    if (-1 != m_cgiPipe[1])
    {
        close(m_cgiPipe[1]);
        m_cgiPipe[1] = -1;
    }
    if (m_pCgiTask)
    {
        delete m_pCgiTask;
        m_pCgiTask = NULL;
    }
}
