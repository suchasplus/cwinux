#ifndef __CWX_APP_HANDLER_4_TIME_H__
#define __CWX_APP_HANDLER_4_TIME_H__
/*
��Ȩ������
    ������Ϊ�������У���ѭGNU LGPL��http://www.gnu.org/copyleft/lesser.html����
�����������⣺
    ��Ѷ��˾������Ѷ��˾��ֱ��ҵ���������ϵ�Ĺ�˾����ʹ�ô�������ԭ��ɲο���
http://it.sohu.com/20100903/n274684530.shtml
    ��ϵ��ʽ��email:cwinux@gmail.com��΢��:http://t.sina.com.cn/cwinux
*/

/**
@file CwxAppHandler4Time.h
@brief Time Handle
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
@class CwxAppHandler4Time
@brief time click Handle
*/
class CWX_API CwxAppHandler4Time:public CwxAppHandler4Base
{
public:
    ///���캯��
    CwxAppHandler4Time(CwxAppFramework* pApp, CwxAppReactor* reactor, CwxTimeValue const& internal);
    ///��������
    ~CwxAppHandler4Time();
public:
    /**
    @brief handler open����reactor��ע�ᡣ
    @param [in] arg  ��Ч����
    @return -1��ʧ�ܣ� 0���ɹ�
    */
    virtual int open (void * arg= 0);
    /**
    @brief ����Timeout���¼�
    @param [in] event timeout �¼�
    @param [in] handle ��Ч
    @return -1������ʧ�ܣ� 0�������ɹ�
    */
    virtual int handle_event(int event, CWX_HANDLE handle=CWX_INVALID_HANDLE);
    ///handle close
    virtual int close(CWX_HANDLE handle=CWX_INVALID_HANDLE);
private:
    CwxTimeValue    m_internal;
    CwxTimeValue    m_cur;
};

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif