#include "log.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

static struct
{
    LogLevel level;
    bool is_muted;
} config;

static const char* level_names[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

// Colors are Windows console colors according to the table here:
// https://github.com/shiena/ansicolor/blob/4593a90318c32343a5cf962281026d61e96a4e70/README.md
static const char* level_colors[] = {
  "\x1b[37m", // white
  "\x1b[36m", // cyan
  "\x1b[32m", // green
  "\x1b[33m", // yellow
  "\x1b[31m", // red
  "\x1b[31;1m"  // bold red
};

void log_log(LogLevel level, const char* file, int line, const char* format, ...)
{
    if (level < config.level || config.is_muted)
        return;

    // Get the current local time and convert it to a human readable format
    time_t current_time = time(NULL);
    struct tm local_time;
    localtime_s(&local_time, &current_time);
    char formatted_time[16];
    formatted_time[strftime(formatted_time, sizeof(formatted_time), "%H:%M:%S", &local_time)] = '\0';

    // Log to console
    // \x1b[0m = reset attributes
    // \x1b[90m = light gray
    FILE* output_stream = stdout;
    fprintf(output_stream, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", formatted_time, level_colors[level], level_names[level], file, line);

    va_list args;
    va_start(args, format);
    vfprintf(output_stream, format, args);
    va_end(args);
    fprintf(output_stream, "\n");
    fflush(output_stream);
}

void log_init()
{
    config.level = LOG_LEVEL_TRACE;
    config.is_muted = false;
}

void log_set_level(LogLevel level)
{
    config.level = level;
}

void log_toggle_mute()
{
    config.is_muted = !config.is_muted;
}
