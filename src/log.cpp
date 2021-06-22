#include "log.h"

#define _CRT_SECURE_NO_WARNINGS

#include <ctime>
#include <cstdio>
#include <cstdarg>

static struct {
    enum ELogLevel level;
    bool is_muted;
} config;

static const char *level_names[] = {
        "TRACE",
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
};

void log_log(enum ELogLevel level, const char *file, int line, const char *format, ...) {
    if (level < config.level || config.is_muted) {
        return;
    }

    // Get the current local time and convert it to a human readable format
    time_t current_time = time(nullptr);
    tm *local_time = localtime(&current_time);
    char formatted_time[16];
    formatted_time[strftime(formatted_time, sizeof(formatted_time), "%H:%M:%S", local_time)] = '\0';

    FILE *output_stream = level < LOG_LEVEL_WARN ? stdout : stderr;
    fprintf(output_stream, "%s %-5s %s:%d: ", formatted_time, level_names[level], file, line);

    va_list args;
            va_start(args, format);
    vfprintf(output_stream, format, args);
            va_end(args);
    fprintf(output_stream, "\n");
    fflush(output_stream);
}

void log_set_level(enum ELogLevel level) {
    config.level = level;
}
