#ifndef SCESTAT_HEADER
#define SCESTAT_HEADER

typedef struct {
    int unknown_0x0;
    int unknown_0x4;
    int size;
    int unknown_0xc;
    int unknown_0x10;
    int unknown_0x14;
    int unknown_0x18;
    int unknown_0x1c;
    int unknown_0x20;
} sceStat;

int sceGetstat(char* path, sceStat* stat);
#endif
