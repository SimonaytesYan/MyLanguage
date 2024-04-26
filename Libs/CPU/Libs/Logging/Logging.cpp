#include "Logging.h"
#include "../Errors.h"

#define LOGS_TO_FILE

#include <stdarg.h>

FILE* logs_file = nullptr;

int OpenLogFile(const char* file_name)
{
    logs_file = fopen(file_name, "w");
    if (logs_file == nullptr)
    {
        fprintf(stderr, "Error during open logs\n");
        return -1;
    }
    return 0;
}

int CloseLogFile()
{
    if (logs_file == nullptr)
    {
        return -1;
    }
    fclose(logs_file);
    return 0;
}

size_t LogPrintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    #ifdef LOGS_TO_CONSOLE
        vprintf(format, args);
    #endif

    #ifdef LOGS_TO_FILE
        if (logs_file == nullptr)
            return ERROR_LOGS_OPEN;
        vfprintf(logs_file, format, args);
    #endif

    va_end(args);
    fflush(logs_file);

    return NO_ERROR;
}
