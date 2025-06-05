#include <iostream>

void handleInit();

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: dit <command> [options]\n";
    std::cout << "Try 'dit help' for a list of commands.\n";
    return 1;
  }

  char* command = argv[1];
  char** options = argv + 2;
  int optionsCount = argc - 2;

  if (strcmp(command, "init") == 0) {
    handleInit();
  } else {
    std::cerr << "Unknown command: " << command << "\n";
    std::cerr << "Run 'dit help' for a list of commands.\n";
    return 1;
  }

  return 0;
}