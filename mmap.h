#ifndef MMAP_H
#define MMAP_H

#include <stdint.h>

typedef struct mmape {
  uint64_t base;
  uint64_t length;
  uint32_t type;
  uint32_t ext_attrs;
} mmape_t;

#endif // !MMAP_H
