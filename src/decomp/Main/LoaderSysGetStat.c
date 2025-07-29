#include "ps2ctx/sce/disk/sceStat.h"

int LoaderSysGetstat(char* path, void* buffer) {
    return sceGetstat(path, buffer);
}
