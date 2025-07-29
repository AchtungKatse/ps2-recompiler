#include "common/logging.h"
#include "ps2ctx/ps2_context.h"
#include "ps2ctx/code_units/code_unit.h"
#include "ps2rc/code_unit.h"

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern u32 __ps2_rc_section_count;
extern ps2rc_section_t __ps2_rc_sections[];

static u32 CountLeadingSignBits(s32 n) {
    // If the sign bit is 1, we invert the bits to 0 for count-leading-zero.
    if (n < 0)
        n = ~n;

    // If BSR is used directly, it would have an undefined value for 0.
    if (n == 0)
        return 32;

    // Perform our count leading zero.
    // return std::countl_zero(static_cast<u32>(n));
    int c =0;
    for (int i = 0; i < 32; i++)
    {
        if (c < 0)
            break;

        c++;
        n <<= 1;
    }

    return c;
}

void* FindRam(u64 address) {
    if (address < 0x2000000 && address >= 0)
        return mem->mainRAM + address;
    else if (address >= 0x10000000 && address < 0x10000000 + 64 * KB)
        return mem->ioRegisterRAM + address -  0x10000000;
    else if (address >= 0x11000000 && address < 0x11000000 + 4 * KB)
        return mem->vu0CodeRAM + address - 0x11000000;
    else if (address >= 0x11004000 && address < 0x11004000 + 4 * KB)
        return mem->vu0DataRAM + address - 0x11004000;
    else if (address >= 0x11008000 && address <= 0x11008000 + 16 * KB)
        return mem->vu1CodeRAM + address - 0x11008000;
    else if (address >= 0x1100C000 && address < 0x1100C000 + 16 * KB)
        return mem->vu1DataRAM + address - 0x1100c000;
    else if (address >= 0x12000000 && address < 0x12000000 + 8 * KB)
        return mem->gsPrivilegedRAM + address - 0x12000000;
    else if (address >= 0x1C000000 && address < 0x1C000000 + 2 * MB)
        return mem->iopRAM + address - 0x1c000000;

    if (address < 0xffffffff)
    {
        // Something went wrong
        // Usually reading a u64 as u32
        // Try to append top 32 bits to value
        if (address >> 28 == 0xf) {
            void* addr = (void*)((0x7fffl << 32) | address);
            return addr;
        }
        if (address >> 28 == 0x5) { 
            void* addr = (void*)((0x5555l << 32) | address);
            return addr; 
        }

        printf("Failed to get proper host address: %lu\n", address);
        return mem->mainRAM;
    }

    return (void*)address;
}

void RecompilationInit() {
    log_info("RC init");
    ctx = (RecompilationContext*)malloc(sizeof(RecompilationContext));
    memset(ctx, 0, sizeof(RecompilationContext));

    mem = malloc(sizeof(PS2Memory));
    mem->mainRAM = malloc(64 * MB);
    mem->ioRegisterRAM = malloc(64 * KB);
    mem->vu0CodeRAM = malloc(4 * KB);
    mem->vu0DataRAM = malloc(4 * KB);
    mem->vu1CodeRAM = malloc(16 * KB);
    mem->vu1DataRAM = malloc(16 * KB);
    mem->gsPrivilegedRAM = malloc(8 * KB);
    mem->iopRAM = malloc(2 * MB);

    ctx->Status = 0x70030C11;
    ctx->gp.U32[0] = 0x141D70;
    ctx->sp.U32[0] = 0x1FFFD60;
    /*ctx->gp.U32[0] = 62 * MB;*/
    /*ctx->sp.U32[0] = 63 * MB ;*/

    memset(mem->mainRAM, 0, 32 * MB);
    memset(mem->ioRegisterRAM, 0, 64 * KB);
    memset(mem->vu0CodeRAM, 0, 4 * KB);
    memset(mem->vu0DataRAM, 0, 4 * KB);
    memset(mem->vu1CodeRAM, 0, 16 * KB);
    memset(mem->vu1DataRAM, 0, 16 * KB);
    memset(mem->gsPrivilegedRAM, 0, 8 * KB);
    memset(mem->iopRAM, 0, 2 * MB);

    // Load code units
    for (u32 i = 0; i < __ps2_rc_section_count; i++) {
        ps2rc_section_t section = __ps2_rc_sections[i];
        if (section.address) {
            memcpy(mem->mainRAM + section.address, section.data, section.length);
        }
    }

    const struct CodeUnit* codeUnits = GetUnitPaths();
    for (int i = 0; i < numUnitPaths; i++) {
        switch (codeUnits[i].type) {
            case CodeUnit_ELF:
                // LoadELF(&codeUnits[i]);
                break;
            case CodeUnit_XFF:
                // Done in runtime by xff or known at compile time
                break;
        }
    }
}
