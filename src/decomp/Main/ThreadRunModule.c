#include "common/logging.h"
#include "ps2ctx/ps2_context.h"
#include "ps2ctx/sce/patches/convert_path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

undefined LoadModule(const char* module_path);

void ThreadRunModule(char* moduleName) {
    // Basically, find the known entry point for a given module path
    const char* path = ConvertDiskPath(FindRam((u64)moduleName));
    LoadModule(FindRam((u64)moduleName));

    if (strcmp(path, "GameFiles/KERNEL.XFF") == 0) {
        log_info("Attempting to load kernel entry point\n");
        ctx->a0.U32[0] = 1;
        ctx->a1.U32[0] = 0x00131BAC;
        ioskernel();
    } else if (strcmp(path, "GameFiles/GAMECORE.XFF") == 0) {
        log_info("Attempting to load gamecore entry point\n");
    } else {
        log_error("Unknown module entry point: %s\n", (const char*)FindRam((u64)moduleName));
        abort();
    }


    // int threadId = GetThreadId();
    // ModuleInfo* moduleInfo = (ModuleInfo*)LoadModule(moduleName);
    // if (!moduleInfo) {
    //     printf("Can't execute because map fail.\n");
    // }
    // else {
    //     void (* entryPoint)(int) = FindRam(moduleInfo->entryPoint);
    //     FlushCache(0);
    //     FlushCache(2);
    //     LoaderSysPrintf("ld: execute entry point (%p) as h10kmode (id: %x)", entryPoint, 0 /* Meant to be loader version id */);
    //     entryPoint(1);
    // }
    // LoaderSysDeleteExternalThreadList(threadId);
    // ExitDeleteThread();
}
