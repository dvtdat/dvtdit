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

struct FileMetadata {
  struct stat st;
  std::string sha1;
  std::string path;

  FileMetadata() {}
  FileMetadata(const struct stat& statData, const std::string& sha1Hash, const std::string& filePath)
    : st(statData), sha1(sha1Hash), path(filePath) {}
};

std::map<std::string, FileMetadata> readIndex() {
  std::map<std::string, FileMetadata> entries;

  std::ifstream in(".dit/index", std::ios::binary);
  if (!in) {
    return entries;
  }

  char signature[4];
  in.read(signature, 4);
  if (std::string(signature, 4) != "DIRC") {
    return entries;
  }

  uint32_t version;
  in.read(reinterpret_cast<char*>(&version), 4);
  version = ntohl(version);
  if (version != 2) {
      std::cerr << "Unsupported index version: " << version << "\n";
      return entries;
  }

  uint32_t entryCount;
  in.read(reinterpret_cast<char*>(&entryCount), 4);
  entryCount = ntohl(entryCount);

  for (uint32_t i = 0; i < entryCount; ++i) {
    FileMetadata entry;
    struct stat &st = entry.st;

    uint32_t tmp;
    in.read(reinterpret_cast<char*>(&tmp), 4); st.st_ctime = ntohl(tmp);
    in.ignore(4);  
    in.read(reinterpret_cast<char*>(&tmp), 4); st.st_mtime = ntohl(tmp);
    in.ignore(4); 
    in.read(reinterpret_cast<char*>(&tmp), 4); st.st_dev = ntohl(tmp);
    in.read(reinterpret_cast<char*>(&tmp), 4); st.st_ino = ntohl(tmp);
    in.read(reinterpret_cast<char*>(&tmp), 4); st.st_mode = ntohl(tmp);
    in.read(reinterpret_cast<char*>(&tmp), 4); st.st_uid = ntohl(tmp);
    in.read(reinterpret_cast<char*>(&tmp), 4); st.st_gid = ntohl(tmp);
    in.read(reinterpret_cast<char*>(&tmp), 4); st.st_size = ntohl(tmp);

   
    char sha1_bin[20];
    in.read(sha1_bin, 20);

    std::ostringstream sha1_hex;
    for (int i = 0; i < 20; ++i) {
      sha1_hex << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(sha1_bin[i]));
    }
    entry.sha1 = sha1_hex.str();

    uint16_t flags;
    in.read(reinterpret_cast<char*>(&flags), 2);
    flags = ntohs(flags);
    uint16_t nameLength = flags & 0x0FFF;

    std::string path(nameLength, '\0');
    in.read(&path[0], nameLength);
    entry.path = path;

    in.ignore(1); 

    int entryLength = 62 + nameLength + 1;
    int padding = (8 - (entryLength % 8)) % 8;
    in.ignore(padding);

    entries[path] = entry;
  }

  return entries;
}

void writeIndex(const std::map<std::string, FileMetadata>& entries) {
  std::ofstream index(".dit/index", std::ios::binary);
  if (!index) {
    std::cerr << "Failed to open .dit/index for writing.\n";
    return;
  }
  
  index.write("DIRC", 4);                       // File signature
  write_uint32(index, 2);                       // Index version
  write_uint32(index, entries.size());          // Number of entries

  for (const auto& [path, entry] : entries) {
    const struct stat &st = entry.st;

    // write ctime + ctime_nsec
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
    writeSHA1(index, entry.sha1);               // 20 bytes - 80 bits

    // Flags (lower 12 bits = file name length, upper 4 bits = 0)
    uint16_t flags = std::min(static_cast<uint16_t>(path.length()), static_cast<uint16_t>(0xFFF));
    write_uint16(index, flags);

    // Path (not null-padded)
    index.write(path.c_str(), path.length());

    // Null terminator
    index.put('\0');

    // Add 8-byte padding from start of entry to align
    int entryLength = 62 + path.length() + 1;
    int padding = (8 - entryLength % 8) % 8;
    for (int i = 0; i < padding; ++i) {
      index.put('\0');
    }
  }

  index.close();
}

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
  std::map<std::string, FileMetadata> indexes = readIndex();

  for (const std::string& filename : files) {
    std::string fileData = readFile(filename);
    std::string fileBlob = convertToBlob(fileData);
    std::string hashedData = hashDataSHA1(fileBlob);
    struct stat st;
    stat(filename.c_str(), &st);

    auto it = indexes.find(filename);
    if (it != indexes.end()) {
      const FileMetadata& existingEntry = it->second;
      if (st.st_mtime == existingEntry.st.st_mtime && hashedData == existingEntry.sha1) {
        continue;
      }
    }

    FileMetadata metadata(st, hashedData, filename);
    indexes[filename] = metadata;
  }

  writeIndex(indexes);
  addChecksum();
}
