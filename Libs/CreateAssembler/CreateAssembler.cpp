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

static List VARS  = {}; //! This list stores vars:      name of variable + addres in memory(offset from addres stored in rdx if var is local) 
static List FUNCS = {}; //! This list stores fucnitons: name of function + number of label indication the beginning of the function 

const ListElem_t START_SCOPE       = {"#", 0};
const ListElem_t START_FUNC_SCOPE  = {"*", 0};
const char       OFFSET_REGISTER[] = "rdx";

int LabelCounter()
{
    static int cnt = 0;
    cnt++;
    return cnt;
}

int AddVar(const char* var);

int GetVarIndex(const char* var, bool* is_local);

int PutVar(Node* node, FILE* output_file);

int GetVar(Node* node, FILE* output_file);

int PutNodeInFile(Node* node, FILE* output_file);

int PutKeyword(Node* node, FILE* output_file);

int PutOperator(Node* node, FILE* output_file);

int PutIf(Node* node, FILE* output_file);

int PutWhile(Node* node, FILE* output_file);

int PutFunction(Node* node, FILE* output_file);

int PutReturn(Node* node, FILE* output_file);

void StartScope();

void StartFuncScope();

int EndScope();

int GetArgsInFunction(Node* node, FILE* output_file);


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

    return 0;
}

void StartScope()
{
    ListInsert(&VARS, START_SCOPE, VARS.size);
}

void StartFuncScope()
{
    ListInsert(&VARS, START_FUNC_SCOPE, VARS.size);
}

int EndScope()
{
    int begin = 0;
    ListBegin(&VARS, &begin);
    int index = 0;
    ListEnd(&VARS, &index);
    
    while (index != -1)
    {
        if (strcmp(VARS.data[index].val.name, START_SCOPE.name) == 0)
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
int GetVarIndex(const char* var, bool* is_local)
{
    *is_local = false;
    int index   = 0;
    int list_end = 0;

    ListBegin(&VARS, &index);
    ListEnd(&VARS, &list_end);
    while (index != -1)
    {
        printf("VARS[i] = <%s>\n", VARS.data[index].val.name);
        if (!strcmp(VARS.data[index].val.name, START_FUNC_SCOPE.name))
            *is_local = true;

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

    #ifdef DEBUG
        printf("end asm tree\n");
    #endif
    fclose(fp);
    return 0;
}

int PutVar(Node* node, FILE* output_file)
{
    bool is_local = false;
    int  index    = GetVarIndex(VAL_VAR(node), &is_local);

    if (index == -1)
    {
        printf(RED "Unknown var <%s>\n" DEFAULT_COLOR, VAL_VAR(node));
        CheckSyntaxError(0, node, -1);
    }

    if (is_local)
        fprintf(output_file, "push [%d + %s]\n", index, OFFSET_REGISTER);
    else
        fprintf(output_file, "push [%d]\n", index);
    
    return 0;
}

/*!
 Put in asm file line to take var from processor stack and put it in processor RAM
 @param[in] node Need to take from it name of var
 @return         error code  
*/
int GetVar(Node* node, FILE* output_file)
{
    bool is_local = false;
    int  index    = GetVarIndex(VAL_VAR(node), &is_local);
    
    if (index == -1)
    {
        printf(RED "Unknown var <%s>\n" DEFAULT_COLOR, VAL_VAR(node));
        CheckSyntaxError(0, node, -1);
    }

    if (is_local)
        fprintf(output_file, "pop [%d + %s]\n", index, OFFSET_REGISTER);
    else
        fprintf(output_file, "pop [%d]\n", index);
    
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
            
            fprintf(output_file, "in\n");
            GetVar(R(node), output_file); 
            break;
        }
        case OP_EQ:
        {
            printf("Equal\n");
            CheckSyntaxError(IS_VAR(L(node)), L(node), -1);

            ReturnIfError(PutNodeInFile(R(node), output_file));

            GetVar(L(node), output_file);
            break;
        }

    }
    return 0;
}

int PutIf(Node* node, FILE* output_file)
{    
    int else_label     = LabelCounter();
    int end_else_label = LabelCounter();

    CheckSyntaxError(L(node) != nullptr, L(node), -1);
    PutNodeInFile(L(node), output_file);                    //<cond
    fprintf(output_file, "push 0\n");
    fprintf(output_file, "jne label%d\n", else_label);      //<go to else branch

    StartScope();
    PutNodeInFile(RL(node), output_file);                   //<true branch
    fprintf(output_file, "jmp label%d\n", end_else_label);  //<skip else branch
    EndScope();

    fprintf(output_file, "label%d:\n", else_label);         //<else branch
    StartScope();
    PutNodeInFile(RR(node), output_file);            
    EndScope();

    fprintf(output_file, "label%d:\n", end_else_label);
    
    return 0;
}

int PutWhile(Node* node, FILE* output_file)
{ 
    CheckSyntaxError(L(node) != nullptr, L(node), -1);

    int start_loop_label = LabelCounter();
    int end_loop_label   = LabelCounter();
            
    fprintf(output_file, "label%d:\n", start_loop_label);
    PutNodeInFile(L(node), output_file);                        //<cond
    fprintf(output_file, "push 0\n");
    fprintf(output_file, "jne label%d\n", end_loop_label);      //<end loop

    StartScope();                                               //<loop body
    PutNodeInFile(R(node), output_file);                    
    EndScope();
    fprintf(output_file, "jmp label%d\n", start_loop_label);   //<end loop
            
    fprintf(output_file, "label%d:\n", end_loop_label);

    return 0;
}

int GetArgsInFunction(Node* node, FILE* output_file)
{
    if (node == nullptr)
        return 0;

    if (IS_VAR(node))
    {
        ReturnIfError(AddVar(VAL_VAR(node)));
        ReturnIfError(GetVar(node, output_file));
    }
    
    ReturnIfError(GetArgsInFunction(L(node), output_file));
    ReturnIfError(GetArgsInFunction(R(node), output_file));

    return 0;
}

int PutFunction(Node* node, FILE* output_file)
{
    int skip_label  = LabelCounter();
    int start_label = LabelCounter();

    StartScope();                                           //<Start new scope
    StartFuncScope();                                       //<All variable, that will created is local

    fprintf(output_file, "jmp label%d\n", skip_label);      //<jump to skip_label
            
    fprintf(output_file, "label%d:\n", start_label);        //<start_function_label
    
    GetArgsInFunction(L(node), output_file);                //<get args

    PutNodeInFile(R(node), output_file);                    //<function code

    fprintf(output_file, "label%d:\n", skip_label);         //<skip_label

    EndScope();

    return 0;
}

int PutKeyword(Node* node, FILE* output_file)
{
    switch (VAL_KEYWORD(node))
    {
        case KEYWORD_VAR:
        {
            #ifdef DEBUG
                printf("var\n");
            #endif
            CheckSyntaxError(R(node) != nullptr && IS_VAR(R(node)), R(node), -1);
            printf("add var <%s>\n", VAL_VAR(R(node)));
            ReturnIfError(AddVar(VAL_VAR(R(node))));
            return 0;
        }
        case KEYWORD_IF:
        {
            #ifdef DEBUG
                printf("if\n");
            #endif
            ReturnIfError(PutIf(node, output_file));
            break;
        }

        case KEYWORD_WHILE:
        {
            ReturnIfError(PutWhile(node, output_file));
            break;
        }

        default:
            break;
    }
    return 0;
}

int PutReturn(Node* node, FILE* output_file)
{
    ReturnIfError(PutNodeInFile(L(node), output_file));
    ReturnIfError(PutNodeInFile(R(node), output_file));

    fprintf(output_file, "ret\n");
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
            #ifdef DEBUG
                printf("fict\n");
            #endif
            PutNodeInFile(L(node), output_file);
            PutNodeInFile(R(node), output_file);
            break;
        }
        case TYPE_VAR:
        {
            #ifdef DEBUG
                printf("put variable\n");
            #endif
            ReturnIfError(PutVar(node, output_file));
            break;
        }
        case TYPE_NUM:
        {
            #ifdef DEBUG
                printf("num\n");
            #endif
            fprintf(output_file, "push %d\n", VAL_N(node));
            break;
        }
        case TYPE_KEYWORD:
        {
            #ifdef DEBUG
                printf("keyword ");
            #endif
            ReturnIfError(PutKeyword(node, output_file));
            break;
        }

        case TYPE_FUNCTION:
        {
            #ifdef DEBUG
                printf("function\n");
            #endif
            ReturnIfError(PutFunction(node, output_file));
            break;
        }

        case TYPE_RETURN:
        {
            #ifdef DEBUG
                printf("return\n");
            #endif
            ReturnIfError(PutReturn(node, output_file));
            break;
        }

        case TYPE_OP:
        {
            #ifdef DEBUG
                printf("operator\n");
            #endif
            ReturnIfError(PutOperator(node, output_file));
            break;
        }
        default:
            break;
    }

    return 0;
}