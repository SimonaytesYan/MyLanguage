#include "RebuildCodeFromTree.h"

const int GLOVAL_VARS    = 0;
const int IN_FUNCTION    = 1;
const int AFTER_FUNCTION = 2;
const int MAIN_CODE      = 3;

#define MakeIndent(number_tabs)                  \
    {                                               \
        for(int i = 0; i < number_tabs; i++)     \
            fprintf(fp, "\t");                      \
    }

int RebuildCodeFromNode(Node* node, FILE* fp, int* number_tabs, int* program_block);

int RebuildFict(Node* node, FILE* fp, int* number_tabs, int* program_block);

int RebuildKeyword(Node* node, FILE* fp, int* number_tabs, int* program_block);

int RebuildIf(Node* node, FILE* fp, int* number_tabs, int* program_block);

int RebuildFunction(Node* node, FILE* fp, int* number_tabs, int* program_block);

int RebuildReturn(Node* node, FILE* fp, int* number_tabs, int* program_block);

int RebuildCall(Node* node, FILE* fp, int* number_tabs, int* program_block);

int RebuildArgs(Node* node, FILE* fp, bool* first, int* number_tabs, int* program_block);

int RebuildOperator(Node* node, FILE* fp, int* number_tabs, int* program_block);

int RebuildOperator(Node* node, FILE* fp, int* number_tabs, int* program_block)
{
    if (VAL_OP(node) == OP_OUT)
    {
        fprintf(fp, "%s ", STD_OPERATORS[VAL_OP(node) - 1].name);
        ReturnIfError(RebuildCodeFromNode(R(node), fp, number_tabs, program_block));
        return 0;
    }
    if (VAL_OP(node) == OP_IN)
    {
        fprintf(fp, "%s", STD_OPERATORS[VAL_OP(node) - 1].name);
        return 0;
    }
    if (VAL_OP(node) == OP_SQRT || VAL_OP(node) == OP_NOT || VAL_OP(node) == OP_SIN)
    {
        fprintf(fp, "%s( ", STD_OPERATORS[VAL_OP(node) - 1].name);
        ReturnIfError(RebuildCodeFromNode(R(node), fp, number_tabs, program_block));
        fprintf(fp, " )");
        return 0;
    }
    if (VAL_OP(node) == OP_EQ)
    {
        ReturnIfError(RebuildCodeFromNode(L(node), fp, number_tabs, program_block));
        fprintf(fp, " = ");
        ReturnIfError(RebuildCodeFromNode(R(node), fp, number_tabs, program_block));
        return 0;
    }
    fprintf(fp, "(");
    ReturnIfError(RebuildCodeFromNode(L(node), fp, number_tabs, program_block));
    fprintf(fp, " %s ", STD_OPERATORS[VAL_OP(node) - 1].name);
    ReturnIfError(RebuildCodeFromNode(R(node), fp, number_tabs, program_block));
    fprintf(fp, ")");

    return 0;
}

int RebuildArgs(Node* node, FILE* fp, bool* first, int* number_tabs, int* program_block)
{
    CHECK(fp == nullptr,    "", -1);
    CHECK(first == nullptr, "", -1);

    if (node == nullptr)
        return 0;

    if (!IS_FICT(node))
    {
        if ((*first))
            *first = false;
        else
            fprintf(fp, ", ");

        if (IS_VAR(node))
            fprintf(fp, "%s", VAL_VAR(node));
        if (IS_NUM(node))
            fprintf(fp, "%d", VAL_N(node));
        if (IS_OP(node))
            RebuildOperator(node, fp, number_tabs, program_block);
    }

    if (IS_FICT(node))
    {
        RebuildArgs(L(node), fp, first, number_tabs, program_block);
        RebuildArgs(R(node), fp, first, number_tabs, program_block);
    }

    return 0;
}

int RebuildCall(Node* node, FILE* fp, int* number_tabs, int* program_block)
{
    fprintf(fp, "call %s(", VAL_FUNC(node));
    bool first = true;
    ReturnIfError(RebuildArgs(R(node), fp, &first, number_tabs, program_block));
    fprintf(fp, ")");

    return 0;
}

int RebuildReturn(Node* node, FILE* fp, int* number_tabs, int* program_block)
{
    MakeIndent(*number_tabs);
    fprintf(fp, "return ");
    ReturnIfError(RebuildCodeFromNode(R(node), fp, number_tabs, program_block));
    fprintf(fp, ";\n");

    return 0;
}

int RebuildFunction(Node* node, FILE* fp, int* number_tabs, int* program_block)
{
    fprintf(fp, "function %s(", VAL_FUNC(node));
    bool first = true;
    RebuildArgs(L(node), fp, &first, number_tabs, program_block);
    fprintf(fp, ")\n");

    MakeIndent(*number_tabs);
    fprintf(fp, "begin\n");
    (*number_tabs)++; 
    RebuildCodeFromNode(R(node), fp, number_tabs, program_block);
    (*number_tabs)--;
    MakeIndent(*number_tabs);
    fprintf(fp, "end\n\n");

    return 0;
}

int RebuildIf(Node* node, FILE* fp, int* number_tabs, int* program_block)
{
    fprintf(fp, "if ");
    ReturnIfError(RebuildCodeFromNode(L(node), fp, number_tabs, program_block));
    fprintf(fp, " then\n");

    MakeIndent(*number_tabs);
    fprintf(fp, "begin\n");
    (*number_tabs)++;
    ReturnIfError(RebuildCodeFromNode(RL(node), fp, number_tabs, program_block)); 
    (*number_tabs)--;  

    MakeIndent(*number_tabs);
    fprintf(fp, "end\n");
    if (RR(node) != nullptr)
    {
        MakeIndent(*number_tabs);
        fprintf(fp, "else\n"); 
        MakeIndent(*number_tabs);
        fprintf(fp, "begin\n");

        (*number_tabs)++;
        ReturnIfError(RebuildCodeFromNode(RR(node), fp, number_tabs, program_block)); 
        (*number_tabs)--;  

        MakeIndent(*number_tabs);
        fprintf(fp, "end\n");
    }

    return 0;
}

int RebuildKeyword(Node* node, FILE* fp, int* number_tabs, int* program_block)
{
    MakeIndent(*number_tabs);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"

    switch (VAL_KW(node))
    {
        case KEYWORD_IF:
            ReturnIfError(RebuildIf(node, fp, number_tabs, program_block));
            break;

        case KEYWORD_WHILE:
            fprintf(fp, "while ");
            ReturnIfError(RebuildCodeFromNode(L(node), fp, number_tabs, program_block));
            fprintf(fp, "do\n");
            
            MakeIndent(*number_tabs);
            fprintf(fp, "begin\n");
            (*number_tabs)++;
            ReturnIfError(RebuildCodeFromNode(R(node), fp, number_tabs + 1, program_block));  
            (*number_tabs)--;       
            MakeIndent(*number_tabs);
            fprintf(fp, "end\n");
            break;

        default:
            break;
    }

    #pragma GCC diagnostic pop

    return 0;
}

int RebuildFict(Node* node, FILE* fp, int* number_tabs, int* program_block)
{
    if (IS_L_OP(node) || IS_L_CALL(node))
        MakeIndent(*number_tabs);
    ReturnIfError(RebuildCodeFromNode(L(node), fp, number_tabs, program_block));

    if (IS_L_OP(node) || IS_L_CALL(node))
        fprintf(fp, ";\n");
    

    if (IS_R_OP(node) || IS_R_CALL(node))
        MakeIndent(*number_tabs);
    ReturnIfError(RebuildCodeFromNode(R(node), fp, number_tabs, program_block));

    if (IS_R_OP(node) || IS_R_CALL(node))
        fprintf(fp, ";\n");

    return 0;
}

int RebuildCodeFromNode(Node* node, FILE* fp, int* number_tabs, int* program_block)
{
    if (node == nullptr)
        return 0;

    if (!IS_FICT(node) && !IS_FUNC(node))
    {
        if ((*program_block == AFTER_FUNCTION) || 
            (*program_block == GLOVAL_VARS && !IS_VAR(node) && !IS_VAR_KEYWORD(node)))
        {
            *program_block = MAIN_CODE;
            fprintf(fp, "\nbegin\n");
            (*number_tabs)++;
        }
    }

    switch (TYPE(node))
    {
        case TYPE_FICT:
            ReturnIfError(RebuildFict(node, fp, number_tabs, program_block));
            break;
        case TYPE_OP:
            ReturnIfError(RebuildOperator(node, fp, number_tabs, program_block));
            break;
        case TYPE_NUM:
            fprintf(fp, "%d", VAL_N(node));
            break;
        case TYPE_VAR:
            fprintf(fp, "%s", VAL_VAR(node));
            break;
        case TYPE_RETURN:
            ReturnIfError(RebuildReturn(node, fp, number_tabs, program_block));
            break;
        case TYPE_CALL:
            ReturnIfError(RebuildCall(node, fp, number_tabs, program_block));
            break;
        case TYPE_FUNCTION:
            *program_block = IN_FUNCTION;
            ReturnIfError(RebuildFunction(node, fp, number_tabs, program_block));
            *program_block = AFTER_FUNCTION;
            break;
        case TYPE_KEYWORD:
            ReturnIfError(RebuildKeyword(node, fp, number_tabs, program_block));
            break;

        case TYPE_CREATE_VAR:
            MakeIndent(*number_tabs);
            fprintf(fp, "var %s;\n", VAL_VAR(node));
            break;
        
        case TYPE_SYMB:
            break; 

        case UNDEF_NODE_TYPE:
            break;

        default:
            break;
    }
    return 0;
}

int RebuildCodeFromTreeInFile(const Tree* tree, const char* file_path)
{
    CHECK(tree      == nullptr, "", -1);
    CHECK(file_path == nullptr, "", -1);

    FILE* fp = fopen(file_path, "w");

    int program_block = GLOVAL_VARS;
    int number_tabs   = 0;
    RebuildCodeFromNode(tree->root, fp, &number_tabs, &program_block);

    fprintf(fp, "end\n");

    fclose(fp);

    return 0;
}