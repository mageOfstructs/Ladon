#ifndef MMAP_H
#define MMAP_H

#include <stdint.h>
#include "printf.h"

typedef struct mmape {
  uint64_t base;
  uint64_t length;
  uint32_t type;
  uint32_t ext_attrs;
} mmape_t;

#define MMAP_FREE (1)

int get_next_free_space(mmape_t *entries, uint32_t entries_l, int start);
int cmp_mmape_len(void *a, void *b);
void tostr_mmape_len(void *e);

#endif // !MMAP_H
