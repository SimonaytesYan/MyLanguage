#include <string.h>

#include "Libs/Setup.h"
#include "Libs/SaveAndGetTree/SaveAndGetTree.h"
#include "Libs/LexicalAnalysis/LexicalAnalysis.h"
#include "Libs/ResursiveDescent/RecursiveDescent.h"
#include "Libs/CreateAssembler/CreateAssembler.h"

const char PROGRAM_FILE_NAME[] = "Main.sym";
const char ASM_FILE_NAME[]     = "Main.sy";

static void Program_tDtor(Program_t program);

static void GraphicDumpComands(Program_t program);

const int  COMPILE_PROGRAM = 0;
const int  COMPILE_TREE    = 1;
const char TREE_PATH[]     = "Tree.alt";

int main()
{
    printf("Start main\n");
    printf("Enter %d to compile  program\n",       COMPILE_PROGRAM);
    printf("Enter %d to compile tree from file\n", COMPILE_TREE);
    
    int operation_mode = 0;
    scanf("%d", &operation_mode);

    OpenLogFile("Make_tree_log.log");

    if (operation_mode == COMPILE_PROGRAM)
    {
        Program_t program = {};
        program.comands = GetProgramFromFile(PROGRAM_FILE_NAME, &program.comands_num);

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
    else
    {
        Tree lang_tree = {};
        TreeCtor(&lang_tree);

        ReturnIfError(GetTreeFromFile(&lang_tree, TREE_PATH));

        GraphicDump(&lang_tree);

        ReturnIfError(CreateAsmFromTree(&lang_tree, ASM_FILE_NAME));

        DeleteNode(lang_tree.root);
    }

    CloseLogFile();
    printf("End main\n");
}

static void GraphicDumpComands(Program_t program)
{
    Tree comands = {};
    TreeCtor(&comands);
    comands.root = program.comands;

    for(int i = 0; i < program.comands_num - 1; i++)
    {
        program.comands[i].right = &program.comands[i+1];
        program.comands[i].left = nullptr;
    }
    DUMP_T(&comands);
    GraphicDump(&comands);

    for(int i = 0; i < program.comands_num; i++)
    {
        program.comands[i].right = nullptr;
        program.comands[i].left = nullptr;
    }
}

static void Program_tDtor(Program_t program)
{
    for(int i = 0; i < program.comands_num; i++)
        if (program.comands[i].val.type == TYPE_VAR)
            free(program.comands[i].val.val.var);
    free(program.comands);
}