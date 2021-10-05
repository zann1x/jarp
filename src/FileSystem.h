#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>

class FileSystem {
public:
    static std::string load_as_string(const char* path);
};

#endif
