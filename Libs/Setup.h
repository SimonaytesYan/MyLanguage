#ifndef __SYM_TREE_SETUP__
#define __SYM_TREE_SETUP__

#include <cstddef>

//#define DEBUG

enum NODE_TYPES
{
    UNDEF_NODE_TYPE = -1,
    TYPE_FICT       = 0,
    TYPE_NUM        = 1,        //!standart number
    TYPE_VAR        = 2,        //!standart number
    TYPE_SYMB       = 3,
    TYPE_KEYWORD    = 4,
    TYPE_OP         = 6,        //!standart number
    TYPE_FUNCTION   = 8,        //!standart number
    TYPE_CALL       = 9,       //!standart number
    TYPE_RETURN     = 10,        //!standart number
};

enum KEYWORD_TYPES
{
    UNDEF_KEYWORD_TYPE = 0,
    KEYWORD_VAR        = 1,
    KEYWORD_IF         = 2,
    KEYWORD_THEN       = 3,
    KEYWORD_ELSE       = 4,
    KEYWORD_BEGIN      = 5,
    KEYWORD_END        = 6,
    KEYWORD_WHILE      = 7,
    KEYWORD_DO         = 8,
    KEYWORD_FUNCTION   = 9,
    KEYWORD_RETURN     = 10,
    KEYWORD_CALL       = 11,
};

enum OPER_TYPES
{
    UNDEF_OPER_TYPE = 0,
    OP_PLUS         = 1,
    OP_SUB          = 2,
    OP_MUL          = 3,
    OP_DIV          = 4,
    OP_POW          = 5,
    OP_IN           = 6,
    OP_OUT          = 7,
    OP_IS_EQ        = 8,
    OP_IS_B         = 9,
    OP_IS_S         = 10,
    OP_IS_BE        = 11,
    OP_IS_SE        = 12,
    OP_IS_NE        = 13,
    OP_NOT          = 14,
    OP_AND          = 15,
    OP_OR           = 16,
    OP_EQ           = 17,
    OP_SQRT         = 18,
};

struct Keyword_and_stundart_t
{
    KEYWORD_TYPES keyword;
    int           stundart_number;
};

const Keyword_and_stundart_t KEYWORD_TO_STANDART[] = {{KEYWORD_VAR,       7},
                                                      {KEYWORD_IF,        3},
                                                      {KEYWORD_THEN,      -1},
                                                      {KEYWORD_ELSE,      4},
                                                      {KEYWORD_BEGIN,     -1},
                                                      {KEYWORD_END,       -1},
                                                      {KEYWORD_WHILE,     5},
                                                      {KEYWORD_DO,        -1},
                                                      {KEYWORD_FUNCTION,  8},
                                                      {KEYWORD_RETURN,    10},
                                                      {KEYWORD_CALL,      9}};

struct Node_t 
{
    NODE_TYPES type = UNDEF_NODE_TYPE;
    union
    {
        int           symb = 0;
        int           dbl;
        OPER_TYPES    op;
        char*         var;
        char*         function;
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

const int STD_OPERATORS_NUM = 18;

const Operator_t STD_OPERATORS[] = {
                                    {"+",    OP_PLUS },
                                    {"-",    OP_SUB  },
                                    {"*",    OP_MUL  },
                                    {"/",    OP_DIV  },
                                    {"^",    OP_POW  },
                                    {"in",   OP_IN   },
                                    {"out",  OP_OUT  },
                                    {"==",   OP_IS_EQ},
                                    {">",    OP_IS_B },
                                    {"<",    OP_IS_S },
                                    {">=",   OP_IS_BE},
                                    {"<=",   OP_IS_SE},
                                    {"!=",   OP_IS_NE},
                                    {"!",    OP_NOT},
                                    {"and",  OP_AND  },
                                    {"or",   OP_OR   },
                                    {"=",    OP_EQ   },
                                    {"sqrt", OP_SQRT},
                                   };

const int KEYWORDS_NUM = 11;

const Keyword_t KEYWORDS[] = {
                                {"var",      KEYWORD_VAR},
                                {"if",       KEYWORD_IF},
                                {"then",     KEYWORD_THEN},
                                {"else",     KEYWORD_ELSE},
                                {"begin",    KEYWORD_BEGIN},
                                {"end",      KEYWORD_END},
                                {"while",    KEYWORD_WHILE},
                                {"do",       KEYWORD_DO},
                                {"function", KEYWORD_FUNCTION},
                                {"return",   KEYWORD_RETURN},
                                {"call",     KEYWORD_CALL},
                             };

void PrintElemInLog(Node_t elem);

#include "BinaryTree/BinaryTree.h"

struct Program_t 
{
    Node*  comands     = nullptr;;
    size_t comands_num = 0;
};


#endif