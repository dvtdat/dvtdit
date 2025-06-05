#include <iostream>
#include <vector>
#include <string>

void handleInit();
void handleAdd(const std::vector<std::string>& files);

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: dit <command> [options]\n";
    std::cout << "Try 'dit help' for a list of commands.\n";
    return 1;
  }

  std::string command = argv[1];
  std::vector<std::string> options(argv + 2, argv + argc);

  if (command == "init") {
    handleInit();
  } else if (command == "add") {
    handleAdd(options);
  } else {
    std::cerr << "Unknown command: " << command << "\n";
    std::cerr << "Run 'dit help' for a list of commands.\n";
    return 1;
  }

  return 0;
}