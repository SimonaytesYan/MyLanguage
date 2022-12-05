#ifndef __PRINT_ELEM_SYM__
#define __PRINT_ELEM_SYM__

#include <stdio.h>

#include "Errors.h"
#include "Logging/Logging.h"

const int OUTPUT_TYPE = 0;   //!This constant is used to print stack elements to logs in right format
                             //!0 - int
                             //!1 - char
                             //!2 - float
                             //!3 - double
                             //!4 - long long

#define LOGS_TO_FILE

inline size_t PrintElem(Elem value)
{
    char format[5] = "%";
    switch (OUTPUT_TYPE)
    {
        case 0:
            format[1] = 'd';
        break;
        case 1:
            format[1] = 'c';
        break;
        case 2:
            format[1] = 'f';
        break;
        case 3:
            format[1] = 'l';
            format[2] = 'g';
        break;
        case 4:
            format[1] = 'l';
            format[2] = 'l';
            format[3] = 'u';
        break;
        default:
            LogPrintf("Wrong format in PrintElem\n");            
    }
    
    LogPrintf(format, value);

    return NO_ERROR; 
}

#endif //__PRINT_ELEM_SYM__