#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <sys/stat.h>

#include "../utils/types.h"
#include "../utils/index_utils.h"
#include "../utils/file_utils.h"

bool hasFlag(std::vector<std::string>& flags, std::string flag) {
  return std::find(flags.begin(), flags.end(), flag) != flags.end();
}

void handleLsFiles(std::vector<std::string>& flags) {
  std::map<std::string, FileMetadata> indexes = index_utils::readIndex();
  
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

    if (showDeleted && !file_utils::isFileExisted(entry.path)) {
      shouldShow = true;
    }

    if (showModified && (!file_utils::isFileExisted(entry.path) || file_utils::isFileModified(entry))) {
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