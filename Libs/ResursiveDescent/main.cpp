#include <stdio.h>

#include "RecursiveDescent.h"

#include "../Setup.h"

const int TESTS_NUMBER = 15;

int main()
{
    FILE* fp = fopen("RecursiveDescentTests.txt", "r");
    assert(fp);

    for(size_t i = 0; i < TESTS_NUMBER; i++)
    {
        char buffer[100] = "";
        fscanf(fp, "%s", buffer);
        Tree tree = {};
        TreeCtor(&tree);
        tree.root = GetNodeFromStr(buffer);

        GraphicDump(&tree);

        TreeDtor(&tree);
    }

}