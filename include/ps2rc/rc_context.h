#pragma once

#include "ps2rc/code_unit.h"
typedef struct rc_context {
    code_unit_t* code_units;
    u32 code_unit_count;

} rc_context_t;

