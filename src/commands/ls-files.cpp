#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <unordered_set>

#include "../utils/types.h"
#include "../utils/index_utils.h"
#include "../utils/file_utils.h"

bool hasFlag(const std::unordered_set<std::string>& flags, const std::string& flag) {
  return flags.find(flag) != flags.end();
}

void handleLsFiles(const Arguments& args) {
  std::map<std::string, FileMetadata> indexes = index_utils::readIndex();
  
  bool showDeleted = hasFlag(args.flags, "-d") || hasFlag(args.flags, "--deleted");
  bool showCached = hasFlag(args.flags, "-c") || hasFlag(args.flags, "--cached");
  bool showModified = hasFlag(args.flags, "-m") || hasFlag(args.flags, "--modified");
  bool showStage = hasFlag(args.flags, "-s") || hasFlag(args.flags, "--stage");

  if (args.flags.empty() || (!showDeleted && !showModified)) {
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