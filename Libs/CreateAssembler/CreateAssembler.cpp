#include "CreateAssembler.h"
#include "../Logging/Logging.h"
#include "../InAndOut/InAndOut.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"

struct ListElem_t
{
    char*  name = "";
    size_t index = 0;
};

void PrintStackElemInLog(ListElem_t val);

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

#pragma GCC diagnostic pop

int LabelCounter();

int LabelCounter()
{
    static int cnt = 0;
    cnt++;
    return cnt;
}

int  AddVar(char* var);

int  AddFunction(char* name, int label_number);

int  GetVarIndex(const char* var, bool* is_local);

int  GetFuncIndex(const char* name);

int  PutVar(Node* node, FILE* output_file);

int  GetVar(Node* node, FILE* output_file);

int  PutNodeInFile(Node* node, FILE* output_file);

int  PutKeyword(Node* node, FILE* output_file);

int  PutOperator(Node* node, FILE* output_file);

int  PutIf(Node* node, FILE* output_file);

int  PutWhile(Node* node, FILE* output_file);

int  PutFunction(Node* node, FILE* output_file);

int  PutCall(Node* node, FILE* output_file);

int  PutReturn(Node* node, FILE* output_file);

int  GetLastVarIndexToChangeOffset();

void StartScope();

void StartFuncScope();

int  EndScope();

int  GetArgsInFunction(Node* node, FILE* output_file);

int  PutArgsInCallFunc(Node* node, FILE* output_file);

//!-----------------
//!@return error code
//!------------
int AddVar(char* var)
{
    int var_index = 1;
    int index     = 0;
    int list_end  = 0;

    ListBegin(&VARS, &index);
    ListEnd(&VARS, &list_end);

    while (index != -1)
    {
        if (!strcmp(VARS.data[index].val.name, START_FUNC_SCOPE.name))
            var_index = 0;
        if (!strcmp(VARS.data[index].val.name, var))
            return -1;
        if (index == list_end)
            break;
        ListIterate(&VARS, &index);
        var_index++;
    }
    if (VARS.size == 0)
        var_index = 0;
    ListElem_t value = {var, (size_t)var_index};
    ListInsert(&VARS, value, (int)VARS.size);

    return 0;
}

//!-----------------
//!@return error code
//!------------
int AddFunction(char* name, int label_number)
{
    int index   = 0;
    int list_end = 0;
    
    DUMP_L(&FUNCS);
    ListBegin(&FUNCS, &index);
    ListEnd(&FUNCS, &list_end);
    while (index != -1)
    {
        if (!strcmp(FUNCS.data[index].val.name, name))
            return -1;
        if (index == list_end)
            break;
        ListIterate(&FUNCS, &index);
    }
    ListElem_t value = {name, (size_t)label_number};
    ListInsert(&FUNCS, value, (int)FUNCS.size);

    DUMP_L(&FUNCS);

    return 0;
}

void StartScope()
{
    ListInsert(&VARS, START_SCOPE, (int)VARS.size);
}

void StartFuncScope()
{
    ListInsert(&VARS, START_FUNC_SCOPE, (int)VARS.size);
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

int  GetFuncIndex(const char* name)
{
    int index   = 0;
    int list_end = 0;

    ListBegin(&FUNCS, &index);
    ListEnd(&FUNCS, &list_end);
    while (index != -1)
    {

        if (!strcmp(FUNCS.data[index].val.name, name))
            return (int)FUNCS.data[index].val.index;
        if (index == list_end)
            break;
        ListIterate(&FUNCS, &index);
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
        if (!strcmp(VARS.data[index].val.name, START_FUNC_SCOPE.name))
            *is_local = true;

        if (!strcmp(VARS.data[index].val.name, var))
            return (int)VARS.data[index].val.index;
        if (index == list_end)
            break;
        ListIterate(&VARS, &index);
    }

    return -1;
}

int  GetLastVarIndexToChangeOffset()
{
    int list_end = 0;
    ListEnd(&VARS, &list_end);

    if (list_end == -1)
        return -1;

    return (int)VARS.data[list_end].val.index;
}

int CreateAsmFromTree(Tree* tree, const char* output_file)
{
    TreeCheck(tree);

    FILE* fp = fopen(output_file, "w");
    ListCtor(&VARS,  0);
    ListCtor(&FUNCS, 0);
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
    #ifdef DEBUG
        printf("start operator\n");
    #endif
    if (IS_OP(node) && VAL_OP(node) != OP_OUT && VAL_OP(node) != OP_EQ && VAL_OP(node) != OP_IN)
    {
        PutNodeInFile(L(node), output_file);
        PutNodeInFile(R(node), output_file);
    }

    switch (VAL_OP(node))
    {
        case OP_POW:
            fprintf(output_file, "pow\n");
            break;
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
            fprintf(output_file, "or\n");
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
        case OP_NOT:
            fprintf(output_file, "not\n");
            break;
        case OP_SQRT:
            fprintf(output_file, "sqrt\n");
            break;
        case OP_SIN:
            fprintf(output_file, "sin\n");
            break;
        case OP_OUT:
        {
            PutNodeInFile(R(node), output_file);
            fprintf(output_file, "out\n");
            break;
        }
        case OP_IN:
        {            
            fprintf(output_file, "in\n"); 
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
        case UNDEF_OPER_TYPE:
            break;

        default:
            break;

    }
    return 0;
}

int PutIf(Node* node, FILE* output_file)
{    
    int else_label     = LabelCounter();
    int end_else_label = LabelCounter();

    CheckSyntaxError(L(node) != nullptr, L(node), -1);
    PutNodeInFile(L(node), output_file);                    //!cond
    fprintf(output_file, "push 0\n");
    fprintf(output_file, "jne label%d\n", else_label);      //!go to else branch

    StartScope();
    PutNodeInFile(RL(node), output_file);                   //!true branch
    fprintf(output_file, "jmp label%d\n", end_else_label);  //!skip else branch
    EndScope();

    fprintf(output_file, "label%d:\n", else_label);         //!else branch
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
    PutNodeInFile(L(node), output_file);                        //!cond
    fprintf(output_file, "push 0\n");
    fprintf(output_file, "jne label%d\n", end_loop_label);      //!end loop

    StartScope();                                               //!loop body
    PutNodeInFile(R(node), output_file);                    
    EndScope();
    fprintf(output_file, "jmp label%d\n", start_loop_label);   //!end loop
            
    fprintf(output_file, "label%d:\n", end_loop_label);

    return 0;
}

int  PutArgsInCallFunc(Node* node, FILE* output_file)
{
    if (node == nullptr)
        return 0;
    
    if (TYPE(node) == TYPE_FICT)
    {
        ReturnIfError(PutArgsInCallFunc(L(node), output_file));
        ReturnIfError(PutArgsInCallFunc(R(node), output_file));
    }
    else
        PutNodeInFile(node, output_file);

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
    
    AddFunction(VAL_FUNC(node), start_label);

    StartScope();                                           //!Start new scope
    StartFuncScope();                                       //!All variable, that will created is local

    fprintf(output_file, "jmp label%d\n", skip_label);      //!jump to skip_label
            
    fprintf(output_file, "label%d:\n", start_label);        //!start_function_label
    
    GetArgsInFunction(L(node), output_file);                //!get args

    PutNodeInFile(R(node), output_file);                    //!function code

    fprintf(output_file, "label%d:\n", skip_label);         //!skip_label

    EndScope();

    return 0;
}

int PutKeyword(Node* node, FILE* output_file)
{
    CheckSyntaxError(node != nullptr, node, -1);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (VAL_KW(node))
    {
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
    #pragma GCC diagnostic pop

    return 0;
}

int PutReturn(Node* node, FILE* output_file)
{
    ReturnIfError(PutNodeInFile(L(node), output_file));
    ReturnIfError(PutNodeInFile(R(node), output_file));

    fprintf(output_file, "ret\n");
    return 0;
}

int CalcArgsNumber(Node* node)
{
    if (node == nullptr)
        return 0;
    
    size_t arg_num = 0;
    if (TYPE(node) == TYPE_VAR || TYPE(node))
        arg_num++;
    arg_num += CalcArgsNumber(L(node)) + CalcArgsNumber(R(node));
    
    return arg_num;
}

int  PutCall(Node* node, FILE* output_file)
{
    fprintf(output_file, "\nPUT_ARGS_%s_%d:\n", VAL_FUNC(node), LabelCounter());
    PutArgsInCallFunc(L(node), output_file);
    PutArgsInCallFunc(R(node), output_file);

    int offset_change = GetLastVarIndexToChangeOffset() + 1;
    fprintf(output_file, "push rdx\n");
    fprintf(output_file, "push %d\n", offset_change);
    fprintf(output_file, "add\n");
    fprintf(output_file, "pop rdx\n");

    if (strcmp(VAL_FUNC(node), "get_cell") == 0)
    {
        // assert(CalcArgsNumber(node) == 2);
        // if (CalcArgsNumber(node) != 2)
        //     return -1;
        fprintf(output_file, "get_cell\n");
    }
    else if (strcmp(VAL_FUNC(node), "build_cell") == 0)
    {
        // assert(CalcArgsNumber(node) == 3);
        // if (CalcArgsNumber(node) != 3)
        //     return -1;
        fprintf(output_file, "build_cell\n");
    }
    else
        fprintf(output_file, "call label%d\n", GetFuncIndex(VAL_FUNC(node)));

    fprintf(output_file, "push rdx\n");
    fprintf(output_file, "push %d\n", offset_change);
    fprintf(output_file, "sub\n");
    fprintf(output_file, "pop rdx\n");

    return 0;
}

int PutNodeInFile(Node* node, FILE* output_file)
{
    DUMP_L(&VARS);
    if (node == nullptr)
        return 0;
    if (output_file == nullptr)
        return -1;
    
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch-enum"
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

        case TYPE_CALL:
        {
            #ifdef DEBUG
                printf("return\n");
            #endif
            ReturnIfError(PutCall(node, output_file));
            break;
        }

        case TYPE_CREATE_VAR:
        {
            #ifdef DEBUG
                printf("create var\n");
            #endif
            
            printf("add var <%s>\n", VAL_VAR(node));
            ReturnIfError(AddVar(VAL_VAR(node)));
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
    #pragma GCC diagnostic pop

    return 0;
}