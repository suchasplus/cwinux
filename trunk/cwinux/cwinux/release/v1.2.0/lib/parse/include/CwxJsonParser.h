#ifndef JSON_PARSER_H
#define JSON_PARSER_H

/*
版权声明：
    本软件为个人所有，遵循GNU LGPL（http://www.gnu.org/copyleft/lesser.html），
但有以下例外：
    腾讯公司及与腾讯公司有直接业务与合作关系的公司不得使用此软件。原因可参考：
http://it.sohu.com/20100903/n274684530.shtml
    联系方式：email:cwinux@gmail.com；微博:http://t.sina.com.cn/cwinux
*/

/* JSON_parser.h */

#include "CwxPre.h"
#include "CwxType.h"


#include <stddef.h>

CWINUX_BEGIN_NAMESPACE

typedef CWX_INT64 JSON_int_t;
#define JSON_PARSER_INTEGER_SSCANF_TOKEN "%"PRId64
#define JSON_PARSER_INTEGER_SPRINTF_TOKEN "%"PRId64



typedef enum 
{
    JSON_T_NONE = 0,
    JSON_T_ARRAY_BEGIN,
    JSON_T_ARRAY_END,
    JSON_T_OBJECT_BEGIN,
    JSON_T_OBJECT_END,
    JSON_T_INTEGER,
    JSON_T_FLOAT,
    JSON_T_NULL,
    JSON_T_TRUE,
    JSON_T_FALSE,
    JSON_T_STRING,
    JSON_T_KEY,
    JSON_T_MAX
} JSON_type;

typedef struct JSON_value_struct
{
    union 
    {
        JSON_int_t integer_value;
        
        double float_value;
        
        struct
        {
            const char* value;
            size_t length;
        } str;
    } vu;
} JSON_value;

typedef struct JSON_parser_struct* JSON_parser;

/*! \brief JSON parser callback 

    \param ctx The pointer passed to new_JSON_parser.
    \param type An element of JSON_type but not JSON_T_NONE.    
    \param value A representation of the parsed value. This parameter is NULL for
        JSON_T_ARRAY_BEGIN, JSON_T_ARRAY_END, JSON_T_OBJECT_BEGIN, JSON_T_OBJECT_END,
        JSON_T_NULL, JSON_T_TRUE, and SON_T_FALSE. String values are always returned
        as zero-terminated C strings.

    \return Non-zero if parsing should continue, else zero.
*/    
typedef int (*JSON_parser_callback)(void* ctx, int type, const struct JSON_value_struct* value);


/*! \brief The structure used to configure a JSON parser object 
    
    \param depth If negative, the parser can parse arbitrary levels of JSON, otherwise
        the depth is the limit
    \param Pointer to a callback. This parameter may be NULL. In this case the input is merely checked for validity.
    \param Callback context. This parameter may be NULL.
    \param depth. Specifies the levels of nested JSON to allow. Negative numbers yield unlimited nesting.
    \param allowComments. To allow C style comments in JSON, set to non-zero.
    \param handleFloatsManually. To decode floating point numbers manually set this parameter to non-zero.
    
    \return The parser object.
*/
typedef struct
{
    JSON_parser_callback     callback;
    void*                    callback_ctx;
    int                      depth;
    int                      allow_comments;
    int                      handle_floats_manually;
} JSON_config;


/**
    @brief Initializes the JSON parser configuration structure to default values.
    The default configuration is
    - 127 levels of nested JSON (depends on JSON_PARSER_STACK_SIZE, see json_parser.c)
    - no parsing, just checking for JSON syntax
    - no comments
    @param [in] config  Used to configure the parser.
*/
CWX_API void init_JSON_config(JSON_config* config);

/**
    @brief Create a JSON parser object     
    @param [in] config Used to configure the parser. Set to NULL to use the default configuration. 
        See init_JSON_config    
    @return The parser object.
*/
CWX_API extern JSON_parser new_JSON_parser(JSON_config* config);

/*! \brief Destroy a previously created JSON parser object. */
CWX_API extern void delete_JSON_parser(JSON_parser jc);

/*! \brief Parse a character.

    \return Non-zero, if all characters passed to this function are part of are valid JSON.
*/
CWX_API extern int JSON_parser_char(JSON_parser jc, int next_char);

/*! \brief Finalize parsing.

    Call this method once after all input characters have been consumed.
    
    \return Non-zero, if all parsed characters are valid JSON, zero otherwise.
*/
CWX_API extern int JSON_parser_done(JSON_parser jc);

/*! \brief Determine if a given string is valid JSON white space 

    \return Non-zero if the string is valid, zero otherwise.
*/
CWX_API extern int JSON_parser_is_legal_white_space_string(const char* s);


    

CWINUX_END_NAMESPACE
#include "CwxPost.h"

#endif /* JSON_PARSER_H */
