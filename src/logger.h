#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "types.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#ifdef NDEBUG
    #define LOG_DEBUG_ENABLED 1
    #define LOG_TRACE_ENABLED 1
#endif

typedef enum log_level {
	LOG_LEVEL_FATAL = 0,
	LOG_LEVEL_ERROR = 1,
	LOG_LEVEL_WARN = 2,
	LOG_LEVEL_INFO = 3,
	LOG_LEVEL_DEBUG = 4,
	LOG_LEVEL_TRACE = 5
} log_level_t;

bool logger_init(void);
void logger_free(void);

void logger_output(log_level_t level, const char *message, ...);

#define LOG_FATAL(message, ...) logger_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)

#ifndef LOG_ERROR
	#define LOG_ERROR(message, ...) logger_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)
#endif

#if LOG_WARN_ENABLED == 1
	#define LOG_WARN(message, ...) logger_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#else
	#define LOG_WARN(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
	#define LOG_DEBUG(message, ...) logger_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#else
	#define LOG_DEBUG(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
	#define LOG_INFO(message, ...) logger_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
	#define LOG_INFO(message, ...)
#endif


#endif
