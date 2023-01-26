#include <string.h>

#include "LexicalAnalysis.h"

static size_t get_text_size_and_number_lines(const char* file_name, 
                                             size_t*     number_lines_arg  = nullptr, 
                                             size_t*     max_line_size_arg = nullptr);

static size_t skip_spaces_s(const char* s);

static size_t skip_spaces_s(const char* s)
{
    size_t skipped = 0;
    while (s[skipped] == '\n' || s[skipped] == ' ' || s[skipped] == '\t' || s[skipped] == '\r')
        skipped++;
    
    return skipped;
}

static void GetNumber(const char* buffer, size_t *ind, const size_t ip, Node* program, const size_t line)
{
    #ifdef DEBUG
        fprintf(stderr, "NUMBER\n");
    #endif
    int new_number = 0;
    int add_to_ind = 0;
    sscanf(buffer + *ind, "%d%n", &new_number, &add_to_ind);
    program[ip] = _NodeCtorNum(new_number);
    program[ip].val.number_cmd_in_text      = *ind + 1;
    program[ip].val.number_cmd_line_in_text = line + 1;

    (*ind) += (size_t)add_to_ind;
}

static bool GetOperator(const char* buffer, size_t *ind, const size_t ip, Node* program, const size_t line)
{
    bool operator_found = false;
    for(int k = 0; k < STD_OPERATORS_NUM; k++)
    {
        #ifdef DEBUG
            printf("<%s> %d\n", STD_OPERATORS[k].name, STD_OPERATORS[k].code);
        #endif

        size_t lenght = strlen(STD_OPERATORS[k].name);

        if (!strncmp(buffer + *ind, STD_OPERATORS[k].name, lenght))
        {
            #ifdef DEBUG
                fprintf(stderr, "OPERATOR\n");
            #endif

            program[ip] = _NodeCtorOp(STD_OPERATORS[k].code);
            program[ip].val.number_cmd_in_text = *ind + 1;
            program[ip].val.number_cmd_line_in_text = line + 1;
            operator_found = true;
            (*ind) += lenght;
            break;
        }
    }
    return operator_found;
}

static void GetVar(const char* buffer, size_t *ind, const size_t ip, Node* program, const size_t line)
{
    int  add_to_ind               = 0;
    char new_var[MAX_VAR_SIZE]  = "";
    sscanf(buffer + *ind, "%[a-zA-Z0-9_]%n", new_var, &add_to_ind);

    if (add_to_ind != 0)
    {
        #ifdef DEBUG
            fprintf(stderr, "VAR\n");
        #endif
        program[ip] = _NodeCtorVar(new_var);
        program[ip].val.number_cmd_in_text = *ind + 1;
        program[ip].val.number_cmd_line_in_text = line + 1;
        (*ind) += (size_t)add_to_ind;
    }
}

static void GetSymbol(const char* buffer, size_t *ind, const size_t ip, Node* program, const size_t line)
{
    #ifdef DEBUG
        fprintf(stderr, "SYMBOL\n");
    #endif

    int symbol   = 0;
    int add_to_ind = 0;
    
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat="
    sscanf(buffer + *ind, "%c%n", &symbol, &add_to_ind);
    #pragma GCC diagnostic pop

    program[ip] = _NodeCtorSymb(symbol);
    program[ip].val.number_cmd_in_text      = *ind + 1;
    program[ip].val.number_cmd_line_in_text = line + 1;
    (*ind) += (size_t)add_to_ind;
}

static bool GetKeyword(const char* buffer, size_t *ind, const size_t ip, Node* program, const size_t line)
{
    bool keyword_found = false;
    for(int k = 0; k < KEYWORDS_NUM; k++)
    {
        size_t lenght = strlen(KEYWORDS[k].name);

        if (!strncmp(buffer + *ind, KEYWORDS[k].name, lenght))
        {
            #ifdef DEBUG
                fprintf(stderr, "KEYWORD\n");
            #endif

            program[ip] = _NodeCtorKeyword(KEYWORDS[k].code);
            program[ip].val.number_cmd_in_text = *ind + 1;
            program[ip].val.number_cmd_line_in_text = line + 1;
            keyword_found = true;
            (*ind) += lenght;
            break;
        }
    }
    return keyword_found;
}


Node* GetProgramFromFile(const char* program_file_name, size_t* program_size)
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
    
    FILE*  fp      = fopen(program_file_name, "r");
    char*  buffer  = (char*)calloc(1, max_line_size + 1);
    Node*  program = (Node*)calloc(program_text_size, sizeof(Node));
    size_t ip      = 0;

    CHECK(fp      == nullptr, "error during opening program file",          nullptr);
    CHECK(buffer  == nullptr, "error during allocation memory for buffer",  nullptr);
    CHECK(program == nullptr, "error during allocation memory for program", nullptr);

    for(size_t line = 0; line < number_lines; line++)
    {
        int buffer_size = 0;
        fscanf(fp, "%[^\n]%n", buffer, &buffer_size);
        fgetc(fp);  //!skip \n. If put \n in end of fscanf program will skip all space character including empy strings, so line numbering breaks down
       
        #ifdef DEBUG
            for(int i = 0; i < buffer_size; i++)
                printf("<%c>[%d]\n", buffer[i], buffer[i]);
            printf("buffer_size = %d\n", buffer_size);
            printf("line = %d\n", line);
        #endif

        if (buffer_size == 0)
            continue;

        size_t ind = 0;
        ind += skip_spaces_s(buffer + ind);
        while (buffer[ind] != '\0' && buffer[ind] != '\r' && buffer[ind] != '\n')
        {
            size_t old_ind = ind;
            if ('0' <= buffer[ind] && buffer[ind] <= '9')
                GetNumber(buffer, &ind, ip, program, line);
            else if (!GetOperator(buffer, &ind, ip, program, line))
            {
                if (!GetKeyword(buffer, &ind, ip, program, line))
                    GetVar(buffer, &ind, ip, program, line);
            }
            
            if (ind == old_ind)
                GetSymbol(buffer, &ind, ip, program, line);
            ip++;
            ind += skip_spaces_s(buffer + ind);
        }
        buffer_size = 0;
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