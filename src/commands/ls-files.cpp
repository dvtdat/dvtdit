#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "../utils/types.h"
#include "../utils/index_io.h"

void handleLsFiles(std::vector<std::string>& flags) {
  std::map<std::string, FileMetadata> indexes = index_io::readIndex();

  if ((int)flags.size() == 0) {
    flags.push_back("--cached");
  }

  for (std::string flag : flags) {
    if (flag == "-c" || flag == "--cached") {
      for (auto it = indexes.begin(); it != indexes.end(); it++) {
        std::cout << it->first << '\n';
      }
    }

    if (flag == "-d" || flag == "--deleted") {

    }

    if (flag == "-m" || flag == "--modified") {

    }

    if (flag == "-s" || flag == "--stage") {
      
    }
  }
}