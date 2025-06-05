#pragma once
#include <string>

namespace hashing {
  namespace sha1 {
    // Returns a dynamically allocated 20-byte digest (caller must delete[])
    void* hash(const std::string& message);

    // Helper to convert the digest into a hex string
    std::string sig2hex(void* sig);
  }
}
