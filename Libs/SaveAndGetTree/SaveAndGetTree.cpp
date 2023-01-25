#include "SaveAndGetTree.h"

typedef char* Elem;

#include "../Stack.h"

#define DEBUG

const int MAX_NUMBER_LENGHT = 10;

Stack VARS = {};
Stack FUNC = {};

int   GetVarsFromTree(Node* node);
char* GetFuncArgs(Node* node, int* number_var);
int   PutVars(Node* root, FILE* fp);
int   PutFunctions(Node* root, FILE* fp);
int   GetFuncFromTree(Node* node);
int   SaveNodeInFile(const Node* node, FILE* fp);
int   FindFunction(const char* func);
int   FindVar(const char* var);
int   AddVar(const char* var);

int SaveTreeInFile(const Tree* tree, const char* path)
{
    CHECK(tree == nullptr, "tree = nullptr", -1);
    CHECK(path == nullptr, "path = nullptr", -1);

    FILE* fp = fopen(path, "w");

    ReturnIfError(PutVars(tree->root, fp));
    ReturnIfError(PutFunctions(tree->root, fp));

    fprintf(fp, "{ ");
    ReturnIfError(SaveNodeInFile(tree->root, fp));
    fprintf(fp, "\n}");

    for(int i = 0; i < FUNC.size; i++)
        free(FUNC.data[i]);

    StackDtor(&VARS);
    StackDtor(&FUNC);

    fclose(fp);

    return 0;
}

int PutVars(Node* root, FILE* fp)
{
    StackCtor(&VARS, 0);
    ReturnIfError(GetVarsFromTree(root));

    fprintf(fp, "%d\n", VARS.size);
    for(int i = 0; i < VARS.size; i++)
        fprintf(fp, "%s\n", VARS.data[i]);
    fprintf(fp, "\n");
    
    return 0;
}

int PutFunctions(Node* root, FILE* fp)
{
    StackCtor(&FUNC, 0);
    ReturnIfError(GetFuncFromTree(root));

    fprintf(fp, "%d\n", FUNC.size);
    for(int i = 0; i < FUNC.size; i++)
        fprintf(fp, "%s\n", FUNC.data[i]);
    fprintf(fp, "\n");

    return 0;
}

int AddVar(char* var)
{
    for(int i = 0; i < VARS.size; i++)
    {
        if (!strcmp(VARS.data[i], var))
            return 0;
    }
    
    return StackPush(&VARS, var);
}

int GetVarsFromTree(Node* node)
{
    if (node == nullptr) return 0;
    if (IS_VAR(node))
        ReturnIfError(AddVar(VAL_VAR(node)));

    ReturnIfError(GetVarsFromTree(L(node)));
    ReturnIfError(GetVarsFromTree(R(node)));
    
    return 0;
}

char* GetFuncArgs(Node* node, int* number_var)
{
    if (node == nullptr)
        return nullptr;
    if (TYPE(node) == TYPE_FICT)
    {
        char* left  = nullptr;
        char* right = nullptr;

        left  = GetFuncArgs(L(node), number_var);
        right = GetFuncArgs(R(node), number_var);

        
        int leftLen  = 0;
        if (left != nullptr) leftLen = strlen(left);

        int rightLen = 0;
        if (right != nullptr) rightLen = strlen(right);
        
        char* result = (char*)calloc(leftLen + rightLen + 2, sizeof(char));

        if (left != nullptr) strcpy(result, left);

        result[leftLen] = ' ';
        
        if (right != nullptr) strcpy(result + leftLen + 1, right);
        
        free(left);
        free(right);

        return result;
    }
    else
    {
        (*number_var)++;
        char* result = (char*)calloc(strlen(VAL_VAR(node)) + 1, sizeof(char));
        strcpy(result, VAL_VAR(node));
        return result;
    }
}

int GetFuncFromTree(Node* node)
{
    if (node == nullptr) return 0;

    if (IS_FUNC(node))
    {
        int number_vars = 0;
        char* arguments = GetFuncArgs(L(node), &number_vars);

        int  func_name_len      = strlen(VAL_FUNC(node));
        char number_vars_str[MAX_NUMBER_LENGHT + 1] = "";

        for(int i = 0; i < MAX_NUMBER_LENGHT; i++)
        {
            number_vars_str[i] = ' ';
            if (number_vars > 0)
            {
                number_vars_str[i] = '0' + number_vars % 10;
                number_vars /= 10;
            }
        }
        
        char* result = (char*)calloc(func_name_len + strlen(arguments) + strlen(number_vars_str) + 1, sizeof(char));

        strcpy(result, VAL_FUNC(node));
        result[func_name_len] = ' ';

        strcpy(result + func_name_len + 1, number_vars_str);

        strcpy(result + func_name_len + strlen(number_vars_str), arguments);
        
        free(arguments);
        ReturnIfError(StackPush(&FUNC, result));
    }

    ReturnIfError(GetFuncFromTree(L(node)));
    ReturnIfError(GetFuncFromTree(R(node)));

    return 0;
}

int FindFunction(const char* func)
{
    int func_length = strlen(func);
    for(int i = 0; i < FUNC.size; i++)
    {
        if (!strncmp(FUNC.data[i], func, func_length))
            return i;
    }

    return -1;
}

int FindVar(const char* var)
{
    for(int i = 0; i < VARS.size; i++)
    {
        if (!strcmp(VARS.data[i], var))
            return i;
    }

    return -1;
}

int SaveNodeInFile(const Node* node, FILE* fp)
{
    if (node == nullptr)
        return 0;

    switch (TYPE(node))
    {
        case TYPE_KEYWORD:
            fprintf(fp, " %d 0", KEYWORD_TO_STANDART[VAL_KEYWORD(node) - 1].stundart_number);
            break;
        case TYPE_VAR:
            fprintf(fp, " %d %d", TYPE_VAR, FindVar(VAL_VAR(node)));
            break;
        case TYPE_NUM:
            fprintf(fp, " %d %d", TYPE_NUM, VAL_N(node));
            break;
        case TYPE_OP:
            fprintf(fp, " %d %d", TYPE_OP, VAL_OP(node));
            break;
        case TYPE_CALL:
            fprintf(fp, " %d %d", TYPE_CALL, FindFunction(VAL_FUNC(node)));
            break;
        case TYPE_FUNCTION:
            fprintf(fp, " %d %d", TYPE_FUNCTION, FindFunction(VAL_FUNC(node)));
            break;
        case TYPE_RETURN:
            fprintf(fp, " %d %d", TYPE_RETURN, 0);
            break;
        default:
            fprintf(fp, " %d 0", TYPE(node));
    }

    fprintf(fp, " {\n");
    ReturnIfError(SaveNodeInFile(L(node), fp));
    fprintf(fp, "\n}\n{\n");
    ReturnIfError(SaveNodeInFile(R(node), fp));
    fprintf(fp, "}");

    return 0;
}

Node* RecognizeNode(int node_type_std, int node_value)
{
    switch (node_type_std)
    {
        case 0:
            return NodeCtorFict();
        case 1:
            return NodeCtorNum(node_value);
        case 2:
            return NodeCtorVar(VARS.data[node_value]);
        case 3:
            return NodeCtorKeyword(KEYWORD_IF);
        case 4:
            return NodeCtorKeyword(KEYWORD_ELSE);
        case 5:
            return NodeCtorKeyword(KEYWORD_WHILE);
        case 6:
            return NodeCtorOp((OPER_TYPES)node_value);
        case 7:
            return NodeCtorKeyword(KEYWORD_VAR);
        case 8:
            return NodeCtorFunction(FUNC.data[node_value]);
        case 9:
            return NodeCtorCall(FUNC.data[node_value]);
        case 10:
            return NodeCtorReturn();
        
        default:
            return nullptr;
    }
}

Node* GetNodeFromFile(FILE* fp)
{
    char c = 0;
    fscanf(fp, " %c", &c);                          //! get '{'
    
    fscanf(fp, " %c", &c);                          //! if node is only { } then node is null 
    if (c == '}')
        return nullptr;

    ungetc(c, fp);

    int node_type_std = 0;
    int node_value    = 0;
    fscanf(fp, "%d %d", &node_type_std, &node_value); //!

    Node* result = RecognizeNode(node_type_std, node_value);

    if (result == nullptr)
        return nullptr;

    L(result) = GetNodeFromFile(fp);
    R(result) = GetNodeFromFile(fp);

    fscanf(fp, " %c", &c);                          //! get '}'

    return result;
}

int GetTreeFromFile(Tree* tree, const char* path)
{
    CHECK(tree == nullptr, "", -1);
    CHECK(path == nullptr, "", -1);

    FILE* fp = fopen(path, "r");

    int n_vars = 0;
    fscanf(fp, "%d", &n_vars);                                  //!Start get vars
    
    StackCtor(&VARS, n_vars);
    
    for(int i = 0; i < n_vars; i++)                                 
    {
        char var[MAX_VAR_NAME_LEN] = "";
        fscanf(fp, "%s", var);

        VARS.data[i] = (char*)calloc(strlen(var) + 1, sizeof(char));
        strcpy(VARS.data[i], var);
        
        #ifdef DEBUG
            printf("%s\n", VARS.data[i]);
        #endif
    }

    int n_funcs = 0;                                            //!Start get funcs
    fscanf(fp, "%d", &n_funcs);

    StackCtor(&FUNC, n_funcs);

    for(int i = 0; i < n_funcs; i++)
    {
        char func[MAX_VAR_NAME_LEN] = "";
        fscanf(fp, "%s", func);
        
        FUNC.data[i] = (char*)calloc(strlen(func) + 1, sizeof(char));
        strcpy(FUNC.data[i], func);

        int n_args = 0;
        fscanf(fp, "%d", &n_args);
        for(int j = 0; j < n_args; j++)
        {
            char var[MAX_VAR_NAME_LEN] = "";
            fscanf(fp, "%s", var);
        }
    }

    tree->root = GetNodeFromFile(fp);

    for(int i = 0; i < n_vars; i++)
        free(VARS.data[i]);
    for(int i = 0; i < n_funcs; i++)
        free(FUNC.data[i]);
    StackDtor(&VARS);
    StackDtor(&FUNC);

    fclose(fp);

    return 0;
}