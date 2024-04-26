#ifndef __CPU_SYM__
#define __CPU_SYM__

#define LOGS_TO_FILE

#include "../Libs/Logging/Logging.h"
#include "../Libs/ComandSystem/ComandSystem.h"

int  GetExecFileFromCLArgs(FILE** fp, int argc, char* argv[]);

int  CheckHeaderFromFile(Header *header, FILE* executable_file);

int  GetCPUFromFile(CPU* cpu, int comands_number, FILE* executable_file);

void Run(CPU* cpu);

int ExecProgramFromCL(int argc, char* argv[]);

#endif  //__CPU_SYM__