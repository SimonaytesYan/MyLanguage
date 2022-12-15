#include <assert.h>

#include "InAndOut.h"
#include "../ResursiveDescent/RecursiveDescent.h"

void GetDataFromFile(const char* file_name, FileStruct* data_from_file)
{
    assert(data_from_file);
    assert(file_name);

    FILE* fp = fopen(file_name, "r");

    if (fscanf(fp, "%s", data_from_file->tree_str) == EOF)
        return;

    if (fscanf(fp, "%lf %lf", &(data_from_file->x_l), &data_from_file->x_r) == EOF)
        return;
    if (fscanf(fp, "%d", &(data_from_file->taylor_order)) == EOF)
        return;
    fscanf(fp, "%lf", &(data_from_file->taylor_x0));

    fclose(fp);
}

void PrintElem(FILE* fp, Node* node)
{
    fprintf(fp, "node_p = %p\n", node);
    switch (TYPE(node))
    {
        case TYPE_NUM:
        {
            fprintf(fp, "Type = NUM\n");
            fprintf(fp, "val = %d\n", VAL_N(node));
            break;
        }
        case TYPE_OP:
        {
            fprintf(fp, "Type = OP\n");
            break;
        }
        case TYPE_VAR:
        {
            fprintf(fp, "Type = VAR\n");
            fprintf(fp, "val = <%s>\n", VAL_VAR(node));
            break;
        }
        case TYPE_SYMB:
        {
            fprintf(fp, "Type = SYMB\n");
            fprintf(fp, "val = %c\n", VAL_SYMB(node));
            break;
        }
        case UNDEF_NODE_TYPE:
        {
            fprintf(fp, "Type = UNDEF\n");
            break;
        }
        default:
            fprintf(fp, "Type = Unknown\n");
            break;
    }
    fprintf(fp, "type = %d");

    fflush(fp);
}

void PrintElemInLog(Node_t elem)
{
    LogPrintf("type = %d\n", elem.type);
    switch (elem.type)
    {
    case TYPE_NUM:
        LogPrintf("\tdbl  = %lg\n", elem.val.dbl);
        break;
    case TYPE_OP:
        LogPrintf("\top   = %d\n", elem.val.op);
        break;
    case TYPE_VAR:
        LogPrintf("\tvar  = <%s>\n", elem.val.var);
        break;
    case TYPE_KEYWORD:
        LogPrintf("\tkeyword = <%d>\n", elem.val.keyword);
        break;
    case TYPE_SYMB:
        LogPrintf("\t symb = <%c>%d\n", elem.val.symb, elem.val.symb);
    default:
        break;
    }
}

void OutputGraphicDump(Tree* tree)
{
    assert(tree);

    GraphicDump(tree);

    char graphic_dump_file[70] = "";
    #ifdef _WIN32
        sprintf(graphic_dump_file, ".\\GraphicDumps\\Dump%d.png", GRAPHIC_DUMP_CNT - 1);
    #else
        sprintf(graphic_dump_file, "xdg-open GraphicDumps/Dump%d.png", GRAPHIC_DUMP_CNT - 1);
    #endif

    system(graphic_dump_file);
}