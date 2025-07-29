#pragma once

typedef enum {
    CodeUnit_ELF,
    CodeUnit_XFF
} CodeUnit_TYPE;

struct CodeUnit {
    CodeUnit_TYPE type;
    const char* name;
};

extern const int numUnitPaths;
extern const struct CodeUnit* GetUnitPaths();
