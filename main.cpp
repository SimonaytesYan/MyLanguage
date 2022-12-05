#include <sys\stat.h>
#include <string.h>

#include "Libs/TreeSetup.h"
#include "Libs/LexicalAnalysis/LexicalAnalysis.h"
#include "Libs/ResursiveDescent/RecursiveDescent.h"

const char PROGRAM_FILE_NAME[] = "Main.sym";

int main()
{
    Node* program = GetProgramFromFile(PROGRAM_FILE_NAME);
}