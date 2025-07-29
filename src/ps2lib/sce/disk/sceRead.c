#include <stdio.h>

int sceRead(FILE* data, void* output, int length)
{
    return fread(output, 1, length, data);
}
