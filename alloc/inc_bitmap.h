#ifndef INC_BITMAP_H
#define INC_BITMAP_H

#include "../binops.h"
#include "../math.h"
#include "../types.h"
#include "../utils.h"
#include <stdint.h>
typedef struct ib_ctx {
  void *bitmap_start;
  void *data_start;
  void *data_end;
  void *cur_off;
} ib_ctx_t;

#endif // !INC_BITMAP_H
