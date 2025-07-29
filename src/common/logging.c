#include "common/logging.h"
#include <stdarg.h>
#include <stdio.h>

#define log_buffer_length 0x8000
char log_buffer[log_buffer_length] = {};
char log_buffer_2[log_buffer_length] = {};

void pvt_log(log_level_t level, const char* format, ...) {
    const char* level_strings[6] = {
        "\x1B[32m[DEBUG]: ",
        "\x1B[36m[TRACE]: ",
        "\x1B[37m[INFO]:  ",
        "\x1B[33m[WARN]:  ",
        "\x1B[91m[ERROR]: ",
        "\x1B[31m[FATAL]: ",
    };

    // Format original message.
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(log_buffer, log_buffer_length, format, arg_ptr);
    va_end(arg_ptr);

    printf("%s%s\x1B[0m\n", level_strings[level], log_buffer);
}
