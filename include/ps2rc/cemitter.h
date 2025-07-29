#pragma once

#include "ps2rc/code_unit.h"
#include "ps2rc/rc_context.h"
#include "ps2rc/splits.h"

void cemitter_process_split(rc_context_t* context, elf_section_header_t* section, split_t split, code_unit_t* unit, elf_section_header_t* header, const char* output_path);
