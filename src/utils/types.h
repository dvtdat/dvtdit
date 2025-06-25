#ifndef TYPES_H
#define TYPES_H

#include <sys/stat.h>
#include <string>

struct FileMetadata {
  struct stat st;
  std::string sha1;
  std::string path;
  uint16_t flags;

  FileMetadata() {}
  FileMetadata(const struct stat& statData, const std::string& sha1Hash, const std::string& filePath, const uint16_t& flags)
    : st(statData), sha1(sha1Hash), path(filePath), flags(flags) {}
};

#endif // TYPES_H