#include "FileSystem.h"

#include <fstream>
#include <sstream>

std::string FileSystem::load_as_string(const std::string& path) {
    std::ifstream filestream(path,std::ifstream::in);
    if (filestream.is_open()) {
        std::stringstream sstream;
        sstream << filestream.rdbuf();
        return sstream.str();
    } else {
        throw std::runtime_error("Could not open file");
    }
}
