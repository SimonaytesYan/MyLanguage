#ifndef __SYM_TREE_SETUP__
#define __SYM_TREE_SETUP__

#include <cstddef>

enum NODE_TYPES
{
    UNDEF_NODE_TYPE = -1,
    TYPE_FICT       = 0,
    TYPE_VAR        = 1,
    TYPE_OP         = 2,
    TYPE_NUM        = 3,
    TYPE_SYMB       = 4,
    TYPE_KEYWORD    = 5,
};

enum KEYWORD_TYPES
{
    UNDEF_KEYWORD_TYPE = 0,
    KEYWORD_VAR        = 1,
};

enum OPER_TYPES
{
    UNDEF_OPER_TYPE = 0,
    OP_PLUS         = 1,
    OP_SUB          = 2,
    OP_MUL          = 3,
    OP_DIV          = 4,
    OP_POW          = 5,
//    OP_IN           = 6,
//    OP_OUT          = 7,
//    OP_IS_EQ        = 8,
//    OP_IS_B         = 9,
//    OP_IS_S         = 10,
//    OP_IS_BE        = 11,
//    OP_IS_SE        = 12,
//    OP_IS_NE        = 13,
//    OP_NOT          = 14,
//    OP_AND          = 15,
//    OP_OR           = 16,
    OP_EQ           = 17,
};

struct Node_t 
{
    NODE_TYPES type = UNDEF_NODE_TYPE;
    union
    {
        int           symb = 0;
        int           dbl;
        OPER_TYPES    op;
        char*         var;
        KEYWORD_TYPES keyword;
    } val;
    size_t number_cmd_line_in_text = 0;
    size_t number_cmd_in_text      = 0;
};

struct Operator_t
{
    const char*      name = "";
    const OPER_TYPES code = UNDEF_OPER_TYPE; 
};

struct Keyword_t
{
    const char* name = "";
    const KEYWORD_TYPES code = UNDEF_KEYWORD_TYPE;
};

const int MAX_VAR_SIZE      = 20;

const int STD_OPERATORS_NUM = 6;

const Operator_t STD_OPERATORS[] = {
                                    {"+", OP_PLUS},
                                    {"-", OP_SUB},
                                    {"*", OP_MUL},
                                    {"/", OP_DIV},
                                    {"^", OP_POW},
                                    {"=", OP_EQ}
                                   };

const int KEYWORDS_NUM = 1;

const Keyword_t KEYWORDS[] = {
                                {"var", KEYWORD_VAR},
                             };

void PrintElemInLog(Node_t elem);

#include "BinaryTree/BinaryTree.h"

struct Program_t 
{
    Node*  comands     = nullptr;;
    size_t comands_num = 0;
};


#endif