#include <string>

namespace blob {
  std::string convertToBlob(std::string data) {
    return "blob " + std::to_string(data.size()) + '\0' + data;
  }
}