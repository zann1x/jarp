/*
====================
file_read_asc
====================
*/
struct FileContent file_read_asc(const char* path) {
    struct FileContent file_content = { 0 };
    FILE* file = fopen(path, "r");

    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        file_content.length = ftell(file) + 1u;
        rewind(file);

        file_content.buffer = (char*)malloc(sizeof(char) * file_content.length + 1u);
        if (file_content.buffer != NULL) {
            size_t read_count = fread(file_content.buffer, 1, ((size_t)file_content.length) - 1u, file);
            if (!ferror(file)) {
                file_content.buffer[read_count] = '\0';
            } else {
                log_error("failed to read file %s", path);
                free(file_content.buffer);
            }
        } else {
            log_error("reading buffer for file %s could not be allocated", path);
        }

        fclose(file);
    } else {
        log_error("file %s could not be opened", path);
    }

    return file_content;
}

/*
====================
file_read_bin
====================
*/
struct FileContent file_read_bin(const char* path) {
    struct FileContent file_content = { 0 };
    FILE* file = fopen(path, "rb");

    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        file_content.length = ftell(file);
        rewind(file);

        file_content.buffer = (char*)malloc(sizeof(char) * file_content.length);
        if (file_content.buffer != NULL) {
            size_t read_count = fread(file_content.buffer, 1, ((size_t)file_content.length), file);
            if (ferror(file)) {
                log_error("failed to read file %s", path);
                free(file_content.buffer);
            }
        } else {
            log_error("reading buffer for file %s could not be allocated", path);
        }

        fclose(file);
    } else {
        log_error("file %s could not be opened", path);
    }

    return file_content;
}
