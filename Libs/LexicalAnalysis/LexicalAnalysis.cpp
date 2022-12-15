#include <string.h>

#include "LexicalAnalysis.h"

static size_t get_text_size_and_number_lines(const char* file_name, 
                                             size_t*     number_lines_arg  = nullptr, 
                                             size_t*     max_line_size_arg = nullptr);

static size_t skip_spaces_s(const char* s);

static size_t skip_spaces_s(const char* s)
{
    size_t skipped = 0;
    while (s[skipped] == ' ' || s[skipped] == '\n' || s[skipped] == '\t' || s[skipped] == '\r')
        skipped++;
    
    return skipped;
}

Node* GetProgramFromFile(const char* program_file_name, int* program_size)
{
    size_t number_lines      = 0;
    size_t max_line_size     = 0;
    size_t program_text_size = get_text_size_and_number_lines(program_file_name, &number_lines, 
                                                                                 &max_line_size);

    #ifdef DEBUG
        printf("program text size = %d\n", program_text_size);
        printf("max line size     = %d\n", max_line_size);
        printf("number lines      = %d\n", number_lines);
    #endif
    
    FILE*       fp      = fopen(program_file_name, "r");
    const char* buffer  = (char*)calloc(1, max_line_size + 1);
    Node*       program = (Node*)calloc(program_text_size, sizeof(Node));
    int         ip      = 0;

    CHECK(fp      == nullptr, "error during opening program file",          nullptr);
    CHECK(buffer  == nullptr, "error during allocation memory for buffer",  nullptr);
    CHECK(program == nullptr, "error during allocation memory for program", nullptr);

    for(int i = 0; i < number_lines; i++)
    {
        int buffer_size = 0;
        char skip_n = 0;
        fscanf(fp, "%[^\n]%n%c", buffer, &buffer_size, &skip_n);
        #ifdef DEBUG
            printf("buffer_size = %d\n", buffer_size);
            fprintf(stderr, "[%d]buffer = <%s>\n", i, buffer);
        #endif

        int j = 0;
        j += skip_spaces_s(buffer + j);
        while (buffer[j] != '\0' && buffer[j] != '\n')
        {
            #ifdef DEBUG
                printf("ip = %d\n", ip);
                //fprintf(stderr, "buffer[%d] = <%s>\n", j, buffer + j);
            #endif

            int old_j = j;
            if ('0' <= buffer[j] && buffer[j] <= '9')
            {
                #ifdef DEBUG
                    fprintf(stderr, "NUMBER\n");
                #endif
                int new_number = 0;
                int add_to_j   = 0;
                sscanf(buffer + j, "%d%n", &new_number, &add_to_j);
                program[ip] = _NodeCtorNum(new_number);
                program[ip].val.number_cmp_in_text = j;

                j += add_to_j;
            }
            else
            {
                bool operator_found = false;
                for(int k = 0; k < STD_OPERATORS_NUM; k++)
                {
                    size_t lenght = strlen(STD_OPERATORS[k].name);

                    if (!strncmp(buffer + j, STD_OPERATORS[k].name, lenght))
                    {
                        #ifdef DEBUG
                            fprintf(stderr, "OPERATOR\n");
                        #endif

                        program[ip] = _NodeCtorOp(STD_OPERATORS[k].code);
                        program[ip].val.number_cmp_in_text = j;
                        operator_found = true;
                        j += lenght;
                        break;
                    }
                }
                if (!operator_found)
                {
                    #ifdef DEBUG
                        fprintf(stderr, "VAR\n");
                    #endif
                    int        add_to_j               = 0;
                    const char new_var[MAX_VAR_SIZE]  = "";
                    sscanf(buffer + j, "%[a-zA-Z0-9_]%n", new_var, &add_to_j);

                    if (add_to_j != 0)
                    {
                        program[ip] = _NodeCtorVar(new_var);
                        program[ip].val.number_cmp_in_text = j;
                        j += add_to_j;
                    }
                }
            }
            
            #ifdef DEBUG
                printf("j     = %d\n", j);
                printf("old_j = %d\n", old_j);
            #endif

            if (j == old_j)
            {
                #ifdef DEBUG
                    fprintf(stderr, "SYMBOL\n");
                #endif
                int symbol   = 0;
                int add_to_j = 0;
                sscanf(buffer + j, "%c%n", &symbol, &add_to_j);

                program[ip] = _NodeCtorSymb(symbol);
                program[ip].val.number_cmp_in_text = j;
                j += add_to_j;
            }
            ip++;
            j += skip_spaces_s(buffer + j);
        }

        #ifdef DEBUG
            fprintf(stderr, "End loop\n");
        #endif
    }
    
    #ifdef DEBUG
        fprintf(stderr, "End program\n");
    #endif
    program = (Node*)realloc(program, ip * sizeof(Node));

    free((void*)buffer);
    fclose(fp);

    if (program_size != nullptr)
        *program_size = ip;

    return program;
}

static size_t get_text_size_and_number_lines(const char *file_name, size_t* number_lines_arg, 
                                                             size_t* max_line_size_arg)
{
    assert(file_name != nullptr);
    FILE* fp = fopen(file_name, "r");

    size_t line_size     = 0;
    size_t max_line_size = 0;

    size_t text_size     = 0;
    size_t number_lines  = 1;
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