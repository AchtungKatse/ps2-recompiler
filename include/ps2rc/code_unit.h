#pragma once

#include "ps2rc/defines.h"
#include "ps2rc/elf/elf.h"
#include "ps2rc/splits.h"

#define MAX_CODE_UNITS 32

typedef struct code_unit {
    const char* name;
    const char* file_path;
    const char* splits_path;
    const char* type;

    elf_section_header_t* section_headers;
    const char** section_names;
    const char* string_table;
    relocation_t* relocations;
    split_t* splits;
    elf_symbol_t* symbols;
    void* data;

    u32 section_header_count;
    u32 split_count;
    u32 relocation_count;
    u32 symbol_count;

} code_unit_t;

typedef struct ps2rc_section {
    const char* name;
    u8* data;
    u32 length;
    u32 address;
} ps2rc_section_t;
