#pragma once
#include "ps2rc/defines.h"
#include "ps2rc/code_unit.h"

typedef struct rc_config {
    code_unit_t* code_units;
    u32 code_unit_count;
} rc_config_t;

void process_config_file(const char *filename, rc_config_t* out_config);

