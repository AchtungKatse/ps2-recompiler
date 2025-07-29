#include "ps2rc/defines.h"
typedef struct ElfInfoHeader ElfInfoHeader, *PElfInfoHeader;

typedef struct RelocationHeader RelocationHeader, *PRelocationHeader;

typedef struct Symbol Symbol, *PSymbol;

typedef struct SectionHeader SectionHeader, *PSectionHeader;

typedef struct Relocation Relocation, *PRelocation;

typedef enum SymbolFlags {
    None=0,
    Object=1,
    Function=2,
    Section=3,
    Invalid=4,
    External=16
} SymbolFlags;

typedef enum SH_Type {
    Null=0,
    ProgBits=1,
    SymbolTable=2,
    StringTable=3,
    RelocationAddend=4,
    Hash=5,
    Dynamic=6,
    Notes=7,
    NoBits=8,
    Relocatable=9,
    SHLIB=10,
    DynamicSymbol=11
} SH_Type;

struct Symbol {
    int nameOffset;
    void *data; /* Offset of the symbol relative to its section */
    int length;
    enum SymbolFlags flags;
    undefined field4_0xd;
    u16 section;
};

struct SectionHeader {
    void *data; /* I doubt this */
    void *sourceData; /* Address of the section data */
    int length;
    int alignment;
    enum SH_Type Type;
    int useCustomAlignment;
    int somethingSectionAlignment;
    int fileOffset;
};

typedef struct { /* What is used by the .xff file extension */
    int fileMagic; /* Created by retype action */
    int field1_0x4;
    struct RelocationHeader *nextRelocationHeader;
    u8 field3_0xc;
    u8 field4_0xd;
    u8 field5_0xe;
    u8 field6_0xf;
    int entryPoint;
    int fileLength;
    int endAddress;
    int stringCount;
    int stringTablePtr;
    int symbolCount;
    struct Symbol *symbolArray; /* doubt */
    int symbolStringTable;
    struct SectionHeader *sectionHeaders;
    void **symbolToAddress;
    int relocationHeaderCount;
    struct RelocationHeader *relocationHeaders;
    int sectionCount;
    char **sectionNameOffsets;
    char *sectionStringTable;
    int entryPointOffset;
    int stringOffsetsFileOffset;
    int symbolFileOffset;
    int stringTableOffset;
    int sectionHeaderOffset;
    int symbolAddresses;
    int relocationHeaderOffset;
    int file_sectionNameOffsets;
    int sectionStringTableOffset;
} ModuleInfo ;

struct RelocationHeader {
    int type;
    int relocationCount;
    int sectionIndex;
    struct Relocation *relocations;
    void *unknownDataPtr;
    int relocationArrayFileOffset;
    int someFileOffset;
};

struct Relocation {
    int offset;
    int packedSymbolIndex;
};

