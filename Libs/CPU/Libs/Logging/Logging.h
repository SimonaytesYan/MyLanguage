#ifndef __LOGGING_SYM__
#define __LOGGING_SYM__

#include <stdio.h>

int OpenLogFile(const char* file_name);

int CloseLogFile();

size_t LogPrintf(const char *format, ...);

#define CHECK(cond, msg, return_code)                                                          \
    if (cond)                                                                                  \
    {                                                                                          \
        LogPrintf("In %s in %s(%d)\n", __PRETTY_FUNCTION__ , __FILE__, __LINE__);                \
        LogPrintf(msg);                                                                        \
        return return_code;                                                                    \
    }

#define CHECK_SYNTAX(cond, msg, return_code, line)                                             \
    if (cond)                                                                                  \
    {                                                                                          \
        if (msg != "")                                                                         \
            LogPrintf("in line %d\n%s", line, msg);                                            \
        return return_code;                                                                    \
    }

#endif //__LOGGING_SYM__
