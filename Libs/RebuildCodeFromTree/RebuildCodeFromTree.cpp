#include "RebuildCodeFromTree.h"

int RebuildCodeFromNode(Node* node, FILE* fp);

int RebuildOperator(Node* node, FILE* fp)
{
    if (VAL_OP(node) == OP_IN || VAL_OP(node) == OP_OUT)
    {
        fprintf(fp, "%s ", STD_OPERATORS[VAL_OP(node) - 1]);
        ReturnIfError(RebuildCodeFromNode(R(node), fp));
        fprintf(fp, ";\n");
        return 0;
    }
    if (VAL_OP(node) == OP_SQRT || VAL_OP(node) == OP_NOT)
    {
        fprintf(fp, "%s( ", STD_OPERATORS[VAL_OP(node) - 1]);
        ReturnIfError(RebuildCodeFromNode(R(node), fp));
        fprintf(fp, " )");
        return 0;
    }
    if (VAL_OP(node) == OP_EQ)
    {
        ReturnIfError(RebuildCodeFromNode(L(node), fp));
        fprintf(fp, " = ");
        ReturnIfError(RebuildCodeFromNode(R(node), fp));
        fprintf(fp, ";\n");
    }
    fprintf(fp, "(");
    RebuildCodeFromNode(L(node), fp);
    fprintf(fp, " %s ", STD_OPERATORS[VAL_OP(node) - 1]);
    RebuildCodeFromNode(R(node), fp);
    fprintf(fp, ")");

    return 0;
}

int RebuildArgs(Node* node, FILE* fp, bool* first)
{
    CHECK(fp == nullptr,    "", -1);
    CHECK(first == nullptr, "", -1);

    if (node == nullptr)
        return 0;

    if (IS_VAR(node))
    {
        if (*first)
        {
            fprintf(fp, "%s", VAL_VAR(node));
            *first = false;
        }
        else
            fprintf(fp, ", %s", VAL_VAR(node));
    }

    RebuildArgs(L(node), fp, first);
    RebuildArgs(R(node), fp, first);

    return 0;
}

int RebuildCall(Node* node, FILE* fp)
{
    fprintf(fp, "call %s(", VAL_FUNC(node));
    bool first = true;
    ReturnIfError(RebuildArgs(node, fp, &first));
    fprintf(fp, ")");

    return 0;
}

int RebuildReturn(Node* node, FILE* fp)
{
    fprintf(fp, "return ");
    ReturnIfError(RebuildCodeFromNode(R(node), fp));
    fprintf(fp, ";\n");

    return 0;
}

int RebuildFunction(Node* node, FILE* fp)
{
    fprintf(fp, "\nfunction %s(", VAL_FUNC(node));
    bool first = true;
    RebuildArgs(L(node), fp, &first);
    fprintf(fp, ")\n");

    fprintf(fp, "begin\n");
    RebuildCodeFromNode(R(node), fp);
    fprintf(fp, "end\n");

    return 0;
}

int RebuildKeyword(Node* node, FILE* fp)
{
    switch (VAL_KEYWORD(node))
    {
        case KEYWORD_IF:
            fprintf(fp, "if ");
            ReturnIfError(RebuildCodeFromNode(L(node), fp));
            fprintf(fp, " then\n");

            fprintf(fp, "begin\n");
            ReturnIfError(RebuildCodeFromNode(R(node), fp));        
            fprintf(fp, "end\n");
            break;

        case KEYWORD_WHILE:
            fprintf(fp, "while ");
            ReturnIfError(RebuildCodeFromNode(L(node), fp));
            fprintf(fp, "do\n");

            fprintf(fp, "begin\n");
            ReturnIfError(RebuildCodeFromNode(R(node), fp));        
            fprintf(fp, "end\n");
            break;
        
        case KEYWORD_VAR:
            fprintf(fp, "var ");
            ReturnIfError(RebuildCodeFromNode(R(node), fp));
            fprintf(fp, ";\n");
            break;

        default:
            break;
    }

    return 0;
}

int RebuildCodeFromNode(Node* node, FILE* fp)
{
    if (node == nullptr)
        return 0;

    switch (TYPE(node))
    {
        case TYPE_FICT:
            ReturnIfError(RebuildCodeFromNode(L(node), fp));
            ReturnIfError(RebuildCodeFromNode(R(node), fp));
            break;
        case TYPE_OP:
            ReturnIfError(RebuildOperator(node, fp));
            break;
        case TYPE_NUM:
            fprintf(fp, "%d", VAL_N(node));
            break;
        case TYPE_VAR:
            fprintf(fp, "%s", VAL_VAR(node));
            break;
        case TYPE_RETURN:
            ReturnIfError(RebuildReturn(node, fp));
            break;
        case TYPE_CALL:
            ReturnIfError(RebuildCall(node, fp));
            break;
        case TYPE_FUNCTION:
            ReturnIfError(RebuildFunction(node, fp));
            break;
        case TYPE_KEYWORD:
            ReturnIfError(RebuildKeyword(node, fp));
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

    RebuildCodeFromNode(tree->root, fp);

    fclose(fp);

    return 0;
}