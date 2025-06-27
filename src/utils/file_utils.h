#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "types.h"

namespace file_utils {
  std::string readFile(const std::string &path);
  bool isFileExisted(const std::string &path);
  bool isFileContentModified(const FileMetadata &existingEntry);
  bool isFileModified(const FileMetadata &entry);
}

#endif // FILE_UTILS_H