#include "ps2rc/cemitter.h"
#include "ps2rc/code_unit.h"
#include "common/logging.h"
#include "ps2rc/elf/elf.h"
#include "ps2rc/ps2_registers.h"
#include "ps2rc/rc_context.h"
#include "ps2rc/splits.h"

#include "instructions/RabbitizerInstrDescriptor.h"
#include "instructions/RabbitizerInstruction.h"
#include "instructions/RabbitizerInstructionR5900.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern u32 failed_jumps_count;
extern u32 failed_jumps[1000];
const char* section_name_to_index(const char* name);
void parse_instruction_args(char* arg_buffer, u32 arg_buffer_size, const char* opcode, const char* base, u32 address);
void split_to_function_call(const split_t split, char* buffer, u32 buffer_size);
void append_string(FILE* file, const char* format, ...);
void process_instruction(const rc_context_t* context, 
        const code_unit_t* unit,
        const elf_section_header_t* section, 
        const split_t split, 
        char* output, 
        u32 output_buffer_size, 
        const u32* instructions, 
        s32* index, 
        const u32 address,
        u32 relocation_start,
        u32 relocation_end);
b8 find_jump_to_function(char* output, 
        u32 output_buffer_size, 
        const rc_context_t* context, 
        const code_unit_t* unit, 
        const elf_section_header_t* section, 
        const split_t split, 
        const u32* instructions, 
        u32 jump_address, 
        u32 address, 
        s32* index);
split_t find_static_address(const rc_context_t* context, const elf_symbol_t symbol);
void append_split_prototype(FILE* file, const split_t split);

void cemitter_process_split(rc_context_t* context, elf_section_header_t* section, split_t split, code_unit_t* unit, elf_section_header_t* header, const char* output_path) {
    if (split.signature.is_lib_c || !split.signature.generate) {
         return;
    }
    if (split.section != ELF_SECTION_TYPE_TEXT) { 
        return;
    }

    // Open file
    char out_path_buffer[0x200];
    snprintf(out_path_buffer, 0x200, "%s/%s.c", output_path, split.signature.name);
    FILE* file = fopen(out_path_buffer, "wb");

    // Write includes
    append_string(file, "#include \"ps2ctx/ps2_context.h\"\n");
    append_string(file, "#include \"rc_functions.h\"\n");
    append_string(file, "#include \"sections.h\"\n");
    append_string(file, "#include \"ps2ctx/asm_macros.h\"\n\n");

    // Get instructiosn
    const u32 instruction_count = (split.end_address - split.start_address) / 4;
    u32* instructions = unit->data + header->offset + split.start_address;

    if (strcmp(split.signature.name, "main") == 0) {
        append_string(file, "void RecompilationInit();\n");
    }

    // Write signature
    append_string(file, "%s %s(", split.signature.return_type, split.signature.name);
    for (u32 i = 0; i < split.signature.function_parameter_count; i++) {
        const function_parameter_t parameter = split.signature.function_parameters[i];
        append_string(file, "%s %s", parameter.type, parameter.name);
        if (i < split.signature.function_parameter_count - 1) {
            append_string(file, ", ");
        }
    }
    if (split.varg) {
        append_string(file, ", ...");
    }
    append_string(file, ") {\n");

    // Check for main and inject new entry point
    if (strcmp(split.signature.name, "main") == 0) {
        append_string(file, "\tRecompilationInit();\n");
        append_string(file, "\targc--;\n"); // Added because the ps2 doesnt expect the first argument to be the executable path
    }

    // Set the registers to parameters
    for (u32 i = 0; i < split.signature.function_parameter_count; i++) {
        // Probably going to use the incorrect registers
        const function_parameter_t param = split.signature.function_parameters[i];
        append_string(file, "\tctx->ee_registers[%d].U32[0] = (u32)%s;\n", EE_REGISTER_A0 + i, param.name);
    }

    // Identify branches
    const u32 MAX_BRANCHES = 0x100;
    u32 branch_capacity = MAX_BRANCHES;
    s32* branch_indices = malloc(sizeof(u32) * MAX_BRANCHES);
    u32 branch_count = 0;

    for (s32 i = 0; i < instruction_count; i++) {
        char buffer[255];
        u32 vram = split.start_address + i * 4;

        RabbitizerInstruction instr;
        RabbitizerInstructionR5900_init(&instr, instructions[i], vram);
        RabbitizerInstructionR5900_processUniqueId(&instr);
        RabbitizerInstruction_disassemble(&instr, buffer, NULL, 0, 0);

        if (!RabbitizerInstrDescriptor_isBranch(instr.descriptor)) {
            continue;
        }

        s32 offset = RabbitizerInstruction_getGenericBranchOffset(&instr, vram);
        s32 instruction_index = i + offset / 4;

        b8 branch_exists = false;
        for (u32 j = 0; j < branch_count; j++) {
            if (branch_indices[j] == instruction_index) {
                branch_exists = true;
                break;
            }
        }

        if (branch_exists) {
            continue;
        }

        if (branch_count >= branch_capacity) {
            s32* tmp = malloc(sizeof(u32) * branch_capacity * 2);
            memcpy(tmp, branch_indices, sizeof(u32) * branch_capacity);
            free(branch_indices);
            branch_indices = tmp;
            branch_capacity *= 2;
        }

        branch_indices[branch_count++] = instruction_index;
    }

    // Write unmodified instructions for reference
    u32 relocation_start = 0;
    u32 relocation_end = unit->relocation_count;
    // for (u32 r = 0; r < unit->relocation_count; r++) {
    //     const relocation_t relocation = unit->relocations[r];
    //     if (relocation.offset < split.start_address) {
    //         continue;
    //     }
    //
    //     relocation_start = r;
    //     break;
    // }
    // for (u32 r = relocation_start; r < unit->relocation_count; r++) {
    //     const relocation_t relocation = unit->relocations[r];
    //     if (relocation.offset > split.start_address) {
    //         continue;
    //     }
    //
    //     relocation_end = r;
    //     break;
    // }
    //

    for (u32 i = 0; i < instruction_count; i++) {
        u32 vram = split.start_address + i * 4;

        char instr_buffer[0x40];
        RabbitizerInstruction instr;
        RabbitizerInstructionR5900_init(&instr, instructions[i], vram);
        RabbitizerInstructionR5900_processUniqueId(&instr);
        RabbitizerInstruction_disassemble(&instr, instr_buffer, NULL, 0, 0);

        // Apply relocations to instructions
        u32 relocation_index = -1;
        u32 symbol_index = -1;
        elf_symbol_t symbol = {};

        u32 address = split.start_address + i * 4;
        for (u32 r = relocation_start; r < relocation_end; r++) {
            const relocation_t relocation = unit->relocations[r];

            if (relocation.offset != address) {
                continue;
            }

            const relocation_type_t type = relocation.packed_symbol_index;
            const u32 symbol_index = relocation.packed_symbol_index >> 8;
            symbol = unit->symbols[symbol_index];
            switch (type) {
                case RELOCATION_TYPE_NONE:
                    log_warn("Cannot apply relocation type 'None'");
                    break;
                case RELOCATION_TYPE_16:
                    instructions[i] |= symbol.value & 0xFFFF;
                    break;
                case RELOCATION_TYPE_32:
                    instructions[i] = symbol.value;
                    break;
                case RELOCATION_TYPE_REL32:
                case RELOCATION_TYPE_26:
                    instructions[i] += symbol.value;
                    break;
                case RELOCATION_TYPE_HIGH16:
                    instructions[i] |= (symbol.value >> 16) & 0xFFFF;
                    break;
                case RELOCATION_TYPE_LOW16:
                    instructions[i] |= symbol.value & 0xFFFF;
                    break;
                case RELOCAITION_TYPE_GPREL16:
                case RELOCATION_TYPE_LITERAL:
                case RELOCATION_TYPE_GOT16:
                case RELOCATION_TYPE_PC16:
                case RELOCATION_TYPE_CALL16:
                case RELOCATION_TYPE_GPREL32:
                    break;
            }

            relocation_index = r;
            break;
        }

        append_string(file, "\t/* %5d%s%4d */ /* %s ", i, relocation_index != -1 ? "r" : " ", relocation_index, instr_buffer);
        if (relocation_index != -1) {
            append_string(file, " (Symbol: %s, Address: 0x%x, Section: %s)", unit->string_table + symbol.name, symbol.value, symbol.section_header_index != 0xFFF1 ? unit->section_names[symbol.section_header_index] : "static");
        }
        append_string(file, "*/ \n");
    }

    // Write instructions
    for (s32 i = 0; i < instruction_count; i++) {
        // Check for branches
        for (u32 j = 0; j < branch_count; j++) {
            if (branch_indices[j] == i) {
                append_string(file, "branch_%d:\n", i);
                break;
            }
        }

        // Check for patches
        b8 found_patch = false;
        for (u32 j = 0; j < split.patch_count; j++) {
            if (split.patches[j].line_index == i) {
                if (split.patches[j].value == NULL) {
                    log_error("Failed to get value of patch. %s (%d: %s)", split.signature.name, split.patches[j].line_index, split.patches[j].value);
                    continue;
                }
                log_debug("Applied patch %s", split.patches[j].value);
                append_string(file, "\t/* %5d */ %s; /* patched */\n", i, split.patches[j].value);
                found_patch = true;
                break;
            }
        }

        if (found_patch) {
            continue;
        }

        // Check for relocations
        u32 vram = split.start_address + i * 4;

        char instr_buffer[0x40];
        RabbitizerInstruction instr;
        RabbitizerInstructionR5900_init(&instr, instructions[i], vram);
        RabbitizerInstructionR5900_processUniqueId(&instr);
        RabbitizerInstruction_disassemble(&instr, instr_buffer, NULL, 0, 0);

        char output[0x800] = {};
        process_instruction(context, unit, section, split, output, 0x400, instructions, &i, vram, relocation_start, relocation_end);
        u32 pad_length = 75 - strlen(output);
        if (pad_length < 0) {
            pad_length = 0;
        }

        append_string(file, "\t/* %5d */ %s; %*c /* %s */ \n", i, output, pad_length, ' ', instr_buffer);
    }

    append_string(file, "}\n");

    fclose(file);
}

void append_string(FILE* file, const char* format, ...) {
    char buffer[0x1000];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    fwrite(buffer, strlen(buffer), 1, file);
}

void process_instruction(const rc_context_t* context, 
        const code_unit_t* unit,
        const elf_section_header_t* section, 
        const split_t split, 
        char* output, 
        u32 output_buffer_size, 
        const u32* instructions, 
        s32* index, 
        const u32 address,
        u32 relocation_start,
        u32 relocation_end) {
    // Read instructions
    RabbitizerInstruction instr;
    RabbitizerInstructionR5900_init(&instr, instructions[*index], address);
    RabbitizerInstructionR5900_processUniqueId(&instr);

    // Check if its a branch and if so replace the address to a goto branch
    if (RabbitizerInstrDescriptor_isBranch(instr.descriptor)) {
        s32 offset = RabbitizerInstruction_getGenericBranchOffset(&instr, address);
        s32 branch_index = *index + offset / 4;

        char branch_string_buffer[0x100];
        snprintf(branch_string_buffer, 0x100, "branch_%d", branch_index);
        RabbitizerInstruction_disassemble(&instr, output, branch_string_buffer, strlen(branch_string_buffer), 0);
    }
    else {
        RabbitizerInstruction_disassemble(&instr, output, NULL, 0, 0);
    }

    // Build the remainder of the macro
    const char* _opcode = RabbitizerInstrId_getOpcodeName(instr.uniqueId);
    char opcode[64] = {};
    strcpy(opcode, _opcode);

    // Remove '.' from the opcode
    u32 opcode_len = strlen(opcode);
    for (u32 i = 0; i < opcode_len; i++) {
        if (opcode[i] == '.') {
            for (u32 j = i; j < opcode_len; j++) {
                opcode[j] = opcode[j + 1];
            }
            i--;
            opcode_len--;
        }
    }

    // HACK: specific opcodes can have overloading, this makes them cooperate with the recompiler
    u32 arg_count = 0;
    for (u32 i = 0; i < strlen(output); i++) {
        if (output[i] == ',') {
            arg_count++;
        }
    }
    if (strlen(output) > 0xC) {
        arg_count++;
    }

    if (strcmp(opcode, "mult") == 0 || strcmp(opcode, "multu") == 0) {
        if (arg_count == 3) {
            strcat(opcode, "_return");
        } else {
            strcat(opcode, "_short");
        }
    }

    if (strcmp(opcode, "syscall") == 0) {
        strncpy(output, "EESyscall(ctx->v1.U8[0]);", output_buffer_size);
        return;
    }

    if (strcmp(opcode, "jalr") == 0) {
        *index += 1;
        char next_instr_buffer[0x100];
        process_instruction(context, unit, section, split, next_instr_buffer, 0x100, instructions, index, address, relocation_start, relocation_end);
        snprintf(output, output_buffer_size, "__mips_asm__jalr(ctx->%s.U32[0], %s); // 0x%x", ee_register_names[(instructions[*index - 1] >> 21) & 0x1f], next_instr_buffer, instructions[*index - 1]);
        return;
    }

    // Check if it is a function call and return if so
    if (RabbitizerInstrDescriptor_isJump(instr.descriptor) && !RabbitizerInstrDescriptor_isJumpWithAddress(instr.descriptor)) {
        *index += 1;
        char next_instr_buffer[0x100];
        process_instruction(context, unit, section, split, next_instr_buffer, 0x100, instructions, index, address + 4, relocation_start, relocation_end);
        if (strcmp(split.signature.return_type, "void") == 0) {
            snprintf(output, output_buffer_size, "__mips_asm_ret(%s)", next_instr_buffer);
            return;
        } else {
            // TODO: May be wrong return register
            snprintf(output, output_buffer_size, "__mips_asm_ret_value(%s, (%s)(ctx->%s.U32[0]))", next_instr_buffer, split.signature.return_type, ee_register_names[EE_REGISTER_V0]);
            return;
        }
    }

    if (RabbitizerInstrDescriptor_isJumpWithAddress(instr.descriptor)) {
        u32 jump_address = RabbitizerInstruction_getInstrIndexAsVram(&instr);
        if (find_jump_to_function(output, output_buffer_size, context, unit, section, split, instructions, jump_address, address, index)) {
            return;
        }
    }

    // Check if there are arguments and parse them
    char new_opcode[64];
    for (int i = 0, j = 0; i < strlen(output); i++) {
        char c = opcode[i];
        if (c == 0)
            break;

        if (c == '.')
            continue;
        new_opcode[j++] = c;
    }

    if (strlen(output) < 0xC) { // Rabbitizer puts the args after 0xc bytes
        snprintf(output, output_buffer_size, "__mips_asm_%s", opcode); // If there are none, just send the instruction
        return;
    }

    char arg_buffer[0x2000] = {};
    parse_instruction_args(arg_buffer, sizeof(arg_buffer), opcode, output + 0xC, address);

    // Check if its a branch instruction and give it the next instruction on branch if so
    ASSERT(opcode != NULL, "opcode is null.");
    ASSERT(arg_buffer != NULL, "Arg buffer is null.");
    if (RabbitizerInstrDescriptor_isBranch(instr.descriptor)) {
        *index += 1;
        char next_instr_buffer[0x100];
        process_instruction(context, unit, section, split, next_instr_buffer, 0x100, instructions, index, address + 4, relocation_start, relocation_end);

        ASSERT(opcode != NULL, "next instruction is null.");
        snprintf(output, output_buffer_size, "__mips_asm_%s(%s, %s, %d)", opcode, arg_buffer, next_instr_buffer, *index);
        return;
    }

    // Check if dest register needs to have a data relocation injected.
    relocation_t* relocation = NULL;
    for (u32 i = relocation_start; i < relocation_end; i++) {
        if (unit->relocations[i].offset == address) {
            relocation = &unit->relocations[i];
            break;
        }
    }

    snprintf(output, output_buffer_size, "__mips_asm_%s(%s)", opcode, arg_buffer);
    RabbitizerInstruction_destroy(&instr);

    if (relocation) {
        elf_symbol_t symbol = unit->symbols[relocation->packed_symbol_index >> 8];
        if (symbol.section_header_index == 0xFFF1) {
            return;
        }

        if ((relocation->packed_symbol_index & 0xFF) != RELOCATION_TYPE_HIGH16) {
            return;
        }

        char dest_reg[32];
        strcpy(dest_reg, arg_buffer);
        for (u32 i = 0; i < strlen(dest_reg); i++) {
            const char c = dest_reg[i];
            if (c == ' ' || c == ',') {
                dest_reg[i] = 0;
                break;
            }
        }

        snprintf(output, 
                output_buffer_size, 
                "__mips_asm_%s(%s); ctx->ee_registers[%s].U32[0] = (u32)(__ps2_rc_sections[%s_%s_section_index].data + 0x%x)", 
                opcode, 
                arg_buffer, 
                dest_reg, 
                unit->name, 
                section_name_to_index(unit->section_names[symbol.section_header_index]),
                symbol.value + ((instructions[*index] & 0xFFFF) << 0x10));
        return;
    } 

    // output += ")";
}

void split_to_function_call(const split_t split, char* buffer, u32 buffer_size) {
    u32 offset = 0;
    if (strcmp(split.signature.return_type, "void") != 0) {
        offset += snprintf(buffer + offset, buffer_size - offset, "ctx->v0.U32[0] = (u32)");
    }

    ASSERT(split.signature.name != NULL, "Split signature name is null.");
    offset += snprintf(buffer + offset, buffer_size - offset, "%s(", split.signature.name);
    for (u32 i = 0; i < split.signature.function_parameter_count; i++) {
        const function_parameter_t parameter = split.signature.function_parameters[i];

        b8 is_ptr = false;
        for (u32 c = 0; c < strlen(parameter.type); c++) {
            if (parameter.type[c] == '*') {
                is_ptr = true;
                break;
            }
        }

        ASSERT(parameter.type != NULL, "Parameter type is null in split '%s'", split.signature.name);
        if (is_ptr) {
            offset += snprintf(buffer + offset, buffer_size - offset, "(%s)FindRam(ctx->%s.U32[0])", parameter.type, ee_register_names[i + parameter.ee_register]);
        } else {
            offset += snprintf(buffer + offset, buffer_size - offset, "(%s)ctx->%s.U32[0]", parameter.type, ee_register_names[i + parameter.ee_register]);
        }

        if (i < split.signature.function_parameter_count - 1) {
            offset += snprintf(buffer + offset, buffer_size - offset, ", ");
        }
    }
    offset += snprintf(buffer + offset, buffer_size - offset, ")");
}

void parse_instruction_args(char* arg_buffer, u32 arg_buffer_size, const char* opcode, const char* base, u32 address) {
    int offsetsToRemove = 0;

    // Arg collection
    char num_buffer[64];

    for (int i = 0; i < strlen(base); i++) {
        char c = base[i];

        // Check if uppercase (hopefully a COP0 register)
        if (c >= 'A' && c <= 'Z') {
            b8 foundCop0 = false;
            for (int j = 0; j < cop0_register_names_count; j++) {
                if (strncmp(base + i, cop0_register_names[j], strlen(cop0_register_names[j])) == 0) {
                    strcat(arg_buffer, "ctx->");
                    strcat(arg_buffer, cop0_register_names[j]);
                    i += strlen(cop0_register_names[j]);
                    foundCop0 = true;
                    break;
                }
            }

            if (foundCop0)
                continue;
        }

        switch (c) {
            case ' ':
                break;
            case '$':
                strcat(arg_buffer, "EE_");
                break;
            case '(':
                if (base[i + 1] == '$') {
                    // This means that there is a 0xOffset(register) thing
                    offsetsToRemove++;
                    strcat(arg_buffer, ", ");
                } else { 
                    arg_buffer[strlen(arg_buffer)] = c;
                }
                break;
            case ')':
                if (offsetsToRemove > 0) {
                    offsetsToRemove--;
                    break;
                }
                else {
                    arg_buffer[strlen(arg_buffer)] = ')';
                }
                break;
            case '.':
                snprintf(num_buffer, sizeof(num_buffer), "%d", address);
                strcat(arg_buffer, num_buffer);
                break;
            case ',':
                strcat(arg_buffer, ", ");
                break;
            default:
                arg_buffer[strlen(arg_buffer)] += c;
                break;
        }
    }

    // Hacks to make specific opcodes cooperate with rabbitizer
    // if (opcode == "mult" || opcode == "multu") {
    //     if (args[0] == "0") {
    //         args.erase(args.begin());
    //         opcode += "_short";
    //     }
    //
    //     if (args.size() == 3)
    //         opcode += "_return";
    // }
    //
    // string out;
    // out.reserve(source.size() + 15); // 15 = strlen("ctx->") * 3
    // for (int i = 0; i < args.size(); i++) {
    //     out += args[i]; 
    //     if (i < args.size()  - 1)
    //         out += ", ";
    // }
    //
    // return out;
}

b8 find_jump_to_function(char* output, 
        u32 output_buffer_size, 
        const rc_context_t* context, 
        const code_unit_t* unit, 
        const elf_section_header_t* section, 
        const split_t split, 
        const u32* instructions, 
        u32 jump_address, 
        u32 address, 
        s32* index) {

    *index += 1;
    char next_instr_buffer[0x200];
    char function_call[0x200];
    // HACK: Actually use relocation start and end indices
    process_instruction(context, unit, section, split, next_instr_buffer, sizeof(next_instr_buffer), instructions, index, address, 0, unit->relocation_count);

    // Check if in self symbols
    for (u32 j = 0; j < unit->split_count; j++) {
        const split_t split = unit->splits[j];
        bool is_split_address = split.start_address + section->address == jump_address;
        bool is_section_address = split.start_address == jump_address;
        bool is_correct_address = is_split_address || is_section_address;
        bool is_same_section = split.section == ELF_SECTION_TYPE_TEXT;

        if (!is_correct_address || !is_same_section)
            continue;

        split_to_function_call(split, function_call, sizeof(function_call));
        snprintf(output, output_buffer_size, "__mips_asm__jal(%s, %s)", function_call, next_instr_buffer);
        return true;
    }

    // Check for relocation
    for (u32 j = 0; j < unit->relocation_count; j++) {
        relocation_t relocation = unit->relocations[j];


        const relocation_type_t type = relocation.packed_symbol_index & 0xFF;
        const u32 symbol_index = relocation.packed_symbol_index >> 0x8;

        const elf_symbol_t symbol = unit->symbols[symbol_index];

        // TODO: Match section index
        if (relocation.offset != address) {
            continue;
        }

        //  Check if symbol is always at address
        if (symbol.section_header_index == 0xfff1) {
            // TODO: Check every unit for a global address
            snprintf(output, output_buffer_size, "// Jump to constant address 0x%x", symbol.value);
            split_t split = find_static_address(context, symbol);

            split_to_function_call(split, function_call, sizeof(function_call));
            snprintf(output, output_buffer_size, "__mips_asm__jal(%s, %s)", function_call, next_instr_buffer);
            return true;
        }

        elf_section_type_t target_section_type = ELF_SECTION_TYPE_NULL;

        if (symbol.info.type == ELF_SYMBOL_TYPE_SECTION) {
            target_section_type = elf_section_type_from_string(unit->section_names[symbol.section_header_index]);
        }

        const char* symbol_name = unit->string_table + symbol.name;

        // Find the unit with the matching symbol
        // Assuming this is C and the symbol name matches exactly
        for (u32 j = 0; j < unit->split_count; j++) {
            const split_t split = unit->splits[j];

            // If targeting a section, dont look at splits with a different type.
            if (target_section_type != ELF_SECTION_TYPE_NULL && split.section != target_section_type) {
                continue;
            }

            if (strcmp(split.signature.name, symbol_name) == 0 || 
                    split.start_address == jump_address) {
                split_to_function_call(split, function_call, sizeof(function_call));
                snprintf(output, output_buffer_size, "__mips_asm__jal(%s, %s)", function_call, next_instr_buffer);
                return true;
            }
        }
    }

    // Try to find the jump function
    for (u32 c = 0; c < context->code_unit_count; c++) {
        const code_unit_t* next_unit = &context->code_units[c];
        for (u32 j = 0; j < next_unit->split_count; j++) {
            const split_t split = next_unit->splits[j];
            bool is_split_address = split.start_address + section->address == jump_address;
            bool is_section_address = split.start_address == jump_address;
            bool is_correct_address = is_split_address || is_section_address;
            bool is_same_section = split.section == ELF_SECTION_TYPE_TEXT;

            if (!is_correct_address || !is_same_section)
                continue;

            split_to_function_call(split, function_call, sizeof(function_call));
            snprintf(output, output_buffer_size, "__mips_asm__jal(%s, %s)", function_call, next_instr_buffer);
            return true;
        }
    }

    log_error("Split '%s' Failed to find jump to address 0x%x from address 0x%x (%s)", split.signature.name, jump_address, address, output);
    failed_jumps[failed_jumps_count++] = jump_address;
    snprintf(output, output_buffer_size, "// Jump to undefined section: 0x%x", jump_address);
    return false;
}

split_t find_static_address(const rc_context_t* context, const elf_symbol_t symbol) {
    for (u32 i = 0; i < context->code_unit_count; i++) {
        code_unit_t* unit = &context->code_units[i];

        // Search for section
        for (u32 s = 0; s < unit->section_header_count; s++) {
            const elf_section_header_t section = unit->section_headers[s];
            for (u32 j = 0; j < unit->split_count; j++) {
                split_t split = unit->splits[j];
                if (split.start_address + section.address == symbol.value) {
                    return split;
                }
            }
        }
    }

    return (split_t) { };
}

void append_split_prototype(FILE* file, const split_t split) {
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
