#include <string.h>

#include "Libs/Setup.h"
#include "Libs/LexicalAnalysis/LexicalAnalysis.h"
#include "Libs/ResursiveDescent/RecursiveDescent.h"

const char PROGRAM_FILE_NAME[] = "Main.sym";

int main()
{
    printf("Start main\n");
    OpenLogFile("Make_tree_log.log");

    Program_t program = {};
    program.comands = GetProgramFromFile(PROGRAM_FILE_NAME, &program.comands_num);

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

    Tree lang_tree = {};
    TreeCtor(&lang_tree);

    for(int i = 0; i < program.comands_num - 1; i++)
        program.comands[i].right = nullptr;

    MakeTreeFromComands(&lang_tree, program.comands, program.comands_num);

    GraphicDump(&lang_tree);

    DeleteNode(lang_tree.root);

    for(int i = 0; i < program.comands_num; i++)
    {
        if (program.comands[i].val.type == TYPE_VAR)
        {
            free(program.comands[i].val.val.var);
        }
    }
    free(program.comands);
    printf("End main\n");
}