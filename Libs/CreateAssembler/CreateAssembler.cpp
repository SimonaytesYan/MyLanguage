#include "CreateAssembler.h"
#include "../Logging/Logging.h"
#include "../InAndOut/InAndOut.h"

struct ListElem_t
{
    char*  name = "";
    size_t index = 0;
};

void PrintStackElemInLog(ListElem_t val)
{
    LogPrintf("{name = <%s>, index = %ld}", val.name, val.index);
}

#include "../List/List.h"

static List VARS = {};

int AddVar(const char* var);

int GetVarIndex(const char* var);

int PutVar(Node* node, FILE* output_file);

int PutNodeInFile(Node* node, FILE* output_file);

//!-----------------
//!@return error code
//!------------
int AddVar(const char* var)
{
    for(int i = 1; i <= VARS.size; i++)
    {
        if (!strcmp(VARS.data[i].val.name, var))
            return -1;
    }
    ListElem_t value = {(char*)var, VARS.size};
    ListInsert(&VARS, value, VARS.size);
    //int begin = 0;
    //ListBegin(&VARS, &begin);
    //printf("begin = %d\n", begin);
    //printf("VARS.data[0].name = <%s>\n", VARS.data[begin].val.name);
    //printf("VARS.data[0] = <%s>\n", VARS.data[0].val.name);

    return 0;
}

//!-----------------
//!@return index of var in RAM and -1 if var not found
//!------------
int GetVarIndex(const char* var)
{
    for(int i = 1; i <= VARS.size; i++)
    {
        printf("VARS[i] = <%s>\n", VARS.data[i].val.name);
        if (!strcmp(VARS.data[i].val.name, var))
            return VARS.data[i].val.index;
    }

    return -1;
}

int CreateAsmFromTree(Tree* tree, const char* output_file)
{
    TreeCheck(tree);

    FILE* fp = fopen(output_file, "w");
    ListCtor(&VARS, 0);
    CHECK(fp == nullptr, "fp = nullptr", -1);

    ReturnIfError(PutNodeInFile(tree->root, fp));

    fclose(fp);
    return 0;
}

int PutVar(Node* node, FILE* output_file)
{
    int index = GetVarIndex(VAL_VAR(node));
    if (index == -1) return -1;

    fprintf(output_file, "push [%d]\n", index);
    return 0;
}

int PutOperator(Node* node, FILE* output_file)
{
    printf("start operator\n");
    if (VAL_OP(node) != OP_EQ)
    {
        PutNodeInFile(L(node), output_file);
        PutNodeInFile(R(node), output_file);
    }

    switch (VAL_OP(node))
    {
        case OP_PLUS:
            fprintf(output_file, "add\n");
            break;
        case OP_SUB:
            fprintf(output_file, "sub\n");
            break;
        case OP_MUL:
            fprintf(output_file, "mul\n");
            break;
        case OP_DIV:
            fprintf(output_file, "sub\n");
            break;
        case OP_EQ:
        {
            printf("Equal\n");
            CheckSyntaxError(IS_VAR(L(node)), L(node), -1);

            printf("var = <%s>\n", VAL_VAR(L(node)));

            int index = GetVarIndex(VAL_VAR(L(node)));
            if (index == -1) return -1;
            printf("index = %d\n", index);

            ReturnIfError(PutNodeInFile(R(node), output_file));
            fprintf(output_file, "pop [%d]\n", index);
            break;
        }
        
        default:
            break;
    }
    return 0;
}

int PutNodeInFile(Node* node, FILE* output_file)
{
    if (node == nullptr)
        return 0;
    if (output_file == nullptr)
        return -1;
    
    switch (TYPE(node))
    {
        case TYPE_FICT:
        {
            PutNodeInFile(L(node), output_file);
            PutNodeInFile(R(node), output_file);
            break;
        }
        case TYPE_VAR:
        {
            ReturnIfError(PutVar(node, output_file));
            break;
        }
        case TYPE_NUM:
        {
            fprintf(output_file, "push %d\n", VAL_N(node));
            break;
        }
        case TYPE_KEYWORD:
        {
            switch (VAL_KEYWORD(node))
            {
                case KEYWORD_VAR:
                {
                    CheckSyntaxError(L(node) != nullptr && IS_VAR(L(node)), L(node), -1);
                    printf("add var <%s>\n", VAL_VAR(L(node)));
                    ReturnIfError(AddVar(VAL_VAR(L(node))));
                    break;
                }
            
            default:
                break;
            }
            break;
        }
        case TYPE_OP:
        {
            ReturnIfError(PutOperator(node, output_file));
            break;
        }
        default:
            break;
    }

    return 0;
}