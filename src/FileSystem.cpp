#include "FileSystem.h"

#include <fstream>
#include <sstream>

std::string FileSystem::load_as_string(const char* path) {
    std::ifstream filestream;
    filestream.open(path, std::ifstream::in);
    if (filestream.is_open()) {
        std::stringstream sstream;
        sstream << filestream.rdbuf();
        filestream.close();
        return sstream.str();
    }
    else {
        throw std::runtime_error("Could not open file");
    }
}
