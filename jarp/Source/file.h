#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "log.h"

static char* file_read_asc(const char* path)
{
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        log_error("file %s could not be opened", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*) malloc(sizeof(char) * size + 1u);
    if (buffer == NULL)
    {
        log_error("reading buffer for file %s could not be allocated", path);
        return NULL;
    }

    size_t read_count = fread(buffer, 1, size, file);
    if (ferror(file))
    {
        log_error("failed to read file %s", path);
        return NULL;
    }
    buffer[read_count] = '\0';

    fclose(file);

    return buffer;
}
