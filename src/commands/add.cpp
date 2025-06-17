#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <arpa/inet.h>
#include <sys/stat.h>

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

std::string hashDataSHA1(std::string data) {
  void *hashedData = hashing::sha1::hash(data);
  std::string hexHashedData = hashing::sha1::sig2hex(hashedData);
  delete[] static_cast<uint8_t *>(hashedData);
  return hexHashedData;
}

void write_uint32(std::ofstream &out, uint32_t value) {
  uint32_t be = htonl(value); 
  out.write(reinterpret_cast<char*>(&be), sizeof(be));
};

void write_uint16(std::ofstream &out, uint16_t value) {
  uint16_t be = htons(value);
  out.write(reinterpret_cast<char*>(&be), sizeof(be));
};

void writeSHA1(std::ofstream &out, const std::string& hash) {
  for (int i = 0; i < 40; i += 2) {
      std::string byteStr = hash.substr(i, 2);
      uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
      out.write(reinterpret_cast<char*>(&byte), 1);
  }
};

// std::map<std::string, std::string> readIndex() {
 
// }

void addChecksum() {
  std::string indexFileData = readFile(".dit/index");
  std::string hashedIndexFileData = hashDataSHA1(indexFileData);

  std::ofstream index(".dit/index", std::ios::app | std::ios::binary);

  for (int i = 0; i < 40; i += 2) {
    std::string byteStr = hashedIndexFileData.substr(i, 2);
    uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
    index.write(reinterpret_cast<char*>(&byte), 1);
  }

  index.close();
}

void handleAdd(const std::vector<std::string>& files) {
  std::ofstream index(".dit/index", std::ios::binary);

  index.write("DIRC", 4);
  write_uint32(index, 2); // version
  write_uint32(index, files.size()); // number of entries

  for (const std::string& file : files) {
    std::string fileData = readFile(file);
    std::string fileBlob = convertToBlob(fileData);
    std::string hashedData = hashDataSHA1(fileBlob);

    struct stat st;
    stat(file.c_str(), &st);

    // write time + ctime_nsec
    write_uint32(index, st.st_ctime);           // 4 bytes - 32 bits
    write_uint32(index, 0);                     // 4 bytes - 32 bits
    
    // Write mtime + mtime_nsec
    write_uint32(index, st.st_mtime);           // 4 bytes - 32 bits 
    write_uint32(index, 0);                     // 4 bytes - 32 bits  

    write_uint32(index, st.st_dev);             // 4 bytes - 32 bits 
    write_uint32(index, st.st_ino);             // 4 bytes - 32 bits 
    write_uint32(index, st.st_mode);            // 4 bytes - 32 bits 

    write_uint32(index, st.st_uid);             // 4 bytes - 32 bits 
    write_uint32(index, st.st_gid);             // 4 bytes - 32 bits 
    write_uint32(index, st.st_size);            // 4 bytes - 32 bits 

    // Write binary SHA1
    writeSHA1(index, hashedData);               // 20 bytes - 80 bits

    // Flags (8-bit name length maxed to 0xFFF)
    uint16_t flags = std::min(static_cast<uint16_t>(file.length()), static_cast<uint16_t>(0xFFF));
    write_uint16(index, flags);                 // 2 bytes - 8 bits

    // Write path (not null-padded yet)
    index.write(file.c_str(), file.length());   // dynamic number of bytes based on the file name

    // Null Terminator
    index.put('\0');                            // 1 bytes - 4 bits
    
    // Add 8-byte padding from start of entry to align
    int entryLength = 62 + file.length() + 1;
    int padding = 8 - (entryLength + 8) % 8;
    for (int i = 0; i < padding % 8; ++i) {
      index.put('\0');
    }
  }

  index.close();
  addChecksum();
}
