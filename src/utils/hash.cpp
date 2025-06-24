#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "sha1.h"
#include "hash.h"

namespace hash {
  std::string hashDataSHA1(std::string data) {
    void *hashedData = hashing::sha1::hash(data);
    std::string hexHashedData = hashing::sha1::sig2hex(hashedData);
    delete[] static_cast<uint8_t *>(hashedData);
    return hexHashedData;
  }

  void writeSHA1(std::ofstream &out, const std::string& hash) {
    for (int i = 0; i < 40; i += 2) {
      std::string byteStr = hash.substr(i, 2);
      uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
      out.write(reinterpret_cast<char*>(&byte), 1);
    }
  };
}