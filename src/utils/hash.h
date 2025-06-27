#ifndef HASH_H
#define HASH_H

namespace hash {
  std::string hashDataSHA1(std::string data);
  void writeSHA1(std::ofstream &out, const std::string &hash);
}

#endif // HASH_H