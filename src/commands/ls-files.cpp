#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <sys/stat.h>

#include "../utils/types.h"
#include "../utils/index_io.h"
#include "../utils/file_io.h"
#include "../utils/hash.h"
#include "../utils/blob.h"

bool hasFlag(std::vector<std::string>& flags, std::string flag) {
  return std::find(flags.begin(), flags.end(), flag) != flags.end();
}

bool isFileExisted(const std::string& path) {
  std::filesystem::path repoRoot = std::filesystem::current_path(); 
  return std::filesystem::exists(std::filesystem::path(repoRoot) / path);
}

bool isFileContentModified(const FileMetadata& existingEntry) {
  std::string fileData = file_io::readFile(existingEntry.path);
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

void handleLsFiles(std::vector<std::string>& flags) {
  std::map<std::string, FileMetadata> indexes = index_io::readIndex();
  
  bool showDeleted = hasFlag(flags, "-d") || hasFlag(flags, "--deleted");
  bool showCached = hasFlag(flags, "-c") || hasFlag(flags, "--cached");
  bool showModified = hasFlag(flags, "-m") || hasFlag(flags, "--modified");
  bool showStage = hasFlag(flags, "-s") || hasFlag(flags, "--stage");

  if (flags.empty() || (!showDeleted && !showModified)) {
    showCached = true;
  }

  for (auto it = indexes.begin(); it != indexes.end(); it++) {
    FileMetadata &entry = it->second;
    uint16_t entryStage = ((entry.flags & 0x3000) >> 12);

    if ((showDeleted || showModified) && entryStage != 0) {
      continue;
    }

    bool shouldShow = false;

    if (showCached) {
      shouldShow = true;
    }

    if (showDeleted && !isFileExisted(entry.path)) {
      shouldShow = true;
    }

    if (showModified && isFileExisted(entry.path) && isFileModified(entry)) {
      shouldShow = true;
    }

    if (!shouldShow) {
      continue;
    }

    if (showStage) {
      std::cout << std::oct << entry.st.st_mode << std::dec << " " << entry.sha1 << " " << entryStage << "\t" << entry.path << '\n';
    } else {
      std::cout << entry.path << '\n';
    }
  }
}