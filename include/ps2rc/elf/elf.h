#pragma once

#include "ps2rc/defines.h"
#define EI_NIDENT 16

typedef unsigned int elf_addr;
typedef unsigned short elf_half;
typedef unsigned int elf_off;
typedef int elf_sword;
typedef unsigned int elf_word;
typedef unsigned char u8;

// =====================================
// HEADER
// =====================================

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

typedef enum elf_class : u8 {
    ELF_CLASS_NONE = 0,
    ELF_CLASS_32 = 1,
    ELF_CLASS_64 = 2
} elf_class_t;

typedef enum elf_data : u8 {
    ELF_DATA_NONE = 0,
    ELF_DATA_LSB = 1,
    ELF_DATA_MSB = 2,
} elf_data_t;

typedef struct elf_identification {
    u8 magic[4];
    elf_class_t class;
    elf_data_t data_encoding;
    u8 version;
    u8 padding[9];
} elf_identification_t;

typedef enum elf_type : elf_half {
    ELF_TYPE_NONE   = 0,
    ELF_TYPE_REL    = 1,
    ELF_TYPE_EXEC   = 2,
    ELF_TYPE_DYN    = 3,
    ELF_TYPE_CORE   = 4,
    ELF_TYPE_LOPROC = 0xFF00,
    ELF_TYPE_HIPROC = 0xFFFF,
} elf_type_t;

typedef enum elf_machine : elf_half {
    ELF_MACHINE_NONE        = 0,
    ELF_MACHINE_M32         = 1,
    ELF_MACHINE_SPARC       = 2,
    ELF_MACHINE_386         = 3,
    ELF_MACHINE_68K         = 4,
    ELF_MACHINE_88K         = 5,
    ELF_MACHINE_860         = 7,
    ELF_MACHINE_MIPS        = 8,
    ELF_MACHINE_MIPS_RS4_BE = 9,
    ELF_MACHINE_RESERVED    = 0,
} elf_machine_t;

typedef struct elf_header {
    elf_identification_t identification;
    elf_type_t type;
    elf_half machnie;
    elf_word version;
    elf_addr entry;
    elf_off program_header_offset;
    elf_off section_header_offset;
    elf_word flags;
    elf_half ehsize;
    elf_half program_header_entry_size;
    elf_half program_header_num;
    elf_half section_header_entry_size;
    elf_half section_header_num;
    elf_half string_section_index;

} elf_header_t;

// =====================================
// Section Headers
// =====================================

typedef enum elf_section_header_type : elf_word {
    SHT_NULL     = 0,
    SHT_PROGBTIS = 1,
    SHT_SYMTAB   = 2,
    SHT_STRTAB   = 3,
    SHT_RELA     = 4,
    SHT_HASH     = 5,
    SHT_DYNAMIC  = 6,
    SHT_NOTE     = 7,
    SHT_NOBITS   = 8,
    SHT_REL      = 9,
    SHT_SHLIB    = 10,
    SHT_DYNSYM   = 11,
    SHT_LOPROC   = 0x70000000,
    SHT_HIPROC   = 0x7FFFFFFF,
    SHT_LOUSER   = 0x80000000,
    SHT_HIUSER   = 0xFFFFFFFF,
} elf_section_header_type_t;

typedef enum elf_section_header_flags : elf_word {
    SHF_NULL = 0,
    SHF_WRITE = 1,
    SHF_ALLOC = 2,
    SHF_EXECINSTR = 0x4,
    SHF_MASKPROC = 0xF000000,
} elf_section_header_flags_t;

typedef struct elf_section_header {
    /**
     * @brief Index into the section header string table
     */
    elf_word name_index;
    elf_section_header_type_t type;
    elf_section_header_flags_t flags;
    elf_addr address;
    elf_off offset;
    elf_word size;
    elf_word link;
    elf_word info;
    elf_word alignment;
    elf_word entry_size;
} elf_section_header_t;

typedef enum elf_section_type : u8 {
    ELF_SECTION_TYPE_NULL = 0,
    ELF_SECTION_TYPE_BSS,
    ELF_SECTION_TYPE_COMMENT,
    ELF_SECTION_TYPE_DATA,
    ELF_SECTION_TYPE_DATA1,
    ELF_SECTION_TYPE_DEBUG,
    ELF_SECTION_TYPE_DYNAMIC,
    ELF_SECTION_TYPE_HASH,
    ELF_SECTION_TYPE_LINE,
    ELF_SECTION_TYPE_NOTE,
    ELF_SECTION_TYPE_RODATA,
    ELF_SECTION_TYPE_RODATA1,
    ELF_SECTION_TYPE_SHSTRTAB,
    ELF_SECTION_TYPE_STRTAB,
    ELF_SECTION_TYPE_SYMTAB,
    ELF_SECTION_TYPE_TEXT,
    ELF_SECTION_TYPE_TEXT_NOP,
    ELF_SECTION_TYPE_REGINFO,
    ELF_SECTION_TYPE_CTORS,
    ELF_SECTION_TYPE_DTORS,
    ELF_SECTION_TYPE_EH_FRAME,
    ELF_SECTION_TYPE_LIT4,
    ELF_SECTION_TYPE_DATA_NOP,
    ELF_SECTION_TYPE_SDATA,
    ELF_SECTION_TYPE_SBSS,
    ELF_SECTION_TYPE_MDEBUG_EABI64,
    ELF_SECTION_TYPE_RELTEXT,
    ELF_SECTION_TYPE_RELSTRTAB,
    ELF_SECTION_TYPE_RELRODATA,
    ELF_SECTION_TYPE_RELDATA,
} elf_section_type_t;

elf_section_type_t elf_section_type_from_string(const char* string);
elf_section_header_type_t elf_section_header_type_from_section_type(elf_section_type_t type); // This name is awful

// =====================================
// Section Headers
// =====================================
#define elf_symbol_bind(type) ((type)>>4)
#define elf_symbol_type(type) ((type)&0xf)

typedef enum elf_symbol_type : u8 {
    ELF_SYMBOL_TYPE_NOTYPE  = 0,
    ELF_SYMBOL_TYPE_OBJECT  = 1,
    ELF_SYMBOL_TYPE_FUNC    = 2,
    ELF_SYMBOL_TYPE_SECTION = 3,
    ELF_SYMBOL_TYPE_FILE    = 4,
    ELF_SYMBOL_TYPE_LOPROC  = 13,
    ELF_SYMBOL_TYPE_HIPROC  = 15,
} elf_symbol_type_t;

typedef enum elf_symbol_binding : u8 {
    ELF_SYMBOL_BINDING_LOCAL  = 0,
    ELF_SYMBOL_BINDING_GLOBAL = 1,
    ELF_SYMBOL_BINDING_WEAK   = 2,
    ELF_SYMBOL_BINDING_LOPROC = 13,
    ELF_SYMBOL_BINDING_HIPROC = 15,
} elf_symbol_binding_t;

typedef struct elf_symbol_info {
    elf_symbol_type_t type : 4;
    elf_symbol_binding_t binding : 4;
} elf_symbol_info_t;

typedef struct elf_symbol {
    elf_word name;
    elf_addr value;
    elf_word size;
    elf_symbol_info_t info;
    u8 other;
    elf_half section_header_index;
} elf_symbol_t;

// =====================================
// Relocations
// =====================================
#define ELF_REL_SYM(info) ((info)>>8)
#define ELF_REL_TYPE(info) ((u8)info)

typedef struct elf_rel {
    elf_addr offset;
    elf_word info;
} elf_rel_t;

typedef struct elf_rel_a {
    elf_addr offset;
    elf_word info;
    elf_sword addend;
} elf_rel_a_t;

// =====================================
// Program Headers
// =====================================

typedef struct elf_program_header {
    elf_word type;
    elf_off offset;
    elf_addr vaddr;
    elf_addr paddr;
    elf_word file_size;
    elf_word mem_size;
    elf_word flags;
    elf_word alignment;
} elf_program_header_t;

// Relocations
typedef enum relocation_type : u8 {
    RELOCATION_TYPE_NONE = 0x0,
    RELOCATION_TYPE_16 = 0x1,
    RELOCATION_TYPE_32 = 0x2,
    RELOCATION_TYPE_REL32 = 0x3,
    RELOCATION_TYPE_26 = 0x4,
    RELOCATION_TYPE_HIGH16 = 0x5,
    RELOCATION_TYPE_LOW16 = 0x6,
    RELOCAITION_TYPE_GPREL16 = 0x7,
    RELOCATION_TYPE_LITERAL = 0x8,
    RELOCATION_TYPE_GOT16 = 0x9,
    RELOCATION_TYPE_PC16 = 0xA,
    RELOCATION_TYPE_CALL16 = 0xB,
    RELOCATION_TYPE_GPREL32 = 0xC,
} relocation_type_t;

typedef struct relocation {
    u32 offset;
    u32 packed_symbol_index;
} relocation_t;

