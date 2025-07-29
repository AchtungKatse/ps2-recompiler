#pragma once

#include "ps2rc/defines.h"
typedef enum ee_register {
    EE_REGISTER_ZERO,
    EE_REGISTER_AT,
    EE_REGISTER_V0,
    EE_REGISTER_V1,
    EE_REGISTER_A0,
    EE_REGISTER_A1,
    EE_REGISTER_A2,
    EE_REGISTER_A3,
    EE_REGISTER_T0,
    EE_REGISTER_T1,
    EE_REGISTER_T2,
    EE_REGISTER_T3,
    EE_REGISTER_T4,
    EE_REGISTER_T5,
    EE_REGISTER_T6,
    EE_REGISTER_T7,
    EE_REGISTER_S0,
    EE_REGISTER_S1,
    EE_REGISTER_S2,
    EE_REGISTER_S3,
    EE_REGISTER_S4,
    EE_REGISTER_S5,
    EE_REGISTER_S6,
    EE_REGISTER_S7,
    EE_REGISTER_T8,
    EE_REGISTER_T9,
    EE_REGISTER_K0,
    EE_REGISTER_K1,
    EE_REGISTER_GP,
    EE_REGISTER_SP,
    EE_REGISTER_FP,
    EE_REGISTER_RA,
    EE_REGISTER_MAX,
} ee_register_t;

static const char* ee_register_names[] = {
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9",
    "k0", "k1",
    "gp", "sp", "fp", "ra"
};

static const char* cop0_register_names[] = {        
    "Index",
    "Random",
    "EntryLo0",
    "EntryLo1",
    "Context",
    "PageMask",
    "Wired",
    "BadVaddr",
    "Count",
    "EntryHi",
    "Compare",
    "Status",
    "Cause",
    "EPC",
    "PRid",
    "Config",
    "BadPAddr",
    "Debug",
    "Perf",
    "TagLo",
    "TagHi",
    "ErrorEPC",
    "Reserved23"
};

static const u32 cop0_register_names_count = sizeof(cop0_register_names) / sizeof(const char*);
