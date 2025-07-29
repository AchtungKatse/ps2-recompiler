#include "ps2rc/code_unit.h"
#include "ps2rc/defines.h"
#include "ps2rc/elf/elf.h"
#include "ps2rc/ps2_registers.h"
#include "ps2rc/rc_config.h"
#include "ps2rc/rc_context.h"
#include "ps2rc/splits.h"
#include "ps2rc/cemitter.h"
#include "common/logging.h"
#include "ps2rc/xff/xff.h"

#include <rabbitizer.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <yaml.h>

u32 failed_jumps_count = 0;
u32 failed_jumps[1000] = {};
void try_make_directory(const char* path);
void process_section(rc_context_t* context, code_unit_t* unit, u32 section_index, const char* output_path);
void process_code_unit(rc_context_t* context, code_unit_t* unit, const char* output_path);
void append_string(FILE* file, const char* format, ...);
void write_function_list(rc_context_t* context, const char* output_path);
void write_functions(rc_context_t* context, const char* output_path);
void write_sections(const char* output_path, rc_context_t* context);
void write_sections_header(const char* output_path, rc_context_t* context);
int sort_failed_jumps(const void* _a, const void* _b);

s32 main(s32 argc, char** argv) {
    enum args {
        arg_program_path,
        arg_config_path,
        arg_output_path,
        arg_count
    };

    ASSERT(argc >= arg_count, "Invalid format: Requires at least one argument. Expected format ./recompiler <config.yaml> <output_path>");

    // Create output path char* output_path = argv[arg_output_path];
    const char* output_path = argv[arg_output_path];
    try_make_directory(output_path);

    // Load config
    char* config_path = argv[arg_config_path];

    rc_config_t rc_config = {};
    process_config_file(config_path, &rc_config);

    // Create rc context
    rc_context_t context = { 
        .code_units = malloc(sizeof(code_unit_t) * rc_config.code_unit_count),
        .code_unit_count = rc_config.code_unit_count,
    };
    memset(context.code_units, 0, sizeof(code_unit_t) * rc_config.code_unit_count);

    // Check if we can load the file
    const u32 path_buffer_size = 0x100;
    char path_buffer[path_buffer_size];
    for (u32 i = 0; i < rc_config.code_unit_count; i++) {
        code_unit_t* unit = &rc_config.code_units[i];
        memcpy(&context.code_units[i], unit, sizeof(code_unit_t));
        context.code_units[i].splits = split_read_from_yaml(unit->splits_path, &context.code_units[i].split_count);
    }

    for (u32 i = 0; i < rc_config.code_unit_count; i++) {
        snprintf(path_buffer, path_buffer_size, "%s/%s", output_path, context.code_units[i].name);
        process_code_unit(&context, &context.code_units[i], path_buffer);
    }

    qsort(failed_jumps, failed_jumps_count, sizeof(u32), sort_failed_jumps);
    for (u32 i = 0; i < failed_jumps_count; i++) {
        log_error("\t0x%x", failed_jumps[i]);
    }
    if (failed_jumps_count > 0) {
        log_error("Failed to find %d jumps.", failed_jumps_count);
    }

    write_functions(&context, output_path);
    write_function_list(&context, output_path);
    write_sections_header(output_path, &context);
    write_sections(output_path, &context);

    return 0;
}

int sort_failed_jumps(const void* _a, const void* _b) {
    u32 a = *(u32*)_a;
    u32 b = *(u32*)_b;
    if (a < b) return 1;
    if (a > b) return -1;
    return 0;
}

const char* section_name_to_index(const char* name) {
    u32 len = strlen(name);
    char* new_name = malloc(len);
    strcpy(new_name, name);
    for (u32 i = 0; i < len; i++) {
        if (new_name[i] == '.') {
            new_name[i] = '_';
        }
    }

    return new_name;
}

void write_sections_header(const char* output_path, rc_context_t* context) {
    // Open file
    char path_buffer[0x200];
    snprintf(path_buffer, 0x200, "%s/sections.h", output_path);
    FILE* file = fopen(path_buffer, "w");
    ASSERT(file, "Failed to open file at %s", path_buffer);

    // Get all loaded section count
    u32 loaded_section_count = 0;
    for (u32 i = 0; i < context->code_unit_count; i++) {
        code_unit_t* unit = &context->code_units[i];
        for (u32 s = 0; s < unit->section_header_count; s++) {
            elf_section_header_t* header = &unit->section_headers[s];

            if (header->type == SHT_NULL) {
                continue;
            }

            loaded_section_count++;
        }
    }

    // Write all section info
    append_string(file, "#pragma once\n");
    append_string(file, "#include \"ps2rc/code_unit.h\"\n");
    append_string(file, "static const u32 __ps2_rc_section_count = %d;\n", loaded_section_count);
    append_string(file, "extern ps2rc_section_t __ps2_rc_sections[];\n");
    for (u32 i = 0, section_index = 0; i < context->code_unit_count; i++) {
        code_unit_t* unit = &context->code_units[i];
        for (u32 s = 0; s < unit->section_header_count; s++) {
            elf_section_header_t* header = &unit->section_headers[s];

            if (header->type == SHT_NULL) {
                continue;
            }

            append_string(file, "static const u32 %s_%s_section_index = %d;\n", unit->name, section_name_to_index(unit->section_names[s]), section_index++);
        }
    }

    fclose(file);
}

void write_sections(const char* output_path, rc_context_t* context) {
    // Open file
    char path_buffer[0x200];
    snprintf(path_buffer, 0x200, "%s/sections.c", output_path);
    FILE* file = fopen(path_buffer, "w");
    ASSERT(file, "Failed to open file at %s", path_buffer);

    // Get all loaded section count
    u32 loaded_section_count = 0;
    for (u32 i = 0; i < context->code_unit_count; i++) {
        code_unit_t* unit = &context->code_units[i];
        for (u32 s = 0; s < unit->section_header_count; s++) {
            elf_section_header_t* header = &unit->section_headers[s];

            if (header->type == SHT_NULL) {
                continue;
            }

            loaded_section_count++;
        }
    }

    // Write all sections
    char section_name_buffer[0x200];

    append_string(file, "#include \"ps2rc/code_unit.h\"\n");
    append_string(file, "u32 __ps2_rc_section_count = %d;\n", loaded_section_count);
    append_string(file, "ps2rc_section_t __ps2_rc_sections[] = {");
    for (u32 i = 0; i < context->code_unit_count; i++) {
        code_unit_t* unit = &context->code_units[i];
        for (u32 s = 0; s < unit->section_header_count; s++) {
            elf_section_header_t* header = &unit->section_headers[s];

            if (header->type == SHT_NULL) {
                continue;
            }

            snprintf(section_name_buffer, sizeof(section_name_buffer), "%s_%s", unit->name, unit->section_names[s]);
            append_string(file, "\t(ps2rc_section_t) {\n\t\t.name=\"%s\",\n\t\t.address=0x%x,\n\t\t.length = 0x%x,\n\t\t.data=(u8[]){", section_name_buffer, header->address, header->size);

            for (u32 d = 0; d < header->size; d++) {
                append_string(file, "0x%x", ((u8*)unit->data)[header->offset + d]);
                if (d < header->size - 1) {
                    append_string(file, ", ");
                }
            }

            append_string(file, "\t\t}\n\t},\n");
        }
    }
    append_string(file, "};");

    fclose(file);
}

void write_functions(rc_context_t* context, const char* output_path) {
    // Open file
    char path_buffer[0x200];
    snprintf(path_buffer, 0x200, "%s/functions.c", output_path);
    FILE* file = fopen(path_buffer, "w");
    ASSERT(file, "Failed to open file at %s", path_buffer);

    // Write unit info
    append_string(file, "const u32 numUnitPaths = %d;\n", context->code_unit_count);
    append_string(file, "\n\tconst char* pvt_paths[] = {\n");
    for (u32 i = 0; i < context->code_unit_count; i++) {
        append_string(file, "\t\t\"%s\",\n", context->code_units[i].file_path);
    }
    append_string(file, "\t};\nconst char** GetUnitPaths() { return (const char**)pvt_paths; }\n\n");

    // Headers
    append_string(file, "#include \"rc_functions.h\"\n");

    // Write function lookup table
    append_string(file, "void* FunctionLookup(u32 addr) {\n\tswitch (addr) {\n\t\tdefault:\n\t\t\tprintf(\"Failed to get function at address 0x\%x\", addr);\n\t\t\tbreak;\n");
    for (u32 i = 0; i < context->code_unit_count; i++) {
        code_unit_t* unit = &context->code_units[i];

        for (u32 section = 0; section < unit->section_header_count; section++) {
            const elf_section_header_t* header = &unit->section_headers[section];
            // TODO: Dont skip dynamically linked functions
            if (header->address == 0x0) {
                continue;
            }

            const char* section_name = unit->section_names[section];
            if (strcmp(section_name, ".text") != 0) {
                continue;
            }

            elf_section_type_t section_type = elf_section_type_from_string(section_name);

            for (u32 s = 0; s < unit->split_count; s++) {
                const split_t split = unit->splits[s];

                // HACK: Should just manage duplicates instead of blocking
                if (split.generated) {
                    continue;
                }

                if (split.section != section_type || 
                        split.section != ELF_SECTION_TYPE_TEXT ||
                        split.start_address == 0x0 || 
                        split.signature.is_lib_c) {
                    continue;
                }

                append_string(file, "\t\t\tcase 0x%x: return %s;\n", header->address + split.start_address, split.signature.name);
            }
        }
    }
    append_string(file, "\t}\n\treturn NULL;\n}\n");

    // Jalr
    append_string(file, "\nvoid jalr(u32 address)\n");
    append_string(file, "{\n");
    append_string(file, "\tswitch(address)\n");
    append_string(file, "\t{\n");

    for (u32 i = 0; i < context->code_unit_count; i++) {
        code_unit_t* unit = &context->code_units[i];

        for (u32 section = 0; section < unit->section_header_count; section++) {
            const elf_section_header_t* header = &unit->section_headers[section];
            // TODO: Dont skip dynamically linked functions
            if (header->address == 0x0) {
                continue;
            }

            const char* section_name = unit->section_names[section];
            if (strcmp(section_name, ".text") != 0) {
                continue;
            }
            elf_section_type_t section_type = elf_section_type_from_string(section_name);

            append_string(file, "\t// ======================================= //\n");
            append_string(file, "\t// %s\n", unit->name);
            append_string(file, "\t// ======================================= //\n");

            for (u32 s = 0; s < unit->split_count; s++) {
                const split_t split = unit->splits[s];

                // HACK: Should just manage duplicates instead of blocking
                if (split.generated) {
                    continue;
                }
                if (split.section != section_type || 
                        split.section != ELF_SECTION_TYPE_TEXT ||
                        split.start_address == 0x0 || 
                        split.signature.is_lib_c) {
                    continue;
                }

                append_string(file, "\t\t case 0x%x:\n", split.start_address + header->address);
                append_string(file, "\t\t\t%s(", split.signature.name);
                for (int i = 0; i < split.signature.function_parameter_count; i++) {
                    const function_parameter_t param = split.signature.function_parameters[i];
                    append_string(file, "(%s)", param.type);

                    b8 is_ptr = false;
                    for (u32 c = 0; c < strlen(param.type); c++) {
                        if (param.type[c] == '*') {
                            is_ptr = true;
                            break;
                        }
                    }

                    if (is_ptr) {
                        append_string(file, "FindRam(ctx->%s.U64[0])", ee_register_names[EE_REGISTER_A0 + i]);
                    } else {
                        append_string(file, "ctx->%s.U64[0]", ee_register_names[EE_REGISTER_A0 + i]);
                    }

                    if (i < split.signature.function_parameter_count - 1) {
                        append_string(file, ", ");
                    }
                }

                append_string(file, ");\n");
                append_string(file, "\t\t\treturn;\n");
            }
        }
    }

    append_string(file, "\t}\n");
    append_string(file, "\tprintf(\"Failed to find function with split at %p\\n\", address);\n");
    append_string(file, "}\n");
    append_string(file, "\n\n");

    fclose(file);
}

void try_make_directory(const char* path) {
    DIR* dir = opendir(path);
    if (dir) {
        closedir(dir);
    } else if (ENOENT == errno) {
        ASSERT(mkdir(path, 0777) == 0, "Failed to create directory '%s'", path);
    } else {
        log_error("Failed to create directory.");
    }
}

void process_section(rc_context_t* context, code_unit_t* unit, u32 section_index, const char* output_path) {
    // Get header info
    ASSERT(unit->section_headers, "Code unit missing section headers");

    elf_section_header_t* header = &unit->section_headers[section_index];
    if (header->type == SHT_NULL) {
        return;
    }

    // Get header name and type
    const char* section_name = unit->section_names[section_index];
    elf_section_type_t section_type = elf_section_type_from_string(section_name);

    if (section_type == ELF_SECTION_TYPE_NULL) {
        log_error("Failed to get section type from string '%s'", section_name);
        return;
    }

    // TODO: Add support for sections other than the .text section
    if (section_type != ELF_SECTION_TYPE_TEXT) {
        return;
    }

    if (strcmp(section_name, ".text") != 0) {
        return;
    }

    // Open output file
    log_debug("Processing unit section: %d", unit->split_count);

    // Go through each split
    for (u32 i = 0; i < unit->split_count; i++) {
        const split_t split = unit->splits[i];

        if (split.signature.is_lib_c || 
                !split.signature.generate || 
                !header->offset) {
            continue;
        }

        if (split.section != section_type) {
            // log_debug("section mismatch: %d != %d", split.section, section_type);
            continue;
        }

        cemitter_process_split(context, header, split, unit, header, output_path);
    }

}

void process_code_unit(rc_context_t* context, code_unit_t* unit, const char* output_path) {
    try_make_directory(output_path);

    // Open unit source file
    FILE* file = fopen(unit->file_path, "r");
    ASSERT(file, "Cannot open file at path '%s' for code unit '%s'", unit->file_path, unit->name);

    // Get length
    fseek(file, 0, SEEK_END);
    u32 len = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Get data
    void* data = malloc(len);
    fread(data, len, 1, file);

    unit->data = data;

    if (strcmp(unit->type, "elf") == 0) {
        elf_header_t* header = data;
        if (header->identification.magic[0] != ELFMAG0 || 
                header->identification.magic[1] != ELFMAG1 || 
                header->identification.magic[2] != ELFMAG2 ||
                header->identification.magic[3] != ELFMAG3) {
            log_error("File is not an elf file.");
            return;
        }

        unit->section_headers = data + header->section_header_offset;
        unit->section_header_count = header->section_header_num;
        unit->string_table = data + unit->section_headers[header->string_section_index].offset;

        unit->string_table = data + unit->section_headers[header->string_section_index].offset;
        unit->section_names = malloc(sizeof(const char*) * header->section_header_num);
        for (u32 i = 0; i < header->section_header_num; i++) {
            unit->section_names[i] = unit->string_table + unit->section_headers[i].name_index;
        }

        for (u32 i = 0; i < header->section_header_num; i++) {
            process_section(context, unit, i, output_path);
        }
    } else if (strcmp(unit->type, "xff") == 0) {
        xff_header_t* header = data;

        // Convert section headers
        xff_section_header_t* headers = data + header->section_header_offset;
        unit->section_header_count = header->section_count;
        unit->string_table = header->string_table_offset + data;

        unit->section_names = malloc(sizeof(const char*) * header->section_count);
        for (u32 i = 0; i < header->section_count; i++) {
            unit->section_names[i] = data + ((u32*)(data + header->section_name_offsets))[i] + header->section_string_table_ptr;
        }

        unit->section_headers = malloc(sizeof(elf_section_header_t) * header->section_count);
        for (u32 i = 0; i < header->section_count; i++) {
            unit->section_headers[i] = xff_section_header_to_elf(unit, data, &headers[i], i);
        }

        // Load relocations
        u32 total_relocation_count = 0;
        xff_relocation_header_t* reloc_headers = data + header->relocation_header_offset;
        for (u32 i = 0; i < header->relocation_header_count; i++) {
            total_relocation_count += reloc_headers[i].relocation_count;
        }

        unit->relocations = malloc(sizeof(relocation_t) * total_relocation_count);

        // TODO: Memcpy instead of this
        for (u32 i = 0; i < header->relocation_header_count; i++) {
            relocation_t* relocations = data + reloc_headers[i].file_offset;
            for (u32 r = 0; r < reloc_headers[i].relocation_count; r++) {
                unit->relocations[unit->relocation_count++] = relocations[r];
            }
        }

        // Load symbols
        unit->symbol_count = header->symbol_count;
        unit->symbols = data + header->symbol_file_offset;

        // Add found splits
        // u32 found_split_count = 0;
        // split_t* found_splits = xff_find_splits(unit, data, &found_split_count);
        //
        // split_t* all_splits = malloc(sizeof(split_t) * (unit->split_count + found_split_count));
        // memcpy(all_splits, unit->splits, unit->split_count * sizeof(split_t));
        // memcpy(all_splits + unit->split_count, found_splits, found_split_count * sizeof(split_t));
        //
        // unit->splits = all_splits;
        // log_debug("had %d splits", unit->split_count);
        // log_debug("found %d splits", found_split_count);
        // unit->split_count += found_split_count;
        // log_debug("now have %d splits", unit->split_count);

        for (u32 i = 0; i < header->section_count; i++) {
            log_info("Processing xff section %d", i);
            process_section(context, unit, i, output_path);
        }
        log_debug("Found xff: %p", header); 

    } else {
        log_error("Undefined code unit type '%s'", unit->type);
    }
}

void append_string(FILE* file, const char* format, ...);

void write_function_list(rc_context_t* context, const char* output_path) {
    char path_buffer[0x200];
    snprintf(path_buffer, 0x200, "%s/rc_functions.h", output_path);
    FILE* file = fopen(path_buffer, "w");
    ASSERT(file, "Failed to open file at %s", path_buffer);

    for (u32 u = 0; u < context->code_unit_count; u++) {
        const code_unit_t unit = context->code_units[u];

        for (u32 i = 0; i < unit.split_count; i++) {
            const split_t split = unit.splits[i];

            // HACK: Should just manage duplicates instead of blocking
            // if (split.generated) {
            //     continue;
            // }

            // // HACK: Need a better way to jump from register.
            // // Relocated functions will not work.
            // if (split.start_address == 0x0) {
            //     continue;
            // }

            if (split.section != ELF_SECTION_TYPE_TEXT || split.signature.is_lib_c) {
                continue;
            }

            append_string(file, "%s %s(", split.signature.return_type, split.signature.name);

            for (u32 p = 0; p < split.signature.function_parameter_count; p++) {
                const function_parameter_t param = split.signature.function_parameters[p];
                append_string(file, "%s %s", param.type, param.name);

                if (p < split.signature.function_parameter_count - 1) {
                    append_string(file, ", ");
                }
            }

            if (split.varg) {
                append_string(file, ", ...");
            }

            append_string(file, ");\n");
        }
    }

    fclose(file);
}
