#include <string.h>

#include "Libs/Setup.h"
#include "Libs/LexicalAnalysis/LexicalAnalysis.h"
#include "Libs/ResursiveDescent/RecursiveDescent.h"
#include "Libs/CreateAssembler/CreateAssembler.h"

const char PROGRAM_FILE_NAME[] = "Main.sym";
const char ASM_FILE_NAME[]     = "Main.sy";

static void Program_tDtor(Program_t program);

int main()
{
    printf("Start main\n");
    OpenLogFile("Make_tree_log.log");

    Program_t program = {};
    program.comands = GetProgramFromFile(PROGRAM_FILE_NAME, &program.comands_num);

    Tree lang_tree = {};
    TreeCtor(&lang_tree);

    MakeTreeFromComands(&lang_tree, program.comands, program.comands_num);

    GraphicDump(&lang_tree);

    ReturnIfError(CreateAsmFromTree(&lang_tree, ASM_FILE_NAME));

    DeleteNode(lang_tree.root);

    Program_tDtor(program);
    printf("End main\n");
}

static void Program_tDtor(Program_t program)
{
    for(int i = 0; i < program.comands_num; i++)
        if (program.comands[i].val.type == TYPE_VAR)
            free(program.comands[i].val.val.var);
    free(program.comands);
}