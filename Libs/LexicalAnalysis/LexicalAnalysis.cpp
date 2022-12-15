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

static void GetNumber(const char* buffer, size_t *ind, const size_t ip, Node* program, const int line)
{
    #ifdef DEBUG
        fprintf(stderr, "NUMBER\n");
    #endif
    int new_number = 0;
    int add_to_ind = 0;
    sscanf(buffer + *ind, "%d%n", &new_number, &add_to_ind);
    program[ip] = _NodeCtorNum(new_number);
    program[ip].val.number_cmd_in_text      = *ind;
    program[ip].val.number_cmd_line_in_text = line;

    (*ind) += add_to_ind;
}

static bool GetOperator(const char* buffer, size_t *ind, const size_t ip, Node* program, const int line)
{
    bool operator_found = false;
    for(int k = 0; k < STD_OPERATORS_NUM; k++)
    {
        size_t lenght = strlen(STD_OPERATORS[k].name);

        if (!strncmp(buffer + *ind, STD_OPERATORS[k].name, lenght))
        {
            #ifdef DEBUG
                fprintf(stderr, "OPERATOR\n");
            #endif

            program[ip] = _NodeCtorOp(STD_OPERATORS[k].code);
            program[ip].val.number_cmd_in_text = *ind;
            program[ip].val.number_cmd_line_in_text = line;
            operator_found = true;
            (*ind) += lenght;
            break;
        }
    }
    return operator_found;
}

static void GetVar(const char* buffer, size_t *ind, const size_t ip, Node* program, const int line)
{
    int        add_to_ind               = 0;
    const char new_var[MAX_VAR_SIZE]  = "";
    sscanf(buffer + *ind, "%[a-zA-Z0-9_]%n", new_var, &add_to_ind);

    if (add_to_ind != 0)
    {
        #ifdef DEBUG
            fprintf(stderr, "VAR\n");
        #endif
        program[ip] = _NodeCtorVar(new_var);
        program[ip].val.number_cmd_in_text = *ind;
        program[ip].val.number_cmd_line_in_text = line;
        (*ind) += add_to_ind;
    }
}

static void GetSymbol(const char* buffer, size_t *ind, const size_t ip, Node* program, const int line)
{
    #ifdef DEBUG
        fprintf(stderr, "SYMBOL\n");
    #endif

    int symbol   = 0;
    int add_to_ind = 0;
    sscanf(buffer + *ind, "%c%n", &symbol, &add_to_ind);

    program[ip] = _NodeCtorSymb(symbol);
    program[ip].val.number_cmd_in_text      = *ind;
    program[ip].val.number_cmd_line_in_text = line;
    (*ind) += add_to_ind;
}

static bool GetKeyword(const char* buffer, size_t *ind, const size_t ip, Node* program, const int line)
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
            program[ip].val.number_cmd_in_text = *ind;
            program[ip].val.number_cmd_line_in_text = line;
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
    
    FILE*       fp      = fopen(program_file_name, "r");
    const char* buffer  = (char*)calloc(1, max_line_size + 1);
    Node*       program = (Node*)calloc(program_text_size, sizeof(Node));
    int         ip      = 0;

    CHECK(fp      == nullptr, "error during opening program file",          nullptr);
    CHECK(buffer  == nullptr, "error during allocation memory for buffer",  nullptr);
    CHECK(program == nullptr, "error during allocation memory for program", nullptr);

    for(int line = 0; line < number_lines; line++)
    {
        int buffer_size = 0;
        fscanf(fp, "%[^\n]\n%n", buffer, &buffer_size);
        if (buffer_size == 0)
            continue;
        #ifdef DEBUG
            printf("buffer_size = %d\n", buffer_size);
            fprintf(stderr, "[%d]buffer = <%s>\n", line, buffer);
        #endif

        size_t ind = 0;
        ind += skip_spaces_s(buffer + ind);
        while (buffer[ind] != '\0' && buffer[ind] != '\n')
        {
            #ifdef DEBUG
                printf("ip = %d\n", ip);
            #endif

            int old_ind = ind;
            if ('0' <= buffer[ind] && buffer[ind] <= '9')
                GetNumber(buffer, &ind, ip, program, line);
            else if (!GetOperator(buffer, &ind, ip, program, line))
            {
                if (!GetKeyword(buffer, &ind, ip, program, line))
                    GetVar(buffer, &ind, ip, program, line);
            }
            
            #ifdef DEBUG
                printf("ind     = %d\n", ind);
                printf("old_ind = %d\n", old_ind);
            #endif

            if (ind == old_ind)
                GetSymbol(buffer, &ind, ip, program, line);
            ip++;
            ind += skip_spaces_s(buffer + ind);
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