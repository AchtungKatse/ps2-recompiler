#pragma once

#include "ps2rc/defines.h"
#include <stdlib.h>

typedef enum log_level : u8 {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
} log_level_t;

void pvt_log(log_level_t level, const char* format, ...);

#define log_debug(...) pvt_log(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define log_trace(...) pvt_log(LOG_LEVEL_TRACE, __VA_ARGS__)
#define log_info(...) pvt_log(LOG_LEVEL_INFO, __VA_ARGS__)
#define log_warn(...) pvt_log(LOG_LEVEL_WARN, __VA_ARGS__)
#define log_error(...) pvt_log(LOG_LEVEL_ERROR, __VA_ARGS__)
#define log_fatal(...) pvt_log(LOG_LEVEL_FATAL, __VA_ARGS__); abort()

#define ASSERT(cond, ...) if (!(cond)) { log_error(__VA_ARGS__); abort(); }
