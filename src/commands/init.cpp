#include <iostream>
#include <filesystem>
#include <fstream>

void handleInit() {
  namespace fs = std::filesystem;
  
  if (fs::exists(".dit")) {
    std::cout << "Repository already initialized.\n";
  }

  try {
    fs::create_directory(".dit");
    fs::create_directory(".dit/objects");
    fs::create_directory(".dit/refs");

    std::ofstream head(".dit/HEAD");
    head << "ref: refs/heads/main\n";
    head.close();
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error initializing repository: " << e.what() << '\n';
  }
}