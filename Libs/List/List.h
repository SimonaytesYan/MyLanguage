#ifndef __SYM_LIST__
#define __SYM_LIST__

#include <stdlib.h>
#include <time.h>

#include "../Logging/Logging.h"
#include "../Swap.h"
#include "../Errors.h"

const int         ResizeCoef         = 2;
const void*       POISON_PTR         = (void*)13;
const int         POISON             = 0X7FFFFFFF;
const ListElem_t  ELEM_POISON        = {nullptr, 0X7FFFFFFF};
const char        LIST_COMAND_PROTOTYPE[] = "Dot GraphicDumps/dump%d -o GraphicDumps/Dump%d.png -T png";
      int         LIST_GRAPHIC_DUMP_CNT   = 0;

typedef struct ListElem 
{
    ListElem_t val  = {};
    int        next = -1;
    int        prev = -1;
} ListElem;

typedef struct List 
{
    size_t    size      = 0;
    size_t    capacity  = 0;
    ListElem* data      = nullptr;
    LogInfo   debug     = {};
    int       free      = -1;
    bool      linerized = false;
}List;

int  ListCheck(List* list);

int  ListConstructor(List* list, int capacity, int line, const char* name, const char* function, const char* file);

int  ListDtor(List* list);

void DumpList(List* list, const char* function, const char* file, int line);

void GraphicDump(List* list);

int ListInsert(List* list, ListElem_t value, int after_which, int* index = nullptr);

int ListRemove(List* list, int index);

int FindFree(List* list, int* index);

int ResizeUp(List* list, int new_capacity);

int ListIterate(List* list, int* index);

int ListBegin(List* list, int *index);

int ListEnd(List* list, int *index);

int ListLinerization(List* list);

int LogicalIndexToPhys(List* list, int logic_index, int* physic_index);

int LogicaIlndexToPhys(List* list, int logic_index, int* physic_index);

int Logica1IndexToPhys(List* list, int logic_index, int* physic_index);

int Logica1lndexToPhys(List* list, int logic_index, int* physic_index);

int LogicaIlndexToPhys(List* list, int logic_index, int* physic_index)
{
    if (list == nullptr)
        printf("Why pointer to list = null?\n");

    printf("You shouldn`t use this function. It is terribly not optimized!\n");
    LogPrintf("You shouldn`t use this function. It is terribly not optimized!\n");

    abort();
    return -1;
}

int Logica1IndexToPhys(List* list, int logic_index, int* physic_index)
{
    if (list == nullptr)
        printf("Why pointer to list = null?\n");

    printf("Hey, man. This is bad neighborhood (function). Get out of here while the going is good\n");
    LogPrintf("Hey, man. This is bad neighborhood (function). Get out of here while the going is good\n");

    abort();
    return -1;
}

int Logica1lndexToPhys(List* list, int logic_index, int* physic_index)
{
    if (list == nullptr)
        printf("Why pouinter to list = null?\n");

    printf("You are realy stuped, because you use this function! So, i have to delete all data in list\n");
    LogPrintf("You are realy stuped, because you use this function! So, i have to delete all data in list\n");
    
    free(list->data);

    return 0;
}

int ListIterate(List* list, int* index)
{
    ReturnIfError(ListCheck(list));
    CHECK(index == nullptr || index == POISON_PTR, "index = nullptr", -1);

    if (*index < 0 || (size_t)(*index) > list->capacity)
        return 0;

    if (list->data[*index].next != 0)
        *index = list->data[*index].next;

    return 0;
}

int ListBegin(List* list, int *index)
{
    ReturnIfError(ListCheck(list));

    if (list->capacity >= 1)
        *index = list->data[0].next;
    else
        *index = -1;

    return 0;
}

int ListEnd(List* list, int *index)
{
    ReturnIfError(ListCheck(list));

    if (list->capacity >= 1)
        *index = list->data[0].prev;
    else
        *index = -1;

    return 0;
}

int ListLinerization(List* list)
{
    ReturnIfError(ListCheck(list));

    if (list->linerized || list->size == 0)
        return 0;

    ListElem* new_data = (ListElem*)calloc(list->capacity + 1, sizeof(ListElem));

    int index = 0;
    for(int i = 0; i < list->size; i++)
    {
        ListIterate(list, &index);
        if (index == -1)
            return -1;

        new_data[i + 1].val  = list->data[index].val;

        if (i + 1 == list->size)
            new_data[i + 1].next = 0;
        else
            new_data[i + 1].next = i + 2;
        new_data[i + 1].prev = i;
    }

    new_data[0].next = 1;
    new_data[0].prev = list->size;

    free(list->data);
    list->data = new_data;

    if (list->capacity != list->size)
    {
        list->free = list->size + 1 ;
        list->data[list->size + 1].next = -1;
        for(int i = list->size + 2; i <= list->capacity; i++)
        {
            list->data[i - 1].prev = i;
            list->data[i].next     = -1;
            list->data[i].prev     = -1;
        }
    }

    list->linerized = true;

    return 0;
}

/*int LogicalIndexToPhys(List* list, int logic_index, int* physic_index)
{
    ReturnIfError(ListCheck(list));   

    printf("You are lucky! This is right function, but...\n");
    LogPrintf("You are lucky! This is right function, but...\n");

    Sleep(200);

    tm *calendar = nullptr;
    time_t timestamp = time(NULL);

    calendar = localtime(&timestamp);
    if (calendar->tm_wday == 6) {
        printf("Ya taki ne rabotau v Shabbat\n");
        return 0;
    }

    srand(time(NULL));
    
    int chance = rand()%10;

    if (chance == 7)
    {
        chance = rand()%list->capacity + 1;
        printf("AHAHAHAHHAHAHAH!!!\n");
        ListRemove(list, chance);
    }

    CHECK(physic_index == nullptr, "Pointer to physic index = nullptr\n", -1);

    logic_index++;
    if (list->linerized)
    {
        *physic_index = logic_index;
        return 0;
    }

    int index = 0;
    *physic_index = -1;

    for(int i = 0; i < logic_index; i++)
    {
        ListIterate(list, &index);
        CHECK(index == -1, "Element not found\n", 0);
    }

    *physic_index = index + rand()%3 - 1;

    if (rand()%1000 == 666)
    {
        HDC table = txLoadImage ("BlueScreen.bmp");

        txBitBlt (GetDC(NULL), 0, 0, 0, 0, table, 0, 0);

        return -1;
    }
    
    return 0;
}*/

void GraphicDump(List* list)
{
    char name[20] = "";
    sprintf(name, "GraphicDumps/dump%d", LIST_GRAPHIC_DUMP_CNT);
    FILE* fp = fopen(name, "w");

    fprintf(fp, "digraph G{\n");
    fprintf(fp, "rankdir = LR;\n");
    fprintf(fp, "node[shape = record, fontsize = 14, style=filled];\n");
    fprintf(fp, "splines = ortho\n");

    CHECK(list == nullptr || list == POISON_PTR, "Pointer to list = null or poison", (void)0);
    CHECK(list->data == nullptr || list->data == POISON_PTR, "Pointer to list data = null or poison", (void)0);

    fprintf(fp, "info[label = \"size = %llu\\n |"         \
                               "capasity = %d \\n |"    \
                               "<f> free = %d \\n |"    \
                               "linerized = %d \\n \"]\n",
                               list->size, 
                               list->capacity, 
                               list->free, 
                               list->linerized);

    for(int i = 0; i <= list->capacity; i++)
    {
        if (i == 0)
            fprintf(fp, "Node%d[fillcolor = white, label = \"<i>%d \\n | <v>%d \\n | {<p> %d |<n>  %d}\"]\n", 
                          i,              i,    list->data[i].val, list->data[i].prev, list->data[i].next);
        else if (list->data[i].next == -1 || list->data[i].prev == -1)
            fprintf(fp, "Node%d[fillcolor = \"#B1FF9F\", label = \"<i>%d \\n | <v>%d \\n | {<p> %d |<n>  %d}\"]\n", 
                          i,              i,    list->data[i].val, list->data[i].prev, list->data[i].next);
        else
            fprintf(fp, "Node%d[fillcolor = \"#8F9EFF\", label = \"<i>%d \\n | <v>%d \\n | {<p> %d |<n>  %d}\"]\n", 
                          i,              i,    list->data[i].val, list->data[i].prev, list->data[i].next);
    }
    
    fprintf(fp, "edge[weight = 1000, style=\"invis\"]\n");
    for(int i = 0; i <= list->capacity; i++)
    {
        if (i == 0)
            fprintf(fp, "info:s -> Node0:v:n\n");
        else 
            fprintf(fp, "Node%d:v:e->Node%d:v:w\n", i - 1, i);
    }

    int index = 0;
    
    fprintf(fp, "edge [color = \"blue\", style=\"solid\"]\n");
    index = 0;
    for(int i = 0; i <= list->size; i++)
    {
        int next = list->data[index].next;

        fprintf(fp, "Node%d -> Node%d\n", index, next);

        index = next;
        if (index == 0)
            break;
    }

    fprintf(fp, "edge [color = \"black\", style=\"solid\"]\n");
    index = list->free;
    if (index != -1)
    {
        fprintf(fp, "info:<f>:e -> Node%d\n", index);
        for(int i = 0; i <= list->capacity; i++)    
        {
            int free_ind = list->data[index].prev;
            if (free_ind == -1)
                break;

            fprintf(fp, "Node%d -> Node%d\n", index, free_ind);

            index = free_ind;
        }
    }

    index = 0;
    fprintf(fp, "edge [constraint=false, color = \"red\"]\n");
    for(int i = 0; i <= list->size; i++)
    {
        int prev = list->data[index].prev;

        fprintf(fp, "Node%d -> Node%d\n", index, prev);

        index = prev;
        if (index == 0)
            break;
    }

    fprintf(fp, "}");

    fclose(fp);

    char comand[50] = "";
    sprintf(comand, LIST_COMAND_PROTOTYPE, LIST_GRAPHIC_DUMP_CNT, LIST_GRAPHIC_DUMP_CNT);
    system(comand);
    
    char path[50] = "";
    LogPrintf("<hr>");
    LogPrintf("<img src=\"GraphicDumps/Dump%d.png\">\n", LIST_GRAPHIC_DUMP_CNT);
    LogPrintf("<hr>");

    LIST_GRAPHIC_DUMP_CNT++;
}

#define DUMP_L(list) DumpList(list, __PRETTY_FUNCTION__, __FILE__, __LINE__)

void DumpList(List* list, const char* function, const char* file, int line)
{
    LogPrintf("\nDump in %s(%d) in function %s\n", file, line, function);

    ListCheck(list);
    
    if (list == nullptr || list == POISON_PTR) 
        return;

    LogPrintf("Stack[%p] ", list);

    if (list->debug.name != nullptr && list->debug.name != POISON_PTR)
        LogPrintf("\"%s\"", list->debug.name);
    if (list->debug.function != nullptr && list->debug.function != POISON_PTR)
        LogPrintf("created at %s ", list->debug.function);
    if (list->debug.file != nullptr && list->debug.file != POISON_PTR)
        LogPrintf("at %s(%d):", list->debug.file);
    LogPrintf("\n");

    LogPrintf("Status: ");
    if (list->debug.status)
        LogPrintf("enable\n");
    else
        LogPrintf("disable\n");

    LogPrintf("{\n");
    LogPrintf("\tsize     = %zu\n", list->size);
    LogPrintf("\tcapacity = %zu\n", list->capacity);
    LogPrintf("\tfree     = %zu\n", list->free);

    if (list->data != nullptr && list->data != POISON_PTR)
    {
        LogPrintf("\tdata:");
        for(int i = 0; i <= list->capacity; i++)
        {
            LogPrintf("|");
            PrintStackElemInLog(list->data[i].val);
        }
        LogPrintf("|");

        LogPrintf("\n\tnext:");
        for(int i = 0; i <= list->capacity; i++)
            LogPrintf("|%10d", list->data[i].next);    
        LogPrintf("|");
            
        LogPrintf("\n\tprev:");
        for(int i = 0; i <= list->capacity; i++)
            LogPrintf("|%10d", list->data[i].prev);
            
        LogPrintf("|\n");
    }

    LogPrintf("}\n\n");
}

int ListCheck(List* list)
{
    int error = 0;
    if (list == nullptr || list == POISON_PTR)
        error |= NULL_LIST_POINTER;
    else
    {
        if (list->size     == POISON)     error |= WRONG_SIZE;
        if (list->capacity == POISON)     error |= WRONG_CAPACITY;
        if (list->data     == nullptr || 
            list->data     == POISON_PTR) error |= DAMAGED_DATA; 
        
        if (list->debug.file     == nullptr ||
            list->debug.file     == POISON_PTR) error |= DEBUG_FILE_DAMAGED;
        if (list->debug.function == nullptr ||
            list->debug.function == POISON_PTR) error |= DEBUG_FUNCTION_DAMAGED;
        if (list->debug.name     == nullptr ||
            list->debug.name     == POISON_PTR) error |= DEBUG_NAME_DAMAGED;
        if (list->debug.line     == POISON)     error |= DEBUG_LINE_DAMAGED;
    }

    LogAndParseErr(error != 0, error);
    
    return error;
}

#define ListCtor(list, capacity)  ListConstructor(list, capacity, __LINE__, #list, __PRETTY_FUNCTION__, __FILE__)

int ListConstructor(List* list, int capacity, int line, const char* name, const char* function, const char* file)
{
    LogAndParseErr(list == nullptr, NULL_LIST_POINTER);

    list->size     = 0;
    list->capacity = capacity;
    list->free     = -1;
    list->data     = (ListElem*)calloc(capacity + 1, sizeof(ListElem));
    if (list->data != nullptr)
        for(int i = list->capacity; i >= 1; i--)
        {
            list->data[i] = {ELEM_POISON, -1, list->free};
            list->free    = i;
        }

    list->debug.name     = name;
    list->debug.function = function;
    list->debug.file     = file;
    list->debug.line     = line;
    list->debug.status   = true;
    list->linerized      = true;

    return ListCheck(list);
}

int ListDtor(List* list)
{
    ListCheck(list);

    list->capacity  = POISON;
    list->size      = POISON;
    list->free      = POISON;
    list->linerized = false;

    free(list->data);
    list->data = (ListElem*)POISON_PTR;

    list->debug.file     = (const char*)POISON_PTR;
    list->debug.function = (const char*)POISON_PTR;
    list->debug.name     = (const char*)POISON_PTR;
    list->debug.line     = POISON;

    return 0;
}

int FindFree(List* list, int* index)
{
    ReturnIfError(ListCheck(list));

    *index = list->free;
    list->free = list->data[list->free].prev;

    return 0;
}

int ListRemove(List* list, int index)
{
    ReturnIfError(ListCheck(list));

    CHECK(index > list->capacity || index <= 0, "Error index", -1);

    int next_ind = list->data[index].next;
    int prev_ind = list->data[index].prev;

    CHECK(next_ind == -1 || prev_ind == -1, "Index to not inserted element", -1);

    int tail = 0;
    ReturnIfError(ListEnd(list, &tail));
    if (index != tail)
        list->linerized = false;

    list->data[next_ind].prev = prev_ind;
    list->data[prev_ind].next = next_ind;

    list->data[index].val  = ELEM_POISON;
    list->data[index].prev = list->free;
    list->data[index].next = -1;

    list->free = index;
 
    list->size--;

    return 0;
}

int ResizeUp(List* list, int new_capacity)
{
    ReturnIfError(ListCheck(list));

    list->data = (ListElem*)realloc(list->data, sizeof(ListElem)*(new_capacity + 1));

    if (list->data == nullptr)
        return MEMORY_ALLOCATION_ERROR;

    for(int i = new_capacity; i >= list->capacity + 1; i--)
    {
        list->data[i] = {ELEM_POISON, -1, list->free};
        list->free = i;
    }
    
    list->capacity = new_capacity;

    return 0;
}

int ResizeIfNeed(List *list)
{
    if (list->capacity == list->size)
    {
        int new_capacity = 0;
        if (list->capacity == 0)
            new_capacity = 2;
        else
            new_capacity = list->capacity * ResizeCoef;
        ReturnIfError(ResizeUp(list, new_capacity));
    }

    return 0;
}

int ListInsert(List* list, ListElem_t value, int after_which, int* index) 
{
    ReturnIfError(ListCheck(list));

    CHECK(after_which > list->capacity || after_which < 0, "Error index", -1);
    
    CHECK(list->data[after_which].next == -1 || list->data[after_which].prev == -1, "Index to not inserted element", -1);

    ReturnIfError(ResizeIfNeed(list));

    int free_elem_index = -1;
    ReturnIfError(FindFree(list, &free_elem_index));

    if (index != nullptr && index != POISON_PTR)
        *index = free_elem_index;

    int tail = 0;
    ReturnIfError(ListEnd(list, &tail));
    if (after_which != tail)
        list->linerized = false;
    
    ListElem* new_elem = &list->data[free_elem_index];
    new_elem->val.name = (char*)calloc(strlen(value.name) + 1, sizeof(char));
    strcpy(new_elem->val.name, value.name);
    new_elem->val.index = value.index;
    
    int next       = list->data[after_which].next;
    new_elem->next = next;
    new_elem->prev = after_which;

    list->data[next].prev        = free_elem_index; 
    list->data[after_which].next = free_elem_index;

    list->size++;

    return 0;
}

#endif //__SYM_LIST__