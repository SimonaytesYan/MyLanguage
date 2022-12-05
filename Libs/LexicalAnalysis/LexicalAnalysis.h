#ifndef __SYM_LEXICAL_ANAL__
#define __SYM_LEXICAL_ANAL__

#include "../Libs/TreeSetup.h"

const int STD_OPERATORS_NUM = 5;

struct Operator_t
{
    const char*      name = "";
    const OPER_TYPES code = UNDEF_OPER_TYPE; 
};

const Operator_t STD_OPERATORS[] = {
                                    {"+", OP_PLUS},
                                    {"-", OP_SUB},
                                    {"*", OP_MUL},
                                    {"/", OP_DIV},
                                    {"^", OP_POW},
                                   };

Node* GetProgramFromFile(const char* program_file_name);

#endif //__SYM_LEXICAL_ANAL__