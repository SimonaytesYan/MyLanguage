#include "SaveAndGetTree.h"

int SaveTreeInFile(const Tree* tree, const char* path)
{
    CHECK(tree == nullptr, "tree = nullptr", -1);
    CHECK(path == nullptr, "path = nullptr", -1);

    FILE* fp = fopen(path, "w");
    
    PutVars();
    PutFunctions();

    fclose(fp);
}

int SaveNodeinFile(const Node* node, FILE* fp)
{
    if (node == nullptr)
        return 0;

    switch (TYPE(node))
    {
        case TYPE_KEYWORD:
            fprintf(fp, "%d 0", KEYWORD_TO_STANDART[VAL_KEYWORD(node) - 1]);
            break;
        case TYPE_VAR:
            fprintf(fp, "%d %d", TYPE_VAR, FindVar(VAL_VAR(node)));
            break;
        case TYPE_NUM:
            fprintf(fp, "%d %d", TYPE_NUM, VAL_N(node));
            break;
        case TYPE_OP:
            fprintf(fp, "%d 0", KEYWORD_TO_STANDART[VAL_KEYWORD(node) - 1]);
            break;
        case TYPE_CALL:
            fprintf(fp, "%d %d", TYPE_CALL, FindFunction(VAL_FUNC(node)));
            break;
        case TYPE_FUNCTION:
            fprintf(fp, "%d %d", TYPE_FUNCTION, FindFunction(VAL_FUNC(node)));
            break;
        default:
            fprintf(fp, "%d 0", TYPE(node));
    }

    fprintf(fp, "\n{");
    ReturnIfError(SaveNodeinFile(L(node), fp));
    fprintf(fp, "\n}\n{\n");
    ReturnIfError(SaveNodeinFile(R(node), fp));
    fprintf(fp, "\n}");
}