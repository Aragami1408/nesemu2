#include "logger.h"
#include "asserts.h"

bool logger_initbool() {
    // TODO: create a log file
    return true;
}

void logger_free() {
    // TODO: cleanup logger/write queued entries.
}

void logger_output(log_level_t level, const char *message, ...) {
    const char *level_strings[6] = {
			"[FATAL]: ",
			"[\033[0;31mERROR\033[0m]: " ,
			"[\033[0;33mWARN\033[0m]: ",
			"[\033[0;32mINFO\033[0m]: ",
			"[DEBUG]: ",
			"[TRACE]: "
	};
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	fprintf(stderr, "[%d/%d/%d %d:%d:%d] %s %10s:%10d:%15s(): ",
			timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1,
			timeinfo->tm_mday,
			timeinfo->tm_hour,
			timeinfo->tm_min,
			timeinfo->tm_sec,

			level_strings[level],

			__FILE__,
			__LINE__,
			__func__);

	va_list args;
	va_start(args, message);
	vfprintf(stderr, message, args);
	va_end(args);
}

void report_assertion_failure(const char *expression, const char *message, const char *file, i32 line) {
	logger_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}
