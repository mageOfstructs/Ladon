#include "mmap.h"
#include <stdint.h>

int get_next_free_space(mmape_t *entries, uint32_t entries_l, int start) {
  if (start > entries_l || !entries)
    return -1;
  for (; start < entries_l; start++) {
    if (entries[start].type == MMAP_FREE)
      return start;
  }
  return -1;
}

int cmp_mmape_len(void *a, void *b) {
  return ((mmape_t *)a)->length - ((mmape_t *)b)->length;
}

void tostr_mmape_len(void *e) {
  printf("{%p}\n", (void *)((mmape_t *)e)->length);
}
