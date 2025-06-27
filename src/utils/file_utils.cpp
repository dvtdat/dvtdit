#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "file_utils.h"
#include "types.h"
#include "blob.h"
#include "hash.h"

namespace file_utils {
  std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
  }

  bool isFileExisted(const std::string& path) {
    std::filesystem::path repoRoot = std::filesystem::current_path(); 
    return std::filesystem::exists(std::filesystem::path(repoRoot) / path);
  }

  bool isFileContentModified(const FileMetadata& existingEntry) {
    std::string fileData = file_utils::readFile(existingEntry.path);
    std::string fileBlob = blob::convertToBlob(fileData);
    std::string hashedData = hash::hashDataSHA1(fileBlob);

    return hashedData != existingEntry.sha1;
  }

  bool isFileModified(const FileMetadata& entry) {
    std::filesystem::path repoRoot = std::filesystem::current_path();
    std::filesystem::path filename = std::filesystem::path(repoRoot) / entry.path;

    if (!std::filesystem::exists(filename)) {
      return false;
    }

    struct stat st;
    stat(filename.c_str(), &st);

    if (st.st_size != entry.st.st_size) {
      return true;
    }

    if (st.st_mtime != entry.st.st_mtime) {
      return isFileContentModified(entry);
    }

    return false;
  }

}