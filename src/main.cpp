#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

void handleInit();
void handleAdd(const std::vector<std::string>& files, const std::vector<std::string>& flags);
void handleLsFiles(std::vector<std::string> &flags);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: dit <command> [options]\n";
    std::cout << "Try 'dit help' for a list of commands.\n";
    return 1;
  }

  std::string command = argv[1];
  std::vector<std::string> options(argv + 2, argv + argc);
  std::vector<std::string> files, flags;
  for (std::string option : options) {
    if (option[0] == '-' || (option[0] == '-' && option[1] == '-')) {
      flags.push_back(option);
    } else {
      files.push_back(option);
    }
  }

  files.erase(unique(files.begin(), files.end()), files.end());
  flags.erase(unique(flags.begin(), flags.end()), flags.end());

  if (command == "init") {
    handleInit();
  } else if (command == "add") {
    handleAdd(files, flags);
  } else if (command == "ls-files") {
    handleLsFiles(flags);
  } else if (command == "commit") {
    // TODO
  } else if (command == "log") {
    // TODO
  } else if (command == "status") {
    // TODO
  } else if (command == "help") {
    // TODO
  } else {
    std::cerr << "Unknown command: " << command << "\n";
    std::cerr << "Run 'dit help' for a list of commands.\n";
    return 1;
  }

  return 0;
}