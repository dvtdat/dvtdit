#ifndef INDEX_IO_H
#define INDEX_IO_H

namespace index_io {
  std::map<std::string, FileMetadata> readIndex();
  void writeIndex(const std::map<std::string, FileMetadata> &entries);
}

#endif // INDEX_IO_H