#include "ps2rc/xff/xff.h"
#include "common/logging.h"
#include "instructions/RabbitizerInstruction.h"
#include "instructions/RabbitizerInstructionR5900.h"
#include "ps2rc/code_unit.h"
#include "ps2rc/elf/elf.h"
#include "ps2rc/splits.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

elf_section_header_t xff_section_header_to_elf(code_unit_t* unit, void* xff_file, xff_section_header_t* header, u32 header_index) {
    xff_header_t* xff = xff_file;
    elf_section_header_t new_section = {
        .name_index = ((u32*)xff_file + xff->section_name_offsets)[header_index],
        .offset = header->file_offset,
        .flags = 0, // TODO: Convert flags
        .address = 0,
        .size = header->length,
        .link = 0,
        .info = 0,
        .alignment = header->alignment,
        .entry_size = 0x20,
        .type = elf_section_header_type_from_section_type(elf_section_type_from_string(unit->section_names[header_index])),
    };
    
    return new_section;
}

s32 sort_splits(const void* a, const void* b) {
    const split_t* split_a = a;
    const split_t* split_b = b;

    if (split_a->start_address < split_b->start_address) {
        return -1;
    } else if (split_a->start_address > split_b->start_address) {
        return 1;
    } else {
        return 0;
    }
}

void add_split(split_t** splits, split_t split, u32* split_count, u32* split_capacity);

split_t* xff_find_splits(code_unit_t* unit, void* xff_file, u32* out_split_count) {
    u32 split_count = 0;
    u32 split_capacity = 1024;
    split_t* splits = malloc(sizeof(split_t) * split_capacity);

    xff_header_t* header = xff_file;

    // Search symbols
    elf_symbol_t* symbols = xff_file + header->symbol_file_offset;
    for (u32 i = 0; i < header->symbol_count; i++) {
        const elf_symbol_t symbol = symbols[i];

        // Skip special sections
        if (symbol.section_header_index < 0 || symbol.section_header_index >= header->section_count) {
            continue;
        }

        // TODO: Sections other than .text
        if (strcmp(unit->section_names[symbol.section_header_index], ".text") != 0) {
            continue;
        }
        elf_section_type_t section_type = elf_section_type_from_string(unit->section_names[symbol.section_header_index]);

        // TODO: Sections other than .text
        if (section_type != ELF_SECTION_TYPE_TEXT) {
            continue;
        }

        char* name_buffer = malloc(0x100);
        snprintf(name_buffer, 0x100, "function_0x%x", symbol.value);

        split_t split = {
            .signature = {
                .name = name_buffer,
                .function_parameter_count = 0,
                .is_lib_c = false,
                .return_type = "void",
                .generate = true,
            },
            .start_address = symbol.value,
            .end_address = 0,
            .section = elf_section_type_from_string(unit->section_names[symbol.section_header_index]),
            .generated = true,
        };

        if (split_count >= split_capacity) {
            split_t* tmp = malloc(sizeof(split_t) * split_capacity * 2);
            memcpy(tmp, splits, sizeof(split_t) * split_count);
            free(splits);
            split_capacity *= 2;
            splits = tmp;
        }

        splits[split_count++] = split;
    }

    // Find from relocations
    const xff_section_header_t* section_headers = xff_file + header->section_header_offset;
    const xff_relocation_header_t* reloc_headers = xff_file + header->relocation_header_offset;
    for (u32 i = 0; i < header->relocation_header_count; i++) {

        for (u32 header_index = 0; header_index < reloc_headers[i].relocation_count; header_index++) {
            relocation_t* relocations = xff_file + reloc_headers[i].file_offset;
            for (u32 r = 0; r < reloc_headers[i].relocation_count; r++) {
                const relocation_t relocation = relocations[r];

                const relocation_type_t type = relocation.packed_symbol_index & 0xff;
                const elf_symbol_t symbol = unit->symbols[relocation.packed_symbol_index >> 8];

                if (symbol.section_header_index < 0 || symbol.section_header_index >= header->section_count) {
                    continue;
                }

                // TODO: Non .text sections
                if (strcmp(unit->section_names[symbol.section_header_index], ".text") != 0) {
                    continue;
                }

                const xff_section_header_t section_header = section_headers[symbol.section_header_index];
                elf_section_type_t section_type = elf_section_type_from_string(unit->section_names[symbol.section_header_index]);

                // TODO: Sections other than .text
                if (section_type != ELF_SECTION_TYPE_TEXT) {
                    continue;
                }

                u32 instruction = *(u32*)(xff_file + section_header.file_offset + relocation.offset);

                switch (type) {
                    case RELOCATION_TYPE_HIGH16:
                    case RELOCATION_TYPE_LOW16:
                    case RELOCAITION_TYPE_GPREL16:
                    case RELOCATION_TYPE_LITERAL:
                    case RELOCATION_TYPE_GOT16:
                    case RELOCATION_TYPE_PC16:
                    case RELOCATION_TYPE_CALL16:
                    case RELOCATION_TYPE_GPREL32:
                    case RELOCATION_TYPE_NONE:
                    case RELOCATION_TYPE_16:
                    case RELOCATION_TYPE_32:
                    case RELOCATION_TYPE_REL32:
                        // log_warn("Cannot create split from relocation type %d", type);
                        break;
                    case RELOCATION_TYPE_26:
                        instruction += symbol.value & ((1 << 26) - 1);
                        break;
                }

                RabbitizerInstruction instr;
                RabbitizerInstructionR5900_init(&instr, instruction, relocation.offset);
                RabbitizerInstructionR5900_processUniqueId(&instr);
                u32 address = RabbitizerInstruction_getInstrIndexAsVram(&instr);

                if (address == 0) {
                    continue;
                }

                b8 existing_split = false;
                for (u32 split_index = 0; split_index < split_count; split_index++) {
                    if (splits[split_index].start_address == address) {
                        existing_split = true;
                        break;
                    }
                }

                if (existing_split) {
                    continue;
                }

                char* name_buffer = malloc(0x100);
                snprintf(name_buffer, 0x100, "function_0x%x", address);

                split_t split = {
                    .signature = {
                        .name = name_buffer,
                        .function_parameter_count = 0,
                        .is_lib_c = false,
                        .return_type = "void",
                        .generate = true,
                    },
                    .start_address = address,
                    .end_address = 0,
                    .section = section_type,
                    .generated = true,
                };

                log_debug("Created split 0x%x from relocation (0x%x)", address, instruction);
                if (split_count >= split_capacity) {
                    split_t* tmp = malloc(sizeof(split_t) * split_capacity * 2);
                    memcpy(tmp, splits, sizeof(split_t) * split_count);
                    free(splits);
                    split_capacity *= 2;
                    splits = tmp;
                }

                splits[split_count++] = split;
            }
        }
    }

    // Sort splits by address
    qsort(splits, split_count, sizeof(split_t), sort_splits);

    // TODO: This doesn't create an end address for the last symbol
    // Not 100% on how to calculate this without hackey workarounds 
    // Its a future me problem now
    for (u32 i = 0; i < split_count - 1; i++) {
        splits[i].end_address = splits[i + 1].start_address;

        log_debug("Found split 0x%x - 0x%x", splits[i].start_address, splits[i].end_address);
    }

    *out_split_count = split_count;
    return splits;
}
