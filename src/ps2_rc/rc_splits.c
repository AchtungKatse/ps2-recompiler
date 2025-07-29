#include "ps2rc/elf/elf.h"
#include "ps2rc/ps2_registers.h"
#include "ps2rc/splits.h"
#include "common/logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#define INITIAL_SPLIT_BUFFER_COUNT 8192

typedef enum split_parser_state : s8 {
    SPLIT_PARSER_STATE_ROOT = 0,
    SPLIT_PARSER_STATE_SPLIT = 1,
    SPLIT_PARSER_STATE_SIGNATURE = 2,
    SPLIT_PARSER_STATE_PARAMETER = 3,
    SPLIT_PARSER_STATE_PATCH_ARRAY = 4,
    SPLIT_PARSER_STATE_PATCH = 5,
} split_parse_state_t;

const char* parser_state_strings[6] = {
    "ROOT",
    "SPLIT", 
    "SIGNATURE", 
    "PARAMETER", 
    "PATCH_ARRAY", 
    "PATCH", 
};

const char* clone_string(const char* string);
b8 string_to_bool(const char* string);
ee_register_t ee_register_from_string(const char* string);

split_t* split_read_from_yaml(const char* filepath, u32* out_split_count) {
    FILE *fh = fopen(filepath, "rb");
    ASSERT(fh, "Failed to read splits from yaml file '%s'", filepath);

    yaml_parser_t parser;
    yaml_event_t event;

    ASSERT(yaml_parser_initialize(&parser), "Failed to initialize YAML parser!\n");


    yaml_parser_set_input_file(&parser, fh);
    split_parse_state_t state = 0;
    split_t split = {};

    u32 split_count = 0;
    u32 split_buffer_capacity = INITIAL_SPLIT_BUFFER_COUNT;
    split_t* out_splits = malloc(sizeof(split_t) * INITIAL_SPLIT_BUFFER_COUNT);

    while (1) {
        if (!yaml_parser_parse(&parser, &event))
            break;

        switch (event.type) {
            default:
                break;
            case YAML_SCALAR_EVENT:
                break;
            case YAML_MAPPING_START_EVENT:
                // log_debug("Incrementing state from %s", parser_state_strings[state]); 
                switch (state) {
                case SPLIT_PARSER_STATE_ROOT:
                    state = SPLIT_PARSER_STATE_SPLIT;
                    break;
                case SPLIT_PARSER_STATE_SPLIT:
                    state = SPLIT_PARSER_STATE_SIGNATURE;
                    break;
                case SPLIT_PARSER_STATE_SIGNATURE:
                    state = SPLIT_PARSER_STATE_PARAMETER;
                    split.signature.function_parameters[split.signature.function_parameter_count].ee_register = EE_REGISTER_A0;
                    break;
                case SPLIT_PARSER_STATE_PARAMETER:
                    log_warn("Cant move up from parameter");
                    break;
                case SPLIT_PARSER_STATE_PATCH_ARRAY:
                    state = SPLIT_PARSER_STATE_PATCH;
                    break;
                case SPLIT_PARSER_STATE_PATCH:
                    log_warn("Cant move up from patch");
                    break;
                }
                // log_debug("\tto %s", parser_state_strings[state]); 

                break;
            case YAML_SEQUENCE_END_EVENT:
                if (state != SPLIT_PARSER_STATE_PATCH_ARRAY) {
                // log_warn("SEQ_END");
                    break;
                }
            case YAML_MAPPING_END_EVENT:
                // log_debug("Decrementing state from %s", parser_state_strings[state]); 
                switch (state) {
                    case SPLIT_PARSER_STATE_ROOT:
                        log_error("Going before state root is not allowed.");
                        break;
                    case SPLIT_PARSER_STATE_SPLIT:
                        state = SPLIT_PARSER_STATE_ROOT;
                        break;
                    case SPLIT_PARSER_STATE_SIGNATURE:
                        state = SPLIT_PARSER_STATE_SPLIT;
                        break;
                    case SPLIT_PARSER_STATE_PARAMETER:
                        state = SPLIT_PARSER_STATE_SIGNATURE;
                        split.signature.function_parameter_count++;
                        break;
                    case SPLIT_PARSER_STATE_PATCH_ARRAY:
                        split.patch_count++;
                        state = SPLIT_PARSER_STATE_SPLIT;
                        break;
                    case SPLIT_PARSER_STATE_PATCH:
                        state = SPLIT_PARSER_STATE_PATCH_ARRAY;
                        break;
                }
                if (state == SPLIT_PARSER_STATE_ROOT) {
                    if (split_count >= split_buffer_capacity) {
                        split_t* tmp = malloc(sizeof(split_t) * split_buffer_capacity * 2);
                        memcpy(tmp, out_splits, split_count * sizeof(split_t));
                        out_splits = tmp;
                    }

                    if (split.section == ELF_SECTION_TYPE_NULL) {
                        split.section = ELF_SECTION_TYPE_TEXT;
                    }

                    out_splits[split_count++] = split;
                    split = (split_t) {};
                }
                // log_debug("\tto %s", parser_state_strings[state]); 
                break;
        }

        if (event.type == YAML_SCALAR_EVENT) {
            const char* key = (const char*)event.data.scalar.value;

            if (strcmp(key, "FunctionSignature") == 0 ||
                    strcmp(key, "Parameters") == 0) {
                continue;
            }

            if (strcmp(key, "Patches") == 0) {
                state = SPLIT_PARSER_STATE_PATCH_ARRAY;
                // log_debug("Switch to patch array");
                continue;
            }

            ASSERT(yaml_parser_parse(&parser, &event), "Failed to get value from scalar event.");
            const char* value  = (const char*)event.data.scalar.value;

            // log_debug("\t'%s' : '%s'", key, value);

            switch (state) {
                default:
                    log_error("Failed to get state of split parser: %d", state);
                    break;
                case SPLIT_PARSER_STATE_ROOT:
                    log_warn("Cannot parse scalar in split root. Key: '%s'", key);
                    break;
                case SPLIT_PARSER_STATE_SPLIT:
                    if (strcmp(key, "StartAddress") == 0) {
                        if (value[1] == 'x') {
                            split.start_address = strtol(value, NULL, 16);
                        } else {
                            split.start_address = atoi(value);
                        }
                    } else if (strcmp(key, "VArgs") == 0) {
                        split.varg = string_to_bool(value);
                    } else if (strcmp(key, "EndAddress") == 0) {
                        if (value[1] == 'x') {
                            split.end_address = strtol(value, NULL, 16);
                        } else {
                            split.end_address = atoi(value);
                        }
                    } else if (strcmp(key, "Type") == 0) {
                        log_warn("Type key in split state not implemented.");
                    } else if (strcmp(key, "Section") == 0) {
                        split.section = elf_section_type_from_string(value);
                    } else {
                        log_error("Failed to match key '%s' in split state", key);
                    }
                    break;
                case SPLIT_PARSER_STATE_SIGNATURE:
                    if (strcmp(key, "ReturnType") == 0) {
                        split.signature.return_type = clone_string(value);
                    } else if (strcmp(key, "VArgs") == 0) {
                        split.varg = string_to_bool(value);
                    } else if (strcmp(key, "Name") == 0) {
                        split.signature.name = clone_string(value);
                        // log_debug("READING split '%s'", split.signature.name);
                    } else if (strcmp(key, "IsLibC") == 0) {
                        split.signature.is_lib_c = string_to_bool(value);
                    } else if (strcmp(key, "Generate") == 0) {
                        split.signature.generate = string_to_bool(value);
                    } else {
                        log_error("Failed to match key '%s' in signature state", key);
                    }

                    break;
                case SPLIT_PARSER_STATE_PARAMETER:
                    if (strcmp(key, "Type") == 0) {
                        split.signature.function_parameters[split.signature.function_parameter_count].type = clone_string(value);
                    } else if (strcmp(key, "Name") == 0) {
                        split.signature.function_parameters[split.signature.function_parameter_count].name = clone_string(value);
                    } else if (strcmp(key, "Register") == 0) {
                        // split.signature.function_parameters[split.signature.function_parameter_count].ee_register = ee_register_from_string(value);
                    } else {
                        log_error("Failed to match key '%s' in parameter state.", key);
                    }
                    break;
                case SPLIT_PARSER_STATE_PATCH_ARRAY:
                    log_error("Got key '%s' in patch array. No keys are expected.", key);
                    break;
                case SPLIT_PARSER_STATE_PATCH:
                    if (split.patches[split.patch_count].value &&
                            split.patches[split.patch_count].value) {
                        split.patch_count++;
                    }
                    if (strcmp(key, "Index") == 0) {
                        split.patches[split.patch_count].line_index = atoi(value);
                    } else if (strcmp(key, "Value") == 0) {
                        split.patches[split.patch_count].value = clone_string(value);
                        log_debug("'%s' Patch value: '%s' at %d", split.signature.name, value, split.patches[split.patch_count].line_index);
                    } else {
                        log_error("Failed to match key '%s' in parameter state.", key);
                    }
                    break;
            }
        }

        if (event.type == YAML_STREAM_END_EVENT)
            break;

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(fh);

    log_debug("Found %d splits", split_count);
    *out_split_count = split_count;
    return out_splits;
}

ee_register_t ee_register_from_string(const char* string) {
    for (u32 i = 0; i < EE_REGISTER_MAX; i++) {
        if (strcmp(string, ee_register_names[i]) == 0) {
            return i;
        }
    }

    log_error("Failed to get ee register '%s'. Expected all lower case register.", string);
    return EE_REGISTER_ZERO;
}

const char* clone_string(const char* string) {
    u32 len = strlen(string) + 1;
    char* out = malloc(len);
    strcpy(out, string);
    return out;
}

b8 string_to_bool(const char* string) {
    if (strcmp(string, "true") == 0) {
        return true;
    } else if (strcmp(string, "false") == 0) {
        return false;
    } else {
        log_warn("Failed to get valid bool from string. Got '%s', expected 'true' or 'false'", string);
        return false;
    }
}
