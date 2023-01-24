#ifndef __SYM_SAVE_GET_ALT__
#define __SYM_SAVE_GET_ALT__

#include "../Setup.h"

int SaveTreeInFile(const Tree* tree, const char* path);

int GetTreeFromFile(Tree* tree, const char* path);

#endif //__SYM_SAVE_GET_ALT__