#ifndef TYPES_H
#define TYPES_H

#include <sys/stat.h>
#include <string>

struct FileMetadata {
  struct stat st;
  std::string sha1;
  std::string path;
  
  FileMetadata() {}
  FileMetadata(const struct stat& statData, const std::string& sha1Hash, const std::string& filePath)
    : st(statData), sha1(sha1Hash), path(filePath) {}
};

#endif