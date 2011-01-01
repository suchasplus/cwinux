#ifndef __CWX_APP_HANDLER_4_STDIO_H__
#define __CWX_APP_HANDLER_4_STDIO_H__
/*
��Ȩ������
    ��������ѭGNU LGPL��http://www.gnu.org/copyleft/lesser.html��
*/

/**
@file CwxAppHandler4Stdio.h
@brief ��׼����STDIN��������ӦHandle
@author cwinux@gmail.com
@version 0.1
@date 2009-07-20
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxGlobalMacro.h"
#include "CwxType.h"
#include "CwxAppMacro.h"
#include "CwxAppConfig.h"
#include "CwxAppHandler4Base.h"
#include "CwxAppReactor.h"

CWINUX_BEGIN_NAMESPACE

class CwxAppFramework;
/**
@class CwxAppHandler4StdIo
@brief ��׼����STDIN��������ӦHandle
*/
class CWX_API CwxAppHandler4StdIo:public CwxAppHandler4Base
{
    enum{
        MAX_LINE_LENGTH= 1023///<�������buf��С
    };
public:
    ///���캯��
    CwxAppHandler4StdIo(CwxAppFramework* pApp, CwxAppReactor* reactor);
    ///��������
    ~CwxAppHandler4StdIo();
public:
    /**
    @brief handler open����reactor��ע�ᡣ
    @param [in] arg  ��Ч����
    @return -1��ʧ�ܣ� 0���ɹ�
    */
    virtual int open (void * arg= 0);
    /**
    @brief ���������¼�������
    @param [in] event �����ϵ��¼�
    @param [in] handle  �������¼���handle��
    @return -1������ʧ�ܣ� 0�������ɹ�
    */
    virtual int handle_event(int event, CWX_HANDLE handle=CWX_INVALID_HANDLE);
    ///handle close
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE);
private:
    char              m_szLine[MAX_LINE_LENGTH + 1];///<STDIN�����ݽ���BUF
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif