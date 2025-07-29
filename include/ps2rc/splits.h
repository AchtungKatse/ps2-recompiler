#pragma once

/* 
- FunctionSignature:
    ReturnType: undefined
    Name: DecodeSection
    Generate: true
    Parameters:
      - Type: void*
        Name: sectionData
        Register: a0
      - Type: void*
        Name: funcA
        Register: a1
      - Type: void*
        Name: funcB
        Register: a2
      - Type: void*
        Name: printFunction
        Register: a3
  StartAddress: 632
  EndAddress: 1208
  */
#include "ps2rc/defines.h"
#include "ps2rc/elf/elf.h"
#include "ps2rc/ps2_registers.h"

typedef struct function_parameter {
    const char* type; 
    const char* name;
    ee_register_t ee_register;
} function_parameter_t;

typedef struct function_signature {
    const char* name;
    const char* return_type;
    function_parameter_t function_parameters[16];
    u8 function_parameter_count;
    b8 generate;
    b8 is_lib_c;
} function_signature_t;

typedef struct split_patch {
    u32 line_index;
    const char* value;
} split_patch_t;

typedef struct rc_split {
    function_signature_t signature;
    split_patch_t patches[32];
    u32 patch_count;
    u32 start_address;
    u32 end_address;
    elf_section_type_t section;
    b8 varg;
    b8 generated;
} split_t;

split_t* split_read_from_yaml(const char* filepath, u32* out_split_count);
