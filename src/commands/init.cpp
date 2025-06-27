#include <iostream>
#include <filesystem>
#include <fstream>

void handleInit() {
  if (std::filesystem::exists(".dit")) {
    std::cout << "Repository already initialized.\n";
  }

  std::filesystem::create_directory(".dit");
  std::filesystem::create_directory(".dit/objects");
  std::filesystem::create_directory(".dit/refs");

  std::ofstream head(".dit/HEAD");
  head << "ref: refs/heads/main\n";
  head.close();
}