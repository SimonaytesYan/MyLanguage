#include "Processor.h"

int main(int argc, char* argv[])
{
    CHECK(ExecProgramFromCL(argc, argv) != 0, "", -1);
}