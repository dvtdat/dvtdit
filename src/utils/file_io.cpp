#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "file_io.h"

namespace file_io {
  std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
  }
}