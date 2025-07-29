#include "ps2ctx/sce/disk/sceStat.h"
#include <stdio.h>
#include "ps2ctx/sce/patches/convert_path.h"

int sceGetstat(char* path, sceStat* stat) {
    path = ConvertDiskPath(path);
    FILE* file = fopen(path, "rb");
    
    // Failed to open file
    if (!file)
    {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    stat->size = ftell(file);

    fclose(file);
    return 0;
}
