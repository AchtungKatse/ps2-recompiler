#include "ps2rc/elf/elf.h"
#include "common/logging.h"

#include <string.h>

elf_section_type_t elf_section_type_from_string(const char* string) {
    if (strcmp(string, ".text") == 0) {
        return ELF_SECTION_TYPE_TEXT;
    } else if (strcmp(string, ".bss") == 0) {
        return ELF_SECTION_TYPE_BSS;
    } else if (strcmp(string, ".comment") == 0) {
        return ELF_SECTION_TYPE_COMMENT;
    } else if (strcmp(string, ".data") == 0) {
        return ELF_SECTION_TYPE_DATA;
    } else if (strcmp(string, ".rodata") == 0) {
        return ELF_SECTION_TYPE_RODATA;
    } else if (strcmp(string, ".text_nop") == 0) {
        return ELF_SECTION_TYPE_TEXT_NOP;
    } else if (strcmp(string, ".reginfo") == 0) {
        return ELF_SECTION_TYPE_REGINFO;
    } else if (strcmp(string, ".ctors") == 0) {
        return ELF_SECTION_TYPE_CTORS;
    } else if (strcmp(string, ".dtors") == 0) {
        return ELF_SECTION_TYPE_DTORS;
    } else if (strcmp(string, ".eh_frame") == 0) {
        return ELF_SECTION_TYPE_EH_FRAME;
    } else if (strcmp(string, ".lit4") == 0) {
        return ELF_SECTION_TYPE_LIT4;
    } else if (strcmp(string, ".data_nop") == 0) {
        return ELF_SECTION_TYPE_DATA_NOP;
    } else if (strcmp(string, ".sdata") == 0) {
        return ELF_SECTION_TYPE_SDATA;
    } else if (strcmp(string, ".sbss") == 0) {
        return ELF_SECTION_TYPE_SBSS;
    } else if (strcmp(string, ".mdebug.eabi64") == 0) {
        return ELF_SECTION_TYPE_MDEBUG_EABI64;
    } else if (strcmp(string, ".shstrtab") == 0) {
        return ELF_SECTION_TYPE_SHSTRTAB;
    } else if (strcmp(string, ".rel.text") == 0) {
        return ELF_SECTION_TYPE_RELTEXT;
    } else if (strcmp(string, ".strtab") == 0) {
        return ELF_SECTION_TYPE_RELSTRTAB;
    } else if (strcmp(string, ".symtab") == 0) {
        return ELF_SECTION_TYPE_SYMTAB;
    } else if (strcmp(string, ".rela.rodata") == 0) {
        return ELF_SECTION_TYPE_RELRODATA;
    } else if (strcmp(string, ".rel.rodata") == 0) {
        return ELF_SECTION_TYPE_RELRODATA;
    } else if (strcmp(string, "") == 0) {
        return ELF_SECTION_TYPE_NULL;
    } else if (strcmp(string, ".rela.text") == 0) {
        return ELF_SECTION_TYPE_RELTEXT;
    } else if (strcmp(string, ".rela.data") == 0) {
        return ELF_SECTION_TYPE_RELDATA;
    } else if (strcmp(string, ".rel.data") == 0) {
        return ELF_SECTION_TYPE_RELDATA;
    } else {
        log_error("Unknown section '%s'", string);
        return ELF_SECTION_TYPE_NULL;
    }

}

// TODO: Enusure this is implemented correctly, I just guessed what should be where
elf_section_header_type_t elf_section_header_type_from_section_type(elf_section_type_t type) {
    switch (type) {
        case ELF_SECTION_TYPE_NULL:
            return SHT_NULL;
        case ELF_SECTION_TYPE_BSS:
            return SHT_NOBITS;
        case ELF_SECTION_TYPE_COMMENT:
            return SHT_NOTE;
        case ELF_SECTION_TYPE_DATA:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_DATA1:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_DEBUG:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_DYNAMIC:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_HASH:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_LINE:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_NOTE:
            return SHT_NOTE;
        case ELF_SECTION_TYPE_RODATA:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_RODATA1:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_SHSTRTAB:
            return SHT_STRTAB;
        case ELF_SECTION_TYPE_STRTAB:
            return SHT_STRTAB;
        case ELF_SECTION_TYPE_SYMTAB:
            return SHT_SYMTAB;
        case ELF_SECTION_TYPE_TEXT:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_TEXT_NOP:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_REGINFO:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_CTORS:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_DTORS:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_EH_FRAME:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_LIT4:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_DATA_NOP:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_SDATA:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_SBSS:
            return SHT_NOBITS;
        case ELF_SECTION_TYPE_MDEBUG_EABI64:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_RELTEXT:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_RELSTRTAB:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_RELRODATA:
            return SHT_PROGBTIS;
        case ELF_SECTION_TYPE_RELDATA:
            return SHT_PROGBTIS;
          break;
        }
}
