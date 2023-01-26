#include "SaveAndGetTree.h"

typedef char* Elem;

#include "../Stack.h"

#define DEBUG

const int MAX_NUMBER_LENGHT = 10;

Stack VARS = {};
Stack FUNC = {};

int   GetVarsFromTree(Node* node);
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

int GetFuncFromTree(Node* node)
{
    if (node == nullptr) return 0;

    if (IS_FUNC(node))
        ReturnIfError(StackPush(&FUNC, VAL_FUNC(node)));

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
            fprintf(fp, " %d 0", KEYWORD_TO_STANDART[VAL_KW(node) - 1].stundart_number);
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
        case TYPE_CREATE_VAR:
            fprintf(fp, " %d %d", TYPE_CREATE_VAR, FindVar(VAL_VAR(node)));
            break;
        default:
            fprintf(fp, " %d 0", TYPE(node));
    }

    if (L(node) != nullptr)
    {
        fprintf(fp, " {\n");
        ReturnIfError(SaveNodeInFile(L(node), fp));
        fprintf(fp, "\n}\n");
    }
    if (R(node) != nullptr)
    {
        fprintf(fp, "{\n");
        ReturnIfError(SaveNodeInFile(R(node), fp));
        fprintf(fp, "\n}\n");
    }

    return 0;
}


Node* GetNodeFromFile(FILE* fp);

Node* RecognizeNode(int node_type_std, int node_value, FILE* fp)
{
    switch (node_type_std)
    {
        case 0:                                                 //!FICT
        {
            Node* result = NodeCtorFict();
            L(result) = GetNodeFromFile(fp);
            R(result) = GetNodeFromFile(fp);
            return result;
        }
        case 1:                                                 //!NUMBER
            return NodeCtorNum(node_value);
        case 2:                                                 //!VAR
            return NodeCtorVar(VARS.data[node_value]);
        case 3:                                                 //!IF
        {
            Node* result = NodeCtorKeyword(KEYWORD_IF);
            L(result) = GetNodeFromFile(fp);
            R(result) = GetNodeFromFile(fp);
            return result;
        }
        case 4:                                                 //!ELSE
        {
            Node* result = NodeCtorKeyword(KEYWORD_ELSE);
            L(result) = GetNodeFromFile(fp);
            R(result) = GetNodeFromFile(fp);
            return result;
        }
        case 5:                                                 //!WHILE
        {
            Node* result = NodeCtorKeyword(KEYWORD_WHILE);
            L(result) = GetNodeFromFile(fp);
            R(result) = GetNodeFromFile(fp);
            return result;
        }
        case 6:                                                 //!OPERATOR
        {
            Node* result = NodeCtorOp((OPER_TYPES)node_value);

            if (node_value == OP_SQRT || node_value == OP_NOT ||node_value == OP_OUT)
            {
                R(result) = GetNodeFromFile(fp);
            }
            else
            {
                L(result) = GetNodeFromFile(fp);
                R(result) = GetNodeFromFile(fp);
            }

            return result;
        }
        case 7:                                                 //!CREATE VAR                                            
            return NodeCtorCreateVar(VARS.data[node_value]);
        case 8:                                                 //!DEF FUNCTION
        {
            Node* result = NodeCtorFunction(FUNC.data[node_value]);
            L(result) = GetNodeFromFile(fp);
            R(result) = GetNodeFromFile(fp);
            return result;
        }
        case 9:                                                 //!CALL
        {
            Node* result = NodeCtorCall(FUNC.data[node_value]);
            R(result) = GetNodeFromFile(fp);
            return result;
        }
        case 10:                                                //!RETURN
        {
            Node* result = NodeCtorReturn();
            R(result) = GetNodeFromFile(fp);
            return result;
        }
        
        default:
            return nullptr;
    }
}

Node* GetNodeFromFile(FILE* fp)
{
    char c = 0;
    fscanf(fp, " %c", &c);                          //! get '{'
    if (c == '}')
    {
        ungetc(c, fp);
        return nullptr;
    }

    int node_type_std = 0;
    int node_value    = 0;
    fscanf(fp, "%d %d", &node_type_std, &node_value);

    Node* result = RecognizeNode(node_type_std, node_value, fp);

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
        
        #ifdef DEBUG
            printf("%s\n", FUNC.data[i]);
        #endif
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