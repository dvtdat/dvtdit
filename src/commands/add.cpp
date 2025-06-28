#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <sys/stat.h>
#include <iomanip> 

#include "../utils/types.h"
#include "../utils/hash.h"
#include "../utils/file_utils.h"
#include "../utils/index_utils.h"

void addChecksum() {
  std::string indexFileData = file_utils::readFile(".dit/index");
  std::string hashedIndexFileData = hash::hashDataSHA1(indexFileData);

  std::ofstream index(".dit/index", std::ios::app | std::ios::binary);

  for (int i = 0; i < 40; i += 2) {
    std::string byteStr = hashedIndexFileData.substr(i, 2);
    uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
    index.write(reinterpret_cast<char*>(&byte), 1);
  }

  index.close();
}

void handleAdd(const Arguments& args) {
  std::map<std::string, FileMetadata> indexes = index_utils::readIndex();

  for (const std::string& filename : args.files) {
    std::string fileData = file_utils::readFile(filename);
    std::string fileBlob = file_utils::convertToBlob(fileData);
    std::string hashedData = hash::hashDataSHA1(fileBlob);
    struct stat st;
    stat(filename.c_str(), &st);

    auto it = indexes.find(filename);
    if (it != indexes.end()) {
      const FileMetadata& existingEntry = it->second;
      if (st.st_mtime == existingEntry.st.st_mtime && hashedData == existingEntry.sha1) {
        continue;
      }
    }

    FileMetadata metadata(st, hashedData, filename, 0);
    indexes[filename] = metadata;
  }

  index_utils::writeIndex(indexes);
  addChecksum();
}
