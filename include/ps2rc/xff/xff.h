#pragma once

#include "ps2rc/code_unit.h"
#include "ps2rc/defines.h"
#include "ps2rc/elf/elf.h"
typedef struct xff_relocation_header {
    u32 type;
    u32 relocation_count;
    u32 section_index;
    u32 runtime_relocations_offset;
    u32 runtime_ptr;
    u32 file_offset;
    u32 length;
} xff_relocation_header_t;

typedef struct xff_section_header {
    u32 header_padding;
    u32 runtime_address;
    u32 length;
    u32 alignment;
    u32 type;
    u32 unk_1;
    u32 unk_2;
    u32 file_offset;
} xff_section_header_t;

elf_section_header_t xff_section_header_to_elf(code_unit_t* unit, void* xff_file, xff_section_header_t* header, u32 header_index);

typedef struct xff_header {
    // 0x0
    u32 magic;
    u32 unk_1;
    u32 unk_2;
    u32 unk_sections;

    // 0x10
    u32 runtime_unk_3;
    u32 file_length;
    u32 runtime_end_offset; // Runtime calculated
    u32 runtime_string_count; // Runtime calculated

    // 0x20
    u32 runtime_unk_count; // Runtime calculated
    u32 symbol_count;
    u32 runtime_symbol_ptr; // Runtime calculated
    u32 runtime_symbol_sections;

    // 0x30
    u32 runtime_section_headers;
    u32 symbol_addresses;
    u32 relocation_header_count;
    u32 runtime_relocation_headers;

    // 0x40
    u32 section_count;
    u32 runtime_section_name_offset;
    u32 runtime_section_str_table_vram;
    u32 unk_count;

    // 0x50
    u32 runtime_string_offset_ptr;
    u32 symbol_file_offset; 
    u32 string_table_offset;
    u32 section_header_offset;

    // 0x60
    u32 symbols_offset_table; // Ptr to a table of u32 addresses where the index is the same as the symbol index
    u32 relocation_header_offset;
    u32 section_name_offsets; // Ptr to table with an array of offsets into the string table for the section names
    u32 section_string_table_ptr;
} xff_header_t;

split_t* xff_find_splits(code_unit_t* unit, void* xff_file, u32* out_split_count);
