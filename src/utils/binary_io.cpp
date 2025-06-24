#include <fstream>
#include <cstdint>
#include <arpa/inet.h> 

#include "binary_io.h"

namespace binary_io {
  void write_uint32(std::ofstream &out, uint32_t value) {
    uint32_t be = htonl(value); 
    out.write(reinterpret_cast<char*>(&be), sizeof(be));
  };

  void write_uint16(std::ofstream &out, uint16_t value) {
    uint16_t be = htons(value);
    out.write(reinterpret_cast<char*>(&be), sizeof(be));
  };
}
