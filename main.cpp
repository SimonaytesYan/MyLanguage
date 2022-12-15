#include <string.h>

#include "Libs/Setup.h"
#include "Libs/LexicalAnalysis/LexicalAnalysis.h"
#include "Libs/ResursiveDescent/RecursiveDescent.h"

const char PROGRAM_FILE_NAME[] = "Main.sym";

int main()
{

    OpenLogFile("Make_tree_log.log");
    printf("start main\n");
    int   program_size = 0;
    Node* program      = GetProgramFromFile(PROGRAM_FILE_NAME, &program_size);

    Tree comands = {};
    TreeCtor(&comands);
    comands.root = program;

    for(int i = 0; i < program_size - 1; i++)
    {
        program[i].right = &program[i+1];
        program[i].left = nullptr;
    }
    DUMP_T(&comands);
    GraphicDump(&comands);

    Tree lang_tree = {};
    TreeCtor(&lang_tree);

    for(int i = 0; i < program_size - 1; i++)
        program[i].right = nullptr;

    MakeTreeFromComands(&lang_tree, program);

    GraphicDump(&lang_tree);

    DeleteNode(lang_tree.root);

    for(int i = 0; i < program_size; i++)
    {
        if (program[i].val.type == TYPE_VAR)
        {
            free(program[i].val.val.var);
        }
    }
    free(program);
    printf("End main\n");
}