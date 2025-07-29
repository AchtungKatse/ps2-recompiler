#include "ps2rc/rc_config.h"
#include "common/logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <yaml.h>

void process_config_file(const char *filename, rc_config_t* out_config) {
    FILE *fh = fopen(filename, "rb");
    yaml_parser_t parser;
    yaml_event_t event;

    if (!yaml_parser_initialize(&parser))
        fputs("Failed to initialize YAML parser!\n", stderr);

    if (fh == NULL)
        fputs("Failed to open file!\n", stderr);

    yaml_parser_set_input_file(&parser, fh);

    out_config->code_units = malloc(sizeof(rc_config_t) * MAX_CODE_UNITS);
    out_config->code_unit_count = 0;
    while (1) {
        if (!yaml_parser_parse(&parser, &event))
            break;

        if (event.type == YAML_MAPPING_END_EVENT) {
            out_config->code_unit_count++;
        }

        if (event.type == YAML_SCALAR_EVENT) {
            code_unit_t* unit = &out_config->code_units[out_config->code_unit_count];
            const char* key = (const char*)event.data.scalar.value;

            if (!yaml_parser_parse(&parser, &event))
                break;
            const char* value = (const char*)event.data.scalar.value;

            log_debug("Scalar %d: '%s' : '%s'", out_config->code_unit_count, key, value);
            u32 value_len = strlen(value);
            if (strcmp(key, "name") == 0) {
                unit->name = malloc(value_len + 1);
                strcpy((char*)unit->name, value);
            } else if (strcmp(key, "file_path") == 0) {
                unit->file_path = malloc(value_len + 1);
                strcpy((char*)unit->file_path, value);
            } else if (strcmp(key, "split_path") == 0) {
                unit->splits_path = malloc(value_len + 1);
                strcpy((char*)unit->splits_path, value);
            } else if (strcmp(key, "type") == 0) {
                unit->type = malloc(value_len + 1);
                strcpy((char*)unit->type, value);
            } 
        }

        if (event.type == YAML_STREAM_END_EVENT)
            break;

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(fh);
}
