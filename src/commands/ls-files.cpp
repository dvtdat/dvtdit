#include <iostream>
#include <filesystem>
#include <fstream>

void handleLsFiles(const std::vector<std::string>& flags) {
  if ((int)flags.size() == 0) {
    flags.push_back("--cached");
  }

  for (std::string flag : flags) {
    if (flag == "-c" || flag == "--cached") {

    }

    if (flag == "-d" || flag == "--deleted") {

    }

    if (flag == "-m" || flag == "--modified") {

    }

    if (flag == "-s" || flag == "--stage") {
      
    }
  }
}