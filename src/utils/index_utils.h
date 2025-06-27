#ifndef INDEX_UTILS_H
#define INDEX_UTILS_H

namespace index_utils {
  std::map<std::string, FileMetadata> readIndex();
  void writeIndex(const std::map<std::string, FileMetadata> &entries);
}

#endif // INDEX_UTILS_H