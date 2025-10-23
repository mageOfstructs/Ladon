#include "math.h"
#include <stdint.h>

inline uint32_t max(uint32_t n1, uint32_t n2) { return n1 > n2 ? n1 : n2; }

inline uint32_t min(uint32_t n1, uint32_t n2) { return n1 < n2 ? n1 : n2; }

inline uint32_t pad(uint32_t n, uint32_t div) {
  return n + (div - n % div) % div;
}
