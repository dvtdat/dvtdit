
#ifndef BINARY_IO_H
#define BINARY_IO_H

namespace binary_io {
  void write_uint32(std::ofstream &out, uint32_t value);
  void write_uint16(std::ofstream &out, uint16_t value);
}

#endif