#ifndef __STACK_SYM__
#define __STACK_SYM__

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <cstdint>

#include "Errors.h"
#include "Logging/Logging.h"

const char StackLogs[] = "StackLogs.txt";

const int  DUMP_LEVEL        = 2;  //!This constant is used to print stack elements to logs in right format
const int  HOW_MANY_ELEM_OUT = 10; //!This constant is used to determine how many stack elements to output when DUMP_LEVEL = 2

#define CANARY_PROTECTION 1
#define HASH_PROTECTION   2
#define PROTECTION_LEVEL  0

#define POISON_PTR ((void*)13)
//const void*    POISON_PTR  = (void*)13;
const double   FOR_RESIZE  = 2; 
const Elem     POISON      = (Elem)0X7FFFFFFF;
const uint64_t LEFT_KENAR  = 0xABABABABADEADDED;
const uint64_t RIGHT_KENAR = (uint64_t)(-1)^LEFT_KENAR;

//!------------------------------
//!status = true  if stack is ready to be used
//!status = false if stack wasn`t constructed or was destructed
//!
//!------------------------------
typedef struct LogInfo_t
{
    const char* name     = "(null)";
    const char* function = "(null)";
    const char* file     = "(null)";
    int  line            = (size_t)POISON;
    bool status          = false;
} LogInfo_t;

typedef struct Stack
{
    uint64_t left_border = LEFT_KENAR;

    LogInfo_t debug = {};

    Elem   *data    = (Elem*)POISON_PTR;
    size_t size     = (size_t)POISON;
    size_t capacity = (size_t)POISON - 1;

    uint64_t struct_hash = 0;
    uint64_t data_hash   = 0;

    uint64_t right_border = RIGHT_KENAR;
} Stack;

inline void   DumpStack(Stack *stk, int deep, const char function[], const char file[], int line);

inline size_t   StackCheck(Stack* stk, int line, const char function[], const char file[]);
inline size_t   StackConstructor(Stack* stk, int capacity, int line, const char function[], const char file[], const char name[]);
inline size_t   StackDtor(Stack* stk);
inline size_t   StackResizeUp(Stack* stk);
inline int      StackPush(Stack* stk, Elem value);
inline size_t   StackResizeDown(Stack* stk);
inline Elem     StackPop(Stack* stk, size_t *err);
inline uint64_t GetStructHash(Stack* stk);
inline size_t   ChangeStackData(Stack* stk, size_t resizeType);
inline void     Rehash(Stack* stk);
inline void     DoDumpStack(Stack* stk);

#define DUMP_STACK(stk) { DumpStack(&stk, DUMP_LEVEL, __PRETTY_FUNCTION__, __FILE__, __LINE__); }

#ifdef SAVE_MODE
void DoDumpStack(Stack* stk)
{
    DUMP_STACK(*stk);
}
#else

void DoDumpStack(Stack*) {}
#endif

#define OK_ASSERT(stk) {             \
    StackCheck(&stk, __LINE__, __PRETTY_FUNCTION__, __FILE__); \
    DoDumpStack(&stk); \
}

#define StackCtor(stk, capacity) StackConstructor(stk, capacity, __LINE__, __PRETTY_FUNCTION__, __FILE__, #stk)

//!------------------------------
//!deep = 1 - dump without printing stack elements
//!deep = 2 - 10 elements from start and 10 elements from end of the stack will be printed 
//!deep > 2 - all elements of the stack will be printed
//!
//!-----------------------------
void DumpStack(Stack *stk, int deep, const char function[], const char file[], int line)
{    
    LogPrintf("%s at %s(%d):\n", function, file, line);
    if (stk == nullptr)
    {
        LogPrintf("Null pointer to stack\n");
        return;
    }
    LogPrintf("Stack[%p] \"%s\" at %s at %s(%d):\n", stk, stk->debug.name, stk->debug.function, stk->debug.file, stk->debug.line);
    LogPrintf("Status: ");
    if (stk->debug.status)
        LogPrintf("enable\n");
    else
        LogPrintf("disable\n");

    #if (PROTECTION_LEVEL & CANARY_PROTECTION)
    {
        LogPrintf("Left border  = %llu\n", stk->left_border);
        LogPrintf("Rigth border = %llu\n", stk->right_border);
    }
    #endif

    LogPrintf("{\n");
    LogPrintf("\tsize        = %d\n", stk->size);
    LogPrintf("\tcapacity    = %d\n", stk->capacity);
    LogPrintf("\tdata_hash   = %llu\n", stk->data_hash);
    LogPrintf("\tstruct_hash = %llu\n", stk->struct_hash);

    LogPrintf("\tdata[%p]\n", stk->data);
    LogPrintf("\t{\n");


    if (deep > 1 && stk->data != nullptr && stk->size != (size_t)POISON && stk->capacity != (size_t)POISON && stk->size <= stk->capacity)
    {
        #if (PROTECTION_LEVEL & CANARY_PROTECTION)
        {
            LogPrintf("\t\tLeftCan  = %llu\n", ((size_t*)stk->data)[-1]);
            LogPrintf("\t\tRightCan = %llu\n", *(size_t*)((char*)stk->data + sizeof(Elem)*stk->capacity));
        }
        #endif

        if (deep > 2 || stk->capacity <= 20)
        {
            size_t i = 0;
            for(i = 0; i < stk->size; i++)
            {
                LogPrintf("\t\t*[%d] = ", i);
                //PrintElem(stk->data[i]);
                LogPrintf("\n");
            }
            
            while(i < stk->capacity)
            {
                LogPrintf("\t\t[%d] = ", i);
                //PrintElem(stk->data[i]);
                LogPrintf("\n");
                i++;
            }
        }
        else
        {
            for(size_t i = 0; i < HOW_MANY_ELEM_OUT; i++)
            {
                LogPrintf("\t\t");
                if (stk->size > i)
                    LogPrintf("*");
                LogPrintf("[%d] = ", i);
                //PrintElem(stk->data[i]);
                LogPrintf("\n");
            }
            LogPrintf("\t\t...\n");
            for(size_t i = stk->capacity - HOW_MANY_ELEM_OUT; i < stk->capacity; i++)
            {
                LogPrintf("\t\t");
                if (stk->size > i)
                    LogPrintf("*");
                LogPrintf("[%d] = ", i);
                //PrintElem(stk->data[i]);
                LogPrintf("\n");
            }
        }
    }

    LogPrintf("\t}\n}\n\n");
}

size_t StackCheck(Stack* stk, int line, const char function[], const char file[])
{
    size_t error = NO_ERROR;

    if (false)
    {
        return (size_t)line;
        return (size_t)function[0];
        return (size_t)file[0];
        return stk->capacity;
    }

    /*if (stk == nullptr)
        error |= NULL_STACK_POINTER;
    else
    {
        if (stk->size == POISON)
            error |= WRONG_SIZE;
        
        if (stk->capacity == POISON || stk->capacity == POISON - 1)
            error |= WRONG_CAPACITY;
        
        if (stk->size > stk->capacity)
            error |= SIZE_BIGGER_CAPACITY;
        
        if (stk->data == nullptr || stk->data == POISON_PTR)
            error |= NULL_DATA;
        else
        {
            #if (PROTECTION_LEVEL & HASH_PROTECTION)
            {
                uint64_t hash = GetHash(stk->data, stk->capacity*sizeof(Elem));
                if (hash != stk->data_hash)
                    error |= DATA_HASH_MISMATCH;
                
            }
            #endif

            #if (PROTECTION_LEVEL & CANARY_PROTECTION)
            {
                if (((uint64_t*)stk->data)[-1] != LEFT_KENAR)
                    error |= LEFT_BORDER_DAMAGED;
                if (stk->capacity != POISON - 1 && 
                    *(uint64_t*)((char*)stk->data + stk->capacity*sizeof(Elem)) != RIGHT_KENAR)
                    error |= RIGHT_BORDER_DAMAGED;
            }
            #endif
        }
        
        #if (PROTECTION_LEVEL & HASH_PROTECTION)
        {
            uint64_t now_hash = GetStructHash(stk);
            if (stk->struct_hash != now_hash)
                error |= STRUCT_HASH_MISMATCH;
        }
        #endif

        if (stk->debug.file == nullptr)
            error |= DEBUG_INFO_DAMAGED; 
        if (stk->debug.name == nullptr)
            error |= DEBUG_INFO_DAMAGED; 
        if (stk->debug.line == POISON)
            error |= DEBUG_INFO_DAMAGED; 
    }

    if (error != 0)
    {
        LogPrintf("Stack = %p\n" "Chech status = %d\n", stk, error);
        LogPrintf("At %s in %s(%d)\n", function, file, line); 
        if (error != 0)
        {
            for(size_t i = 0; i < 32; i++)
                if (error & (1 << i))
                    LogPrintf(ERROR_DESCRIPTION[i]);
            LogPrintf("\n");
        }
        LogPrintf("\n");
    }*/
    return error;
}

size_t StackConstructor(Stack* stk, int capacity, int line, const char function[], const char file[], const char name[]) 
{
    size_t error = 0;
    *stk = {};
    ChangeStackData(stk, (size_t)capacity);
    stk->size = 0;

    stk->debug          = {};
    stk->debug.file     = file;
    stk->debug.function = function;
    stk->debug.name     = name;
    stk->debug.line     = line;
    stk->debug.status   = true;

    Rehash(stk);

    OK_ASSERT(*stk);
    return error;
}


size_t StackDtor(Stack* stk)
{
    OK_ASSERT(*stk);
    stk->capacity    = (size_t)POISON - 1;
    stk->size        = (size_t)POISON;

    #if (PROTECTION_LEVEL & CANARY_PROTECTION)
        free((char*)stk->data - sizeof(LEFT_KENAR));
    #else
        free(stk->data);
    #endif
    stk->data         = (Elem*)POISON_PTR;

    stk->data_hash    = 0;
    stk->struct_hash  = 0;

    stk->debug.status = false;

    return NO_ERROR;
}

size_t StackResizeUp(Stack* stk)
{
    OK_ASSERT(*stk);

    size_t error = NO_ERROR;
    if (stk->capacity == 0)
        error = ChangeStackData(stk, 10);
    else
        error = ChangeStackData(stk, stk->capacity * 2);
    if (error != NO_ERROR)
        return error;
    
    Rehash(stk);    

    OK_ASSERT(*stk);
    return NO_ERROR;
}

int StackPush(Stack* stk, Elem value)
{
    OK_ASSERT(*stk);

    if (stk->capacity == stk->size)
    {
        size_t error = 0;
        if ((error = StackResizeUp(stk)))
        {   
            OK_ASSERT(*stk);
            return (int)error;
        }
    }
    stk->data[stk->size++] = value;

    Rehash(stk);

    OK_ASSERT(*stk);
    return NO_ERROR;
}

size_t StackResizeDown(Stack* stk)
{
    OK_ASSERT(*stk);

    if (stk->capacity == 0)
        return NO_ERROR;

    if (stk->size == 0)
        return NO_ERROR;
        
    if (stk->capacity/stk->size >= (size_t)(FOR_RESIZE*FOR_RESIZE))
    {
        size_t error = ChangeStackData(stk, stk->capacity / (size_t)FOR_RESIZE);
        if (error != 0)
            return error;
        Rehash(stk);
    }
    
    OK_ASSERT(*stk);
    if (stk->data == nullptr)
        return MEMORY_ALLOCATION_ERROR;
    return NO_ERROR;
}

Elem StackPop(Stack* stk, size_t *err = nullptr)
{
    OK_ASSERT(*stk);

    if (stk->size > 0)
        stk->size--;
    if (stk->capacity <= 0)
    {
        if (err != nullptr)
            *err |= WRONG_CAPACITY;
        return POISON;
    }
    Elem result = stk->data[stk->size];
    stk->data[stk->size] = POISON;

    size_t now_error = StackResizeDown(stk);
    if (err != nullptr)
        *err = now_error;
    if (now_error != NO_ERROR)
    {
        OK_ASSERT(*stk);
        return POISON;
    }

    return result;
}

uint64_t GetStructHash(Stack* stk)
{
    if (false)
        return stk->capacity;
    #if PROTECTION_LEVEL & HASH_PROTECTION
    uint64_t old_hash = stk->struct_hash;
    stk->struct_hash = 0;
    uint64_t now_hash = GetHash(stk, sizeof(Stack));
    stk->struct_hash = old_hash;
    return now_hash;
    #endif
    return 0;
}

void Rehash(Stack* stk)
{  
    if (stk == nullptr)
        return;
    #if (PROTECTION_LEVEL & HASH_PROTECTION)
    {
        stk->data_hash   = GetHash(stk->data, stk->capacity*sizeof(Elem));
        stk->struct_hash = GetStructHash(stk);
    }
    #endif
}

size_t ChangeStackData(Stack* stk, size_t newCapacity)
{
    OK_ASSERT(*stk);

    stk->capacity = newCapacity;

    uint64_t new_capacity = stk->capacity*sizeof(Elem);
    #if (PROTECTION_LEVEL & CANARY_PROTECTION)
    {
        new_capacity += sizeof(LEFT_KENAR) + sizeof(RIGHT_KENAR);
        
        char* mem_block = nullptr;
        if (stk->data == nullptr)
            mem_block = (char*)calloc(new_capacity, 1);
        else    
            mem_block = (char*)realloc((char*)stk->data - sizeof(LEFT_KENAR), new_capacity);

        if (mem_block == nullptr)
        {
            stk->data = nullptr;
            return MEMORY_ALLOCATION_ERROR;
        }

        *(uint64_t*)mem_block = LEFT_KENAR;
        *(uint64_t*)(mem_block + stk->capacity*sizeof(Elem) + sizeof(LEFT_KENAR)) = RIGHT_KENAR;
        stk->data = (Elem*)(mem_block + sizeof(LEFT_KENAR));
    }
    #else
        if (stk->data == nullptr || stk->data == POISON_PTR)
        {
            stk->data = (Elem*)calloc(new_capacity, 1);
        }
        else
            stk->data = (Elem*)realloc(stk->data, new_capacity);
    #endif

    Rehash(stk);

    return NO_ERROR;
}

#endif  //__STACK_SYM__