#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <sys/stat.h>
#include <iomanip> 

#include "types.h"
#include "hash.h"
#include "binary_io.h"
#include "file_utils.h"

namespace index_utils {
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
      entry.flags = flags;
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
    
    index.write("DIRC", 4);                                  // File signature
    binary_io::write_uint32(index, 2);                       // Index version
    binary_io::write_uint32(index, entries.size());          // Number of entries

    for (const auto& [path, entry] : entries) {
      const struct stat &st = entry.st;

      // write ctime + ctime_nsec
      binary_io::write_uint32(index, st.st_ctime);           // 4 bytes - 32 bits
      binary_io::write_uint32(index, 0);                     // 4 bytes - 32 bits
      
      // Write mtime + mtime_nsec
      binary_io::write_uint32(index, st.st_mtime);           // 4 bytes - 32 bits 
      binary_io::write_uint32(index, 0);                     // 4 bytes - 32 bits  

      binary_io::write_uint32(index, st.st_dev);             // 4 bytes - 32 bits 
      binary_io::write_uint32(index, st.st_ino);             // 4 bytes - 32 bits 
      binary_io::write_uint32(index, st.st_mode);            // 4 bytes - 32 bits 

      binary_io::write_uint32(index, st.st_uid);             // 4 bytes - 32 bits 
      binary_io::write_uint32(index, st.st_gid);             // 4 bytes - 32 bits 
      binary_io::write_uint32(index, st.st_size);            // 4 bytes - 32 bits 

      // Write binary SHA1
      hash::writeSHA1(index, entry.sha1);                    // 20 bytes - 80 bits

      // Flags (lower 12 bits = file name length, upper 4 bits = 0)
      // TODO: update upper 4 bits with custom flag, which in clude stage version (0, 1, 2, 3)
      uint16_t flags = std::min(static_cast<uint16_t>(path.length()), static_cast<uint16_t>(0xFFF));
      binary_io::write_uint16(index, flags);

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
}