#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "utils/types.h"

void handleInit();
void handleAdd(const Arguments& args);
void handleLsFiles(const Arguments& args);

std::unordered_map<std::string, std::unordered_set<std::string>> FLAGS_WITH_VALUES = {
  {
    "commit",
    {
      "-m", "--message", "-F", "--file", "--author"
    }
  },
  {
    "add",
    {
      "-p", "--patch"
    }
  },
  {
    "log",
    {
      "-n", "--max-count", "--since", "--until", "--author", "--grep"
    }
  },
  {
    "status",
    {}
  },
  {
    "init",
    {}
  },
  {
    "ls-files",
    {}
  }
};

Arguments parseArgument(int argc, char* argv[]) {
  Arguments result;
  result.command = argv[1];

  std::unordered_set<std::string> flagsExpectingValues;
  if (FLAGS_WITH_VALUES.find(result.command) != FLAGS_WITH_VALUES.end()) {
    flagsExpectingValues = FLAGS_WITH_VALUES[result.command];
  }

  for (int i = 2; i < argc; ++i) {
    std::string arg = argv[i];
    
    if (arg[0] == '-') {
      if (flagsExpectingValues.find(arg) != flagsExpectingValues.end()) {
        if (i + 1 < argc && argv[i + 1][0] != '-') {
          result.flagsWithValue[arg] = argv[i + 1];
          ++i; 
        } else {
          std::cerr << "Error: Flag " << arg << " requires a value\n";
          result.command = ""; 
          return result;
        }
      } else {
        result.flags.insert(arg);
      }
    } else {
      result.files.push_back(arg);
    }
  }

  std::sort(result.files.begin(), result.files.end());
  result.files.erase(std::unique(result.files.begin(), result.files.end()), result.files.end());
  
  return result;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: dit <command> [options]\n";
    std::cout << "Try 'dit help' for a list of commands.\n";
    return 1;
  }

  Arguments args = parseArgument(argc, argv);

  if (args.command.empty()) {
    return 1;
  }

  if (args.command == "init") {
    handleInit();
  } else if (args.command == "add") {
    handleAdd(args);
  } else if (args.command == "ls-files") {
    handleLsFiles(args);
  } else if (args.command == "commit") {
    //
  } else {
    std::cerr << "Unknown command: " << args.command << "\n";
    std::cerr << "Run 'dit help' for a list of commands.\n";
    return 1;
  }
}