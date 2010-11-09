#ifndef __CWX_HTML_TMPL_H__
#define __CWX_HTML_TMPL_H__
/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/**
@file CwxHtmlTmpl.h
@brief 定义文本模板替换类。
@author cwinux@gmail.com
@version 0.1
@date 2009-10-10
@warning
@bug
*/

#include "CwxPre.h"
#include "CwxType.h"
#include "CwxStl.h"
#include "CwxStlFunc.h"
#include "CwxReplMachine.h"
#include "CwxCharset.h"
#include "CwxMemLineReader.h"
#include "CwxTypePool.h"
#include "CwxCharPool.h"
#include "CwxXmlConfigParser.h"
#include "CwxHtmlTmplCmp.h"
#include "CwxHtmlTmplEscape.h"
#include "CwxHtmlTmplLoop.h"

CWINUX_BEGIN_NAMESPACE
/**
@class CwxHtmlTmplTagKind
@brief 文本模板的TAG类型定义。
*/
class CWX_API CwxHtmlTmplTagKind
{
public:
    enum{
        TAG_TEXT = 1,///<文本TAG
        TAG_VAL = 2,///<变量TAG
        TAG_LOOP = 3,///<循环TAG
        TAG_IF = 4,///<IF TAG
        TAG_ELIF = 5,///<ELIF TAG
        TAG_ELSE = 6,///<ELSE TAG
        TAG_JOIN = 7 ///<语法连接TAG
    };
};

/**
@class CwxHtmlTmplValSysAttr
@brief 文本模板变量的属性定义。
*/
class CWX_API CwxHtmlTmplValSysAttr
{
public:
    enum{
        ATTR_REPLACE = 0X01,///<变量的内容需要执行字符替换
        ATTR_CHARSET_CONVERT = 0X02,///<变量的内容需要进行字符集变换
        ATTR_EXPLICT = 0X04///<变量必须存在
    };
};

/**
@class CwxHtmlTmplTagNode
@brief 文本模板TAG的ITEM定义。
*/
struct CwxHtmlTmplTagNode
{
    CWX_UINT8  m_kind;///<tag 类型
    CwxHtmlTmplTagNode* m_next; ///<下一个TAG
    union
    {
        struct 
        {
            char const * m_szText; ///<文本的内容
            int          m_iTextLen;///<文本的长度
        }m_text;///<文本TAG节点

        struct
        {
            char const * m_szName;///<变量的名字
            char const * m_szObjName;///<变量的对象名
            CWX_UINT8    m_ucSysAttr;///<变量的属性
            char const * m_szUserAttr;///<变量的用户属性
            int          m_iDataLen;///<变量内容长度限制，<0：字符不能超过其绝对值；0：没限制；>0：字节不能超过此值
            char const * m_szDefault;///<变量的缺省值
            char const * m_szEscape;///<变量的变换函数对象的名称
            char const * m_szArgument;///<变量的变换函数的参数
            CwxHtmlTmplEscape* m_pEscapeObj;///<变量变换函数对象
        }m_var;///<变量TAG节点

        struct
        {
            char const * m_szName;///<IF中的变量名
            char const * m_szObjName;///<IF中的变量的对象名
            char const * m_szCompair;///<IF中比较函数对象的名字
            char const * m_szData;///<比较中的常量
            char const * m_szArgument;///<比较函数对象的参数
            bool        m_bReverse; ///<取反
            CwxHtmlTmplTagNode * m_pMain;///<IF比较为TRUE时的分支
            CwxHtmlTmplTagNode * m_pBranch;///<IF比较为FALSE时的分支
            CwxHtmlTmplCmp  * m_pCompareObj;///<比较函数对象
        }m_if;///<IF TAG节点

        struct
        {
            char const * m_szName;///<elif中的变量名
            char const * m_szObjName;///<变量的对象名
            char const * m_szCompair;///<比较函数对象的名字
            char const * m_szData;///<比较中的常量数据
            char const * m_szArgument;///<比较函数对象的参数
            bool        m_bReverse; ///<取反
            CwxHtmlTmplTagNode * m_pMain;///<比较结果为true时的分支
            CwxHtmlTmplTagNode * m_pBranch;///<比较结果为false时的分支
            CwxHtmlTmplCmp * m_pCompareObj;///<比较函数对象
        }m_elif;///<elif TAG节点

        struct
        {
            CwxHtmlTmplTagNode * m_pBody; ///<else的分支
        }m_else;///<else TAG节点


        struct 
        {
            char const *         m_szLoopName;///<loop的名字，也是对象的名字
            list<pair<char*, char *> >* m_attrs;///<loop中设置的对象的属性
            CwxHtmlTmplTagNode * m_pBody;///<loop的主体
        }m_loop;///<LOOP TAG节点
    }m_tag;
};

class CwxHtmlTmpl;

/**
@class CwxHtmlTmplEnv
@brief 模板替换的输出环境，用于保存中间信息，实现CwxHtmlTmpl的多线程安全。
*/
class CWX_API CwxHtmlTmplEnv
{
public:
    ///构造函数
    CwxHtmlTmplEnv():m_uiErrBufLen(1023)
    {
        m_uiOutTextPos = 0;
        m_uiOutTextLen = 0;
        m_szOutText = NULL;
        memset(m_szErr1K, 0x00, m_uiErrBufLen + 1);
    }
public:
    /**
    @brief 初始化模板输出环境。
    @param [in] root 根模板对象
    @param [out] szOutText 模板输出的BUFFER
    @param [in] uiOutTextBufLen Buffer的大小
    @return void
    */
    void init(CwxHtmlTmplLoop* root, char* szOutText, CWX_UINT32 uiOutTextBufLen)
    {
        m_loopStackList.clear();
        m_loopStackList.push_front(pair<string,CwxHtmlTmplLoop*>("", root));
        m_loopMap.clear();
        m_szOutText = szOutText;
        m_uiOutTextLen = uiOutTextBufLen;
        m_uiOutTextPos = 0;
        memset(m_szErr1K, 0x00, 1024);
    }
    ///获取模板变量替换过程中的错误信息
    char const* getErrMsg() const
    {
        return m_szErr1K;
    }
    ///获取形成的文本的大小
    CWX_UINT32 getBufLen()
    {
        return m_uiOutTextPos;
    }
public:
    friend  class CwxHtmlTmpl;
private:
    ///获取模板buf剩余的空间，在模板变量替换的过程中使用
    CWX_UINT32 getBufLeft()
    {
        return m_uiOutTextLen>=m_uiOutTextPos?m_uiOutTextLen -m_uiOutTextPos - 1:0;
    }
private:
    list<pair<string, CwxHtmlTmplLoop*> >    m_loopStackList;///<模板变量替换过程中的LOOP对象嵌套列表
    map<string, CwxHtmlTmplLoop*> m_loopMap;///<模板LOOP对象名字与实例的MAP
    char*                    m_szOutText;///<模板输出的BUF
    CWX_UINT32               m_uiOutTextPos;///<模板输出BUF中输出字符的当前位置
    CWX_UINT32               m_uiOutTextLen;///<模板输出BUF的大小
    CWX_UINT32 const         m_uiErrBufLen;///<错误信息BUF的大小
    char	                 m_szErr1K[1024];///<错误信息BUF
};

///模板变量定义的MAP，映射变量名与其定义变量格式信息
typedef map<char const*, CwxHtmlTmplTagNode*, CwxCharLess> CwxHtmlValMap;
///模板常量的名字与内容的映射MAP
typedef map<string, string> CwxHtmlConstMap;

/**
@class CwxHtmlTmpl
@brief 模板替换类，实现多线程安全的文本模板的变量替换。
*/
class CWX_API CwxHtmlTmpl
{
public:
    ///构造函数
    CwxHtmlTmpl()
    {
        m_repl = NULL;
        memset(&m_root, 0x00, sizeof(m_root));
        m_bCaseInsensitive = false;
        m_szTmpBuf = NULL;
    }
    ///析构函数
    ~CwxHtmlTmpl()
    {
        clear();
    }
public:
    ///初始化全局的模板比较函数对象与ESCAPE函数对象，非多线程安全
    static void init();
    ///释放全局的模板比较函数对象与ESCAPE函数对象，非多线程安全
    static void destroy();
    /**
    @brief 添加全局的比较函数对象，非多线程安全。
    @param [in] pCmpObj 添加的比较函数对象
    @return false：相同名字的对象已经存在；true：添加成功
    */
    static bool addCmp(CwxHtmlTmplCmp* pCmpObj);
    /**
    @brief 获取全局的比较函数对象，非多线程安全。
    @param [in] strName 比较函数对象的名字
    @return NULL：指定名字的对象不存在；否则，返回具有此名字的比较函数对象
    */
    static CwxHtmlTmplCmp* getCmp(string const& strName);
    /**
    @brief 删除全局的比较函数对象，非多线程安全。
    @param [in] strName 要删除的比较函数对象的名字
    @return NULL：指定名字的对象不存在；否则，返回具有此名字的比较函数对象
    */
    static CwxHtmlTmplCmp* eraseCmp(string const& strName);
    /**
    @brief 清空所有注册的比较函数对象。
    @return void
    */
    static void clearCmp();
    /**
    @brief 添加全局的ESCACPE函数对象，非多线程安全。
    @param [in] pEscapeObj 添加的ESCAPE函数对象
    @return false：相同名字的对象已经存在；true：添加成功
    */
    static bool addEscape(CwxHtmlTmplEscape* pEscapeObj);
    /**
    @brief 获取全局的ESCACPE函数对象，非多线程安全。
    @param [in] strName ESCAPE函数对象的名字
    @return NULL：对象不存在；否则，返回具有此名字的ESCAPE函数对象
    */
    static CwxHtmlTmplEscape* getEscape(string const& strName);
    /**
    @brief 删除全局的ESCACPE函数对象，非多线程安全。
    @param [in] strName 要删除的ESCAPE函数对象的名字
    @return NULL：对象不存在；否则，删除具有此名字的ESCAPE函数对象并返回
    */
    static CwxHtmlTmplEscape* eraseEscape(string const& strName);
    ///清空所有注册的escape函数对象
    static void clearEscape();
public:
    /**
    @brief 解析变量替换文本模板。
    @param [in] szTemplate 变量替换文本模板的定义
    @param [in] uiTemplateLen 文本模板的定义的长度
    @return false：解析失败，通过getErr()获取失败的原因；true：解析成功
    */
    bool parse(char const* szTemplate, CWX_UINT32 uiTemplateLen);
    /**
    @brief 执行模板的变量替换。
    @param [in] env 模板变量替换的环境
    @return false：执行失败，通过CwxHtmlTmplEnv的getErrMsg()获取失败的原因；true：替换成功
    */
    bool output(CwxHtmlTmplEnv& env) const;
public:
    ///获取模板的名字
    string const& getTemplateName() const
    {
        return m_strTmplName;
    }
    ///设置模板的名字
    void setTemplateName(string const& strName)
    {
        m_strTmplName = strName;
    }
    ///获取模板的文件名字
    string const& getTemplateFile() const
    {
        return m_strTmplFile;
    }
    ///设置模板的文件名字
    void setTemplateFile(string const& strFile)
    {
        m_strTmplFile = strFile;
    }
    ///获取模板的其他属性
    string const& getTemplateAttr(string const& name)
    {
        static string strEmptyAttr;
        map<string, string>::iterator iter = m_attrTmplMap.find(name);
        return iter==m_attrTmplMap.end()?strEmptyAttr:iter->second;
    }
    ///设置模板的其他属性
    void setTemplateAttr(string const& name, string const& attr)
    {
        m_attrTmplMap[name] = attr; 
    }
    ///获取模板的字符集
    string const& getCharset() const
    {
        return m_strCharset;
    }
    ///获取是否模板的变量字符替换需要大小写不敏感
    bool isCaseInsensitive() const
    {
        return m_bCaseInsensitive;
    }
    ///获取模板的变量字符替换对象
    CwxReplBase* getRepl()
    {
        return  m_repl;
    }
    ///获取模板解析过程中的错误
    string const& getErr() const
    {
        return m_strErr;
    }
private:
    /**
    @brief 内部执行模板的变量替换。
    @param [in] pNode 模板变量替换的开始位置
    @param [in] env 模板变量替换的环境
    @return false：执行失败，通过CwxHtmlTmplEnv的getErrMsg()获取失败的原因；true：替换成功
    */
    bool output(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env) const;
    /**
    @brief 执行模板的一个变量的替换。
    @param [in] pNode 变量的节点
    @param [in] env 模板变量替换的环境
    @return false：执行失败，通过CwxHtmlTmplEnv的getErrMsg()获取失败的原因；true：替换成功
    */
    bool outputVal(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env) const;
    /**
    @brief 执行模板的一个IF节点的替换。
    @param [in] pNode IF的节点
    @param [in] env 模板变量替换的环境
    @return false：执行失败，通过CwxHtmlTmplEnv的getErrMsg()获取失败的原因；true：替换成功
    */
    bool outputIf(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env) const;
    /**
    @brief 执行模板的一个LOOP节点的替换。
    @param [in] pNode LOOP的节点
    @param [in] env 模板变量替换的环境
    @return false：执行失败，通过CwxHtmlTmplEnv的getErrMsg()获取失败的原因；true：替换成功
    */
    bool outputLoop(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env) const;
    /**
    @brief 获取一个变量内容。
    @param [in] pNode 变量节点
    @param [in] env 模板变量替换的环境
    @param [in] bVal 是否为变量取值
    @return NULL：变量不存在；否则：变量的值
    */
    char const* getValValue(CwxHtmlTmplTagNode* pNode, CwxHtmlTmplEnv& env, bool bVal) const;
    /**
    @brief 根据变量定义，初始化一个变量节点。
    @param [out] node 变量节点
    @param [in] name 变量的名字
    @param [in] szObjectName 变量所属的对象名
    @param [in] ucSysAttr 变量的系统属性
    @return void
    */
    void initValNode(CwxHtmlTmplTagNode& node, char const* szName, char const* szObjectName, CWX_UINT8 ucSysAttr) const;
    ///清空模板对象
    void clear();
    ///解析REPLACE的语法，false：失败；true：成功
    bool parseReplace();
    ///解析CONST的语法，false：失败；true：成功
    bool parseConst();
    ///解析变量定义的语法，false：失败；true：成功
    bool parseValFormat();
    /**
    @brief 解析LOOP语法。
    @param [in] pPrev LOOP的前一个节点
    @return LOOP节点，NULL表示失败
    */
    CwxHtmlTmplTagNode* parseLoopBegin(CwxHtmlTmplTagNode* pPrev);
    /**
    @brief 解析IF语法。
    @param [in] pPrev IF的前一个节点
    @return IF节点，NULL表示失败
    */
    CwxHtmlTmplTagNode* parseIfBegin(CwxHtmlTmplTagNode* pPrev);
    /**
    @brief 解析ELIF语法。
    @param [in] pPrev ELIF的前一个节点
    @return ELIF节点，NULL表示失败
    */
    CwxHtmlTmplTagNode* parseIfElif(CwxHtmlTmplTagNode* pPrev);
    /**
    @brief 解析ELSE语法。
    @param [in] pPrev ELSE的前一个节点
    @return ELSE节点，NULL表示失败
    */
    CwxHtmlTmplTagNode* parseIfElse(CwxHtmlTmplTagNode* pPrev);
    /**
    @brief 解析模板中的一行文本，包括其中定义的变量。
    @param [in] pPrev 本行的的前一个节点
    @return 本行的末节点，NULL表示失败
    */
    CwxHtmlTmplTagNode* parseLine(CwxHtmlTmplTagNode* pPrev);
    ///判断一个语法行是否模板的字符集语法
    bool isCharsetDef();
    ///判断一个语法行是否模板的REPLACE语法
    bool isReplaceDef();
    ///判断一个语法行是否模板的CONST语法
    bool isConstDef();
    ///判断一个语法行是否模板的变量格式定义语法
    bool isValFormatDef();
    ///判断一个语法行是否模板的LOOP开始语法
    bool isLoopBeginDef();
    ///判断一个语法行是否模板的LOOP结束语法
    bool isLoopEndDef();
    ///判断一个语法行是否模板的IF开始语法
    bool isIfBeginDef();
    ///判断一个语法行是否模板的ELIF开始语法
    bool isIfElseIfDef();
    ///判断一个语法行是否模板的ELSE开始语法
    bool isIfElseDef();
    ///判断一个语法行是否模板的IF的结束语法
    bool isIfEndDef();
    ///判断一行是否以【续行符】结束
    bool isLineContinue(char const*& szLineEnd);
    ///判断模板的字符集是否为GBK或GB2312
    bool isGbkTmpl();
    ///从模板定义内容中读取一行，以续行符分割的行为一行
    bool readLine();
    /**
    @brief 解析XML格式的语法。
    @param [in] szXml XML格式的语法
    @param [out] lsAttrs 语法中的属性定义
    @return false：解析失败；true：解析成功
    */
    bool parseXml(char const* szXml, list<pair<char*, char*> >const *& lsAttrs);
    ///获取模板定义的字符集，默认为UTF-8
    bool parseCharset();
    /**
    @brief 解析当前行的一个语法命令，获取语法中定义的属性。
    @param [out] lsAttrs 语法中的属性定义
    @return false：解析失败；true：解析成功
    */
    bool prepareCmd(list<pair<char*, char*> >& lsAttrs);
    /**
    @brief 提取一个变量所属的对象名。
    @param [in] szValName 变量的名字
    @param [out] szObjName 对象的名字，NULL表示没有限定对象的名字
    @return void
    */
    void parseValObject(char const* szValName, char const*& szObjName);
    ///判断指定的对象名是否存在
    bool isExistObject(char const* szObjName);
    /**
    @brief 解析一个变量节点。
    @param [in,out] pTag 变量的tag
    @param [in] szName 变量的名字
    @param [in] szAttr 变量的属性
    @param [in] szLength 变量的长度
    @param [in] szDefault 变量的缺省值
    @param [in] szEscape 变量的ESCAPE变换对象名
    @param [in] szArgument ESCAPE的参数
    @return false：失败；true：成功
    */
    bool parseVal(CwxHtmlTmplTagNode* pTag, char const* szName, char const* szAttr, char const* szLength, char const* szDefault, char const* szEscape, char const* szArgument);
    /**
    @brief 检查当前的输出BUF的空间是否足够。
    @param [in] env 模板输出的环境对象
    @param [in] uiSize 内容输出需要的BUF空间大小
    @return false：空间不足；true：空间足够
    */
    inline bool checkBufSize(CwxHtmlTmplEnv& env, CWX_UINT32 uiSize) const
    {
        if (env.getBufLeft() < uiSize)
        {
            CwxCommon::snprintf(env.m_szErr1K, env.m_uiErrBufLen, "Output text's size too small, left:%u, need:%u.", env.getBufLeft(), uiSize);
            return false;
        }
        return true;
    }
    /**
    @brief 检查If的条件是否为true。
    @param [in] szVal name变量的值
    @param [in] pIfTag if tag
    @param [in] env 模板输出的环境对象
    @return false：条件不成立；true：条件成立
    */
    inline bool checkIfCondition(char const* szVal, CwxHtmlTmplTagNode* pIfTag, CwxHtmlTmplEnv& env)  const
    {
        bool bRet = false;
        if (pIfTag->m_tag.m_if.m_pCompareObj)
        {
           bRet = pIfTag->m_tag.m_if.m_pCompareObj->compare(szVal, pIfTag->m_tag.m_if.m_szData, pIfTag->m_tag.m_if.m_szArgument, this, &env);
        }
        else
        {
            size_t len = szVal?strlen(szVal):0;
            if (!szVal || !len || ((1==len) && ('0'==szVal[0])))
            {
                bRet = false;
            }
            else
            {
                bRet = true;
            }
        }
        return pIfTag->m_tag.m_if.m_bReverse?!bRet:bRet;
    }
    /**
    @brief 检查elif的条件是否为true。
    @param [in] szVal name变量的值
    @param [in] pElifTag elif tag
    @param [in] env 模板输出的环境对象
    @return false：条件不成立；true：条件成立
    */
    inline bool checkElifCondition(char const* szVal, CwxHtmlTmplTagNode* pElifTag, CwxHtmlTmplEnv& env) const
    {
        bool bRet = false;
        if (pElifTag->m_tag.m_elif.m_pCompareObj)
        {
            bRet = pElifTag->m_tag.m_elif.m_pCompareObj->compare(szVal, pElifTag->m_tag.m_elif.m_szData, pElifTag->m_tag.m_elif.m_szArgument, this, &env);
        }
        else
        {
            size_t len = szVal?strlen(szVal):0;
            if (!szVal || !len || ((1==len) && ('0'==szVal[0])))
            {
                bRet = false;
            }
            else
            {
                bRet = true;
            }
        }
        return pElifTag->m_tag.m_elif.m_bReverse?!bRet:bRet;
    }
private:
    ///禁止执行赋值操作
    CwxHtmlTmpl& operator=(CwxHtmlTmpl const& )
    {
        return *this;
    }
private:
    string                  m_strTmplName;///<模板的名字
    string                  m_strTmplFile;///<模板对应的定义文件名字
    string                  m_strCharset;///<模板定义的字符集
    bool                    m_bCaseInsensitive;///<直行字符替换的时候，是否大小写不敏感
    map<string, string>      m_attrTmplMap;///<模板的其他属性
    CwxReplBase*            m_repl;///<模板的字符替换器
    CwxHtmlTmplTagNode      m_root;///<模板语法树的根节点
    CwxHtmlValMap            m_valFormatMap;///<变量格式定义MAP
    CwxHtmlConstMap          m_constMap;///<常量格式定义MAP
    CwxTypePool<CwxHtmlTmplTagNode> m_tagPool;///<节点的MEM POOL
    CwxCharPool              m_memPool;///<字符的MEM POOL
    string                  m_strErr;///<错误信息

    CwxMemLineReader        m_tmplLineReader;///<中间的模板行读取器对象
    CwxXmlConfigParser      m_cmdParse;///<模板语法的XML解析对象
    string                  m_strLine;///<模板行的变量对象
    CWX_UINT32              m_uiLineBegin;///<当前解析到的模块的开始行号。
    CWX_UINT32              m_uiLineEnd;///<当前解析到的模块的结束行号。
    char*                   m_szTmpBuf;///<临时空间，供XML解释使用
    CWX_UINT32              m_uiTmpBufLen;///<临时空间的大小
    CWX_UINT32              m_uiReplaceNum;///<执行字符替换的字符串的数量
    list<string>            m_loopList;///<解析过程中的loop嵌套层次
    static CwxHtmlTmplCmpMap   m_cmpMap;///<比较函数对象的MAP
    static CwxHtmlTmplEscapeMap m_escapeMap;///<excape对象函数的MAP
};
///模板对象的MAP
typedef map<string const, CwxHtmlTmpl*> CwxFileTmplHash;


CWINUX_END_NAMESPACE


#include "CwxPost.h"

#endif
