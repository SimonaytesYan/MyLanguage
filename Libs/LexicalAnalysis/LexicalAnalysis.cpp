#include <sys\stat.h>
#include <string.h>

#include "LexicalAnalysis.h"

static size_t get_text_size_and_number_lines(const char* file_name, 
                                             size_t*     number_lines_arg  = nullptr, 
                                             size_t*     max_line_size_arg = nullptr);

Node* GetProgramFromFile(const char* program_file_name)
{
    size_t number_lines      = 0;
    size_t max_line_size     = 0;
    size_t program_text_size = get_text_size_and_number_lines(program_file_name, &number_lines, 
                                                                                 &max_line_size);

    FILE*       fp      = fopen(program_file_name, "r");    
    const char* Buffer  = (char*)calloc(1, max_line_size + 1);
    Node*       program = (Node*)calloc(program_text_size, sizeof(Node));
    int         ip      = 0;

    for(int i = 0; i < number_lines; i++)
    {
        fscanf(fp, "[^\n]" ,Buffer);
        int j = 0;
        while (Buffer[j] != '\0' && Buffer[j] != '\n')
        {
            if ('0' <= Buffer[j] && Buffer[j] <= '9')
            {
                int new_number = 0;
                int add_to_j   = 0;
                sscanf(Buffer + j, "%d%n", &new_number, &add_to_j);
                program[ip] = _NodeCtorNum(new_number);

                j += add_to_j;
            }
            else
            {
                bool operator_found = false;
                for(int k = 0; k < STD_OPERATORS_NUM; k++)
                {
                    size_t lenght = strlen(STD_OPERATORS[k].name);

                    if (!strncmp(Buffer + j, STD_OPERATORS[k].name, lenght))
                    {
                        program[ip] = _NodeCtorOp(STD_OPERATORS[k].code);
                        operator_found = true;
                        j += lenght;
                        break;
                    }
                }
                if (!operator_found)
                {
                    int         add_to_j = 0;
                    const char* new_var  = nullptr;
                    sscanf(Buffer + j, "%[a-zA-Z0-9_]%n", new_var, &add_to_j);
                    program[ip] = _NodeCtorVar(new_var);
                }
            }
            ip++;
        }
    }
    program = (Node*)realloc(program, ip * sizeof(Node));

    free((void*)Buffer);
    fclose(fp);

    return program;
}

size_t get_text_size_and_number_lines(const char *file_name, size_t* number_lines_arg, 
                                                             size_t* max_line_size_arg)
{
    assert(file_name != nullptr);
    FILE* fp = fopen(file_name, "r");

    size_t line_size     = 0;
    size_t max_line_size = 0;

    size_t text_size     = 0;
    size_t number_lines  = 0;
    int    c             = 0;
    while ((c = getc(fp)) != EOF)
    {
        text_size++;
        if (c == '\n')
        {
            if (max_line_size < line_size)
                max_line_size = line_size;
            line_size = 0;

            number_lines++;
        }
        else
            line_size++;
    }
    if (max_line_size < line_size)
        max_line_size = line_size;
    
    fclose(fp);

    if (number_lines_arg != nullptr)
        *number_lines_arg = number_lines;
    if (max_line_size_arg != nullptr)
        *max_line_size_arg = max_line_size;
    return text_size;
}