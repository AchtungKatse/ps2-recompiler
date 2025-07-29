#ifndef SCEOPENHEADER
#define SCEOPENHEADER
#include <stdio.h>

#define SCE_OPEN_READONLY 1
#define SCE_OPEN_WRITEONLY 2
#define SCE_OPEN_READWRITE 3

FILE* sceOpen(char* name, int flags, int permissions);

#endif
