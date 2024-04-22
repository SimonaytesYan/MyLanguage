#ifndef __COMAND_SYSTEM_SYM__
#define __COMAND_SYSTEM_SYM__

typedef int Elem;

#include "../PrintElem.h"
#include "../Stack.h"

#define BIG_FILE_WORK

const int REG_N = 4;

enum REGISTERS
{
    RAX = 1,
    RBX = 2,
    RCX = 3,
    RDX = 4, 
};

enum CMD_MASKS
{
    CMD_MASK  = 0xFF,
    ARG_IMMED = 0x100,
    ARG_REG   = 0x200,
    ARG_MEM   = 0x400
};

const int SIGNATURE   = 'S' * 256 + 'Y';
const int ASM_VERSION = 3;

const int RAM_SIZE     = 230400;
const int FI_BYTE      = 0xFF;
const int WINDOW_HIGHT = 360;
const int WINDOW_WIDTH = 636;
struct CPU
{
    int*  code            = nullptr;
    int   number_comands  = 0;
    int   pc              = 0;
    Stack stk             = {};
    Stack call_stack      = {};
    int   regs[REG_N + 1] = {};
    int   ram [RAM_SIZE]  = {};
};

const int MAX_LABELS    = 64;
const int MAX_LABEL_LEN = 20;

struct Label 
{
    char name[MAX_LABEL_LEN] = "";
    int  cmd_to              = -1;
};

struct Header
{
    int    signature      = -1;
    int    version        = -1;
    size_t comands_number = -1;
};

int InitHeader(Header* header, int comands_number);

void DumpCPU(CPU* cpu);

void DumpLabels(Label* labels);

#endif //__COMAND_SYSTEM_SYM__