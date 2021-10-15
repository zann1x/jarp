#include "FileSystem.h"

#include <fstream>
#include <sstream>

std::string FileSystem::load_as_string(const std::string& path) {
    std::ifstream filestream(path, std::ifstream::in | std::ifstream::binary);
    if (!filestream.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    std::stringstream sstream;
    sstream << filestream.rdbuf();
    filestream.close();
    return sstream.str();
}
