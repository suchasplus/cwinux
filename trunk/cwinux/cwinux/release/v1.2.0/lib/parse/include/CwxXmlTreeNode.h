#ifndef __CWX_XML_TREE_NODE_H__
#define __CWX_XML_TREE_NODE_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxXmlTreeNode.h
@brief XML的数据节点定义。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/
#include "CwxPre.h"
#include "CwxStl.h"
#include "CwxCommon.h"
#include "CwxCharPool.h"


#include "CwxParseMacro.h"

CWINUX_BEGIN_NAMESPACE

/**
@class CwxXmlTreeNode
@brief 树状的数据节点对象，用于表示XML、JSON的数据节点。
*/
class CWX_API CwxXmlTreeNode
{
public:
    ///构造函数
    CwxXmlTreeNode()
    {
        m_pChildHead = NULL;
        m_pChildTail = NULL;
        m_prev = NULL;
        m_next = NULL;
        m_pParent = NULL;
    }
    ///析构函数
    ~CwxXmlTreeNode()
    {
        if (m_pChildHead) delete m_pChildHead;
        if (m_next) delete m_next;
    }
public:
    char*   m_szElement;///<节点的名字
    list<char*>   m_listData; ///<XML的\<aaa\>aaaaa\</aaa\>类型节点的数据
    list<pair<char*, char*> > m_lsAttrs;///<节点属性的key,value对
    CwxXmlTreeNode* m_pChildHead;///<节点的孩子结点的头
    CwxXmlTreeNode* m_pChildTail;///<节点的孩子结点的尾
    CwxXmlTreeNode* m_prev;///<节点的前一个兄弟节点
    CwxXmlTreeNode* m_next;///<节点的下一个兄弟节点
    CwxXmlTreeNode* m_pParent;///<节点的父节点
};

CWINUX_END_NAMESPACE

#include "CwxPost.h"

#endif

