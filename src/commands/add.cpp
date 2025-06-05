#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "../utils/sha1.h"

std::string readFile(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  std::ostringstream contents;
  contents << in.rdbuf();
  return contents.str();
}

std::string convertToBlob(std::string data) {
  return "blob " + std::to_string(data.size()) + '\0' + data;
}

std::string hashData(std::string data) {
  void *hashedData = hashing::sha1::hash(data);
  std::string hexHashedData = hashing::sha1::sig2hex(hashedData);
  delete[] static_cast<uint8_t *>(hashedData);
  return hexHashedData;
}

void handleAdd(const std::vector<std::string>& files) {
  for (std::string file : files) {
    std::string fileData = readFile(file);
    std::string fileBlob = convertToBlob(fileData);
    std::string hashedData = hashData(fileBlob);
    std::cout << hashedData << '\n';
  }
}