// https://forums.ps2dev.org/viewtopic.php?t=347
#include "ps2ctx/sce/disk/sceOpen.h"
#include "ps2ctx/sce/patches/convert_path.h"

#include <stdio.h>

FILE* sceOpen(char* path, int flags, int permissions)
{
    path = ConvertDiskPath(path);
    printf("Opening file: %s", path);
    char* mode;
    switch (flags & 0x7) // Assuming that the first 3 bits are the RW? flags
    { 
        case SCE_OPEN_READONLY:
            mode = "r";
            break;
        case SCE_OPEN_WRITEONLY:
            mode = "w";
            break;
        default:
            mode = "r";
            break;
    }
    return fopen(path, mode);
}
