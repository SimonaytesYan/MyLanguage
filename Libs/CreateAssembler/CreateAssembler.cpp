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

static List VARS  = {}; //! This list stores vars:      name of variable + addres in memory(offset from addres stored in rdx) 
static List FUNCS = {}; //! This list stores fucnitons: name of function + number of label indication the beginning of the function 

const ListElem_t FICT_VAR = {"#", 0};

int LabelCounter()
{
    static int cnt = 0;
    cnt++;
    return cnt;
}

int AddVar(const char* var);

int GetVarIndex(const char* var);

int PutVar(Node* node, FILE* output_file);

int PutNodeInFile(Node* node, FILE* output_file);

int PutKeyword(Node* node, FILE* output_file);

//!-----------------
//!@return error code
//!------------
int AddVar(const char* var)
{
    int index   = 0;
    int list_end = 0;
    ListBegin(&VARS, &index);
    ListEnd(&VARS, &list_end);
    while (index != -1)
    {
        if (!strcmp(VARS.data[index].val.name, var))
            return -1;
        if (index == list_end)
            break;
        ListIterate(&VARS, &index);
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

void StartScope()
{
    ListInsert(&VARS, FICT_VAR, VARS.size);
}

int EndScope()
{
    int begin = 0;
    ListBegin(&VARS, &begin);
    int index = 0;
    ListEnd(&VARS, &index);
    
    while (index != -1)
    {
        if (strcmp(VARS.data[index].val.name, FICT_VAR.name) == 0)
        {
            ListRemove(&VARS, index);
            return 0;
        }
        ListRemove(&VARS, index);
        ListEnd(&VARS, &index);
    }
    return -1;    
}

//!-----------------
//!@return index of var in RAM and -1 if var not found
//!------------
int GetVarIndex(const char* var)
{
    int index   = 0;
    int list_end = 0;
    ListBegin(&VARS, &index);
    ListEnd(&VARS, &list_end);
    while (index != -1)
    {
        printf("VARS[i] = <%s>\n", VARS.data[index].val.name);
        if (!strcmp(VARS.data[index].val.name, var))
            return VARS.data[index].val.index;
        if (index == list_end)
            break;
        ListIterate(&VARS, &index);
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
    if (index == -1)
    {
        printf(RED "Unknown var <%s>\n" DEFAULT_COLOR, VAL_VAR(node));
        CheckSyntaxError(0, node, -1);
    }

    fprintf(output_file, "push [%d]\n", index);
    return 0;
}

int PutOperator(Node* node, FILE* output_file)
{
    printf("start operator\n");
    if (IS_OP(node) && VAL_OP(node) != OP_OUT && VAL_OP(node) != OP_EQ && VAL_OP(node) != OP_IN)
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
            fprintf(output_file, "div\n");
            break;
        case OP_AND:
            fprintf(output_file, "and\n");
            break;
        case OP_OR:
            fprintf(output_file, "and\n");
            break;
        case OP_IS_B:
            fprintf(output_file, "is_b\n");
            break;
        case OP_IS_BE:
            fprintf(output_file, "is_be\n");
            break;
        case OP_IS_EQ:
            fprintf(output_file, "is_eq\n");
            break;
        case OP_IS_NE:
            fprintf(output_file, "is_ne\n");
            break;
        case OP_IS_S:
            fprintf(output_file, "is_s\n");
            break;
        case OP_IS_SE:
            fprintf(output_file, "is_se\n");
            break;
        case OP_OUT:
        {
            PutNodeInFile(R(node), output_file);
            fprintf(output_file, "out\n");
            break;
        }
        case OP_IN:
        {
            CheckSyntaxError(R(node) != nullptr, node, -1);
            CheckSyntaxError(IS_VAR(R(node)), R(node), -1);

            int index = GetVarIndex(VAL_VAR(R(node)));
            if (index == -1)
                return -1;

            fprintf(output_file, "in\n");       
            fprintf(output_file, "pop [%d]\n", index);     
            break;
        }
        case OP_EQ:
        {
            printf("Equal\n");
            CheckSyntaxError(IS_VAR(L(node)), L(node), -1);

            int index = GetVarIndex(VAL_VAR(L(node)));
            if (index == -1)
            {
                printf(RED "Unknown var <%s>\n" DEFAULT_COLOR, VAL_VAR(L(node)));
                CheckSyntaxError(0, node, -1);
            }
            printf("index = %d\n", index);

            ReturnIfError(PutNodeInFile(R(node), output_file));
            fprintf(output_file, "pop [%d]\n", index);
            break;
        }

    }
    return 0;
}

int PutKeyword(Node* node, FILE* output_file)
{
    switch (VAL_KEYWORD(node))
    {
        case KEYWORD_VAR:
        {
            CheckSyntaxError(R(node) != nullptr && IS_VAR(R(node)), R(node), -1);
            printf("add var <%s>\n", VAL_VAR(R(node)));
            ReturnIfError(AddVar(VAL_VAR(R(node))));
            return 0;
        }
        case KEYWORD_IF:
        {
            int else_label     = LabelCounter();
            int end_else_label = LabelCounter();

            CheckSyntaxError(L(node) != nullptr, L(node), -1);
            PutNodeInFile(L(node), output_file);                    //cond
            fprintf(output_file, "push 0\n");
            fprintf(output_file, "jne label%d\n", else_label);      //go to else branch

            StartScope();
            PutNodeInFile(RL(node), output_file);                   //true branch
            fprintf(output_file, "jmp label%d\n", end_else_label);  //skip else branch
            EndScope();

            fprintf(output_file, "label%d:\n", else_label);         //else branch
            StartScope();
            PutNodeInFile(RR(node), output_file);            
            EndScope();       
            fprintf(output_file, "label%d:\n", end_else_label);

            return 0;
        }

        case KEYWORD_WHILE:
        {
            CheckSyntaxError(L(node) != nullptr, L(node), -1);

            int start_loop_label = LabelCounter();
            int end_loop_label   = LabelCounter();
            
            fprintf(output_file, "label%d:\n", start_loop_label);
            PutNodeInFile(L(node), output_file);                        //cond
            fprintf(output_file, "push 0\n");
            fprintf(output_file, "jne label%d\n", end_loop_label);      //end loop

            StartScope();                                               //loop body
            PutNodeInFile(R(node), output_file);                    
            EndScope();
            fprintf(output_file, "jmp label%d\n", start_loop_label);   //end loop
            
            fprintf(output_file, "label%d:\n", end_loop_label);
            return 0;
        }
    
        default:
            break;
    }
    return 0;
}

int PutNodeInFile(Node* node, FILE* output_file)
{
    DUMP_L(&VARS);
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
            ReturnIfError(PutKeyword(node, output_file));
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