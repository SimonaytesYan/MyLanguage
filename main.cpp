#include <string.h>

#include "Libs/Setup.h"
#include "Libs/SaveAndGetTree/SaveAndGetTree.h"
#include "Libs/LexicalAnalysis/LexicalAnalysis.h"
#include "Libs/ResursiveDescent/RecursiveDescent.h"
#include "Libs/CreateAssembler/CreateAssembler.h"
#include "Libs/RebuildCodeFromTree/RebuildCodeFromTree.h"

const char PROGRAM_FILE_NAME[] = "Main.sym";
const char ASM_FILE_NAME[]     = "Main.sy";

static void Program_tDtor(Program_t program);

static void GraphicDumpComands(Program_t program);

int GetFlagsFromCML(int argc, const char* argv[], int* operation_mode, char** path);

const int   COMPILE_PROGRAM = 0;
const int   COMPILE_TREE    = 1;
const int   REBUILD_CODE    = 2;
const char  TREE_PATH[]     = "Tree.alt";

const int   STD_VALUE_OP_MODE = -1;

const char* flags[] = {"-m",
                       "-p"};

int main(int argc, const char* argv[])
{
    int   operation_mode = 0;
    char* path           = nullptr;
    ReturnIfError(GetFlagsFromCML(argc, argv, &operation_mode, &path));   

    OpenLogFile("Make_tree_log.log");

    if (operation_mode == COMPILE_PROGRAM)
    {
        Program_t program = {};
        program.comands = GetProgramFromFile(path, &program.comands_num);

        GraphicDumpComands(program);

        Tree lang_tree = {};
        TreeCtor(&lang_tree);

        ReturnIfError(MakeTreeFromComands(&lang_tree, program.comands, program.comands_num));

        GraphicDump(&lang_tree);

        SaveTreeInFile(&lang_tree, TREE_PATH);

        ReturnIfError(CreateAsmFromTree(&lang_tree, ASM_FILE_NAME));

        DeleteNode(lang_tree.root);

        Program_tDtor(program);
    }
    else if (operation_mode == COMPILE_TREE)
    {
        Tree lang_tree = {};
        TreeCtor(&lang_tree);

        ReturnIfError(GetTreeFromFile(&lang_tree, path));

        GraphicDump(&lang_tree);

        ReturnIfError(CreateAsmFromTree(&lang_tree, ASM_FILE_NAME));

        DeleteNode(lang_tree.root);
    }
    else if (operation_mode == REBUILD_CODE)
    {
        Tree lang_tree = {};
        TreeCtor(&lang_tree);

        ReturnIfError(GetTreeFromFile(&lang_tree, path));

        RebuildCodeFromTreeInFile(&lang_tree, "REBUILD.sym");
        
        DeleteNode(lang_tree.root);
    }

    free(path);
    CloseLogFile();
    printf("End main\n");
}

int GetFlagsFromCML(int argc, const char* argv[], int* operation_mode, char** path)
{
    *operation_mode = STD_VALUE_OP_MODE;
    *path           = nullptr; 

    bool get_op_mode = false;
    bool get_path    = false;
    for(int i = 0; i < argc; i++)
    {
        if (*operation_mode == -1 && get_op_mode)
        {
            sscanf(argv[i], "%d", operation_mode);
            continue;
        }
        if (*path == nullptr && get_path)
        {
            *path = (char*)calloc(strlen(argv[i]) + 1, sizeof(char));
            sscanf(argv[i], "%s", *path);
            continue;
        }
        if (!strcmp(argv[i], flags[0]))
            get_op_mode = true;
        if (!strcmp(argv[i], flags[1]))
            get_path = true;
    }

    if (!get_op_mode || *operation_mode == STD_VALUE_OP_MODE)
    {
        printf("opearation mode not defined\n");
        return -1;
    }
    if (!get_path || *path == nullptr)
    {
        printf("path to tree/program not defined\n");
        return -1;
    }

    printf("op_mode = %d\n", *operation_mode);
    printf("path    = %s\n", *path);
    return 0;
}

static void GraphicDumpComands(Program_t program)
{
    Tree comands = {};
    TreeCtor(&comands);
    comands.root = program.comands;

    for(size_t i = 0; i < program.comands_num - 1; i++)
    {
        program.comands[i].right = &program.comands[i+1];
        program.comands[i].left = nullptr;
    }
    DUMP_T(&comands);
    GraphicDump(&comands);

    for(size_t i = 0; i < program.comands_num; i++)
    {
        program.comands[i].right = nullptr;
        program.comands[i].left = nullptr;
    }
}

static void Program_tDtor(Program_t program)
{
    for(size_t i = 0; i < program.comands_num; i++)
        if (program.comands[i].val.type == TYPE_VAR)
            free(program.comands[i].val.val.var);
    free(program.comands);
}