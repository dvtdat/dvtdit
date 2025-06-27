#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <iomanip> 

#include "../utils/types.h"
#include "../utils/index_utils.h"

void handleCommit(const Arguments& args) {
  std::map<std::string, FileMetadata> indexes = index_utils::readIndex();

  if (args.flagsWithValue.find("-m") == args.flagsWithValue.end() && args.flagsWithValue.find("--message") == args.flagsWithValue.end()) {
    std::cerr << "Aborting commit due to empty commit message.\n";
    std::cerr << "Please use \"dit commit\" with \"-m\" or \"--message\" flag.\n";
    return;
  }

  std::string message = "";
  
  if (args.flagsWithValue.find("-m") != args.flagsWithValue.end()) {
    message = args.flagsWithValue.at("-m");
  }

  if (args.flagsWithValue.find("--message") != args.flagsWithValue.end()) {
    message = args.flagsWithValue.at("--message"); 
  }

  std::cout << message << '\n';
}