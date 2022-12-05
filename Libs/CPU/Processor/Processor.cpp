#include "Processor.h"
#include "..\Libs\TXLib.h"

#include <math.h>

#define DEF_CMD(name, num, ...) \
    CMD_##name = num,

enum COMANDS
{
    #include "..\Cmd.h"
};

#undef DEF_CMD

int GetCPUFromFile(CPU* cpu, int comands_number, FILE* executable_file)
{
    cpu->stk = {};
    StackCtor(&(cpu->stk),        0);
    StackCtor(&(cpu->call_stack), 0);

    cpu->code = (int*)calloc(comands_number, sizeof(int));

    CHECK(cpu->code == nullptr, "\nError during allocation memory for comands array\n", -1);

    fread(cpu->code, sizeof(int) ,comands_number, executable_file);
    
    cpu->number_comands = comands_number;
    cpu->pc             = 0;
    
    return 0;
}

int CheckHeaderFromFile(Header *header, FILE* executable_file)
{
    CHECK(executable_file == nullptr, "\nExecutable file = nullptr\n", -1); 

    fread(header, sizeof(*header), 1, executable_file);

    CHECK(header->signature != SIGNATURE,   "\nFile isn`t executable\n",     -1); 
    CHECK(header->version   != ASM_VERSION, "\nWrong version of compiler\n", -1);
    return 0;
}

int GetExecFileFromCLArgs(FILE** fp, int argc, char* argv[])
{
    CHECK(fp   == nullptr, "\nFp damaged\n",                    -1);
    CHECK(argv == nullptr, "\nArgv damaged\n",                  -1);
    CHECK(argc != 2,       "\nWrong number of cmd arguments\n", -1);

    char* executable_file_name = argv[1];
    
    *fp = fopen(executable_file_name, "rb");
    CHECK(*fp == nullptr, "\nError during executable file open", -1);   

    return 0;
}

//!-------------------
//!@param [out] arg
//!
//!-------------------
int GetArg(int* arg, int cmd, CPU* cpu)
{
    *arg = 0;
    if ((cmd & ARG_IMMED) != 0)
        *arg += cpu->code[cpu->pc++];

    if ((cmd & ARG_REG) != 0)
    {
        int reg = cpu->code[cpu->pc++];
        CHECK((reg < 0 || reg > REG_N), "\nWrong register number in push\n", -1);
        *arg += cpu->regs[reg];
    }

    return 0;
}

//!-------------------
//!@param [out] arg
//!
//!-------------------
int GetPushArg(int* arg, int cmd, CPU* cpu)
{
    if (GetArg(arg, cmd, cpu) != 0)
        return -1;
    if ((cmd & ARG_MEM) != 0)
    {
        CHECK(*arg < 0 || *arg >= RAM_SIZE, "\nAttempt to read from wrong addres in ram\n", -1);
        *arg = cpu->ram[*arg];                
    }

    return 0;
}

//!-------------------
//!@param [out] arg
//!
//!-------------------
int GetPopArg(int* arg, int* write_to, int cmd, CPU* cpu)
{
    if ((cmd & ARG_MEM) != 0)
    {
        GetArg(arg, cmd, cpu);
        CHECK(*arg < 0 || *arg >= RAM_SIZE, "\nAttempt to write to wrong addres in ram\n", -1);
        *write_to = ARG_MEM;
    }
    else
    {
        CHECK((cmd & ARG_IMMED) != 0, "\nWrong pop arguments\n", -1);
        *arg = cpu->code[cpu->pc++];
        CHECK(*arg < 0 || *arg > REG_N, "\nAttempt to write to wrong register in ram\n", -1);
        *write_to = ARG_REG;
    }

    return 0;
}

int DrawRam(CPU* cpu)
{
    CHECK(cpu == nullptr, "Cpu == nullptr\n", -1);
    RGBQUAD* buf = txVideoMemory();

    for(int i = 0; i < WINDOW_WIDTH; ++i)
    {
        for(int j = 0; j < WINDOW_HIGHT; ++j)
        {
            int index = i*WINDOW_HIGHT + j;
            int val   = cpu->ram[index];
            
            RGBQUAD rgb = { (BYTE) (val & FI_BYTE),
                            (BYTE)((val >> 8) & FI_BYTE),
                            (BYTE)((val >> 16) & FI_BYTE) };

            buf[i + (WINDOW_HIGHT - j)*WINDOW_WIDTH] = rgb;
        }
    }
    //Sleep(16);
    txRedrawWindow();

    return 0;
}

#define PUSH(arg) StackPush(&cpu->stk, arg);
#define POP(a)                                                          \
{                                                                       \
    size_t error = 0;                                                   \
    Elem non_repeatable_name = StackPop(&(cpu->stk), &error);           \
    CHECK(error != NO_ERROR, "Error during stack pop\n", (void)0);      \
    a = non_repeatable_name;                                            \
}                                                                        

#define DEF_CMD(name, num, arg, ...)                                    \
    case CMD_##name:                                                    \
        __VA_ARGS__                                                     \
    break;

void Run(CPU* cpu)
{
    while (cpu->pc < cpu->number_comands)
    {
        int cmd = cpu->code[cpu->pc++];

        switch(cmd & CMD_MASK)
        {   
            #include "..\Cmd.h"
            default:
                LogPrintf("\nWrong comand\n");
            break;
        }
    }
}

int InitTXLib()
{   
    txBegin();
    CHECK(txCreateWindow(WINDOW_WIDTH, WINDOW_HIGHT, true) == nullptr, "Error during creating windows to draw", -1);

    return 0;
}

#undef DEF_CMD
#undef CaseCMD

int ExecProgramFromCL(int argc, char* argv[])
{
    CHECK(argv == nullptr, "Argv = nullptr", -1);

    OpenLogFile("Processor/CPULogs.txt");

    FILE* executable_file = nullptr;
    if (GetExecFileFromCLArgs(&executable_file, argc, argv) != 0)
        return -1;

    Header header = {};
    if (CheckHeaderFromFile(&header, executable_file) != 0)
        return -1;

    CPU cpu = {};
    if (GetCPUFromFile(&cpu, header.comands_number, executable_file) != 0)
        return -1;
    if (InitTXLib() != 0)
        return -1;
        
    Run(&cpu);

    CloseLogFile();
}
