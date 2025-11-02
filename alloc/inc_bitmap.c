#include "inc_bitmap.h"
#include <stdbool.h>
#include <stdint.h>

void init_ib(ib_ctx_t *ctx, void *start, ulong sz) {
  ctx->bitmap_start = start;
  ctx->data_start = ctx->bitmap_start + ceild(sz, 8);
  ctx->data_end = start + sz;
  ctx->cur_off = ctx->data_start;
  KASSERT(ctx->cur_off);
}

void *ib_alloc_ctx(ib_ctx_t *ctx, ulong sz) {
  if (ctx->cur_off >= ctx->data_end)
    ctx->cur_off = ctx->data_start;

  void *old_cur_off = ctx->cur_off;

  ulong nook_sz = 0;
  while (nook_sz < sz) {
    while (!(sz - nook_sz > ctx->data_end - ctx->cur_off) &&
           get_bit(ctx->bitmap_start, ctx->cur_off - ctx->data_start)) {
      ctx->cur_off++;
      nook_sz = 0; // need to reset this every time we find something occupied
    }

    if (!(sz - nook_sz > ctx->data_end - ctx->cur_off)) {
      nook_sz++;
      ctx->cur_off++; // need this otherwise the ptr won't increment if we find
                      // a free block
    } else if (old_cur_off != ctx->cur_off)
      ctx->cur_off = ctx->data_start;
    else
      return NULL; // we looped back around, there's no free space
  }
  if (nook_sz >= sz) { // >= is just a precaution
    void *ret = ctx->cur_off - nook_sz;
    for (ulong i = 0; i < nook_sz; i++) {
      set_bit(ctx->bitmap_start, i);
    }
    return ret;
  }
  return NULL; // i don't think this is even possible to reach
}

void ib_dealloc_ctx(ib_ctx_t *ctx, void *start, ulong sz) {
  for (ulong i = 0; i < sz; i++) {
    KASSERT(get_bit(ctx->bitmap_start, start - ctx->data_start + i));
    clear_bit(ctx->bitmap_start, start - ctx->data_start + i);
  }
}
